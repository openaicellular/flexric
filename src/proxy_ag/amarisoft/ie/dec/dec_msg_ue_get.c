#include "dec_msg_ue_get.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "util/alg_ds/alg/defer.h"
#include "parse_cjson.h"

void dec_msg_ue_get_amr(const char* in, msg_ue_get_t* out)
{
  assert(in != NULL);
  assert(out != NULL);

  cJSON *json = cJSON_Parse(in);
  defer({ cJSON_Delete(json); });
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    assert(error_ptr != NULL);
    fprintf(stderr, "Error before: %s\n", error_ptr);
  }
  assert(json != NULL && "Error parsing the input json");

  ans_cjson_t ans = find_object(json, "ue_list");

  out->sz = cJSON_GetArraySize(ans.it);
  out->ue_lst = calloc(out->sz, sizeof(ue_lst_t));
  assert(out->ue_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < out->sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    out->ue_lst[i] = parse_ue_lst(tmp);
  }

  out->time = parse_float(json, "time" );
}

