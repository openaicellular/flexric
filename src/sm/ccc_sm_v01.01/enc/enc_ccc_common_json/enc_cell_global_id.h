#ifndef ENCRYPTION_CELL_GLOBAL_ID_JSON_H
#define ENCRYPTION_CELL_GLOBAL_ID_JSON_H

#include <cjson/cJSON.h>
#include "../../../../lib/e2sm_common_ie/sm_common_ie/cell_global_id.h"

cJSON * enc_ccc_cell_global_id_json(const cell_global_id_t * cell_global_id);


#endif
