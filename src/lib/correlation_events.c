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
#include "correlation_events.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int cmp_correlation_ev(void const* pend_v1, void const* pend_v2)
{
  assert(pend_v1 != NULL);
  assert(pend_v2 != NULL);

  correlation_event_t* ev1 = (correlation_event_t*)pend_v1; 
  correlation_event_t* ev2 = (correlation_event_t*)pend_v2; 

  if(ev1->ric_id.ric_req_id < ev2->ric_id.ric_req_id) 
    return 1;
  if(!memcmp(&ev1->ric_id, &ev2->ric_id, sizeof(ev2->ric_id)))
    return 0;

  return -1;
}

void free_correlation_ev(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);
  
  free(value);
}
