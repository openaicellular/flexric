#ifndef PARSE_CJSON_MIR_H
#define PARSE_CJSON_MIR_H

#include "cJSON.h"

typedef struct{
  cJSON const* it; 
  union{
  char* str;
  int val;
  float real;
  };
} ans_cjson_t;

ans_cjson_t parse_int(cJSON const* it, char* const needle);

ans_cjson_t parse_string(cJSON const* it, char* const needle);

ans_cjson_t parse_bool(cJSON const* it, char* const needle);

ans_cjson_t parse_float(cJSON const* it, char* const needle);

ans_cjson_t find_object(cJSON const* it, char* const needle);

#endif
