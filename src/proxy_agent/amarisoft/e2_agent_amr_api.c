#include "e2_agent_amr_api.h"
#include "../../agent/e2_agent_api.h"
#include "e2_agent_amr.h"
#include <assert.h>
#include <pthread.h>
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

static
e2_agent_amr_t* ag = NULL;

static
pthread_t thrd;

static
void* start_e2_agent_amr_api(void* arg)
{
  assert(ag != NULL);
  assert(arg == NULL);
  start_e2_agent_amr(ag);
  return NULL;
}

void init_agent_amr_api(args_proxy_ag_t const* args)
{
  // Create Amarisoft interface
  ag = calloc(1, sizeof(e2_agent_amr_t)); 
  assert(ag != NULL && "Memory exhausted");
  *ag = init_e2_agent_amr(args);

  int const rc = pthread_create(&thrd, NULL, start_e2_agent_amr_api, NULL);
  assert(rc == 0);
}

void stop_agent_amr_api(void)
{
  assert(ag != NULL);

  stop_agent_api();

  free_e2_agent_amr(ag);
  int const rc = pthread_join(thrd, NULL);
  assert(rc == 0);
}

e2sm_plmn_t plmn_agent_amr_api()
{
  assert(ag != NULL);

  return ag->plmn;
}

void fill_msg_kpm_sm_api(kpm_msgs_amr_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  fill_msg_kpm_sm(ag, msg);
}

void fill_msg_rc_sm_api(rc_msgs_amr_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  fill_msg_rc_sm(ag, msg);
}

void ho_rc_sm_api(uint64_t n_id_nrcell, uint64_t ran_ue_id, size_t ssb_nr_arfcn ,rc_msgs_amr_t* msg)
{
  assert(ag != NULL);
  ho_rc_sm(ag, n_id_nrcell, ran_ue_id, ssb_nr_arfcn, msg);
}

void config_set_ccc_sm_api(uint64_t cell_id, uint64_t pusch_fixed_rb_start, uint64_t pusch_fixed_l_crb, ccc_msgs_amr_t* msg)
{
  assert(ag != NULL);
  config_set_ccc_sm(ag, cell_id, pusch_fixed_rb_start, pusch_fixed_l_crb, msg);
}

