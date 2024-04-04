#ifndef JSON_CCC_RIC_ACTION_DEFINITION_H
#define JSON_CCC_RIC_ACTION_DEFINITION_H

#include <cJSON.h>
#include <stdint.h>
#include <list.h>
#include "../ir/report_type.h"

struct plmn_identity {
  char * mcc;
  char * mnc;
};

struct cell_global_id {
  char * n_r_cell_identity;
  struct plmn_identity * plmn_identity;
};

typedef struct {
  char * attribute_name;
} list_of_attribute_element_t;

typedef struct {
  list_t * list_of_attributes;
  char * ran_configuration_structure_name;
  report_type_e report_type;
} lst_act_def_ran_conf_element_t;

typedef struct {
  struct cell_global_id * cell_global_id;
  list_t * list_of_cell_level_ran_configuration_structures_for_adf;
} lst_act_def_cell_ran_conf_element_t;

typedef struct {
  list_t * lst_act_def_node_ran_conf;
  list_t * lst_act_def_cell_ran_conf;
} action_definition_format_t;

typedef struct{
  action_definition_format_t * action_definition_format;
  uint32_t ric_style_type;
} ric_action_definition_t;

report_type_e cJSON_Getreport_typeValue(const cJSON * j);
cJSON * cJSON_Createreport_type(const report_type_e x);

struct plmn_identity * cJSON_Parseplmn_identity(const char * s);
struct plmn_identity * cJSON_Getplmn_identityValue(const cJSON * j);
cJSON * cJSON_Createplmn_identity(const struct plmn_identity * x);
char * cJSON_Printplmn_identity(const struct plmn_identity * x);
void cJSON_Deleteplmn_identity(struct plmn_identity * x);

struct cell_global_id * cJSON_Parsecell_global_id(const char * s);
struct cell_global_id * cJSON_Getcell_global_idValue(const cJSON * j);
cJSON * cJSON_Createcell_global_id(const struct cell_global_id * x);
char * cJSON_Printcell_global_id(const struct cell_global_id * x);
void cJSON_Deletecell_global_id(struct cell_global_id * x);

list_of_attribute_element_t * cJSON_Parselist_of_attribute_element(const char * s);
list_of_attribute_element_t * cJSON_Getlist_of_attribute_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_attribute_element(const list_of_attribute_element_t * x);
char * cJSON_Printlist_of_attribute_element(const list_of_attribute_element_t * x);
void cJSON_Deletelist_of_attribute_element(list_of_attribute_element_t * x);

lst_act_def_ran_conf_element_t * cJSON_Parselst_act_def_node_ran_conf_element(const char * s);
lst_act_def_ran_conf_element_t * cJSON_Getlst_act_def_node_ran_conf_elementValue(const cJSON * j);
cJSON * cJSON_Createlst_act_def_node_ran_conf_element(const lst_act_def_ran_conf_element_t * x);
char * cJSON_Printlst_act_def_node_ran_conf_element(const lst_act_def_ran_conf_element_t * x);
void cJSON_Deletelst_act_def_node_ran_conf_element(lst_act_def_ran_conf_element_t * x);

lst_act_def_cell_ran_conf_element_t * cJSON_Parselst_act_def_cell_ran_conf_element(const char * s);
lst_act_def_cell_ran_conf_element_t * cJSON_Getlst_act_def_cell_ran_conf_elementValue(const cJSON * j);
cJSON * cJSON_Createlst_act_def_cell_ran_conf_element(const lst_act_def_cell_ran_conf_element_t * x);
char * cJSON_Printlst_act_def_cell_ran_conf_element(const lst_act_def_cell_ran_conf_element_t * x);
void cJSON_Deletelst_act_def_cell_ran_conf_element(lst_act_def_cell_ran_conf_element_t * x);

action_definition_format_t * cJSON_Parseaction_definition_format(const char * s);
action_definition_format_t * cJSON_Getaction_definition_formatValue(const cJSON * j);
cJSON * cJSON_Createaction_definition_format(const action_definition_format_t * x);
char * cJSON_Printaction_definition_format(const action_definition_format_t * x);
void cJSON_Deleteaction_definition_format(action_definition_format_t * x);

ric_action_definition_t * cJSON_Parseric_action_definition(const char * s);
ric_action_definition_t * cJSON_Getric_action_definitionValue(const cJSON * j);
cJSON * cJSON_Createric_action_definition(const ric_action_definition_t * x);
char * cJSON_Printric_action_definition(const ric_action_definition_t * x);
void cJSON_Deleteric_action_definition(ric_action_definition_t * x);


#endif
