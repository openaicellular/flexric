#ifndef FLEXRIC_RAN_FUNCTION_DEFINITION_JSON_H
#define FLEXRIC_RAN_FUNCTION_DEFINITION_JSON_H

#include <stdint.h>
#include <stdbool.h>
#include <cJSON.h>
#include <hashtable.h>
#include <list.h>

#include "cell_global_id_json.h"

typedef struct {
  int64_t control_service_control_outcome_format_type;
  int64_t control_service_header_format_type;
  int64_t control_service_message_format_type;
  char * control_service_style_name;
  int64_t control_service_style_type;
  int64_t * ric_call_process_id_format_type;
} list_of_supported_control_style_element_t;

typedef struct {
  list_t * list_of_supported_control_styles;
} control_service_json_t;

typedef struct {
  int64_t event_trigger_format_type;
  char * event_trigger_style_name;
  int64_t event_trigger_style_type;
} list_of_supported_event_trigger_style_element_t;

typedef struct {
  list_t * list_of_supported_event_trigger_styles;
} event_trigger_json_t;

typedef struct {
  int64_t event_trigger_style_type;
} list_of_supported_event_trigger_styles_for_report_style_element_t;

typedef struct {
  list_t * list_of_supported_event_trigger_styles_for_report_style;
  int64_t report_service_action_definition_format_type;
  int64_t report_service_indication_header_format_type;
  int64_t report_service_indication_message_format_type;
  char * report_service_style_name;
  int64_t report_service_style_type;
} list_of_supported_report_style_element_t;

typedef struct {
  list_t * list_of_supported_report_styles;
} report_service_json_t;

typedef struct {
  control_service_json_t * control_service;
  event_trigger_json_t * event_trigger;
  hashtable_t * insert_service;
  hashtable_t * policy_service;
  report_service_json_t * report_service;
} supported_services_json_t;

typedef struct {
  char * attribute_name;
  supported_services_json_t * supported_services;
} list_of_supported_attribute_element_t;

typedef struct {
  list_t * list_of_supported_attributes;
  char * ran_configuration_structure_name;
} list_of_supported_node_level_configuration_structure_element_t;

typedef struct {
  cell_global_id_json_t * cell_global_id;
  list_t * list_of_supported_cell_level_ran_configuration_structures;
} list_of_cells_for_ran_function_definition_element_t;

typedef struct {
  char * ran_function_description;
  int64_t * ran_function_instance;
  char * ran_function_service_model_oid;
  char * ran_function_short_name;
} ran_function_name_json_t;

typedef struct {
  list_t * list_of_cells_for_ran_function_definition;
  list_t * list_of_supported_node_level_configuration_structures;
  ran_function_name_json_t * ran_function_name;
} ric_function_definition_json_t ;

list_of_supported_control_style_element_t * cJSON_Parselist_of_supported_control_style_element(const char * s);
list_of_supported_control_style_element_t * cJSON_Getlist_of_supported_control_style_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_supported_control_style_element(const list_of_supported_control_style_element_t * x);
char * cJSON_Printlist_of_supported_control_style_element(const list_of_supported_control_style_element_t * x);
void cJSON_Deletelist_of_supported_control_style_element(list_of_supported_control_style_element_t * x);

control_service_json_t * cJSON_Parsecontrol_service(const char * s);
control_service_json_t * cJSON_Getcontrol_serviceValue(const cJSON * j);
cJSON * cJSON_Createcontrol_service(const control_service_json_t * x);
char * cJSON_Printcontrol_service(const control_service_json_t * x);
void cJSON_Deletecontrol_service(control_service_json_t * x);

list_of_supported_event_trigger_style_element_t * cJSON_Parselist_of_supported_event_trigger_style_element(const char * s);
list_of_supported_event_trigger_style_element_t * cJSON_Getlist_of_supported_event_trigger_style_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_supported_event_trigger_style_element(const list_of_supported_event_trigger_style_element_t * x);
char * cJSON_Printlist_of_supported_event_trigger_style_element(const list_of_supported_event_trigger_style_element_t * x);
void cJSON_Deletelist_of_supported_event_trigger_style_element(list_of_supported_event_trigger_style_element_t * x);

event_trigger_json_t * cJSON_Parseevent_trigger(const char * s);
event_trigger_json_t * cJSON_Getevent_triggerValue(const cJSON * j);
cJSON * cJSON_Createevent_trigger(const event_trigger_json_t * x);
char * cJSON_Printevent_trigger(const event_trigger_json_t * x);
void cJSON_Deleteevent_trigger(event_trigger_json_t * x);

