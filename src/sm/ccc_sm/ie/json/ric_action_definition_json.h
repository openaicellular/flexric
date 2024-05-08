#ifndef JSON_CCC_RIC_ACTION_DEFINITION_H
#define JSON_CCC_RIC_ACTION_DEFINITION_H

#include <cJSON.h>
#include <stdint.h>
#include <list.h>
#include "../ir/report_type.h"
#include "cell_global_id_json.h"

typedef struct {
  char * attribute_name;
} lst_attribute_element_json_t;

typedef struct {
  list_t * list_of_attributes;
  char * ran_configuration_structure_name;
  report_type_e report_type;
} lst_act_def_ran_conf_element_json_t;

typedef struct {
  cell_global_id_json_t * cell_global_id;
  list_t * list_of_cell_level_ran_configuration_structures_for_adf;
} lst_act_def_cell_ran_conf_element_json_t;

typedef struct {
  list_t * lst_act_def_node_ran_conf;
  list_t * lst_act_def_cell_ran_conf;
} action_definition_format_json_t;

typedef struct{
  action_definition_format_json_t * action_definition_format;
  uint32_t ric_style_type;
} ric_action_definition_json_t;

report_type_e cJSON_Getreport_typeValue(const cJSON * j);
cJSON * cJSON_Createreport_type(const report_type_e x);

lst_attribute_element_json_t * cJSON_Parselist_of_attribute_element(const char * s);
lst_attribute_element_json_t * cJSON_Getlist_of_attribute_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_attribute_element(const lst_attribute_element_json_t * x);
char * cJSON_Printlist_of_attribute_element(const lst_attribute_element_json_t * x);
void cJSON_Deletelist_of_attribute_element(lst_attribute_element_json_t * x);

lst_act_def_ran_conf_element_json_t * cJSON_Parselst_act_def_node_ran_conf_element(const char * s);
lst_act_def_ran_conf_element_json_t * cJSON_Getlst_act_def_node_ran_conf_elementValue(const cJSON * j);
cJSON * cJSON_Createlst_act_def_node_ran_conf_element(const lst_act_def_ran_conf_element_json_t * x);
char * cJSON_Printlst_act_def_node_ran_conf_element(const lst_act_def_ran_conf_element_json_t * x);
void cJSON_Deletelst_act_def_node_ran_conf_element(lst_act_def_ran_conf_element_json_t * x);

lst_act_def_cell_ran_conf_element_json_t * cJSON_Parselst_act_def_cell_ran_conf_element(const char * s);
lst_act_def_cell_ran_conf_element_json_t * cJSON_Getlst_act_def_cell_ran_conf_elementValue(const cJSON * j);
cJSON * cJSON_Createlst_act_def_cell_ran_conf_element(const lst_act_def_cell_ran_conf_element_json_t * x);
char * cJSON_Printlst_act_def_cell_ran_conf_element(const lst_act_def_cell_ran_conf_element_json_t * x);
void cJSON_Deletelst_act_def_cell_ran_conf_element(lst_act_def_cell_ran_conf_element_json_t * x);

action_definition_format_json_t * cJSON_Parseaction_definition_format(const char * s);
action_definition_format_json_t * cJSON_Getaction_definition_formatValue(const cJSON * j);
cJSON * cJSON_Createaction_definition_format(const action_definition_format_json_t * x);
char * cJSON_Printaction_definition_format(const action_definition_format_json_t * x);
void cJSON_Deleteaction_definition_format(action_definition_format_json_t * x);

ric_action_definition_json_t * cJSON_Parseric_action_definition(const char * s);
ric_action_definition_json_t * cJSON_Getric_action_definitionValue(const cJSON * j);
cJSON * cJSON_Createric_action_definition(const ric_action_definition_json_t * x);
char * cJSON_Printric_action_definition(const ric_action_definition_json_t * x);
void cJSON_Deleteric_action_definition(ric_action_definition_json_t * x);


#endif
