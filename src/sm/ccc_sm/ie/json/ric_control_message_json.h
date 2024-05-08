#ifndef FLEXRIC_RIC_CONTROL_MESSAGE_JSON_H
#define FLEXRIC_RIC_CONTROL_MESSAGE_JSON_H

#include <cJSON.h>
#include <list.h>
#include <stdbool.h>
#include "values_of_attributes_json.h"

typedef struct {
  values_of_attributes_json_t * new_values_of_attributes;
  values_of_attributes_json_t * old_values_of_attributes;
  char * ran_configuration_structure_name;
} list_of_configuration_structure_control_element_t;

typedef struct {
  cell_global_id_json_t * cell_global_id;
  list_t * list_of_configuration_structures;
} list_of_cells_controlled_element_t;

typedef struct {
  list_t * list_of_configuration_structures;
  list_t * list_of_cells_controlled;
} control_message_format_json_t;

typedef struct {
  control_message_format_json_t * control_message_format;
} ric_control_message_json_t;

list_of_configuration_structure_control_element_t * cJSON_Parselist_of_configuration_structure_control_element(const char * s);
list_of_configuration_structure_control_element_t * cJSON_Getlist_of_configuration_structure_control_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_configuration_structure_control_element(const list_of_configuration_structure_control_element_t * x);
char * cJSON_Printlist_of_configuration_structure_control_element(const list_of_configuration_structure_control_element_t * x);
void cJSON_Deletelist_of_configuration_structure_control_element(list_of_configuration_structure_control_element_t * x);

list_of_cells_controlled_element_t * cJSON_Parselist_of_cells_controlled_element(const char * s);
list_of_cells_controlled_element_t * cJSON_Getlist_of_cells_controlled_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_cells_controlled_element(const list_of_cells_controlled_element_t * x);
char * cJSON_Printlist_of_cells_controlled_element(const list_of_cells_controlled_element_t * x);
void cJSON_Deletelist_of_cells_controlled_element(list_of_cells_controlled_element_t * x);

control_message_format_json_t * cJSON_Parsecontrol_message_format(const char * s);
control_message_format_json_t * cJSON_Getcontrol_message_formatValue(const cJSON * j);
cJSON * cJSON_Createcontrol_message_format(const control_message_format_json_t * x);
char * cJSON_Printcontrol_message_format(const control_message_format_json_t * x);
void cJSON_Deletecontrol_message_format(control_message_format_json_t * x);

ric_control_message_json_t * cJSON_Parseric_control_message(const char * s);
ric_control_message_json_t * cJSON_Getric_control_messageValue(const cJSON * j);
cJSON * cJSON_Createric_control_message(const ric_control_message_json_t * x);
char * cJSON_Printric_control_message(const ric_control_message_json_t * x);
void cJSON_Deleteric_control_message(ric_control_message_json_t * x);

#endif
