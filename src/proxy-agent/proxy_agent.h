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
#ifndef PROXY_AGENT_H 
#define PROXY_AGENT_H

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>

#include "../agent/e2_agent_api.h"
#include "../agent/e2_agent.h"
#include "ran_if.h"
#include "e2_sm_agent.h"


typedef struct proxy_agent_t 
{
  // RAN interface (we support just WS implementation)
  ran_if_t ran_if;

  // E2 interface
  e2_agent_t* e2_if;

  // user configuration 
  struct proxy_conf_t conf;

  // Registered SMs
  // plugin_ag_t plugin;

  

} proxy_agent_t;

void proxy_set_exit(void);
int proxy_get_exit_flag(void);

struct lws_context *ws_get_global_ctx (void);
void ws_set_global_ctx (struct lws_context *ctx);
void ws_set_global_lws(struct lws *lws);
struct lws *ws_get_global_lws(void);
struct proxy_conf_t  *get_proxy_conf(void);
proxy_agent_t * get_proxy_agent(void);


#endif