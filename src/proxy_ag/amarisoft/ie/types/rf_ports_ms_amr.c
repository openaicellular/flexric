#include "rf_ports_ms_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../dec/parse_cjson.h"

void free_rf_ports_ms_amr(rf_ports_ms_amr_t* src)
{
  assert(src != NULL);
  //float min;
  //float max;
  //float sd;
}

rf_ports_ms_amr_t parse_rf_ports_ms_amr(void* it)
{
  assert(it != NULL);

  rf_ports_ms_amr_t dst = {0}; 

  dst.min= parse_float(it, "min");
  dst.max= parse_float(it, "max");
  dst.sd= parse_float(it, "sd");

  return dst;
}

