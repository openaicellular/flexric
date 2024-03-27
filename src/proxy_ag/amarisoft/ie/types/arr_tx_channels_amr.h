#ifndef ARRAY_TX_CHANNELS_AMR_MIR_H
#define ARRAY_TX_CHANNELS_AMR_MIR_H

#include "tx_channels_amr.h"
#include <stdlib.h>

typedef struct{
  tx_channels_amr_t* tx_channels_amr; 
  size_t sz;
} arr_tx_channels_amr_t;

void free_arr_tx_channels_amr(arr_tx_channels_amr_t* src );

arr_tx_channels_amr_t parse_arr_tx_channels_amr(void* it);

#endif



