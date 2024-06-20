#include "arr_rx_channels_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_arr_rx_channels_amr(arr_rx_channels_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_rx_channels_amr(&src->rx_channels_amr[i]);
  }
 
  free(src->rx_channels_amr);
}


