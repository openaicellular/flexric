#ifndef JSON_CCC_RIC_CONTROL_HEADER_H
#define JSON_CCC_RIC_CONTROL_HEADER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include <hashtable.h>
#include <list.h>

typedef struct {
    uint32_t ric_style_type;
} control_header_format_json_t;

typedef struct {
    control_header_format_json_t * control_header_format;
    uint32_t ric_style_type;
} ric_control_header_json_t;

control_header_format_json_t * cJSON_ParseControlHeaderFormat(const char * s);
control_header_format_json_t * cJSON_GetControlHeaderFormatValue(const cJSON * j);
cJSON * cJSON_CreateControlHeaderFormat(const control_header_format_json_t * x);
char * cJSON_PrintControlHeaderFormat(const control_header_format_json_t * x);
void cJSON_DeleteControlHeaderFormat(control_header_format_json_t * x);

ric_control_header_json_t * cJSON_ParseRicControlHeaderJson(const char * s);
ric_control_header_json_t * cJSON_GetRicControlHeaderJsonValue(const cJSON * j);
cJSON * cJSON_CreateRicControlHeaderJson(const ric_control_header_json_t * x);
char * cJSON_PrintRicControlHeaderJson(const ric_control_header_json_t * x);
void cJSON_DeleteRicControlHeaderJson(ric_control_header_json_t * x);

#endif





