#include "arr_cell_amr.h"

#include <assert.h>
#include <string.h>

void free_arr_cell_amr(arr_cell_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_cell_amr(&src->cells[i]);
    free(src->names[i]);
  }

  free(src->cells);
  free(src->names);
}


