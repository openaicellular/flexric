#ifndef E2_AGENT_AMARISOFT_PROXY_MIR_H
#define E2_AGENT_AMARISOFT_PROXY_MIR_H 
#include "../../util/conf_file.h"
#include "asio_agent_amr.h"
#include "../../sm/sm_io.h"
#include "ep_amr.h"

#include <stdatomic.h>

typedef struct{
  args_proxy_ag_t args;
  // asio
  asio_agent_amr_t asio; 
  // endpoint
  ep_amr_t ep;
  // sm io 
  sm_io_ag_ran_t sm_io;

  atomic_int msg_id;
  atomic_bool stopped;
  atomic_bool stop_token;
} e2_agent_amr_t;

e2_agent_amr_t init_e2_agent_amr(args_proxy_ag_t const* args);

// Blocking call
void start_e2_agent_amr(e2_agent_amr_t* ag);

void free_e2_agent_amr(e2_agent_amr_t* ag);

#endif
