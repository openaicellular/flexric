#ifndef FLEXRIC_RIC_INDICATION_MESSAGE_JSON_H
#define FLEXRIC_RIC_INDICATION_MESSAGE_JSON_H

#include <cJSON.h>
#include <list.h>
#include <stdbool.h>
#include "plmn_id_json.h"
#include "cell_global_id_json.h"
#include "plmn_id_json.h"
#include "../ir/change_type.h"
#include "../ir/e2sm_ccc_o_rrm_policy_ratio.h"
#include "../ir/e2sm_ccc_o_nr_cell_du.h"
#include "../ir/e2sm_ccc_o_ces_management_function.h"
#include "../ir/e2sm_ccc_o_bwp.h"

typedef struct {
  bwp_context_e * bwp_context;
  cyclic_prefix_e * cyclic_prefix;
  is_initial_bwp_e * is_initial_bwp;
  int64_t * number_of_r_bs;
  int64_t * start_rb;
  int64_t * sub_carrier_spacing;
} bwp_list_element_t;

typedef struct {
  char * sd;
  uint8_t * sst;
} snssai_json_t;

typedef struct {
  list_t * the_5_qi_list;
  plmn_id_json_t * plmn_id;
  snssai_json_t * snssai;
} partition_flow_list_element_t;

typedef struct {
  list_t * partition_flow_list;
  int64_t * p_number_of_r_bs;
  int64_t * p_offset_to_point_a;
} partition_list_element_t ;

typedef struct {
  plmn_id_json_t * plmn_id;
  snssai_json_t * snssai;
} plmn_info_list_element_t;

typedef struct {
  plmn_id_json_t * plmn_id;
  snssai_json_t * snssai;
} r_rm_policy_member_list_element_t;

typedef struct {
  char * gnb_cu_name;
  uint32_t* gnb_id;
  uint8_t* gnb_id_length;
  plmn_id_json_t * plmn_id;
  list_t * x2_allow_list;
  list_t * x2_block_list;
  list_t * x2_ho_block_list;
  list_t * xn_allow_list;
  list_t * xn_block_list;
  list_t * xn_ho_block_list;
  uint64_t * gnb_cu_up_id;
  list_t * plmn_info_list;
  uint64_t * gnb_du_id;
  char * gnb_du_name;
  int64_t * cell_local_id;
  administrative_state_e * administrative_state;
  int64_t * arfcn_dl;
  int64_t * arfcn_sul;
  int64_t * arfcn_ul;
  int64_t * b_s_channel_bw_dl;
  int64_t * b_s_channel_bw_sul;
  int64_t * b_s_channel_bw_ul;
  list_t * bwp_list;
  cell_state_e * cell_state;
  int64_t * nr_pci;
  int64_t * nr_tac;
  operational_state_e * operational_state;
  list_t * partition_list;
  int64_t * ssb_duration;
  int64_t * ssb_frequency;
  int64_t * ssb_offset;
  int64_t * ssb_periodicity;
  int64_t * ssb_sub_carrier_spacing;
  resource_type_e * resource_type;
  uint8_t * r_rm_policy_dedicated_ratio;
  uint8_t * r_rm_policy_max_ratio;
  list_t * r_rm_policy_member_list;
  uint8_t * r_rm_policy_min_ratio;
  bwp_context_e * bwp_context;
  cyclic_prefix_e * cyclic_prefix;
  is_initial_bwp_e * is_initial_bwp;
  int64_t * number_of_r_bs;
  int64_t * start_rb;
  int64_t * sub_carrier_spacing;
  bool * ces_switch;
  energy_saving_control_e * energy_saving_control;
  energy_saving_state_e * energy_saving_state;
} ran_configuration_structure_json_t;

typedef struct {
  ran_configuration_structure_json_t * ran_configuration_structure;
}values_of_attributes_json_t;

typedef struct {
  change_type_e change_type;
  char * ran_configuration_structure_name;
  values_of_attributes_json_t * values_of_attributes;
  values_of_attributes_json_t * old_values_of_attributes;
}list_of_configuration_structures_reported_element_t ;

