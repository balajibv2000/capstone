/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
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
 */

/**
 * \file
 *      Erbium (Er) CoAP Engine example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"


/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP "coap://[::1]"

//critical data 
uint8_t c_data[100]={0x00};

//////////////////////////////////////////////////////////////////////////////////



//medium-critical data

uint8_t m_data[100]={0x00};


/////////////////////////////////////////////////////////////////////////////////////

//periodic data

uint8_t p_data[100]={0x00};


///////////////////////////////////////////////////////////////////////////////////////



#define NUMBER_OF_URLS 3
char *service_urls[NUMBER_OF_URLS] =
{ "node/critical", "node/mid-critical", "node/periodic" };




void
client_chunk_handler(coap_message_t *response)
{
  const uint8_t *chunk;

  if(response == NULL) {
    puts("Request timed out");
    return;
  }

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////

//udp server post handlers
static void res_post_handler1(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler2(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler3(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/////////////////////////////////////////////////////////////////////////////////////////////////

//proxy get handlers

static void res_get_handler1(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_get_handler2(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_get_handler3(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);


//////////////////////////////////////////////////////////////////////////////////////

RESOURCE(res_critical,"title=\"Hello world: ?len=0..\";rt=\"Text\"",res_get_handler1,res_post_handler1,NULL,NULL);
         
RESOURCE(res_mid_critical,"title=\"Hello world: ?len=0..\";rt=\"Text\"",res_get_handler2, res_post_handler2,NULL,NULL);
 
RESOURCE(res_periodic,"title=\"Hello world: ?len=0..\";rt=\"Text\"",res_get_handler3,res_post_handler3,NULL,NULL);

///////////////////////////////////////////////////////////////////////////////////////////////////


//defn - post handlers
static void
res_post_handler1(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  const uint8_t *bytes = NULL;

  int len = 0;
  int strpos = 0;
  if(strpos <= COAP_MAX_CHUNK_SIZE && (len = coap_get_payload(request, &bytes))) {
    strpos += snprintf((char *)buffer + strpos, 100 - strpos + 1, "%.*s", len, bytes);
  } 

  coap_set_payload(response, "Ok", strpos);
  
  sprintf((char *)c_data , "%s" ,  buffer);

  LOG_DBG("/DATA received critical: %s \n", (char *)c_data);
  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void
res_post_handler2(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  const uint8_t *bytes = NULL;
  int len = 0;


  int strpos = 0;

  if(strpos <= REST_MAX_CHUNK_SIZE && (len = coap_get_payload(request, &bytes))) {
    strpos += snprintf((char *)buffer + strpos, REST_MAX_CHUNK_SIZE - strpos + 1, "%.*s", len, bytes);
  }
  

  coap_set_payload(response, "Ok", strpos);

  sprintf((char *)m_data , "%s" ,  buffer);

  LOG_DBG("/DATA received mid-critical: %s\n", (char *)m_data );


  
}

static void
res_post_handler3(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  const uint8_t *bytes = NULL;
  int len = 0;


  int strpos = 0;

  if(strpos <= REST_MAX_CHUNK_SIZE && (len = coap_get_payload(request, &bytes))) {
    strpos += snprintf((char *)buffer + strpos, REST_MAX_CHUNK_SIZE - strpos + 1, "%.*s", len, bytes);
  }
  

  coap_set_payload(response, "Ok", strpos);

   sprintf((char *)p_data , "%s" ,  buffer);

  LOG_DBG("/DATA received periodic: %s\n", (char *)p_data );
  
}


/////////////////////////////////////////////////////////////////////////////////////////

//get handler definition
static void res_get_handler1(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){

  unsigned int accept = -1;
  coap_get_header_accept(request, &accept);

  if(accept == -1 || accept == TEXT_PLAIN) {
    coap_set_header_content_format(response, TEXT_PLAIN);

    coap_set_payload(response, (uint8_t *)c_data, strlen((char *)c_data));
  } else if(accept == APPLICATION_JSON) {
    coap_set_header_content_format(response, APPLICATION_JSON);
    coap_set_payload(response, (uint8_t *)c_data, strlen((char *)c_data));
  } else {
    coap_set_status_code(response, NOT_ACCEPTABLE_4_06);
    const char *msg = "Supporting content-types text/plain and application/json";
    coap_set_payload(response, msg, strlen(msg));
  }

}



static void res_get_handler2(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){

unsigned int accept = -1;
  coap_get_header_accept(request, &accept);

  if(accept == -1 || accept == TEXT_PLAIN) {
    coap_set_header_content_format(response, TEXT_PLAIN);

    coap_set_payload(response, (uint8_t *)m_data, strlen((char *)m_data));
  } else if(accept == APPLICATION_JSON) {
    coap_set_header_content_format(response, APPLICATION_JSON);
    coap_set_payload(response, (uint8_t *)m_data, strlen((char *)m_data));
  } else {
    coap_set_status_code(response, NOT_ACCEPTABLE_4_06);
    const char *msg = "Supporting content-types text/plain and application/json";
    coap_set_payload(response, msg, strlen(msg));
  }

}




static void res_get_handler3(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){

unsigned int accept = -1;
  coap_get_header_accept(request, &accept);

  if(accept == -1 || accept == TEXT_PLAIN) {
    coap_set_header_content_format(response, TEXT_PLAIN);

    coap_set_payload(response, (uint8_t *)p_data, strlen((char *)p_data));
  } else if(accept == APPLICATION_JSON) {
    coap_set_header_content_format(response, APPLICATION_JSON);
    coap_set_payload(response, (uint8_t *)p_data, strlen((char *)p_data));
  } else {
    coap_set_status_code(response, NOT_ACCEPTABLE_4_06);
    const char *msg = "Supporting content-types text/plain and application/json";
    coap_set_payload(response, msg, strlen(msg));
  }

}

/////////////////////////////////////////////////////////////////////////////////////////

PROCESS(er_example_server, "Erbium Example Server");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  LOG_INFO("Starting Erbium Example Server\n");

  /*
   * Bind the resources to their Uri-Path.
   * WARNING: Activating twice only means alternate path, not two instances!
   * All static variables are the same for each URI path.
   */
  coap_activate_resource(&res_critical, "node/critical");
  coap_activate_resource(&res_mid_critical, "node/mid-critical");
  coap_activate_resource(&res_periodic, "node/periodic");


  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
  }                            

  PROCESS_END();
}




  


