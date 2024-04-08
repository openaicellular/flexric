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

#include "ccc_sm_agent.h"

#include "../../util/alg_ds/alg/defer.h"
#include "ccc_sm_id.h"
#include "enc/ccc_enc_generic.h"
#include "dec/ccc_dec_generic.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef JSON
  ccc_enc_json_t enc;
#elif PLAIN
  ccc_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif

} sm_ccc_agent_t;

// Function pointers provided by the RAN for the
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
sm_ag_if_ans_subs_t on_subscription_ccc_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  assert(0!=0 && "Not implemented");

}

static
exp_ind_data_t on_indication_ccc_sm_ag(sm_agent_t const* sm_agent, void* ind_data)
{
//  printf("on_indication CCC called \n");
  assert(sm_agent != NULL);
  assert(ind_data != NULL && "Indication data needed for this SM");
  
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;

  exp_ind_data_t ret = {.has_value = true};

  // Liberate the memory if previously allocated by the RAN. It sucks
  ccc_ind_data_t* ind = (ccc_ind_data_t*)ind_data;
  defer({ free_ccc_ind_data(ind);  free(ind); });

  // Fill Indication Header
  byte_array_t ba_hdr = ccc_enc_ind_hdr(&sm->enc, &ind->hdr);
  assert(ba_hdr.len < 1024 && "Are you really encoding so much info?" );
  ret.data.ind_hdr = ba_hdr.buf;
  ret.data.len_hdr = ba_hdr.len;

  // Fill Indication Message
  byte_array_t ba_msg = ccc_enc_ind_msg(&sm->enc, &ind->msg);
  assert(ba_msg.len < 10*1024 && "Are you really encoding so much info?" );
  ret.data.ind_msg = ba_msg.buf;
  ret.data.len_msg = ba_msg.len;

  return ret;
}

static
sm_ctrl_out_data_t on_control_ccc_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  assert(0!=0 && "Not implemented");

}

static
sm_e2_setup_data_t on_e2_setup_ccc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  assert(0!=0 && "Not implemented");

}

static
sm_ric_service_update_data_t on_ric_service_update_ccc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0};
  return dst;
}

static
void free_ccc_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;
  free(sm);
}


// General SM information

// Definition
static
char const* def_ccc_sm_ag(void)
{
  return SM_CCC_SHORT_NAME;
}

// ID
static
uint16_t id_ccc_sm_ag(void)
{
  return SM_CCC_ID;
}

  // Revision
static
uint16_t rev_ccc_sm_ag (void)
{
  return SM_CCC_REV;
}

// OID
static
char const* oid_ccc_sm_ag (void)
{
  return SM_CCC_OID;
}



sm_agent_t* make_ccc_sm_agent(sm_io_ag_ran_t io)
{
  sm_ccc_agent_t* sm = calloc(1, sizeof(sm_ccc_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  //*(uint16_t*)(&sm->base.ran_func_id) = SM_ccc_ID; 

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[CCC_STATS_V3_0];
  sm->base.io.read_setup = io.read_setup_tbl[CCC_V3_0_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[CCC_CTRL_REQ_V3_0];
  sm->base.io.write_subs = io.write_subs_tbl[CCC_SUBS_V3_0];

  sm->base.free_sm = free_ccc_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_ccc_sm_ag;

  sm->base.proc.on_subscription = on_subscription_ccc_sm_ag;
  sm->base.proc.on_indication = on_indication_ccc_sm_ag;
  sm->base.proc.on_control = on_control_ccc_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_ccc_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_ccc_sm_ag;
  sm->base.handle = NULL;

  // General SM information
  sm->base.info.def = def_ccc_sm_ag;
  sm->base.info.id =  id_ccc_sm_ag;
  sm->base.info.rev = rev_ccc_sm_ag;
  sm->base.info.oid = oid_ccc_sm_ag;


//  assert(strlen(SM_RAN_CTRL_SHORT_NAME) < sizeof( sm->base.ran_func_name) );
//  memcpy(sm->base.ran_func_name, SM_RAN_CTRL_SHORT_NAME, strlen(SM_RAN_CTRL_SHORT_NAME)); 

  return &sm->base;
}
/*
uint16_t id_ccc_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}
*/

