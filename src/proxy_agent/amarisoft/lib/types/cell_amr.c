#include "cell_amr.h"

#include <assert.h>
#include <string.h>

void free_cell_amr(cell_amr_t* src)
{
  assert(src != NULL);

//  float dl_bitrate;
//  float ul_bitrate;
//  float mbms_bitrate;
//  int dl_tx;
//  int ul_tx;
//  int dl_err;
//  int ul_err;
//  int dl_retx;
//  int ul_retx;
//
//  float dl_sched_users_min;
//  float dl_sched_users_avg;
//  float dl_sched_users_max;
//  float ul_sched_users_min;
//  float ul_sched_users_avg;
//  float ul_sched_users_max;
//
//  float dl_use_min;
//  float dl_use_max;
//  float dl_use_avg;
//  float ul_use_min;
//  float ul_use_max;
//  float ul_use_avg;
//
//  float ctrl_use_min;
//  float ctrl_use_avg;
//  float ctrl_use_max;
//
//  int ue_count_min;
//  int ue_count_max;
//  int ue_count_avg;
//  int ue_active_count_min;
//  int ue_active_count_max;
//  int ue_active_count_avg;
//  int ue_inactive_count_min;
//  int ue_inactive_count_max;
//  int ue_inactive_count_avg;
//
//  // LTE erab values ignored!!
//  int drb_count_min;
//  int drb_count_max;
//  int drb_count_avg;
//  int dl_gbr_use_min;
//  int dl_gbr_use_max;
//  int dl_gbr_use_avg;
//  int ul_gbr_use_min;
//  int ul_gbr_use_max;
//  int ul_gbr_use_avgint;

  free_cnt_msgs_amr(&src->counter);
}



