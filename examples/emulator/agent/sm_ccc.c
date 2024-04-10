#include "sm_ccc.h"
#include "../../../test/rnd/fill_rnd_data_ccc.h"
#include <assert.h>
#include <unistd.h>

void init_ccc_sm(void)
{
  // No allocation needed
}

void free_ccc_sm(void)
{
  // No allocation needed
}

bool read_ccc_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == RAN_CTRL_STATS_V1_03);
  assert(0!=0 && "Not implemented");
  return true;
}

void read_ccc_setup_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0);
  ccc_e2_setup_t* rc = (ccc_e2_setup_t*)data;
  rc->ran_func_def = fill_ccc_ran_func_def();
}

sm_ag_if_ans_t write_ctrl_ccc_sm(void const* data)
{
  assert(0!= 0 && "Not implemented");
  assert(data != NULL);

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0;
  return ans;
}

sm_ag_if_ans_t write_subs_ccc_sm(void const* src)
{
  assert(0!= 0 && "Not implemented");
  assert(src != NULL);
}
