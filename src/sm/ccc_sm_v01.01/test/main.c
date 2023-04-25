#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../../../src/util/alg_ds/alg/defer.h"

#include "../enc/ccc_enc_json.h"
#include "../dec/ccc_dec_json.h"

#include "fill_rnd_data_ccc.h"

void test_ccc_ric_event_trigger_def_json(void)
{
  e2sm_ccc_event_trigger_t msg = fill_ccc_event_trigger_def();
  defer({ free_ccc_event_trigger_def(&msg); }); 

  byte_array_t ba = ccc_enc_event_trigger_json(&msg);
  defer({ free_byte_array(ba); });
  printf("CCC Event Trigger encoding test succeded\n");
  
  e2sm_ccc_event_trigger_t out = ccc_dec_event_trigger_json(ba.len, ba.buf);
  defer({ free_ccc_event_trigger_def(&out); });
  printf("CCC Event Trigger decoding test succeded\n");

  assert(eq_ccc_event_trigger_def(&msg, &out) == true);
  printf("CCC Event Trigger equivalency test succeded\n");

}

void test_ccc_ric_action_def_json(void)
{
  e2sm_ccc_action_def_t msg = fill_ccc_action_def();
  defer({ free_ccc_action_def(&msg); });

  byte_array_t ba = ccc_enc_action_def_json(&msg);
  defer({ free_byte_array(ba); });
  printf("CCC Action Definition encoding test succeded\n");

  e2sm_ccc_action_def_t out = ccc_dec_action_def_json(ba.len, ba.buf);
  defer({ free_ccc_action_def(&out); });
  printf("CCC Action Definition decoding test succeded\n");

  assert(eq_ccc_action_def(&msg, &out) == true);
  printf("CCC Action Definition equivalency test succeded\n");

}


int main()
{
  for (size_t i = 0; i<10; i++)
  {
    // Test CCC RIC Event Trigger Definition
    test_ccc_ric_event_trigger_def_json();

    // Test CCC RIC Action Definition
    test_ccc_ric_action_def_json();
    
  }
  

  return EXIT_SUCCESS;
}
