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
e2sm_plmn_t fill_rnd_plmn_id(){
  e2sm_plmn_t dst = (e2sm_plmn_t) {
      .mcc = (rand()%900) + 100,
      .mnc = (rand()%90) + 10,
      .mnc_digit_len = 2
  };

  return dst;
}

static
s_nssai_e2sm_t fill_rnd_s_nssai_e2sm(){
  s_nssai_e2sm_t dst = {0};

  dst.sST = (rand() % (1 << 8)) + 1;
  dst.sD = calloc(1, sizeof(uint32_t));
  *dst.sD = 786432; //  (rand() % 2^24) + 0;

  return dst;
}


static
e2sm_ccc_plmn_info_t fill_rnd_e2sm_ccc_plmn_info(){
  e2sm_ccc_plmn_info_t dst = {0};
  dst.plmn_id = fill_rnd_plmn_id();
  if (rand()%2 == 1){
    dst.s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
    assert(dst.s_nssai != NULL);
    *dst.s_nssai = fill_rnd_s_nssai_e2sm();
  }

  return dst;
}

e2sm_ccc_5qi_lst_t fill_rnd_e2sm_ccc_5qi_lst(){
  e2sm_ccc_5qi_lst_t dst = {0};
  // [1..128]
  dst.sz_lst_5qi = (rand()%3) + 1;
  dst.lst_5qi = calloc(dst.sz_lst_5qi, sizeof(uint32_t));
  assert(dst.lst_5qi != NULL);
  for (size_t i = 0; i < dst.sz_lst_5qi; i++) {
    dst.lst_5qi[i] = rand()%1024;
  }

  return dst;
}

e2sm_ccc_partition_flow_lst_t fill_rnd_e2sm_ccc_partition_flow_lst(){
  e2sm_ccc_partition_flow_lst_t dst = {0};
  dst.sz_partition_flow_lst_item = (rand()%3) + 1;
  dst.partition_flow_lst_item = calloc(dst.sz_partition_flow_lst_item, sizeof(e2sm_ccc_partition_flow_lst_item_t));
  for (size_t i = 0; i < dst.sz_partition_flow_lst_item; i++){
    dst.partition_flow_lst_item[i].plmn_id = fill_rnd_plmn_id();
    dst.partition_flow_lst_item[i].s_nssai = fill_rnd_s_nssai_e2sm();
    // Optional
    if (rand()%2 == 1){
      dst.partition_flow_lst_item[i].lst_5qi = calloc(1, sizeof(e2sm_ccc_5qi_lst_t));
      assert(dst.partition_flow_lst_item[i].lst_5qi != NULL);
      *dst.partition_flow_lst_item[i].lst_5qi = fill_rnd_e2sm_ccc_5qi_lst();
    }
  }
  return dst;
}

static
cell_global_id_t fill_rnd_cell_global_id()
{
  cell_global_id_t dst = {0};

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_CGI_RAT_TYPE;
  dst.nr_cgi.plmn_id = fill_rnd_plmn_id();
  dst.nr_cgi.nr_cell_id = rand()% (1UL << 36);

  return dst;
}

static
ev_trg_ran_conf_t fill_ev_trg_ran_conf(){
  ev_trg_ran_conf_t res = {0};

  res.sz_attribute = rand()%3;
  if (res.sz_attribute > 0){
    res.attribute = calloc(res.sz_attribute, sizeof(attribute_t));
    assert(res.attribute != NULL && "Memory exhausted");
    for (size_t i = 0; i < res.sz_attribute; ++i){
      res.attribute[i].attribute_name = cp_str_to_ba("attribute_name");
    }
  }

  res.ran_conf_name = cp_str_to_ba("test");

  return res;
}

static
e2sm_ccc_ev_trg_frmt_1_t fill_rnd_ccc_ev_trg_frmt_1(){
  e2sm_ccc_ev_trg_frmt_1_t res = {0};

  res.sz_ev_trg_ran_conf = (rand()%7) + 1;
  res.ev_trg_ran_conf = calloc(res.sz_ev_trg_ran_conf, sizeof(ev_trg_ran_conf_t));
  assert(res.ev_trg_ran_conf != NULL && "Memory exhausted");
  for (size_t i = 0; i < res.sz_ev_trg_ran_conf; ++i){
    res.ev_trg_ran_conf[i] = fill_ev_trg_ran_conf();
  }

  return res;
};

