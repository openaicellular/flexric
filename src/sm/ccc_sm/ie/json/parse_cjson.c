#include "parse_cjson.h"

#include <assert.h>
#include <string.h>

ans_cjson_t parse_int(cJSON const* it, char* const needle)
{
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItemCaseSensitive(it, needle);
  assert(cJSON_IsNumber(new_it));

  ans_cjson_t dst = {.it = new_it};
  dst.val = new_it->valueint;

  return dst;
}

ans_cjson_t parse_string(cJSON const* it, char* const needle)
{
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  assert(cJSON_IsString(new_it));

  ans_cjson_t dst = {.it = new_it};
  dst.str = strndup(new_it->valuestring, 256);
  assert(dst.str != NULL && "String larger than 256 needed?");

  return dst;
}

ans_cjson_t parse_bool(cJSON const* it, char* const needle)
{
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);
  assert(cJSON_IsBool(new_it));

  ans_cjson_t dst = {.it = new_it};
  dst.val = new_it->valueint;
  return dst;
}

ans_cjson_t parse_float(cJSON const* it, char* const needle)
{
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);

  ans_cjson_t dst = {.it = new_it};
  dst.real = new_it->valuedouble;
  return dst;
}

ans_cjson_t find_object(cJSON const* it, char* const needle)
{
  assert(it != NULL);
  assert(needle != NULL);

  cJSON const* new_it = cJSON_GetObjectItem(it, needle);

  ans_cjson_t dst = {.it = new_it};

  return dst;
}


