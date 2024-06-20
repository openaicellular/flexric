#include "logs_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


void free_logs_amr(logs_amr_t* src)
{
  // Mandatory. Layers
  for(size_t i = 0; i < src->sz_layers; ++i){
    free_layers_amr(&src->layers[i]);
  }
  free(src->layers);

  // Mandatory. Count
  // int count;

  // Optional. Rotate
  free(src->rotate); 

  // Optional. Path
  free(src->path);

  // Mandatory bcch
  // True if BCCH dump is enabled (eNB only)
  // bool bcch;

  // Mandatory mib
  // True if MIB dump is enabled (eNB only).
  // bool mib;
}



