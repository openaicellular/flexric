#include "msg_ue_get.h"

#include <assert.h>

void free_msg_ue_get(msg_ue_get_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_ue_lst_amr(&src->ue_lst[i]);
  }

  free(src->ue_lst);
}

