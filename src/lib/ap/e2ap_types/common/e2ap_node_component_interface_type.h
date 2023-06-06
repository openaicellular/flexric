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


#ifndef E2AP_NODE_COMPONENT_INTERFACE_TYPE_H
#define E2AP_NODE_COMPONENT_INTERFACE_TYPE_H

#include <stdbool.h>

typedef enum {
  E2_NODE_COMPONENT_INTERFACE_TYPE_NG = 0,
  E2_NODE_COMPONENT_INTERFACE_TYPE_XN = 1,
  E2_NODE_COMPONENT_INTERFACE_TYPE_E1 = 2,
  E2_NODE_COMPONENT_INTERFACE_TYPE_F1 = 3,
  E2_NODE_COMPONENT_INTERFACE_TYPE_W1 = 4,
  E2_NODE_COMPONENT_INTERFACE_TYPE_S1 = 5,
  E2_NODE_COMPONENT_INTERFACE_TYPE_X2 = 6
} e2_node_component_interface_type_e;

e2_node_component_interface_type_e cp_e2_node_component_interface_type(e2_node_component_interface_type_e src);

void free_e2_node_component_interface_type(e2_node_component_interface_type_e src);

bool eq_e2_node_component_interface_type(e2_node_component_interface_type_e m0, e2_node_component_interface_type_e m1);

#endif
