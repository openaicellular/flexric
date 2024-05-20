#ifndef SEND_MESSAGE_AMARISOFT_H
#define SEND_MESSAGE_AMARISOFT_H 

#include "ep_amr.h"

void send_config_get(ep_amr_t const* ep, int msg_id);

void send_msg_stats(ep_amr_t const* ep, int msg_id);

void send_msg_ue_get(ep_amr_t const* ep, int msg_id);

void send_msg_stats_ue_get(ep_amr_t const* ep, int msg_id);

void send_ho(ep_amr_t const* ep, int msg_id, uint64_t pci, uint64_t ran_ue_id, size_t ssb_nr_arfcn);

void send_config_set_rb_control_dl(ep_amr_t const* ep, int msg_id, uint64_t cell_id, uint64_t pdsch_fixed_rb_start, uint64_t pdsch_fixed_l_crb);

void send_config_set_rb_control_ul(ep_amr_t const* ep, int msg_id, uint64_t cell_id, uint64_t pusch_fixed_rb_start, uint64_t pusch_fixed_l_crb);

#endif

