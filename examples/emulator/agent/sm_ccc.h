#ifndef SM_CCC_READ_WRITE_AGENT_H
#define SM_CCC_READ_WRITE_AGENT_H

#include "../../../src/agent/e2_agent_api.h"
#include "../../../src/sm/ccc_sm/ccc_sm_id.h"

void init_ccc_sm(void);

void free_ccc_sm(void);

bool read_ccc_sm(void *);

void read_ccc_setup_sm(void* data);

sm_ag_if_ans_t write_ctrl_ccc_sm(void const* data);

sm_ag_if_ans_t write_subs_ccc_sm(void const* src);

#endif

