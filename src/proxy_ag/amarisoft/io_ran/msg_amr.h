#ifndef MESSAGE_AMARISOFT_MIR_H
#define MESSAGE_AMARISOFT_MIR_H 

#include "../lib/ie/msg_config_get.h"
#include "../lib/ie/msg_stats.h"
#include "../lib/ie/msg_ue_get.h"
#include "../lib/ie/msg_ho_ans_amr.h"

typedef enum{
  MSG_READY_AMR_E,
  MSG_CONFIG_GET_AMR_E,
  MSG_STATS_AMR_E,
  MSG_UE_GET_E,
  MSG_HANDOVER_E,

  END_MSG_AMR_E
} msg_amr_e;

typedef struct{
   msg_amr_e type;
   union{
     msg_config_get_amr_t config;
     msg_stats_amr_t stats;
     msg_ue_get_t ue;
     msg_ho_ans_amr_t ho;
   };
} msg_amr_t;

void free_msg_amr(msg_amr_t* src);

#endif

