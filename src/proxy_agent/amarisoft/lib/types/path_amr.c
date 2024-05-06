#include "path_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_path_amr(path_amr_t* src)
{
  // Mandatory
  free(src->type);

  // Mandatory. Path delay in ns
  //float delay;

  // Mandatory. Path gain in dB
  //float gain;

  // Mandatory. Frequency shift in Hz
  //float freq_shift;

  // Mandatory. Doppler frequency in Hz
  //float freq_doppler;
}


