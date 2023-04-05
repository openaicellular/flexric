#ifndef CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_INDICATION_MESSAGE_FORMAT_2_SM_H

#include <stdlib.h>

#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"
#include "change_type.h"
#include "cell_level_ran_conf_struct.h"


typedef union {
    o_nr_cell_cu_cell_level_u *o_nr_cell_cu_cell;  //  8.8.2.1
    o_nr_cell_du_cell_u *o_nr_cell_du_cell;  //  8.8.2.2
    o_bwp_cell_u *o_bwp_cell;  //  8.8.2.3
    o_rrm_policy_ratio_u *o_rrm_policy_ratio_cell;  //  8.8.2.4

} values_of_attributes_cell_u;


typedef struct {
    // Mandatory
    // Change Type
    change_type_e change_type;

    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.2
    cell_level_ran_conf_struct_name_e type;

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


} cell_level_conf_list_ind_msg_frm_2_t;


typedef struct {
    // Mandatory
    // Cell Global ID
    // 9.3.6
    cell_global_id_t cell_global_id;

    // Mandatory
    // List of Configuration Structures Reported
    size_t cell_level_conf_list_len;  // [1..65535]
    cell_level_conf_list_ind_msg_frm_2_t *cell_level_conf_list;

} cells_reported_list_t;


/*  9.2.1.4.2  E2SM-CCC Indication Message Format 2  */

typedef struct {
    // Mandatory
    // List of Cells Reported
    size_t cells_reported_list_len;  // [1..65535]
    cells_reported_list_t *cells_reported_list;

} e2sm_ccc_ind_msg_frm_2_t;


#endif
