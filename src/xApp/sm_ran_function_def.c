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

#include "sm_ran_function_def.h"
#include <assert.h>

void free_sm_ran_function_def(sm_ran_function_def_t* src)
{
  assert(src != NULL);

  if(src->type == KPM_RAN_FUNC_DEF_E){
    free_kpm_ran_function_def(&src->kpm); 
  } else if(src->type == RC_RAN_FUNC_DEF_E){
    free_e2sm_rc_func_def(&src->rc);
  } else if(src->type == MAC_RAN_FUNC_DEF_E){
    free_mac_func_def(&src->mac); 
  } else if(src->type == RLC_RAN_FUNC_DEF_E){
    free_rlc_func_def(&src->rlc);  
  } else if(src->type == PDCP_RAN_FUNC_DEF_E){
    free_pdcp_func_def(&src->pdcp); 
  } else if(src->type == GTP_RAN_FUNC_DEF_E){
    free_gtp_func_def(&src->gtp);
  } else if(src->type == SLICE_RAN_FUNC_DEF_E){
    free_slice_func_def(&src->slice);
  } else if(src->type == TC_RAN_FUNC_DEF_E){
    free_tc_func_def(&src->tc);
  } else{
    assert(0 != 0 && "Unknown type");
  }

}


sm_ran_function_def_t cp_sm_ran_function_def(sm_ran_function_def_t const* src)
{
  assert(src != NULL);

  sm_ran_function_def_t dst = {0}; 

  if(src->type == KPM_RAN_FUNC_DEF_E){
   dst.kpm = cp_kpm_ran_function_def(&src->kpm); 
  } else if(src->type == RC_RAN_FUNC_DEF_E){
    dst.rc =  cp_e2sm_rc_func_def(&src->rc);
  } else if(src->type == MAC_RAN_FUNC_DEF_E){
    dst.mac = cp_mac_func_def(&src->mac); 
  } else if(src->type == RLC_RAN_FUNC_DEF_E){
   dst.rlc = cp_rlc_func_def(&src->rlc);  
  } else if(src->type == PDCP_RAN_FUNC_DEF_E){
   dst.pdcp = cp_pdcp_func_def(&src->pdcp); 
  } else if(src->type == GTP_RAN_FUNC_DEF_E){
   dst.gtp = cp_gtp_func_def(&src->gtp);
  } else if(src->type == SLICE_RAN_FUNC_DEF_E){
   dst.slice = cp_slice_func_def(&src->slice);
  } else if(src->type == TC_RAN_FUNC_DEF_E){
   dst.tc = cp_tc_func_def(&src->tc);
  } else{
    assert(0 != 0 && "Unknown type");
  }
  return dst;
}

bool eq_sm_ran_function_def(sm_ran_function_def_t const* m0,sm_ran_function_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->type != m1->type)
    return false;

  if(m0->type == KPM_RAN_FUNC_DEF_E){
    return eq_kpm_ran_function_def(&m0->kpm, &m0->kpm); 
  } else if(m0->type == RC_RAN_FUNC_DEF_E){
    return eq_e2sm_rc_func_def(&m0->rc, &m1->rc);
  } else if(m0->type == MAC_RAN_FUNC_DEF_E){
    return eq_mac_func_def(&m0->mac, &m1->mac); 
  } else if(m0->type == RLC_RAN_FUNC_DEF_E){
    return eq_rlc_func_def(&m0->rlc, &m1->rlc);  
  } else if(m0->type == PDCP_RAN_FUNC_DEF_E){
    return eq_pdcp_func_def(&m0->pdcp, &m1->pdcp); 
  } else if(m0->type == GTP_RAN_FUNC_DEF_E){
    return eq_gtp_func_def(&m0->gtp, &m1->gtp);
  } else if(m0->type == SLICE_RAN_FUNC_DEF_E){
    return eq_slice_func_def(&m0->slice, &m1->slice);
  } else if(m0->type == TC_RAN_FUNC_DEF_E){
    return eq_tc_func_def(&m0->tc, &m1->tc);
  } else{
    assert(0 != 0 && "Unknown type");
  }
  return false;
}


