/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */


#include "../../../../src/util/byte_array.h"
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/sm/ccc_sm/enc/ccc_enc_json.h"
#include "../../../../src/sm/ccc_sm/dec/ccc_dec_json.h"
#include "../../../rnd/fill_rnd_data_ccc.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


void test_ric_event_trigger_ccc(void)
{
  assert(0 != 0 && "Not implemented");
}

 
void test_ccc_act_def(void)
{
  e2sm_ccc_action_def_t act_def = fill_rnd_ccc_action_def();
  defer({free_e2sm_ccc_action_def(&act_def); });

  byte_array_t ba = ccc_enc_action_def_json(&act_def);
  defer({free_byte_array(ba);});

  e2sm_ccc_action_def_t out = ccc_dec_action_def_json(ba.len, ba.buf);
  defer({free_e2sm_ccc_action_def(&out);});

  assert(eq_e2sm_ccc_action_def(&act_def, &out) == true);
}

void test_ccc_ind_hdr(void)
{
  e2sm_ccc_ind_hdr_t hdr = fill_rnd_ccc_ind_hdr();
  defer({ free_e2sm_ccc_ind_hdr(&hdr); });

  byte_array_t ba = ccc_enc_ind_hdr_json(&hdr);
  defer({ free_byte_array(ba); });

  e2sm_ccc_ind_hdr_t out = ccc_dec_ind_hdr_json(ba.len, ba.buf);
  defer({ free_e2sm_ccc_ind_hdr(&out); });

  assert(eq_e2sm_ccc_ind_hdr(&hdr, &out) == true);
}


void test_ccc_ind_msg(void)
{
  assert(0 != 0 && "Not implemented");
}

void test_ccc_call_process_id()
{
  assert(0 != 0 && "Not implemented");
}

void test_ccc_ctrl_hdr(void)
{
  assert(0 != 0 && "Not implemented");
}


void test_ccc_ctrl_msg(void)
{
  assert(0 != 0 && "Not implemented");
}

void test_ccc_ctrl_out(void)
{
  assert(0 != 0 && "Not implemented");
}

void test_ccc_ran_func_def(void)
{
  assert(0 != 0 && "Not implemented");
}





int main()
{
  time_t t;
  srand((unsigned) time(&t));

  // Test off all the functions
  printf("\nCCC SM enc/dec test launched\n");

  // Event Trigger
//  test_ric_event_trigger_ccc();
//  printf("\nCCC Event Trigger test succeeded\n");

  // Action Definition
  test_ccc_act_def();
  printf("\nCCC Action Definition test succeeded\n");

  // Indication Header
  test_ccc_ind_hdr();
  printf("\nCCC Indication Header test succeeded\n");

  // Indication Message
//  test_ccc_ind_msg();
//  printf("\nCCC Indication Message test succeeded\n");

  // Call Process ID 
//  test_ccc_call_process_id();
//  printf("\nCCC Call Process ID\n");

  // Control Header 
//   test_ccc_ctrl_hdr();
//   printf("\nCCC Control Header \n");

  // Control Message  
//   test_ccc_ctrl_msg();
//   printf("\nCCC Control Message\n");

  // Control Outcome 
//  test_ccc_ctrl_out();
//  printf("\nCCC Control Outcome\n");

  // RAN Function Definition
//  test_ccc_ran_func_def();
//  printf("\nCCC RAN Function Definition\n");

  return EXIT_SUCCESS;
}

