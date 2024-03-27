#ifndef ARR_NCELL_LIST_AMR_MIR_H
#define ARR_NCELL_LIST_AMR_MIR_H 

#include <stdlib.h>
#include "ncell_list_amr.h"

typedef struct{
  size_t sz;
  ncell_list_amr_t* ncell_lst;
} arr_ncell_list_amr_t;

arr_ncell_list_amr_t parse_arr_ncell_list_amr(void* it);

#endif
