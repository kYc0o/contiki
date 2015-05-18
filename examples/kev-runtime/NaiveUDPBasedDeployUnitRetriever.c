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

#include "rtkev.h"
#include "NaiveUDPBasedDeployUnitRetriever.h"


#include "cfs/cfs.h"

#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h"

#include "simple-udp.h"
#include "net/netstack.h"
#include "dev/button-sensor.h"
#include "dev/slip.h"

#include "lib/crc16.h"

PROCESS(udp_retriever_p, "Naive UDP Retriever");

/* kevoree event types */
static process_event_t NEW_DEPLOY_UNIT_REQUEST;
static process_event_t SUMMARY_DOWNLADED;

static int getDeployUnit00(const char* deployUnitName);

const DeployUnitRetriver naive_udp_retriever = {
	.getDeployUnit = getDeployUnit00
}; 

#define PORT 1234

enum State {
	DONE,
	WAITING_FOR_SUMMARY,
	RECEIVING_PACKETS
};

struct DeployUnitRequest {
	uint8_t status;
	char* deployUnitName;
	char* filename;
	int fd;
	uint16_t current_packet;
	uint16_t nr_packets;
	uint16_t packet_size;
};

static struct DeployUnitRequest request = {
	.status = DONE,
	.deployUnitName = NULL,
	.filename = NULL,
	.fd = 0,
	.current_packet = 0
};

/* command types */
#define CMD_GET_ARTIFACT 3
#define CMD_GET_PACKET 4

/* responce types */
#define RESPONSE_SUMMARY 5
#define RESPONSE_PACKET 6

/* general packet */
struct KevoreePacket {
	uint16_t crc;
	uint16_t cmd;
	char data[64 - 2 * sizeof(uint16_t)];
};

/* summary packet */
struct KevoreeSummaryPacket {
	uint16_t crc;
	uint16_t cmd;
	uint16_t nr_pages;
	uint16_t page_size;
};

/* packet to request a packet */
struct KevoreePacketRequest {
	uint16_t crc;
	uint16_t cmd;
	uint16_t packet_id;
};

/* file chunck */
struct DeployUnitChunck {
	uint16_t crc;
	uint16_t cmd;
	uint16_t packet_id;
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
	uint16_t crcC;
	struct KevoreePacket* pkt = (struct KevoreePacket*)data;
	
	/* first check if it is a vlid packet (no transmission errors) */
	uint16_t crc = pkt->crc;
	pkt->crc = 0;
	crcC = crc16_data((unsigned char*)pkt, datalen, 0);
#if 0
	printf("Data received from ");
	uip_debug_ipaddr_print(sender_addr);
	printf(" on port %d from port %d with length %d, crc0 %d and crc1 %d\n",
			receiver_port, sender_port, datalen, crc, crcC);
#endif

	/* stop processecing if the packet has errors */
	//if (crc != crcC) return;
	
	if (request.status == WAITING_FOR_SUMMARY && pkt->cmd == RESPONSE_SUMMARY) {
		struct KevoreeSummaryPacket * summary = (struct KevoreeSummaryPacket*)pkt;
		printf("The summary in retriever says %d pages and page size = %d\n", summary->nr_pages, summary->page_size);
		
		request.nr_packets = summary->nr_pages;
		request.packet_size = summary->page_size;
		request.status = RECEIVING_PACKETS;
		
		process_post(&udp_retriever_p, SUMMARY_DOWNLADED, NULL);
	}
	else if (request.status == RECEIVING_PACKETS && pkt->cmd == RESPONSE_PACKET) {
		struct DeployUnitChunck* chunck = (struct DeployUnitChunck*)pkt;
		if (chunck->packet_id == request.current_packet) {
			printf("The chunk %d with %d bytes of the file just arrived\n", chunck->packet_id, (datalen - sizeof(struct DeployUnitChunck)));
			char* b = (char*)data + sizeof(struct DeployUnitChunck);
			cfs_seek(request.fd, 0, CFS_SEEK_END);
			cfs_write(request.fd, b, (datalen - sizeof(struct DeployUnitChunck)));
			
			request.current_packet++;
			
			request.status = (request.current_packet == request.nr_packets)? DONE : RECEIVING_PACKETS;
			
			
			if (request.status == DONE) {
				/* close the file when it is done */
				cfs_close(request.fd);
				
				/* notify to whoever is listening that the deploy unit is locally available */
				notifyDeployUnitDownloaded(request.filename);
			}
		}
	}
}

