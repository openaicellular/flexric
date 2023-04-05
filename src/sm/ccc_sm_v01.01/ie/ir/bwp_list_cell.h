#ifndef BWP_LIST_CELL_H
#define BWP_LIST_CELL_H

#include <stdlib.h>

#include "o_bwp_cell.h"

//  9.3.18  bWPList

typedef struct {
    size_t o_bwp_list_len;  //  [1..256]
    o_bwp_cell_t *o_bwp;  // 8.8.2.3


} bwp_list_t;


#endif
