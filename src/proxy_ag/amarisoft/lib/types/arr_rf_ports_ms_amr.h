#ifndef ARR_RF_PORTS_MESSAGE_STATS_AMR_MIR_H
#define ARR_RF_PORTS_MESSAGE_STATS_AMR_MIR_H 

#include <stdlib.h>

#include "rf_ports_ms_amr.h"

typedef struct{
  rf_ports_ms_amr_t* rf_ports_ms_amr;
  size_t sz;
  char** name;
} arr_rf_ports_ms_amr_t;

void free_arr_rf_ports_ms_amr( arr_rf_ports_ms_amr_t* src);

#endif
