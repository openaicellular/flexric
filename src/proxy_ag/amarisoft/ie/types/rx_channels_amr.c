#include "rx_channels_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "../dec/parse_cjson.h"

void free_rx_channels_amr(rx_channels_amr_t *src) 
{
  assert(src != NULL);

  // Mandatory. RF reception gain, in dB
  // float gain;

  // Mandatory. RF reception frequency, in MHz 
  // float freq;

  // Mandatory? Optional? Bug? 2023-12-15
  // int port;
}

rx_channels_amr_t parse_rx_channels_amr(void* it)
{
  assert(it != NULL);
  rx_channels_amr_t dst = {0}; 

  // Mandatory. RF reception gain, in dB
  dst.gain = parse_float(it, "gain");

  // Mandatory. RF reception frequency, in MHz 
  dst.freq = parse_float(it, "freq");

  // Mandatory? Optional? Bug? 2023-12-15
  // int port;

  return dst;
}


