#ifndef NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_EVENT_TRIGGER_AND_ACTION_DEFINITION_H
#define NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_EVENT_TRIGGER_AND_ACTION_DEFINITION_H

#include "o_gnb_du_node_attribute_name.h"
#include "o_gnb_cu_cp_node_attribute_name.h"
#include "o_gnb_cu_up_node_attribute_name.h"
#include "o_rrm_policy_ratio_attribute_name.h"
#include "node_level_ran_conf_name_report.h"

typedef union {
    o_gnb_du_node_attribute_name_e *o_gnb_du;                           // 8.8.1.1  REPORT
    o_gnb_cu_cp_node_attribute_name_e *o_gnb_cu_cp;                     // 8.8.1.2  REPORT & CONTROL
    o_gnb_cu_up_node_attribute_name_e *o_gnb_cu_up;                     // 8.8.1.3  REPORT
    o_rrm_policy_ratio_attribute_name_e *o_rrm_policy_ratio_node;       // 8.8.1.4  REPORT & CONTROL

        
} node_attribute_name_list_report_u;


/*  8.2.1  Node-Level RAN Configuration Structures
           EVENT TRIGGER & ACTION DEFINITION        */

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.1
    node_level_ran_conf_struct_name_report_e type;

    // Optional
    // List of Attributes
    // 8.8.1
    size_t attribute_name_list_len;  // [0..65535]  allocation within the union
    node_attribute_name_list_report_u attribute_name_list;

} node_level_ran_conf_struct_report_et_ad_t;

#endif
