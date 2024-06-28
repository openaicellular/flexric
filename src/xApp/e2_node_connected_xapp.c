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
 
#include "e2_node_connected_xapp.h"
#include "../sm/agent_if/read/sm_ag_if_rd.h"

#include <assert.h>

void free_e2_node_connected_xapp(e2_node_connected_xapp_t* src)
{
  assert(src != NULL);

  free_global_e2_node_id(&src->id);

#ifdef E2AP_V1
#elif defined(E2AP_V2) || defined(E2AP_V3)
  // [1-256]
  assert(src->len_cca > 0);
  for(size_t i = 0; i < src->len_cca; ++i){
    free_e2ap_node_component_config_add(&src->cca[i]);
  }
  free(src->cca);
#endif

  // Decoded RAN Function ran_function_t
  for(size_t i = 0; i < src->len_rf; ++i){
    free_sm_ran_function(&src->rf[i]);
  }

  if(src->rf != NULL)
    free(src->rf);
}

/*
e2_node_connected_xapp_t cp_e2_node_connected_xapp(e2_node_connected_xapp_t const* src)
{
  assert(src != NULL);

  e2_node_connected_xapp_t dst = {0}; 
  
  dst.id = cp_global_e2_node_id(&src->id);
  
  dst.len_rf = src->len_rf;
  if(dst.len_rf > 0){
    dst.ack_rf = calloc(dst.len_rf, sizeof(sm_ran_function_def_t));
    assert( );
  }
  for(size_t i = 0; i < dst.len_rf; ++i){
    dst.ack_rf[i] = cp_sm_ran_function_def(&src->ack_rf[i]);
  }

  return dst;
}

bool eq_e2_node_connected_xapp(e2_node_connected_xapp_t const* m0, e2_node_connected_xapp_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_global_e2_node_id(&m0->id, &m1->id) == false)
    return false;

  if(m0->len_rf != m1->len_rf)
    return false;
  
  for(size_t i = 0; i < m0->len_rf; ++i){
    if(eq_sm_ran_function_def(&m0->ack_rf[i], &m1->ack_rf[i]) == false)
      return false;
  }

  return true;
}
*/


