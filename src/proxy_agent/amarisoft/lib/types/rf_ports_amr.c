#include "rf_ports_amr.h"
#include <assert.h>
#include <stdlib.h>

void free_rf_ports_amr(rf_ports_amr_t* src)
{
  assert(src != NULL);

  free_channel_dl_amr(&src->channel_dl);

  // Mandatory. Uplink frequency shift in Hz 
  // float ul_freq_shift;

  // Mandatory. Sample rate in Hz
  // float sample_rate;
}


