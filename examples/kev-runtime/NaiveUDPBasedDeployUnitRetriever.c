/*
 * NaiveUDPBasedDeployUnitRetriever.c
 * This file is part of Kevoree-Contiki
 *
 * Copyright (C) 2015 - Inti Gonzalez-Herrera
 *
 * Kevoree-Contiki is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Kevoree-Contiki is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kevoree-Contiki. If not, see <http://www.gnu.org/licenses/>.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfs/cfs.h"
#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h"

#include "lib/random.h"

#include "simple-udp.h"
#include "net/netstack.h"
#include "dev/button-sensor.h"
#include "dev/slip.h"

#include "rtkev.h"
#include "NaiveUDPBasedDeployUnitRetriever.h"
#include "requests.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...)	printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#define PRINT6ADDR(addr) printf("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])

PROCESS(udp_retriever_p, "NaiveUDPRetriever");
PROCESS(artifact_resolver, "artifact_resolver");

/* kevoree event types */
static process_event_t NEW_DEPLOY_UNIT_REQUEST;
static process_event_t SUMMARY_DOWNLADED;
static process_event_t ARTIFACT_AVAILABLE;
static process_event_t ARTIFACT_REQUESTED;
static process_event_t NEXT_CHUNK;

static uip_ipaddr_t routerAddr;

/* Kevoree core interface */
static int getDeployUnit00(const char* deployUnitName);
static int my_init();
const DeployUnitRetriver naive_udp_retriever = {
		.init = my_init,
		.getDeployUnit = getDeployUnit00
}; 

static struct DeployUnitRequest* active_request = NULL;

static void kevoree_onNewSummary(uint16_t session_id, uint16_t nr_chunks);
static void kevoree_onNewChunk(uint16_t chunk_id, uint16_t len, const uint8_t* data);
static void kevoree_onAckArtifactRequest(void);
static void kevoree_onArtifactRequest(const uip_ipaddr_t* source_address, const char* artifact);
static void kevoree_onChunkRequest(uint16_t session_id, uint16_t chunk_id);
static void kevoree_onRouteRequest(uip_ipaddr_t *reqAddr);
static void kevoree_onRouteResponse(uint16_t *addrs);

/*static uip_ipaddr_t repoAddr;*/
static uip_ipaddr_t addr;
static struct simple_udp_connection unicast_connection;
static struct KevoreePacket pkt;

static const struct RequestProcessingCallback kevoree_runtime_request_callbacks = {
		.onNewSummary = kevoree_onNewSummary,
		.onNewChunk = kevoree_onNewChunk,
		.onAckArtifactRequest = kevoree_onAckArtifactRequest,
		.onArtifactRequest = kevoree_onArtifactRequest,
		.onChunkRequest = kevoree_onChunkRequest,
		.onRouteRequest = kevoree_onRouteRequest,
		.onRouteResponse = kevoree_onRouteResponse
};

static void
receiver(struct simple_udp_connection *c,
		const uip_ipaddr_t *sender_addr,
		uint16_t sender_port,
		const uip_ipaddr_t *receiver_addr,
		uint16_t receiver_port,
		const uint8_t *data,
		uint16_t datalen)
{
	enum MessageProcessingErroCode err;

	// TODO: find address
	err = process_message(sender_addr, datalen, data, &kevoree_runtime_request_callbacks);

	// TODO: notify when there is an error in the message
	if (err == WRONG_CRC) {
		PRINTF("Wrong CRC detected\n");
	}
	else if (err == UNKNOWN_MESSAGE_TYPE) {
		PRINTF("Wow, a wrong command\n");
	}
}

static uip_ipaddr_t*
get_local_address(void)
{
	int i;
	uint8_t state;
	char* r = (char*)malloc(sizeof(char)*10);

	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused &&
				(/*state == ADDR_TENTATIVE ||*/ state == ADDR_PREFERRED)) {
			uip_ipaddr_t *ip_addr = &uip_ds6_if.addr_list[i].ipaddr;
			/*uint16_t a = (ip_addr->u8[14] << 8) + ip_addr->u8[15];
			sprintf(r, "%x", a);
			return r;*/ // please, just one address, I don't care if the interface has more than one address
			return ip_addr;
		}
	}

	return  NULL;
}

/*=====================================================================================
 * Server process: other motes request artifacts to this server
 *=====================================================================================*/
LIST(requests_as_server);

/* 
	Executed when the server already knows that you are requesting an artifact but it doesn't have it available yet.
	It is kind of "ohhh.. shut up!!!"
 */
static void
kevoree_onAckArtifactRequest(void)
{
	/* are we processing something */
	if (active_request == NULL) {
		return;
	}

	/* check if the request is in the proper state */
	if (active_request->state == WAITING_FOR_SUMMARY) {

		/* update the request's internal state */
		active_request->state = PASSIBALY_WAITING_FOR_SUMMARY;
	}
}

