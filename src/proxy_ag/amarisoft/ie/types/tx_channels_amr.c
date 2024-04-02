#include "tx_channels_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"


void free_tx_channels_amr(tx_channels_amr_t* src )
{
  assert(src != NULL);
  // Mandatory RF transmission gain, in dB.
  //float gain; 

  // Mandatory RF transmission frequency, in MHz. 
  //float freq;

  // Mandatory. RF port index.
  //int port;
}

tx_channels_amr_t parse_tx_channels_amr(void* it)
{
  assert(it != NULL);
  tx_channels_amr_t dst = {0};

  // Mandatory RF transmission gain, in dB.
  dst.gain = parse_float(it, "gain"); 

  // Mandatory RF transmission frequency, in MHz.
  dst.freq = parse_float(it, "freq");

  // Mandatory. RF port index.
  dst.port = parse_int(it, "port");

  return dst;
}


