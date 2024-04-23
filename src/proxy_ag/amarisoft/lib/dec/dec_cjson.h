#ifndef PARSE_CJSON_MIR_H
#define PARSE_CJSON_MIR_H

#include <stdbool.h>

typedef struct{
  void const* it; 
  union{
  char* str;
  int val;
  float real;
  };
} ans_cjson_t;

int dec_int(void const* it, char* const needle);
//ans_cjson_t dec_int_it(cJSON const* it, char* const needle);

char* dec_string(void const* it, char* const needle);
//ans_cjson_t dec_string_it(cJSON const* it, char* const needle);

bool dec_bool(void const* it, char* const needle);
//ans_cjson_t dec_bool_it(cJSON const* it, char* const needle);

float dec_float(void const* it, char* const needle);
//ans_cjson_t dec_float_it(cJSON const* it, char* const needle);

ans_cjson_t find_object(void const* it, char* const needle);

#endif
