#ifndef JSON_CCC_RIC_INDICATION_HEADER_H
#define JSON_CCC_RIC_INDICATION_HEADER_H

#include "cJSON.h"
#include "../ir/e2sm_ccc_ind_hdr_frmt_1.h"

typedef struct {
  char * event_time;
  indication_reason_e indication_reason;
} indication_header_format_json_t;

typedef struct {
  indication_header_format_json_t * indication_header_format;
} ric_indication_header_json_t;

indication_reason_e cJSON_Getindication_reasonValue(const cJSON * j);
cJSON * cJSON_Createindication_reason(const indication_reason_e x);

indication_header_format_json_t * cJSON_Parseindication_header_format(const char * s);
indication_header_format_json_t * cJSON_Getindication_header_formatValue(const cJSON * j);
cJSON * cJSON_Createindication_header_format(const indication_header_format_json_t * x);
char * cJSON_Printindication_header_format(const indication_header_format_json_t * x);
void cJSON_Deleteindication_header_format(indication_header_format_json_t * x);

ric_indication_header_json_t * cJSON_Parseric_indication_header(const char * s);
ric_indication_header_json_t * cJSON_Getric_indication_headerValue(const cJSON * j);
cJSON * cJSON_Createric_indication_header(const ric_indication_header_json_t * x);
char * cJSON_Printric_indication_header(const ric_indication_header_json_t * x);
void cJSON_Deleteric_indication_header(ric_indication_header_json_t * x);

#endif
