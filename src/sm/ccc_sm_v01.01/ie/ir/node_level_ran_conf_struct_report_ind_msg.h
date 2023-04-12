#ifndef NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_INDICATION_MESSAGE_H
#define NODE_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_INDICATION_MESSAGE_H

#include "o_gnb_du_node_attribute_value.h"
#include "o_gnb_cu_cp_node_attribute_value.h"
#include "o_gnb_cu_up_node_attribute_value.h"
#include "o_rrm_policy_ratio_attribute_value.h"
#include "node_level_ran_conf_name_report.h"

typedef union {
    o_gnb_du_node_attribute_value_u *o_gnb_du;                        // 8.8.1.1  REPORT
    o_gnb_cu_cp_node_attribute_value_u *o_gnb_cu_cp;                  // 8.8.1.2  REPORT & CONTROL
    o_gnb_cu_up_node_attribute_value_u *o_gnb_cu_up;                  // 8.8.1.3  REPORT
    o_rrm_policy_ratio_attribute_value_u *o_rrm_policy_ratio;         // 8.8.1.4  REPORT & CONTROL

} values_of_attributes_node_report_u;


/*  8.2.1  Node-Level RAN Configuration Structures
           INDICATION MESSAGE                      */

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.1
    node_level_ran_conf_struct_name_report_e type;

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


} node_level_ran_conf_struct_report_ind_msg_t;

#endif
