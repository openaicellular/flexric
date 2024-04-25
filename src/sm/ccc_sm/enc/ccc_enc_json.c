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

#include <stdio.h>

#include "ccc_enc_json.h"
#include "../ie/json/ric_control_message_json.h"
#include "../ie/json/ric_indication_header_json.h"
#include "../ie/json/ric_action_definition_json.h"
#include "../ie/json/ric_indication_message_json.h"
#include "../ie/json/ric_function_definition_json.h"
#include "../ie/json/ric_event_trigger_definition_json.h"

#include "../../../util/alg_ds/alg/defer.h"

#include <assert.h>

static
plmn_id_json_t* create_plmn_id_json(e2sm_plmn_t const src){
  plmn_id_json_t* dst = calloc(1, sizeof(plmn_id_json_t));
  assert(dst != NULL && "Memory exhausted");

  int length = snprintf(NULL, 0, "%hu", src.mcc);
  dst->mcc = (char *)malloc((length + 1) * sizeof(char));
  assert(dst->mcc != NULL && "Memory exhausted");
  snprintf(dst->mcc, length + 1, "%hu", src.mcc);

  length = snprintf(NULL, 0, "%hu", src.mnc);
  dst->mnc = (char *)malloc((length + 1) * sizeof(char));
  assert(dst->mnc != NULL && "Memory exhausted");
  snprintf(dst->mnc, length + 1, "%hu", src.mnc);

  return dst;
}

static
snssai_json_t* create_snssai_json(s_nssai_e2sm_t const src){
  snssai_json_t * dst = calloc(1, sizeof(snssai_json_t));
  assert(dst != NULL && "Memory exhausted");

  dst->sst = calloc(1, sizeof(uint8_t));
  assert(dst->sst != NULL);
  *dst->sst = src.sST;

  if (src.sD){
    int length = snprintf(NULL, 0, "%d", *src.sD);
    dst->sd = (char *)malloc((length + 1) * sizeof(char));
    assert(dst->sd!= NULL && "Memory exhausted");
    snprintf(dst->sd, length + 1, "%d", *src.sD);
  }

  return dst;
}

static
cell_global_id_json_t* create_cell_global_id(cell_global_id_t const src){
  cell_global_id_json_t* cell_id = calloc(1, sizeof(cell_global_id_json_t));
  assert(cell_id != NULL && "Memory exhausted");

  int length = snprintf(NULL, 0, "%lu", (uint64_t)src.nr_cgi.nr_cell_id);
  cell_id->n_r_cell_identity = (char *)malloc((length + 1) * sizeof(char));
  assert(cell_id->n_r_cell_identity != NULL && "Memory exhausted");
  snprintf(cell_id->n_r_cell_identity, length + 1, "%lu", (uint64_t)src.nr_cgi.nr_cell_id);

  cell_id->plmn_identity = create_plmn_id_json(src.nr_cgi.plmn_id);

  return cell_id;
}

static
plmn_info_list_element_t* create_info_list_element(e2sm_ccc_plmn_info_t const src){
  plmn_info_list_element_t* dst = calloc(1, sizeof(plmn_info_list_element_t));
  assert(dst != NULL);

  dst->plmn_id = create_plmn_id_json(src.plmn_id);

  if (src.s_nssai!= NULL){
    dst->snssai = create_snssai_json(*src.s_nssai);
  }

  return dst;
}

static
list_of_node_level_configuration_structures_for_event_trigger_element_t* create_ev_trg_ran_conf_element(ev_trg_ran_conf_t const src){
  list_of_node_level_configuration_structures_for_event_trigger_element_t* dst = calloc(1, sizeof(list_of_node_level_configuration_structures_for_event_trigger_element_t));
  assert(dst != NULL && "Memory exhausted");

  dst->ran_configuration_structure_name = copy_ba_to_str(&src.ran_conf_name);

  if (src.sz_attribute > 0){
    dst->list_of_attributes = list_create(false, NULL);
    for (size_t i = 0; i < src.sz_attribute; ++i){
      list_add_tail(
          dst->list_of_attributes,
          copy_ba_to_str(&src.attribute[i].attribute_name),
          sizeof(char *)
          );
    }
  }

  return dst;
}

static
event_trigger_definition_format_json_t* create_ev_trigger_def_1(e2sm_ccc_ev_trg_frmt_1_t const src){
  event_trigger_definition_format_json_t* dst = calloc(1, sizeof(event_trigger_definition_format_json_t));
  assert(dst != NULL && "Memory exhausted");

  dst->list_of_node_level_configuration_structures_for_event_trigger = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ev_trg_ran_conf; ++i){
    list_add_tail(
        dst->list_of_node_level_configuration_structures_for_event_trigger,
        create_ev_trg_ran_conf_element(src.ev_trg_ran_conf[i]),
        sizeof(list_of_node_level_configuration_structures_for_event_trigger_element_t*)
        );
  }
  return dst;
}

