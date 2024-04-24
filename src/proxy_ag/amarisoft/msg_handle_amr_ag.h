#ifndef MSG_HANDLE_AMR_AG_MIR_H
#define MSG_HANDLE_AMR_AG_MIR_H 

#include "e2_agent_amr.h"
#include "io_ran/msg_amr.h"

void msg_handle_amr_ag(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_ready(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_config_get(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_stats(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_ue_get(e2_agent_amr_t* ag, msg_amr_t const* msg);

void msg_handle_ho(e2_agent_amr_t* ag, msg_amr_t const* msg);

// Send KPM
void send_msg_stats_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm);

void send_msg_ue_get_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm);

void send_config_get_kpm(e2_agent_amr_t* ag, int msg_id, kpm_pend_msg_t* kpm);

// Send RC
void send_msg_ue_get_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc);

void send_config_get_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc);

void send_ho_rc(e2_agent_amr_t* ag, int msg_id, rc_pend_msg_t* rc, uint64_t pci, uint64_t ue_id, size_t ssb_nr_arfcn);

#endif
