#ifndef ARRAY_RX_CHANNELS_AMR_MIR_H
#define ARRAY_RX_CHANNELS_AMR_MIR_H

#include "rx_channels_amr.h"
#include <stdlib.h>

typedef struct{
  rx_channels_amr_t* rx_channels_amr; 
  size_t sz;
} arr_rx_channels_amr_t;

void free_arr_rx_channels_amr(arr_rx_channels_amr_t* src );

arr_rx_channels_amr_t parse_arr_rx_channels_amr(void* it);


#endif

