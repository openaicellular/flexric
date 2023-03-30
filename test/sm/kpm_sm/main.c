/*  
 * E2E test for KPM SM, no messages on the wire transported by E2AP.
 */

#include "../../../src/sm/kpm_sm_v03.00/kpm_sm_agent.h"
#include "../../../src/sm/kpm_sm_v03.00/kpm_sm_ric.h"
#include "../../../src/sm/kpm_sm_v03.00/kpm_sm_id.h"
#include "../../../src/util/alg_ds/alg/defer.h"
#include "../common/fill_ind_data.h"


#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// 'cp' is buffer in  reception to compare the received indication message against the sent one
static kpm_ric_indication_t cp_indication; 
// static kpm_ric_subscription_t cp_subscription;
// static kpm_e2_setup_t cp_e2_setup;


// AGENT part of the architecture. The communication with the RAN is achieved via READ/WRITE methods below
// write_RAN() for the moment does nothing.
static void read_RAN(sm_ag_if_rd_t* read)
{
  assert(read != NULL);
  assert(read->type == KPM_STATS_V0);


  fill_kpm_ind_data(&read->kpm_stats); 
  cp_indication = cp_kpm_ind_data(&read->kpm_stats);
}

static 
sm_ag_if_ans_t write_RAN(sm_ag_if_wr_t const* data)
{
  assert(data != NULL);
  
  sm_ag_if_ans_t ans = {0};
  return ans;
}


/////////////////////////////
// Check Functions
// //////////////////////////

/* IE exchanged: none. Direction: none
 * XXX: This should be extended to check IE Ran Function definition
 */
static
void check_eq_ran_function(sm_agent_t * ag, sm_ric_t const* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
  assert(ag->ran_func_id == ric->ran_func_id);
  assert(ag->ran_func_id == SM_KPM_ID);
  printf ("[IE RAN function ID]: Agent and RIC are using the same RAN function ID for KPM: %d\n", SM_KPM_ID);

  // Encoding of E2 Setup message
  sm_e2_setup_t e2_setup_data = ag->proc.on_e2_setup(ag);
  printf ("[IE E2 Setup] correctly encoded\n");
  
  // Decoding of E2 Setup message - void for now
  // printf ("[IE E2 Setup] correctly decoded\n");

  // Checking the E2 Setup correctness
  // assert(msg.type == KPM_E2_SETUP_ANS_V0);
  // assert(eq_kpm_e2_setup(&cp_e2_setup, data) == true && "Failure checking for correctness in E2 Setup data IE");

  free_sm_e2_setup(&e2_setup_data);
  // free_kpm_e2_setup(&cp_e2_setup);
  // free_kpm_e2_setup(data);
}

/* Direction: RIC -> E2 
 * IE exchanged: RIC Event Trigger Definition + RIC Action Definition 
 */
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
 
  // Encoding of RIC SUBSCRIPTION message
  sm_subs_data_t subs_data = ric->proc.on_subscription(ric, "2_ms");
  printf ("[IE RIC Event Trigger Definition] correctly encoded\n");
  // printf ("[IE RIC Action Definition] correctly encoded\n");
  

  // Decoding of RIC SUBSCRIPTION message
  // printf ("[IE RIC Event Trigger Definition] correctly decoded\n");
  // printf ("[IE RIC Action Definition] correctly decoded\n");


  // Checking the RIC SUBSCRIPTION correctness
  assert ((ag->proc.on_subscription(ag, &subs_data)).ms == 2 && "error in decoding trigger");
  // assert(eq_kpm_subscription_data(&cp_subscription, data) == true && "Failure checking for correctness in subscription data IE");

  // free_kpm_subscription_data(data);
  free_sm_subs_data(&subs_data);
  // free_kpm_subscription_data(&cp_subscription);

}

/* Direction: E2 -> RIC
 * IE exchanged: RIC Indication Header, RIC Indication Message
 */
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  // sending IE indication. Behind the scenes it will call the read_RAN()
  sm_ind_data_t sm_data = ag->proc.on_indication(ag);
  printf ("[IE RIC Indication Message]: correctly encoded\n");

  // receiving IE indication  (decoding)
  sm_ag_if_rd_t msg = ric->proc.on_indication(ric, &sm_data);
  printf ("[IE RIC Indication Message] correctly decoded\n");

  // check for indication message correctness
  kpm_ric_indication_t* data = &msg.kpm_stats;
  assert(msg.type == KPM_STATS_V0);
  assert(eq_kpm_ind_data(&cp_indication, data) == true && "Failure checking for correctness in indication data IE");
    
  free_kpm_ind_data(data);
  free_sm_ind_data(&sm_data);
  free_kpm_ind_data(&cp_indication);

}

int main()
{
  for (size_t i = 0; i<10; i++)
  {

  printf("KPM SM unit test launched\n");
  sm_io_ag_t io_ag = {.read = read_RAN, .write = write_RAN};  
  sm_agent_t* sm_ag = make_kpm_sm_agent(io_ag);
  sm_ric_t* sm_ric = make_kpm_sm_ric();

  printf("-> STEP 1. Controlling RAN function ................\n");
  check_eq_ran_function(sm_ag, sm_ric);
  printf("-> STEP 2. Controlling Subscription procedure.......\n");
  check_subscription(sm_ag, sm_ric);
  printf("-> STEP 3. Controlling Indication procedure.........\n");
  check_indication(sm_ag, sm_ric);

  printf("-> STEP 4. Freeing memory...........................\n");
  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);
  
  printf("-> Test completed with success. Closing all\n");

  }
  
  return EXIT_SUCCESS;
}
