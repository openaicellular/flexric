/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include "generate_setup_failure.h"
#include "near_ric.h"
#include "../util/alg_ds/alg/alg.h"
#include "../util/compare.h"
#include "assert.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#ifdef E2AP_V1

e2_setup_failure_t generate_setup_failure_v1(near_ric_t* ric, const e2_setup_request_t* req)
{
  assert(ric != NULL);
  assert(req != NULL);
  assert(0 != 0 && "Not implemented");
  e2_setup_failure_t dst = {0};
  return dst;
}

#elif defined(E2AP_V2) || defined(E2AP_V3)

e2_setup_failure_t generate_setup_failure_v2(near_ric_t* ric, const e2_setup_request_t* req)
{
  assert(ric != NULL);
  assert(req != NULL);

  cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};

  //  e2ap_time_to_wait_e* time_to_wait_ms = NULL;
  e2ap_time_to_wait_e* time_to_wait_ms = calloc(1, sizeof(e2ap_time_to_wait_e)); 
  *time_to_wait_ms = TIMETOWAIT_V1S; 

  criticality_diagnostics_t* crit_diag = NULL; // optional
  transport_layer_information_t* tl_info = calloc(1,sizeof(transport_layer_information_t)); // optional
  const char* addr = "192.168.1.0";
  tl_info->address.buf = malloc(strlen(addr)); 
  memcpy(tl_info->address.buf, addr, strlen(addr) );
  tl_info->address.len = strlen(addr); 
  tl_info->port = calloc(1,sizeof(byte_array_t));
  const char* port = "10";
  tl_info->port->buf = malloc(strlen(port));
  memcpy(tl_info->port->buf, port, strlen(port));
  tl_info->port->len = strlen(port);  

  e2_setup_failure_t dst = {
    .trans_id = rand(),
    .cause = cause,
    .time_to_wait_ms = time_to_wait_ms,            // optional
    .crit_diag = crit_diag, // optional
    .tl_info = tl_info, // optional
  };

//  e2_setup_failure_t dst = {0};
//  dst.cause.present = CAUSE_RICREQUEST; 
//  dst.cause.ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID;

  return dst;
}

e2_setup_failure_t generate_setup_failure_v3(near_ric_t* ric, const e2_setup_request_t* req)
{
  return  generate_setup_failure_v2(ric, req);
}

#endif

