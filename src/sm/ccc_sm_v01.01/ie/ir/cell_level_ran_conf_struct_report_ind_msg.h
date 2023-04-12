#ifndef CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_INDICATION_MESSAGE_H
#define CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_INDICATION_MESSAGE_H

#include "o_nr_cell_cu_attribute_value.h"
#include "o_nr_cell_du_attribute_value.h"
#include "o_bwp_attribute_value.h"
#include "o_rrm_policy_ratio_attribute_value.h"
#include "cell_level_ran_conf_struct_name_report.h"

typedef union {
    o_nr_cell_cu_attribute_value_u *o_nr_cell_cu_cell;              //  8.8.2.1
    o_nr_cell_du_attribute_value_u *o_nr_cell_du_cell;              //  8.8.2.2
    o_bwp_attribute_value_u *o_bwp_cell;                            //  8.8.2.3
    o_rrm_policy_ratio_attribute_value_u *o_rrm_policy_ratio_cell;  //  8.8.2.4

} values_of_attributes_cell_u;


/*  8.2.2  Cell-Level RAN Configuration Structures
           INDICATION MESSAGE                      */

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.2
    cell_level_ran_conf_struct_name_report_e type;

    // Mandatory
    // Values of Attributes
    // 8.8.2
    size_t values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_cell_u values;  //  OCTET STRING

    // Optional
    // Old Values of Attributes
    // 8.8.2
    size_t old_values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_cell_u *old_values;  //  OCTET STRING


} cell_level_ran_conf_struct_report_ind_msg_t;




#endif
