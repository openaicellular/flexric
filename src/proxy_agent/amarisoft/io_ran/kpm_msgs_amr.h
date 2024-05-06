#ifndef KPM_MSGS_AMR_H
#define KPM_MSGS_AMR_H

#include "../lib/ie/msg_config_get.h"
#include "../lib/ie/msg_stats.h"
#include "../lib/ie/msg_ue_get.h"

typedef struct {
  msg_stats_amr_t stats;
  msg_ue_get_t ues;
  msg_config_get_amr_t cfg;
} kpm_msgs_amr_t;

void free_kpm_msgs_amr( kpm_msgs_amr_t*);

#endif
