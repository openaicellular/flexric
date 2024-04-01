#ifndef JSON_CCC_RIC_ACTION_DEFINITION_H
#define JSON_CCC_RIC_ACTION_DEFINITION_H

#include <cJSON.h>
#include <stdint.h>
#include <list.h>

struct plmn_identity {
  char * mcc;
  char * mnc;
};

struct cell_global_id {
  char * n_r_cell_identity;
  struct plmn_identity * plmn_identity;
};

struct list_of_attribute_element {
  char * attribute_name;
};

enum report_type {
  REPORT_TYPE_all,
  REPORT_TYPE_change,
};

struct list_of_node_level_ran_configuration_structures_for_adf_element {
  list_t * list_of_attributes;
  char * ran_configuration_structure_name;
  enum report_type report_type;
};

struct list_of_cell_configurations_to_be_reported_for_adf_element {
  struct cell_global_id * cell_global_id;
  list_t * list_of_cell_level_ran_configuration_structures_for_adf;
};

struct action_definition_format {
  list_t * list_of_node_level_ran_configuration_structures_for_adf;
  list_t * list_of_cell_configurations_to_be_reported_for_adf;
};

struct ric_action_definition {
  struct action_definition_format * action_definition_format;
  uint32_t ric_style_type;
};

enum report_type cJSON_Getreport_typeValue(const cJSON * j);
cJSON * cJSON_Createreport_type(const enum report_type x);

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

struct list_of_attribute_element * cJSON_Parselist_of_attribute_element(const char * s);
struct list_of_attribute_element * cJSON_Getlist_of_attribute_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_attribute_element(const struct list_of_attribute_element * x);
char * cJSON_Printlist_of_attribute_element(const struct list_of_attribute_element * x);
void cJSON_Deletelist_of_attribute_element(struct list_of_attribute_element * x);

struct list_of_node_level_ran_configuration_structures_for_adf_element * cJSON_Parselist_of_node_level_ran_configuration_structures_for_adf_element(const char * s);
struct list_of_node_level_ran_configuration_structures_for_adf_element * cJSON_Getlist_of_node_level_ran_configuration_structures_for_adf_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_node_level_ran_configuration_structures_for_adf_element(const struct list_of_node_level_ran_configuration_structures_for_adf_element * x);
char * cJSON_Printlist_of_node_level_ran_configuration_structures_for_adf_element(const struct list_of_node_level_ran_configuration_structures_for_adf_element * x);
void cJSON_Deletelist_of_node_level_ran_configuration_structures_for_adf_element(struct list_of_node_level_ran_configuration_structures_for_adf_element * x);

struct list_of_cell_configurations_to_be_reported_for_adf_element * cJSON_Parselist_of_cell_configurations_to_be_reported_for_adf_element(const char * s);
struct list_of_cell_configurations_to_be_reported_for_adf_element * cJSON_Getlist_of_cell_configurations_to_be_reported_for_adf_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_cell_configurations_to_be_reported_for_adf_element(const struct list_of_cell_configurations_to_be_reported_for_adf_element * x);
char * cJSON_Printlist_of_cell_configurations_to_be_reported_for_adf_element(const struct list_of_cell_configurations_to_be_reported_for_adf_element * x);
void cJSON_Deletelist_of_cell_configurations_to_be_reported_for_adf_element(struct list_of_cell_configurations_to_be_reported_for_adf_element * x);

struct action_definition_format * cJSON_Parseaction_definition_format(const char * s);
struct action_definition_format * cJSON_Getaction_definition_formatValue(const cJSON * j);
cJSON * cJSON_Createaction_definition_format(const struct action_definition_format * x);
char * cJSON_Printaction_definition_format(const struct action_definition_format * x);
void cJSON_Deleteaction_definition_format(struct action_definition_format * x);

struct ric_action_definition * cJSON_Parseric_action_definition(const char * s);
struct ric_action_definition * cJSON_Getric_action_definitionValue(const cJSON * j);
cJSON * cJSON_Createric_action_definition(const struct ric_action_definition * x);
char * cJSON_Printric_action_definition(const struct ric_action_definition * x);
void cJSON_Deleteric_action_definition(struct ric_action_definition * x);


#endif
