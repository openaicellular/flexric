#ifndef MESSAGE_HANDOVER_AMARISOFT_H
#define MESSAGE_HANDOVER_AMARISOFT_H 

typedef struct{
  int msg_id;
  char* error;
  float time;
} msg_ho_ans_amr_t;

void free_msg_ho_ans_amr(msg_ho_ans_amr_t* src);

#endif
