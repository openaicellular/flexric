#ifndef ARR_NR_CELLS_AMR_H
#define ARR_NR_CELLS_AMR_H 

#include <stdlib.h>
#include "nr_cells_amr.h"

typedef struct{
  size_t sz;
  nr_cells_amr_t* nr_cells;
  // Don't like this solution
  char** names;
} arr_nr_cells_amr_t;

void free_arr_nr_cells_amr(arr_nr_cells_amr_t* src);

arr_nr_cells_amr_t parse_arr_nr_cells_amr(void* it);

#endif
