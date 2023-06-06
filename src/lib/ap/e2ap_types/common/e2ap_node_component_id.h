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


#ifndef E2AP_NODE_COMPONENT_ID_H
#define E2AP_NODE_COMPONENT_ID_H

#include "e2ap_node_component_interface_type.h"
#include "../../../../util/byte_array.h"
#include <stdint.h>

typedef union {
  struct {
    byte_array_t amf_name;
  } ng;
  struct {
    uint64_t global_ng_ran_node_id; // incomplete/wrong type
  } xn;
  struct {
    uint64_t gnb_cu_up_id;
  } e1;
  struct {
    uint64_t gnb_du_id;
  } f1;
  struct {
    uint64_t ng_enb_du_id;
  } w1;
  struct {
    byte_array_t mme_name;
  } s1;
  struct {
    uint64_t* global_enb_id; // optional
    uint64_t* global_en_gnb_id; // optional
  } x2;
} e2_node_component_id_t;

e2_node_component_id_t cp_e2_node_component_id(e2_node_component_interface_type_e interface_type, const e2_node_component_id_t* src);
void free_e2_node_component_id(e2_node_component_interface_type_e interface_type, e2_node_component_id_t* src);
bool eq_e2_node_component_id(e2_node_component_interface_type_e interface_type, const e2_node_component_id_t* m0, const e2_node_component_id_t* m1);

#endif

