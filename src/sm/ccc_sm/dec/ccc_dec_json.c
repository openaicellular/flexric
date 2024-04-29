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

#include "ccc_dec_json.h"
#include "../ie/json/ric_indication_header_json.h"
#include "../ie/json/ric_action_definition_json.h"
#include "../ie/json/ric_indication_message_json.h"
#include "../ie/json/ric_control_message_json.h"
#include "../ie/json/ric_function_definition_json.h"
#include "../ie/json/ric_control_outcome_json.h"
#include "../ie/json/ric_event_trigger_definition_json.h"
#include "../ie/json/ric_control_header_json.h"
#include "../../../util/alg_ds/alg/defer.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

static
e2sm_plmn_t get_plmn_id_json(plmn_id_json_t const* src){
  e2sm_plmn_t dst = {0};

  dst.mnc = atoi(src->mnc);
  dst.mcc = atoi(src->mcc);
  dst.mnc_digit_len = strlen(src->mnc);

  return dst;
}

static
s_nssai_e2sm_t get_s_nssai_e2sm(snssai_json_t const* src){
  assert(src != NULL);
  s_nssai_e2sm_t dst = {0};

  if (src->sd){
    dst.sD = calloc(1, sizeof(uint32_t));
    assert(dst.sD != NULL);
    *dst.sD = (uint32_t)atoi(src->sd);
  }

  if (src->sst)
    dst.sST = *src->sst;

  return dst;
}

static
cell_global_id_t get_cell_global_id(cell_global_id_json_t const* src){
  assert(src != NULL);

  cell_global_id_t res = {0};
  res.type = NR_CGI_RAT_TYPE;
  res.nr_cgi.nr_cell_id = atoi(src->n_r_cell_identity);
  res.nr_cgi.plmn_id = get_plmn_id_json(src->plmn_identity);
  return res;
}

static
e2sm_ccc_plmn_info_t get_e2sm_ccc_plmn_info(plmn_info_list_element_t const* src){
  assert(src != NULL);

  e2sm_ccc_plmn_info_t dst = {0};

  dst.plmn_id = get_plmn_id_json(src->plmn_id);
  if (src->snssai != NULL){
    dst.s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
    assert(dst.s_nssai != NULL);
    *dst.s_nssai = get_s_nssai_e2sm(src->snssai);
  }

  return dst;
}

static
e2sm_ccc_rrm_policy_member_t get_e2sm_ccc_rrm_policy_member(r_rm_policy_member_list_element_t const* src){
  assert(src != NULL);

  e2sm_ccc_rrm_policy_member_t dst = {0};

  dst.plmn_id = get_plmn_id_json(src->plmn_id);
  if (src->snssai != NULL){
    dst.s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
    assert(dst.s_nssai != NULL);
    *dst.s_nssai = get_s_nssai_e2sm(src->snssai);
  }

  return dst;
}

