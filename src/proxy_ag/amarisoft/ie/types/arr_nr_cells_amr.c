#include "arr_nr_cells_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"


void free_arr_nr_cells_amr(arr_nr_cells_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_nr_cells_amr(&src->nr_cells[i]);
    free(src->names[i]);
  }

  free(src->nr_cells);
  free(src->names);
}

arr_nr_cells_amr_t parse_arr_nr_cells_amr(void* it)
{
  assert(it != NULL);

  arr_nr_cells_amr_t dst = {0};

  dst.sz = cJSON_GetArraySize(it);
  assert(dst.sz > 0);

  dst.names = calloc(dst.sz , sizeof(char*));
  assert(dst.names != NULL && "Memory exhausted");
  
  dst.nr_cells = calloc(dst.sz, sizeof(nr_cells_amr_t));
  assert(dst.nr_cells != NULL && "Memory exhausted");


  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i);
    dst.names[i] = strdup(tmp->string);  
    dst.nr_cells[i] = parse_nr_cells_amr(tmp);
  }

  return dst;
}

