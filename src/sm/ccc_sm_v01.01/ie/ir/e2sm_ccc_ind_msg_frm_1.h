#ifndef CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_1_SM_H

#include <stdlib.h>

#include "change_type.h"
#include "node_level_ran_conf_struct_report_ind_msg.h"


typedef struct {
    // Mandatory
    // Change Type
    change_type_e change_type;

    // Mandatory
    // List of attribute values
    node_level_ran_conf_struct_report_ind_msg_t node_level_item;


} node_level_conf_list_ind_msg_frm_1_t;


/*  9.2.1.4.1  E2SM-CCC Indication Message Format 1  */

typedef struct {
    // Mandatory
    // List of Configuration Structures Reported
    size_t node_level_conf_list_len;  //  [1..65535]
    node_level_conf_list_ind_msg_frm_1_t *node_level_conf_list;

} e2sm_ccc_ind_msg_frm_1_t;



#endif
