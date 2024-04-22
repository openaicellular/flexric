#include "dec_msg_ho_ans_amr.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util/alg_ds/alg/defer.h"
#include "parse_cjson.h"

void dec_msg_ho_ans_amr(const char* in, msg_ho_ans_amr_t* out)
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

  // Message ID. Mandatory 
  out->msg_id = parse_int(json, "message_id"); 
  
  // Error. Optional
  ans_cjson_t ans = find_object(json, "error");
  if(ans.it != NULL){
    assert(ans.it->valuestring != NULL); 
    size_t const sz = strlen(ans.it->valuestring);
    out->error = calloc(sz + 1, sizeof(char));
    assert(out->error != NULL && "Memory exhausted");
    memcpy(out->error, ans.it->valuestring, sz);
  }

  // Time
  out->time = parse_float(json, "time");  
}

