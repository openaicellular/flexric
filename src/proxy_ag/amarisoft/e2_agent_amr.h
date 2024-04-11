#ifndef E2_AGENT_AMARISOFT_PROXY_MIR_H
#define E2_AGENT_AMARISOFT_PROXY_MIR_H 
#include "../../util/conf_file.h"
#include "asio_agent_amr.h"
#include "../../sm/sm_io.h"
#include "ep_amr.h"
#include "pending_event_proxy.h"
#include "ie/config_get.h"
#include "ie/msg_stats.h"
#include "ie/msg_ue_get.h"
#include "msg_amr.h"

#include <stdatomic.h>

// Forward declaration
typedef struct e2_agent_amr_s e2_agent_amr_t;

typedef void (*fp_msg_hndlr)(e2_agent_amr_t* ag,  msg_amr_t const* msg); 

typedef struct e2_agent_amr_s{
  args_proxy_ag_t args;
  // asio
  asio_agent_amr_t asio; 
  // endpoint
  ep_amr_t ep;
  // sm io 
  sm_io_ag_ran_t sm_io;
  // Pending events. Websocket
  pend_ev_prox_t pend;
  // Pending events. SMs 
  // key: msg_id, val: kpm_sm_pend_t*
  assoc_rb_tree_t kpm_pend;  
  pthread_mutex_t mtx_kpm_pend;

  // Message handler
  fp_msg_hndlr msg_hndl[END_MSG_AMR_E]; 

  atomic_int msg_id;
  atomic_bool stopped;
  atomic_bool stop_token;
} e2_agent_amr_t;

e2_agent_amr_t init_e2_agent_amr(args_proxy_ag_t const* args);

// Blocking call
void start_e2_agent_amr(e2_agent_amr_t* ag);

void free_e2_agent_amr(e2_agent_amr_t* ag);

// SM calls. Blocking
void fill_msg_kpm_sm(e2_agent_amr_t* ag, msg_stats_amr_t *stats, msg_ue_get_t *ues);

#endif
