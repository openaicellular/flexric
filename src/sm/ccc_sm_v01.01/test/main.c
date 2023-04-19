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
  
  // e2sm_ccc_event_trigger_t out = ccc_dec_event_trigger_json(ba.len, ba.buf);
  // defer({ free_ccc_event_trigger_def(&out); });
  // printf("CCC Event Trigger decoding test succeded\n");

}



int main()
{
  for (size_t i = 0; i<10; i++)
  {
    // Test CCC RIC Event Trigger Definition
    test_ccc_ric_event_trigger_def_json();
    
  }
  

  return EXIT_SUCCESS;
}
