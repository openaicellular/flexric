#ifndef CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_SM_H

#include "e2sm_ccc_event_trigger_frm_1.h"
#include "e2sm_ccc_event_trigger_frm_2.h"
#include "e2sm_ccc_event_trigger_frm_3.h"

typedef enum {
    FORMAT_1_EVENT_TRIGGER_CCC,
    FORMAT_2_EVENT_TRIGGER_CCC,
    FORMAT_3_EVENT_TRIGGER_CCC,

    END_EVENT_TRIGGER_CCC

} e2sm_ccc_event_trigger_e;


/*  9.2.1.1   RIC Event Trigger Definition IE  */

typedef struct {
    e2sm_ccc_event_trigger_e type;

    union {
        e2sm_ccc_ev_trg_frm_1_t frm_1;  //  9.2.1.1.1  Triggered upon subscription and when a node-level configuration change occurs.
        e2sm_ccc_ev_trg_frm_2_t frm_2;  //  9.2.1.1.2  Triggered upon subscription and when a cell-level configuration change occurs.
        e2sm_ccc_ev_trg_frm_3_t frm_3;  //  9.2.1.1.3  Used for periodic event triggering
    };

} e2sm_ccc_event_trigger_t;

void free_ccc_event_trigger_def(e2sm_ccc_event_trigger_t *src);

#endif

// to be finished; add ric styles...
