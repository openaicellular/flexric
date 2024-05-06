#include "arr_ncell_list_amr.h"

#include <assert.h>

void free_arr_ncell_list_amr(arr_ncell_list_amr_t* src)
{
  assert(src != NULL);
  for(size_t i = 0; i < src->sz; ++i){
    free_ncell_list_amr(&src->ncell_lst[i]);
  }
  free(src->ncell_lst);
}


