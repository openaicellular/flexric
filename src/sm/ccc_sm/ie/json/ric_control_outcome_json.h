#ifndef FLEXRIC_RIC_CONTROL_OUTCOME_JSON_H
#define FLEXRIC_RIC_CONTROL_OUTCOME_JSON_H

#include <cJSON.h>
#include <list.h>
#include <stdbool.h>
#include "values_of_attributes_json.h"
#include "../ir/cause.h"

typedef struct {
  char * applied_timestamp;
  values_of_attributes_json_t * current_values_of_attributes;
  values_of_attributes_json_t * old_values_of_attributes;
  char * ran_configuration_structure_name;
} ran_configuration_structures_accepted_list_element_t;

typedef struct {
  ccc_cause_e cause;
  values_of_attributes_json_t * old_values_of_attributes;
  char * ran_configuration_structure_name;
  values_of_attributes_json_t * requested_values_of_attributes;
} ran_configuration_structures_failed_list_element_t;

typedef struct {
  cell_global_id_json_t * cell_global_id;
  list_t * ran_configuration_structures_accepted_list;
  list_t * ran_configuration_structures_failed_list;
} list_of_cells_for_control_outcome_element_t;

typedef struct {
  list_t * ran_configuration_structures_accepted_list;
  list_t * ran_configuration_structures_failed_list;
  char * received_timestamp;
  list_t * list_of_cells_for_control_outcome;
}control_outcome_format_json_t;

typedef struct {
  control_outcome_format_json_t * control_outcome_format;
}ric_control_outcome_json_t;

ccc_cause_e cJSON_GetcauseValue(const cJSON * j);
cJSON * cJSON_Createcause(const ccc_cause_e x);

ran_configuration_structures_accepted_list_element_t * cJSON_Parseran_configuration_structures_accepted_list_element(const char * s);
ran_configuration_structures_accepted_list_element_t * cJSON_Getran_configuration_structures_accepted_list_elementValue(const cJSON * j);
cJSON * cJSON_Createran_configuration_structures_accepted_list_element(const ran_configuration_structures_accepted_list_element_t * x);
char * cJSON_Printran_configuration_structures_accepted_list_element(const ran_configuration_structures_accepted_list_element_t * x);
void cJSON_Deleteran_configuration_structures_accepted_list_element(ran_configuration_structures_accepted_list_element_t * x);

ran_configuration_structures_failed_list_element_t * cJSON_Parseran_configuration_structures_failed_list_element(const char * s);
ran_configuration_structures_failed_list_element_t * cJSON_Getran_configuration_structures_failed_list_elementValue(const cJSON * j);
cJSON * cJSON_Createran_configuration_structures_failed_list_element(const ran_configuration_structures_failed_list_element_t * x);
char * cJSON_Printran_configuration_structures_failed_list_element(const ran_configuration_structures_failed_list_element_t * x);
void cJSON_Deleteran_configuration_structures_failed_list_element(ran_configuration_structures_failed_list_element_t * x);

list_of_cells_for_control_outcome_element_t * cJSON_Parselist_of_cells_for_control_outcome_element(const char * s);
list_of_cells_for_control_outcome_element_t * cJSON_Getlist_of_cells_for_control_outcome_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_cells_for_control_outcome_element(const list_of_cells_for_control_outcome_element_t * x);
char * cJSON_Printlist_of_cells_for_control_outcome_element(const list_of_cells_for_control_outcome_element_t * x);
void cJSON_Deletelist_of_cells_for_control_outcome_element(list_of_cells_for_control_outcome_element_t * x);

control_outcome_format_json_t * cJSON_Parsecontrol_outcome_format(const char * s);
control_outcome_format_json_t * cJSON_Getcontrol_outcome_formatValue(const cJSON * j);
cJSON * cJSON_Createcontrol_outcome_format(const control_outcome_format_json_t * x);
char * cJSON_Printcontrol_outcome_format(const control_outcome_format_json_t * x);
void cJSON_Deletecontrol_outcome_format(control_outcome_format_json_t * x);

ric_control_outcome_json_t * cJSON_Parseric_control_outcome(const char * s);
ric_control_outcome_json_t * cJSON_Getric_control_outcomeValue(const cJSON * j);
cJSON * cJSON_Createric_control_outcome(const ric_control_outcome_json_t * x);
char * cJSON_Printric_control_outcome(const ric_control_outcome_json_t * x);
void cJSON_Deleteric_control_outcome(ric_control_outcome_json_t * x);


#endif
