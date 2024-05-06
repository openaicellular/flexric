#ifndef RC_SERVICE_MODEL_AMARISOFT_H
#define RC_SERVICE_MODEL_AMARISOFT_H 

#include "../../../sm/agent_if/ans/sm_ag_if_ans.h"

// RC SM
void init_rc_sm_amr(void);

void free_rc_sm_amr(void);

void read_rc_setup_sm_amr(void* data);

sm_ag_if_ans_t write_ctrl_rc_sm_amr(void const* data);

bool read_rc_sm_amr(void* data);

// Aperiodic events
sm_ag_if_ans_t write_subs_rc_sm_amr(void const* src);

#endif

