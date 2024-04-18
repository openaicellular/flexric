#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <poll.h>
#include "../src/proxy_ag/amarisoft/e2_agent_amr_api.h"

static
void stop_and_exit()
{
  // Stop the E2 Agent
  stop_agent_amr_api();
  exit(EXIT_SUCCESS);
}

static
pthread_once_t once = PTHREAD_ONCE_INIT;

static
void sig_handler(int sig_num)
{
  printf("\n[E2 AGENT]: Abruptly ending with signal number = %d\n[E2 AGENT]: Please, wait.\n", sig_num);
  // For the impatient, do not break my code
  pthread_once(&once, stop_and_exit);
}


int main(int argc, char *argv[])
{
  // Signal handler
  signal(SIGINT, sig_handler);


  // Remove this after testing
  ///////////////////
  uint32_t port = 9001;

  if(argc > 1)
    port = atoi(argv[1]);
  ///////////////////


  args_proxy_ag_t args = {.ip = "127.0.0.1", .port = port}; 
  args.fr_args = init_fr_args(argc, argv);

  init_agent_amr_api(&args);

  while(1){
    poll(NULL, 0, 1000);
  }

  return EXIT_SUCCESS;
}

