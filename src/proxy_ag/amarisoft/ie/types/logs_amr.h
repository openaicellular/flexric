#ifndef LOGS_AMARISOFT_MIR_H
#define LOGS_AMARISOFT_MIR_H 

#include <stdbool.h>
#include <stdlib.h>
#include "layers_amr.h"

typedef struct{

  // Mandatory. Layers
  layers_amr_t* layers;
  size_t sz_layers;

  // Mandatory. Count
  int count;

  // Optional. Rotate
  int* rotate;

  // Optional. Path
  char* path;

  // Mandatory bcch
  // True if BCCH dump is enabled (eNB only)
  bool bcch;

  // Mandatory mib
  // True if MIB dump is enabled (eNB only).
  bool mib;

} logs_amr_t;

void free_logs_amr(logs_amr_t* src);

// Parsing 
logs_amr_t parse_logs_amr(void* it); 

#endif