static
list_of_cell_level_configuration_structures_for_event_trigger_element_t* create_ev_trg_cell_element(ev_trg_cell_t const src){
  list_of_cell_level_configuration_structures_for_event_trigger_element_t* dst = calloc(1, sizeof(list_of_cell_level_configuration_structures_for_event_trigger_element_t));
  assert(dst != NULL && "Memory exhausted");

  dst->cell_global_id = create_cell_global_id(src.cell_global_id);
  dst->list_of_ran_configuration_structures_for_event_trigger = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ev_trg_ran_conf; ++i){
    list_add_tail(
        dst->list_of_ran_configuration_structures_for_event_trigger,
        create_ev_trg_ran_conf_element(src.ev_trg_ran_conf[i]),
        sizeof(list_of_node_level_configuration_structures_for_event_trigger_element_t*)
        );
  }

  return dst;
}

static
event_trigger_definition_format_json_t* create_ev_trigger_def_2(e2sm_ccc_ev_trg_frmt_2_t const src){
  event_trigger_definition_format_json_t* dst = calloc(1, sizeof(event_trigger_definition_format_json_t));
  assert(dst != NULL && "Memory exhausted");

  dst->list_of_cell_level_configuration_structures_for_event_trigger = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ev_trg_cell; ++i){
    list_add_tail(
        dst->list_of_cell_level_configuration_structures_for_event_trigger,
        create_ev_trg_cell_element(src.ev_trg_cell[i]),
        sizeof(list_of_cell_level_configuration_structures_for_event_trigger_element_t*)
    );
  }
  return dst;
}

static
event_trigger_definition_format_json_t* create_ev_trigger_def_3(e2sm_ccc_ev_trg_frmt_3_t const src){
  event_trigger_definition_format_json_t* dst = calloc(1, sizeof(event_trigger_definition_format_json_t));
  assert(dst != NULL && "Memory exhausted");
  dst->period = calloc(1, sizeof(uint64_t));
  *dst->period = (uint64_t)src.period;
  return dst;
}

byte_array_t ccc_enc_event_trigger_json(e2sm_ccc_event_trigger_t const* src)
{
  ric_event_trigger_definition_json_t* ric_ev_trigger = calloc(1, sizeof(ric_event_trigger_definition_json_t));
  assert(ric_ev_trigger != NULL && "Memory exhausted");
  defer({cJSON_Deleteric_event_trigger_definition(ric_ev_trigger); });

  if (src->format == FORMAT_1_E2SM_CCC_EV_TRIGGER_FORMAT) {
    ric_ev_trigger->event_trigger_definition_format = create_ev_trigger_def_1(src->frmt_1);
  } else if (src->format == FORMAT_2_E2SM_CCC_EV_TRIGGER_FORMAT){
    ric_ev_trigger->event_trigger_definition_format = create_ev_trigger_def_2(src->frmt_2);
  } else if (src->format == FORMAT_3_E2SM_CCC_EV_TRIGGER_FORMAT){
    ric_ev_trigger->event_trigger_definition_format = create_ev_trigger_def_3(src->frmt_3);
  } else {
    assert(0 != 0 && "unknown format type");
  }

  char * res = cJSON_Printric_event_trigger_definition(ric_ev_trigger);
  byte_array_t ba = {.len = strlen(res) + 1, .buf = (uint8_t *)res};
  return ba;

  return ba;
}

static
lst_act_def_ran_conf_element_json_t* create_act_def_ran_element(act_def_ran_conf_t const src){
  lst_act_def_ran_conf_element_json_t* res = calloc(1, sizeof(lst_act_def_ran_conf_element_json_t));
  assert(res != NULL && "Memory exhausted");
  res->ran_configuration_structure_name = copy_ba_to_str(&src.ran_conf_name);
  res->report_type = src.report_type;
  res->list_of_attributes = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_attribute; ++i){
    list_add_tail(res->list_of_attributes, copy_ba_to_str(&src.attribute[i].attribute_name), sizeof(char *));
  }

  return res;
}

static
action_definition_format_json_t* create_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t const src){
  action_definition_format_json_t* act_def_frm = calloc(1, sizeof(action_definition_format_json_t));
  assert(act_def_frm != NULL && "Memory exhausted");
  act_def_frm->lst_act_def_node_ran_conf = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_act_def_ran_conf; ++i){
    list_add_tail(
        act_def_frm->lst_act_def_node_ran_conf,
        create_act_def_ran_element(src.act_def_ran_conf[i]),
        sizeof(lst_act_def_ran_conf_element_json_t *)
        );
  }

  return act_def_frm;
}

