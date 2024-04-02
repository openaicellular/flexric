#include "rf_ports_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_rf_ports_amr(rf_ports_amr_t* src)
{
  assert(src != NULL);

  free_channel_dl_amr(&src->channel_dl);

  // Mandatory. Uplink frequency shift in Hz 
  // float ul_freq_shift;

  // Mandatory. Sample rate in Hz
  // float sample_rate;
}

rf_ports_amr_t parse_rf_ports_amr(void* it)
{
  assert(it != NULL);
  rf_ports_amr_t dst = {0};

  ans_cjson_t ans = find_object(it, "channel_dl" );
  if(ans.it != NULL)
    dst.channel_dl = parse_channel_dl_amr((void*)ans.it); 

  // Mandatory. Uplink frequency shift in Hz
  dst.ul_freq_shift = parse_float(it, "ul_freq_shift");

  // Mandatory. Sample rate in Hz
  dst.sample_rate = parse_float(it, "sample_rate");

  return dst;
}
