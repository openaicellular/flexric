#include "msg_handle_amr_ag.h"
#include "lib/dec/dec_msg_amr_json.h"

#include "../../agent/e2_agent_api.h"
#include "../../util/alg_ds/alg/defer.h"
#include "../../util/alg_ds/alg/find.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../util/compare.h"
#include "../../util/e.h"
#include "../../util/time_now_us.h"

#include "io_ran/send_msg_amr.h"
#include "io_ran/ep_amr.h"

#include "e2_agent_amr_api.h"
#include "kpm_pend_msg.h"
#include "rc_pend_msg.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>


typedef struct{
  int mcc;
  int mnc;
} mcc_mnc_t;  

static
mcc_mnc_t convert_plm_mcc_mnc(char const* plmn)
{
  assert(plmn != NULL);
  mcc_mnc_t dst = {0}; 

  char tmp[5] = {0};

  memcpy(tmp, plmn+3, 2);
  dst.mnc = atoi(tmp);

  memcpy(tmp, plmn, 3);
  dst.mcc = atoi(tmp);
  
  return dst;
}

/////
// KPM SM
/////

void send_msg_stats_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm)
{
  // Add kpm answer
  add_kpm_pend_ds(&ag->kpm_pend_ds, msg_id, kpm);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, CONFIG_MSG_STATS_PENDING_EVENT, KPM_ORIGINATED_MSG_E);

  // Send message 
  send_msg_stats(&ag->ep,msg_id);
}

void send_msg_ue_get_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm)
{
  // Add kpm answer
  add_kpm_pend_ds(&ag->kpm_pend_ds, msg_id, kpm);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, UE_GET_PENDING_EVENT, KPM_ORIGINATED_MSG_E);

  // Send message 
  send_msg_ue_get(&ag->ep,msg_id);
}

void send_config_get_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm)
{
  // Add kpm answer place holder
  add_kpm_pend_ds(&ag->kpm_pend_ds, msg_id, kpm);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, CONFIG_GET_PROXY_PENDING_EVENT, KPM_ORIGINATED_MSG_E);

  // Send message 
  send_config_get(&ag->ep,msg_id);
}


/////
// RC SM
/////
void send_msg_ue_get_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc)
{
  // Add rc answer
  add_rc_pend_ds(&ag->rc_pend_ds, msg_id, rc);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, UE_GET_PENDING_EVENT, RC_ORIGINATED_MSG_E);

  // Send message 
  send_msg_ue_get(&ag->ep,msg_id);
}

void send_config_get_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc)
{
  // Add rc answer place holder
  add_rc_pend_ds(&ag->rc_pend_ds, msg_id, rc);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, CONFIG_GET_PROXY_PENDING_EVENT, RC_ORIGINATED_MSG_E);

  // Send message 
  send_config_get(&ag->ep, msg_id);
}

void send_ho_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc, uint64_t pci, uint64_t ue_id, size_t ssb_nr_arfcn)
{
  assert(ag != NULL);
  assert(msg_id > 0);

  // Add rc answer place holder. In this case only the latch will be used
  add_rc_pend_ds(&ag->rc_pend_ds, msg_id, rc);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, HAND_OVER_PENDING_EVENT, RC_ORIGINATED_MSG_E);

  // Send message 
  send_ho(&ag->ep, msg_id, pci, ue_id, ssb_nr_arfcn);
}

/////
// CCC SM
/////

void send_config_set_ccc(e2_agent_amr_t* ag, int msg_id, ccc_pend_msg_t* ccc, uint64_t cell_id, uint64_t pusch_fixed_rb_start, uint64_t pusch_fixed_l_crb)
{
  assert(ag != NULL);
  assert(msg_id > 0);

  // Add ccc answer placeholder. In this case only the latch will be used
  add_ccc_pend_ds(&ag->ccc_pend_ds, msg_id, ccc);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, HAND_OVER_PENDING_EVENT, CCC_ORIGINATED_MSG_E);

  // Send message
  send_config_set(&ag->ep, msg_id, cell_id, pusch_fixed_rb_start, pusch_fixed_l_crb);
}

void msg_handle_ready(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  assert(ag->msg_id == 0 && "Ready message only interchanged at the beginning of connection");
  int const msg_id = ag->msg_id++; 

  // Send configuration as data needed to init the E2 Agent 

  // Add pending event
  add_pend_ev_prox(&ag->pend, &ag->asio ,msg_id, CONFIG_GET_PROXY_PENDING_EVENT, READY_MSG_ORIGINATED_MSG_E);
  // Send msg
  send_config_get(&ag->ep, msg_id); 
}

