#include "arr_rf_ports_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_arr_rf_ports_amr(arr_rf_ports_amr_t* src)
{
  assert(src != NULL);

}

arr_rf_ports_amr_t parse_arr_rf_ports_amr(void* it)
{
  assert(it != NULL);
  arr_rf_ports_amr_t dst = {0};

  ans_cjson_t ans = find_object(it, "rf_ports");

  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0);

  dst.rf_ports_amr = calloc(dst.sz, sizeof(rf_ports_amr_t));
  assert(dst.rf_ports_amr != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.rf_ports_amr[i] = parse_rf_ports_amr(tmp);
  }

  return dst;
}

