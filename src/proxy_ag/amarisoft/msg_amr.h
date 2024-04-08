#ifndef MESSAGE_AMARISOFT_MIR_H
#define MESSAGE_AMARISOFT_MIR_H 

#include "ie/config_get.h"
#include "ie/msg_stats.h"
#include "ie/msg_ue_get.h"

typedef enum{
  MSG_CONFIG_GET_AMR_E,
  MSG_STATS_AMR_E,
  MSG_UE_GET_E,

  END_MSG_AMR_E
} msg_amr_e;

typedef struct{
   msg_amr_e type;
   union{
     config_get_amr_t config;
     msg_stats_amr_t stats;
     msg_ue_get_t ue;
   };
} msg_amr_t;

void free_msg_amr(msg_amr_t* src);

#endif

