#ifndef O_NR_CELL_CU_CELL_H
#define O_NR_CELL_CU_CELL_H

#include <stdint.h>

#include "plmn_info_list.h"

typedef enum {
    CELL_LOCAL_ID_ATTRIBUTE_O_NR_CELL_CU,
    PLMN_INFO_LIST_ATTRIBUTE_O_NR_CELL_CU,

    END_ATTRIBUTE_O_NR_CELL_CU

} o_nr_cell_cu_attributes_e;  //  for event trigger and action definition


//  8.8.2.1  O-NRCellCU - REPORT & CONTROL services

typedef union {
    const uint64_t cell_local_id;  //  It identifies a NR cell of a gNB. 
    plmn_info_list_t plmn_info_list;  //  9.3.15

} o_nr_cell_cu_cell_level_u;  //  for indication message



#endif
