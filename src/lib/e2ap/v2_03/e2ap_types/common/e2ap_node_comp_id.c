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

#include "e2ap_node_comp_id.h"
#include <assert.h>

void free_e2ap_node_comp_id(e2ap_node_comp_id_t* src)
{
  assert(src != NULL);

  assert((src->type == NG_E2AP_NODE_COMP_INTERFACE_TYPE || src->type == F1_E2AP_NODE_COMP_INTERFACE_TYPE)  && "Only these two types supported" );

  if(src->type == NG_E2AP_NODE_COMP_INTERFACE_TYPE  ){
    free_byte_array(src->ng_amf_name);
  } else if (src->type ==  F1_E2AP_NODE_COMP_INTERFACE_TYPE ){

  } else {
    assert(0 != 0 && "Unknown type");
  }

}

e2ap_node_comp_id_t cp_e2ap_node_comp_id(e2ap_node_comp_id_t const* src)
{
  assert(src != NULL);

  assert((src->type == NG_E2AP_NODE_COMP_INTERFACE_TYPE || src->type == F1_E2AP_NODE_COMP_INTERFACE_TYPE)  && "Only these two types supported" );

  e2ap_node_comp_id_t dst = {.type = src->type }; 

  if(src->type == NG_E2AP_NODE_COMP_INTERFACE_TYPE  ){
    dst.ng_amf_name = copy_byte_array(src->ng_amf_name); 
  } else if (src->type ==  F1_E2AP_NODE_COMP_INTERFACE_TYPE ){
    assert(src->f1_gnb_du_id < (1UL << 36));
    dst.f1_gnb_du_id = src->f1_gnb_du_id;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  return dst;
}

bool eq_e2ap_node_comp_id(e2ap_node_comp_id_t const* m0, e2ap_node_comp_id_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  assert((m0->type == NG_E2AP_NODE_COMP_INTERFACE_TYPE || m0->type == F1_E2AP_NODE_COMP_INTERFACE_TYPE)  && "Only these two types supported" );
  assert((m1->type == NG_E2AP_NODE_COMP_INTERFACE_TYPE || m1->type == F1_E2AP_NODE_COMP_INTERFACE_TYPE)  && "Only these two types supported" );

  if(m0->type != m1->type)
    return false;

  if(m0->type == NG_E2AP_NODE_COMP_INTERFACE_TYPE  ){
    if(eq_byte_array(&m0->ng_amf_name, &m1->ng_amf_name) ==false)
      return false;
  } else if (m0->type == F1_E2AP_NODE_COMP_INTERFACE_TYPE ){
    assert(m0->f1_gnb_du_id < (1UL << 36));
    assert(m1->f1_gnb_du_id < (1UL << 36));
    if(m0->f1_gnb_du_id != m1->f1_gnb_du_id)
      return false;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  return true;
}

