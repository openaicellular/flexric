#include "arr_cells_ue_lst_amr.h"

#include <assert.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_arr_cells_ue_lst_amr( arr_cells_ue_lst_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_cells_ue_lst_amr(&src->cell[i]);
  }

  free(src->cell);
}

arr_cells_ue_lst_amr_t parse_arr_cells_ue_lst_amr(void* it)
{
  assert(it != NULL);

  arr_cells_ue_lst_amr_t dst = {0}; 

  dst.sz = cJSON_GetArraySize(it);
  assert(dst.sz > 0);

  dst.cell = calloc(dst.sz, sizeof(cells_ue_lst_amr_t));
  assert(dst.cell != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i);
    dst.cell[i] = parse_cells_ue_lst_amr(tmp);
  }

  return dst;
}

