#ifndef CELL_CONFIGURATION_AND_CONTROL_ACTION_DEFINITION_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_ACTION_DEFINITION_FORMAT_2_SM_H

#include <stdlib.h>

#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"
#include "report_type.h"
#include "cell_level_ran_conf_struct_report_et_ad.h"

typedef struct {
    // Mandatory
    // Report Type
    // 9.3.9
    report_type_ccc_e report_type;

    // Mandatory
    // Cell-Level RAN Configuration Structures
    // 8.2.2
    cell_level_ran_conf_struct_report_et_ad_t cell_level_ran_conf_struct;


} cell_level_conf_list_act_def_frm_2_t;


typedef struct {
    // Optional
    // Cell Global ID
    // 9.3.6
    cell_global_id_t *cell_global_id;

    // Mandatory
    // List of Cell-level RAN Configuration Structures
    size_t node_level_conf_list_len;  // [1..1024]
    cell_level_conf_list_act_def_frm_2_t *cell_level_conf_list_frm_2;


} cell_configurations_list_t;



/*  9.2.1.2.2  E2SM-CCC Action Definition Format 2  */

typedef struct {
    // Mandatory
    // List of Cell Configurations To Be Reported
    size_t cell_level_conf_list_len;  //  [1..1024]
    cell_configurations_list_t *cell_level_conf_list_frm_2;


} e2sm_ccc_action_def_frm_2_t;

#endif
