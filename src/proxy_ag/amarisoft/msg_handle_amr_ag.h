#ifndef MSG_HANDLE_AMR_AG_MIR_H
#define MSG_HANDLE_AMR_AG_MIR_H 

#include "e2_agent_amr.h"
#include "msg_amr.h"


void msg_handle_amr_ag(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_ready(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_config_get(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_stats(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_ue_get(e2_agent_amr_t* ag, msg_amr_t const* msg);




#endif
