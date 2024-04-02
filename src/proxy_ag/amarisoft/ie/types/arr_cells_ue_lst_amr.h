#ifndef ARR_CELLS_UE_LST_AMR_MIR_H
#define ARR_CELLS_UE_LST_AMR_MIR_H 

#include <stdlib.h>
#include "cells_ue_lst_amr.h"

typedef struct{
  size_t sz;
  cells_ue_lst_amr_t* cell;
} arr_cells_ue_lst_amr_t;

arr_cells_ue_lst_amr_t parse_arr_cells_ue_lst_amr(void* it);

#endif
