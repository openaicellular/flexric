#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../../../src/util/alg_ds/alg/defer.h"

#include "../enc/ccc_enc_json.h"

#include "fill_rnd_data_ccc.h"

void test_ccc_ric_event_trigger_def_json(void)
{
  e2sm_ccc_event_trigger_t msg = fill_ccc_event_trigger_def();
  // defer

  byte_array_t ba = ccc_enc_event_trigger_json(&msg);
  // defer


}



int main()
{
  // Test CCC RIC Event Trigger Definition
  test_ccc_ric_event_trigger_def_json();
  printf("CCC Event Trigger encoding test succeded\n");

  return EXIT_SUCCESS;
}
