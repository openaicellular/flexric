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
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>

#define PROXY_AGENT
#include "../src/util/compare.h"
#include "../agent/e2_agent_api.h"
#include "ws_ran_if.h"
#include "ran_if.h"
#include "e2_sm_agent.h"
#include "proxy_agent.h"
#include "proxy_agent_conf.h"
#include "notif_e2_ws.h"


static int global_interrupted;
static proxy_agent_t *proxy_agent = NULL;

static void stop_e2_agent_api(void)
{
  assert(proxy_agent->e2_if != NULL);
  e2_free_agent(proxy_agent->e2_if );
  int const rc = pthread_join(get_e2_agent_thread_id(),NULL);
  assert(rc == 0);
}


static void sigint_handler(int sig)
{
  (void)sig; // UNUSED: we just handle interrupt signal
  lwsl_user("CTRL-C received !\n");
  proxy_set_exit();
}

void proxy_set_exit(void) { global_interrupted = 1; }
int proxy_get_exit_flag(void) { return global_interrupted; }
struct lws_context *ws_get_global_ctx(void) { return proxy_agent->ran_if.ctx; }
void ws_set_global_lws(struct lws *lws) {  proxy_agent->ran_if.lws = lws; }
struct lws *ws_get_global_lws(void) {   return proxy_agent->ran_if.lws;}
struct proxy_conf_t  *get_proxy_conf(void) { return &proxy_agent->conf;}
proxy_agent_t * get_proxy_agent(void) {return proxy_agent;}

// WARN: code duplication from e2_agent.c code: free_ind_event(), free_key(), init_indication_event()
static inline
void free_ind_event(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  ind_event_t* ev = (ind_event_t* )value;
  free(ev);
}

static inline
void free_key(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  int* fd = (int* )value;
  free(fd);
}


static inline
void init_indication_event(bi_map_t *ind_event)
{
  size_t key_sz_fd = sizeof(int);
  size_t key_sz_ind = sizeof(ind_event_t);

  bi_map_init(ind_event, key_sz_fd, key_sz_ind, cmp_fd, cmp_ind_event, free_ind_event, free_key);
}



/* 
 * init RAN interface and proxy configuration
 * We will init the interface E2 (proxy_agent->e2_if) when we receive the E2Setup reply from WS interface
 */
static proxy_agent_t * init_proxy_agent(int argc, char *argv[])
{
  proxy_agent_t *proxy_agent = calloc(1, sizeof(proxy_agent_t));
  assert(proxy_agent != NULL);
  
  proxy_agent->ran_if.init = ws_init;
  proxy_agent->ran_if.conn = ws_conn;
  proxy_agent->ran_if.poll = ws_poll;
  proxy_agent->ran_if.free = ws_free;
  init_indication_event(&proxy_agent->ran_if.ind_event);
  proxy_agent->ran_if.user = &static_user;
  proxy_agent->ran_if.ind_timer_ready = false;

  proxy_agent->conf.e2args = init_fr_args(argc, argv);
  ws_initconf(&proxy_agent->conf, argc, argv);

  proxy_agent->ran_if.ctx = proxy_agent->ran_if.init(proxy_agent->conf);
  if (!proxy_agent->ran_if.ctx)
  {
    lwsl_err("websocket: init failed\n");
    free (proxy_agent);
    return NULL;
  }  
  
  lwsl_user("websocket: starting scheduling a connection\n");
  proxy_agent->ran_if.conn(proxy_agent->ran_if.ctx);
   
  if (proxy_get_exit_flag()) {
    free (proxy_agent);
    return NULL;
  }
  return proxy_agent;
}

static void close_proxy_agent (proxy_agent_t *proxy_agent) {

  lwsl_user("Closing all\n");
  proxy_agent->ran_if.free(ws_get_global_ctx());
  stop_e2_agent_api();
  free (proxy_agent);
}


/****************************************** */
int main(int argc, char *argv[])
{
  signal(SIGINT, sigint_handler);
      
  proxy_agent = init_proxy_agent(argc, argv);
  if (proxy_agent == NULL)
    return EXIT_FAILURE; 

  lwsl_user("Initialization of WS and E2 interface successful. Waiting for events.\n");

  // polling WS channel until interrrupted. Note that E2 polling is done already by init_e2_agent_api()
  int n = 0;
  while (n >= 0 && !proxy_get_exit_flag())
    n = proxy_agent->ran_if.poll(ws_get_global_ctx()); 
 
  lwsl_user("Removing all resources. Closing all\n");
  close_proxy_agent(proxy_agent);
  
  return EXIT_SUCCESS;
}


