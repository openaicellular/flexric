#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_OUTCOME_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_OUTCOME_FORMAT_2_SM_H

#include "../../../../util/byte_array.h"
#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"

#include "cell_level_ran_conf_struct_control.h"
#include "control_cause_failure.h"

typedef struct {
    // Mandatory
    // Cell Level RAN Configuration Structures CONTROL
    cell_level_ran_conf_struct_control_t ran_conf_struct_list;

    // Optional
    // Applied Timestamp
    byte_array_t *applied_tstamp;  //  Time RAN configuration change was applied by the RAN Function.



} cell_level_conf_accepted_list_t;


typedef struct {
    // Mandatory
    // Cell Level RAN Configuration Structures CONTROL
    cell_level_ran_conf_struct_control_t ran_conf_struct_list;

    // Mandatory
    // Cause
    // 9.3.11
    control_cause_failure_e cause;  // Provides the cause of the failure



} cell_level_conf_failed_list_t;

typedef struct {
    // Mandatory
    // Cell Global ID
    // 9.3.6
    cell_global_id_t cell_global_id;

    // Optional
    // RAN Configuration Structures Accepted List
    size_t cell_level_accepted_list_len;  //  [0..65535]
    cell_level_conf_accepted_list_t *cell_level_accepted_list;

    // Optional
    // RAN Configuration Structures Failed List
    size_t cell_level_failed_list_len;  //  [0..65535]
    cell_level_conf_failed_list_t *cell_level_failed_list;


} cells_list_control_outcome_frm_2_t;


/*  9.2.1.8.2  E2SM-CCC Control Outcome Format 2  */

typedef struct {
    // Optional
    // Received Timestamp
    byte_array_t *received_tstamp;  //  Time RIC Control Request message received by RAN Function over E2 interface.

    // Mandatory
    // List of Cells
    size_t cells_list_len;  // [1..65535]
    cells_list_control_outcome_frm_2_t *cells_list;

} e2sm_ccc_control_outcome_frm_2_t;


#endif
