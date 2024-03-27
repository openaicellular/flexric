#include "scell_list_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

scell_list_amr_t parse_scell_list_amr(void* it)
{
  assert(it != NULL);
  scell_list_amr_t dst = {0}; 

  // Mandatory. Cell identifier 
  dst.cell_id = parse_int(it, "cell_id");

  // Mandatory. Indicates if PUSCH transmission is allowed
  dst.ul_allowed = parse_int(it, "ul_allowed");

  return dst;
}

