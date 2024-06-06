#ifndef CCC_MSGS_AMR_H
#define CCC_MSGS_AMR_H

#include "../lib/ie/msg_config_get.h"
#include "../lib/ie/msg_config_set_ans_amr.h"

typedef struct {
  msg_config_set_ans_amr_t cfg_set;
  msg_config_get_amr_t cfg_get;
} ccc_msgs_amr_t;

void free_ccc_msgs_amr(ccc_msgs_amr_t* src);

#endif


