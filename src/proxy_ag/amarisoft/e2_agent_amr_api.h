#ifndef E2_AGENT_AMR_API_MIR_H
#define E2_AGENT_AMR_API_MIR_H 

#include "../../util/conf_file.h"
#include "ie/config_get.h"
#include "ie/msg_stats.h"
#include "ie/msg_ue_get.h"

void init_agent_amr_api(args_proxy_ag_t const* args);

void stop_agent_amr_api(void);

void fill_msg_kpm_sm_api(msg_stats_amr_t *stats, msg_ue_get_t *ues);

void fill_msg_rc_sm_api();

#endif
