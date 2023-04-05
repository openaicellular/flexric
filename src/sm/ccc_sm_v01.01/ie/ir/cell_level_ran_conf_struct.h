#ifndef CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_H
#define CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_H

#include "o_nr_cell_cu_cell.h"
#include "o_nr_cell_du_cell.h"
#include "o_bwp_cell.h"
#include "o_rrm_policy_ratio.h"

typedef enum {
    O_NR_Cell_CU_CELL_RAN_CONF_NAME,
    O_NR_Cell_DU_CELL_RAN_CONF_NAME,
    O_BWP_CELL_RAN_CONF_NAME,
    O_RRM_POLICY_RATIO_CELL_RAN_CONF_NAME,

    END_CELL_RAN_CONF_NAME

} cell_level_ran_conf_struct_name_e;

typedef union {
    o_nr_cell_cu_attributes_e *o_nr_cell_cu_cell;  //  8.8.2.1  REPORT & CONTROL
    o_nr_cell_du_attributes_e *o_nr_cell_du_cell;  //  8.8.2.2  REPORT
    o_bwp_attributes_e *o_bwp_cell;  //  8.8.2.3  REPORT & CONTROL
    o_rrm_policy_ratio_attributes_e *o_rrm_policy_ratio_cell;  //  8.8.2.4  REPORT & CONTROL

} cell_attributes_list_u;


//  8.2.2  Cell-Level RAN Configuration Structures

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.2
    cell_level_ran_conf_struct_name_e type;

    
    // Optional
    // List of Attributes
    // 8.8.2
    size_t attribute_name_list_len;  // [0..65535]  allocation within the union
    cell_attributes_list_u *attribute_name_list;


} cell_level_ran_conf_struct_t;



#endif
