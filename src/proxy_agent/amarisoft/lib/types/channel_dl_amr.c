#include "channel_dl_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_channel_dl_amr(channel_dl_amr_t* src)
{
  assert(src != NULL);
  // Mandatory. Array of float. Noise level table, in dB
  free(src->noise_level);
  //size_t sz_noise_level;

  // Mandatory. Global frequency shift in Hz
  //float freq_shift;

  // Mandatory. Array Each object contains the
  // parameters of a path
  for(size_t i = 0; i < src->sz_path; ++i){
    free_path_amr(&src->path[i]);
  } 

  free(src->path);
}


