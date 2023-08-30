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

#include "e2ap_node_comp_conf.h"
#include <assert.h>

void free_e2ap_node_comp_conf(e2ap_node_comp_conf_t* src)
{
  assert(src != NULL);

  free_byte_array(src->request); 
  free_byte_array(src->response); 
}

bool eq_e2ap_node_comp_conf(e2ap_node_comp_conf_t const* m0, e2ap_node_comp_conf_t const* m1)
{
  if(m0 == m1)
    return true;
  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_byte_array(&m0->request, &m1->request) == false)
    return false;

  if(eq_byte_array(&m0->response, &m1->response) == false)
    return false;

  return true;
}

e2ap_node_comp_conf_t cp_e2ap_node_comp_conf(e2ap_node_comp_conf_t const* src)
{
  assert(src != NULL);

  e2ap_node_comp_conf_t dst = {0};

  dst.request = copy_byte_array(src->request); 
  dst.response = copy_byte_array(src->response);  

  return dst;
}