typedef struct{
  cell_global_id_json_t * cell_global_id;
  list_t * list_of_configuration_structures_reported;
} list_of_cells_reported_element_t;

typedef struct{
  list_t * list_of_configuration_structures_reported;
  list_t * list_of_cells_reported;
} indication_message_format_json_t;

typedef struct {
  indication_message_format_json_t * indication_message_format;
} ric_indication_message_json_t ;

change_type_e cJSON_Getchange_typeValue(const cJSON * j);
cJSON * cJSON_Createchange_type(const change_type_e x);

administrative_state_e cJSON_Getadministrative_stateValue(const cJSON * j);
cJSON * cJSON_Createadministrative_state(const administrative_state_e x);

bwp_context_e cJSON_Getbwp_contextValue(const cJSON * j);
cJSON * cJSON_Createbwp_context(const bwp_context_e x);

cyclic_prefix_e cJSON_Getcyclic_prefixValue(const cJSON * j);
cJSON * cJSON_Createcyclic_prefix(const cyclic_prefix_e x);

is_initial_bwp_e cJSON_Getis_initial_bwpValue(const cJSON * j);
cJSON * cJSON_Createis_initial_bwp(const is_initial_bwp_e x);

cell_state_e cJSON_Getcell_stateValue(const cJSON * j);
cJSON * cJSON_Createcell_state(const cell_state_e x);

energy_saving_control_e cJSON_Getenergy_saving_controlValue(const cJSON * j);
cJSON * cJSON_Createenergy_saving_control(const energy_saving_control_e x);

energy_saving_state_e cJSON_Getenergy_saving_stateValue(const cJSON * j);
cJSON * cJSON_Createenergy_saving_state(const energy_saving_state_e x);

operational_state_e cJSON_Getoperational_stateValue(const cJSON * j);
cJSON * cJSON_Createoperational_state(const operational_state_e x);

resource_type_e cJSON_Getresource_typeValue(const cJSON * j);
cJSON * cJSON_Createresource_type(const resource_type_e x);

cell_global_id_json_t * cJSON_Parsecell_global_id(const char * s);
cell_global_id_json_t * cJSON_Getcell_global_idValue(const cJSON * j);
cJSON * cJSON_Createcell_global_id(const cell_global_id_json_t * x);
char * cJSON_Printcell_global_id(const cell_global_id_json_t * x);
void cJSON_Deletecell_global_id(cell_global_id_json_t * x);

bwp_list_element_t * cJSON_Parsebwp_list_element(const char * s);
bwp_list_element_t * cJSON_Getbwp_list_elementValue(const cJSON * j);
cJSON * cJSON_Createbwp_list_element(const bwp_list_element_t * x);
char * cJSON_Printbwp_list_element(const bwp_list_element_t * x);
void cJSON_Deletebwp_list_element(bwp_list_element_t * x);

snssai_json_t * cJSON_Parsesnssai(const char * s);
snssai_json_t * cJSON_GetsnssaiValue(const cJSON * j);
cJSON * cJSON_Createsnssai(const snssai_json_t * x);
char * cJSON_Printsnssai(const snssai_json_t * x);
void cJSON_Deletesnssai(snssai_json_t * x);

partition_flow_list_element_t * cJSON_Parsepartition_flow_list_element(const char * s);
partition_flow_list_element_t * cJSON_Getpartition_flow_list_elementValue(const cJSON * j);
cJSON * cJSON_Createpartition_flow_list_element(const partition_flow_list_element_t * x);
char * cJSON_Printpartition_flow_list_element(const partition_flow_list_element_t * x);
void cJSON_Deletepartition_flow_list_element(partition_flow_list_element_t * x);

partition_list_element_t * cJSON_Parsepartition_list_element(const char * s);
partition_list_element_t * cJSON_Getpartition_list_elementValue(const cJSON * j);
cJSON * cJSON_Createpartition_list_element(const partition_list_element_t * x);
char * cJSON_Printpartition_list_element(const partition_list_element_t * x);
void cJSON_Deletepartition_list_element(partition_list_element_t * x);