static int
is_artifact_location_known(struct DeployUnitRequest* req)
{
	return 1;
}

/*
	Executed on a server of deploy units when someone requests a new artifact
 */
static void
kevoree_onArtifactRequest(const uip_ipaddr_t* source_address, const char* artifact)
{
	struct KevoreePacket pkt;
	struct DeployUnitRequest* req = find_request_by_source(requests_as_server, source_address, artifact);

	printf("======================================== We shouldn't be executing this ============ \n");

	int is_new = req == NULL;

	if (req == NULL) {
		/* a new request */
		req = create_request(source_address, artifact, SENDING_SUMMARY);
		list_add(requests_as_server, req);
	}

	/* requesting the same */
	if (req->state == SENDING_SUMMARY || req->state == WAITING_FOR_LOCATION) {
		/* send summary to the source */

		if (is_artifact_location_known(req)) {
			build_summary_packet(&pkt, req->session_id, req->nr_packets);

			/* TODO : send packet */
		}
		else {
			/* send ack to the source */
			build_ack_packet(&pkt);

			/* if needed, enqueue new request for the upward server */
			if (req->state != WAITING_FOR_LOCATION) {
				req = create_request(source_address, artifact, WAITING_FOR_SUMMARY);
				//list_add(requests_as_client, req);
			}

			// TODO: assign active_request
		}
	}
}

/*
	Executed on a server of deploy units when someone requests a new artifact's chunk
 */
static void
kevoree_onChunkRequest(uint16_t session_id, uint16_t chunk_id)
{
	struct DeployUnitRequest* req = find_request_by_session(requests_as_server, session_id);

	if (req != NULL && req->state == SENDING_CHUNKS && req->current_packet == chunk_id) {
		/* read from the file */
		uint8_t buf [REQUEST_PACKET_SIZE];
		cfs_seek(req->fd, chunk_id * REQUEST_PACKET_SIZE, CFS_SEEK_SET);
		int n = cfs_read(req->fd, buf, REQUEST_PACKET_SIZE);

		/* build the packet */
		struct KevoreePacket pkt;
		build_chunk_packet(&pkt, chunk_id, n, buf);

		/* TODO: send the packet */
		//simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), &addr);
	}
}

/*=====================================================================================
 * Client process to download artifacts from a remote server
 *=====================================================================================*/

static uint16_t last_filename_suffix;

LIST(requests_as_client);

/* executed when a new summary arrive */
static void
kevoree_onNewSummary(uint16_t session_id, uint16_t nr_chunks)
{
	/* are we processing something */
	if (active_request == NULL) {
		return;
	}

	/* check if the request is in the proper state */
	// TODO: fix the typo
	if (active_request->state == WAITING_FOR_SUMMARY
			|| active_request->state == PASSIBALY_WAITING_FOR_SUMMARY) {
		printf("The summary in retriever says %d chunks\n", nr_chunks);

		/* update the request's internal state */
		active_request->nr_packets = nr_chunks;
		active_request->session_id = session_id;
		active_request->current_packet = 0;
		active_request->state = RECEIVING_CHUNKS;

		/* continue the processing */
		/* prepare request */
		build_get_chunk_packet(&pkt, active_request->session_id, active_request->current_packet);

		/* send message to the server */
		simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), &addr);
		/*process_post(&udp_retriever_p, SUMMARY_DOWNLADED, NULL);*/
	}
}

/* executed when a new chunk arrive */
static void
kevoree_onNewChunk(uint16_t chunk_id, uint16_t len, const uint8_t* data)
{
	struct MapEntry* entry;
	/* are we processing something */
	if (active_request == NULL) {
		return;
	}

	/* check if the request is in the proper state */
	if (active_request->state == RECEIVING_CHUNKS 
			&& active_request->current_packet == chunk_id) {

		if (chunk_id % 10 == 0) {
			printf("The chunk %d with %d bytes of the file just arrived\n", chunk_id, len);
		}

		/* write the new chunk at the end of the file */
		cfs_seek(active_request->fd, 0, CFS_SEEK_END);
		cfs_write(active_request->fd, data, len);

		/* update the request's internal state */
		active_request->current_packet++;
		active_request->state = (active_request->current_packet == active_request->nr_packets)? DONE : RECEIVING_CHUNKS;


		/* are we done? */
		if (active_request->state == DONE) {
			/* close the file when it is done */
			cfs_close(active_request->fd);

			/* notify to whoever is listening that the deploy unit is locally available */
			entry = (struct MapEntry*)malloc(sizeof(struct MapEntry));
			entry->artifact = strdup(active_request->deployUnitName);
			entry->filename = strdup(active_request->localFilename);
			process_post(&artifact_resolver, ARTIFACT_AVAILABLE , (char*)entry);
		} else {
			build_get_chunk_packet(&pkt, active_request->session_id, active_request->current_packet);
			/* TODO: send message to the server */
			simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), &addr);
		}
	}
}

