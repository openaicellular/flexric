#ifndef DECRYPTION_CELL_GLOBAL_ID_JSON_H
#define DECRYPTION_CELL_GLOBAL_ID_JSON_H

#include <cjson/cJSON.h>
#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"

cell_global_id_t dec_ccc_cell_global_id_json(const cJSON * cell_global_id_json);


#endif