plmn_info_list_element_t * cJSON_Parseplmn_info_list_element(const char * s);
plmn_info_list_element_t * cJSON_Getplmn_info_list_elementValue(const cJSON * j);
cJSON * cJSON_Createplmn_info_list_element(const plmn_info_list_element_t * x);
char * cJSON_Printplmn_info_list_element(const plmn_info_list_element_t * x);
void cJSON_Deleteplmn_info_list_element(plmn_info_list_element_t * x);

r_rm_policy_member_list_element_t * cJSON_Parser_rm_policy_member_list_element(const char * s);
r_rm_policy_member_list_element_t * cJSON_Getr_rm_policy_member_list_elementValue(const cJSON * j);
cJSON * cJSON_Creater_rm_policy_member_list_element(const r_rm_policy_member_list_element_t * x);
char * cJSON_Printr_rm_policy_member_list_element(const r_rm_policy_member_list_element_t * x);
void cJSON_Deleter_rm_policy_member_list_element(r_rm_policy_member_list_element_t * x);

ran_configuration_structure_json_t * cJSON_Parseran_configuration_structure(const char * s);
ran_configuration_structure_json_t * cJSON_Getran_configuration_structureValue(const cJSON * j);
cJSON * cJSON_Createran_configuration_structure(const ran_configuration_structure_json_t * x);
char * cJSON_Printran_configuration_structure(const ran_configuration_structure_json_t * x);
void cJSON_Deleteran_configuration_structure(ran_configuration_structure_json_t * x);

values_of_attributes_json_t * cJSON_Parseold_values_of_attributes(const char * s);
values_of_attributes_json_t * cJSON_Getold_values_of_attributesValue(const cJSON * j);
cJSON * cJSON_Createold_values_of_attributes(const values_of_attributes_json_t * x);
char * cJSON_Printold_values_of_attributes(const values_of_attributes_json_t * x);
void cJSON_Deleteold_values_of_attributes(values_of_attributes_json_t * x);

list_of_configuration_structures_reported_element_t * cJSON_Parselist_of_configuration_structures_reported_element(const char * s);
list_of_configuration_structures_reported_element_t * cJSON_Getlist_of_configuration_structures_reported_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_configuration_structures_reported_element(const list_of_configuration_structures_reported_element_t * x);
char * cJSON_Printlist_of_configuration_structures_reported_element(const list_of_configuration_structures_reported_element_t * x);
void cJSON_Deletelist_of_configuration_structures_reported_element(list_of_configuration_structures_reported_element_t * x);

list_of_cells_reported_element_t * cJSON_Parselist_of_cells_reported_element(const char * s);
list_of_cells_reported_element_t * cJSON_Getlist_of_cells_reported_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_cells_reported_element(const list_of_cells_reported_element_t * x);
char * cJSON_Printlist_of_cells_reported_element(const list_of_cells_reported_element_t * x);
void cJSON_Deletelist_of_cells_reported_element(list_of_cells_reported_element_t * x);

indication_message_format_json_t * cJSON_Parseindication_message_format(const char * s);
indication_message_format_json_t * cJSON_Getindication_message_formatValue(const cJSON * j);
cJSON * cJSON_Createindication_message_format(const indication_message_format_json_t * x);
char * cJSON_Printindication_message_format(const indication_message_format_json_t * x);
void cJSON_Deleteindication_message_format(indication_message_format_json_t * x);

ric_indication_message_json_t * cJSON_Parseric_indication_message(const char * s);
ric_indication_message_json_t * cJSON_Getric_indication_messageValue(const cJSON * j);
cJSON * cJSON_Createric_indication_message(const ric_indication_message_json_t * x);
char * cJSON_Printric_indication_message(const ric_indication_message_json_t * x);
void cJSON_Deleteric_indication_message(ric_indication_message_json_t * x);


#endif
