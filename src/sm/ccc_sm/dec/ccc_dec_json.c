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
#include "../../../util/alg_ds/alg/defer.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

e2sm_ccc_event_trigger_t ccc_dec_event_trigger_json(size_t len, uint8_t const buf[len])
{
  assert(buf != NULL);
  assert(len != 0);
  assert(0 != 0 && "Not implemented");

  e2sm_ccc_event_trigger_t dst = {0};

  return dst;
}

static
attribute_t* get_lst_attribute(list_t* const src){
  assert(src != NULL);
  attribute_t* res = calloc(src->count, sizeof(attribute_t));
  assert(res != NULL && "Memory exhausted");

  size_t index = 0;
  lst_attribute_element_json_t* attribute = list_get_head(src);
  while (attribute != NULL){
    res[index].attribute_name = cp_str_to_ba(attribute->attribute_name);
    attribute = list_get_next(src);
    index++;
  }

  return res;
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
    res[index].sz_attribute = ran_conf->list_of_attributes->count;
    if (ran_conf->list_of_attributes != 0 && ran_conf->list_of_attributes->count > 0)
      res[index].attribute = get_lst_attribute(ran_conf->list_of_attributes);
    ran_conf = list_get_next(src);
    index++;
  }

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
    res[index].vals_attributes = cp_str_to_ba(ran_conf->values_of_attributes);
    res[index].old_vals_attributes = cp_str_to_ba(ran_conf->old_values_of_attributes);
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
cell_global_id_t get_cell_global_id(cell_global_id_json_t const* src){
  assert(src != NULL);
  cell_global_id_t res = {0};
  res.type = NR_CGI_RAT_TYPE;
  res.nr_cgi.nr_cell_id = atoi(src->n_r_cell_identity);
  res.nr_cgi.plmn_id.mnc = atoi(src->plmn_identity->mnc);
  res.nr_cgi.plmn_id.mcc = atoi(src->plmn_identity->mcc);
  res.nr_cgi.plmn_id.mnc_digit_len = strlen(src->plmn_identity->mnc);
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

e2sm_ccc_func_def_t ccc_dec_func_def_json(size_t len, uint8_t const func_def[len])
{
  assert(func_def != NULL);
  assert(len > 0);
  assert(0 != 0 && "Not implemented");
  e2sm_ccc_func_def_t dst = {0};

  return dst;
}

