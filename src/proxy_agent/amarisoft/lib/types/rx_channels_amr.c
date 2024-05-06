#include "rx_channels_amr.h"
#include <assert.h>
#include <stdlib.h>

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


