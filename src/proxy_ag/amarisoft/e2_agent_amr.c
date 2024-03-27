#include "e2_agent_amr.h"
#include <assert.h>
#include <stdlib.h>

#include "../../lib/async_event.h"

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

  int const fd = event_asio_agent_amr(&ag->io);

  async_event_t e = {.type = UNKNOWN_EVENT,
                     .fd = fd};

  if(fd == -1){ // no event happened. Just for checking the stop_token condition
    e.type = CHECK_STOP_TOKEN_EVENT;
  } else if (net_pkt(ag, fd) == true){
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
          defer({ free_ws_msg(&e.ws_msg); });
            
          amr_msg_t msg = amr_msg_dec_ag(&ag->ap, e.ws_msg);
          defer({ free_amr_msg(&msg); });

          amr_msg_t ans = amr_msg_handle_agent(ag, &msg);
          defer( { free_amr_msg(&ans); } );

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
  e2_agent_amr_t dst = {0}; 

  // asio
  init_asio_agent_amr(&dst.io);

  // endpoint
  dst.ep = init_ep_amr();


  dst->stop_token = false;
  dst->stopped = false;
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

  dst->stop_token = true;
  while(dst->stopped == false)
    usleep(1000);

  stop_ep_amr(&ag->ep);



}




