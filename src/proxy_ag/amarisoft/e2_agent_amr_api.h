#ifndef E2_AGENT_AMR_API_MIR_H
#define E2_AGENT_AMR_API_MIR_H 

#include "../../lib/3gpp/ie/plmn_identity.h"
#include "../../util/conf_file.h"

#include "kpm_msgs_amr.h"
#include "rc_msgs_amr.h"

void init_agent_amr_api(args_proxy_ag_t const* args);

void stop_agent_amr_api(void);

e2sm_plmn_t plmn_agent_amr_api(void); 

void fill_msg_kpm_sm_api(kpm_msgs_amr_t* msg);

void fill_msg_rc_sm_api(rc_msgs_amr_t* msg);

#endif
