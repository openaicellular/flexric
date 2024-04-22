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
#include "../ie/json/ric_function_definition_json.h"
#include "../ie/json/ric_event_trigger_definition_json.h"
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
values_of_attributes_t get_o_gnb_cu_cp_func_node(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  e2sm_ccc_o_gnb_cu_cp_function_t cu_cp_function = {0};

  if (src->gnb_id)
    cu_cp_function.gnb_id = *src->gnb_id;

  if (src->gnb_id_length)
    cu_cp_function.gnb_id_len = *src->gnb_id_length;

  if (src->gnb_cu_name)
    cu_cp_function.gnb_cu_name = strdup(src->gnb_du_name);

  if (src->plmn_id)
    cu_cp_function.plmn_id = get_plmn_id_json(src->plmn_id);

  if (src->x2_block_list)
    // Conflict type with json schema
    assert(0 != 0 && "Not implemented");

  if (src->x2_allow_list)
    assert(0 != 0 && "Not implemented");

  if (src->xn_block_list)
    assert(0 != 0 && "Not implemented");

  if (src->xn_allow_list)
    assert(0 != 0 && "Not implemented");

  if (src->x2_ho_block_list)
    assert(0 != 0 && "Not implemented");

  if (src->xn_ho_block_list)
    assert(0 != 0 && "Not implemented");

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
      cu_up_function.plmn_info_lst[index].plmn_id = get_plmn_id_json(node->plmn_id);
      if (node->snssai != NULL){
        cu_up_function.plmn_info_lst[index].s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
        assert(cu_up_function.plmn_info_lst[index].s_nssai != NULL);
        *cu_up_function.plmn_info_lst[index].s_nssai = get_s_nssai_e2sm(node->snssai);
      }
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
      rrm_policy_ratio.rrm_policy_member_lst[index].plmn_id = get_plmn_id_json(node->plmn_id);
      if (node->snssai){
        rrm_policy_ratio.rrm_policy_member_lst[index].s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
        assert(rrm_policy_ratio.rrm_policy_member_lst[index].s_nssai != NULL);
        *rrm_policy_ratio.rrm_policy_member_lst[index].s_nssai = get_s_nssai_e2sm(node->snssai);
      }
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
  assert(0 != 0 && "Not implemented");
}

static
values_of_attributes_t get_o_nr_cell_du_cell(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
values_of_attributes_t get_o_bwp_cell(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
values_of_attributes_t get_o_ces_man_func_cell(ran_configuration_structure_json_t const* src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
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
e2sm_ccc_ind_msg_frmt_1_t get_ind_msg_frmt1(list_t* const src){
  assert(src != NULL);
  e2sm_ccc_ind_msg_frmt_1_t res = {};

  res.sz_ind_msg_node_conf = src->count;
  res.ind_msg_ran_conf = get_ind_msg_ran_conf(src);

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
  assert(ctrl_hdr != NULL);
  assert(0 != 0 && "Not implemented");
  e2sm_ccc_ctrl_hdr_t dst = {0};

  return dst;
}

e2sm_ccc_ctrl_msg_t ccc_dec_ctrl_msg_json(size_t len, uint8_t const ctrl_msg[len])
{
  assert(ctrl_msg != NULL);
  assert(len > 0);
  assert(0 != 0 && "Not implemented");
  e2sm_ccc_ctrl_msg_t dst = {0};

  return dst;
}

e2sm_ccc_ctrl_out_t ccc_dec_ctrl_out_json(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(ctrl_out != NULL);
  assert(len > 0);
  assert(0 != 0 && "Not implemented");
  e2sm_ccc_ctrl_out_t dst = {0};

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

