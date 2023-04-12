#ifndef CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_2_SM_H

#include <stdlib.h>

#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"
#include "change_type.h"
#include "cell_level_ran_conf_struct_report_ind_msg.h"


typedef struct {
    // Mandatory
    // Change Type
    change_type_e change_type;

    // Mandatory
    // List of attribute values
    cell_level_ran_conf_struct_report_ind_msg_t cell_level_item;
    

} cell_level_conf_list_ind_msg_frm_2_t;


typedef struct {
    // Mandatory
    // Cell Global ID
    // 9.3.6
    cell_global_id_t cell_global_id;

    // Mandatory
    // List of Configuration Structures Reported
    size_t cell_level_conf_list_len;  // [1..65535]
    cell_level_conf_list_ind_msg_frm_2_t *cell_level_conf_list;

} cells_reported_list_t;


/*  9.2.1.4.2  E2SM-CCC Indication Message Format 2  */

typedef struct {
    // Mandatory
    // List of Cells Reported
    size_t cells_reported_list_len;  // [1..65535]
    cells_reported_list_t *cells_reported_list;

} e2sm_ccc_ind_msg_frm_2_t;


#endif
