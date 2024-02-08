#ifndef E2_SM_RAN_CTRL_READ_WRITE_AGENT_H
#define E2_SM_RAN_CTRL_READ_WRITE_AGENT_H

#include "agent/e2_agent_api.h"

void init_rc_sm(void);

void free_rc_sm(void);

bool read_rc_sm(void *);

void read_rc_setup_sm(void* data);

sm_ag_if_ans_t write_ctrl_rc_sm(void const* data);

sm_ag_if_ans_t write_subs_rc_sm(void const* src);

void proxy_fill_rnd_rc_ind_data(uint32_t ric_req_id, e2sm_rc_action_def_t ad, e2sm_rc_event_trigger_t et);

#endif

