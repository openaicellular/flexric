#ifndef CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_2_SM_H

#include <stdlib.h>

#include "../../../../../util/byte_array.h"
#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"
#include "cell_level_ran_conf_struct.h"

// typedef struct {
//     // Mandatory
//     // 9.3.7  =>  8.3.1.2
//     byte_array_t ran_conf_struct_name;
    
//     // Optional
//     size_t attributes_list_len;  // [0..65535]
//     byte_array_t *attribute_name;   // 9.3.8


// } cell_level_ran_conf_struct_list_t;


typedef struct {
    // Optional
    // Cell Global ID
    // 9.3.6
    cell_global_id_t *cell_global_id;

    // Mandatory
    // List of RAN Configuration Structures
    size_t cell_level_ran_conf_struct_list_len;  //  [1..1024]
    cell_level_ran_conf_struct_t *cell_level_ran_conf_struct_list;  // 8.2.2


} cell_level_conf_list_ev_trg_frm_2_t;


//  9.2.1.1.2  E2SM-CCC Event Trigger Definition Format 2: Cell-Level Configuration Change

typedef struct {
    // Mandatory
    // List of Cell-level Configuration Structures
    size_t cell_level_conf_list_len;  // [1..1024]
    cell_level_conf_list_ev_trg_frm_2_t *cell_level_conf_list;


} e2sm_ccc_ev_trg_frm_2_t;


#endif
