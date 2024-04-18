#include "kpm_msgs_amr.h"
#include <assert.h>


void free_kpm_msgs_amr(kpm_msgs_amr_t* src)
{
  assert(src != NULL);

  free_msg_stats_amr(&src->stats); 
  free_msg_ue_get(&src->ues); 
  free_config_get_amr(&src->cfg); 
}

