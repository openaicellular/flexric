#include "plmn_id_json.h"
#include <string.h>
#include <regex.h>
#include <assert.h>
#include <stdbool.h>
#include "util/alg_ds/alg/defer.h"

static
bool isMCCValid(const char* in){
  regex_t regex;
  if (regcomp(&regex, "[0-9]{3}", REG_EXTENDED) != 0)
    return false;
  defer({regfree(&regex);});

  if (regexec(&regex, in, 0, NULL, 0))
    return false;
  return true;
}

static
bool isMNCValid(const char* in){
  regex_t regex;
  if (regcomp(&regex, "[0-9]{3}|[0-9]{2}", REG_EXTENDED) != 0)
    return false;
  defer({regfree(&regex);});

  if (regexec(&regex, in, 0, NULL, 0))
    return false;
  return true;
}

plmn_id_json_t * cJSON_Parseplmn_id(const char * s) {
  plmn_id_json_t * x = NULL;
  if (NULL != s) {
    cJSON * j = cJSON_Parse(s);
    if (NULL != j) {
      x = cJSON_Getplmn_idValue(j);
      cJSON_Delete(j);
    }
  }
  return x;
}

plmn_id_json_t * cJSON_Getplmn_idValue(const cJSON * j) {
  plmn_id_json_t * x = NULL;
  if (NULL != j) {
    if (NULL != (x = cJSON_malloc(sizeof(plmn_id_json_t)))) {
      memset(x, 0, sizeof(plmn_id_json_t));
      if (cJSON_HasObjectItem(j, "mcc")) {
        char * mcc = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(j, "mcc"));
        assert(isMCCValid(mcc) && "mcc is not 3 digits string");
        x->mcc = strdup(mcc);
      }
      if (cJSON_HasObjectItem(j, "mnc")) {
        char * mnc = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(j, "mnc"));
        assert(isMNCValid(mnc) && "mcc is not 3 or 2 digits string");
        x->mnc = strdup(mnc);
      }
    }
  }
  return x;
}

cJSON * cJSON_Createplmn_id(const plmn_id_json_t * x) {
  cJSON * j = NULL;
  if (NULL != x) {
    if (NULL != (j = cJSON_CreateObject())) {
      if (NULL != x->mcc) {
        cJSON_AddStringToObject(j, "mcc", x->mcc);
      }
      if (NULL != x->mnc) {
        cJSON_AddStringToObject(j, "mnc", x->mnc);
      }
    }
  }
  return j;
}

char * cJSON_Printplmn_id(const plmn_id_json_t * x) {
  char * s = NULL;
  if (NULL != x) {
    cJSON * j = cJSON_Createplmn_id(x);
    if (NULL != j) {
      s = cJSON_PrintUnformatted(j);
      cJSON_Delete(j);
    }
  }
  return s;
}

void cJSON_Deleteplmn_id(plmn_id_json_t * x) {
  if (NULL != x) {
    if (NULL != x->mcc) {
      cJSON_free(x->mcc);
    }
    if (NULL != x->mnc) {
      cJSON_free(x->mnc);
    }
    cJSON_free(x);
  }
}