#ifndef CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_1_SM_H

#include <stdlib.h>

#include "change_type.h"
#include "node_level_ran_conf_struct.h"

typedef union {
    o_gnb_du_function_node_u *o_gnb_du_function_node;                      // 8.8.1.1  REPORT
    o_gnb_cu_cp_function_node_u *o_gnb_cu_cp_function_node;                // 8.8.1.2  REPORT & CONTROL
    o_gnb_cu_up_function_node_u *o_gnb_cu_up_function_node;                // 8.8.1.3  REPORT
    o_rrm_policy_ratio_u *o_rrm_policy_ratio_node;                         // 8.8.1.4  REPORT & CONTROL

} values_of_attributes_node_report_u;


typedef struct {
    // Mandatory
    // Change Type
    change_type_e change_type;

    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.1
    node_level_ran_conf_struct_name_e type;

    // Mandatory
    // Values of Attributes
    // 8.8.1
    size_t values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_node_report_u values;  //  OCTET STRING

    // Optional
    // Old Values of Attributes
    // 8.8.1
    size_t old_values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_node_report_u *old_values;  //  OCTET STRING

} node_level_conf_list_ind_msg_frm_1_t;


/*  9.2.1.4.1  E2SM-CCC Indication Message Format 1  */

typedef struct {
    // Mandatory
    // List of Configuration Structures Reported
    size_t node_level_conf_list_len;  //  [1..65535]
    node_level_conf_list_ind_msg_frm_1_t *node_level_conf_list;

} e2sm_ccc_ind_msg_frm_1_t;



#endif
