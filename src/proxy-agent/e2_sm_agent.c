/* 
 * Layer between flexric E2SM callbacks and WS interface
 * it manages SUBSCRIPTION procedure (indication) and CTRL procedure
 */
#include <stdio.h>
#include <libwebsockets.h>

#include "util/time_now_us.h"

#include "e2_sm_agent.h"
#include "e2_sm_mac_agent.h"
#include "e2_sm_kpm_agent.h"
#include "e2_sm_rc_agent.h"
#include "ran_if.h"
#include "proxy_agent.h"
#include "ran_msg_hdlr.h"
#include "ringbuffer.h"
#include "notif_e2_ran.h"
#include "e2_read_setup_ran.h"

/* this function checks if SM is implemented and can be reported in E2SETUP as list of SM availables from RAN layer */
bool is_sm_whitelisted(int sm_id) 
{
  bool ret = false;
  int whitelisted_sm_proxy_agent[] = { SM_KPM_ID, SM_MAC_ID };
  for (size_t i = 0; i < sizeof(whitelisted_sm_proxy_agent)/sizeof(int); i++)
  {
    if (sm_id == whitelisted_sm_proxy_agent[i])
      return true;
  }
  return ret;
}

static
void init_read_ind_tbl(read_ind_fp (*read_ind_tbl)[SM_AGENT_IF_READ_V0_END])
{
  (*read_ind_tbl)[MAC_STATS_V0] = read_mac_sm;
  (*read_ind_tbl)[RLC_STATS_V0] = NULL;
  (*read_ind_tbl)[PDCP_STATS_V0] = NULL;
  (*read_ind_tbl)[SLICE_STATS_V0] = NULL;
  (*read_ind_tbl)[TC_STATS_V0] = NULL;
  (*read_ind_tbl)[GTP_STATS_V0] = NULL;
  (*read_ind_tbl)[KPM_STATS_V3_0] = read_kpm_sm;
  (*read_ind_tbl)[RAN_CTRL_STATS_V1_03] = NULL;
}

static
void init_read_setup_tbl(read_e2_setup_fp (*read_setup_tbl)[SM_AGENT_IF_E2_SETUP_ANS_V0_END])
{
  (*read_setup_tbl)[MAC_AGENT_IF_E2_SETUP_ANS_V0] = read_mac_setup_sm;
  (*read_setup_tbl)[RLC_AGENT_IF_E2_SETUP_ANS_V0] = NULL ;
  (*read_setup_tbl)[PDCP_AGENT_IF_E2_SETUP_ANS_V0] = NULL ;
  (*read_setup_tbl)[SLICE_AGENT_IF_E2_SETUP_ANS_V0] = NULL ;
  (*read_setup_tbl)[TC_AGENT_IF_E2_SETUP_ANS_V0] = NULL ;
  (*read_setup_tbl)[GTP_AGENT_IF_E2_SETUP_ANS_V0] = NULL ;
  (*read_setup_tbl)[KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0] = read_kpm_setup_sm ;
  (*read_setup_tbl)[RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0] = read_rc_setup_sm;
}

static
void init_write_ctrl( write_ctrl_fp (*write_ctrl_tbl)[SM_AGENT_IF_WRITE_CTRL_V0_END])
{
  // TODO:
  (*write_ctrl_tbl)[MAC_CTRL_REQ_V0] = write_ctrl_mac_sm;
  (*write_ctrl_tbl)[RLC_CTRL_REQ_V0] =  NULL;
  (*write_ctrl_tbl)[PDCP_CTRL_REQ_V0] =  NULL;
  (*write_ctrl_tbl)[SLICE_CTRL_REQ_V0] =  NULL;
  (*write_ctrl_tbl)[TC_CTRL_REQ_V0] =  NULL;
  (*write_ctrl_tbl)[GTP_CTRL_REQ_V0] =  NULL;
  (*write_ctrl_tbl)[RAN_CONTROL_CTRL_V1_03] =  NULL;
}


static
void init_write_subs(write_subs_fp (*write_subs_tbl)[SM_AGENT_IF_WRITE_SUBS_V0_END])
{
  (*write_subs_tbl)[MAC_SUBS_V0] = NULL;
  (*write_subs_tbl)[RLC_SUBS_V0] = NULL;
  (*write_subs_tbl)[PDCP_SUBS_V0] = NULL;
  (*write_subs_tbl)[SLICE_SUBS_V0] = NULL;
  (*write_subs_tbl)[TC_SUBS_V0] = NULL;
  (*write_subs_tbl)[GTP_SUBS_V0] = NULL;
  (*write_subs_tbl)[KPM_SUBS_V3_0] = NULL;
  (*write_subs_tbl)[RAN_CTRL_SUBS_V1_03] = NULL;
}

sm_io_ag_ran_t init_io_proxy_ag()
{
  sm_io_ag_ran_t io = {0};
  init_read_ind_tbl(&io.read_ind_tbl);
  init_read_setup_tbl(&io.read_setup_tbl);
#if defined(E2AP_V2) || defined(E2AP_V3)
  io.read_setup_ran = read_setup_ran;
#endif
  init_write_ctrl(&io.write_ctrl_tbl);
  init_write_subs(&io.write_subs_tbl);

  return io;
}

/*
 * We send the message into the notification system that will reach RAN interface that will take care of creating the serialization 
 * and send it to RAN endpoint. At RAN interface, this message will be then managed by I/O module at use case LWS_CALLBACK_USER, 
 * for receiving the commands from E2, and at LWS_CALLBACK_CLIENT_RECEIVE  for managing the reply to the RIC (i.e. forward the reply directly into SCTP).
 * This function is called by e2ap_handle_control_request_agent() --> sm.on_control() -> like `on_control_mac_sm_ag` that returns immediately.
 * I modified the e2ap_handle_control_request_agent() to ignore the answer of e2_write_RAN() in case of proxy_agent.
 */
//sm_ag_if_ans_t e2_write_RAN(void const *data)
//{
//  assert(data != NULL);
//
//  sm_ag_if_wr_t* wr = (sm_ag_if_wr_t*)data;
//  fwd_e2_ran_ctrl(&get_proxy_agent()->ran_if, *wr);
//
//  sm_ag_if_ans_t ignored_ans = {0};
//  return ignored_ans;
//}