static
attribute_t* get_lst_attribute(list_t* const src){
  assert(src != NULL);
  attribute_t* res = calloc(src->count, sizeof(attribute_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  char* attribute_name = list_get_head(src);
  while (attribute_name != NULL){
    res[index].attribute_name = cp_str_to_ba(attribute_name);
    attribute_name = list_get_next(src);
    index++;
  }

  return res;
}

static
ev_trg_ran_conf_t* get_ev_trg_ran_conf(list_t* const src){
  size_t index = 0;

  ev_trg_ran_conf_t* res = calloc(src->count, sizeof(ev_trg_ran_conf_t));
  assert(res != NULL && "Memory exhausted");
  list_of_node_level_configuration_structures_for_event_trigger_element_t* node = list_get_head(src);
  while(node != NULL){
    res[index].ran_conf_name = cp_str_to_ba(node->ran_configuration_structure_name);

    if (node->list_of_attributes != NULL && node->list_of_attributes->count > 0){
      res[index].sz_attribute = node->list_of_attributes->count;
      res[index].attribute = get_lst_attribute(node->list_of_attributes);
    } else {
      res[index].sz_attribute = 0;
    }
    node = list_get_next(src);
    index++;
  }
  return res;
}

static
e2sm_ccc_ev_trg_frmt_1_t get_ev_trg_1(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_ev_trg_frmt_1_t res = {};

  res.sz_ev_trg_ran_conf = src->count;
  res.ev_trg_ran_conf = get_ev_trg_ran_conf(src);

  return res;
}

static
e2sm_ccc_ev_trg_frmt_2_t get_ev_trg_2(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_ev_trg_frmt_2_t res = {};
  size_t index = 0;

  res.sz_ev_trg_cell= src->count;
  res.ev_trg_cell = calloc(src->count, sizeof(ev_trg_cell_t));
  assert(res.ev_trg_cell != NULL && "Memory exhausted");
  list_of_cell_level_configuration_structures_for_event_trigger_element_t* node = list_get_head(src);
  while(node != NULL){
    res.ev_trg_cell[index].cell_global_id = get_cell_global_id(node->cell_global_id);

    res.ev_trg_cell[index].sz_ev_trg_ran_conf = node->list_of_ran_configuration_structures_for_event_trigger->count;
    res.ev_trg_cell[index].ev_trg_ran_conf = get_ev_trg_ran_conf(node->list_of_ran_configuration_structures_for_event_trigger);

    node = list_get_next(src);
    index++;
  }

  return res;
}

e2sm_ccc_event_trigger_t ccc_dec_event_trigger_json(size_t len, uint8_t const buf[len])
{
  assert(buf != NULL);
  assert(len > 0);
  assert(buf[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_event_trigger_definition_json_t * ric_ev_trg = cJSON_Parseric_event_trigger_definition((char *)buf);
  defer({cJSON_Deleteric_event_trigger_definition(ric_ev_trg);});

  e2sm_ccc_event_trigger_t dst = {0};

  if(ric_ev_trg->event_trigger_definition_format->list_of_node_level_configuration_structures_for_event_trigger != NULL){
    dst.format = FORMAT_1_E2SM_CCC_EV_TRIGGER_FORMAT;
    dst.frmt_1 = get_ev_trg_1(ric_ev_trg->event_trigger_definition_format->list_of_node_level_configuration_structures_for_event_trigger);
  }

  if (ric_ev_trg->event_trigger_definition_format->list_of_cell_level_configuration_structures_for_event_trigger != NULL){
    dst.format = FORMAT_2_E2SM_CCC_EV_TRIGGER_FORMAT;
    dst.frmt_2 = get_ev_trg_2(ric_ev_trg->event_trigger_definition_format->list_of_cell_level_configuration_structures_for_event_trigger);
  }

  if (ric_ev_trg->event_trigger_definition_format->period){
    dst.format = FORMAT_3_E2SM_CCC_EV_TRIGGER_FORMAT;
    dst.frmt_3.period = *ric_ev_trg->event_trigger_definition_format->period;
  }

  return dst;
}

static
act_def_ran_conf_t* get_act_def_ran_conf(list_t* const src){
  assert(src != NULL);
  act_def_ran_conf_t* res = calloc(src->count, sizeof(act_def_ran_conf_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  lst_act_def_ran_conf_element_json_t* ran_conf = list_get_head(src);
  while (ran_conf!= NULL){
    res[index].ran_conf_name = cp_str_to_ba(ran_conf->ran_configuration_structure_name);
    res[index].report_type = ran_conf->report_type;
    if (ran_conf->list_of_attributes != NULL && ran_conf->list_of_attributes->count > 0){
      res[index].sz_attribute = ran_conf->list_of_attributes->count;
      res[index].attribute = get_lst_attribute(ran_conf->list_of_attributes);
    } else {
      res[index].sz_attribute = 0;
    }

    ran_conf = list_get_next(src);
    index++;
  }

  return res;
}

static
values_of_attributes_t get_o_gnb_du_func_node(ran_configuration_structure_json_t const* src){
  assert(src != NULL);

  e2sm_ccc_o_gnb_du_function_t du_function = {0};

  if(src->gnb_du_id)
    du_function.gnb_du_id = *src->gnb_du_id;

  if (src->gnb_du_name)
    du_function.gnb_du_name = strdup(src->gnb_du_name);

  if (src->gnb_id)
    du_function.gnb_id = *src->gnb_id;

  if (src->gnb_id_length)
    du_function.gnb_id_len = *src->gnb_id_length;

  values_of_attributes_t dst = {0};
  dst.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_GNBDUFunction;
  dst.e2sm_ccc_o_gnb_du_function = du_function;

  return dst;
}

static
g_gnb_id_lst_t get_g_gnb_id_lst(list_t* const src){
  assert(src != NULL);
  size_t index = 0;
  g_gnb_id_lst_t res = {0};
  res.sz_g_gnb_id_lst = src->count;
  res.g_gnb_id_lst = calloc(res.sz_g_gnb_id_lst, sizeof(char*));
  char* node = list_get_head(src);
  while (node!= NULL){
    res.g_gnb_id_lst[index] = strdup(node);
    node = list_get_next(src);
    index++;
  }
  return res;
}

static
g_enb_id_lst_t get_g_enb_id_lst(list_t* const src){
  assert(src != NULL);
  size_t index = 0;
  g_enb_id_lst_t res = {0};
  res.sz_g_enb_id_lst = src->count;
  res.g_enb_id_lst = calloc(res.sz_g_enb_id_lst, sizeof(char*));
  char* node = list_get_head(src);
  while (node!= NULL){
    res.g_enb_id_lst[index] = strdup(node);
    node = list_get_next(src);
    index++;
  }
  return res;
}

static
values_of_attributes_t get_o_gnb_cu_cp_func_node(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  e2sm_ccc_o_gnb_cu_cp_function_t cu_cp_function = {0};

  if (src->gnb_id)
    cu_cp_function.gnb_id = *src->gnb_id;

  if (src->gnb_id_length)
    cu_cp_function.gnb_id_len = *src->gnb_id_length;

  if (src->gnb_cu_name)
    cu_cp_function.gnb_cu_name = strdup(src->gnb_cu_name);

  if (src->plmn_id)
    cu_cp_function.plmn_id = get_plmn_id_json(src->plmn_id);

  if (src->x2_block_list)
    cu_cp_function.x2_block_list = get_g_enb_id_lst(src->x2_block_list);

  if (src->x2_allow_list)
    cu_cp_function.x2_allow_list = get_g_enb_id_lst(src->x2_allow_list);

  if (src->xn_block_list)
    cu_cp_function.xn_block_list = get_g_gnb_id_lst(src->xn_block_list);

  if (src->xn_allow_list)
    cu_cp_function.xn_allow_list = get_g_gnb_id_lst(src->xn_allow_list);

  if (src->x2_ho_block_list)
    cu_cp_function.x2_ho_block_list = get_g_enb_id_lst(src->x2_ho_block_list);

  if (src->xn_ho_block_list)
    cu_cp_function.xn_ho_block_list = get_g_gnb_id_lst(src->xn_ho_block_list);

  values_of_attributes_t res = {0};
  res.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_GNBCUCPFunction;
  res.e2sm_ccc_o_gnb_cu_cp_function = cu_cp_function;

  return res;
}

static
values_of_attributes_t get_o_gnb_cu_up_func_node(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  e2sm_ccc_o_gnb_cu_up_function_t cu_up_function = {0};

  if(src->gnb_id)
    cu_up_function.gnb_id = *src->gnb_id;

  if(src->gnb_id_length)
    cu_up_function.gnb_id_len = *src->gnb_id_length;

  if(src->gnb_cu_up_id)
    cu_up_function.gnb_cu_up_id = *src->gnb_cu_up_id;

  if(src->plmn_info_list){
    int index = 0;
    cu_up_function.sz_plmn_info_lst = src->plmn_info_list->count;
    cu_up_function.plmn_info_lst = calloc(src->plmn_info_list->count, sizeof(e2sm_ccc_plmn_info_t));
    assert(cu_up_function.plmn_info_lst != NULL && "Memory exhausted");
    plmn_info_list_element_t* node = list_get_head(src->plmn_info_list);
    while(node != NULL){
      cu_up_function.plmn_info_lst[index] = get_e2sm_ccc_plmn_info(node);
      node = list_get_next(src->plmn_info_list);
      index++;
    }
  }

  values_of_attributes_t dst = {0};
  dst.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_GNBCUUPFunction;
  dst.e2sm_ccc_o_gnb_cu_up_function = cu_up_function;


  return dst;
}

static
values_of_attributes_t get_o_rrm_policy_ratio(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  e2sm_ccc_o_rrm_policy_ratio_t rrm_policy_ratio = {0};

  rrm_policy_ratio.resource_type = *src->resource_type;

  if (src->r_rm_policy_dedicated_ratio)
    rrm_policy_ratio.rrm_policy_dedicated_ratio = *src->r_rm_policy_dedicated_ratio;

  if (src->r_rm_policy_max_ratio)
    rrm_policy_ratio.rrm_policy_max_ratio = *src->r_rm_policy_max_ratio;

  if (src->r_rm_policy_min_ratio)
    rrm_policy_ratio.rrm_policy_min_ratio = *src->r_rm_policy_min_ratio;

  if(src->r_rm_policy_member_list){
    int index = 0;
    rrm_policy_ratio.sz_rrm_policy_member_lst= src->r_rm_policy_member_list->count;
    rrm_policy_ratio.rrm_policy_member_lst = calloc(src->r_rm_policy_member_list->count, sizeof(e2sm_ccc_rrm_policy_member_t));
    assert(rrm_policy_ratio.rrm_policy_member_lst != NULL && "Memory exhausted");
    r_rm_policy_member_list_element_t* node = list_get_head(src->r_rm_policy_member_list);
    while(node != NULL){
      rrm_policy_ratio.rrm_policy_member_lst[index] = get_e2sm_ccc_rrm_policy_member(node);
      node = list_get_next(src->r_rm_policy_member_list);
      index++;
    }
  }

  values_of_attributes_t dst = {0};
  dst.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_RRMPolicyRatio;
  dst.e2sm_ccc_o_rrm_policy_ratio = rrm_policy_ratio;

  return dst;
}

static
values_of_attributes_t  get_o_nr_cell_cu_cell(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  e2sm_ccc_o_nr_cell_cu_t nr_cell_cu = {0};

  if (src->cell_local_id)
    nr_cell_cu.cell_local_id = *src->cell_local_id;

  if(src->plmn_info_list){
    int index = 0;
    nr_cell_cu.sz_plmn_info_lst = src->plmn_info_list->count;
    nr_cell_cu.plmn_info_lst = calloc(src->plmn_info_list->count, sizeof(e2sm_ccc_plmn_info_t));
    assert(nr_cell_cu.plmn_info_lst != NULL && "Memory exhausted");
    plmn_info_list_element_t* node = list_get_head(src->plmn_info_list);
    while(node != NULL){
      nr_cell_cu.plmn_info_lst[index] = get_e2sm_ccc_plmn_info(node);
      node = list_get_next(src->plmn_info_list);
      index++;
    }
  }

  values_of_attributes_t dst = {0};
  dst.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_NRCellCU;
  dst.e2sm_ccc_o_nr_cell_cu = nr_cell_cu;

  return dst;
}

static
values_of_attributes_t get_o_bwp_cell(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  e2sm_ccc_o_bwp_t o_bwp = {0};

  if (src->start_rb)
    o_bwp.start_rb = *src->start_rb;

  if (src->number_of_r_bs)
    o_bwp.number_of_rbs = *src->number_of_r_bs;

  if (src->is_initial_bwp)
    o_bwp.is_initial_bwp = *src->is_initial_bwp;

  if (src->cyclic_prefix)
    o_bwp.cyclic_prefix = *src->cyclic_prefix;

  if (src->bwp_context)
    o_bwp.bwp_context = *src->bwp_context;

  if (src->sub_carrier_spacing){
    switch (*src->sub_carrier_spacing) {
      case 15:
        o_bwp.sub_carrier_spacing = SUB_CARRIER_SPACING_15;
        break;
      case 30:
        o_bwp.sub_carrier_spacing = SUB_CARRIER_SPACING_30;
        break;
      case 60:
        o_bwp.sub_carrier_spacing = SUB_CARRIER_SPACING_60;
        break;
      case 120:
        o_bwp.sub_carrier_spacing = SUB_CARRIER_SPACING_120;
        break;
      default:
        break;
    }
  }

  values_of_attributes_t dst = {0};
  dst.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_BWP;
  dst.e2sm_ccc_o_bwp = o_bwp;

  return dst;
}


static
e2sm_ccc_partition_flow_lst_item_t get_e2sm_ccc_partition_flow_lst_item(partition_flow_list_element_t const* src){
  assert(src != NULL);

  e2sm_ccc_partition_flow_lst_item_t dst = {0};

  if (src->snssai)
    dst.s_nssai = get_s_nssai_e2sm(src->snssai);

  if (src->plmn_id){
    dst.plmn_id = get_plmn_id_json(src->plmn_id);
  }

  if (src->the_5_qi_list != NULL){
    dst.lst_5qi = calloc(1, sizeof(e2sm_ccc_5qi_lst_t));
    assert(dst.lst_5qi != NULL);

    int index = 0;
    dst.lst_5qi->sz_lst_5qi = src->the_5_qi_list->count;
    dst.lst_5qi->lst_5qi = calloc(src->the_5_qi_list->count, sizeof(uint32_t));
    assert(dst.lst_5qi->lst_5qi != NULL && "Memory exhausted");
    uint32_t* node = list_get_head(src->the_5_qi_list);
    while(node != NULL){
      dst.lst_5qi->lst_5qi[index] = *node;
      node = list_get_next(src->the_5_qi_list);
      index++;
    }
  }

  return dst;
}

static
e2sm_ccc_partition_lst_item_t get_e2sm_ccc_partition_lst_item(partition_list_element_t const* src){
  assert(src != NULL);

  e2sm_ccc_partition_lst_item_t dst = {0};

  if (src->p_offset_to_point_a)
    dst.p_offset_to_point_A = *src->p_offset_to_point_a;

  if (src->p_number_of_r_bs){
    dst.p_number_of_rbs = *src->p_number_of_r_bs;
  }

  if (src->partition_flow_list != NULL){
    int index = 0;
    dst.partition_flow_lst.sz_partition_flow_lst_item = src->partition_flow_list->count;
    dst.partition_flow_lst.partition_flow_lst_item = calloc(src->partition_flow_list->count, sizeof(e2sm_ccc_partition_flow_lst_item_t));
    assert(dst.partition_flow_lst.partition_flow_lst_item != NULL && "Memory exhausted");
    partition_flow_list_element_t* node = list_get_head(src->partition_flow_list);
    while(node != NULL){
      dst.partition_flow_lst.partition_flow_lst_item[index] = get_e2sm_ccc_partition_flow_lst_item(node);
      node = list_get_next(src->partition_flow_list);
      index++;
    }
  }

  return dst;
}

static
values_of_attributes_t get_o_nr_cell_du_cell(ran_configuration_structure_json_t const* src){
  assert(src != NULL);

  e2sm_ccc_o_nr_cell_du_t nr_cell_du = {0};

  if (src->cell_local_id)
    nr_cell_du.cell_local_id = *src->cell_local_id;

  if (src->operational_state)
    nr_cell_du.operational_state = *src->operational_state;

  if (src->administrative_state)
    nr_cell_du.administrative_state = *src->administrative_state;

  if (src->cell_state)
    nr_cell_du.cell_state = *src->cell_state;

  if (src->nr_pci)
    nr_cell_du.nr_pci = *src->nr_pci;

  if(src->nr_tac)
    nr_cell_du.nr_tac = *src->nr_tac;

  if (src->arfcn_dl)
    nr_cell_du.arfcn_dl = *src->arfcn_dl;

  if (src->arfcn_ul)
    nr_cell_du.arfcn_ul = *src->arfcn_ul;

  if (src->arfcn_sul)
    nr_cell_du.arfcn_sul = *src->arfcn_sul;

  if (src->ssb_frequency)
    nr_cell_du.ssb_frequency = *src->ssb_frequency;

  if (src->ssb_periodicity){
    switch (*src->ssb_periodicity) {
      case 5:
        nr_cell_du.ssb_periodicity = SSB_PERIODICITY_5;
        break;
      case 10:
        nr_cell_du.ssb_periodicity = SSB_PERIODICITY_10;
        break;
      case 20:
        nr_cell_du.ssb_periodicity = SSB_PERIODICITY_20;
        break;
      case 40:
        nr_cell_du.ssb_periodicity = SSB_PERIODICITY_40;
        break;
      case 80:
        nr_cell_du.ssb_periodicity = SSB_PERIODICITY_80;
        break;
      case 160:
        nr_cell_du.ssb_periodicity = SSB_PERIODICITY_160;
        break;
      default:
        break;
    }
  }

  if (src->ssb_sub_carrier_spacing){
    switch (*src->ssb_sub_carrier_spacing) {
      case 15:
        nr_cell_du.ssb_sub_carrier_spacing = SSB_SUB_CARRIER_SPACING_15;
        break;
      case 30:
        nr_cell_du.ssb_sub_carrier_spacing = SSB_SUB_CARRIER_SPACING_30;
        break;
      case 120:
        nr_cell_du.ssb_sub_carrier_spacing = SSB_SUB_CARRIER_SPACING_120;
        break;
      case 240:
        nr_cell_du.ssb_sub_carrier_spacing = SSB_SUB_CARRIER_SPACING_240;
        break;
      default:
        break;
    }
  }

  if (src->ssb_offset)
    nr_cell_du.ssb_off_set = *src->ssb_offset;

  if (src->ssb_duration){
    switch (*src->ssb_duration) {
      case 1:
        nr_cell_du.ssb_duration= SSB_DURATION_1;
        break;
      case 2:
        nr_cell_du.ssb_duration= SSB_DURATION_2;
        break;
      case 3:
        nr_cell_du.ssb_duration= SSB_DURATION_3;
        break;
      case 4:
        nr_cell_du.ssb_duration= SSB_DURATION_4;
        break;
      case 5:
        nr_cell_du.ssb_duration= SSB_DURATION_5;
        break;
      default:
        break;
    }
  }

  if (src->b_s_channel_bw_dl)
    nr_cell_du.bS_Channel_BwDL = *src->b_s_channel_bw_dl;

  if (src->b_s_channel_bw_ul)
    nr_cell_du.bS_Channel_BwUL = *src->b_s_channel_bw_ul;

  if (src->b_s_channel_bw_sul)
    nr_cell_du.bS_Channel_BwSUL = *src->b_s_channel_bw_sul;

  if(src->plmn_info_list){
    int index = 0;
    nr_cell_du.sz_plmn_info_lst = src->plmn_info_list->count;
    nr_cell_du.plmn_info_lst = calloc(src->plmn_info_list->count, sizeof(e2sm_ccc_plmn_info_t));
    assert(nr_cell_du.plmn_info_lst != NULL && "Memory exhausted");
    plmn_info_list_element_t* node = list_get_head(src->plmn_info_list);
    while(node != NULL){
      nr_cell_du.plmn_info_lst[index] = get_e2sm_ccc_plmn_info(node);
      node = list_get_next(src->plmn_info_list);
      index++;
    }
  }

  if (src->bwp_list){
    int index = 0;
    nr_cell_du.sz_bwp_lst = src->bwp_list->count;
    nr_cell_du.bwp_lst = calloc(src->bwp_list->count, sizeof(e2sm_ccc_o_bwp_t));
    assert(nr_cell_du.bwp_lst!= NULL && "Memory exhausted");
    bwp_list_element_t* node = list_get_head(src->bwp_list);
    while(node != NULL){
      values_of_attributes_t values_of_attributes = get_o_bwp_cell((ran_configuration_structure_json_t*)node);
      nr_cell_du.bwp_lst[index] = values_of_attributes.e2sm_ccc_o_bwp;
      node = list_get_next(src->bwp_list);
      index++;
    }
  }

  if (src->partition_list){
    int index = 0;
    nr_cell_du.partition_lst.sz_partition_lst_item = src->partition_list->count;
    nr_cell_du.partition_lst.partition_lst_item = calloc(src->partition_list->count, sizeof(e2sm_ccc_partition_lst_item_t));
    assert(nr_cell_du.partition_lst.partition_lst_item != NULL && "Memory exhausted");
    partition_list_element_t* node = list_get_head(src->partition_list);
    while(node != NULL){
      nr_cell_du.partition_lst.partition_lst_item[index] = get_e2sm_ccc_partition_lst_item(node);
      node = list_get_next(src->partition_list);
      index++;
    }
  }

  values_of_attributes_t dst = {0};
  dst.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_NRCellDU;
  dst.e2sm_ccc_o_nr_cell_du = nr_cell_du;

  return dst;
}

static
values_of_attributes_t get_o_ces_man_func_cell(ran_configuration_structure_json_t const* src){
  assert(src != NULL);

  e2sm_ccc_o_ces_management_function_t ces_management = {0};

  if (src->energy_saving_state)
    ces_management.energy_saving_state = *src->energy_saving_state;

  if (src->energy_saving_control)
    ces_management.energy_saving_control = *src->energy_saving_control;

  if (src->ces_switch)
    ces_management.ces_switch = *src->ces_switch;

  values_of_attributes_t dst = {0};
  dst.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_CESManagementFunction;
  dst.e2sm_ccc_o_ces_management_function = ces_management;

  return dst;
}

static
values_of_attributes_t get_values_of_attributes(const char* ran_conf_name, ran_configuration_structure_json_t const* ran_conf_str_json){
  assert(ran_conf_name != NULL);
  assert(ran_conf_str_json != NULL);

  values_of_attributes_t res = {0};

  if (strcmp(ran_conf_name, "O-GNBDUFunction") == 0)
    res = get_o_gnb_du_func_node(ran_conf_str_json);

  if (strcmp(ran_conf_name, "O-GNBCUCPFunction") == 0)
    res = get_o_gnb_cu_cp_func_node(ran_conf_str_json);

  if (strcmp(ran_conf_name, "O-GNBCUUPFunction") == 0)
    res = get_o_gnb_cu_up_func_node(ran_conf_str_json);

  if (strcmp(ran_conf_name, "O-RRMPolicyRatio") == 0)
    res = get_o_rrm_policy_ratio(ran_conf_str_json);

  if (strcmp(ran_conf_name, "O-NRCellCU") == 0)
    res = get_o_nr_cell_cu_cell(ran_conf_str_json);

  if (strcmp(ran_conf_name, "O-NRCellDU") == 0)
    res = get_o_nr_cell_du_cell(ran_conf_str_json);

  if (strcmp(ran_conf_name, "O-BWP") == 0)
    res = get_o_bwp_cell(ran_conf_str_json);

  if (strcmp(ran_conf_name, "O-CESManagementFunction") == 0)
    res = get_o_ces_man_func_cell(ran_conf_str_json);

  return res;
}

static
ind_msg_ran_conf_t* get_ind_msg_ran_conf(list_t* const src){
  assert(src != NULL);
  ind_msg_ran_conf_t* res = calloc(src->count, sizeof(ind_msg_ran_conf_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  list_of_configuration_structures_reported_element_t* ran_conf = list_get_head(src);
  while (ran_conf!= NULL){
    res[index].ran_conf_name = cp_str_to_ba(ran_conf->ran_configuration_structure_name);
    res[index].change_type = ran_conf->change_type;
    res[index].vals_attributes = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->values_of_attributes->ran_configuration_structure);
    if (ran_conf->old_values_of_attributes){
      res[index].old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
      assert(res[index].old_vals_attributes != NULL);
      *res[index].old_vals_attributes = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->old_values_of_attributes->ran_configuration_structure);
    }
    ran_conf = list_get_next(src);
    index++;
  }

  return res;
}

static
ctrl_msg_ran_conf_t* get_ctrl_msg_ran_conf(list_t* const src){
  assert(src != NULL);
  ctrl_msg_ran_conf_t* res = calloc(src->count, sizeof(ctrl_msg_ran_conf_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  list_of_configuration_structure_control_element_t* ran_conf = list_get_head(src);
  while (ran_conf!= NULL){
    res[index].ran_conf_name = cp_str_to_ba(ran_conf->ran_configuration_structure_name);
    res[index].vals_attributes = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->new_values_of_attributes->ran_configuration_structure);
    if (ran_conf->old_values_of_attributes){
      res[index].old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
      assert(res[index].old_vals_attributes != NULL);
      *res[index].old_vals_attributes = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->old_values_of_attributes->ran_configuration_structure);
    }
    ran_conf = list_get_next(src);
    index++;
  }

  return res;
}

static
e2sm_ccc_ind_msg_frmt_1_t get_ind_msg_frmt1(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_ind_msg_frmt_1_t res = {};

  res.sz_ind_msg_node_conf = src->count;
  res.ind_msg_ran_conf = get_ind_msg_ran_conf(src);

  return res;
}



static
e2sm_ccc_ctrl_msg_frmt_1_t get_ctrl_msg_frmt1(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_ctrl_msg_frmt_1_t res = {};

  res.sz_ctrl_msg_ran_conf = src->count;
  res.ctrl_msg_ran_conf = get_ctrl_msg_ran_conf(src);

  return res;
}

static
ctrl_out_conf_accepted_t* get_ctrl_out_conf_accepted(list_t* const src){
  assert(src != NULL);
  ctrl_out_conf_accepted_t* res = calloc(src->count, sizeof(ctrl_out_conf_accepted_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  ran_configuration_structures_accepted_list_element_t* ran_conf = list_get_head(src);
  while (ran_conf!= NULL){
    if (ran_conf->applied_timestamp)
      res[index].app_timestamp = cp_str_to_ba(ran_conf->applied_timestamp);
    res[index].ran_conf_name = cp_str_to_ba(ran_conf->ran_configuration_structure_name);
    res[index].cur_atr_val = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->current_values_of_attributes->ran_configuration_structure);
    res[index].old_atr_val = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->old_values_of_attributes->ran_configuration_structure);
    ran_conf = list_get_next(src);
    index++;
  }

  return res;
}

static
ctrl_out_conf_failed_t* get_ctrl_out_conf_failed(list_t* const src){
  assert(src != NULL);
  ctrl_out_conf_failed_t* res = calloc(src->count, sizeof(ctrl_out_conf_failed_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  ran_configuration_structures_failed_list_element_t* ran_conf = list_get_head(src);
  while (ran_conf!= NULL){
    res[index].ran_conf_name = cp_str_to_ba(ran_conf->ran_configuration_structure_name);
    res[index].req_atr_val = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->requested_values_of_attributes->ran_configuration_structure);
    res[index].old_atr_val = get_values_of_attributes(ran_conf->ran_configuration_structure_name, ran_conf->old_values_of_attributes->ran_configuration_structure);
    res[index].cause = ran_conf->cause;
    ran_conf = list_get_next(src);
    index++;
  }

  return res;
}

static
e2sm_ccc_ctrl_out_frmt_1_t get_ctrl_out_frmt1(control_outcome_format_json_t* const src){
  assert(src != NULL);
  e2sm_ccc_ctrl_out_frmt_1_t res = {0};

  if (src->received_timestamp)
    res.rev_timestamp = cp_str_to_ba(src->received_timestamp);

  if (src->ran_configuration_structures_accepted_list != NULL && src->ran_configuration_structures_accepted_list->count > 0){
    res.sz_ctrl_out_conf_accepted = src->ran_configuration_structures_accepted_list->count;
    res.ctrl_out_conf_accepted = get_ctrl_out_conf_accepted(src->ran_configuration_structures_accepted_list);
  }

  if (src->ran_configuration_structures_failed_list != NULL && src->ran_configuration_structures_failed_list->count > 0){
    res.sz_ctrl_out_conf_failed = src->ran_configuration_structures_failed_list->count;
    res.ctrl_out_conf_failed = get_ctrl_out_conf_failed(src->ran_configuration_structures_failed_list);
  }

  return res;
}

static
ctrl_out_cell_t* get_ctrl_out_cell(list_t* const src){
  assert(src != NULL);
  ctrl_out_cell_t* res = calloc(src->count, sizeof(ctrl_out_cell_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  list_of_cells_for_control_outcome_element_t* ran_conf = list_get_head(src);
  while (ran_conf!= NULL){
    res[index].cell_global_id = get_cell_global_id(ran_conf->cell_global_id);

    if (ran_conf->ran_configuration_structures_accepted_list != NULL && ran_conf->ran_configuration_structures_accepted_list->count > 0){
      res[index].sz_ctrl_out_conf_accepted = ran_conf->ran_configuration_structures_accepted_list->count;
      res[index].ctrl_out_conf_accepted = get_ctrl_out_conf_accepted(ran_conf->ran_configuration_structures_accepted_list);
    }

    if (ran_conf->ran_configuration_structures_failed_list != NULL && ran_conf->ran_configuration_structures_failed_list->count > 0){
      res[index].sz_ctrl_out_conf_failed = ran_conf->ran_configuration_structures_failed_list->count;
      res[index].ctrl_out_conf_failed = get_ctrl_out_conf_failed(ran_conf->ran_configuration_structures_failed_list);
    }

    ran_conf = list_get_next(src);
    index++;
  }

  return res;
}

static
e2sm_ccc_ctrl_out_frmt_2_t get_ctrl_out_frmt2(control_outcome_format_json_t* const src){
  assert(src != NULL);
  e2sm_ccc_ctrl_out_frmt_2_t res = {0};

  if (src->received_timestamp)
    res.rev_timestamp = cp_str_to_ba(src->received_timestamp);

  if (src->list_of_cells_for_control_outcome != NULL && src->list_of_cells_for_control_outcome->count > 0){
    res.sz_ctrl_out_cell = src->list_of_cells_for_control_outcome->count;
    res.ctrl_out_cell = get_ctrl_out_cell(src->list_of_cells_for_control_outcome);
  }



  return res;
}

static
e2sm_ccc_act_def_frmt_1_t get_act_def_frmt1(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_act_def_frmt_1_t res = {};

  res.sz_act_def_ran_conf = src->count;
  res.act_def_ran_conf = get_act_def_ran_conf(src);

  return res;
}

static
e2sm_ccc_ctrl_msg_frmt_2_t get_ctrl_msg_frmt2(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_ctrl_msg_frmt_2_t res = {};
  size_t index = 0;

  res.sz_ctrl_msg_cell = src->count;
  res.ctrl_msg_cell = calloc(src->count, sizeof(ctrl_msg_cell_t));
  assert(res.ctrl_msg_cell != NULL && "Memory exhausted");
  list_of_cells_controlled_element_t* node = list_get_head(src);
  while(node != NULL){
    res.ctrl_msg_cell[index].cell_global_id = get_cell_global_id(node->cell_global_id);
    res.ctrl_msg_cell[index].sz_ctrl_msg_ran_conf = node->list_of_configuration_structures->count;
    res.ctrl_msg_cell[index].ctrl_msg_ran_conf = get_ctrl_msg_ran_conf(node->list_of_configuration_structures);
    node = list_get_next(src);
    index++;
  }

  return res;
}

static
e2sm_ccc_ind_msg_frmt_2_t get_ind_msg_frmt2(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_ind_msg_frmt_2_t res = {};
  size_t index = 0;

  res.sz_ind_msg_cell_report = src->count;
  res.ind_msg_cell_report = calloc(src->count, sizeof(ind_msg_ran_conf_t));
  assert(res.ind_msg_cell_report != NULL && "Memory exhausted");
  list_of_cells_reported_element_t* node = list_get_head(src);
  while(node != NULL){
    res.ind_msg_cell_report[index].cell_global_id = get_cell_global_id(node->cell_global_id);
    res.ind_msg_cell_report[index].sz_ind_msg_ran_conf = node->list_of_configuration_structures_reported->count;
    res.ind_msg_cell_report[index].ind_msg_ran_conf = get_ind_msg_ran_conf(node->list_of_configuration_structures_reported);
    node = list_get_next(src);
    index++;
  }

  return res;
}

static
e2sm_ccc_act_def_frmt_2_t get_act_def_frmt2(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_act_def_frmt_2_t res = {};
  size_t index = 0;

  res.sz_act_def_cell_report = src->count;
  res.act_def_cell_report = calloc(src->count, sizeof(act_def_cell_report_t));
  assert(res.act_def_cell_report != NULL && "Memory exhausted");
  lst_act_def_cell_ran_conf_element_json_t* node = list_get_head(src);
  while(node != NULL){
    res.act_def_cell_report[index].cell_global_id = get_cell_global_id(node->cell_global_id);
    res.act_def_cell_report[index].sz_act_def_ran_conf = node->list_of_cell_level_ran_configuration_structures_for_adf->count;
    res.act_def_cell_report[index].act_def_ran_conf = get_act_def_ran_conf(node->list_of_cell_level_ran_configuration_structures_for_adf);
    node = list_get_next(src);
    index++;
  }

  return res;
}

e2sm_ccc_action_def_t ccc_dec_action_def_json(size_t len, uint8_t const action_def[len])
{
  assert(action_def != NULL);
  assert(len != 0);
  assert(action_def[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_action_definition_json_t* ric_act_def= cJSON_Parseric_action_definition((char *)action_def);
  defer({cJSON_Deleteric_action_definition(ric_act_def); });

  e2sm_ccc_action_def_t dst = {0};
  dst.ric_style_type = ric_act_def->ric_style_type;
  if(ric_act_def->action_definition_format->lst_act_def_node_ran_conf != NULL){
    // Format 1
    dst.format = FORMAT_1_E2SM_CCC_ACT_DEF;
    dst.frmt_1 = get_act_def_frmt1(ric_act_def->action_definition_format->lst_act_def_node_ran_conf);
  } else if (ric_act_def->action_definition_format->lst_act_def_cell_ran_conf != NULL){
    // Format 2
    dst.format = FORMAT_2_E2SM_CCC_ACT_DEF;
    dst.frmt_2 = get_act_def_frmt2(ric_act_def->action_definition_format->lst_act_def_cell_ran_conf);
  }

  return dst;
}

e2sm_ccc_ind_hdr_t ccc_dec_ind_hdr_json(size_t len, uint8_t const ind_hdr[len])
{
  static_assert(sizeof(e2sm_ccc_ind_hdr_frmt_1_t) == sizeof(indication_header_format_json_t));
  assert(ind_hdr != NULL);
  assert(len != 0);
  assert(ind_hdr[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_indication_header_json_t * ric_ind_hdr = cJSON_Parseric_indication_header((char *)ind_hdr);
  defer({cJSON_Deleteric_indication_header(ric_ind_hdr);});

  e2sm_ccc_ind_hdr_t dst = {.format = FORMAT_1_E2SM_CCC_IND_HDR};
  dst.frmt_1 = cp_e2sm_ccc_ind_hdr_frmt_1((e2sm_ccc_ind_hdr_frmt_1_t *)ric_ind_hdr->indication_header_format);

  return dst;
}

e2sm_ccc_ind_msg_t ccc_dec_ind_msg_json(size_t len, uint8_t const ind_msg[len])
{
  assert(ind_msg!= NULL);
  assert(len != 0);
  assert(ind_msg[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_indication_message_json_t * ric_ind_msg = cJSON_Parseric_indication_message((char *)ind_msg);
  defer({cJSON_Deleteric_indication_message(ric_ind_msg);});
   
  e2sm_ccc_ind_msg_t dst = {0};
  if(ric_ind_msg->indication_message_format->list_of_configuration_structures_reported != NULL){
    // Format 1
    dst.format = FORMAT_1_E2SM_CCC_IND_MSG;
    dst.frmt_1 = get_ind_msg_frmt1(ric_ind_msg->indication_message_format->list_of_configuration_structures_reported);
  } else if (ric_ind_msg->indication_message_format->list_of_cells_reported != NULL){
    // Format 2
    dst.format = FORMAT_2_E2SM_CCC_IND_MSG;
    dst.frmt_2 = get_ind_msg_frmt2(ric_ind_msg->indication_message_format->list_of_cells_reported);
  }

  return dst;
}

e2sm_ccc_cpid_t ccc_dec_cpid_json(size_t len, uint8_t const call_proc_id[len])
{
  assert(call_proc_id != NULL);
  assert(0 != 0 && "Not implemented");
  e2sm_ccc_cpid_t dst = {};

  return dst;
}

e2sm_ccc_ctrl_hdr_t ccc_dec_ctrl_hdr_json(size_t len, uint8_t const ctrl_hdr[len])
{
  static_assert(sizeof(e2sm_ccc_ctrl_hdr_frmt_1_t) == sizeof(control_header_format_json_t));
  assert(ctrl_hdr != NULL);
  assert(len != 0);
  assert(ctrl_hdr[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_control_header_json_t * ric_ctrl_hdr = cJSON_ParseRicControlHeaderJson((char *)ctrl_hdr);
  defer({cJSON_DeleteRicControlHeaderJson(ric_ctrl_hdr);});

  
  e2sm_ccc_ctrl_hdr_t dst = {.format = FORMAT_1_E2SM_CCC_CTRL_HDR};
  dst.frmt_1 = cp_e2sm_ccc_ctrl_hdr_frmt_1((e2sm_ccc_ctrl_hdr_frmt_1_t *)ric_ctrl_hdr->control_header_format);

  return dst;
}

e2sm_ccc_ctrl_msg_t ccc_dec_ctrl_msg_json(size_t len, uint8_t const ctrl_msg[len])
{
  assert(ctrl_msg != NULL);
  assert(len > 0);
  assert(ctrl_msg[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_control_message_json_t* ric_ctrl_msg = cJSON_Parseric_control_message((char *)ctrl_msg);
  defer({cJSON_Deleteric_control_message(ric_ctrl_msg);});

  e2sm_ccc_ctrl_msg_t dst = {0};
  if(ric_ctrl_msg->control_message_format->list_of_configuration_structures != NULL){
    // Format 1
    dst.format = FORMAT_1_E2SM_CCC_CTRL_MSG;
    dst.frmt_1 = get_ctrl_msg_frmt1(ric_ctrl_msg->control_message_format->list_of_configuration_structures);
  } else if (ric_ctrl_msg->control_message_format->list_of_cells_controlled != NULL){
    // Format 2
    dst.format = FORMAT_2_E2SM_CCC_CTRL_MSG;
    dst.frmt_2 = get_ctrl_msg_frmt2(ric_ctrl_msg->control_message_format->list_of_cells_controlled);
  }

  return dst;
}

e2sm_ccc_ctrl_out_t ccc_dec_ctrl_out_json(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(ctrl_out != NULL);
  assert(len > 0);
  assert(ctrl_out[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_control_outcome_json_t * ric_ctrl_out = cJSON_Parseric_control_outcome((char *)ctrl_out);
  defer({cJSON_Deleteric_control_outcome(ric_ctrl_out);});

  e2sm_ccc_ctrl_out_t dst = {0};
  if(ric_ctrl_out->control_outcome_format->list_of_cells_for_control_outcome == NULL){
    // Format 1
    dst.format = FORMAT_1_E2SM_CCC_CTRL_OUT;
    dst.frmt_1 = get_ctrl_out_frmt1(ric_ctrl_out->control_outcome_format);
  } else {
    // Format 2
    dst.format = FORMAT_2_E2SM_CCC_CTRL_OUT;
    dst.frmt_2 = get_ctrl_out_frmt2(ric_ctrl_out->control_outcome_format);
  }

  return dst;
}

static
ran_function_name_t create_ran_function_name(ran_function_name_json_t const* src){
  assert(src != NULL);
  ran_function_name_t dst = {0};
  dst.name = cp_str_to_ba(src->ran_function_short_name);
  dst.description = cp_str_to_ba(src->ran_function_description);
  dst.oid = cp_str_to_ba(src->ran_function_service_model_oid);
  if(src->ran_function_instance)
    memcpy(dst.instance, src->ran_function_instance, sizeof(long));

  return dst;
}

e2sm_ccc_func_def_t ccc_dec_func_def_json(size_t len, uint8_t const func_def[len])
{
  assert(func_def != NULL);
  assert(len > 0);
  assert(func_def[len-1] == '\0' && "Need zero terminated string for this interface");

  ric_function_definition_json_t* ric_ind_msg = cJSON_Parseric_function_definition((char *)func_def);
  defer({cJSON_Deleteric_function_definition(ric_ind_msg);});

  e2sm_ccc_func_def_t dst = {0};
  dst.name = create_ran_function_name(ric_ind_msg->ran_function_name);

  if (ric_ind_msg->list_of_supported_node_level_configuration_structures != NULL)
    assert(0 != 0 && "Not implemented");

  if (ric_ind_msg->list_of_cells_for_ran_function_definition != NULL)
    assert(0 != 0 && "Not implemented");

  return dst;
}

