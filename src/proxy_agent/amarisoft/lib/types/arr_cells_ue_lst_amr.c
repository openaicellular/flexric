#include "arr_cells_ue_lst_amr.h"

#include <assert.h>
#include <string.h>

void free_arr_cells_ue_lst_amr( arr_cells_ue_lst_amr_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_cells_ue_lst_amr(&src->cell[i]);
  }

  free(src->cell);
}


