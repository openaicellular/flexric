#include "arr_cell_amr.h"

#include <assert.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

arr_cell_amr_t parse_arr_cell_amr(void* it)
{
  assert(it != NULL);

  arr_cell_amr_t dst = {0};

  ans_cjson_t ans = find_object( it, "cells");
 
  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0);

  dst.cells = calloc(dst.sz, sizeof(cell_amr_t));
  assert(dst.cells != NULL && "Memory exhausted");

  dst.names = calloc(dst.sz, sizeof(char*));
  assert(dst.names != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.names[i] = strdup(tmp->string);
    dst.cells[i] = parse_cell_amr(tmp);
  }

  return dst;
}