static
e2sm_ccc_ev_trg_frmt_2_t fill_rnd_ccc_ev_trg_frmt_2(){
  e2sm_ccc_ev_trg_frmt_2_t res = {0};

  res.sz_ev_trg_cell = rand()%3 + 1;
  res.ev_trg_cell = calloc(res.sz_ev_trg_cell, sizeof(ev_trg_cell_t));
  assert(res.ev_trg_cell!= NULL && "Memory exhausted");
  for (size_t i = 0; i < res.sz_ev_trg_cell; ++i){
    res.ev_trg_cell[i].cell_global_id = fill_rnd_cell_global_id();
    res.ev_trg_cell[i].sz_ev_trg_ran_conf = (rand()%8) + 1;
    res.ev_trg_cell[i].ev_trg_ran_conf = calloc(res.ev_trg_cell[i].sz_ev_trg_ran_conf, sizeof(ev_trg_ran_conf_t));
    for (size_t z = 0; z < res.ev_trg_cell[i].sz_ev_trg_ran_conf; ++z){
      res.ev_trg_cell[i].ev_trg_ran_conf[z] = fill_ev_trg_ran_conf();
    }
  }

  return res;
};

e2sm_ccc_event_trigger_t fill_rnd_ccc_event_trigger(void){
  e2sm_ccc_event_trigger_t res = {0};

  res.format = rand()%END_E2SM_CCC_EV_TRIGGER_FORMAT;
  if (res.format == FORMAT_1_E2SM_CCC_EV_TRIGGER_FORMAT){
    res.frmt_1 = fill_rnd_ccc_ev_trg_frmt_1();
  } else if (res.format == FORMAT_2_E2SM_CCC_EV_TRIGGER_FORMAT){
    res.frmt_2 = fill_rnd_ccc_ev_trg_frmt_2();
  } else if (res.format == FORMAT_3_E2SM_CCC_EV_TRIGGER_FORMAT){
    res.frmt_3.period = (rand()%1024) + 1;
  } else {
    assert(0!=0 && "Not implemented");
  }

  return res;
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
e2sm_ccc_o_gnb_du_function_t fill_rnd_du_function_node(){
  e2sm_ccc_o_gnb_du_function_t dst = {0};

  dst.gnb_id_len = rand()%(1 << 8);
  dst.gnb_id = rand()%4294967295;
  dst.gnb_du_id = rand()%(1UL << 36);
  dst.gnb_du_name = strdup("gnb_du_name");

  return dst;
}

g_enb_id_lst_t fill_rnd_g_enb_id_lst(){
  g_enb_id_lst_t dst = {0};

  dst.sz_g_enb_id_lst = rand()%3;
  if (dst.sz_g_enb_id_lst > 0){
    dst.g_enb_id_lst = calloc(dst.sz_g_enb_id_lst, sizeof(g_enb_id_lst_t));
    assert(dst.g_enb_id_lst != NULL);
    for (size_t i = 0; i < dst.sz_g_enb_id_lst; i++){
      dst.g_enb_id_lst[i] = strdup("g_enb_id_lst");
    }
  }

  return dst;
}

g_gnb_id_lst_t fill_rnd_g_gnb_id_lst(){
  g_gnb_id_lst_t dst = {0};

  dst.sz_g_gnb_id_lst= rand()%3;
  if (dst.sz_g_gnb_id_lst> 0){
    dst.g_gnb_id_lst = calloc(dst.sz_g_gnb_id_lst, sizeof(g_gnb_id_lst_t));
    assert(dst.g_gnb_id_lst!= NULL);
    for (size_t i = 0; i < dst.sz_g_gnb_id_lst; i++){
      dst.g_gnb_id_lst[i] = strdup("g_gnb_id_lst");
    }
  }

  return dst;
}

static
e2sm_ccc_o_gnb_cu_cp_function_t fill_rnd_cu_cp_function_node(){
  e2sm_ccc_o_gnb_cu_cp_function_t dst = {0};

  dst.gnb_id_len = rand()%(1 << 8);
  dst.gnb_id = rand()%4294967295;
  dst.gnb_cu_name = strdup("gnb_cu_name");
  dst.plmn_id = fill_rnd_plmn_id();
  dst.xn_block_list = fill_rnd_g_gnb_id_lst();
  dst.x2_block_list = fill_rnd_g_enb_id_lst();
  dst.x2_allow_list = fill_rnd_g_enb_id_lst();
  dst.x2_ho_block_list = fill_rnd_g_enb_id_lst();
  dst.xn_allow_list = fill_rnd_g_gnb_id_lst();
  dst.xn_ho_block_list = fill_rnd_g_gnb_id_lst();

  return dst;
}

static
e2sm_ccc_o_gnb_cu_up_function_t fill_rnd_cu_up_function_node(){
  e2sm_ccc_o_gnb_cu_up_function_t dst = {0};

  dst.gnb_id_len = rand()%(1 << 8);
  dst.gnb_id = rand()%4294967295;
  dst.gnb_cu_up_id = rand()%(1UL << 36);

  dst.sz_plmn_info_lst = (rand()%3) + 1;
  dst.plmn_info_lst = calloc(dst.sz_plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));
  assert(dst.plmn_info_lst != NULL);
  for (size_t i = 0; i < dst.sz_plmn_info_lst; i++){
    dst.plmn_info_lst[i].plmn_id = fill_rnd_plmn_id();
    if (rand()%2 == 1){
      dst.plmn_info_lst[i].s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
      assert(dst.plmn_info_lst[i].s_nssai != NULL);
      *dst.plmn_info_lst[i].s_nssai = fill_rnd_s_nssai_e2sm();
    }
  }
  return dst;
}

