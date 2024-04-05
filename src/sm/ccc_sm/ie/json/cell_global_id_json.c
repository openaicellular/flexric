#include "cell_global_id_json.h"
#include <string.h>

cell_global_id_json_t * cJSON_Parsecell_global_id(const char * s) {
  cell_global_id_json_t * x = NULL;
  if (NULL != s) {
    cJSON * j = cJSON_Parse(s);
    if (NULL != j) {
      x = cJSON_Getcell_global_idValue(j);
      cJSON_Delete(j);
    }
  }
  return x;
}

cell_global_id_json_t * cJSON_Getcell_global_idValue(const cJSON * j) {
  cell_global_id_json_t * x = NULL;
  if (NULL != j) {
    if (NULL != (x = cJSON_malloc(sizeof(cell_global_id_json_t)))) {
      memset(x, 0, sizeof(cell_global_id_json_t));
      if (cJSON_HasObjectItem(j, "nRCellIdentity")) {
        x->n_r_cell_identity = strdup(cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(j, "nRCellIdentity")));
      }
      else {
        if (NULL != (x->n_r_cell_identity = cJSON_malloc(sizeof(char)))) {
          x->n_r_cell_identity[0] = '\0';
        }
      }
      if (cJSON_HasObjectItem(j, "plmnIdentity")) {
        x->plmn_identity = cJSON_Getplmn_identityValue(cJSON_GetObjectItemCaseSensitive(j, "plmnIdentity"));
      }
    }
  }
  return x;
}

cJSON * cJSON_Createcell_global_id(const cell_global_id_json_t * x) {
  cJSON * j = NULL;
  if (NULL != x) {
    if (NULL != (j = cJSON_CreateObject())) {
      if (NULL != x->n_r_cell_identity) {
        cJSON_AddStringToObject(j, "nRCellIdentity", x->n_r_cell_identity);
      }
      else {
        cJSON_AddStringToObject(j, "nRCellIdentity", "");
      }
      cJSON_AddItemToObject(j, "plmnIdentity", cJSON_Createplmn_identity(x->plmn_identity));
    }
  }
  return j;
}

char * cJSON_Printcell_global_id(const cell_global_id_json_t * x) {
  char * s = NULL;
  if (NULL != x) {
    cJSON * j = cJSON_Createcell_global_id(x);
    if (NULL != j) {
      s = cJSON_PrintUnformatted(j);
      cJSON_Delete(j);
    }
  }
  return s;
}

void cJSON_Deletecell_global_id(cell_global_id_json_t * x) {
  if (NULL != x) {
    if (NULL != x->n_r_cell_identity) {
      cJSON_free(x->n_r_cell_identity);
    }
    if (NULL != x->plmn_identity) {
      cJSON_Deleteplmn_identity(x->plmn_identity);
    }
    cJSON_free(x);
  }
}
