#ifndef CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_EVENT_TRIGGER_AND_ACTION_DEFINITION_H
#define CELL_LEVEL_RAN_CONFIGURATION_STRUCTURES_REPORT_EVENT_TRIGGER_AND_ACTION_DEFINITION_H

#include <stdlib.h>

#include "o_nr_cell_cu_attribute_name.h"
#include "o_nr_cell_du_attribute_name.h"
#include "o_bwp_attribute_name.h"
#include "o_rrm_policy_ratio_attribute_name.h"
#include "cell_level_ran_conf_struct_name_report.h"



typedef union {
    o_nr_cell_cu_attribute_name_e *o_nr_cell_cu_cell;              //  8.8.2.1  REPORT & CONTROL
    o_nr_cell_du_attribute_name_e *o_nr_cell_du_cell;              //  8.8.2.2  REPORT
    o_bwp_attribute_name_e *o_bwp_cell;                            //  8.8.2.3  REPORT & CONTROL
    o_rrm_policy_ratio_attribute_name_e *o_rrm_policy_ratio_cell;  //  8.8.2.4  REPORT & CONTROL

} cell_attribute_name_list_report_u;


/*  8.2.2  Cell-Level RAN Configuration Structures
           EVENT TRIGGER & ACTION DEFINITION          */

typedef struct {
    // Mandatory
    // RAN Configuration Structure Name
    // 9.3.7 => 8.2.2
    cell_level_ran_conf_struct_name_report_e type;

    
    // Optional
    // List of Attributes
    // 8.8.2
    size_t attribute_name_list_len;  // [0..65535]  allocation within the union
    cell_attribute_name_list_report_u attribute_name_list;


} cell_level_ran_conf_struct_report_et_ad_t;

void free_ccc_cell_level_ran_conf_struct_report_et_ad(cell_level_ran_conf_struct_report_et_ad_t *src);

#endif
