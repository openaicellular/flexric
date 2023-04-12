#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_2_SM_H

#include <stdlib.h>

#include "cell_level_ran_conf_struct_control.h"
#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"

typedef struct {
    // Mandatory
    // Cell Global ID
    // 9.3.6
    cell_global_id_t cell_global_id;

    // Mandatory
    // List of Configuration Structures
    size_t cell_level_conf_list_len;  // [1..65535]
    cell_level_ran_conf_struct_control_t *cell_level_conf_list;

} cells_list_control_msg_frm_2_t;


/*  9.2.1.7.2  E2SM-CCC Control Message Format 2  */

typedef struct {
    // Mandatory
    // List of Cells
    size_t cells_list_len;  // [1..65535]
    cells_list_control_msg_frm_2_t *cells_list;

} e2sm_ccc_control_msg_frm_2_t;


#endif