static
e2sm_ccc_o_rrm_policy_ratio_t fill_rnd_o_rrm_policy_ratio(){
  e2sm_ccc_o_rrm_policy_ratio_t dst = {0};

  dst.rrm_policy_min_ratio = rand()%(1 << 8);
  dst.rrm_policy_max_ratio = rand()%(1 << 8);
  dst.rrm_policy_dedicated_ratio = rand()%(1 << 8);
  dst.resource_type = rand()%END_RESOURCE_TYPE;

  dst.sz_rrm_policy_member_lst = (rand()%3) + 1;
  dst.rrm_policy_member_lst = calloc(dst.sz_rrm_policy_member_lst, sizeof(e2sm_ccc_rrm_policy_member_t));
  assert(dst.rrm_policy_member_lst != NULL);
  for (size_t i = 0; i < dst.sz_rrm_policy_member_lst; i++){
    dst.rrm_policy_member_lst[i].plmn_id = fill_rnd_plmn_id();
    if (rand()%2 == 1){
      dst.rrm_policy_member_lst[i].s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
      assert(dst.rrm_policy_member_lst[i].s_nssai != NULL);
      *dst.rrm_policy_member_lst[i].s_nssai = fill_rnd_s_nssai_e2sm();
    }
  }

  return dst;
}

static
e2sm_ccc_o_bwp_t fill_rnd_e2sm_ccc_o_bwp() {
  e2sm_ccc_o_bwp_t dst;

  // Enum fields (assuming appropriate range functions exist)
  dst.bwp_context = rand()%END_BWP_CONTEXT;
  dst.is_initial_bwp = rand()%END_IS_INITIAL_BWP;
  dst.sub_carrier_spacing = rand()%SUB_CARRIER_SPACING_END;
  dst.cyclic_prefix = rand()%CYCLIC_PREFIX_END;

  // Integer fields
  dst.start_rb = rand() % 1024;
  dst.number_of_rbs = rand() % 1024;

  return dst;
}

static
e2sm_ccc_partition_lst_t fill_rnd_e2sm_ccc_partition_lst(){
  e2sm_ccc_partition_lst_t dst = {0};

  // [1..128]
  dst.sz_partition_lst_item = (rand()%3) + 1;
  dst.partition_lst_item = calloc(dst.sz_partition_lst_item, sizeof(e2sm_ccc_partition_lst_item_t));
  for (size_t i = 0; i < dst.sz_partition_lst_item; i++) {
    dst.partition_lst_item[i].p_offset_to_point_A = rand()%1024;
    dst.partition_lst_item[i].p_number_of_rbs = rand()%1024;
    dst.partition_lst_item[i].partition_flow_lst = fill_rnd_e2sm_ccc_partition_flow_lst() ;
  }

  return dst;
}

