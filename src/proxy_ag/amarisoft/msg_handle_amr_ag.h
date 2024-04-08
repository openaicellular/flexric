#ifndef MSG_HANDLE_AMR_AG_MIR_H
#define MSG_HANDLE_AMR_AG_MIR_H 

#include "e2_agent_amr.h"
#include "msg_amr.h"
#include "../../sm/agent_if/ans/sm_ag_if_ans.h"


void msg_handle_amr_ag(e2_agent_amr_t const* ag, msg_amr_t const* msg);


// KPM SM
void init_kpm_sm_amr(void);
void free_kpm_sm_amr(void);
bool read_kpm_sm_amr(void* data);
void read_kpm_setup_sm_amr(void* e2ap);

// RC SM
void init_rc_sm_amr(void);
void free_rc_sm_amr(void);
void read_rc_setup_sm_amr(void* data);
sm_ag_if_ans_t write_ctrl_rc_sm_amr(void const* data);
sm_ag_if_ans_t write_subs_rc_sm_amr(void const* src);

// CCC SM



void send_config_get(ep_amr_t const* ep, int msg_id);

void send_msg_stats(ep_amr_t const* ep, int msg_id);

void send_msg_ue_get(ep_amr_t const* ep, int msg_id);

#endif
