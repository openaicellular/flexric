#ifndef ARRAY_CELL_AMARISOFT_MSG_STAT_MIR_H
#define ARRAY_CELL_AMARISOFT_MSG_STAT_MIR_H 

#include "cell_amr.h"
#include <stdlib.h>

typedef struct{
  cell_amr_t* cells;
  size_t sz;
  // I do not like this solution...
  char** names;
} arr_cell_amr_t;

void free_arr_cell_amr(arr_cell_amr_t* src);


#endif
