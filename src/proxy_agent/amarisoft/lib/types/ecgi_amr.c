#include "ecgi_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_ecgi_amr(ecgi_amr_t* src)
{
  assert(src != NULL);

  // Mandatory.  
  free(src->plmn);

  // Mandatory. Cell Identity. 28 bits
  // int eci; 
}
