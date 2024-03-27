#ifndef ARR_RF_PORTS_AMR_H
#define ARR_RF_PORTS_AMR_H 

#include "rf_ports_amr.h"
#include <stdlib.h>

typedef struct{
  rf_ports_amr_t* rf_ports_amr;
  size_t sz;
} arr_rf_ports_amr_t;

void free_arr_rf_ports_amr(arr_rf_ports_amr_t* src);

arr_rf_ports_amr_t parse_arr_rf_ports_amr(void* it);

#endif
