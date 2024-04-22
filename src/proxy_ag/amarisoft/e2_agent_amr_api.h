#ifndef E2_AGENT_AMR_API_MIR_H
#define E2_AGENT_AMR_API_MIR_H 

#include "../../lib/3gpp/ie/plmn_identity.h"
#include "../../util/conf_file.h"
#include "../../lib/3gpp/ie/nr_cgi.h"
#include "../../lib/sm/ie/ue_id.h"

#include "kpm_msgs_amr.h"
#include "rc_msgs_amr.h"

void init_agent_amr_api(args_proxy_ag_t const* args);

void stop_agent_amr_api(void);

e2sm_plmn_t plmn_agent_amr_api(void); 

void fill_msg_kpm_sm_api(kpm_msgs_amr_t* msg);

void fill_msg_rc_sm_api(rc_msgs_amr_t* msg);

// Hand Over
void ho_rc_sm_api(uint64_t n_id_nrcell, uint64_t ran_ue_id, rc_msgs_amr_t* msg);

#endif
