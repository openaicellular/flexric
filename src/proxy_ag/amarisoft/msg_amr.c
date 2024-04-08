#include "msg_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_msg_amr(msg_amr_t* src)
{
  assert(src != NULL);

  if(src->type == MSG_READY_AMR_E){
  } else if(src->type == MSG_CONFIG_GET_AMR_E){
    free_config_get_amr(&src->config);
  } else if(src->type == MSG_STATS_AMR_E){
    free_msg_stats_amr(&src->stats);
  } else if(src->type == MSG_UE_GET_E){
    free_msg_ue_get(&src->ue);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

