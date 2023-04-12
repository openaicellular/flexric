#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_1_SM_H

#include <stdlib.h>

#include "node_level_ran_conf_struct_control.h"


/*  9.2.1.7.1  E2SM-CCC Control Message Format 1  */

typedef struct {
    // Mandatory
    // List of Configuration Structures
    size_t node_level_conf_list_len;  // [1..65535]
    node_level_ran_conf_struct_control_t *node_level_conf_list;


} e2sm_ccc_control_msg_frm_1_t;

#endif
