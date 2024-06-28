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

#ifndef REGISTERED_E2_NODES_H
#define REGISTERED_E2_NODES_H 

#include "../../ric/plugin_ric.h"
#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../e2ap/e2_node_connected_wrapper.h"
#include "e2_node_arr.h"
#include "../../xApp/e2_node_arr_xapp.h"

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

typedef struct{
  seq_arr_t ran_func; //  ran_function_t
#ifdef E2AP_V1  
#elif defined (E2AP_V2) || defined(E2AP_V3)
  seq_arr_t comp_conf_add; // e2ap_node_component_config_add_t
#endif         
} pair_rf_cca_t;

void free_pair_rf_cca(pair_rf_cca_t* src);  

typedef struct{
  // key:global_e2_node_id_t | value: pair_rf_cca_t* of seq_arr_t of ran_function_t and 
  assoc_rb_tree_t node_to_rf;  
  pthread_mutex_t mtx;

} reg_e2_nodes_t;

void init_reg_e2_node(reg_e2_nodes_t* n); 

void free_reg_e2_node(reg_e2_nodes_t* n); 

#ifdef E2AP_V1
void add_reg_e2_node_v1(reg_e2_nodes_t* i, global_e2_node_id_t const* id, size_t len_rf, ran_function_t const* ran_func);
#else
void add_reg_e2_node(reg_e2_nodes_t* i, global_e2_node_id_t const* id, size_t len_rf, ran_function_t const* ran_func, size_t len_cca, e2ap_node_component_config_add_t const* cca);
#endif

void rm_reg_e2_node(reg_e2_nodes_t* n, global_e2_node_id_t const* id);

size_t sz_reg_e2_node(reg_e2_nodes_t* n);

assoc_rb_tree_t cp_reg_e2_node(reg_e2_nodes_t* n); 

e2_node_arr_t generate_e2_node_arr(reg_e2_nodes_t* n);

e2_node_arr_xapp_t generate_e2_node_arr_xapp(reg_e2_nodes_t* n, plugin_ric_t const* plg_ric);

#endif

