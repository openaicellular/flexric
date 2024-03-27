#ifndef PARSE_CJSON_MIR_H
#define PARSE_CJSON_MIR_H

#include "../cJSON/cJSON.h"

#include <stdbool.h>

typedef struct{
  cJSON const* it; 
  union{
  char* str;
  int val;
  float real;
  };
} ans_cjson_t;

int parse_int(cJSON const* it, char* const needle);
//ans_cjson_t parse_int_it(cJSON const* it, char* const needle);

char* parse_string(cJSON const* it, char* const needle);
//ans_cjson_t parse_string_it(cJSON const* it, char* const needle);

bool parse_bool(cJSON const* it, char* const needle);
//ans_cjson_t parse_bool_it(cJSON const* it, char* const needle);

float parse_float(cJSON const* it, char* const needle);
//ans_cjson_t parse_float_it(cJSON const* it, char* const needle);




ans_cjson_t find_object(cJSON const* it, char* const needle);

#endif
