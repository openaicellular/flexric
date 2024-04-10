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
#include "../../src/sm/ccc_sm/ccc_sm_id.h"

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
  dst.nr_cgi.plmn_id = (e2sm_plmn_t) {
    .mcc = (rand()%900) + 100,
    .mnc = (rand()%90) + 10,
    .mnc_digit_len = 2
  };
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
e2sm_ccc_ind_hdr_frmt_1_t fill_rnd_ccc_ind_hdr_frmt(){
  e2sm_ccc_ind_hdr_frmt_1_t dst = {0};

  dst.ind_reason = rand()%END_IND_REASON;

  const size_t sz = strlen("test");
  dst.event_time = calloc(sz + 1, sizeof(char)); // Including null terminal
  assert(dst.event_time != NULL && "Memory exhausted");
  strcpy(dst.event_time, "test");

  return dst;
}

e2sm_ccc_ind_hdr_t fill_rnd_ccc_ind_hdr(void){
  e2sm_ccc_ind_hdr_t dst = {0};

  dst.format = rand()%END_E2SM_CCC_IND_HDR;

  if(dst.format == FORMAT_1_E2SM_CCC_IND_HDR){
    dst.frmt_1 = fill_rnd_ccc_ind_hdr_frmt();
  } else {
    assert(0!=0 && "Not implemented");
  }

  return dst;
}

static
e2sm_ccc_ind_msg_frmt_1_t fill_rnd_ccc_ind_msg_frmt_1(){
  e2sm_ccc_ind_msg_frmt_1_t dst = {0};

  dst.sz_ind_msg_node_conf = rand()%3 + 1;
  dst.ind_msg_ran_conf = calloc(dst.sz_ind_msg_node_conf, sizeof(ind_msg_ran_conf_t));
  assert(dst.ind_msg_ran_conf != NULL && "Memory exhausted");
  for (size_t i = 0; i < dst.sz_ind_msg_node_conf; i++){
    dst.ind_msg_ran_conf[i].ran_conf_name = cp_str_to_ba("ran_conf_name");
    dst.ind_msg_ran_conf[i].change_type = rand()%END_CHANGE_TYPE;
    dst.ind_msg_ran_conf[i].vals_attributes = cp_str_to_ba("vals_attributes");
    dst.ind_msg_ran_conf[i].old_vals_attributes= cp_str_to_ba("old_vals_attributes");
  }

  return dst;
}

static
e2sm_ccc_ind_msg_frmt_2_t fill_rnd_ccc_ind_msg_frmt_2(){
  e2sm_ccc_ind_msg_frmt_2_t dst = {0};

  dst.sz_ind_msg_cell_report= rand()%3 + 1;
  dst.ind_msg_cell_report = calloc(dst.sz_ind_msg_cell_report, sizeof(ind_msg_cell_report_t));
  assert(dst.ind_msg_cell_report != NULL && "Memory exhausted");
  for (size_t i = 0; i < dst.sz_ind_msg_cell_report; i++) {
    dst.ind_msg_cell_report[i].cell_global_id = fill_rnd_cell_global_id();
    dst.ind_msg_cell_report[i].sz_ind_msg_ran_conf = rand()%3 + 1;
    dst.ind_msg_cell_report[i].ind_msg_ran_conf = calloc(dst.ind_msg_cell_report[i].sz_ind_msg_ran_conf, sizeof(ind_msg_ran_conf_t));
    assert(dst.ind_msg_cell_report[i].ind_msg_ran_conf != NULL && "Memory exhausted");
    for (size_t z = 0; z < dst.ind_msg_cell_report[i].sz_ind_msg_ran_conf; z++) {
      dst.ind_msg_cell_report[i].ind_msg_ran_conf[z].change_type = rand()%END_CHANGE_TYPE;
      dst.ind_msg_cell_report[i].ind_msg_ran_conf[z].ran_conf_name = cp_str_to_ba("ran_conf_name");
      dst.ind_msg_cell_report[i].ind_msg_ran_conf[z].vals_attributes = cp_str_to_ba("vals_attributes");
      dst.ind_msg_cell_report[i].ind_msg_ran_conf[z].old_vals_attributes = cp_str_to_ba("old_vals_attributes");
    }
  }

  return dst;
}

e2sm_ccc_ind_msg_t fill_rnd_ccc_ind_msg(void){
  e2sm_ccc_ind_msg_t dst = {0};

  dst.format = rand()%END_E2SM_CCC_IND_MSG;

  if(dst.format == FORMAT_1_E2SM_CCC_IND_MSG){
    dst.frmt_1 = fill_rnd_ccc_ind_msg_frmt_1();
  } else if (dst.format == FORMAT_2_E2SM_CCC_IND_MSG){
    dst.frmt_2 = fill_rnd_ccc_ind_msg_frmt_2();
  } else {
    assert(0!=0 && "Not implemented");
  }

  return dst;
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

ran_function_name_t fill_ccc_ran_func_name(void){
  ran_function_name_t dst = {0};

  // RAN Function Short Name
  // Mandatory
  // PrintableString [1-150]
  dst.name.buf = calloc(strlen(SM_CCC_SHORT_NAME) + 1, sizeof(uint8_t));
  memcpy(dst.name.buf, SM_CCC_SHORT_NAME, strlen(SM_CCC_SHORT_NAME));
  dst.name.len = strlen(SM_CCC_SHORT_NAME);

  // RAN Function Service Model OID
  // Mandatory
  // PrintableString [1-1000]

  //iso(1) identified-organization(3)
  //dod(6) internet(1) private(4)
  //enterprise(1) 53148 e2(1)
  // version1 (1) e2sm(2) e2sm-RC-
  // IEs (3)
  dst.oid.buf = calloc(strlen(SM_CCC_OID) + 1, sizeof(uint8_t));
  memcpy(dst.oid.buf, SM_CCC_OID, strlen(SM_CCC_OID));
  dst.oid.len = strlen(SM_CCC_OID);

  // RAN Function Description
  // Mandatory
  // PrintableString [1- 150]
  //RAN function RC “RAN Control” performs the following
  //functionalities:
  //- Exposure of RAN control and UE context related
  //information.
  //- Modification and initiation of RAN control related call
  //processes and messages
  //- Execution of policies that may result in change of
  //RAN control behavior

  dst.description.buf = calloc(strlen(SM_CCC_DESCRIPTION) + 1, sizeof(uint8_t));
  memcpy(dst.description.buf, SM_CCC_DESCRIPTION, strlen(SM_CCC_DESCRIPTION));
  dst.description.len = strlen(SM_CCC_DESCRIPTION);

  // RAN Function Instance
  // Optional
  // INTEGER
//    long* instance;	/* OPTIONAL: it is suggested to be used when E2 Node declares
//                                multiple RAN Function ID supporting the same  E2SM specification   ask Mikel */
  return dst;
}

/////////////////////////////
/////////////////////////////
////////// End of RAN Function Definition
/////////////////////////////
/////////////////////////////

ccc_ind_data_t fill_rnd_ccc_ind_data(void){
  ccc_ind_data_t dst = {0};

  dst.hdr = fill_rnd_ccc_ind_hdr();
  dst.msg = fill_rnd_ccc_ind_msg();

  return dst;
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
  e2sm_ccc_func_def_t dst = {0};

  dst.name = fill_ccc_ran_func_name();

  return dst;
}