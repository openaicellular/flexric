#include "rc_msgs_amr.h"
#include <assert.h>


void free_rc_msgs_amr(rc_msgs_amr_t* src)
{
  assert(src != NULL);

  free_msg_ue_get(&src->ues); 
  free_config_get_amr(&src->cfg); 
  free_msg_ho_ans_amr(&src->ho); 
}

