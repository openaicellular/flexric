#include "arr_rf_ports_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_arr_rf_ports_amr(arr_rf_ports_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_rf_ports_amr(&src->rf_ports_amr[i]);
  }

  free(src->rf_ports_amr);
}


