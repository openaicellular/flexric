#include "ncgi_amr.h"

#include <assert.h>
#include <stdlib.h>

#include "../dec/parse_cjson.h"

ncgi_amr_t parse_ncgi_amr(void* it_void)
{
  assert(it_void != NULL);

  ncgi_amr_t dst = {0};

  ans_cjson_t ans_ncgi = find_object(it_void, "ncgi");
  cJSON const* it = ans_ncgi.it;

  // NR Cell Identity PLMN.
  dst.plmn= parse_string(it, "plmn");

  // NR Cell Identity (36 bits)
  dst.nci = parse_int(it, "nci");

  return dst;
}

