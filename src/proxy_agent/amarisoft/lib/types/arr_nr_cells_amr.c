#include "arr_nr_cells_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


void free_arr_nr_cells_amr(arr_nr_cells_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_nr_cells_amr(&src->nr_cells[i]);
    free(src->names[i]);
  }

  free(src->nr_cells);
  free(src->names);
}


