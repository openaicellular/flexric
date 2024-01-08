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

#include "s_nssai.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

bool eq_s_nssai_e2sm(const s_nssai_e2sm_t* m0, const s_nssai_e2sm_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(m0->sST != m1->sST)
    return false;

  if(m0->sD != NULL || m1->sD != NULL){
    if(m0->sD == NULL || m1->sD == NULL)
      return false;
    if(*m0->sD != *m1->sD)
      return false;
  }


  return true;
}

void free_s_nssai_e2sm( s_nssai_e2sm_t* src)
{
  assert(src != NULL);

  if(src->sD != NULL)
    free(src->sD);

}
