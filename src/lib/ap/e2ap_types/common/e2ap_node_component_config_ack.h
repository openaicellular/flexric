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


#ifndef E2AP_NODE_COMPONENT_CONFIG_ACK_H
#define E2AP_NODE_COMPONENT_CONFIG_ACK_H

#include "../../../../util/byte_array.h"
#include "e2ap_node_component_interface_type.h"
#include "e2ap_node_component_id.h"
#include "e2ap_cause.h"
#include <stdint.h>

typedef enum {
  E2_NODE_COMPONENT_CONFIGURATION_ACKNOWLEDGE_SUCCESS = 0,
  E2_NODE_COMPONENT_CONFIGURATION_ACKNOWLEDGE_FAILURE = 1
} e2_node_component_config_ack_outcome_e;

typedef struct {
  e2_node_component_config_ack_outcome_e outcome;
  cause_t* cause; // optional
} e2_node_component_configuration_acknowledge_t;

typedef struct {
  e2_node_component_interface_type_e interface_type;
  e2_node_component_id_t id;
  e2_node_component_configuration_acknowledge_t config_ack;
} e2_node_component_config_ack_t;

e2_node_component_config_ack_t cp_e2_node_component_config_ack(const e2_node_component_config_ack_t* src);
void free_e2_node_component_config_ack(e2_node_component_config_ack_t* src);
bool eq_e2_node_component_config_ack(const e2_node_component_config_ack_t* m0, const e2_node_component_config_ack_t* m1);

#endif
