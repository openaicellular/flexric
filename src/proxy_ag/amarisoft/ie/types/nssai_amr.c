#include "nssai_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_nssai_amr(nssai_amr_t* src)
{
  assert(src != NULL);
  // Mandatory. Slice Srvice Type [1-255] 
  // uint8_t sst;

  // Optional. Slice Differentiator [0-0xFFFFFE]
  free(src->sd);
}

nssai_amr_t parse_nssai_amr(void* it)
{
  assert(it != NULL);
  nssai_amr_t dst = {0};

  // Mandatory. Slice Srvice Type [1-255] 
  dst.sst = parse_int(it, "sst"); 
  assert(dst.sst != 0); 

  // Optional. Slice Differentiator [0-0xFFFFFE]
  ans_cjson_t ans = find_object(it, "sd");
  if(ans.it != NULL){
    dst.sd = calloc(1, sizeof(uint32_t));
    assert(dst.sd != NULL && "Memory exhausted");
    *dst.sd = ans.it->valueint; 
    assert(*dst.sd < 0xFFFFFF);
  }

  return dst;
}

