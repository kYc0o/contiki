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



#include "contiki.h"
#include "rtkev.h"
#include <stdio.h>

#include "udpComponent.h"

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
} UDPClientComponent;

static
void* newUDPClient(const char* componentTypeName)
{
    UDPClientComponent* i = (UDPClientComponent*)malloc(sizeof(UDPClientComponent));
    // probably it is good idea to zeroed the memory
	i->interval = 30000; // 30 seconds as interval
	return i;
}


PROCESS(udp_component_kev, "UDP Component");

static
int startUDPClient(void* instance)
{
    UDPClientComponent* i = (UDPClientComponent*)instance;
	KevContext* ctx = getContext(i);
	printf("Hey instance of udp client %s\n", getInstanceName(ctx));
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

PROCESS_THREAD(udp_component_kev, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer;
  static UDPClientComponent* inst;
  
  inst = (UDPClientComponent*) data;

  etimer_set(&timer, CLOCK_SECOND * (inst->interval/1000));
  
  /* TODO: initialize the UDP stuff */
  
  /* TODO: print the RIPLE(is it like that?) tree */

  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == PROCESS_EVENT_TIMER) {
    	printf("Current time: %d\n", RTIMER_NOW());
    	etimer_restart(&timer);
    	/* TODO: send message to the server */
    }
    else
    {
    	/* process network messages */
    }
  }
  PROCESS_END();
}
