#include "cell_amr.h"

#include <assert.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"


cell_amr_t parse_cell_amr(void* it)
{
  assert(it != NULL);
  cell_amr_t dst = {0};

  dst.dl_bitrate = parse_float(it, "dl_bitrate");
  dst.ul_bitrate = parse_float(it, "ul_bitrate");
  // dst.mbms_bitrate = parse_float(it, "mbms_bitrate");
  dst.dl_tx = parse_int(it, "dl_tx");
  dst.ul_tx = parse_int(it, "ul_tx");
  dst.dl_err = parse_int(it, "dl_err");
  dst.ul_err = parse_int(it, "ul_err");
  dst.dl_retx = parse_int(it, "dl_retx");
  dst.ul_retx = parse_int(it, "ul_retx");

  dst.dl_sched_users_min = parse_float(it, "dl_sched_users_min");
  dst.dl_sched_users_avg = parse_float(it, "dl_sched_users_avg");
  dst.dl_sched_users_max = parse_float(it, "dl_sched_users_max");
  dst.ul_sched_users_min = parse_float(it, "ul_sched_users_min");
  dst.ul_sched_users_avg = parse_float(it, "ul_sched_users_avg");
  dst.ul_sched_users_max = parse_float(it, "ul_sched_users_max");

  dst.dl_use_min = parse_float(it, "dl_use_min");
  dst.dl_use_max = parse_float(it, "dl_use_max");
  dst.dl_use_avg = parse_float(it, "dl_use_avg");
  dst.ul_use_min = parse_float(it, "ul_use_min");
  dst.ul_use_max = parse_float(it, "ul_use_max");
  dst.ul_use_avg = parse_float(it, "ul_use_avg");

  //dst.ctrl_use_min = parse_float(it, "ctrl_use_min");
  //dst.ctrl_use_avg = parse_float(it, "ctrl_use_avg");
  //dst.ctrl_use_max = parse_float(it, "ctrl_use_max");

  dst.ue_count_min = parse_int(it, "ue_count_min");
  dst.ue_count_max = parse_int(it, "ue_count_max");
  dst.ue_count_avg = parse_int(it, "ue_count_avg");
  dst.ue_active_count_min = parse_int(it, "ue_active_count_min");
  dst.ue_active_count_max = parse_int(it, "ue_active_count_max");
  dst.ue_active_count_avg = parse_int(it, "ue_active_count_avg");
  dst.ue_inactive_count_min = parse_int(it, "ue_inactive_count_min");
  dst.ue_inactive_count_max = parse_int(it, "ue_inactive_count_max");
  dst.ue_inactive_count_avg = parse_int(it, "ue_inactive_count_avg");

  // LTE erab values ignored!!

  dst.drb_count_min = parse_int(it, "drb_count_min");
  dst.drb_count_max = parse_int(it, "drb_count_max");
  dst.drb_count_avg = parse_int(it, "drb_count_avg");
 // dst.dl_gbr_use_min = parse_int(it, "dl_gbr_use_min");
 // dst.dl_gbr_use_max = parse_int(it, "dl_gbr_use_max");
 // dst.dl_gbr_use_avg = parse_int(it, "dl_gbr_use_avg");
 // dst.ul_gbr_use_min = parse_int(it, "ul_gbr_use_min");
 // dst.ul_gbr_use_max = parse_int(it, "ul_gbr_use_max");
 // dst.ul_gbr_use_avgint = parse_int(it, "ul_gbr_use_avgint");

  dst.counter = parse_cnt_msgs_amr(it);

  return dst;
}
