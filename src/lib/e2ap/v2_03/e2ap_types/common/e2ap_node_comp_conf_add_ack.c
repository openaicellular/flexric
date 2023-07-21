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

#include "e2ap_node_comp_conf_add_ack.h"
#include <assert.h>
#include <stdlib.h>

void free_e2ap_node_comp_conf_ack(e2ap_node_comp_conf_ack_t* src)
{
  assert(src != NULL);

  // Mandatory
  // Outcome
  //outcome_e2ap_node_comp_conf_ack_e outcome;

  // Optional
  // 9.2.1
  assert(src->cause == NULL && "Not implemented");
}


e2ap_node_comp_conf_ack_t cp_e2ap_node_comp_conf_ack( e2ap_node_comp_conf_ack_t const* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented");

  e2ap_node_comp_conf_ack_t dst = {0}; 
  return dst;
}

bool eq_e2ap_node_comp_conf_ack(e2ap_node_comp_conf_ack_t const* m0, e2ap_node_comp_conf_ack_t const* m1)
{
  if(m0 == m1)
    return true;
  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  // Outcome
  if(m0->outcome != m1->outcome)
    return false;


  // Optional
  // 9.2.1
  assert(m0->cause == NULL && "Not implemented");
  assert(m1->cause == NULL && "Not implemented");

  return true;
}

