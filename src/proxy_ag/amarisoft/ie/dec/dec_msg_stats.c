#include "dec_msg_stats.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "util/alg_ds/alg/defer.h"
#include "parse_cjson.h"

void dec_config_get_amr(const char* in, msg_stats_amr_t* out)
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

 



}



