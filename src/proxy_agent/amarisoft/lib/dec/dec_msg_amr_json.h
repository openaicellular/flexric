#ifndef DEC_MSG_AMR_JSON_H
#define DEC_MSG_AMR_JSON_H 

#include "../../io_ran/msg_amr.h"
#include "lib/ep/ws_msg.h"

#include "../ie/msg_ue_get.h" 
#include "../ie/msg_ho_ans_amr.h"
#include "../ie/msg_config_get.h"
#include "../ie/msg_stats.h"
#include "../ie/msg_config_set_ans_amr.h"

msg_amr_t msg_dec_amr_ag(ws_msg_t const* src);

// 5 Messages from the RAN

void dec_msg_ho_ans_amr(const char* in, msg_ho_ans_amr_t* out);

void dec_config_get_amr(const char* in, msg_config_get_amr_t* out);

void dec_msg_stats_amr(const char* in, msg_stats_amr_t* out);

void dec_msg_ue_get_amr(const char* in, msg_ue_get_t* out);

void dec_msg_config_set_ans_amr(const char* in, msg_config_set_ans_amr_t* out);

// Decode from the types

arr_cell_amr_t dec_arr_cell_amr(void* it);

arr_cells_ue_lst_amr_t dec_arr_cells_ue_lst_amr(void* it);

arr_ncell_list_amr_t dec_arr_ncell_list_amr(void* it);

arr_nr_cells_amr_t dec_arr_nr_cells_amr(void* it);

arr_qos_flows_ue_lst_t dec_arr_qos_flows_ue_lst(void* it);

arr_rf_ports_amr_t dec_arr_rf_ports_amr(void* it);

arr_rf_ports_ms_amr_t dec_arr_rf_ports_ms_amr(void* it);

arr_rx_channels_amr_t dec_arr_rx_channels_amr(void* it);

arr_tx_channels_amr_t dec_arr_tx_channels_amr(void* it);

cell_amr_t dec_cell_amr(void* it); 

cells_ue_lst_amr_t dec_cells_ue_lst_amr(void* it);

channel_dl_amr_t dec_channel_dl_amr(void* it);

connected_mobility_amr_t dec_connected_mobility_amr(void* it);

cnt_msgs_amr_t dec_cnt_msgs_amr(void* it); 

cpu_amr_t dec_cpu_amr(void* it);

global_gnb_id_amr_t dec_global_gnb_id_amr(void* it);

layers_amr_t dec_layers_amr(void* it);

logs_amr_t dec_logs_amr(void* it); 

ncell_list_amr_t dec_ncell_list_amr(void* it );

ncgi_amr_t dec_ncgi_amr(void* it);

exp_ncgi_amr_t dec_opt_ncgi_amr(void* it);

nr_cells_amr_t dec_nr_cells_amr(void* it);

nssai_amr_t dec_nssai_amr(void* it);

path_amr_t dec_path_amr(void* it);

plmn_list_amr_t dec_plmn_list_amr(void* it);

qos_flows_ue_lst_amr_t dec_qos_flows_ue_lst_amr(void* it);

rf_ports_amr_t dec_rf_ports_amr(void* it);

rf_ports_ms_amr_t dec_rf_ports_ms_amr(void* it);

rx_channels_amr_t dec_rx_channels_amr(void* it);

scell_list_amr_t dec_scell_list_amr(void* it);

tx_channels_amr_t dec_tx_channels_amr(void* it);

ue_lst_amr_t dec_ue_lst_amr(void* it);

#endif
