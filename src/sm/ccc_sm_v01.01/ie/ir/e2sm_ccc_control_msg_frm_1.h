#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_1_SM_H

#include <stdlib.h>

#include "node_level_ran_conf_struct.h"

typedef enum {
    O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_CONTROL,
    O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_CONTROL,

    END_NODE_RAN_CONF_NAME_CONTROL


} node_level_ran_conf_struct_name_control_e;


typedef union {
    o_gnb_cu_cp_function_node_u *o_gnb_cu_cp_function_node;                // 8.8.1.2  REPORT & CONTROL
    o_rrm_policy_ratio_u *o_rrm_policy_ratio_node;                         // 8.8.1.4  REPORT & CONTROL

} values_of_attributes_node_control_u;


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
    // Values of Attributes
    // 8.8.1
    values_of_attributes_node_control_u new_values;


} node_level_conf_list_control_msg_frm_1_t;


/*  9.2.1.7.1  E2SM-CCC Control Message Format 1  */

typedef struct {
    // Mandatory
    // List of Configuration Structures
    size_t node_level_conf_list_len;  // [1..65535]
    node_level_conf_list_control_msg_frm_1_t *node_level_conf_list;


} e2sm_ccc_control_msg_frm_1_t;

#endif