static
lst_act_def_cell_ran_conf_element_json_t* create_act_def_cell_report(act_def_cell_report_t const src){
  lst_act_def_cell_ran_conf_element_json_t* res = calloc(1, sizeof(*res));
  assert(res != NULL && "Memory exhausted");
  res->cell_global_id = create_cell_global_id(src.cell_global_id);
  res->list_of_cell_level_ran_configuration_structures_for_adf = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_act_def_ran_conf; ++i){
    list_add_tail(
        res->list_of_cell_level_ran_configuration_structures_for_adf,
        create_act_def_ran_element(src.act_def_ran_conf[i]),
        sizeof(lst_act_def_ran_conf_element_json_t*)
        );
  }
  return res;
}

static
action_definition_format_json_t* create_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t const src){
  action_definition_format_json_t* act_def_frm = calloc(1, sizeof(action_definition_format_json_t));
  assert(act_def_frm != NULL && "Memory exhausted");
  act_def_frm->lst_act_def_cell_ran_conf = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_act_def_cell_report; ++i){
    list_add_tail(
        act_def_frm->lst_act_def_cell_ran_conf,
        create_act_def_cell_report(src.act_def_cell_report[i]),
        sizeof(lst_act_def_cell_ran_conf_element_json_t*)
    );
  }

  return act_def_frm;
}

byte_array_t ccc_enc_action_def_json(e2sm_ccc_action_def_t const* src)
{
  ric_action_definition_json_t* ric_act_def = calloc(1, sizeof(ric_action_definition_json_t));
  assert(ric_act_def != NULL && "Memory exhausted");
  defer({cJSON_Deleteric_action_definition(ric_act_def); });

  ric_act_def->ric_style_type = src->ric_style_type;
  if (src->format == FORMAT_1_E2SM_CCC_ACT_DEF) {
    ric_act_def->action_definition_format = create_act_def_frmt_1(src->frmt_1);
  } else if (src->format == FORMAT_2_E2SM_CCC_ACT_DEF){
    ric_act_def->action_definition_format = create_act_def_frmt_2(src->frmt_2);
  } else {
    assert(0 != 0 && "unknown format type");
  }

  char * res = cJSON_Printric_action_definition(ric_act_def);
  byte_array_t ba = {.len = strlen(res) + 1, .buf = (uint8_t *)res};
  return ba;
}

byte_array_t ccc_enc_ind_hdr_json(e2sm_ccc_ind_hdr_t const* src)
{
  static_assert(sizeof(indication_header_format_json_t) == sizeof(e2sm_ccc_ind_hdr_frmt_1_t), "Size not compatible");
  cJSON * cjson_out = cJSON_CreateObject();
  defer({cJSON_Delete(cjson_out); });

  if (src->format == FORMAT_1_E2SM_CCC_IND_HDR){
    cJSON_AddItemToObject(cjson_out,
                          "indicationHeaderFormat",
                          cJSON_Createindication_header_format((indication_header_format_json_t *)&src->frmt_1)
                          );
  } else {
    assert(0 != 0 && "unknown format type");
  };

  char * res = cJSON_PrintUnformatted(cjson_out);
  byte_array_t ba = {.len = strlen(res) + 1, .buf = (uint8_t *)res};
  return ba;
}

static
ran_configuration_structure_json_t* create_du_function_node(e2sm_ccc_o_gnb_du_function_t const src){
  ran_configuration_structure_json_t* dst = calloc(1, sizeof(ran_configuration_structure_json_t));
  assert(dst != NULL);

  dst->gnb_id = calloc(1, sizeof(uint32_t));
  assert(dst->gnb_id != NULL);
  *dst->gnb_id = src.gnb_id;

  if (src.gnb_du_name != NULL)
    dst->gnb_du_name = strdup(src.gnb_du_name);

  dst->gnb_du_id = calloc(1, sizeof(uint64_t));
  assert(dst->gnb_du_id != NULL);
  *dst->gnb_du_id = src.gnb_du_id;

  dst->gnb_id_length = calloc(1, sizeof(uint8_t));
  assert(dst->gnb_id_length!= NULL);
  *dst->gnb_id_length = src.gnb_id_len;

  return dst;
}

static
ran_configuration_structure_json_t* create_e2sm_ccc_o_ces(e2sm_ccc_o_ces_management_function_t const src){
  ran_configuration_structure_json_t* dst = calloc(1, sizeof(ran_configuration_structure_json_t));
  assert(dst != NULL);

  //  src.energy_saving_control
  dst->energy_saving_control = calloc(1, sizeof(energy_saving_control_e));
  assert(dst->energy_saving_control != NULL);
  *dst->energy_saving_control = src.energy_saving_control;

  //  src.energy_saving_state
  dst->energy_saving_state = calloc(1, sizeof(energy_saving_state_e));
  assert(dst->energy_saving_state != NULL);
  *dst->energy_saving_state = src.energy_saving_state;

  //  src.ces_switch
  dst->ces_switch = calloc(1, sizeof(ces_switch_e));
  assert(dst->ces_switch != NULL);
  *dst->ces_switch = src.ces_switch;

  return dst;
}

