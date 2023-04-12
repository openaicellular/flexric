#ifndef O_NR_CELL_CU_ATTRIBUTE_VALUE_H
#define O_NR_CELL_CU_ATTRIBUTE_VALUE_H

#include <stdint.h>

#include "plmn_info_list.h"


//  8.8.2.1  O-NRCellCU - REPORT & CONTROL services

typedef union {
    const uint64_t cell_local_id;  //  It identifies a NR cell of a gNB. 
    plmn_info_list_t plmn_info_list;  //  9.3.15

} o_nr_cell_cu_attribute_value_u;  //  for indication message



#endif
