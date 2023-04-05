#ifndef CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_1_SM_H

#include <stdlib.h>

#include "node_level_ran_conf_struct.h"

#include "../../../../../util/byte_array.h"

// typedef struct {
//     // Mandatory
//     // 9.3.7  =>  8.3.1.1
//     node_level_ran_conf_struct_t ran_conf_struct_name;
    
//     // Optional
//     size_t attributes_list_len;  // [0..65535]
//     byte_array_t *attribute_name;   // 9.3.8


// } node_level_conf_list_ev_trg_frm_1_t;


// 9.2.1.1.1  E2SM-CCC Event Trigger Definition Format 1: Node-Level Configuration Change

typedef struct {
    // Mandatory
    // List of Node-level Configuration Structures
    size_t node_level_conf_list_len;  // [1..256]
    node_level_ran_conf_struct_t *node_level_ran_conf_struct_list;  // 8.2.1


} e2sm_ccc_ev_trg_frm_1_t;



#endif
