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

#include "e2_node_ric_id.h"

#include <assert.h>
#include <stdlib.h>

e2_node_ric_id_t cp_e2_node_ric_id(e2_node_ric_id_t const* src)
{
  assert(src != NULL);

  e2_node_ric_id_t dst = {0};
  dst.ric_id = src->ric_id;
  dst.ric_req_type = src->ric_req_type;
  dst.e2_node_id = cp_global_e2_node_id(&src->e2_node_id);

  return dst;
}


void free_e2_node_ric_id(e2_node_ric_id_t* src)
{
  assert(src != NULL);

  free_global_e2_node_id(&src->e2_node_id);
}


void free_e2_node_ric_id_wrapper(void* src)
{
  assert(src != NULL);
  return free_e2_node_ric_id(src);
}

