#include "arr_tx_channels_amr.h"

#include <assert.h>
#include "arr_tx_channels_amr.h"

void free_arr_tx_channels_amr(arr_tx_channels_amr_t* src )
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_tx_channels_amr(&src->tx_channels_amr[i]); 
  }
  free(src->tx_channels_amr); 
}


