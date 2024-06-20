#ifndef MSG_UE_GET_AMARISOFT_MIR_H
#define MSG_UE_GET_AMARISOFT_MIR_H 

#include <stdlib.h>
#include "../types/ue_lst_amr.h"

typedef struct{
  int msg_id;
  size_t sz;
  ue_lst_amr_t* ue_lst;
  float time;
} msg_ue_get_t;

void free_msg_ue_get(msg_ue_get_t* src);

#endif
