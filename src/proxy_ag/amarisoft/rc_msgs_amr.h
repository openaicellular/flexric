#ifndef RC_MSGS_AMR_H
#define RC_MSGS_AMR_H

#include "ie/config_get.h"
#include "ie/msg_stats.h"
#include "ie/msg_ue_get.h"
#include "ie/msg_ho_ans_amr.h"

typedef struct {
  msg_ue_get_t ues;
  config_get_amr_t cfg;
  msg_ho_ans_amr_t ho;
} rc_msgs_amr_t;

void free_rc_msgs_amr(rc_msgs_amr_t* src);

#endif


