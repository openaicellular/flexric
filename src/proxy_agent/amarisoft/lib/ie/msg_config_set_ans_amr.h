#ifndef MESSAGE_CONFIG_SET_AMARISOFT_H
#define MESSAGE_CONFIG_SET_AMARISOFT_H

typedef struct{
  int msg_id;
  char* error;
  float time;
} msg_config_set_ans_amr_t;

void free_msg_config_set_ans_amr(msg_config_set_ans_amr_t* src);

#endif