static
e2sm_ccc_o_nr_cell_du_t fill_rnd_e2sm_ccc_o_nr_cell_du() {
  e2sm_ccc_o_nr_cell_du_t dst;

  // Integer fields
  dst.cell_local_id = rand() % 1024;
  dst.n_rpci = rand() % 504;  // 0 to 503
  dst.n_rtac = rand() % 0x1000000;  // 0 to 16777215
  dst.arfcn_dl = rand() % 1024;
  dst.arfcn_ul = rand() % 1024;
  dst.arfcn_sul = rand() % 1024;
  dst.ssb_frequency = rand() % 0x3279166;  // 0 to 3279165
  dst.bS_Channel_BwDL = rand() % 1024;
  dst.bS_Channel_BwUL = rand() % 1024;
  dst.bS_Channel_BwSUL = rand() % 1024;

  // Enum fields
  dst.operational_state = END_OPERATIONAL_STATE;
  dst.administrative_state = END_ADMINISTRATIVE_STATE;
  dst.cell_state = END_CELL_STATE;
  dst.ssb_periodicity = SSB_PERIODICITY_END;
  dst.ssb_sub_carrier_spacing = SSB_SUB_CARRIER_SPACING_END;
  dst.ssb_duration = SSB_DURATION_END;

  // Byte field
  dst.ssb_off_set = rand() % 160;  // 0 to 159
  // [1..65536]
  dst.sz_plmn_info_lst = (rand()%3) + 1;
  dst.plmn_info_lst = calloc(dst.sz_plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));
  assert(dst.plmn_info_lst != NULL);
  for (size_t i = 0; i < dst.sz_plmn_info_lst; i++) {
    dst.plmn_info_lst[i] = fill_rnd_e2sm_ccc_plmn_info();
  }
  // [1..256]
  dst.sz_bwp_lst = (rand()%3) + 1;
  dst.bwp_lst = calloc(dst.sz_bwp_lst, sizeof(e2sm_ccc_o_bwp_t));
  assert(dst.bwp_lst != NULL);
  for (size_t i = 0; i < dst.sz_bwp_lst; i++) {
    dst.bwp_lst[i] = fill_rnd_e2sm_ccc_o_bwp();
  }

  dst.partition_lst = fill_rnd_e2sm_ccc_partition_lst();

  return dst;
}

static
e2sm_ccc_o_nr_cell_cu_t fill_rnd_e2sm_ccc_o_nr_cell_cu() {
  e2sm_ccc_o_nr_cell_cu_t dst;

  // Integer fields
  dst.cell_local_id = rand() % 1024;

  // [1..65536]
  dst.sz_plmn_info_lst = (rand()%3) + 1;
  dst.plmn_info_lst = calloc(dst.sz_plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));
  assert(dst.plmn_info_lst != NULL);
  for (size_t i = 0; i < dst.sz_plmn_info_lst; i++) {
    dst.plmn_info_lst[i] = fill_rnd_e2sm_ccc_plmn_info();
  }

  return dst;
}

static
e2sm_ccc_o_ces_management_function_t fill_rnd_o_ces_man_func_cell(){
  e2sm_ccc_o_ces_management_function_t dst = {0};
  dst.ces_switch = rand()%END_CES_SWITCH;
  dst.energy_saving_state = rand()%END_ENERGY_SAVING_STATE;
  dst.energy_saving_control = rand()%END_ENERGY_SAVING_CONTROL;

  return dst;
}

