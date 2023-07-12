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

#include "generate_setup_response.h"
#include "near_ric.h"
#include "../util/alg_ds/alg/alg.h"
#include "../util/compare.h"
#include "assert.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


typedef uint16_t accepted_ran_function_t;


typedef struct{
 size_t len_acc;
  accepted_ran_function_t* acc; 
} accepted_pair_t ;

accepted_pair_t accept_ran_func(near_ric_t* ric, const e2_setup_request_t* req)
{
  // All the functions are accepted
  accepted_pair_t dst = {.len_acc = req->len_rf }; 

  dst.acc = calloc(dst.len_acc, sizeof(accepted_ran_function_t));
  assert(dst.acc != NULL && "Memory exhausted");

  for (size_t i = 0; i < dst.len_acc; ++i) {
    void* start_it = assoc_front(&ric->plugin.sm_ds);
    void* end_it = assoc_end(&ric->plugin.sm_ds);
    uint16_t const id = req->ran_func_item[i].id;

    void* it = find_if(&ric->plugin.sm_ds, start_it, end_it, &id, eq_ran_func_id); 

    if(it != end_it){
      assert(id == *(uint16_t*)assoc_key(&ric->plugin.sm_ds, it) );
      dst.acc[i] = id;
      sm_ric_t* sm = (sm_ric_t*)assoc_value(&ric->plugin.sm_ds, it);
      char def[33] = {0};
      memcpy(def, sm->ran_func_name, 32);
      printf("[NEAR-RIC]: Accepting RAN function ID %d with def = %s \n", id, def);
    } else {
      printf("Unknown RAN function ID, thus rejecting %d \n", id);

      void* it = assoc_front(&ric->plugin.sm_ds);
      while(it != assoc_end(&ric->plugin.sm_ds)){
        printf("Registered keys = %d \n", *(uint16_t*)assoc_key(&ric->plugin.sm_ds, it));        
        it = assoc_next(&ric->plugin.sm_ds, it);
      }
      fflush(stdout);
      assert(0!=0 && "Unknown RAN function ID from the agent received");
    }
  }
  return dst;
}



#ifdef E2AP_V1

e2_setup_response_t generate_setup_response_v1(near_ric_t* ric, const e2_setup_request_t* req)
{
  assert(ric != NULL);
  assert(req != NULL);

  accepted_pair_t const acc = accept_ran_func(ric, req);

  e2_setup_response_t sr = {
      .id.plmn = req->id.plmn, 
      .id.near_ric_id.double_word = 25,
      .accepted = acc.acc,
      .len_acc = acc.len_acc,
      .rejected = NULL,
      .len_rej = 0,
      .comp_conf_update_ack_list = NULL,
      .len_ccual = 0
  };
  return sr;

}

#elif defined(E2AP_V2) || defined(E2AP_V3)

e2_setup_response_t generate_setup_response_v2(near_ric_t* ric, const e2_setup_request_t* req)
{
  assert(ric != NULL);
  assert(req != NULL);

  accepted_pair_t const acc = accept_ran_func(ric, req);

  // [1-1024]
  assert(req->len_cca > 0 && req->len_cca < 1025);
  size_t const len_ccaa = req->len_cca; 
  e2ap_node_comp_config_add_ack_t* add_ack = calloc(len_ccaa, sizeof(e2ap_node_comp_config_add_ack_t));
  assert(add_ack != NULL && "Memory exhausted");

  for(size_t i = 0; i < len_ccaa; ++i){
    add_ack[i].e2_node_comp_interface_type = req->comp_conf_add[i].e2_node_comp_interface_type;  
    add_ack[i].e2_node_comp_id = cp_e2ap_node_comp_id(&req->comp_conf_add[i].e2_node_comp_id);
    add_ack[i].e2_node_comp_conf_ack.outcome = SUCCESS_E2AP_NODE_COMP_CONF_ACK; 
  }

  e2_setup_response_t sr = {
      .trans_id = req->trans_id,
      .id.plmn = req->id.plmn, 
      .id.near_ric_id.double_word = 25,
      .accepted = acc.acc,
      .len_acc = acc.len_acc,
      .rejected = NULL,
      .len_rej = 0,
      .comp_config_add_ack = add_ack,
      .len_ccaa  = len_ccaa
  };
  return sr;
}

e2_setup_response_t generate_setup_response_v3(near_ric_t* ric, const e2_setup_request_t* req)
{
  return  generate_setup_response_v2(ric, req);
}

#else
static_assert(0!=0, "Unknown E2AP version type");
#endif
