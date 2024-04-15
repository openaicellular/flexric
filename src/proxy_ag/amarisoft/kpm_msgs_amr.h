#ifndef KPM_MSGS_AMR_H
#define KPM_MSGS_AMR_H

#include "ie/config_get.h"
#include "ie/msg_stats.h"
#include "ie/msg_ue_get.h"

typedef struct {
  msg_stats_amr_t stats;
  msg_ue_get_t ues;
  config_get_amr_t cfg;
} kpm_msgs_amr_t;

#endif