static
ran_configuration_structure_json_t* create_e2sm_ccc_nr_cell_cu(e2sm_ccc_o_nr_cell_cu_t const src){
  ran_configuration_structure_json_t* dst = calloc(1, sizeof(ran_configuration_structure_json_t));
  assert(dst != NULL);

//  src.cell_local_id
  dst->cell_local_id = calloc(1, sizeof(uint32_t));
  assert(dst->cell_local_id != NULL);
  *dst->cell_local_id = src.cell_local_id;

  //  src.plmn_info_lst
  //  src.sz_plmn_info_lst
  dst->plmn_info_list = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_plmn_info_lst; ++i){
    list_add_tail(
        dst->plmn_info_list,
        create_info_list_element(src.plmn_info_lst[i]),
        sizeof(plmn_info_list_element_t *)
    );
  }

  return dst;
}

static
ran_configuration_structure_json_t* create_e2sm_ccc_o_bwp(e2sm_ccc_o_bwp_t const src){
  ran_configuration_structure_json_t* dst = calloc(1, sizeof(ran_configuration_structure_json_t));
  assert(dst != NULL);

  //  src.start_rb
  dst->start_rb = calloc(1, sizeof(uint32_t));
  assert(dst->start_rb!= NULL);
  *dst->start_rb = src.start_rb;

  //  src.number_of_rbs
  dst->number_of_r_bs = calloc(1, sizeof(uint32_t));
  assert(dst->number_of_r_bs != NULL);
  *dst->number_of_r_bs = src.number_of_rbs;

  //  src.is_initial_bwp
  dst->is_initial_bwp = calloc(1, sizeof(is_initial_bwp_e));
  assert(dst->is_initial_bwp != NULL);
  *dst->is_initial_bwp = src.is_initial_bwp;

  //  src.cyclic_prefix
  dst->cyclic_prefix = calloc(1, sizeof(cyclic_prefix_e));
  assert(dst->cyclic_prefix != NULL);
  *dst->cyclic_prefix = src.cyclic_prefix;

  //  src.bwp_context
  dst->bwp_context = calloc(1, sizeof(bwp_context_e));
  assert(dst->bwp_context != NULL);
  *dst->bwp_context = src.bwp_context;

  //  src.sub_carrier_spacing
  dst->sub_carrier_spacing = calloc(1, sizeof(uint32_t));
  assert(dst->sub_carrier_spacing != NULL);
  switch (src.sub_carrier_spacing) {
    case SUB_CARRIER_SPACING_15:
      *dst->sub_carrier_spacing = 15;
      break;
    case SUB_CARRIER_SPACING_30:
      *dst->sub_carrier_spacing = 30;
      break;
    case SUB_CARRIER_SPACING_60:
      *dst->sub_carrier_spacing = 60;
      break;
    case SUB_CARRIER_SPACING_120:
      *dst->sub_carrier_spacing = 120;
      break;
    default:
      break;
  }

  return dst;
}

static
r_rm_policy_member_list_element_t* create_rrm_policy_member_list_member(e2sm_ccc_rrm_policy_member_t const src){
  r_rm_policy_member_list_element_t* dst = calloc(1, sizeof(r_rm_policy_member_list_element_t));
  assert(dst != NULL);

  dst->plmn_id = create_plmn_id_json(src.plmn_id);

  if (src.s_nssai!= NULL){
    dst->snssai = create_snssai_json(*src.s_nssai);
  }

  return dst;
}

static
ran_configuration_structure_json_t* create_rrm_policy_ratio(e2sm_ccc_o_rrm_policy_ratio_t const src){
  ran_configuration_structure_json_t* dst = calloc(1, sizeof(ran_configuration_structure_json_t));
  assert(dst != NULL);

  //  src.rrm_policy_min_ratio
  dst->r_rm_policy_min_ratio = calloc(1, sizeof(uint8_t));
  assert(dst->r_rm_policy_min_ratio!= NULL);
  *dst->r_rm_policy_min_ratio = src.rrm_policy_min_ratio;

  //  src.rrm_policy_max_ratio
  dst->r_rm_policy_max_ratio = calloc(1, sizeof(uint8_t));
  assert(dst->r_rm_policy_max_ratio!= NULL);
  *dst->r_rm_policy_max_ratio = src.rrm_policy_max_ratio;

  //  src.rrm_policy_dedicated_ratio
  dst->r_rm_policy_dedicated_ratio = calloc(1, sizeof(uint8_t));
  assert(dst->r_rm_policy_dedicated_ratio != NULL);
  *dst->r_rm_policy_dedicated_ratio = src.rrm_policy_dedicated_ratio;

  //  src.resource_type
  dst->resource_type = calloc(1, sizeof(resource_type_e));
  assert(dst->resource_type != NULL);
  *dst->resource_type = src.resource_type;

  //  src.rrm_policy_member_lst
  //  src.sz_rrm_policy_member_lst
  dst->r_rm_policy_member_list = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_rrm_policy_member_lst ; ++i){
    list_add_tail(
        dst->r_rm_policy_member_list,
        create_rrm_policy_member_list_member(src.rrm_policy_member_lst[i]),
        sizeof(r_rm_policy_member_list_element_t*)
    );
  }
  return dst;
}

