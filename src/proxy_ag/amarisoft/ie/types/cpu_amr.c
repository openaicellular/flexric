#include "cpu_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_cpu_amr(cpu_amr_t* src)
{
  assert(src != NULL);
  // cpu load % in one core
  // int global;
}

cpu_amr_t parse_cpu_amr(void* it)
{
  assert(it != NULL);

  cpu_amr_t dst = {0}; 

  // Mandatory. Array of float. Noise level table, in dB
  ans_cjson_t ans = find_object(it, "cpu");

  dst.global = parse_int(ans.it, "global");

  return dst;
}


