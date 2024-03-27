#include "global_gnb_id_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

global_gnb_id_amr_t parse_global_gnb_id_amr(void* it_void)
{
  assert(it_void != NULL);
  global_gnb_id_amr_t dst = {0};


  cJSON const* it = it_void;

  // Mandatory. PLMN
  dst.plmn = parse_string(it, "plmn");

  // Mandatory. gnb_id_bits
  dst.gnb_id_bits = parse_int(it, "gnb_id_bits");

  // Mandatory. gnb_id
  dst.gnb_id = parse_int(it, "gnb_id");

  // Mandatory. gnb_name
  dst.gnb_name = parse_string(it, "gnb_name");

  return dst;
}

