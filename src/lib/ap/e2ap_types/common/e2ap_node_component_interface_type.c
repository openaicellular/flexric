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

#include "e2ap_node_component_interface_type.h"
#include <assert.h>
#include <stdlib.h>

e2_node_component_interface_type_e cp_e2_node_component_interface_type(e2_node_component_interface_type_e src)
{
  return src;
}

void free_e2_node_component_interface_type(e2_node_component_interface_type_e src)
{
  (void) src;
}

bool eq_e2_node_component_interface_type(e2_node_component_interface_type_e m0, e2_node_component_interface_type_e m1)
{
  return m0 == m1;
}
