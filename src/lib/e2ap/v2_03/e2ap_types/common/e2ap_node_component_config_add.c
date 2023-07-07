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

#include "e2ap_node_component_config_add.h"
#include <assert.h>

void free_e2ap_node_component_config_add(e2ap_node_component_config_add_t* src)
{
  assert(src != NULL);

  // Mandatory
  // 9.2.26
  // e2ap_node_comp_interface_type_e e2_node_comp_interface_type;
  
  // Bug in the standard!!! Optional or Mandatory?
  // 9.2.32
  free_e2ap_node_comp_id(&src->e2_node_comp_id);

  // Mandatory
  // 9.2.27
  free_e2ap_node_comp_conf(&src->e2_node_comp_conf);;
}

e2ap_node_component_config_add_t cp_e2ap_node_component_config_add(e2ap_node_component_config_add_t const* src)
{
  assert(src != NULL);

  e2ap_node_component_config_add_t dst = {0}; 

  // Mandatory
  // 9.2.26
  dst.e2_node_comp_interface_type = src->e2_node_comp_interface_type ;
  // Optional
  // 9.2.32
  dst.e2_node_comp_id = cp_e2ap_node_comp_id(&src->e2_node_comp_id);

  // Mandatory
  // 9.2.27
  dst.e2_node_comp_conf = cp_e2ap_node_comp_conf(&src->e2_node_comp_conf);

  return dst;
}

bool eq_e2ap_node_component_config_add(e2ap_node_component_config_add_t const* m0, e2ap_node_component_config_add_t const* m1)
{
  if(m0 == m1)
    return true;
  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  // 9.2.26
  if(m0->e2_node_comp_interface_type != m1->e2_node_comp_interface_type  ) 
    return false;
  
  // Optional
  // 9.2.32
  if(eq_e2ap_node_comp_id(&m0->e2_node_comp_id,&m1->e2_node_comp_id  ) == false)
    return false;

  // Mandatory
  // 9.2.27
  if(eq_e2ap_node_comp_conf(&m0->e2_node_comp_conf, &m1->e2_node_comp_conf) == false)
    return false;

  return true;
}

