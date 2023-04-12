#ifndef CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_CONTROL_H
#define CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_CONTROL_H

#include "o_nr_cell_cu_attribute_value.h"
#include "o_bwp_attribute_value.h"
#include "o_rrm_policy_ratio_attribute_value.h"
#include "cell_level_ran_conf_struct_name_control.h"

typedef union {
    o_nr_cell_cu_attribute_value_u *o_nr_cell_cu_cell;  //  8.8.2.1  REPORT & CONTROL
    o_bwp_attribute_value_u *o_bwp_cell;  //  8.8.2.3  REPORT & CONTROL
    o_rrm_policy_ratio_attribute_value_u *o_rrm_policy_ratio_cell;  //  8.8.2.4  REPORT & CONTROL

} values_of_attributes_cell_control_u;


/*  8.2.2  Cell-Level RAN Configuration Structures CONTROL  */

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.2
    cell_level_ran_conf_struct_name_control_e type;

    // Mandatory
    // Old Values of Attributes
    // 8.8.2
    size_t old_values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_cell_control_u old_values;

    // Mandatory
    // Values of Attributes
    // 8.8.2
    size_t values_list_len;  // [1..65535] allocation within the union
    values_of_attributes_cell_control_u values;  /* new values (ctrl_msg);
                                                    current values (accepted list in ctrl_out);
                                                    requested values (failed list in ctrl_out);
                                                    OCTET STRING  */


} cell_level_ran_conf_struct_control_t;


#endif
