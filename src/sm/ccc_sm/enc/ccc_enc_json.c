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
#include "../ie/json/ric_indication_header_json.h"
#include "../ie/json/ric_action_definition_json.h"
#include "../ie/json/ric_indication_message_json.h"

#include "../../../util/alg_ds/alg/defer.h"

#include <assert.h>

byte_array_t ccc_enc_event_trigger_json(e2sm_ccc_event_trigger_t const* src)
{
  assert(0 != 0 && "Not implemented");
  byte_array_t ba = {0};

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
cell_global_id_json_t* create_cell_global_id(cell_global_id_t const src){
  cell_global_id_json_t* cell_id = calloc(1, sizeof(cell_global_id_json_t));
  assert(cell_id != NULL && "Memory exhausted");

  int length = snprintf(NULL, 0, "%lu", (uint64_t)src.nr_cgi.nr_cell_id);
  cell_id->n_r_cell_identity = (char *)malloc((length + 1) * sizeof(char));
  assert(cell_id->n_r_cell_identity != NULL && "Memory exhausted");
  snprintf(cell_id->n_r_cell_identity, length + 1, "%lu", (uint64_t)src.nr_cgi.nr_cell_id);

  cell_id->plmn_identity = calloc(1, sizeof(plmn_id_json_t));
  assert(cell_id->plmn_identity != NULL && "Memory exhausted");

  length = snprintf(NULL, 0, "%hu", src.nr_cgi.plmn_id.mcc);
  cell_id->plmn_identity->mcc = (char *)malloc((length + 1) * sizeof(char));
  assert(cell_id->plmn_identity->mcc != NULL && "Memory exhausted");
  snprintf(cell_id->plmn_identity->mcc, length + 1, "%hu", src.nr_cgi.plmn_id.mcc);

  length = snprintf(NULL, 0, "%hu", src.nr_cgi.plmn_id.mnc);
  cell_id->plmn_identity->mnc = (char *)malloc((length + 1) * sizeof(char));
  assert(cell_id->plmn_identity->mnc != NULL && "Memory exhausted");
  snprintf(cell_id->plmn_identity->mnc, length + 1, "%hu", src.nr_cgi.plmn_id.mnc);

  return cell_id;
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
list_of_configuration_structures_reported_element_t* create_ind_msg_ran_element(ind_msg_ran_conf_t const src){
  list_of_configuration_structures_reported_element_t* res = calloc(1, sizeof(list_of_configuration_structures_reported_element_t));
  assert(res != NULL && "Memory exhausted");
  res->change_type = src.change_type;
  res->ran_configuration_structure_name = copy_ba_to_str(&src.ran_conf_name);
  res->old_values_of_attributes = copy_ba_to_str(&src.old_vals_attributes);
  res->values_of_attributes = copy_ba_to_str(&src.vals_attributes);
  return res;
}


static
indication_message_format_t* create_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const src)
{
  indication_message_format_t* dst = calloc(1, sizeof(indication_message_format_t));
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
indication_message_format_t* create_ind_msg_frmt_2(e2sm_ccc_ind_msg_frmt_2_t const src)
{
  indication_message_format_t* dst = calloc(1, sizeof(indication_message_format_t));
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
  assert(0 != 0 && "Not implemented");
  byte_array_t ba = {0};

  return ba;
}

byte_array_t ccc_enc_ctrl_out_json(e2sm_ccc_ctrl_out_t const* src)
{
  assert(0 != 0 && "Not implemented");
  byte_array_t ba = {0};

  return ba;
}

byte_array_t ccc_enc_func_def_json(e2sm_ccc_func_def_t const* src)
{
  assert(0 != 0 && "Not implemented");
  byte_array_t ba = {0};

  return ba;
}

