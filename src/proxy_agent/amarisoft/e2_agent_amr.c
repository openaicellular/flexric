#include "e2_agent_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "io_ran/msg_amr.h"

#include "../../util/alg_ds/alg/defer.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../util/compare.h"
#include "../../util/time_now_us.h"
#include "../../lib/async_event.h"

#include "msg_handle_amr_ag.h"
#include "kpm_pend_msg.h"
#include "rc_pend_msg.h"
#include "ccc_pend_msg.h"

#include "lib/dec/dec_msg_amr_json.h"

#include "io_ran/send_msg_amr.h"
#include "io_ran/kpm_msgs_amr.h"
#include "io_ran/rc_msgs_amr.h"
#include "io_ran/ccc_msgs_amr.h"
#include "io_sm/init_sm_amr.h"

static inline
bool net_pkt(const e2_agent_amr_t* ag, int fd)
{
  assert(ag != NULL);
  assert(fd > 0);
  return fd == ag->ep.fd;
}

static
bool time_out_pkt(e2_agent_amr_t* ag, int fd)
{
  assert(ag != NULL);
  assert(fd > 0);

  exp_msg_id_t dst = find_pend_ev_prox(&ag->pend, fd);
  return dst.found;
}

static
async_event_t next_async_event_agent_amr(e2_agent_amr_t* ag)
{
  assert(ag != NULL);

  int const fd = event_asio_agent_amr(&ag->asio);

  async_event_t e = {.type = UNKNOWN_EVENT,
                     .fd = fd};

  if(fd == -1){ // no event happened. Just for checking the stop_token condition
    e.type = CHECK_STOP_TOKEN_EVENT;
  } else if (net_pkt(ag, fd) == true){
    e.ws_msg = recv_ep_amr(&ag->ep);
    e.type = WS_MSG_ARRIVED_EVENT;
  } else if (time_out_pkt(ag, fd) == true){
    e.type = WS_MSG_TIME_OUT_PENDING;
  } else {
    assert(0!=0 && "Unknown event happened!");
  }
  return e;
}

static
void e2_event_loop_agent_amr(e2_agent_amr_t* ag)
{
  assert(ag != NULL);
  while(ag->stop_token == false){

    async_event_t e = next_async_event_agent_amr(ag); 
    assert(e.type != UNKNOWN_EVENT && "Unknown event triggered ");

    switch(e.type){
      case WS_MSG_ARRIVED_EVENT:
        {
          defer({ free(e.ws_msg.buf); });
          msg_amr_t msg = msg_dec_amr_ag(&e.ws_msg);
          
          // Memory ownership of msg is passed to the handle function
          msg_handle_amr_ag(ag, &msg);

          break;
        }
      case CHECK_STOP_TOKEN_EVENT:
        {
          break;
        }
      case WS_MSG_TIME_OUT_PENDING:
        {
          assert(0 != 0 && "Time out for pending event");
          break;
        }
      default:
        {
          assert(0!=0 && "Unknown event happened");
          break;
        }
    }
  }
  printf("[PROXY E2 AGENT]: ag->agent_stopped = true \n");
  ag->stopped = true;
}

e2_agent_amr_t init_e2_agent_amr(args_proxy_ag_t const* args)
{
  assert(args != NULL);

  // endpoint
  e2_agent_amr_t dst = {.ep = init_ep_amr(args->ip, args->port)}; 

  // asio
  init_asio_agent_amr(&dst.asio);
  add_fd_asio_agent_amr(&dst.asio, dst.ep.fd);

  // SM io
  init_sm_amr(&dst.sm_io);

  // Message ID
  dst.msg_id = 0;
 
  // Pending events
  init_pend_ev_prox(&dst.pend);

  // Copy Args ???
  dst.args = *args;

  // Pending events KPM SMs
  init_kpm_pend_ds(&dst.kpm_pend_ds);

  // Pending events RC SMs
  init_rc_pend_ds(&dst.rc_pend_ds);

  // Pending events CCC SMs
  init_ccc_pend_ds(&dst.ccc_pend_ds);

  // Message handler
  dst.msg_hndl[MSG_READY_AMR_E] = msg_handle_ready; 
  dst.msg_hndl[MSG_CONFIG_GET_AMR_E] = msg_handle_config_get; 
  dst.msg_hndl[MSG_STATS_AMR_E] = msg_handle_stats; 
  dst.msg_hndl[MSG_UE_GET_E] = msg_handle_ue_get; 
  dst.msg_hndl[MSG_HANDOVER_E] = msg_handle_ho;
  dst.msg_hndl[MSG_CONFIG_SET_E] = msg_handle_config_set;

  dst.stop_token = false;
  dst.stopped = false;

  return dst;
}

// Blocking call
void start_e2_agent_amr(e2_agent_amr_t* ag)
{
  assert(ag != NULL);
  e2_event_loop_agent_amr(ag);
}

