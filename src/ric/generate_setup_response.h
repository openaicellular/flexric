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

#ifndef GENERATE_SETUP_RESPONSE_MIR_H
#define GENERATE_SETUP_RESPONSE_MIR_H

#include "../lib/e2ap/e2_setup_response_wrapper.h"
#include "../lib/e2ap/e2_setup_request_wrapper.h"

typedef struct near_ric_s near_ric_t;

e2_setup_response_t generate_setup_response_v1(near_ric_t* ric, const e2_setup_request_t* req);

e2_setup_response_t generate_setup_response_v2(near_ric_t* ric, const e2_setup_request_t* req);

e2_setup_response_t generate_setup_response_v3(near_ric_t* ric, const e2_setup_request_t* req);


#define generate_setup_response(T,U,V) _Generic ((T), e2ap_v1_t*: generate_setup_response_v1, \
                                    e2ap_v2_t*: generate_setup_response_v2, \
                                    e2ap_v3_t*: generate_setup_response_v3, \
                                    default:  generate_setup_response_v1) (U,V)

#endif

