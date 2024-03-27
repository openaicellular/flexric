#include "path_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"


path_amr_t parse_path_amr(void* it)
{
  assert(it != NULL);

  path_amr_t dst = {0};

  // Mandatory
  dst.type = parse_string(it, "type");

  // Mandatory. Path delay in ns
  dst.delay = parse_float(it, "delay");

  // Mandatory. Path gain in dB
  dst.gain = parse_float(it, "gain");

  // Mandatory. Frequency shift in Hz
  dst.freq_shift = parse_float(it, "freq_shift");

  // Mandatory. Doppler frequency in Hz
  dst.freq_doppler = parse_float(it, "freq_doppler");

  return dst;
}


