#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>

#define PROXY_AGENT

#include "util/compare.h"

#include "e2_sm_agent.h"
#include "ran_if.h"
#include "ws_io_ran.h"
#include "io_ran.h"
#include "notif_e2_ran.h"
#include "ran_msg_hdlr.h"
#include "proxy_agent.h"

static bool           global_interrupted = false;
static proxy_agent_t* proxy_agent = NULL;
static pthread_t      thrd_agent;

/* --------------------------------------------------------------------------- */

void proxy_set_exit(void) { global_interrupted = true; }
bool proxy_get_exit_flag(void) { return global_interrupted; }
pthread_t get_e2_agent_thread_id(void){return thrd_agent;}
proxy_agent_t * get_proxy_agent(void) {return proxy_agent;}


/* -------------------------------------------------------------------------- */


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
// end of WARN


static inline void* static_start_agent(void* a)
{
  e2_start_agent(a);
  return NULL;
}

static void init_e2if(proxy_agent_t *p) 
{
  sm_io_ag_ran_t io = init_io_proxy_ag();
  const int e2ap_server_port = 36421;
  char* server_ip_str = get_near_ric_ip(&p->conf.e2args);
  // Caveats on e2_init_agent(): 
  // e2_init_agent() has been modified to not have automatically generate e2setup at init time.
  // The initialization of e2_if->ran_if field in a direct way as below is a trick to avoid modifing the 
  // e2_init_agent() signature. 
  global_e2_node_id_t ge2ni_unused; 
  p->e2_if = e2_init_agent(server_ip_str, e2ap_server_port, ge2ni_unused, io, p->conf.e2args.libs_dir);
  p->e2_if->ran_if = &p->ran_if; 

  free(server_ip_str);

  const int rc = pthread_create(&thrd_agent, NULL, static_start_agent, p->e2_if);
  assert(rc == 0);
}

static proxy_agent_t * init_proxy_agent(int argc, char *argv[])
{
  proxy_agent = calloc(1, sizeof(proxy_agent_t));
  assert(proxy_agent!= NULL);
 
  ws_initconf(&proxy_agent->conf, argc, argv);
  init_e2if(proxy_agent);
  notif_init_ran(&proxy_agent->ran_if); 
  init_indication_event(&proxy_agent->ran_if.ind_event);
  
  proxy_agent->ran_if.io = io_ran_get_instance(proxy_agent->conf.io_ran_conf, &ran_msg_handle);
  if (!proxy_agent->ran_if.io) {
    lwsl_err("websocket RAN: init I/O module failed\n");
    free (proxy_agent);
    return NULL;
  }
  proxy_agent->ran_if.ser = ran_ser_get_instance();
  lwsl_user("websocket: starting scheduling a connection\n");
  ran_errcodes_t retcode = proxy_agent->ran_if.io->open(10);
  if (retcode != ERR_RAN_IO_NONE){
    free (proxy_agent);
    return NULL;
  }
  
  return proxy_agent;
}

static void sigint_handler(int sig)
{
  (void)sig; // UNUSED: we just handle interrupt signal
  lwsl_user("CTRL-C received !\n");
  proxy_set_exit();
}

static void stop_e2_agent_api(void)
{
  if (proxy_agent->e2_if != NULL) {
    // As E2 is not multithreading safe, we circumvent with below trick waiting for the I/O E2 cycle to stop.
    proxy_agent->e2_if->stop_token = true;
    usleep(1000);
    e2_free_agent(proxy_agent->e2_if);
    int const rc = pthread_join(get_e2_agent_thread_id(),NULL);
    assert(rc == 0);
  }
}

static void close_proxy_agent (proxy_agent_t *proxy_agent) {

  lwsl_user("Closing all\n");
  stop_e2_agent_api();
  
  proxy_agent->ran_if.io->destroy();
  bi_map_free(&proxy_agent->ran_if.ind_event);
  // notif_free_ran(&proxy_agent->ran_if);// for some reason this function hangs the program. sImpact of not calling this function: memory leak  
 
  free (proxy_agent);
  proxy_agent = NULL;
}

/* --------------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
  signal(SIGINT, sigint_handler);
  if (init_proxy_agent(argc, argv) == NULL)
  { 
    printf ("FAILURE initializing proxy_agent\n");
    return EXIT_FAILURE; 
  }

  lwsl_user("Initialization of RAN and E2 interface successful. Waiting for events.\n");
  
  // Poll RAN interface until interrupted. Note that polling E2 interface is done already by init_e2_agent_api()
  enum ran_errcodes_t n = ERR_RAN_IO_NONE;
  while (n == ERR_RAN_IO_NONE  && proxy_get_exit_flag() == false)
    n = proxy_agent->ran_if.io->open(10); 
 
  lwsl_user("Removing all resources. Closing all\n");
  close_proxy_agent(proxy_agent);
  return EXIT_SUCCESS;
}