static
ran_configuration_structure_json_t* create_o_gnb_cu_up_function(e2sm_ccc_o_gnb_cu_up_function_t const src){
  ran_configuration_structure_json_t* dst = calloc(1, sizeof(ran_configuration_structure_json_t));
  assert(dst != NULL);

  //  src.gnb_id
  dst->gnb_id = calloc(1, sizeof(uint32_t));
  assert(dst->gnb_id != NULL);
  *dst->gnb_id = src.gnb_id;

  //  src.gnb_id_len
  dst->gnb_id_length = calloc(1, sizeof(uint8_t));
  assert(dst->gnb_id_length!= NULL);
  *dst->gnb_id_length = src.gnb_id_len;

  //  src.gnb_cu_up_id
  dst->gnb_cu_up_id = calloc(1, sizeof(uint64_t));
  assert(dst->gnb_cu_up_id != NULL);
  *dst->gnb_cu_up_id = src.gnb_cu_up_id;

  //  src.plmn_info_lst
  //  src.sz_plmn_info_lst
  dst->plmn_info_list = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_plmn_info_lst; ++i){
    list_add_tail(
        dst->plmn_info_list,
        create_info_list_element(src.plmn_info_lst[i]),
        sizeof(plmn_info_list_element_t *)
    );
  }

  return dst;
}

static
list_t* create_g_enb_id_list_json(g_enb_id_lst_t const src){
  list_t* dst = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_g_enb_id_lst; ++i){
    list_add_tail(
        dst,
        strdup(src.g_enb_id_lst[i]),
        sizeof(char*)
    );
  }

  return dst;
}

static
list_t* create_g_gnb_id_list_json(g_gnb_id_lst_t const src){
  list_t* dst = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_g_gnb_id_lst; ++i){
    list_add_tail(
        dst,
        strdup(src.g_gnb_id_lst[i]),
        sizeof(char*)
    );
  }

  return dst;
}
static
ran_configuration_structure_json_t* create_o_gnb_cu_cp_function(e2sm_ccc_o_gnb_cu_cp_function_t const src){
  ran_configuration_structure_json_t* dst = calloc(1, sizeof(ran_configuration_structure_json_t));
  assert(dst != NULL);

//  src.gnb_id
  dst->gnb_id = calloc(1, sizeof(uint32_t));
  assert(dst->gnb_id != NULL);
  *dst->gnb_id = src.gnb_id;

//  src.gnb_cu_name
  if (src.gnb_cu_name != NULL)
    dst->gnb_cu_name = strdup(src.gnb_cu_name);

//  src.gnb_id_len
  dst->gnb_id_length = calloc(1, sizeof(uint8_t));
  assert(dst->gnb_id_length!= NULL);
  *dst->gnb_id_length = src.gnb_id_len;

//  src.plmn_id
  dst->plmn_id = create_plmn_id_json(src.plmn_id);

//  src.xn_ho_block_list
  if (src.xn_ho_block_list.sz_g_gnb_id_lst > 0)
    dst->xn_ho_block_list = create_g_gnb_id_list_json(src.xn_ho_block_list);

//  src.xn_allow_list
  if (src.xn_allow_list.sz_g_gnb_id_lst > 0)
    dst->xn_allow_list = create_g_gnb_id_list_json(src.xn_allow_list);

//  src.x2_ho_block_list
  if (src.x2_ho_block_list.sz_g_enb_id_lst > 0)
    dst->x2_ho_block_list = create_g_enb_id_list_json(src.x2_ho_block_list);

//  src.x2_allow_list
  if (src.x2_allow_list.sz_g_enb_id_lst > 0)
    dst->x2_allow_list = create_g_enb_id_list_json(src.x2_allow_list);

//  src.x2_block_list
  if (src.x2_block_list.sz_g_enb_id_lst > 0)
    dst->x2_block_list = create_g_enb_id_list_json(src.x2_block_list);

//  src.xn_block_list
  if (src.xn_block_list.sz_g_gnb_id_lst > 0)
    dst->xn_block_list = create_g_gnb_id_list_json(src.xn_block_list);

  return dst;
}

