#include "msg_ho_ans_amr.h"
#include <assert.h>
#include <stdlib.h>

void free_msg_ho_ans_amr(msg_ho_ans_amr_t* src)
{
  assert(src != NULL);
  assert(src->msg_id > -1);

  free(src->error);
}

