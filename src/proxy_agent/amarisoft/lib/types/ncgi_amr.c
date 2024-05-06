#include "ncgi_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_ncgi_amr(ncgi_amr_t* src)
{
  assert(src != NULL);

  // NR Cell Identity PLMN.
  free(src->plmn);
  
  // NR Cell Identity (36 bits)
  //int nci;
}