static
values_of_attributes_json_t* create_values_of_attributes(values_of_attributes_t const* src){
  assert(src != NULL);
  values_of_attributes_json_t* res = calloc(1, sizeof(values_of_attributes_json_t));
  assert(res != NULL);

  switch (src->values_of_attributes_type) {
    case VALUES_OF_ATTRIBUTES_O_GNBDUFunction:
      res->ran_configuration_structure = create_du_function_node(src->e2sm_ccc_o_gnb_du_function);
      break;
    case VALUES_OF_ATTRIBUTES_O_RRMPolicyRatio:
      res->ran_configuration_structure = create_rrm_policy_ratio(src->e2sm_ccc_o_rrm_policy_ratio);
      break;
    case VALUES_OF_ATTRIBUTES_O_GNBCUUPFunction:
      res->ran_configuration_structure = create_o_gnb_cu_up_function(src->e2sm_ccc_o_gnb_cu_up_function);
      break;
    case VALUES_OF_ATTRIBUTES_O_GNBCUCPFunction:
      res->ran_configuration_structure = create_o_gnb_cu_cp_function(src->e2sm_ccc_o_gnb_cu_cp_function);
      break;
    case VALUES_OF_ATTRIBUTES_O_CESManagementFunction:
      res->ran_configuration_structure = create_e2sm_ccc_o_ces(src->e2sm_ccc_o_ces_management_function);
      break;
    case VALUES_OF_ATTRIBUTES_O_BWP:
      res->ran_configuration_structure = create_e2sm_ccc_o_bwp(src->e2sm_ccc_o_bwp);
      break;
    case VALUES_OF_ATTRIBUTES_O_NRCellDU:
      assert(0 != 0 && "No support VALUES_OF_ATTRIBUTES_O_NRCellDU");
    case VALUES_OF_ATTRIBUTES_O_NRCellCU:
      res->ran_configuration_structure = create_e2sm_ccc_nr_cell_cu(src->e2sm_ccc_o_nr_cell_cu);
      break;
    default:
      assert(0 != 0 && "No support for current configuration structure name - node level");
  }
  return res;
}

static
list_of_configuration_structures_reported_element_t* create_ind_msg_ran_element(ind_msg_ran_conf_t const src){
  list_of_configuration_structures_reported_element_t* res = calloc(1, sizeof(list_of_configuration_structures_reported_element_t));
  assert(res != NULL && "Memory exhausted");
  res->change_type = src.change_type;
  res->ran_configuration_structure_name = copy_ba_to_str(&src.ran_conf_name);
  res->values_of_attributes = create_values_of_attributes(&src.vals_attributes);
  if (src.old_vals_attributes)
    res->old_values_of_attributes = create_values_of_attributes(src.old_vals_attributes);
  return res;
}

static
indication_message_format_json_t* create_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const src)
{
  indication_message_format_json_t* dst = calloc(1, sizeof(indication_message_format_json_t));
  assert(dst != NULL && "Memory exhausted");
  dst->list_of_configuration_structures_reported = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ind_msg_node_conf; ++i){
    list_add_tail(
        dst->list_of_configuration_structures_reported,
        create_ind_msg_ran_element(src.ind_msg_ran_conf[i]),
        sizeof(list_of_configuration_structures_reported_element_t*)
    );
  }

  return dst;
}

static
list_of_configuration_structure_control_element_t* create_ctrl_msg_ran_element(ctrl_msg_ran_conf_t const src){
  list_of_configuration_structure_control_element_t* res = calloc(1, sizeof(list_of_configuration_structure_control_element_t));
  assert(res != NULL && "Memory exhausted");
  res->ran_configuration_structure_name = copy_ba_to_str(&src.ran_conf_name);
  res->new_values_of_attributes = create_values_of_attributes(&src.vals_attributes);
  if (src.old_vals_attributes)
    res->old_values_of_attributes = create_values_of_attributes(src.old_vals_attributes);
  return res;
}

static
control_message_format_json_t* create_ctrl_msg_frmt_1(e2sm_ccc_ctrl_msg_frmt_1_t const src)
{
  control_message_format_json_t* dst = calloc(1, sizeof(control_message_format_json_t));
  assert(dst != NULL && "Memory exhausted");
  dst->list_of_configuration_structures = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ctrl_msg_ran_conf; ++i){
    list_add_tail(
        dst->list_of_configuration_structures,
        create_ctrl_msg_ran_element(src.ctrl_msg_ran_conf[i]),
        sizeof(list_of_configuration_structure_control_element_t*)
    );
  }

  return dst;
}