void free_e2_agent_amr(e2_agent_amr_t* ag)
{
  assert(ag != NULL);

  ag->stop_token = true;
  while(ag->stopped == false)
    usleep(1000);

  stop_ep_amr(&ag->ep);

  free_sm_amr(&ag->sm_io);

  free_pend_ev_prox(&ag->pend);

  // Pending events KPM SMs
  free_kpm_pend_ds(&ag->kpm_pend_ds);
 
  // Pending events RC SMs
  free_rc_pend_ds(&ag->rc_pend_ds);

  // Pending events CCC SMs
  free_ccc_pend_ds(&ag->ccc_pend_ds);
}

void fill_msg_kpm_sm(e2_agent_amr_t* ag, kpm_msgs_amr_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  int64_t const t0 = time_now_us();

  kpm_pend_msg_t kpm = init_kpm_pend_msg(msg);
  defer({ free_kpm_pend_msg(&kpm); } );
 
  int const msg_id0 = ag->msg_id++;
  send_msg_stats_kpm(ag, msg_id0, &kpm);

  int const msg_id1 = ag->msg_id++;
  send_msg_ue_get_kpm(ag, msg_id1, &kpm);

  int const msg_id2 = ag->msg_id++;
  send_config_get_kpm(ag, msg_id2, &kpm);

  // Other thread will notify it, once
  // the kpm_msgs_amr_t answer is completely filled 
  wait_untill_filled_kp(&kpm);

  int64_t const t1 = time_now_us();

  printf("[PROXY-AGENT]: Elapsed websocket+fetch messages %ld\n", t1 - t0);
}

void fill_msg_rc_sm(e2_agent_amr_t* ag, rc_msgs_amr_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  int64_t t1 = time_now_us();

  rc_pend_msg_t rc = init_rc_pend_msg(msg);
  defer({ free_rc_pend_msg(&rc); } );
 
  int const msg_id1 = ag->msg_id++;
  send_msg_ue_get_rc(ag, msg_id1, &rc);

  int64_t t2 = time_now_us();

  int const msg_id2 = ag->msg_id++;
  send_config_get_rc(ag, msg_id2, &rc);

  int64_t t3 = time_now_us();

  // Other thread will notify it, once
  // the rc_msgs_amr_t answer is completely filled 
  wait_untill_filled_rp(&rc);

  int64_t t4 = time_now_us();

  printf("Elapsed RAN Control get %ld %ld %ld \n", t2 - t1, t3 - t2, t4 - t3);
}

void fill_msg_ccc_sm(e2_agent_amr_t* ag, ccc_msgs_amr_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  int64_t t1 = time_now_us();

  ccc_pend_msg_t ccc = init_ccc_pend_msg(msg);
  defer({ free_ccc_pend_msg(&ccc); } );

  int const msg_id1 = ag->msg_id++;
  send_config_get_ccc(ag, msg_id1, &ccc);

  // Other thread will notify it, once
  // the ccc_msgs_amr_t answer is completely filled
  wait_untill_filled_cccp(&ccc);

  int64_t t2 = time_now_us();

  printf("Elapsed CCC get %ld \n", t2 - t1);
}

void ho_rc_sm(e2_agent_amr_t* ag, uint64_t pci, uint64_t ran_ue_id, size_t ssb_nr_arfcn, rc_msgs_amr_t* msg)
{
  assert(ag != NULL);

  rc_pend_msg_t rc = {.msg = msg};
  defer({ free_rc_pend_msg(&rc); } );

  rc.latch = init_latch_cv(1);
  defer({ free_latch_cv(&rc.latch); });

  int const msg_id = ag->msg_id++;
  send_ho_rc(ag, msg_id, &rc, pci, ran_ue_id, ssb_nr_arfcn);

  wait_untill_filled_rp(&rc);
}

void config_set_rb_ctrl_ul_ccc_sm(e2_agent_amr_t* ag, uint64_t cell_id, uint64_t pusch_fixed_rb_start, size_t pusch_fixed_l_crb, ccc_msgs_amr_t* msg)
{
  assert(ag != NULL);

  ccc_pend_msg_t ccc = {.msg = msg};
  defer({ free_ccc_pend_msg(&ccc); } );

  ccc.latch = init_latch_cv(1);
  defer({ free_latch_cv(&ccc.latch); });

  int const msg_id = ag->msg_id++;
  send_config_set_rb_ctrl_ul_ccc(ag, msg_id, &ccc, cell_id, pusch_fixed_rb_start, pusch_fixed_l_crb);

  wait_untill_filled_cccp(&ccc);
}


void config_set_rb_ctrl_dl_ccc_sm(e2_agent_amr_t* ag, uint64_t cell_id, uint64_t pdsch_fixed_rb_start, size_t pdsch_fixed_l_crb, ccc_msgs_amr_t* msg)
{
  assert(ag != NULL);

  ccc_pend_msg_t ccc = {.msg = msg};
  defer({ free_ccc_pend_msg(&ccc); } );

  ccc.latch = init_latch_cv(1);
  defer({ free_latch_cv(&ccc.latch); });

  int const msg_id = ag->msg_id++;
  send_config_set_rb_ctrl_dl_ccc(ag, msg_id, &ccc, cell_id, pdsch_fixed_rb_start, pdsch_fixed_l_crb);

  wait_untill_filled_cccp(&ccc);
}