PROCESS_THREAD(udp_retriever_p, ev, data)
{
	static uip_ipaddr_t addr;
	static struct simple_udp_connection unicast_connection;
	static struct etimer timer;
	static uint16_t message_number;
	PROCESS_BEGIN();
	
	printf("Starting the UDP-based retriever\n");
	
	/* register new event type */
	NEW_DEPLOY_UNIT_REQUEST = process_alloc_event();
	SUMMARY_DOWNLADED = process_alloc_event();
	
	/* confire server address */
	uip_ip6addr(&addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);

	etimer_set(&timer, CLOCK_SECOND * (5));

	/* initialize the UDP stuff */
	simple_udp_register(&unicast_connection, PORT, NULL, PORT, receiver);

	/* TODO: print the RPL tree */

	while(1) {
		PROCESS_WAIT_EVENT();
		//printf("Event %d %d\n", ev, SUMMARY_DOWNLADED);
		if (ev == PROCESS_EVENT_TIMER) {
			if (request.status == WAITING_FOR_SUMMARY) {
				struct KevoreePacket pkt;
				char* end;
				uint16_t len;
				
				sprintf(pkt.data, "%s", request.deployUnitName);
				pkt.cmd = CMD_GET_ARTIFACT;
				end = pkt.data + strlen(pkt.data);
				len = (end - (char*)&pkt);
				pkt.crc = 0;
				pkt.crc = crc16_data((unsigned char*)&pkt, len, 0);
				printf("Sending message of length %d with crc %d to ", len, pkt.crc);
				uip_debug_ipaddr_print(&addr);
				printf("\n");
				
				/* send message to the server */
				simple_udp_sendto(&unicast_connection, &pkt, len, &addr);
			}
			else if (request.status == RECEIVING_PACKETS) {
				/* prepare request */
				struct KevoreePacketRequest pkt;
				pkt.cmd = CMD_GET_PACKET;
				pkt.packet_id = request.current_packet;
				pkt.crc = 0;
				pkt.crc = crc16_data((unsigned char*)&pkt, sizeof(struct KevoreePacketRequest), 0);
			
				/* send message to the server */
				simple_udp_sendto(&unicast_connection, &pkt, sizeof(struct KevoreePacketRequest), &addr);
			}
			
			/* generate_routes(); */
			
			if (request.status != DONE) {
				/* timer once again */
				etimer_set(&timer, CLOCK_SECOND * (5));
			}
			
		} else if (ev == NEW_DEPLOY_UNIT_REQUEST) {
			/* process new deploy unit requests */
			
			/* check if it is already here */
			
			/* create request */
			request.deployUnitName = (char*)data;
			request.filename = strdup("pepe");
			request.fd = cfs_open(request.filename, CFS_READ | CFS_WRITE);
			request.current_packet = 0;
			request.status = WAITING_FOR_SUMMARY;
			
			/* go to the next step */
			etimer_set(&timer, CLOCK_SECOND * (5));
		} else if (ev == SUMMARY_DOWNLADED){
			/* request packets */
			
			request.current_packet = 0;
			
			printf("Let's request a packet\n");
			
			/* prepare request */
			struct KevoreePacketRequest pkt;
			pkt.cmd = CMD_GET_PACKET;
			pkt.packet_id = request.current_packet;
			pkt.crc = 0;
			pkt.crc = crc16_data((unsigned char*)&pkt, sizeof(struct KevoreePacketRequest), 0);
			
			printf("mierda: no otra vez %d len=%d \n", pkt.crc, sizeof(struct KevoreePacketRequest));
			
			/* send message to the server */
			simple_udp_sendto(&unicast_connection, &pkt, sizeof(struct KevoreePacketRequest), &addr);
		}
	}
	PROCESS_END();
}

static int
getDeployUnit00(const char* deployUnitName)
{
	/* a request at a time */
	if (request.status == DONE) {
		process_start(&udp_retriever_p, NULL);
		/* notify to the process */
		process_post(&udp_retriever_p, NEW_DEPLOY_UNIT_REQUEST, strdup(deployUnitName));
	}
	return 0;
}
