#include "dec_cjson.h"

#include "../parser/cJSON/cJSON.h"

#include <assert.h>
#include <string.h>

int dec_int(const void* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItemCaseSensitive(it, needle);
  assert(cJSON_IsNumber(new_it));
  return new_it->valueint;
}

int dec_int_opt(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItemCaseSensitive(it, needle);
  
  if(!cJSON_IsNumber(new_it)) 
    return 0;

  return new_it->valueint;
}

ans_cjson_t dec_int_it(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItemCaseSensitive(it, needle);
  assert(cJSON_IsNumber(new_it));

  ans_cjson_t dst = {.it = new_it};
  dst.val = new_it->valueint;

  return dst;
}

char* dec_string(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  assert(cJSON_IsString(new_it));
  char* dst = strndup(new_it->valuestring, 256);
  return dst; 
}

char* dec_string_opt(void const* it_v, char* const needle)
{

  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);

  if(!cJSON_IsString(new_it))
    return NULL;

  return strndup(new_it->valuestring, 256);
}

ans_cjson_t dec_string_it(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  assert(cJSON_IsString(new_it));

  ans_cjson_t dst = {.it = new_it};
  dst.str = strndup(new_it->valuestring, 256);
  assert(dst.str != NULL && "String larger than 256 needed?");

  return dst;
}

bool dec_bool(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  assert(cJSON_IsBool(new_it));
  return new_it->valueint;
}

bool dec_bool_opt(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  if(!cJSON_IsBool(new_it))
    return 0 ;

  return new_it->valueint;
}

ans_cjson_t dec_bool_it(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  assert(cJSON_IsBool(new_it));

  ans_cjson_t dst = {.it = new_it};
  dst.val = new_it->valueint;
  return dst;
}

float dec_float(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  return new_it->valuedouble;
}

float dec_float_opt(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);

  if(!cJSON_IsNumber(new_it))
    return 0.0;

  return new_it->valuedouble;
}

ans_cjson_t dec_float_it(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);

  ans_cjson_t dst = {.it = new_it};
  dst.real = new_it->valuedouble;
  return dst;
}

ans_cjson_t find_object(void const* it_v, char* const needle)
{
  cJSON* it = (cJSON*)(it_v); 
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);

  ans_cjson_t dst = {.it = new_it};

  return dst;
}


