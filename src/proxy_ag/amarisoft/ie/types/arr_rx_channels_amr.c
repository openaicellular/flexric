#include "arr_rx_channels_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_arr_rx_channels_amr(arr_rx_channels_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_rx_channels_amr(&src->rx_channels_amr[i]);
  }
 
  free(src->rx_channels_amr);
}

arr_rx_channels_amr_t parse_arr_rx_channels_amr(void* it)
{
  assert(it != NULL);

  arr_rx_channels_amr_t dst = {0}; 

  ans_cjson_t ans = find_object(it, "rx_channels");

  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0 && "Unclear from specs");

  dst.rx_channels_amr = calloc(dst.sz, sizeof(rx_channels_amr_t ));
  assert(dst.rx_channels_amr != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.rx_channels_amr[i] = parse_rx_channels_amr(tmp);
  }

  return dst;
}


