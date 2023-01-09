/* 
 * Layer between flexric E2SM callbacks and WS interface
 * it manages SUBSCRIPTION procedure (indication) and CTRL procedure
 */
#include <stdio.h>

#include "../sm/mac_sm/ie/mac_data_ie.h"
#include "../sm/rlc_sm/ie/rlc_data_ie.h"
#include "../sm/pdcp_sm/ie/pdcp_data_ie.h"
#include "../sm/slice_sm/ie/slice_data_ie.h"
#include "../sm/tc_sm/ie/tc_data_ie.h"
#include "../sm/gtp_sm/ie/gtp_data_ie.h"
#include "../sm/kpm_sm_v2.02/ie/kpm_data_ie.h"

#include "e2_sm_agent.h"
#include "ws_ran_if.h"
#include "proxy_agent.h"

// TBC: using fake date as workaround until we are ready with real ringbuffer implementation
#include "../test/sm/common/fill_ind_data.h"
#define get_ringbuffer_kpm_data fill_kpm_ind_data

/* 
 * We arrive here when the timer set by subscription procedure in agent/msg_handler_agent.c:e2ap_handle_subscription_request_agent()
 * expires. 
 * XXX-BUG: you have a race condition with the WS timer. You should wait reading data until the first timer is expired. check thes issue 
 * descripion in ws_ran_if.c:loop_callback() use case LWS_CALLBACK_TIMER 
 */
void e2_read_RAN(sm_ag_if_rd_t *data)
{
  assert( data->type == MAC_STATS_V0    || 
          data->type == RLC_STATS_V0    || 
          data->type == PDCP_STATS_V0   || 
          data->type == SLICE_STATS_V0  || 
          data->type == KPM_STATS_V0    || 
          data->type == GTP_STATS_V0);

  if (data->type == KPM_STATS_V0)
  {    
    lwsl_user("[E2]: reading data from ringbuffer (fake)\n");
    get_ringbuffer_kpm_data(&data->kpm_stats);
  }
  else
  {
    assert("unsupported data type");
  }
}

sm_ag_if_ans_t e2_write_RAN(sm_ag_if_wr_t const *data)
{
  assert(data != NULL);
  if (data->type == MAC_CTRL_REQ_V0)
  {
    sm_ag_if_ans_t ans = {.type = MAC_AGENT_IF_CTRL_ANS_V0};
    ans.mac.ans = MAC_CTRL_OUT_OK;
    return ans;
  }
  else if (data->type == SLICE_CTRL_REQ_V0)
  {

    slice_ctrl_req_data_t const *slice_req_ctrl = &data->slice_req_ctrl;
    slice_ctrl_msg_t const *msg = &slice_req_ctrl->msg;

    if (msg->type == SLICE_CTRL_SM_V0_ADD)
    {
      printf("[E2]: SLICE CONTROL ADD rx.\n");
      printf("SLICE CONTROL ADD: unimplemented.\n");
    }
    else if (msg->type == SLICE_CTRL_SM_V0_DEL)
    {
      printf("[E2]: SLICE CONTROL DEL rx\n");
      printf("SLICE CONTROL DEL: unimplemented.\n");
    }
    else if (msg->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC)
    {
      printf("[E2]: SLICE CONTROL ASSOC rx\n");
      printf("SLICE CONTROL ASSOC: unimplemented.\n");
    }
    else
    {
      assert(0 != 0 && "Unknown msg_type!");
    }

    sm_ag_if_ans_t ans = {.type = SLICE_AGENT_IF_CTRL_ANS_V0};
    return ans;
  }
  else if (data->type == TC_CTRL_REQ_V0)
  {
    tc_ctrl_req_data_t const *ctrl = &data->tc_req_ctrl;

    tc_ctrl_msg_e const t = ctrl->msg.type;

    assert(t == TC_CTRL_SM_V0_CLS || t == TC_CTRL_SM_V0_PLC || t == TC_CTRL_SM_V0_QUEUE || t == TC_CTRL_SM_V0_SCH || t == TC_CTRL_SM_V0_SHP || t == TC_CTRL_SM_V0_PCR);

    sm_ag_if_ans_t ans = {.type = TC_AGENT_IF_CTRL_ANS_V0};
    return ans;
  }
  else
  {
    assert(0 != 0 && "Not supported function ");
  }
  sm_ag_if_ans_t ans = {0};
  return ans;
}
