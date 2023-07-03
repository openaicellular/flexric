/*
 *
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

#include "e2_setup_response.h"
#include <assert.h>

bool eq_e2_setup_response(const e2_setup_response_t* m0, const e2_setup_response_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(m0->trans_id != m1->trans_id)
    return false;

  if(eq_global_ric_id(&m0->id, &m1->id) == false )
    return false;

  if(m0->len_acc != m1->len_acc) 
    return false;

  for(size_t i = 0; i < m0->len_acc; ++i){
    if(m0->accepted[i] != m1->accepted[i])
      return false;
  }

  if(m0->len_rej != m1->len_rej)
    return false;

  for(size_t i = 0 ; i < m0->len_rej; ++i){
    if(eq_rejected_ran_function(&m0->rejected[i], &m1->rejected[i]) == false)
      return false;
  }

  if(m0->len_ccaa != m1->len_ccaa)
    return false;

  for(size_t i = 0 ; i < m0->len_ccaa; ++i){
    if(eq_e2ap_node_component_config_add_ack(m0->comp_config_add_ack, m1->comp_config_add_ack) == false)
      return false;
  }

  return true;
}


void free_e2_setup_response(e2_setup_response_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented");
}