static
list_of_cells_reported_element_t* create_ind_msg_cell_report(ind_msg_cell_report_t const src){
  list_of_cells_reported_element_t* res = calloc(1, sizeof(list_of_cells_reported_element_t));
  assert(res != NULL && "Memory exhausted");
  res->cell_global_id = create_cell_global_id(src.cell_global_id);
  res->list_of_configuration_structures_reported = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ind_msg_ran_conf; ++i){
    list_add_tail(
        res->list_of_configuration_structures_reported,
        create_ind_msg_ran_element(src.ind_msg_ran_conf[i]),
        sizeof(list_of_configuration_structures_reported_element_t*)
    );
  }
  return res;
}

static
list_of_cells_controlled_element_t* create_ctrl_msg_cell(ctrl_msg_cell_t const src){
  list_of_cells_controlled_element_t* res = calloc(1, sizeof(list_of_cells_controlled_element_t));
  assert(res != NULL && "Memory exhausted");
  res->cell_global_id = create_cell_global_id(src.cell_global_id);
  res->list_of_configuration_structures = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ctrl_msg_ran_conf; ++i){
    list_add_tail(
        res->list_of_configuration_structures,
        create_ctrl_msg_ran_element(src.ctrl_msg_ran_conf[i]),
        sizeof(list_of_configuration_structure_control_element_t *)
    );
  }
  return res;
}

static
indication_message_format_json_t* create_ind_msg_frmt_2(e2sm_ccc_ind_msg_frmt_2_t const src)
{
  indication_message_format_json_t* dst = calloc(1, sizeof(indication_message_format_json_t));
  assert(dst != NULL && "Memory exhausted");
  dst->list_of_cells_reported = list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ind_msg_cell_report; ++i){
    list_add_tail(
        dst->list_of_cells_reported,
        create_ind_msg_cell_report(src.ind_msg_cell_report[i]),
        sizeof(list_of_cells_reported_element_t*)
    );
  }

  return dst;
}

static
control_message_format_json_t* create_ctrl_msg_frmt_2(e2sm_ccc_ctrl_msg_frmt_2_t const src)
{
  control_message_format_json_t * dst = calloc(1, sizeof(control_message_format_json_t));
  assert(dst != NULL && "Memory exhausted");
  dst->list_of_cells_controlled= list_create(false, NULL);
  for (size_t i = 0; i < src.sz_ctrl_msg_cell; ++i){
    list_add_tail(
        dst->list_of_cells_controlled,
        create_ctrl_msg_cell(src.ctrl_msg_cell[i]),
        sizeof(list_of_cells_controlled_element_t*)
    );
  }

  return dst;
}


byte_array_t ccc_enc_ind_msg_json(e2sm_ccc_ind_msg_t const* src)
{
  ric_indication_message_json_t* ric_ind_msg = calloc(1, sizeof(ric_indication_message_json_t));
  assert(ric_ind_msg != NULL && "Memory exhausted");
  defer({cJSON_Deleteric_indication_message(ric_ind_msg); });

  if (src->format == FORMAT_1_E2SM_CCC_IND_MSG) {
    ric_ind_msg->indication_message_format = create_ind_msg_frmt_1(src->frmt_1);
  } else if (src->format == FORMAT_2_E2SM_CCC_IND_MSG){
    ric_ind_msg->indication_message_format = create_ind_msg_frmt_2(src->frmt_2);
  } else {
    assert(0 != 0 && "unknown format type");
  }

  char * res = cJSON_Printric_indication_message(ric_ind_msg);
  byte_array_t ba = {.len = strlen(res) + 1, .buf = (uint8_t *)res};
  return ba;
}

byte_array_t ccc_enc_cpid_json(e2sm_ccc_cpid_t const* src)
{
  assert(0 != 0 && "Not implemented");
  byte_array_t ba = {0};

  return ba;
}

byte_array_t ccc_enc_ctrl_hdr_json(e2sm_ccc_ctrl_hdr_t const* src)
{
  assert(0 != 0 && "Not implemented");
  byte_array_t ba = {0};

  return ba;
}

byte_array_t ccc_enc_ctrl_msg_json(e2sm_ccc_ctrl_msg_t const* src)
{
  ric_control_message_json_t* ric_ctrl_msg = calloc(1, sizeof(ric_control_message_json_t));
  assert(ric_ctrl_msg != NULL && "Memory exhausted");
  defer({cJSON_Deleteric_control_message(ric_ctrl_msg); });

  if (src->format == FORMAT_1_E2SM_CCC_CTRL_MSG) {
    ric_ctrl_msg->control_message_format = create_ctrl_msg_frmt_1(src->frmt_1);
  } else if (src->format == FORMAT_2_E2SM_CCC_CTRL_MSG){
    ric_ctrl_msg->control_message_format = create_ctrl_msg_frmt_2(src->frmt_2);
  } else {
    assert(0 != 0 && "unknown format type");
  }

  char * res = cJSON_Printric_control_message(ric_ctrl_msg);
  byte_array_t ba = {.len = strlen(res) + 1, .buf = (uint8_t *)res};
  return ba;
}

