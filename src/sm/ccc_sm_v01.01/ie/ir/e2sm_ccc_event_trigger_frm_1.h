#ifndef CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_1_SM_H

#include <stdlib.h>

#include "node_level_ran_conf_struct_report_et_ad.h"

#include "../../../../util/byte_array.h"


// 9.2.1.1.1  E2SM-CCC Event Trigger Definition Format 1: Node-Level Configuration Change

typedef struct {
    // Mandatory
    // List of Node-level Configuration Structures
    size_t node_level_conf_list_len;  // [1..256]
    node_level_ran_conf_struct_report_et_ad_t *node_level_ran_conf_struct_list;  // 8.2.1


} e2sm_ccc_ev_trg_frm_1_t;

void free_ccc_event_trigger_frm_1(e2sm_ccc_ev_trg_frm_1_t *src);

bool eq_ccc_event_trigger_frm_1(e2sm_ccc_ev_trg_frm_1_t const* m0,  e2sm_ccc_ev_trg_frm_1_t const* m1);

#endif
