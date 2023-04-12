#ifndef CCC_ENCRYPTION_JSON_H
#define CCC_ENCRYPTION_JSON_H

#include "../ie/ccc_data_ie.h"

typedef struct { } ccc_enc_json_t;

byte_array_t ccc_enc_event_trigger_json(e2sm_ccc_event_trigger_t const* event_trigger);

#endif
