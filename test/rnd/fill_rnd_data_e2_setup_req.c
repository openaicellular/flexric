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

#include "fill_rnd_data_e2_setup_req.h"
#include "../../src/lib/e2ap/e2ap_node_comp_interface_type_wrapper.h"
#include "../../src/util/byte_array.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef E2AP_V1

#elif defined(E2AP_V2) || defined(E2AP_V3) 

e2ap_node_component_config_add_t fill_ngap_e2ap_node_component_config_add(void)
{
  e2ap_node_component_config_add_t dst = {0};

  // Mandatory
  // 9.2.26
  dst.e2_node_comp_interface_type = NG_E2AP_NODE_COMP_INTERFACE_TYPE;
  // Bug!! Optional in the standard, mandatory in ASN.1
  // 9.2.32
  dst.e2_node_comp_id.type = NG_E2AP_NODE_COMP_INTERFACE_TYPE;
 
  const char ng_msg[] = "Dummy message";
  dst.e2_node_comp_id.ng_amf_name = cp_str_to_ba(ng_msg); 
 
  // Mandatory
  // 9.2.27
  const char req[] = "NGAP Request Message sent";
  const char res[] = "NGAP Response Message reveived";

  dst.e2_node_comp_conf.request = cp_str_to_ba(req); 
  dst.e2_node_comp_conf.response = cp_str_to_ba(res); 
  return dst;
}

// F1AP
e2ap_node_component_config_add_t fill_f1ap_e2ap_node_component_config_add(void)
{
  e2ap_node_component_config_add_t dst = {0};

  // Mandatory
  // 9.2.26
  dst.e2_node_comp_interface_type = F1_E2AP_NODE_COMP_INTERFACE_TYPE;
  // Bug!! Optional in the standard, mandatory in ASN.1
  // 9.2.32
  dst.e2_node_comp_id.type = F1_E2AP_NODE_COMP_INTERFACE_TYPE;

  dst.e2_node_comp_id.f1_gnb_du_id = 1023;

  // Mandatory
  // 9.2.27
  const char req[] = "F1AP Request Message sent";
  const char res[] = "F1AP Response Message reveived";

  dst.e2_node_comp_conf.request = cp_str_to_ba(req);
  dst.e2_node_comp_conf.response = cp_str_to_ba(res);
  return dst;
}
// E1AP
e2ap_node_component_config_add_t fill_e1ap_e2ap_node_component_config_add(void)
{
  e2ap_node_component_config_add_t dst = {0};

  // Mandatory
  // 9.2.26
  dst.e2_node_comp_interface_type = E1_E2AP_NODE_COMP_INTERFACE_TYPE;
  // Bug!! Optional in the standard, mandatory in ASN.1
  // 9.2.32
  dst.e2_node_comp_id.type = E1_E2AP_NODE_COMP_INTERFACE_TYPE;

  dst.e2_node_comp_id.e1_gnb_cu_up_id = 1025;

  // Mandatory
  // 9.2.27
  const char req[] = "E1AP Request Message sent";
  const char res[] = "E1AP Response Message reveived";

  dst.e2_node_comp_conf.request = cp_str_to_ba(req);
  dst.e2_node_comp_conf.response = cp_str_to_ba(res);
  return dst;
}

// S1AP
e2ap_node_component_config_add_t fill_s1ap_e2ap_node_component_config_add(void)
{
  e2ap_node_component_config_add_t dst = {0};

  // Mandatory
  // 9.2.26
  dst.e2_node_comp_interface_type = S1_E2AP_NODE_COMP_INTERFACE_TYPE;
  // Bug!! Optional in the standard, mandatory in ASN.1
  // 9.2.32
  dst.e2_node_comp_id.type = S1_E2AP_NODE_COMP_INTERFACE_TYPE;

  const char str[] = "S1 NAME";
  dst.e2_node_comp_id.s1_mme_name = cp_str_to_ba(str);

  // Mandatory
  // 9.2.27
  const char req[] = "S1AP Request Message sent";
  const char res[] = "S1AP Response Message reveived";

  dst.e2_node_comp_conf.request = cp_str_to_ba(req);
  dst.e2_node_comp_conf.response = cp_str_to_ba(res);
  return dst;
}



#else
static_assert(0!=0, "Unknown E2AP version");
#endif

