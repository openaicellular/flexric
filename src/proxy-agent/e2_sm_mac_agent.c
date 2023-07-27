#include <stdio.h>
#include <libwebsockets.h>

#include "sm/mac_sm/ie/mac_data_ie.h"
#include "../../test/rnd/fill_rnd_data_mac.h"
#include "util/time_now_us.h"

#include "e2_sm_agent.h"
#include "e2_sm_mac_agent.h"
#include "ran_if.h"
#include "proxy_agent.h"
#include "ran_msg_hdlr.h"
#include "ringbuffer.h"
#include "notif_e2_ran.h"


void read_mac_sm(void* data)
{
  sm_ag_if_rd_t *rd = (sm_ag_if_rd_t*)data;
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == MAC_STATS_V0);

  // TODO: need to complete the function to fill the MAC SM indication msg
  assert(0!=0 && "not support MAC SM for proxy agent\n");

  //lwsl_debug("[E2-Agent]: reading data from buffer\n");
  //ran_ind_t temp = get_ringbuffer_data();

  //lwsl_user("[E2-Agent]: filling MAC SM indication message\n");
}

void read_mac_setup_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == MAC_AGENT_IF_E2_SETUP_ANS_V0 );

  assert(0 !=0 && "Not supported");
}

sm_ag_if_ans_t write_ctrl_mac_sm(void const* data)
{
  mac_ctrl_req_data_t* mac_req_ctrl = (mac_ctrl_req_data_t* )data;
  sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR,
                            .ctrl.type = MAC_CTRL_REQ_V0};
  wr.ctrl.mac_ctrl.hdr = cp_mac_ctrl_hdr(&mac_req_ctrl->hdr);
  wr.ctrl.mac_ctrl.msg = cp_mac_ctrl_msg(&mac_req_ctrl->msg);
  // TODO: should not give sm_ag_if_wr_t
  fwd_e2_ran_ctrl(&get_proxy_agent()->ran_if, wr);

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = MAC_AGENT_IF_CTRL_ANS_V0;
  ans.ctrl_out.mac.ans = MAC_CTRL_OUT_OK;
  return ans;
}