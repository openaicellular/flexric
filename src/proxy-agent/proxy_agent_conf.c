#include <stdbool.h>
#include "ran_if.h"
#include "proxy_agent_conf.h"

static const int                  default_host_port = 9001;
static const enum lws_log_levels  default_log_level = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

bool ws_initconf(struct proxy_conf_t * conf, int argc, char *argv[])
{
  conf->e2args = init_fr_args(argc, argv);
  defer({ free_fr_args(&conf->e2args); });
  conf->retry_count = 0;

  conf->io_ran_conf.port = default_host_port;
  conf->io_ran_conf.logl = default_log_level;

  if (conf->e2args.proxy_ran_args.ip){
    strncpy(conf->io_ran_conf.address, conf->e2args.proxy_ran_args.ip, strlen(conf->e2args.proxy_ran_args.ip));
    printf("[E2-Proxy-Agent]: get amr-ran ip %s from conf file %s\n", conf->e2args.proxy_ran_args.ip, conf->e2args.conf_file);
  }

  if (conf->e2args.proxy_ran_args.port != -1){
    printf("[E2-Proxy-Agent]: got RAN port %d from conf file %s\n", conf->e2args.proxy_ran_args.port, conf->e2args.conf_file);
    conf->io_ran_conf.port = conf->e2args.proxy_ran_args.port;
  }
  
  if (conf->e2args.proxy_ran_args.logl != -1) {
    printf("[E2-Proxy-Agent]: got RAN log levels %d from conf file %s\n", conf->e2args.proxy_ran_args.logl, conf->e2args.conf_file);
    conf->io_ran_conf.logl = conf->e2args.proxy_ran_args.logl;
  }
  
  char *envar = getenv("WS_RAN_HOST");
  if (envar)
  {
    printf("Environment variable WS_RAN_HOST caught\n");
    char *p = strtok(envar, ":");
    if (p != NULL) {
      strncpy(conf->io_ran_conf.address, p, sizeof(conf->io_ran_conf.address));
      p = strtok(NULL, " ");
      if (p)
        conf->io_ran_conf.port = atoi(p);
    }else 
      strncpy(conf->io_ran_conf.address, envar, sizeof(conf->io_ran_conf.address));
  } 

  envar = getenv("WS_LOGL");
  if (envar)
  { 
    printf("Environment variable WS_LOGL caught\n");
    conf->io_ran_conf.logl = atoi(envar);
  }
  
  return true;
}

void ws_initconf_print_help(void){

  printf(
  "Configuration help for WS interface.\n"\
  "\n"\
  "Configuration is affected by environment variables WS_RAN_HOST and WS_LOGL, otherwise defaults apply\n"\
  "WS_RAN_HOST=ranhostname[:port]. If port is not specified, it will use the default\n"\
  "WS_LOGL=bitmask. Bitmask is composed of OR values: LLL_ERR(1), LLL_WARN(1 << 1), LLL_NOTICE(1 << 2), LLL_INFO(1 << 3), LLL_DEBUG(1 << 4), LLL_PARSER(1 << 5), LLL_HEADER(1 << 6), LLL_EXT(1 << 7), LLL_CLIENT(1 << 8), LLL_LATENCY(1 << 9), LLL_USER(1 << 10), LLL_THREAD(1 << 11)\n"\
  "Defaults: ranhostname=localhost, port=%d, loglevel=%d (LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE)\n", 
  default_host_port, default_log_level);
}

void ws_conf_print(struct proxy_conf_t * c) 
{ 
  printf(
  "configuration used:\n"\
  "- retry_count:%d\n"\
  "- RAN ip address:%s\n"\
  "- RAN port:%d\n"\
  "- LogLevel:%d\n"\
  "- E2 conf file (%s) and SM lib dir (%s)\n"\
  , c->retry_count, c->io_ran_conf.address, c->io_ran_conf.port, c->io_ran_conf.logl, c->e2args.conf_file,c->e2args.libs_dir);     
}