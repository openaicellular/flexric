#ifndef JSON_CCC_RIC_INDICATION_HEADER_H
#define JSON_CCC_RIC_INDICATION_HEADER_H

#include "cJSON.h"

enum indication_reason {
  INDICATION_REASON_periodic,
  INDICATION_REASON_upon_change,
  INDICATION_REASON_upon_subscription,
};

struct indication_header_format {
  char * event_time;
  enum indication_reason indication_reason;
};

struct ric_indication_header {
  struct indication_header_format * indication_header_format;
};

enum indication_reason cJSON_Getindication_reasonValue(const cJSON * j);
cJSON * cJSON_Createindication_reason(const enum indication_reason x);

struct indication_header_format * cJSON_Parseindication_header_format(const char * s);
struct indication_header_format * cJSON_Getindication_header_formatValue(const cJSON * j);
cJSON * cJSON_Createindication_header_format(const struct indication_header_format * x);
char * cJSON_Printindication_header_format(const struct indication_header_format * x);
void cJSON_Deleteindication_header_format(struct indication_header_format * x);

struct ric_indication_header * cJSON_Parseric_indication_header(const char * s);
struct ric_indication_header * cJSON_Getric_indication_headerValue(const cJSON * j);
cJSON * cJSON_Createric_indication_header(const struct ric_indication_header * x);
char * cJSON_Printric_indication_header(const struct ric_indication_header * x);
void cJSON_Deleteric_indication_header(struct ric_indication_header * x);

#endif
