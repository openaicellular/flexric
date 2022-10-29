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



#include "ric_control_ack.h"
#include <assert.h>
#include <stdlib.h>


ric_control_acknowledge_t cp_ric_control_ack( ric_control_acknowledge_t const* src)
{
  assert(src != NULL);

  ric_control_acknowledge_t dst = {0}; 

  dst.ric_id = src->ric_id;

  if(src->call_process_id != NULL){
    dst.call_process_id = calloc(1, sizeof(byte_array_t)) ;
    assert(dst.call_process_id != NULL);
    *dst.call_process_id = copy_byte_array(*src->call_process_id);
  }

  dst.status = src->status;

  if(src-> control_outcome  != NULL){
    dst.control_outcome  = calloc(1, sizeof(byte_array_t)) ;
    assert(dst.control_outcome != NULL);
    *dst.control_outcome  = copy_byte_array(*src->control_outcome  );
  }

  return dst;
}



bool eq_ric_control_ack_req(const ric_control_acknowledge_t* m0, const ric_control_acknowledge_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(eq_ric_gen_id(&m0->ric_id, &m1->ric_id) == false)
    return false;

  if(eq_byte_array(m0->call_process_id, m1->call_process_id) == false)
    return false;

  if(m0->status != m1->status)
    return false;

  if(eq_byte_array(m0->control_outcome, m1->control_outcome)== false)
    return false;

  return true;
}
