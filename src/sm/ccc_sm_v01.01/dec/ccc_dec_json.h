#ifndef CCC_DECRYPTION_JSON_H
#define CCC_DECRYPTION_JSON_H

#include "../ie/ccc_data_ie.h"

typedef struct { } ccc_dec_json_t;

e2sm_ccc_event_trigger_t ccc_dec_event_trigger_json(size_t len, uint8_t const ev_trg[len]);

#endif
