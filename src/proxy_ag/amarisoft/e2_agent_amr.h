#ifndef E2_AGENT_AMARISOFT_PROXY_MIR_H
#define E2_AGENT_AMARISOFT_PROXY_MIR_H 

#include "../../sm/sm_io.h"
#include "../../util/conf_file.h"
#include "../../util/alg_ds/ds/assoc_container/assoc_ht_open_address.h"
#include "asio_agent_amr.h"
#include "io_ran/ep_amr.h"
#include "io_ran/msg_amr.h"
#include "io_ran/kpm_msgs_amr.h"
#include "io_ran/rc_msgs_amr.h"

#include "lib/ie/msg_config_get.h"
#include "lib/ie/msg_stats.h"
#include "lib/ie/msg_ue_get.h"
#include "kpm_pend_ds.h"
#include "pending_event_proxy.h"
#include "rc_pend_ds.h"

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
  // KPM Pending events. SMs 
  kpm_pend_ds_t kpm_pend_ds;
  // RC Pending events. SMs 
  rc_pend_ds_t rc_pend_ds;
  // 



  // Message handler
  fp_msg_hndlr msg_hndl[END_MSG_AMR_E]; 

  // PLMN
  e2sm_plmn_t plmn; 
  
  atomic_int msg_id;
  atomic_bool stopped;
  atomic_bool stop_token;
} e2_agent_amr_t;

e2_agent_amr_t init_e2_agent_amr(args_proxy_ag_t const* args);

// Blocking call
void start_e2_agent_amr(e2_agent_amr_t* ag);

void free_e2_agent_amr(e2_agent_amr_t* ag);

// SM calls. Blocking
void fill_msg_kpm_sm(e2_agent_amr_t* ag, kpm_msgs_amr_t* msg);

void fill_msg_rc_sm(e2_agent_amr_t* ag, rc_msgs_amr_t* msg);

void ho_rc_sm(e2_agent_amr_t* ag, uint64_t pci, uint64_t ran_ue_id, size_t ssb_nr_arfcn, rc_msgs_amr_t* msg);

#endif
