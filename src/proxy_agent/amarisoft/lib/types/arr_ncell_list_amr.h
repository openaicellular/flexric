#ifndef ARR_NCELL_LIST_AMR_MIR_H
#define ARR_NCELL_LIST_AMR_MIR_H 

#include <stdlib.h>
#include "ncell_list_amr.h"

typedef struct{
  size_t sz;
  ncell_list_amr_t* ncell_lst;
} arr_ncell_list_amr_t;

void free_arr_ncell_list_amr(arr_ncell_list_amr_t* src);


#endif