static
void init_e2_agent_emulator(e2_agent_amr_t* ag, msg_config_get_amr_t const* cfg)
{
  assert(ag != NULL);
  assert(cfg != NULL);
  mcc_mnc_t const tmp = convert_plm_mcc_mnc(cfg->global_gnb_id.plmn);
  ag->plmn.mcc = tmp.mcc;
  ag->plmn.mnc = tmp.mnc;
  ag->plmn.mnc_digit_len = 2;

  int nb_id = cfg->global_gnb_id.gnb_id;
  int cu_du_id = 0;
  e2ap_ngran_node_t ran_type = e2ap_ngran_gNB;
  sm_io_ag_ran_t io = ag->sm_io;
  fr_args_t const* args = &ag->args.fr_args;

  init_agent_api(ag->plmn.mcc, 
      ag->plmn.mnc, 
      ag->plmn.mnc_digit_len,
      nb_id,
      cu_du_id,
      ran_type,
      io,
      args);
}

void msg_handle_config_get(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  msg_config_get_amr_t const* cfg = &msg->config;

  int const first_msg_id = 0; 
  if(cfg->msg_id == first_msg_id){
    defer({ free_msg_amr((void*)msg); });
    // Remove pending event
    rm_pend_ev_prox(&ag->pend, &ag->asio, cfg->msg_id);
    init_e2_agent_emulator(ag, cfg);
    return; 
  } 

  ws_orig_msg_e const orig = rm_pend_ev_prox(&ag->pend, &ag->asio, cfg->msg_id);
  assert(orig == KPM_ORIGINATED_MSG_E || orig == RC_ORIGINATED_MSG_E);
  
  if(orig == KPM_ORIGINATED_MSG_E){
    // Get kpm answer. This is OK, as the pointer points to the stack
    // The other msgs still have a valid pointer to the kpm_pend_msg_t
    kpm_pend_msg_t* k = extract_kpm_pend_ds(&ag->kpm_pend_ds, cfg->msg_id);
    // Move memory ownership
    k->msg->cfg = *cfg;
    notify_part_filled_kp(k);
  } else { // orig == RC_ORIGINATED_MSG_E)
    rc_pend_msg_t* r = extract_rc_pend_ds(&ag->rc_pend_ds, cfg->msg_id);
    // Move memory ownership
    r->msg->cfg = *cfg;
    notify_part_filled_rp(r);
  }

}

void msg_handle_stats(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  msg_stats_amr_t const* s = &msg->stats; 
  ws_orig_msg_e const orig = rm_pend_ev_prox(&ag->pend, &ag->asio, s->msg_id);
  assert(orig == KPM_ORIGINATED_MSG_E);

  // Get kpm answer. This is OK, as the pointer points to the stack
  // The other msgs still have a valid pointer to the kpm_pend_msg_t
  kpm_pend_msg_t* k = extract_kpm_pend_ds(&ag->kpm_pend_ds, s->msg_id);

  // Move memory ownership
  k->msg->stats = *s;
  notify_part_filled_kp(k);
}

void msg_handle_ue_get(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  msg_ue_get_t const* ue = &msg->ue; 
  ws_orig_msg_e const orig = rm_pend_ev_prox(&ag->pend, &ag->asio, ue->msg_id);
  assert(orig == KPM_ORIGINATED_MSG_E || orig == RC_ORIGINATED_MSG_E);

  if(orig == KPM_ORIGINATED_MSG_E){
    kpm_pend_msg_t* k = extract_kpm_pend_ds(&ag->kpm_pend_ds, ue->msg_id);
    // Move memory ownership
    k->msg->ues = *ue;
    notify_part_filled_kp(k);
  } else { // orig == RC_ORIGINATED_MSG_E)
    rc_pend_msg_t* r = extract_rc_pend_ds(&ag->rc_pend_ds, ue->msg_id);
    // Move memory ownership
    r->msg->ues = *ue;
    notify_part_filled_rp(r);
  }
}

void msg_handle_ho(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
 
  msg_ho_ans_amr_t const* ho = &msg->ho; 
  ws_orig_msg_e const orig = rm_pend_ev_prox(&ag->pend, &ag->asio, ho->msg_id);
  assert(orig == RC_ORIGINATED_MSG_E);

  rc_pend_msg_t* r = extract_rc_pend_ds(&ag->rc_pend_ds, ho->msg_id);
  // Move memory ownership
  r->msg->ho = *ho;
  notify_part_filled_rp(r);
}

void msg_handle_config_set(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  msg_config_set_ans_amr_t const* cfg_set = &msg->config_set;
  ws_orig_msg_e const orig = rm_pend_ev_prox(&ag->pend, &ag->asio, cfg_set->msg_id);
  assert(orig == CCC_ORIGINATED_MSG_E);

  ccc_pend_msg_t* c = extract_ccc_pend_ds(&ag->ccc_pend_ds, cfg_set->msg_id);
  // Move memory ownership
  c->msg->cfg_set = *cfg_set;
  notify_part_filled_cccp(c);
}

void msg_handle_amr_ag(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  ag->msg_hndl[msg->type](ag, msg);
}

