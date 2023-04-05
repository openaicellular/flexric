#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_OUTCOME_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_OUTCOME_FORMAT_1_SM_H

#include "../../../../util/byte_array.h"

#include "e2sm_ccc_control_msg_frm_1.h"
#include "control_cause_failure.h"

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.1
    node_level_ran_conf_struct_name_control_e type;

    // Mandatory
    // Old Values of Attributes
    // 8.8.1
    values_of_attributes_node_control_u old_values;

    // Mandatory
    // Current Values of Attributes
    // 8.8.1
    values_of_attributes_node_control_u current_values;

    // Optional
    // Applied Timestamp
    byte_array_t *applied_tstamp;  //  Time RAN configuration change was applied by the RAN Function.


} node_level_conf_accepted_list_t;

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.1
    node_level_ran_conf_struct_name_control_e type;

    // Mandatory
    // Old Values of Attributes
    // 8.8.1
    values_of_attributes_node_control_u old_values;

    // Mandatory
    // Requested Values of Attributes
    // 8.8.1
    values_of_attributes_node_control_u requested_values;

    // Mandatory
    // Cause
    // 9.3.11
    control_cause_failure_e cause;  // Provides the cause of the failure


} node_level_conf_failed_list_t;

/*  9.2.1.8.1  E2SM-CCC Control Outcome Format 1  */

typedef struct {
    // Optional
    // Received Timestamp
    byte_array_t *received_tstamp;  //  Time RIC Control Request message received by RAN Function over E2 interface.

    // Optional
    // RAN Configuration Structures Accepted List
    size_t node_level_accepted_list_len;  //  [0..65535]
    node_level_conf_accepted_list_t *node_level_accepted_list;

    // Optional
    // RAN Configuration Structures Failed List
    size_t node_level_failed_list_len;  //  [0..65535]
    node_level_conf_failed_list_t *node_level_failed_list;



} e2sm_ccc_control_outcome_frm_1_t;

#endif