PROCESS_THREAD(udp_retriever_p, ev, data)
{
	static struct etimer timer;
	char tmpName[20];
	PROCESS_BEGIN();

	last_filename_suffix = 0;

	list_init(requests_as_server);
	list_init(requests_as_client);

	PRINTF("Starting the UDP-based retriever\n");

	/* register new event type */
	NEW_DEPLOY_UNIT_REQUEST = process_alloc_event();
	SUMMARY_DOWNLADED = process_alloc_event();
	NEXT_CHUNK = process_alloc_event();

	/* configure server address */
	uip_ip6addr(&addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
	uip_ip6addr(&routerAddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x3658);

	/* initialize the UDP stuff */
	simple_udp_register(&unicast_connection, PORT, NULL, PORT, receiver);

	/* TODO: print the RPL tree */

	etimer_set(&timer, CLOCK_SECOND * (2));


	while(1) {
		PROCESS_WAIT_EVENT();
		//printf("Event %d %d\n", ev, SUMMARY_DOWNLADED);
		if (ev == PROCESS_EVENT_TIMER || ev == NEXT_CHUNK) {
			unsigned interval = 2 + ((unsigned)random_rand() % 5);
			if (active_request == NULL) {
				etimer_set(&timer, CLOCK_SECOND * interval);
			}
			/*else if (active_request == 0x3a7b5b3a) {
				printf("Look at the bad address %p\n", active_request);
				active_request = NULL;
				etimer_set(&timer, CLOCK_SECOND * interval);
				continue;
			}*/
			if (active_request && active_request->state == WAITING_FOR_SUMMARY) {
				build_get_artifact_packet(&pkt, active_request->deployUnitName);

				//PRINTF("Sending message of length %d with crc %d to ", len, pkt.crc);

				//uip_debug_ipaddr_print(&addr);
				//printf("\n");

				/*TODO: send message to the server */
				//printf("Que mierda pasa ahora?\n");
				simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), &addr);
			}
			else if (active_request && active_request->state == RECEIVING_CHUNKS) {
				/* prepare request */
				build_get_chunk_packet(&pkt, active_request->session_id, active_request->current_packet);
			
				/* TODO: send message to the server */
				simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), &addr);
			}

			/* generate_routes(); */

			/* TODO: marking the future */
			if (active_request && active_request->state == DONE) {
				dispose_request(active_request);
				if (list_length(requests_as_client) > 0)
					active_request = list_pop(requests_as_client);
				else
					active_request = NULL;
			}

			if (active_request) {
				/* timer once again */
				etimer_set(&timer, CLOCK_SECOND * interval);
			}

		} else if (ev == NEW_DEPLOY_UNIT_REQUEST) {
			/* process new deploy unit requests */

			/* check if it is already here */
			/*printf("Ok, let's rock %p\n", active_request);*/
			PRINTF("INFO: Requesting new deploy unit\n");

			/* create request */
			struct DeployUnitRequest* req = create_request(get_local_address(), (char*)data, WAITING_FOR_SUMMARY);
			free((char*)data);
			sprintf(tmpName, "artifact%d", last_filename_suffix++);
			req->localFilename = strdup(tmpName);
			cfs_remove(tmpName);
			req->fd = cfs_open(req->localFilename, CFS_READ | CFS_WRITE);
			req->current_packet = 0;

			/*printf("Everything is Ok, look at active_requests %p\n", active_request);*/

			list_add(requests_as_client, req);

			if (active_request == NULL && list_length(requests_as_client) > 0) {
				active_request = list_pop(requests_as_client);
			}

			/*printf("Mierdaaaaaaa %p\n", active_request);*/

			/* go to the next step */
			etimer_set(&timer, CLOCK_SECOND * (2));
			build_get_artifact_packet(&pkt, active_request->deployUnitName);
			simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), &addr);
		} else if (ev == SUMMARY_DOWNLADED){
			/* request packets */

			/* prepare request */
			build_get_chunk_packet(&pkt, active_request->session_id, active_request->current_packet);

			/* send message to the server */
			simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), &addr);
		}
	}
	PROCESS_END();
}

/*============================================================================
  Process to deal with artifacts
 ============================================================================*/
LIST(known_artifacts);
LIST(artifact_requests);

struct ArtifactRequest {
	void* next;
	char * artifact;
	void* user_data;
	void (*notification_routine)(const char* artifact, const char* filename, void* user_data);
};

