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


#include "ccc_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////



void free_e2sm_ccc_event_trigger(e2sm_ccc_event_trigger_t* src)
{
  assert(src != NULL);

  if(src->format == FORMAT_1_E2SM_CCC_EV_TRIGGER_FORMAT){
    free_e2sm_ccc_ev_trg_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_EV_TRIGGER_FORMAT ){
    free_e2sm_ccc_ev_trg_frmt_2(&src->frmt_2);
  }else if(src->format == FORMAT_3_E2SM_CCC_EV_TRIGGER_FORMAT ){
    free_e2sm_ccc_ev_trg_frmt_3(&src->frmt_3);
  } else {
    assert(0!=0 && "Not implemented");
  }
}

bool eq_e2sm_ccc_event_trigger(e2sm_ccc_event_trigger_t const* m0, e2sm_ccc_event_trigger_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->format == FORMAT_1_E2SM_CCC_EV_TRIGGER_FORMAT){
    eq_e2sm_ccc_ev_trg_frmt_1(&m0->frmt_1, &m1->frmt_1);
  } else if(m0->format == FORMAT_2_E2SM_CCC_EV_TRIGGER_FORMAT ){
    eq_e2sm_ccc_ev_trg_frmt_2(&m0->frmt_2, &m1->frmt_2);
  }else if(m0->format == FORMAT_3_E2SM_CCC_EV_TRIGGER_FORMAT ){
    eq_e2sm_ccc_ev_trg_frmt_3(&m0->frmt_3, &m1->frmt_3);
  } else {
    assert(0!=0 && "Not implemented");
  }

  assert(0!=0 && "Impossible path");
  return false;
}


e2sm_ccc_event_trigger_t cp_e2sm_ccc_event_trigger(e2sm_ccc_event_trigger_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_event_trigger_t dst = {.format = src->format};

  if(src->format == FORMAT_1_E2SM_CCC_EV_TRIGGER_FORMAT){
    cp_e2sm_ccc_ev_trg_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_EV_TRIGGER_FORMAT){
    cp_e2sm_ccc_ev_trg_frmt_2(&src->frmt_2);
  }else if(src->format == FORMAT_3_E2SM_CCC_EV_TRIGGER_FORMAT ){
    cp_e2sm_ccc_ev_trg_frmt_3(&src->frmt_3);
  } else {
    assert(0!=0 && "Not implemented");
  }

  return dst;
}

//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

void free_e2sm_ccc_action_def(e2sm_ccc_action_def_t* src)
{
  assert(src != NULL);

  //  RIC Style Type
  //  Mandatory
  //  9.3.3
  // Defined in common 6.2.2.2.
  // uint32_t ric_style_type;

  if(src->format == FORMAT_1_E2SM_CCC_ACT_DEF ){
    free_e2sm_ccc_act_def_frmt_1(&src->frmt_1);
  } else if(src->format ==  FORMAT_2_E2SM_CCC_ACT_DEF ){
    free_e2sm_ccc_act_def_frmt_2(&src->frmt_2);
  } else {
    assert("Unknown format" );
  }

}

e2sm_ccc_action_def_t cp_e2sm_ccc_action_def(e2sm_ccc_action_def_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_action_def_t dst = {0};
  //  RIC Style Type
  //  Mandatory
  //  9.3.3
  // Defined in common 6.2.2.2.
  dst.ric_style_type = src->ric_style_type; 

  dst.format = src->format;
  if(dst.format == FORMAT_1_E2SM_CCC_ACT_DEF){
    cp_e2sm_ccc_act_def_frmt_1(&src->frmt_1);
  }else if(dst.format == FORMAT_2_E2SM_CCC_ACT_DEF){
    cp_e2sm_ccc_act_def_frmt_2(&src->frmt_2);
  } else {
    assert(0!=0 && "Unknown format");
  }

  return dst;
}


bool eq_e2sm_ccc_action_def(e2sm_ccc_action_def_t* m0,  e2sm_ccc_action_def_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->ric_style_type != m1->ric_style_type)
    return false;

  if(m0->format != m1->format)
    return false;

  if(m0->format == FORMAT_1_E2SM_CCC_ACT_DEF){
    eq_e2sm_ccc_act_def_frmt_1(&m0->frmt_1, &m1->frmt_1);
  } else if(m0->format == FORMAT_2_E2SM_CCC_ACT_DEF){
    eq_e2sm_ccc_act_def_frmt_2(&m0->frmt_2, &m1->frmt_2);
  } else {
    assert( 0!= 0 && "Unknown format type");
  }

  assert( 0!= 0 && "Impossible path");
  return true;
}


