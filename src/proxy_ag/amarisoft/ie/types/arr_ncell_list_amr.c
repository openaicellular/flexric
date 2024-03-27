#include "arr_ncell_list_amr.h"

#include <assert.h>

#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

arr_ncell_list_amr_t parse_arr_ncell_list_amr(void* it)
{
  assert(it != NULL);

  arr_ncell_list_amr_t dst = {0}; 
  dst.sz = cJSON_GetArraySize(it);
  assert(dst.sz > 0);

  dst.ncell_lst = calloc(dst.sz, sizeof(ncell_list_amr_t));
  assert(dst.ncell_lst != NULL && "Memory exhausted");
  
  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i);
    dst.ncell_lst[i] = parse_ncell_list_amr(tmp);
  }

  return dst;
}

