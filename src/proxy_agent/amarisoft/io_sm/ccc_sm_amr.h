#ifndef CCC_SERVICE_MODEL_AMARISOFT_H
#define CCC_SERVICE_MODEL_AMARISOFT_H

#include "../../../sm/agent_if/ans/sm_ag_if_ans.h"

// CCC SM
void init_ccc_sm_amr(void);

void free_ccc_sm_amr(void);

void read_ccc_setup_sm_amr(void* data);

sm_ag_if_ans_t write_ctrl_ccc_sm_amr(void const* data);

bool read_ccc_sm_amr(void* data);

// Aperiodic events
sm_ag_if_ans_t write_subs_ccc_sm_amr(void const* src);

#endif

