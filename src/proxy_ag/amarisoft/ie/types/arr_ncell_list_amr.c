#include "arr_ncell_list_amr.h"

#include <assert.h>

#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_arr_ncell_list_amr(arr_ncell_list_amr_t* src)
{
  assert(src != NULL);
  for(size_t i = 0; i < src->sz; ++i){
    free_ncell_list_amr(&src->ncell_lst[i]);
  }
  free(src->ncell_lst);
}

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

