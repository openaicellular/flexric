#ifndef AMARISOFT_PROXY_AGENT_MIR_H
#define AMARISOFT_PROXY_AGENT_MIR_H 

#include "../../util/conf_file.h"

typedef struct{
  args_proxy_ag_t args;

} proxy_ag_amr_t;

proxy_ag_amr_t init_proxy_ag_amr(args_proxy_ag_t args);

void stop_proxy_ag_amr(void);

#endif
