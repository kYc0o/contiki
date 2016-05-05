/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/uip-debug.h"

#include "sys/node-id.h"

#include "simple-udp.h"
#include "servreg-hack.h"

#include "powertrace.h"

#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234
#define SERVICE_ID 190

#define SEND_INTERVAL		(3 * CLOCK_SECOND)
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))

static struct simple_udp_connection unicast_connection;

/*---------------------------------------------------------------------------*/
PROCESS(unicast_sender_process, "Unicast sender example process");
AUTOSTART_PROCESSES(&unicast_sender_process);
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  printf("Data received on port %d from port %d with length %d\n",
         receiver_port, sender_port, datalen);
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;
  int i;
  uint8_t state;

  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  printf("IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      printf("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(unicast_sender_process, ev, data)
{
  static struct etimer periodic_timer;
  static struct etimer send_timer;
  static rtimer_clock_t clockNow, clockAfter, idleTime;
  static rtimer_clock_t totalClock = 0;
  uip_ipaddr_t *addr;
  static int j = 0;

  PROCESS_BEGIN();

  servreg_hack_init();

  powertrace_start(CLOCK_SECOND * 10);

  set_global_address();

  simple_udp_register(&unicast_connection, UDP_PORT,
                      NULL, UDP_PORT, receiver);

  /*PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  etimer_reset(&periodic_timer);*/
  etimer_set(&send_timer, SEND_TIME);

  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
  etimer_set(&periodic_timer, SEND_INTERVAL);


  idleTime = RTIMER_NOW();
  printf("Sending 50 packets at %d\n", (unsigned int)idleTime);

  /*for (i = 0; i < 50; i = i + 1) {*/
  while (j < 50) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      etimer_reset(&periodic_timer);
      etimer_set(&send_timer, SEND_TIME);

      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
      addr = servreg_hack_lookup(SERVICE_ID);

      clockNow = RTIMER_NOW();

      /*clock_time();
      printf("NOW: %d\n", clockNow);*/

      if(addr != NULL) {
            static unsigned int message_number;
            char buf[65];

            printf("Sending unicast %d to ", j);
            uip_debug_ipaddr_print(addr);
            printf("\n");
            sprintf(buf, "Message buffer with 64 bytes long abcbdefghijklmnopqrstuvwxy %03d", message_number);
            message_number++;
            simple_udp_sendto(&unicast_connection, buf, strlen(buf) + 1, addr);
            j = j + 1;
      } else {
        printf("Service %d not found\n", SERVICE_ID);
      }

      clockAfter = RTIMER_NOW();
      totalClock += clockAfter - clockNow;
      /*printf("AFTER: %d, j is: %d\n", clockAfter, j);*/
  }

  printf("Total active time: %d\n", (unsigned int)totalClock);

  printf("Finished!\n");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
