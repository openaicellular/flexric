#include "layers_amr.h"
#include <assert.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

layers_amr_t parse_layers_amr(void* it_void)
{
  assert(it_void != NULL);
  cJSON* it = (cJSON*)it_void;

  layers_amr_t dst = {0};
  // Mandatory. Name
  dst.layer_name = strdup(it->string);

  // Mandatory. Level
  dst.level = parse_string(it, "level");

  // Mandatory. Max_size 
  dst.max_size = parse_int(it, "max_size");

  // Mandatory. key
  dst.key = parse_bool(it, "key");
  
  // Mandatory. crypto
  dst.crypto = parse_bool(it, "crypto");
 
  // Mandatory. Payload
  dst.payload = parse_bool(it, "payload");

  // There may be more fields... 

  return dst;
}

