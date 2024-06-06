#include "ccc_msgs_amr.h"
#include <assert.h>
#include <stddef.h>


void free_ccc_msgs_amr(ccc_msgs_amr_t* src)
{
  assert(src != NULL);

  free_msg_config_set_ans_amr(&src->cfg_set);
  free_msg_config_get_amr(&src->cfg_get);
}