static void
dispose_ArtifactRequest(struct ArtifactRequest* r)
{
	if (r->artifact != NULL) free(r->artifact);
	free(r);
}

PROCESS_THREAD(artifact_resolver, ev, data)
{
	uip_ipaddr_t *rtAddr;
	struct MapEntry* entry;
	struct ArtifactRequest* r;
	PROCESS_BEGIN();

	/* register new event type */
	ARTIFACT_AVAILABLE = process_alloc_event();
	ARTIFACT_REQUESTED = process_alloc_event();

	/* list of known artifacts */
	list_init(known_artifacts);

	/* list of artifact requests */
	list_init(artifact_requests);

	/* this is a server, so it runs forever */
	for (;;) {
		PROCESS_WAIT_EVENT();
		if (ev == ARTIFACT_AVAILABLE) {
			/* save it */
			entry = (struct MapEntry*)data;
			/* iterate over previous requests to resolve them */
			list_add(known_artifacts, entry);
			int n = list_length(artifact_requests);
			while (n) {
				r = list_pop(artifact_requests);
				if (strcmp(r->artifact, entry->artifact) == 0) {
					r->notification_routine(r->artifact, entry->filename, r->user_data);
					dispose_ArtifactRequest(r);
				}
				else {
					list_add(artifact_requests, r);
				}
				n--;
			}
		}
		else if (ev == ARTIFACT_REQUESTED){
			r = (struct ArtifactRequest*)data;
			/* TODO: check if it is the same */
			rtAddr = uip_ds6_defrt_choose();
			memcpy(&addr, rtAddr, sizeof(addr));
			addr.u8[0] = 0xaa;
			addr.u8[1] = 0xaa;
			if (addr.u8[15] == routerAddr.u8[15] && addr.u8[14] == routerAddr.u8[14]) {
				PRINTF("INFO: Requesting to BR\n");
				uip_ip6addr(&addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
			}
			PRINTF("INFO: Requesting artifact to:");
			PRINT6ADDR(&addr);
			PRINTF("\n");

			entry = find_by_artifact(known_artifacts, r->artifact);
			if (entry != NULL) {
				r->notification_routine(r->artifact, entry->filename, r->user_data);
				dispose_ArtifactRequest(r);
			}
			else {
				list_add(artifact_requests, r);
				process_post(&udp_retriever_p, NEW_DEPLOY_UNIT_REQUEST, strdup(r->artifact));
			}
		}
	}

	PROCESS_END();
}

/*======================================================================================================
 * Local attempts to obtain a software artifact
 *======================================================================================================*/
static int
my_init()
{
	printf("Initializing artifact resolver\n");
	process_start(&udp_retriever_p, NULL);
	process_start(&artifact_resolver, NULL);
	active_request = NULL;
	printf("Initialized at: %p\n", &artifact_resolver);
	return 0;
}

static void
local_artifact_request(const char* artifact, const char* filename, void* user_data)
{
	notifyDeployUnitDownloaded(filename);
}

static int
getDeployUnit00(const char* deployUnitName)
{
	printf("Ahhhhh 0 %p\n", active_request);
	/* notify to the process */
	struct ArtifactRequest* r = (struct ArtifactRequest*)malloc(sizeof(struct ArtifactRequest));
	r->artifact = strdup(deployUnitName);
	r->user_data = NULL;
	r->notification_routine = local_artifact_request;
	printf("Requesting artifact %s to %p\n", r->artifact, &artifact_resolver);
	process_post(&artifact_resolver, ARTIFACT_REQUESTED, r);
	return 0;
}

/*
 * Get the default route
 */
static void
kevoree_onRouteRequest(uip_ipaddr_t *reqAddr)
{
	printf("INFO: Routes requested\n");
	uint16_t addrs[29];
	memset(&addrs[0], 0, sizeof(addrs));
	uip_ipaddr_t *addr;
	int i;
	static uip_ds6_route_t *r;

	PRINTF("INFO: Sending IPs\n");
	addr = uip_ds6_defrt_choose();
	addrs[0] = addr->u16[8];
	PRINTF("%x\n", addrs[0]);

	for(r = uip_ds6_route_head(), i = 1; r != NULL; r = uip_ds6_route_next(r)) {
		addrs[i] = r->ipaddr.u16[8];
		PRINTF("%x\n", addrs[i]);
		i++;
	}

	build_routes_packet(&pkt, addrs, sizeof addrs);
	simple_udp_sendto(&unicast_connection, &pkt, total_len(&pkt), reqAddr);
}

static void
kevoree_onRouteResponse(uint16_t *addrs)
{
	int i;
	for (i = 0; addrs[i] == 0; i++) {
		PRINTF("%x\n", addrs[i]);
	}
}
