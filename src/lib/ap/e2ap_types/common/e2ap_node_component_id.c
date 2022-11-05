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


#include "e2ap_node_component_id.h"
#include <assert.h>
#include <stdlib.h>

e2_node_component_id_t cp_e2_node_component_id(e2_node_component_interface_type_e interface_type, const e2_node_component_id_t* src)
{
  assert(src != NULL);
  e2_node_component_id_t id = { 0 };

  switch (interface_type) {
    case E2_NODE_COMPONENT_INTERFACE_TYPE_NG:
      id.ng.amf_name = copy_byte_array(src->ng.amf_name);
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_XN:
      id.xn.global_ng_ran_node_id = src->xn.global_ng_ran_node_id;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_E1:
      id.e1.gnb_cu_up_id = src->e1.gnb_cu_up_id;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_F1:
      id.f1.gnb_du_id = src->f1.gnb_du_id;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_W1:
      id.w1.ng_enb_du_id = src->w1.ng_enb_du_id;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_S1:
      id.s1.mme_name = copy_byte_array(src->s1.mme_name);
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_X2:
      if (src->x2.global_enb_id) {
        id.x2.global_enb_id = malloc(sizeof(*id.x2.global_enb_id));
        *id.x2.global_enb_id = *src->x2.global_enb_id;
      }
      if (src->x2.global_en_gnb_id) {
        id.x2.global_en_gnb_id = malloc(sizeof(*id.x2.global_en_gnb_id));
        *id.x2.global_en_gnb_id = *src->x2.global_en_gnb_id;
      }
      break;
    default:
      assert(false && "illegal code path");
      break;
  }

  return id;
}

void free_e2_node_component_id(e2_node_component_interface_type_e interface_type, e2_node_component_id_t* src)
{
  assert(src != NULL);

  switch (interface_type) {
    case E2_NODE_COMPONENT_INTERFACE_TYPE_NG:
      free_byte_array(src->ng.amf_name);
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_XN:
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_E1:
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_F1:
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_W1:
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_S1:
      free_byte_array(src->s1.mme_name);
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_X2:
      if (src->x2.global_enb_id)
        free(src->x2.global_enb_id);
      if (src->x2.global_en_gnb_id)
        free(src->x2.global_en_gnb_id);
      break;
    default:
      assert(false && "illegal code path");
      break;
  }
}

static
bool eq_uint64_t(const uint64_t* m0, const uint64_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  return *m0 == *m1;
}

bool eq_e2_node_component_id(e2_node_component_interface_type_e it, const e2_node_component_id_t* m0, const e2_node_component_id_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  switch (it) {
    case E2_NODE_COMPONENT_INTERFACE_TYPE_NG:
      if (!eq_byte_array(&m0->ng.amf_name, &m1->ng.amf_name))
        return false;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_XN:
      if (m0->xn.global_ng_ran_node_id != m1->xn.global_ng_ran_node_id)
        return false;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_E1:
      if (m0->e1.gnb_cu_up_id != m1->e1.gnb_cu_up_id)
        return false;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_F1:
      if (m0->f1.gnb_du_id != m1->f1.gnb_du_id)
        return false;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_W1:
      if (m0->w1.ng_enb_du_id != m1->w1.ng_enb_du_id)
        return false;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_S1:
      if (!eq_byte_array(&m0->s1.mme_name, &m1->s1.mme_name))
        return false;
      break;
    case E2_NODE_COMPONENT_INTERFACE_TYPE_X2:
      if (!eq_uint64_t(m0->x2.global_enb_id, m1->x2.global_enb_id))
        return false;
      if (!eq_uint64_t(m0->x2.global_en_gnb_id, m1->x2.global_en_gnb_id))
        return false;
      break;
    default:
        assert(false && "illegal code path");
        break;
  }

  return true;
}
