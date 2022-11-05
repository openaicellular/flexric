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


#include "e2ap_node_component_config.h"
#include <assert.h>
#include <stdlib.h>

e2_node_component_config_t cp_e2_node_component_config(const e2_node_component_config_t* src)
{
  assert(src != NULL);
  e2_node_component_config_t dst = { 0 };

  dst.interface_type = cp_e2_node_component_interface_type(src->interface_type);
  dst.id = cp_e2_node_component_id(dst.interface_type, &src->id);

  dst.configuration.request_part = copy_byte_array(src->configuration.request_part);
  dst.configuration.response_part = copy_byte_array(src->configuration.response_part);

  return dst;
}

void free_e2_node_component_config(e2_node_component_config_t* src)
{
  assert(src != NULL);

  free_e2_node_component_id(src->interface_type, &src->id);
  free_e2_node_component_interface_type(src->interface_type);

  free_byte_array(src->configuration.request_part);
  free_byte_array(src->configuration.response_part);
}

bool eq_e2_node_component_config(const e2_node_component_config_t* m0, const e2_node_component_config_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (!eq_e2_node_component_interface_type(m0->interface_type, m1->interface_type))
    return false;

  if(!eq_e2_node_component_id(m0->interface_type, &m0->id, &m1->id))
    return false;

  if(!eq_byte_array(&m0->configuration.request_part, &m1->configuration.request_part))
    return false;

  if(!eq_byte_array(&m0->configuration.response_part, &m1->configuration.response_part))
    return false;

  return true;
}

