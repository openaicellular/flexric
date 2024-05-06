#include "global_gnb_id_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_global_gnb_id_amr(global_gnb_id_amr_t* src)
{
  assert(src != NULL);

  // Mandatory. PLMN
  free(src->plmn);

  // Mandatory. gnb_id_bits
  //int  gnb_id_bits;

  // Mandatory. gnb_id
  //int gnb_id;

  // Mandatory. gnb_name
  free(src->gnb_name);
}


