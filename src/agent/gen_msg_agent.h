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

#ifndef GEN_MSG_VERSION_AGENT_MIR_H
#define GEN_MSG_VERSION_AGENT_MIR_H

#include "e2_agent.h"
#include "lib/e2ap/e2_setup_request_wrapper.h"

e2_setup_request_t gen_setup_request_v1(e2_agent_t* ag);

e2_setup_request_t gen_setup_request_v2(e2_agent_t* ag);

e2_setup_request_t gen_setup_request_v3(e2_agent_t* ag);


#define gen_setup_request(T,U) _Generic ((T), e2ap_v1_t*: gen_setup_request_v1, \
                                              e2ap_v2_t*: gen_setup_request_v2, \
                                             e2ap_v3_t*: gen_setup_request_v3, \
                                             default: gen_setup_request_v1) (U)

#endif
