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
  e2_agent_amr_t tmp = init_e2_agent_amr(args);
  memcpy(&ag, &tmp, sizeof(e2_agent_amr_t));

  int const rc = pthread_create(&thrd, NULL, start_e2_agent_amr_api, NULL);
  assert(rc == 0);
}

void stop_agent_amr_api(void)
{
  assert(&ag != NULL);
  free_e2_agent_amr(&ag);
  int const rc = pthread_join(thrd, NULL);
  assert(rc == 0);
}

e2sm_plmn_t plmn_agent_amr_api()
{
  assert(&ag != NULL);
 
  return ag.plmn;
}

void fill_msg_kpm_sm_api(kpm_msgs_amr_t* msg)
{
  assert(&ag != NULL);
  assert(msg != NULL);

  fill_msg_kpm_sm(&ag, msg);
}

void fill_msg_rc_sm_api(rc_msgs_amr_t* msg)
{
  assert(&ag != NULL);
  assert(msg != NULL);

  fill_msg_rc_sm(&ag, msg);
}

