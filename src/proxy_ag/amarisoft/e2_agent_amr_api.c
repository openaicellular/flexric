#include "e2_agent_amr_api.h"
#include "e2_agent_amr.h"
#include <assert.h>
#include <pthread.h>

static
e2_agent_amr_t ag;

static
pthread_t thrd;

static
void* start_e2_agent_amr_api(void* arg)
{
  assert(arg == NULL);
  start_e2_agent_amr(&ag);
  return NULL;
}

void init_agent_amr_api(args_proxy_ag_t const* args)
{
  // Create Amarisoft interface
  ag = init_e2_agent_amr(args);

  int const rc = pthread_create(&thrd, NULL, start_e2_agent_amr_api, NULL);
  assert(rc == 0);

  // Note that the E2 Agent will not be created untill 
  // configuration data from Amarisoft is read
  // i.e., 
  // void init_agent_api(int mcc, 
  //                  int mnc, 
  //                  int mnc_digit_len,
  //                  int nb_id,
  //                  int cu_du_id,
  //                  e2ap_ngran_node_t ran_type,
  //                  sm_io_ag_ran_t io,
  //                  fr_args_t const* args);
}

void stop_agent_amr_api(void)
{
  assert(&ag != NULL);
  free_e2_agent_amr(&ag);
  int const rc = pthread_join(thrd, NULL);
  assert(rc == 0);
}

void fill_msg_kpm_sm_api(msg_stats_amr_t *stats, msg_ue_get_t *ues)
{
  assert(&ag != NULL);
  assert(stats != NULL);
  assert(ues != NULL);

  fill_msg_kpm_sm(&ag, stats, ues);
}


