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

typedef struct{
  bool found;
  int v; 
} ans_int_opt_t;

int dec_int(void const* it, char* const needle);
// ans_cjson_t dec_int_it(void const* it_v, char* const needle)
int dec_int_opt(void const* it, char* const needle);

typedef struct{
  bool found;
  char* v; 
} ans_char_opt_t;

// char* dec_string(void const* it, char* const needle);
// ans_cjson_t dec_string_it(void const* it_v, char* const needle)
char* dec_string_opt(void const* it, char* const needle);

typedef struct{
  bool found;
  bool v; 
} ans_bool_opt_t;

// bool dec_bool(void const* it, char* const needle);
// ans_cjson_t dec_bool_it(void const* it_v, char* const needle)
bool dec_bool_opt(void const* it, char* const needle);

typedef struct{
  bool found;
  float v; 
} ans_float_opt_t;

// float dec_float(void const* it, char* const needle);
// ans_cjson_t dec_float_it(void const* it_v, char* const needle)
float dec_float_opt(void const* it, char* const needle);

ans_cjson_t find_object(void const* it, char* const needle);

#endif
