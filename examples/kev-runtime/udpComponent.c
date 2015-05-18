/*
 * udpComponent.c
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
#include <ctype.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h"

#include "simple-udp.h"
#include "net/netstack.h"
#include "dev/button-sensor.h"
#include "dev/slip.h"

#include "rtkev.h"
#include "udpComponent.h"

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"

#define UDP_PORT 1234

static struct simple_udp_connection unicast_connection;

#if BUF_USES_STACK
static char *bufptr, *bufend;
#define ADD(...) do {                                                   \
		bufptr += snprintf(bufptr, bufend - bufptr, __VA_ARGS__);      \
} while(0)
#else
static char buf[256];
static int blen;
#define ADD(...) do {                                                   \
		blen += snprintf(&buf[blen], sizeof(buf) - blen, __VA_ARGS__);      \
} while(0)
#endif

/* forward declaration */
static void* newUDPClient(const char*);
static int startUDPClient(void* instance);
static int stopUDPClient(void* instance);
static int updateUDPClient(void* instance);

const ComponentInterface UDPClientInterface = {
		.interfaceType = ComponentInstanceInterface,
		.name = UDP_CLIENT_COMPONENT_TYPE_NAME,
		.newInstance = newUDPClient,
		.start = startUDPClient,
		.stop = stopUDPClient,
		.update = updateUDPClient
};

typedef struct {
	uint32_t interval;
	uint16_t remotePort;
} UDPClientComponent;

static
void* newUDPClient(const char* componentTypeName)
{
	UDPClientComponent* i = (UDPClientComponent*)calloc(1, sizeof(UDPClientComponent));
	
	return i;
}


PROCESS(udp_component_kev, "UDP Component");

static
int startUDPClient(void* instance)
{
	UDPClientComponent* i = (UDPClientComponent*)instance;
	KevContext* ctx = getContext(i);
	printf("Hey instance of udp client %s\n", getInstanceName(ctx));
	
	/* setting parameters based on the dictionary or, as in this case, using constant values */
	i->remotePort = UDP_PORT;
	i->interval = 10000; // 10 seconds as interval
	
	free(ctx);
	
	process_start(&udp_component_kev, instance);
	return 0;
}

static
int stopUDPClient(void* instance)
{
	UDPClientComponent* i = (UDPClientComponent*)instance;
	process_exit(&udp_component_kev);
	printf("And now the udp client is gone :-)\n");
	return 0;
}

static
int updateUDPClient(void* instance)
{
	UDPClientComponent* i = (UDPClientComponent*)instance;
	return 0;
}

static void
ipaddr_add(const uip_ipaddr_t *addr)
{
	uint16_t a;
	int i, f;
	for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
		a = (addr->u8[i] << 8) + addr->u8[i + 1];
		if(a == 0 && f >= 0) {
			if(f++ == 0) ADD("::");
		} else {
			if(f > 0) {
				f = -1;
			} else if(i > 0) {
				ADD(":");
			}
			ADD("%x", a);
		}
	}
}

static void generate_routes()
{
	static uip_ds6_route_t *r;
	static uip_ds6_nbr_t *nbr;
#if BUF_USES_STACK
	char buf[256];
#endif

#if BUF_USES_STACK
	bufptr = buf;bufend=bufptr+sizeof(buf);
#else
	blen = 0;
#endif
	ADD("Neighbors\n");

	for(nbr = nbr_table_head(ds6_neighbors);
			nbr != NULL;
			nbr = nbr_table_next(ds6_neighbors, nbr)) {

#if BUF_USES_STACK
		{char* j=bufptr+25;
		ipaddr_add(&nbr->ipaddr);
		while (bufptr < j) ADD(" ");
		switch (nbr->state) {
		case NBR_INCOMPLETE: ADD(" INCOMPLETE");break;
		case NBR_REACHABLE: ADD(" REACHABLE");break;
		case NBR_STALE: ADD(" STALE");break;
		case NBR_DELAY: ADD(" DELAY");break;
		case NBR_PROBE: ADD(" NBR_PROBE");break;
		}
		}
#else
		{uint8_t j=blen+25;
		ipaddr_add(&nbr->ipaddr);
		while (blen < j) ADD(" ");
		switch (nbr->state) {
		case NBR_INCOMPLETE: ADD(" INCOMPLETE");break;
		case NBR_REACHABLE: ADD(" REACHABLE");break;
		case NBR_STALE: ADD(" STALE");break;
		case NBR_DELAY: ADD(" DELAY");break;
		case NBR_PROBE: ADD(" NBR_PROBE");break;
		}
		}
#endif
		ipaddr_add(&nbr->ipaddr);

		ADD("\n");
#if BUF_USES_STACK
		if(bufptr > bufend - 45) {
			SEND_STRING(&s->sout, buf);
			bufptr = buf; bufend = bufptr + sizeof(buf);
		}
#else
		if(blen > sizeof(buf) - 45) {
			blen = 0;
		}
#endif
	}
	ADD("\nRoutes\n");
#if BUF_USES_STACK
	bufptr = buf; bufend = bufptr + sizeof(buf);
#else
	blen = 0;
#endif

	for(r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {

#if BUF_USES_STACK
		ADD("<a href=http://[");
		ipaddr_add(&r->ipaddr);
		ADD("]/status.shtml>");
		ipaddr_add(&r->ipaddr);
		ADD("</a>");

		ipaddr_add(&r->ipaddr);
{
	int i;
	uint8_t state;

	PRINTF("Local IPv6 address: \n");
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused &&
				(state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			printf("\t");
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
			PRINTF("\n");
			return; // please, just one address, I don't care if the interface has more than one address
		}
	}
		blen = 0;
#endif
	}

	printf("%s", buf);
}

PROCESS_THREAD(udp_component_kev, ev, data)
{
	static uip_ipaddr_t addr;
	static struct simple_udp_connection unicast_connection;
	static struct etimer timer;
	static UDPClientComponent* inst;
	static uint16_t message_number;
	PROCESS_BEGIN();
	
	PRINTF("UDP server started\n");
	
	/* confire server address */
	uip_ip6addr(&addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);

	inst = (UDPClientComponent*) data;
	
	/* print local address */
	print_local_addresses();

	etimer_set(&timer, CLOCK_SECOND * (inst->interval/1000));

	/* initialize the UDP stuff */
	simple_udp_register(&unicast_connection, inst->remotePort,
			NULL, inst->remotePort, receiver);

	/* TODO: print the RPL tree */

	while(1) {
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			char buf[20];
			
			sprintf(buf, "Message %d", message_number++);
			
			printf("Sending %s unicast to ", buf);
			uip_debug_ipaddr_print(&addr);
			
			printf("\n");
			
			/* send message to the server */
			simple_udp_sendto(&unicast_connection, buf, strlen(buf) + 1, &addr);
			generate_routes();
			etimer_restart(&timer);
		} else {
			/* process network messages */
		}
	}
	PROCESS_END();
}
