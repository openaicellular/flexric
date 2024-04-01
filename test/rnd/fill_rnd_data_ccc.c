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

#include "fill_rnd_data_ccc.h"

#include <assert.h>
#include <math.h>
#include <limits.h>

static
cell_global_id_t fill_rnd_cell_global_id()
{
  cell_global_id_t dst = {0};

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_CGI_RAT_TYPE;
  dst.nr_cgi.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  dst.nr_cgi.nr_cell_id = rand()% (1UL << 36);

  return dst;
}

e2sm_ccc_event_trigger_t fill_rnd_ccc_event_trigger(void){
  assert(0 != 0 && "Not implemented");
}

static
act_def_ran_conf_t fill_act_def_ran_conf(){
  act_def_ran_conf_t res = {0};

  res.report_type = rand()%END_REPORT_TYPE;
  res.ran_conf_name = cp_str_to_ba("test");

  res.sz_attribute = 0;
  return res;
}

static
act_def_cell_report_t fill_act_def_cell_report(){
  act_def_cell_report_t dst = {0};

  dst.cell_global_id = fill_rnd_cell_global_id();
  dst.sz_act_def_ran_conf = (rand() % 8)+1;
  dst.act_def_ran_conf = calloc(dst.sz_act_def_ran_conf, sizeof(act_def_ran_conf_t));
  assert(dst.act_def_ran_conf != NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_act_def_ran_conf; ++i){
    dst.act_def_ran_conf[i] = fill_act_def_ran_conf();
  }

  return dst;
}

static
e2sm_ccc_act_def_frmt_2_t fill_rnd_ccc_act_def_frmt_2(){
  e2sm_ccc_act_def_frmt_2_t dst = {0};

  dst.sz_act_def_cell_report = (rand() % 8)+1;

  dst.act_def_cell_report = calloc(dst.sz_act_def_cell_report, sizeof(act_def_ran_conf_t));
  assert(dst.act_def_cell_report != NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_act_def_cell_report; ++i){
    dst.act_def_cell_report[i] = fill_act_def_cell_report();
  }

  return dst;
}

static
e2sm_ccc_act_def_frmt_1_t fill_rnd_ccc_act_def_frmt_1(){
  e2sm_ccc_act_def_frmt_1_t dst = {0};

  dst.sz_act_def_ran_conf = (rand() % 3)+1;

  dst.act_def_ran_conf = calloc(dst.sz_act_def_ran_conf, sizeof(act_def_ran_conf_t));
  assert(dst.act_def_ran_conf!= NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_act_def_ran_conf; ++i){
    dst.act_def_ran_conf[i] = fill_act_def_ran_conf();
  }

  return dst;
}

e2sm_ccc_action_def_t fill_rnd_ccc_action_def(void){
  e2sm_ccc_action_def_t dst = {0};

  dst.ric_style_type = (rand()%1024) + 1;
  dst.format = rand()%END_E2SM_CCC_ACT_DEF;

  if(dst.format == FORMAT_1_E2SM_CCC_ACT_DEF){
    dst.frmt_1 = fill_rnd_ccc_act_def_frmt_1();
  } else if (dst.format == FORMAT_2_E2SM_CCC_ACT_DEF) {
    dst.frmt_2 = fill_rnd_ccc_act_def_frmt_2();
  } else {
    assert(0!=0 && "Not implemented");
  }

  return dst;
}

static
e2sm_ccc_ind_hdr_frmt_1_t fill_rnd_ccc_ctrl_hdr_frmt_1(){
  e2sm_ccc_ind_hdr_frmt_1_t dst = {0};

  dst.ind_reason = rand()%END_IND_REASON;
  dst.event_time = cp_str_to_ba("Test");

  return dst;
}

e2sm_ccc_ind_hdr_t fill_rnd_ccc_ind_hdr(void){
  e2sm_ccc_ind_hdr_t dst = {0};

  dst.format = rand()%END_E2SM_CCC_IND_HDR;

  if(dst.format == FORMAT_1_E2SM_CCC_IND_HDR){
    dst.frmt_1 = fill_rnd_ccc_ctrl_hdr_frmt_1();
  } else {
    assert(0!=0 && "Not implemented");
  }

  return dst;
}

e2sm_ccc_ind_msg_t fill_rnd_ccc_ind_msg(void){
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_cpid_t fill_rnd_ccc_cpid(void){
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_ctrl_hdr_t fill_rnd_ccc_ctrl_hdr(void){
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_ctrl_msg_t fill_rnd_ccc_ctrl_msg(void){
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_ctrl_out_t fill_rnd_ccc_ctrl_out(void){
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_func_def_t fill_rnd_ccc_ran_func_def(void){
  assert(0 != 0 && "Not implemented");
}

ran_function_name_t fill_ccc_ran_func_name(void){
  assert(0 != 0 && "Not implemented");
}

ccc_ind_data_t fill_rnd_ccc_ind_data(void){
  assert(0 != 0 && "Not implemented");
}

ccc_sub_data_t fill_rnd_ccc_subscription(void){
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_ctrl_out_t fill_ccc_ctrl_out(void){
  assert(0 != 0 && "Not implemented");
}

ccc_ctrl_req_data_t fill_ccc_ctrl(void){
  assert(0 != 0 && "Not implemented");
}

e2sm_ccc_func_def_t fill_ccc_ran_func_def(void){
  assert(0 != 0 && "Not implemented");
}