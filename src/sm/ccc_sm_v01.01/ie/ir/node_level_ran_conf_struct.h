#ifndef NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_H
#define NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_H

#include "o_gnb_du_function_node.h"
#include "o_gnb_cu_cp_function_node.h"
#include "o_gnb_cu_up_function_node.h"
#include "o_rrm_policy_ratio.h"


typedef enum {
    O_GNB_DU_Function_NODE_RAN_CONF_NAME,
    O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME,
    O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME,
    O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME,

    END_NODE_RAN_CONF_NAME


} node_level_ran_conf_struct_name_e;


typedef union {
    o_gnb_du_attributes_e *o_gnb_du_function_node;                      // 8.8.1.1  REPORT
    o_gnb_cu_cp_attributes_e *o_gnb_cu_cp_function_node;                // 8.8.1.2  REPORT & CONTROL
    o_gnb_cu_up_attributes_e *o_gnb_cu_up_function_node;                // 8.8.1.3  REPORT
    o_rrm_policy_ratio_attributes_e *o_rrm_policy_ratio_node;           // 8.8.1.4  REPORT & CONTROL

        
} node_attributes_list_u;


//  8.2.1  Node-Level RAN Configuration Structures

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.1
    node_level_ran_conf_struct_name_e type;

    // Optional
    // List of Attributes
    // 8.8.1
    size_t attribute_name_list_len;  // [0..65535]  allocation within the union
    node_attributes_list_u *attribute_name_list;

} node_level_ran_conf_struct_t;

#endif
