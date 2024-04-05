#ifndef FLEXRIC_CELL_GLOBAL_ID_CCC_H
#define FLEXRIC_CELL_GLOBAL_ID_CCC_H

#include "plmn_identity_json.h"
#include "cJSON.h"

typedef struct {
  char * n_r_cell_identity;
  plmn_identity_json_t * plmn_identity;
} cell_global_id_json_t;

cell_global_id_json_t * cJSON_Parsecell_global_id(const char * s);
cell_global_id_json_t * cJSON_Getcell_global_idValue(const cJSON * j);
cJSON * cJSON_Createcell_global_id(const cell_global_id_json_t * x);
char * cJSON_Printcell_global_id(const cell_global_id_json_t * x);
void cJSON_Deletecell_global_id(cell_global_id_json_t * x);

#endif
