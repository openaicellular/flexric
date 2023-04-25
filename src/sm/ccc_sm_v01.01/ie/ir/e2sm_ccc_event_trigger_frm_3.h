#ifndef CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_3_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_EVENT_TRIGGER_FORMAT_3_SM_H

#include <stdint.h>
#include <stdbool.h>


//  9.2.1.1.3  E2SM-CCC Event Trigger Definition Format 3: Periodic Event Trigger

typedef struct {
    uint32_t report_period;  // [10..4294967295], Used to indicate the event triggering period in unit of 1 millisecond

} e2sm_ccc_ev_trg_frm_3_t;

void free_ccc_event_trigger_frm_3(e2sm_ccc_ev_trg_frm_3_t *src);

bool eq_ccc_event_trigger_frm_3(e2sm_ccc_ev_trg_frm_3_t const* m0,  e2sm_ccc_ev_trg_frm_3_t const* m1);

#endif
