#ifndef NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_CONTROL_H
#define NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_CONTROL_H

#include "o_gnb_cu_cp_node_attribute_value.h"
#include "o_rrm_policy_ratio_attribute_value.h"
#include "node_level_ran_conf_name_control.h"

typedef union {
    o_gnb_cu_cp_node_attribute_value_u *o_gnb_cu_cp_function_node;     // 8.8.1.2  REPORT & CONTROL
    o_rrm_policy_ratio_attribute_value_u *o_rrm_policy_ratio_node;     // 8.8.1.4  REPORT & CONTROL

} values_of_attributes_node_control_u;


/*  8.2.1  Node-Level RAN Configuration Structures CONTROL  */

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.1
    node_level_ran_conf_struct_name_control_e type;

    // Mandatory
    // Old Values of Attributes
    // 8.8.1
    size_t old_values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_node_control_u old_values;  //  OCTET STRING

    // Mandatory
    // New Values of Attributes
    // 8.8.1
    size_t values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_node_control_u values;  /* new values (ctrl_msg);
                                                    current values (accepted list in ctrl_out);
                                                    requested values (failed list in ctrl_out);
                                                    OCTET STRING  */




} node_level_ran_conf_struct_control_t;

#endif
