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

#include "gen_msg_agent.h"

#include <assert.h>
#include <stdlib.h>

#ifdef E2AP_V1
e2_setup_request_t gen_setup_request_v1(e2_agent_t* ag)
{
  assert(ag != NULL);

  const size_t len_rf = assoc_size(&ag->plugin.sm_ds);
  assert(len_rf > 0 && "No RAN function/service model registered. Check if the Service Models are located at shared library paths, default location is /usr/local/lib/flexric/ ");

  ran_function_t* ran_func = calloc(len_rf, sizeof(*ran_func));
  assert(ran_func != NULL);

  e2_setup_request_t sr = {
    .id = ag->global_e2_node_id,
    .ran_func_item = ran_func,
    .len_rf = len_rf,
    .comp_conf_update = NULL,
    .len_ccu = 0
  };

  void* it = assoc_front(&ag->plugin.sm_ds);
  for(size_t i = 0; i < len_rf; ++i){
    sm_agent_t* sm = assoc_value(&ag->plugin.sm_ds, it);
    assert(sm->info.id() == *(uint16_t*)assoc_key(&ag->plugin.sm_ds, it) && "RAN function mismatch");

    sm_e2_setup_data_t def = sm->proc.on_e2_setup(sm);
    // Pass memory ownership
    ran_func[i].defn.len = def.len_rfd;
    ran_func[i].defn.buf = def.ran_fun_def;

    ran_func[i].id = sm->info.id();
    ran_func[i].rev = sm->info.rev();
   ran_func[i].oid = calloc(1, sizeof(byte_array_t)); 
   assert(ran_func[i].oid != NULL && "Memory exhausted");
   *ran_func[i].oid = cp_str_to_ba(sm->info.oid());

    it = assoc_next(&ag->plugin.sm_ds ,it);
  }
  assert(it == assoc_end(&ag->plugin.sm_ds) && "Length mismatch");

  return sr;
}
#elif defined(E2AP_V2) || defined (E2AP_V3)
e2_setup_request_t gen_setup_request_v2(e2_agent_t* ag)
{
  assert(ag != NULL);

  const size_t len_rf = assoc_size(&ag->plugin.sm_ds);
  assert(len_rf > 0 && "No RAN function/service model registered. Check if the Service Models are located at shared library paths, default location is /usr/local/lib/flexric/ ");

  ran_function_t* ran_func = calloc(len_rf, sizeof(*ran_func));
  assert(ran_func != NULL);

  // ToDO: Transaction ID needs to be considered within the pending messages
  e2_setup_request_t sr = {
    .trans_id = ag->trans_id_setup_req++,
    .id = cp_global_e2_node_id(&ag->global_e2_node_id),
    .ran_func_item = ran_func,
    .len_rf = len_rf,
  };

  void* it = assoc_front(&ag->plugin.sm_ds);
  for(size_t i = 0; i < len_rf; ++i){
    sm_agent_t* sm = assoc_value(&ag->plugin.sm_ds, it);
    assert(sm->info.id() == *(uint16_t*)assoc_key(&ag->plugin.sm_ds, it) && "RAN function mismatch");

    sm_e2_setup_data_t def = sm->proc.on_e2_setup(sm);
    // Pass memory ownership
    ran_func[i].defn.len = def.len_rfd;
    ran_func[i].defn.buf = def.ran_fun_def;

    ran_func[i].id = sm->info.id();
    ran_func[i].rev = sm->info.rev();
    ran_func[i].oid = cp_str_to_ba(sm->info.oid());
    it = assoc_next(&ag->plugin.sm_ds ,it);
  }
  assert(it == assoc_end(&ag->plugin.sm_ds) && "Length mismatch");

  // E2 Node Component Configuration Addition List
  arr_node_component_config_add_t arr = {0};
  ag->read_setup_ran(&arr, ag->global_e2_node_id.type);
  // Move ownership
  sr.len_cca = arr.len_cca;
  sr.comp_conf_add = arr.cca;

  return sr;
}

e2_setup_request_t gen_setup_request_v3(e2_agent_t* ag)
{
  return gen_setup_request_v2(ag);
}


/*
#elif defined(E2AP_V3
e2_setup_request_t gen_setup_request_v3(e2_agent_t* ag)
{
  return gen_setup_request_v2(ag);
}
*/

#else
static_assert(0!=0, "Unknown E2AP version");
#endif