//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_e2sm_ccc_ind_hdr(e2sm_ccc_ind_hdr_t* src)
{
  assert(src != NULL);
 
  if(src->format == FORMAT_1_E2SM_CCC_IND_HDR){
    free_e2sm_ccc_ind_hdr_frmt_1(&src->frmt_1);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

e2sm_ccc_ind_hdr_t cp_e2sm_ccc_ind_hdr(e2sm_ccc_ind_hdr_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ind_hdr_t dst = {.format = src->format}; 

  if(src->format == FORMAT_1_E2SM_CCC_IND_HDR){
    cp_e2sm_ccc_ind_hdr_frmt_1(&src->frmt_1);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

bool eq_e2sm_ccc_ind_hdr(e2sm_ccc_ind_hdr_t const* m0, e2sm_ccc_ind_hdr_t const* m1)
{

  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->format != m1->format)
    return false;

  if(m0->format == FORMAT_1_E2SM_CCC_IND_HDR  ){
    eq_e2sm_ccc_ind_hdr_frmt_1(&m0->frmt_1, &m1->frmt_1);
  } else {
    assert(0!=0 && "Unknown format");
  }

  assert(0!=0 && "Not implemented");

  return true;
}


//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////

void free_e2sm_ccc_ind_msg(e2sm_ccc_ind_msg_t* src)
{
  assert(src != NULL);

  if(src->format == FORMAT_1_E2SM_CCC_IND_MSG){
    free_e2sm_ccc_ind_msg_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_IND_MSG){
    free_e2sm_ccc_ind_msg_frmt_2(&src->frmt_2);
  } else{
    assert(0 != 0 && "Unknown format");
  }


}

e2sm_ccc_ind_msg_t cp_e2sm_ccc_ind_msg(e2sm_ccc_ind_msg_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_ind_msg_t dst = {.format = src->format}; 

  if(src->format == FORMAT_1_E2SM_CCC_IND_MSG){
    cp_e2sm_ccc_ind_msg_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_IND_MSG){
    cp_e2sm_ccc_ind_msg_frmt_2(&src->frmt_2);
  } else{
    assert(0 != 0 && "Unknown format");
  }

  return dst;
}

bool eq_e2sm_ccc_ind_msg(e2sm_ccc_ind_msg_t const* m0, e2sm_ccc_ind_msg_t const* m1)
{
  if(m0 == m1) 
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->format != m1->format)
    return false;

  if(m0->format == FORMAT_1_E2SM_CCC_IND_MSG ){
    eq_e2sm_ccc_ind_msg_frmt_1(&m0->frmt_1, &m1->frmt_1);
  } else if(m0->format == FORMAT_2_E2SM_CCC_IND_MSG){
    eq_e2sm_ccc_ind_msg_frmt_2(&m0->frmt_2, &m1->frmt_2);
  } else {
    assert(0 != 0 && "Unknown format");
  }

  assert(0 != 0 && "Impossible path");


  return true;
}

//////////////////////////////////////
// RIC Call Process ID
/////////////////////////////////////

void free_e2sm_ccc_cpid(e2sm_ccc_cpid_t *src) 
{
  assert(src != NULL);
}

e2sm_ccc_cpid_t cp_e2sm_ccc_cpid(e2sm_ccc_cpid_t const* src)
{
  assert(src != NULL); 
 
  e2sm_ccc_cpid_t dst = {};
  return dst;
}

bool eq_e2sm_ccc_cpid(e2sm_ccc_cpid_t const* m0, e2sm_ccc_cpid_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  return true;
}


//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////

void free_e2sm_ccc_ctrl_hdr(e2sm_ccc_ctrl_hdr_t* src)
{
  assert(src != NULL);
  if(src->format == FORMAT_1_E2SM_CCC_CTRL_HDR){
    free_e2sm_ccc_ctrl_hdr_frmt_1(&src->frmt_1);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

e2sm_ccc_ctrl_hdr_t cp_e2sm_ccc_ctrl_hdr(e2sm_ccc_ctrl_hdr_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ctrl_hdr_t dst = {.format = src->format};

  if(dst.format == FORMAT_1_E2SM_CCC_CTRL_HDR){
    cp_e2sm_ccc_ctrl_hdr_frmt_1(&src->frmt_1);
  } else{
    assert(0!=0 && "Unknown format type");
  } 

  return dst;
}

bool eq_e2sm_ccc_ctrl_hdr(e2sm_ccc_ctrl_hdr_t const* m0, e2sm_ccc_ctrl_hdr_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->format != m1->format)
    return false;

  if(m0->format == FORMAT_1_E2SM_CCC_CTRL_HDR){
    eq_e2sm_ccc_ctrl_hdr_frmt_1(&m0->frmt_1, &m1->frmt_1);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return true;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////


void free_e2sm_ccc_ctrl_msg(e2sm_ccc_ctrl_msg_t* src)
{
  assert(src != NULL);

  if(src->format == FORMAT_1_E2SM_CCC_CTRL_MSG){
    free_e2sm_ccc_ctrl_msg_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_CTRL_MSG){
    free_e2sm_ccc_ctrl_msg_frmt_2(&src->frmt_2);
  } else {
    assert(0 && "Unknown format");
  }

}

e2sm_ccc_ctrl_msg_t cp_e2sm_ccc_ctrl_msg(e2sm_ccc_ctrl_msg_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_ctrl_msg_t dst = {.format = src->format};

  if(src->format == FORMAT_1_E2SM_CCC_CTRL_MSG){
    cp_e2sm_ccc_ctrl_msg_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_CTRL_MSG){
    cp_e2sm_ccc_ctrl_msg_frmt_2(&src->frmt_2);
  } else {
    assert(0!=0 && "Unknown format");
  }

  return dst;;
}

bool eq_e2sm_ccc_ctrl_msg(e2sm_ccc_ctrl_msg_t const* m0, e2sm_ccc_ctrl_msg_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->format != m1->format)
    return false;

  if(m0->format == FORMAT_1_E2SM_CCC_CTRL_MSG){
    eq_e2sm_ccc_ctrl_msg_frmt_1(&m0->frmt_1, &m1->frmt_1);
  } else if(m0->format == FORMAT_2_E2SM_CCC_CTRL_MSG){
    eq_e2sm_ccc_ctrl_msg_frmt_2(&m0->frmt_2, &m1->frmt_2);
  } else {
    assert(0 && "Unknown format");
  }

  return true;
}


//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

void free_e2sm_ccc_ctrl_out(e2sm_ccc_ctrl_out_t* src)
{
  assert(src != NULL);

  if(src->format == FORMAT_1_E2SM_CCC_CTRL_OUT){
    free_e2sm_ccc_ctrl_out_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_CTRL_OUT){
    free_e2sm_ccc_ctrl_out_frmt_2(&src->frmt_2);
  } else {
    assert(0!=0 && "Unknown format");
  }

}

e2sm_ccc_ctrl_out_t cp_e2sm_ccc_ctrl_out(e2sm_ccc_ctrl_out_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_ctrl_out_t dst = {.format = src->format}; 
  
  if(dst.format == FORMAT_1_E2SM_CCC_CTRL_OUT ){
    cp_e2sm_ccc_ctrl_out_frmt_1(&src->frmt_1);
  } else if(src->format == FORMAT_2_E2SM_CCC_CTRL_OUT){
    cp_e2sm_ccc_ctrl_out_frmt_2(&src->frmt_2);
  }  else{
    assert(0!=0 && "Unknown format");
  }

  return dst;
}

bool eq_e2sm_ccc_ctrl_out(e2sm_ccc_ctrl_out_t const* m0, e2sm_ccc_ctrl_out_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->format != m1->format)
    return false;

  if(m0->format == FORMAT_1_E2SM_CCC_CTRL_OUT){
    eq_e2sm_ccc_ctrl_out_frmt_1(&m0->frmt_1, &m1->frmt_1);
  } else if(m0->format == FORMAT_2_E2SM_CCC_CTRL_OUT){
    eq_e2sm_ccc_ctrl_out_frmt_2(&m0->frmt_2, &m1->frmt_2);
  } else {
    assert(0!=0 && "Unknown format");
  }

  return true;
}


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

void free_e2sm_ccc_func_def(e2sm_ccc_func_def_t* src)
{
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_func_def_t cp_e2sm_ccc_func_def(e2sm_ccc_func_def_t const* src)
{
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");

  e2sm_ccc_func_def_t dst = {0};

  return dst;
}

bool eq_e2sm_ccc_func_def(e2sm_ccc_func_def_t const* m0, e2sm_ccc_func_def_t const* m1)
{
  assert(0 != 0 && "Not implemented");

  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  return true;
}

///////////////
/// RIC Subscription
///////////////

void free_ccc_sub_data(ccc_sub_data_t* src)
{
  assert(src != NULL);

  free_e2sm_ccc_event_trigger(&src->et);

  // [1-16]
  assert(src->sz_ad > 0 && src->sz_ad < 17);
  for(size_t i = 0; i < src->sz_ad; ++i){
    free_e2sm_ccc_action_def(&src->ad[i]);
  }
  free(src->ad);
}

bool eq_ccc_sub_data(ccc_sub_data_t const* m0, ccc_sub_data_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_e2sm_ccc_event_trigger(&m0->et, &m1->et) == false)
    return false;

  assert(m0->sz_ad > 0 && m0->sz_ad < 17);
  assert(m1->sz_ad > 0 && m1->sz_ad < 17);
  if(m0->sz_ad != m1->sz_ad)
    return false;

  for(size_t i = 0; i < m0->sz_ad; ++i){
    if(eq_e2sm_ccc_action_def(&m0->ad[i], &m1->ad[i]) == false){
      assert(0 != 0 && "debug");
      return false;
    }
  }

  return true;
}

ccc_sub_data_t cp_ccc_sub_data(ccc_sub_data_t const* src)
{
  assert(src != NULL);
  ccc_sub_data_t dst = {0}; 

  dst.et = cp_e2sm_ccc_event_trigger(&src->et);

  assert(src->sz_ad > 0 && src->sz_ad < 17);
  assert(src->sz_ad == 1 && "Only one supported");
  dst.sz_ad = src->sz_ad;

  dst.ad = calloc(dst.sz_ad, sizeof(e2sm_ccc_action_def_t));
  assert(dst.ad != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_ad; ++i){
    dst.ad[i] = cp_e2sm_ccc_action_def(src->ad); 
  }

  return dst;
}


///////////////
// RIC Indication
///////////////

void free_ccc_ind_data(ccc_ind_data_t* ind)
{
  assert(ind != NULL);
  
  free_e2sm_ccc_ind_hdr(&ind->hdr);
  free_e2sm_ccc_ind_msg(&ind->msg);
}

bool eq_ccc_ind_data(ccc_ind_data_t const* m0, ccc_ind_data_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_e2sm_ccc_ind_hdr(&m0->hdr, &m1->hdr) == false){
    assert(0!=0 && "Debug");
    return false;
  }

  if(eq_e2sm_ccc_ind_msg(&m0->msg, &m1->msg) == false){
    assert(0!=0 && "Debug");
    return false;
  }

  return true;
}


ccc_ind_data_t cp_ccc_ind_data(ccc_ind_data_t const* src)
{
  assert(src != NULL);
  ccc_ind_data_t dst = {0};

  dst.hdr = cp_e2sm_ccc_ind_hdr(&src->hdr);
  dst.msg = cp_e2sm_ccc_ind_msg(&src->msg);

  return dst;
}


///////////////
// RIC Control
///////////////

void free_ccc_ctrl_req_data(ccc_ctrl_req_data_t* src)
{
  assert(src != NULL);

  free_e2sm_ccc_ctrl_hdr(&src->hdr);
  free_e2sm_ccc_ctrl_msg(&src->msg);
}

bool eq_ccc_ctrl_req_data(ccc_ctrl_req_data_t const* m0, ccc_ctrl_req_data_t  const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_e2sm_ccc_ctrl_hdr(&m0->hdr, &m1->hdr) == false)
    return false;

  if(eq_e2sm_ccc_ctrl_msg(&m0->msg, &m1->msg ) == false)
    return false;

  return true;
}

ccc_ctrl_req_data_t cp_ccc_ctrl_req_data(ccc_ctrl_req_data_t const* src)
{
  assert(src != NULL);
  ccc_ctrl_req_data_t dst = {0};

  dst.hdr = cp_e2sm_ccc_ctrl_hdr(&src->hdr);
  dst.msg = cp_e2sm_ccc_ctrl_msg(&src->msg);

  assert( eq_ccc_ctrl_req_data(src, &dst) == true);
  return dst;
}

