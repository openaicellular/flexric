#include "proxy_agent_conf.h"

#define AMARISOFT_HOST_IPv4 "172.21.13.16" // leonard host in t9s
#define AMARISOFT_HOST_PORT 9001
#define LOCALHOST_HOST_PORT 7681
/* 
 * extract configuration for RAN ip address/port and log levels for WS interface from:
 * - environment variable WS_RAN_HOST (partially)
 * - configuration file (not implemented)
 * - cmdline args (not implemented)
 * - defaults to Eurecom lab addresses ip/port, loglevels
 */
void ws_initconf(struct proxy_conf_t * conf, int argc, char *argv[])
{
  (void)argc; (void)argv; //unimplemented
  conf->retry_count = 0;
  char *envar = getenv("WS_RAN_HOST");
  if (envar && strcmp(envar, "localhost") == 0)
  {
    lwsl_user("Environment variable TARGET_HOST caught\n");
    conf->port = LOCALHOST_HOST_PORT;
    conf->address = "localhost";
  } else {
    conf->port = AMARISOFT_HOST_PORT;
    conf->address = AMARISOFT_HOST_IPv4;
  }
  conf->logl = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG; 
  //conf->logl = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO; 
}