list_of_supported_event_trigger_styles_for_report_style_element_t * cJSON_Parselist_of_supported_event_trigger_styles_for_report_style_element(const char * s);
list_of_supported_event_trigger_styles_for_report_style_element_t * cJSON_Getlist_of_supported_event_trigger_styles_for_report_style_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_supported_event_trigger_styles_for_report_style_element(const list_of_supported_event_trigger_styles_for_report_style_element_t * x);
char * cJSON_Printlist_of_supported_event_trigger_styles_for_report_style_element(const list_of_supported_event_trigger_styles_for_report_style_element_t * x);
void cJSON_Deletelist_of_supported_event_trigger_styles_for_report_style_element(list_of_supported_event_trigger_styles_for_report_style_element_t * x);

list_of_supported_report_style_element_t * cJSON_Parselist_of_supported_report_style_element(const char * s);
list_of_supported_report_style_element_t * cJSON_Getlist_of_supported_report_style_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_supported_report_style_element(const list_of_supported_report_style_element_t * x);
char * cJSON_Printlist_of_supported_report_style_element(const list_of_supported_report_style_element_t * x);
void cJSON_Deletelist_of_supported_report_style_element(list_of_supported_report_style_element_t * x);

report_service_json_t * cJSON_Parsereport_service(const char * s);
report_service_json_t * cJSON_Getreport_serviceValue(const cJSON * j);
cJSON * cJSON_Createreport_service(const report_service_json_t * x);
char * cJSON_Printreport_service(const report_service_json_t * x);
void cJSON_Deletereport_service(report_service_json_t * x);

supported_services_json_t * cJSON_Parsesupported_services(const char * s);
supported_services_json_t * cJSON_Getsupported_servicesValue(const cJSON * j);
cJSON * cJSON_Createsupported_services(const supported_services_json_t * x);
char * cJSON_Printsupported_services(const supported_services_json_t * x);
void cJSON_Deletesupported_services(supported_services_json_t * x);

list_of_supported_attribute_element_t * cJSON_Parselist_of_supported_attribute_element(const char * s);
list_of_supported_attribute_element_t * cJSON_Getlist_of_supported_attribute_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_supported_attribute_element(const list_of_supported_attribute_element_t * x);
char * cJSON_Printlist_of_supported_attribute_element(const list_of_supported_attribute_element_t * x);
void cJSON_Deletelist_of_supported_attribute_element(list_of_supported_attribute_element_t * x);

list_of_supported_node_level_configuration_structure_element_t * cJSON_Parselist_of_supported_node_level_configuration_structure_element(const char * s);
list_of_supported_node_level_configuration_structure_element_t * cJSON_Getlist_of_supported_node_level_configuration_structure_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_supported_node_level_configuration_structure_element(const list_of_supported_node_level_configuration_structure_element_t * x);
char * cJSON_Printlist_of_supported_node_level_configuration_structure_element(const list_of_supported_node_level_configuration_structure_element_t * x);
void cJSON_Deletelist_of_supported_node_level_configuration_structure_element(list_of_supported_node_level_configuration_structure_element_t * x);
// TODO:
list_of_supported_node_level_configuration_structure_element_t * list_of_supported_node_level_configuration_structure_element(const char * s);

list_of_cells_for_ran_function_definition_element_t * cJSON_Parselist_of_cells_for_ran_function_definition_element(const char * s);
list_of_cells_for_ran_function_definition_element_t * cJSON_Getlist_of_cells_for_ran_function_definition_elementValue(const cJSON * j);
cJSON * cJSON_Createlist_of_cells_for_ran_function_definition_element(const list_of_cells_for_ran_function_definition_element_t * x);
char * cJSON_Printlist_of_cells_for_ran_function_definition_element(const list_of_cells_for_ran_function_definition_element_t * x);
void cJSON_Deletelist_of_cells_for_ran_function_definition_element(list_of_cells_for_ran_function_definition_element_t * x);

ran_function_name_json_t * cJSON_Parseran_function(const char * s);
ran_function_name_json_t * cJSON_Getran_functionValue(const cJSON * j);
cJSON * cJSON_Createran_function(const ran_function_name_json_t * x);
char * cJSON_Printran_function(const ran_function_name_json_t * x);
void cJSON_Deleteran_function(ran_function_name_json_t * x);

ric_function_definition_json_t * cJSON_Parseric_function_definition(const char * s);
ric_function_definition_json_t * cJSON_Getric_function_definitionValue(const cJSON * j);
cJSON * cJSON_Createric_function_definition(const ric_function_definition_json_t * x);
char * cJSON_Printric_function_definition(const ric_function_definition_json_t * x);
void cJSON_Deleteric_function_definition(ric_function_definition_json_t * x);


#endif