byte_array_t ccc_enc_ctrl_out_json(e2sm_ccc_ctrl_out_t const* src)
{
  assert(0 != 0 && "Not implemented");
  byte_array_t ba = {0};

  return ba;
}


//static
//list_of_supported_control_style_element_t* create_sp_control_style(lst_control_service_ccc_t const src){
//  list_of_supported_control_style_element_t* dst = calloc(1, sizeof(dst*));
//  dst->attribute_name = src.
//  return dst;
//}
//
//static
//control_service_json_t* create_control_service_json(control_service_ccc_t const src)
//{
//  control_service_json_t* dst = calloc(1, sizeof(dst*));
//  assert(dst != NULL && "Memory exhausted");
//  dst->list_of_supported_control_styles = list_create(false, NULL);
//  for (size_t i = 0; i < src.sz_lst_control_service_ccc; i++){
//    list_add_tail(
//        dst->list_of_supported_control_styles,
//        create_sp_control_style(src.lst_control_service_ccc[i]),
//        sizeof(list_of_supported_control_style_element_t *)
//        );
//  }
//
//  return dst;
//}
//
//static
//list_of_supported_attribute_element_t* create_sp_attribute_element(lst_sp_attributes_t const src){
//  list_of_supported_attribute_element_t* dst = calloc(1, sizeof(dst*));
//  assert(dst != NULL && "Memory exhausted");
//  dst->attribute_name = copy_ba_to_str(src.attribute_name);
//  dst->supported_services = calloc(1, sizeof(supported_services_json_t));
//  assert(dst->supported_services != NULL && "Memory exhausted");
//
//  if (src.supported_services.control_service_ccc != NULL)
//    dst->supported_services->control_service = create_control_service_json(src.supported_services.control_service_ccc);
//
//  if (src.supported_services.insert_service_ccc != NULL)
//    assert(0 != 0 && "Not implemented");
//
//  if (src.supported_services.report_service_ccc!= NULL)
//    dst->supported_services->report_service = create_ind_msg_ran_element();
//
//  if (src.supported_services.ev_trigger_ccc!= NULL)
//    dst->supported_services->event_trigger = create_ind_msg_ran_element();
//
//  if (src.supported_services.policy_service_ccc!= NULL)
//    assert(0 != 0 && "Not implemented");
//}
//
//static
//list_of_supported_node_level_configuration_structure_element_t* create_func_def_ran_conf_element(func_def_ran_conf_t const src){
//  list_of_supported_node_level_configuration_structure_element_t* dst = calloc(1, sizeof(dst*));
//  assert(dst != NULL && "Memory exhausted");
//  dst->ran_configuration_structure_name = copy_ba_to_str(src.ran_conf_str_name);
//  if (src.sz_lst_sp_attributes > 0){
//    dst->list_of_supported_attributes = list_create(false, NULL);
//    for (size_t i = 0; i < src.sz_lst_sp_attributes; i++){
//      list_add_tail(
//          dst->list_of_supported_attributes,
//          create_sp_attribute_element(src.lst_sp_attributes[i]),
//          sizeof(list_of_supported_attribute_element_t *)
//          )
//    }
//  }
//  return dst;
//}

static
ran_function_name_json_t create_ran_function_name_json(ran_function_name_t const* src){
  assert(src != NULL);
  ran_function_name_json_t dst = {0};
  dst.ran_function_service_model_oid = copy_ba_to_str(&src->oid);
  dst.ran_function_description = copy_ba_to_str(&src->description);
  dst.ran_function_short_name = copy_ba_to_str(&src->name);
  if (src->instance != NULL)
    memcpy(dst.ran_function_instance, src->instance, sizeof(uint64_t));

  return dst;
}

byte_array_t ccc_enc_func_def_json(e2sm_ccc_func_def_t const* src)
{
  assert(src != NULL);
  ric_function_definition_json_t* ric_func_def = calloc(1, sizeof(ric_function_definition_json_t));
  assert(ric_func_def != NULL && "Memory exhausted");
  defer({cJSON_Deleteric_function_definition(ric_func_def); });

  ric_func_def->ran_function_name = calloc(1, sizeof(ran_function_name_json_t));
  assert(ric_func_def->ran_function_name != NULL && "Memory exhausted");
  *ric_func_def->ran_function_name = create_ran_function_name_json(&src->name);

  if(src->sz_func_def_ran_conf > 0)
    assert(0 != 0 && "Not implemented");

  if(src->sz_func_def_cells > 0)
    assert(0 != 0 && "Not implemented");

  char * res = cJSON_Printric_function_definition(ric_func_def);
  byte_array_t ba = {.len = strlen(res) + 1, .buf = (uint8_t *)res};

  return ba;
}

