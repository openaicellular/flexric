#ifndef DECRYPTION_JSON_EVENT_TRIGGER_FORMAT_1_CCC_V1_H
#define DECRYPTION_JSON_EVENT_TRIGGER_FORMAT_1_CCC_V1_H

#include <cjson/cJSON.h>
#include "../../ie/ir/e2sm_ccc_event_trigger_frm_1.h"

e2sm_ccc_ev_trg_frm_1_t dec_ccc_event_trigger_frm_1(const cJSON * ev_trg_json);


#endif
