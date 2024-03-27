#include "logs_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

logs_amr_t parse_logs_amr(void* it_void)
{
  assert(it_void != NULL);

  cJSON* it = (cJSON*)it_void;
  
  logs_amr_t dst = {0};

  ans_cjson_t ans_logs = find_object(it, "logs");

  ans_cjson_t ans_layers = find_object(ans_logs.it, "layers");

  dst.sz_layers = cJSON_GetArraySize(ans_layers.it);
 
  if(dst.sz_layers > 0){
    dst.layers = calloc(dst.sz_layers, sizeof(layers_amr_t));
  }

  cJSON* tmp_it = ans_layers.it->child; 
  for(size_t i = 0; i < dst.sz_layers; ++i){
    assert(tmp_it != NULL);
        
    dst.layers[i] = parse_layers_amr(tmp_it);
    tmp_it = tmp_it->next;
  }

  // Mandatory. Count
  dst.count = parse_int(ans_logs.it,"count");

  // Optional. Rotate
  // int* rotate;

  // Optional. Path
  // char* path;

  // Mandatory bcch
  // True if BCCH dump is enabled (eNB only)
  // BCCH 
  dst.bcch = parse_bool(ans_logs.it,"bcch");

  // Mandatory mib
  // True if MIB dump is enabled (eNB only).
  dst.mib = parse_bool(ans_logs.it,"mib");

  return dst;
}

