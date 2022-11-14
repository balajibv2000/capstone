/*
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
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"

#include "sys/log.h"



#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL  LOG_LEVEL_APP

/* FIXME: This server address is hard-coded for Cooja and link-local for unconnected border router. */
#define SERVER_EP "coap://[fe80::216:16:16:16]"

#define START_INTERVAL		  (100 * CLOCK_SECOND)
#define TOGGLE_INTERVAL 		200


static struct simple_udp_connection udp_conn;

//periodic data

uint8_t p_data[100]={0x00};

static struct etimer start_timer;
static struct etimer et;  


PROCESS(er_example_client, "CoAP");
PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  LOG_INFO("Received request '%.*s' from", datalen, (char *) data);
  
  //payload[counter] = (char *) data;
  //sprintf(payload , "%s" , (char *) data); 
 
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  
	int j=0;
  
  while(j<100)
  {
  	if(p_data[j]==0x00)
  	{
  		sprintf((char *)p_data+j,"%d %s",sender_addr->u8[sizeof(sender_addr->u8) - 1] ,(char *) data);
  		break;
  	}
  	j++;
  }  

#if WITH_SERVER_REPLY
  
#endif /* WITH_SERVER_REPLY */
}
/*---------------------------------------------------------------------------*/

char *service_urls = "node/periodic";

/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(coap_message_t *response)
{
  const uint8_t *chunk;

  if(response == NULL) {
    puts("Request timed out");
    return;
  }

  int len = coap_get_payload(response, &chunk);

  printf("|%d \n", len);
}

PROCESS_THREAD(udp_server_process, ev, data)
{
   
  PROCESS_BEGIN();
    /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();
  
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,UDP_CLIENT_PORT, udp_rx_callback);

	etimer_set(&start_timer, START_INTERVAL);

	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&start_timer));
  
  etimer_set(&et, TOGGLE_INTERVAL * CLOCK_SECOND);
  while(1) {

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		
    PROCESS_NAME(er_example_client);
  	process_start(&er_example_client, NULL);
    
    etimer_reset(&et);
  	
  }
  

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(er_example_client, ev, data)
{
  static coap_endpoint_t server_ep;
  PROCESS_BEGIN();

  static coap_message_t request[1];      /* This way the packet can be treated as pointer as usual. */

  coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);


  printf("--Toggle timer--\n");

  /* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
  coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
  coap_set_header_uri_path(request, service_urls);
	
  coap_set_payload(request, (uint8_t *)p_data, sizeof(p_data) - 1);

  LOG_INFO_COAP_EP(&server_ep);
  LOG_INFO_("\n");

  COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
 
  
 	int j=0; 
  while(j<100)
  {
  	p_data[j] = 0x00;
  	j++;
  }


  printf("\n--Done--\n");
      

  PROCESS_END();
}
