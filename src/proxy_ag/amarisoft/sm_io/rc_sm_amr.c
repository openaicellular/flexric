#include "rc_sm_amr.h"
#include <stdio.h>
#include "../../../sm/agent_if/write/sm_ag_if_wr.h"
#include "../../../../test/rnd/fill_rnd_data_rc.h"

sm_ag_if_ans_t write_ctrl_rc_sm_amr(void const* data)
{
  assert(data != NULL);
  sm_ag_if_ans_t ans = {0};

  assert(0 != 0 && "Debug point");

  return ans;
}

void init_rc_sm_amr(void)
{

}

void free_rc_sm_amr(void)
{

}



void read_rc_setup_sm_amr(void* data)
{
  assert(data != NULL);

  rc_e2_setup_t* rc = (rc_e2_setup_t*)data;
  // This needs to be changed
  rc->ran_func_def = fill_rc_ran_func_def();
}

static
void free_aperiodic_subscription(uint32_t ric_req_id)
{
  (void)ric_req_id;
}

sm_ag_if_ans_t write_subs_rc_sm_amr(void const* src)
{
  assert(src != NULL);

  wr_rc_sub_data_t* wr_rc = (wr_rc_sub_data_t*)src;
  printf("ric req id %d \n", wr_rc->ric_req_id);


  assert(0!=0 && "Debug Point");
  //fwd_e2_ran_wr_sub_ev(&get_proxy_agent()->ran_if, wr_rc);

  sm_ag_if_ans_t ans = {.type = SUBS_OUTCOME_SM_AG_IF_ANS_V0};
  ans.subs_out.type = APERIODIC_SUBSCRIPTION_FLRC;
  ans.subs_out.aper.free_aper_subs = free_aperiodic_subscription;

  return ans;



  assert(0 != 0 && "Debug point");

  return ans;
}



