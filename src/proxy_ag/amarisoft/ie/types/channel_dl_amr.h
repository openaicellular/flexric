#ifndef CHANNEL_DL_AMR_MIR_H
#define CHANNEL_DL_AMR_MIR_H

#include <stdint.h>
#include <stdlib.h>
#include "path_amr.h"

typedef struct{
  // Mandatory. Array of float. Noise level table, in dB
  float* noise_level;
  size_t sz_noise_level;

  // Mandatory. Global frequency shift in Hz
  float freq_shift;

  // Mandatory. Array Each object contains the
  // parameters of a path
  path_amr_t* path;
  size_t sz_path;

} channel_dl_amr_t;

channel_dl_amr_t parse_channel_dl_amr(void* it);

#endif