ind_msg_ran_conf_t fill_rnd_ind_msg_ran_conf(){
 ind_msg_ran_conf_t res = {0};
 values_of_attributes_e values_of_attributes_type = rand()%VALUES_OF_ATTRIBUTES_END;

  switch (values_of_attributes_type) {
    case VALUES_OF_ATTRIBUTES_O_GNBDUFunction:
      res.ran_conf_name = cp_str_to_ba("O-GNBDUFunction");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_GNBDUFunction;
      res.vals_attributes.e2sm_ccc_o_gnb_du_function = fill_rnd_du_function_node();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_gnb_du_function = fill_rnd_du_function_node();
      }
      break;
    case VALUES_OF_ATTRIBUTES_O_GNBCUCPFunction:
      res.ran_conf_name = cp_str_to_ba("O-GNBCUCPFunction");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_GNBCUCPFunction;
      res.vals_attributes.e2sm_ccc_o_gnb_cu_cp_function= fill_rnd_cu_cp_function_node();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_gnb_cu_cp_function= fill_rnd_cu_cp_function_node();
      }
      break;
    case VALUES_OF_ATTRIBUTES_O_GNBCUUPFunction:
      res.ran_conf_name = cp_str_to_ba("O-GNBCUUPFunction");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_GNBCUUPFunction;
      res.vals_attributes.e2sm_ccc_o_gnb_cu_up_function= fill_rnd_cu_up_function_node();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_gnb_cu_up_function = fill_rnd_cu_up_function_node();
      }
      break;
    case VALUES_OF_ATTRIBUTES_O_RRMPolicyRatio:
      res.ran_conf_name = cp_str_to_ba("O-RRMPolicyRatio");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_RRMPolicyRatio;
      res.vals_attributes.e2sm_ccc_o_rrm_policy_ratio = fill_rnd_o_rrm_policy_ratio();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_rrm_policy_ratio = fill_rnd_o_rrm_policy_ratio();
      }
      break;
    case VALUES_OF_ATTRIBUTES_O_CESManagementFunction:
      res.ran_conf_name = cp_str_to_ba("O-CESManagementFunction");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_CESManagementFunction;
      res.vals_attributes.e2sm_ccc_o_ces_management_function = fill_rnd_o_ces_man_func_cell();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_ces_management_function = fill_rnd_o_ces_man_func_cell();
      }
      break;
    case VALUES_OF_ATTRIBUTES_O_BWP:
      res.ran_conf_name = cp_str_to_ba("O-BWP");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_BWP;
      res.vals_attributes.e2sm_ccc_o_bwp = fill_rnd_e2sm_ccc_o_bwp();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_bwp = fill_rnd_e2sm_ccc_o_bwp();
      }
      break;
    case VALUES_OF_ATTRIBUTES_O_NRCellDU:
      res.ran_conf_name = cp_str_to_ba("O-NRCellDU");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_NRCellDU;
      res.vals_attributes.e2sm_ccc_o_nr_cell_du = fill_rnd_e2sm_ccc_o_nr_cell_du();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_nr_cell_du = fill_rnd_e2sm_ccc_o_nr_cell_du();
      }
      break;
    case VALUES_OF_ATTRIBUTES_O_NRCellCU:
      res.ran_conf_name = cp_str_to_ba("O-NRCellCU");
      res.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_NRCellCU;
      res.vals_attributes.e2sm_ccc_o_nr_cell_cu = fill_rnd_e2sm_ccc_o_nr_cell_cu();
      if(rand()%2 == 1){
        res.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
        assert(res.old_vals_attributes != NULL);
        res.old_vals_attributes->values_of_attributes_type = res.vals_attributes.values_of_attributes_type;
        res.old_vals_attributes->e2sm_ccc_o_nr_cell_cu = fill_rnd_e2sm_ccc_o_nr_cell_cu();
      }
      break;
    default:
      break;
  }

  res.change_type = rand()%END_CHANGE_TYPE;


 return res;
}

static
e2sm_ccc_ind_msg_frmt_1_t fill_rnd_ccc_ind_msg_frmt_1(){
  e2sm_ccc_ind_msg_frmt_1_t dst = {0};

  dst.sz_ind_msg_node_conf = rand()%3 + 1;
  dst.ind_msg_ran_conf = calloc(dst.sz_ind_msg_node_conf, sizeof(ind_msg_ran_conf_t));
  assert(dst.ind_msg_ran_conf != NULL && "Memory exhausted");
  for (size_t i = 0; i < dst.sz_ind_msg_node_conf; i++){
    dst.ind_msg_ran_conf[i] = fill_rnd_ind_msg_ran_conf();
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
      dst.ind_msg_cell_report[i].ind_msg_ran_conf[z] = fill_rnd_ind_msg_ran_conf();
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
  ccc_sub_data_t dst = {0};

  dst.et = fill_rnd_ccc_event_trigger();

  // Action Definition
  dst.sz_ad = 1;
  dst.ad = calloc( dst.sz_ad, sizeof(e2sm_ccc_action_def_t));
  assert(dst.ad != NULL && "Memory exhausted");
  for(size_t i = 0; i < dst.sz_ad; i++){
    dst.ad[i] = fill_rnd_ccc_action_def();
  }

  return dst;
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