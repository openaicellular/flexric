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


#ifndef E2AP_NODE_COMPONENT_CONFIG_H
#define E2AP_NODE_COMPONENT_CONFIG_H

#include "../../../../util/byte_array.h"
#include "e2ap_node_component_interface_type.h"
#include "e2ap_node_component_id.h"
#include <stdint.h>

typedef struct {
  byte_array_t request_part;
  byte_array_t response_part;
} e2_node_component_configuration_t;

typedef struct {
  e2_node_component_interface_type_e interface_type;
  e2_node_component_id_t id;
  e2_node_component_configuration_t configuration;
} e2_node_component_config_t;

e2_node_component_config_t cp_e2_node_component_config(const e2_node_component_config_t* src);
void free_e2_node_component_config(e2_node_component_config_t* src);
bool eq_e2_node_component_config(const e2_node_component_config_t* m0, const e2_node_component_config_t* m1);

#endif

