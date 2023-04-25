#ifndef CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_2_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_2_SM_H

#include <stdlib.h>

#include "../../../../util/byte_array.h"
#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"
#include "cell_level_ran_conf_struct_report_et_ad.h"

typedef struct {
    // Optional
    // Cell Global ID
    // 9.3.6
    cell_global_id_t *cell_global_id;

    // Mandatory
    // List of RAN Configuration Structures
    size_t cell_level_ran_conf_struct_list_len;  //  [1..1024]
    cell_level_ran_conf_struct_report_et_ad_t *cell_level_ran_conf_struct_list;  // 8.2.2


} cell_level_conf_list_ev_trg_frm_2_t;


//  9.2.1.1.2  E2SM-CCC Event Trigger Definition Format 2: Cell-Level Configuration Change

typedef struct {
    // Mandatory
    // List of Cell-level Configuration Structures
    size_t cell_level_conf_list_len;  // [1..1024]
    cell_level_conf_list_ev_trg_frm_2_t *cell_level_conf_list;


} e2sm_ccc_ev_trg_frm_2_t;

void free_ccc_event_trigger_frm_2(e2sm_ccc_ev_trg_frm_2_t *src);

bool eq_ccc_event_trigger_frm_2(e2sm_ccc_ev_trg_frm_2_t const* m0,  e2sm_ccc_ev_trg_frm_2_t const* m1);

#endif
