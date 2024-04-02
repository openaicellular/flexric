#include "arr_qos_flows_ue_lst_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_arr_qos_flows_ue_lst(arr_qos_flows_ue_lst_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_qos_flows_ue_lst_amr(&src->qos_flows[i]);
  }
  free(src->qos_flows);
}

arr_qos_flows_ue_lst_t parse_arr_qos_flows_ue_lst(void* it)
{
  assert(it != NULL);
  arr_qos_flows_ue_lst_t dst = {0}; 

  dst.sz = cJSON_GetArraySize(it);
  assert(dst.sz > 0);

  dst.qos_flows = calloc(dst.sz, sizeof(qos_flows_ue_lst_amr_t));
  assert(dst.qos_flows != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i) ;
    dst.qos_flows[i] = parse_qos_flows_ue_lst_amr(tmp);
  }

  return dst;
}


