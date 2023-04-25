#ifndef CELL_CONFIGURATION_AND_CONTROL_ACTION_DEFINITION_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_ACTION_DEFINITION_FORMAT_1_SM_H

#include <stdlib.h>

#include "node_level_ran_conf_struct_report_et_ad.h"
#include "report_type.h"

typedef struct {
    // Mandatory
    // Report Type
    // 9.3.9
    report_type_ccc_e report_type;

    // Mandatory
    // Node-Level RAN Configuration Structures
    // 8.2.1
    node_level_ran_conf_struct_report_et_ad_t node_level_ran_conf_struct;

} node_level_conf_list_act_def_frm_1_t;



/*  9.2.1.2.1  E2SM-CCC Action Definition Format 1  */

typedef struct {
    // Mandatory
    // List of Node-level RAN Configuration Structures
    size_t node_level_conf_list_len;  // [1..256]
    node_level_conf_list_act_def_frm_1_t *node_level_conf_list_frm_1;

} e2sm_ccc_action_def_frm_1_t;

void free_ccc_action_def_frm_1(e2sm_ccc_action_def_frm_1_t *src);

bool eq_ccc_action_def_frm_1(e2sm_ccc_action_def_frm_1_t const* m0,  e2sm_ccc_action_def_frm_1_t const* m1);

#endif
