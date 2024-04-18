#include "msg_handle_amr_ag.h"
#include "msg_dec_amr_ag.h"
#include "ep_amr.h"
#include "kpm_pend_msg.h"
#include "rc_pend_msg.h"
#include "../../util/alg_ds/alg/find.h"
#include "../../util/e.h"
#include "../../util/time_now_us.h"
#include "../../util/compare.h"

#include "../../util/alg_ds/alg/defer.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

#include "e2_agent_amr_api.h"
#include "send_msg_amr.h"
#include "../../agent/e2_agent_api.h"

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

static
kpm_pend_msg_t* extract_kpm_pend_msg(e2_agent_amr_t* ag, int msg_id)
{
  // Remove pending timeout
  rm_pend_ev_prox(&ag->pend, &ag->asio, msg_id);

  // Get kpm answer. This is OK, as the pointer points to the stack
  // The other msgs still have a valid pointer to the kpm_pend_msg_t
  kpm_pend_msg_t* dst = extract_kpm_pend_ds(&ag->kpm_pend_ds, msg_id);

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
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, CONFIG_MSG_STATS_PENDING_EVENT);

  // Send message 
  send_msg_stats(&ag->ep,msg_id);
}

void send_msg_ue_get_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm)
{
  // Add kpm answer
  add_kpm_pend_ds(&ag->kpm_pend_ds, msg_id, kpm);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, UE_GET_PENDING_EVENT);

  // Send message 
  send_msg_ue_get(&ag->ep,msg_id);
}

void send_config_get_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm)
{
  // Add kpm answer place holder
  add_kpm_pend_ds(&ag->kpm_pend_ds, msg_id, kpm);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, CONFIG_GET_PROXY_PENDING_EVENT);

  // Send message 
  send_config_get(&ag->ep,msg_id);
}


/////
// RC SM
/////
void send_msg_stats_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc)
{
  // Add rc answer
  add_rc_pend_ds(&ag->rc_pend_ds, msg_id, rc);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, CONFIG_MSG_STATS_PENDING_EVENT);

  // Send message 
  send_msg_stats(&ag->ep,msg_id);
}

void send_msg_ue_get_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc)
{
  // Add rc answer
  add_rc_pend_ds(&ag->rc_pend_ds, msg_id, rc);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, UE_GET_PENDING_EVENT);

  // Send message 
  send_msg_ue_get(&ag->ep,msg_id);
}

void send_config_get_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc)
{
  // Add rc answer place holder
  add_rc_pend_ds(&ag->rc_pend_ds, msg_id, rc);

  // Add pending timeout
  add_pend_ev_prox(&ag->pend, &ag->asio, msg_id, CONFIG_GET_PROXY_PENDING_EVENT);

  // Send message 
  send_config_get(&ag->ep, msg_id);
}

void msg_handle_ready(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  assert(ag->msg_id == 0 && "Ready message only interchanged at the beginning of connection");
  int const msg_id = ag->msg_id++; 

  // Send configuration as data needed to init the E2 Agent 

  // Add pending event
  add_pend_ev_prox(&ag->pend, &ag->asio ,msg_id, CONFIG_GET_PROXY_PENDING_EVENT);
  // Send msg
  send_config_get(&ag->ep, msg_id); 
}

static
void init_e2_agent_emulator(e2_agent_amr_t* ag, config_get_amr_t const* cfg)
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

  int64_t t0 = time_now_us();

  config_get_amr_t const* cfg = &msg->config;

  int const first_msg_id = 0; 
  if(cfg->msg_id == first_msg_id){
    defer({ free_msg_amr((void*)msg); });
    // Remove pending event
    rm_pend_ev_prox(&ag->pend, &ag->asio, cfg->msg_id);
    init_e2_agent_emulator(ag, cfg);
    return; 
  } 

  kpm_pend_msg_t* k = extract_kpm_pend_msg(ag, cfg->msg_id);

  // Move memory ownership
  *k->cfg = *cfg;
  notify_part_filled_kp(k);

  int64_t t1 = time_now_us();
  printf("Elapsed time  msg_handle_config_get %ld \n", t1 -t0);
}

void msg_handle_stats(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  int64_t t0 = time_now_us();
  assert(ag != NULL);
  assert(msg != NULL);

  msg_stats_amr_t const* s = &msg->stats; 
  // if msg is from KPM
  kpm_pend_msg_t* k = extract_kpm_pend_msg(ag, s->msg_id);

  // Move memory ownership
  *k->stats = *s;
  notify_part_filled_kp(k);

  int64_t t1 = time_now_us();
  printf("Elapsed time  msg_handle_stats %ld \n", t1 -t0);
}

void msg_handle_ue_get(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  int64_t t0 = time_now_us();
  assert(ag != NULL);
  assert(msg != NULL);

  msg_ue_get_t const* ue = &msg->ue; 
  
  kpm_pend_msg_t* k = extract_kpm_pend_msg(ag, ue->msg_id);
  // Move memory ownership
  *k->ues = *ue;
  notify_part_filled_kp(k);
 
  int64_t t1 = time_now_us();
  printf("Elapsed time msg_handle_ue_get %ld \n", t1 -t0);
}

void msg_handle_amr_ag(e2_agent_amr_t* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  ag->msg_hndl[msg->type](ag, msg);
}

