#include "e2_agent_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "msg_amr.h"

#include "../../util/alg_ds/alg/defer.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../util/compare.h"
#include "../../util/time_now_us.h"
#include "../../lib/async_event.h"
#include "sm_io/io_proxy_ag.h"
#include "msg_dec_amr_ag.h"
#include "msg_handle_amr_ag.h"
#include "kpm_pend_msg.h"
#include "rc_pend_msg.h"
#include "send_msg_amr.h"
#include "kpm_msgs_amr.h"
#include "rc_msgs_amr.h"

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
          int64_t t0 = time_now_us(); 
          msg_amr_t msg = msg_dec_amr_ag(&e.ws_msg);
          int64_t t1 = time_now_us(); 
          printf("Elapsed decoding %ld \n", t1 -t0);
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
  init_io_proxy_ag(&dst.sm_io);

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
   
  // Message handler
  dst.msg_hndl[MSG_READY_AMR_E] = msg_handle_ready; 
  dst.msg_hndl[MSG_CONFIG_GET_AMR_E] = msg_handle_config_get; 
  dst.msg_hndl[MSG_STATS_AMR_E] = msg_handle_stats; 
  dst.msg_hndl[MSG_UE_GET_E] = msg_handle_ue_get; 
  dst.msg_hndl[MSG_HANDOVER_E] = msg_handle_ho; 

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

  free_io_proxy_ag(&ag->sm_io);

  free_pend_ev_prox(&ag->pend);

  // Pending events KPM SMs
  free_kpm_pend_ds(&ag->kpm_pend_ds);
 
  // Pending events RC SMs
  free_rc_pend_ds(&ag->rc_pend_ds);
}

void fill_msg_kpm_sm(e2_agent_amr_t* ag, kpm_msgs_amr_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  int64_t t1 = time_now_us();

  kpm_pend_msg_t kpm = init_kpm_pend_msg(msg);
  defer({ free_kpm_pend_msg(&kpm); } );
 
  int const msg_id0 = ag->msg_id++;
  send_msg_stats_kpm(ag, msg_id0, &kpm);

  int const msg_id1 = ag->msg_id++;
  send_msg_ue_get_kpm(ag, msg_id1, &kpm);

  int64_t t2 = time_now_us();

  int const msg_id2 = ag->msg_id++;
  send_config_get_kpm(ag, msg_id2, &kpm);

  int64_t t3 = time_now_us();

  // Other thread will notify it, once
  // the kpm_msgs_amr_t answer is completely filled 
  wait_untill_filled_kp(&kpm);

  int64_t t4 = time_now_us();

  printf("Elapsed %ld %ld %ld \n", t2 - t1, t3 - t2, t4 - t3);
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

void ho_rc_sm(e2_agent_amr_t* ag, uint64_t pci, uint64_t ran_ue_id, rc_msgs_amr_t* msg)
{
  assert(ag != NULL);

  rc_pend_msg_t rc = {.ho = &msg->ho};
  defer({ free_rc_pend_msg(&rc); } );

  rc.latch = init_latch_cv(1);
  defer({ free_latch_cv(&rc.latch); });

  int const msg_id = ag->msg_id++;
  send_ho_rc(ag, msg_id, &rc, pci, ran_ue_id);

  wait_untill_filled_rp(&rc);
}

