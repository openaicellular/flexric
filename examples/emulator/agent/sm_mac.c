#include "sm_mac.h"
#include "../../../test/rnd/fill_rnd_data_mac.h"
#include <assert.h>
#include <stdio.h>

void read_mac_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == MAC_STATS_V0);

  mac_ind_data_t* mac = (mac_ind_data_t*)data;
  fill_mac_ind_data(mac);
}

void read_mac_setup_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == MAC_AGENT_IF_E2_SETUP_ANS_V0 );

  assert(0 !=0 && "Not supported");
}

sm_ag_if_ans_t write_ctrl_mac_sm(void const* data)
{
  assert(data != NULL);
  mac_ctrl_req_data_t const* mac_req_ctrl = (mac_ctrl_req_data_t const* )data; // &data->slice_req_ctrl;
  mac_ctrl_msg_t const* msg = &mac_req_ctrl->msg;

  printf("[E2-Agent]: MAC CONTROL rx\n");
  for (size_t i = 0; i < msg->ran_conf_len; i++) {
    // TODO
    printf("[E2-Agent]: ran_conf[%ld].isset_pusch_mcs %d\n", i, msg->ran_conf[i].isset_pusch_mcs);
    printf("[E2-Agent]: ran_conf[%ld].pusch_mcs %d\n", i, msg->ran_conf[i].pusch_mcs);
    printf("[E2-Agent]: ran_conf[%ld].rnti %d\n", i, msg->ran_conf[i].rnti);
  }

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = MAC_AGENT_IF_CTRL_ANS_V0;
  ans.ctrl_out.mac.ans = MAC_CTRL_OUT_OK;
  return ans;
}

