#include "arr_tx_channels_amr.h"

#include <assert.h>
#include "arr_tx_channels_amr.h"
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

arr_tx_channels_amr_t parse_arr_tx_channels_amr(void* it)
{
  assert(it != NULL);
  arr_tx_channels_amr_t dst = {0};

  ans_cjson_t ans = find_object(it, "tx_channels");

  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0 && "Unclear from specs");

  dst.tx_channels_amr = calloc(dst.sz, sizeof(tx_channels_amr_t ));
  assert(dst.tx_channels_amr != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.tx_channels_amr[i] = parse_tx_channels_amr(tmp);
  }

  return dst;
}

