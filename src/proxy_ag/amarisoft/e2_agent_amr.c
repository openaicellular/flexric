#include "e2_agent_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "msg_amr.h"

#include "../../util/alg_ds/alg/defer.h"
#include "../../lib/async_event.h"
#include "sm_io/io_proxy_ag.h"
#include "msg_dec_amr_ag.h"
#include "msg_handle_amr_ag.h"

static inline
bool net_pkt(const e2_agent_amr_t* ag, int fd)
{
  assert(ag != NULL);
  assert(fd > 0);
  return fd == ag->ep.fd;
}

static
async_event_t next_async_event_agent_amr(e2_agent_amr_t* ag)
{
  assert(ag != NULL);

  int const fd = event_asio_agent_amr(&ag->asio);

  async_event_t e = {.type = UNKNOWN_EVENT,
                     .fd = fd};

  printf(" next_async_event_agent_amr happened \n");
  fflush(stdout);
  
  if(fd == -1){ // no event happened. Just for checking the stop_token condition
    printf(" CHECK_STOP_TOKEN_EVENT \n ");
    e.type = CHECK_STOP_TOKEN_EVENT;
  } else if (net_pkt(ag, fd) == true){

    printf(" WS_MSG_ARRIVED_EVENT  \n ");
    e.ws_msg = recv_ep_amr(&ag->ep);
    e.type = WS_MSG_ARRIVED_EVENT;
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
          //defer({ free(&e.ws_msg.buf); });
           
          msg_amr_t msg = msg_dec_amr_ag(&e.ws_msg);
          defer({ free_msg_amr(&msg); });

          msg_handle_amr_ag(ag, &msg);

          break;
        }
      case CHECK_STOP_TOKEN_EVENT:
        {
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
 
  // SM io
  init_io_proxy_ag(&dst.sm_io);

  // Message ID
  dst.msg_id = 0;
  dst.stop_token = false;
  dst.stopped = false;

  // Send config_get message to Amarisoft RAN 
  int const msg_id = dst.msg_id++; 
  send_config_get(&dst.ep, msg_id);

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
}

