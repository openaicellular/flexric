#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_MESSAGE_FORMAT_2_SM_H

#include <stdlib.h>

#include "cell_level_ran_conf_struct.h"
#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"

typedef enum {
    O_NR_Cell_CU_CELL_RAN_CONF_NAME_CONTROL,
    O_BWP_CELL_RAN_CONF_NAME_CONTROL,
    O_RRM_POLICY_RATIO_CELL_RAN_CONF_NAME_CONTROL,

    END_CELL_RAN_CONF_NAME_CONTROL

} cell_level_ran_conf_struct_name_control_e;

typedef union {
    o_nr_cell_cu_attributes_e *o_nr_cell_cu_cell;  //  8.8.2.1  REPORT & CONTROL
    o_bwp_attributes_e *o_bwp_cell;  //  8.8.2.3  REPORT & CONTROL
    o_rrm_policy_ratio_attributes_e *o_rrm_policy_ratio_cell;  //  8.8.2.4  REPORT & CONTROL

} values_of_attributes_cell_control_u;

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.2
    cell_level_ran_conf_struct_name_control_e type;

    // Mandatory
    // Old Values of Attributes
    // 8.8.2
    values_of_attributes_cell_control_u old_values;

    // Mandatory
    // Values of Attributes
    // 8.8.2
    values_of_attributes_cell_control_u new_values;


} cell_level_conf_list_control_msg_frm_2_t;


typedef struct {
    // Mandatory
    // Cell Global ID
    // 9.3.6
    cell_global_id_t cell_global_id;

    // Mandatory
    // List of Configuration Structures
    size_t cell_level_conf_list_len;  // [1..65535]
    cell_level_conf_list_control_msg_frm_2_t *cell_level_conf_list;

} cells_list_control_msg_frm_2_t;


/*  9.2.1.7.2  E2SM-CCC Control Message Format 2  */

typedef struct {
    // Mandatory
    // List of Cells
    size_t cells_list_len;  // [1..65535]
    cells_list_control_msg_frm_2_t *cells_list;

} e2sm_ccc_control_msg_frm_2_t;


#endif
