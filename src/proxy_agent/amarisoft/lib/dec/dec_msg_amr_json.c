#include "dec_msg_amr_json.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "util/alg_ds/alg/defer.h"
#include "dec_cjson.h"
#include "../../../../util/byte_array.h"
#include "../../../../util/alg_ds/alg/string/search_naive.h"

#include "../parser/cJSON/cJSON.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct{
  msg_amr_e type;  
  char* buf;
  size_t len; 
} needle_msg_type_t ;

static
const needle_msg_type_t arr_msgs[5] = {
  {.type = MSG_READY_AMR_E , .buf = "\"ready\"" , .len = 7},
  {.type = MSG_CONFIG_GET_AMR_E , .buf = "\"config_get\"" , .len = 12},
  {.type = MSG_STATS_AMR_E, .buf = "\"stats\"", .len = 7},
  {.type = MSG_UE_GET_E, .buf = "\"ue_get\"", .len = 8},
  {.type = MSG_HANDOVER_E, .buf = "\"handover\"", .len = 10},
};

static
msg_amr_e find_msg_type(const char* src)
{
  const char needle[] = "\"message\"";
  char *pos = strstr(src, needle);
  assert(pos != NULL && "message is mandatory in json file!");

  for(size_t i = 0; i < sizeof(arr_msgs)/sizeof(arr_msgs[0]); ++i){
     char* v = search_naive(arr_msgs[i].len, (char*)arr_msgs[i].buf, 64, pos);
     if(v != NULL) 
       return arr_msgs[i].type;
  }

  printf("%s \n", src);
  fflush(stdout);
  assert(0 != 0 && "Unknown message type");
  
  return END_MSG_AMR_E;
}

msg_amr_t msg_dec_amr_ag(ws_msg_t const* src)
{
  assert(src != NULL);

  msg_amr_t dst = {0}; 

  dst.type = find_msg_type((const char*)src->buf);

  if(dst.type == MSG_READY_AMR_E){
  } else if(dst.type == MSG_CONFIG_GET_AMR_E){
    dec_config_get_amr((char*)src->buf, &dst.config);
  } else if(dst.type == MSG_STATS_AMR_E){
    dec_msg_stats_amr((char*)src->buf, &dst.stats);
  } else if(dst.type == MSG_UE_GET_E){
    dec_msg_ue_get_amr((char*)src->buf, &dst.ue);
  } else if(dst.type == MSG_HANDOVER_E){
    dec_msg_ho_ans_amr((char*)src->buf, &dst.ho);
  } else {
    assert(0 !=0 && "Unknown message type");
  }

  return dst;
}

void dec_config_get_amr(const char* in, msg_config_get_amr_t* out)
{
  assert(in != NULL);
  assert(out != NULL);

  cJSON *json = cJSON_Parse(in);
  defer({ cJSON_Delete(json); });
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    assert(error_ptr != NULL);
    fprintf(stderr, "Error before: %s\n", error_ptr);
  }
  assert(json != NULL && "Error parsing the input json");

  // Message ID. Mandatory 
  out->msg_id = dec_int(json, "message_id"); 

  // Version. Mandatory
  out->version = dec_string_opt(json, "version"); 

  // Type. Mandatory
  out->type = dec_string_opt(json, "type"); 

  // Name. Mandatory
  out->name = dec_string_opt(json, "name"); 

  // License ID. Mandatory
  out->license_id = dec_string_opt(json, "license_id"); 

  // License User. Mandatory
  out->license_user = dec_string_opt(json, "license_user"); 

  // Log. Mandatory
  out->logs = dec_logs_amr(json); 

  // Optional
  // bool* locked;
 
  // TAI. Mandatory
  out->tai = dec_float_opt(json, "tai");  

  // 4G Optional
  // global_enb_id_amr_t* global_enb_id; 
  
  // 5G Optional. We make it mandatory as we only support 5G
  
  ans_cjson_t ans = find_object(json, "global_gnb_id");
  out->global_gnb_id = dec_global_gnb_id_amr((void*)ans.it);

  // 4G mandatory 
  // cells
  
  // 4G NB-IoT 
  // nb_cells 
 
  // 5G nr_cells 
  ans = find_object(json, "nr_cells");
  out->arr_nr_cells = dec_arr_nr_cells_amr((void*)ans.it);

  // Mandatory
  out->arr_rx_channels = dec_arr_rx_channels_amr(json); 
 
  // Mandatory
  out->arr_tx_channels = dec_arr_tx_channels_amr(json); 

  // Mandatory
  out->arr_rf_ports = dec_arr_rf_ports_amr(json);  

  // Mandatory to all messages?
  out->utc = dec_float_opt(json, "utc");  
}

void dec_msg_ho_ans_amr(const char* in, msg_ho_ans_amr_t* out)
{
  assert(in != NULL);
  assert(out != NULL);

  cJSON *json = cJSON_Parse(in);
  defer({ cJSON_Delete(json); });
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    assert(error_ptr != NULL);
    fprintf(stderr, "Error before: %s\n", error_ptr);
  }
  assert(json != NULL && "Error parsing the input json");

  // Message ID. Mandatory 
  out->msg_id = dec_int(json, "message_id"); 
  
  // Error. Optional
  ans_cjson_t ans = find_object(json, "error");
  if(ans.it != NULL){
    assert(((cJSON*)ans.it)->valuestring != NULL); 
    size_t const sz = strlen(((cJSON*)ans.it)->valuestring);
    out->error = calloc(sz + 1, sizeof(char));
    assert(out->error != NULL && "Memory exhausted");
    memcpy(out->error, ((cJSON*)ans.it)->valuestring, sz);
  }

  // Time
  out->time = dec_float_opt(json, "time");  
}

void dec_msg_stats_amr(const char* in, msg_stats_amr_t* out)
{
  assert(in != NULL);
  assert(out != NULL);

  cJSON *json = cJSON_Parse(in);
  defer({ cJSON_Delete(json); });
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    assert(error_ptr != NULL);
    fprintf(stderr, "Error before: %s\n", error_ptr);
  }
  assert(json != NULL && "Error parsing the input json");

   // Message ID. Mandatory 
  out->msg_id = dec_int(json, "message_id"); 

  // Mandatory. Constant over process lifetime.
  out->instance_id = dec_string_opt(json, "instance_id"); 

  // Each member name defines a type and its value cpu load in % of one core.
  out->cpu = dec_cpu_amr(json); 

  // Counters  
  out->counter = dec_cnt_msgs_amr(json); 

  // Cells
  out->cells = dec_arr_cell_amr(json); 

  // Optional bits/sec 
  ans_cjson_t ans = find_object(json, "gtp_tx_bitrate");  
  if(ans.it != NULL){
    out->gtp_tx_bitrate = calloc(1, sizeof(float));
    assert(out->gtp_tx_bitrate != NULL && "Memory exhausted" );
    *out->gtp_tx_bitrate = ((cJSON*)ans.it)->valuedouble;
  }

  // Optional bits/sec
  ans = find_object(json, "gtp_rx_bitrate");  
  if(ans.it != NULL){
    out->gtp_rx_bitrate = calloc(1, sizeof(float));
    assert(out->gtp_rx_bitrate != NULL && "Memory exhausted" );
    *out->gtp_rx_bitrate = ((cJSON*)ans.it)->valuedouble;
  }

  // Each member name is the RF port ID and each value is an object representing the TX-RX latency statistics 
  //arr_rf_ports_ms_amr_t rf_port; 
  out->arr_rf_port = dec_arr_rf_ports_ms_amr(json);

  // Set if samples has been set to true in request
  // samples

  // Mandatory. Time elapsed in seconds since the last call to the stats API
  out->duration = dec_float_opt(json, "duration"); 
}

void dec_msg_ue_get_amr(const char* in, msg_ue_get_t* out)
{
  assert(in != NULL);
  assert(out != NULL);

  cJSON *json = cJSON_Parse(in);
  defer({ cJSON_Delete(json); });
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    assert(error_ptr != NULL);
    fprintf(stderr, "Error before: %s\n", error_ptr);
  }
  assert(json != NULL && "Error parsing the input json");

  // Message ID. Mandatory 
  out->msg_id = dec_int(json, "message_id"); 

  ans_cjson_t ans = find_object(json, "ue_list");

  out->sz = cJSON_GetArraySize(ans.it);
  out->ue_lst = calloc(out->sz, sizeof(ue_lst_amr_t));
  assert(out->ue_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < out->sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    out->ue_lst[i] = dec_ue_lst_amr(tmp);
  }

  out->time = dec_float_opt(json, "time" );
}

arr_cell_amr_t dec_arr_cell_amr(void* it)
{
  assert(it != NULL);

  arr_cell_amr_t dst = {0};

  ans_cjson_t ans = find_object( it, "cells");
 
  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0);

  dst.cells = calloc(dst.sz, sizeof(cell_amr_t));
  assert(dst.cells != NULL && "Memory exhausted");

  dst.names = calloc(dst.sz, sizeof(char*));
  assert(dst.names != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.names[i] = strdup(tmp->string);
    dst.cells[i] = dec_cell_amr(tmp);
  }

  return dst;
}

arr_cells_ue_lst_amr_t dec_arr_cells_ue_lst_amr(void* it)
{
  assert(it != NULL);

  arr_cells_ue_lst_amr_t dst = {0}; 

  dst.sz = cJSON_GetArraySize(it);
  assert(dst.sz > 0);

  dst.cell = calloc(dst.sz, sizeof(cells_ue_lst_amr_t));
  assert(dst.cell != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i);
    dst.cell[i] = dec_cells_ue_lst_amr(tmp);
  }

  return dst;
}

arr_ncell_list_amr_t dec_arr_ncell_list_amr(void* it)
{
  assert(it != NULL);

  arr_ncell_list_amr_t dst = {0}; 
  dst.sz = cJSON_GetArraySize(it);
  assert(dst.sz > 0);

  dst.ncell_lst = calloc(dst.sz, sizeof(ncell_list_amr_t));
  assert(dst.ncell_lst != NULL && "Memory exhausted");
  
  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i);
    dst.ncell_lst[i] = dec_ncell_list_amr(tmp);
  }

  return dst;
}

arr_nr_cells_amr_t dec_arr_nr_cells_amr(void* it)
{
  assert(it != NULL);

  arr_nr_cells_amr_t dst = {0};

  dst.sz = cJSON_GetArraySize(it);
  assert(dst.sz > 0);

  dst.names = calloc(dst.sz , sizeof(char*));
  assert(dst.names != NULL && "Memory exhausted");
  
  dst.nr_cells = calloc(dst.sz, sizeof(nr_cells_amr_t));
  assert(dst.nr_cells != NULL && "Memory exhausted");


  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i);
    dst.names[i] = strdup(tmp->string);  
    dst.nr_cells[i] = dec_nr_cells_amr(tmp);
  }

  return dst;
}

arr_qos_flows_ue_lst_t dec_arr_qos_flows_ue_lst(void* it)
{
  assert(it != NULL);
  arr_qos_flows_ue_lst_t dst = {0}; 

  dst.sz = cJSON_GetArraySize(it);
  if(dst.sz == 0)
    return dst;

  dst.qos_flows = calloc(dst.sz, sizeof(qos_flows_ue_lst_amr_t));
  assert(dst.qos_flows != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(it, i) ;
    dst.qos_flows[i] = dec_qos_flows_ue_lst_amr(tmp);
  }

  return dst;
}

arr_rf_ports_amr_t dec_arr_rf_ports_amr(void* it)
{
  assert(it != NULL);
  arr_rf_ports_amr_t dst = {0};

  ans_cjson_t ans = find_object(it, "rf_ports");

  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0);

  dst.rf_ports_amr = calloc(dst.sz, sizeof(rf_ports_amr_t));
  assert(dst.rf_ports_amr != NULL && "Memory exhausted");

  //dst.name = calloc(dst.sz, sizeof(char*));
  //assert(dst.name != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    //dst.name[i] = strdup(tmp->string);
    dst.rf_ports_amr[i] = dec_rf_ports_amr(tmp);
  }

  return dst;
}

arr_rf_ports_ms_amr_t dec_arr_rf_ports_ms_amr(void* it)
{
  assert(it != NULL);
  arr_rf_ports_ms_amr_t dst = {0};

  ans_cjson_t ans = find_object(it, "rf_ports");

  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0);

  dst.rf_ports_ms_amr = calloc(dst.sz, sizeof(rf_ports_ms_amr_t));
  assert(dst.rf_ports_ms_amr != NULL && "Memory exhausted");

  dst.name = calloc(dst.sz, sizeof(char*));
  assert(dst.name != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.name[i] = strdup(tmp->string);
    // Here I am jumping the rxtx_delay value as it does not add info
    dst.rf_ports_ms_amr[i] = dec_rf_ports_ms_amr(tmp->child);
  }

  return dst;
}

arr_rx_channels_amr_t dec_arr_rx_channels_amr(void* it)
{
  assert(it != NULL);

  arr_rx_channels_amr_t dst = {0}; 

  ans_cjson_t ans = find_object(it, "rx_channels");

  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0 && "Unclear from specs");

  dst.rx_channels_amr = calloc(dst.sz, sizeof(rx_channels_amr_t ));
  assert(dst.rx_channels_amr != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.rx_channels_amr[i] = dec_rx_channels_amr(tmp);
  }

  return dst;
}

arr_tx_channels_amr_t dec_arr_tx_channels_amr(void* it)
{
  assert(it != NULL);
  arr_tx_channels_amr_t dst = {0};

  ans_cjson_t ans = find_object(it, "tx_channels");

  dst.sz = cJSON_GetArraySize(ans.it);
  assert(dst.sz > 0 && "Unclear from specs");

  dst.tx_channels_amr = calloc(dst.sz, sizeof(tx_channels_amr_t ));
  assert(dst.tx_channels_amr != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.tx_channels_amr[i] = dec_tx_channels_amr(tmp);
  }

  return dst;
}

cell_amr_t dec_cell_amr(void* it)
{
  assert(it != NULL);
  cell_amr_t dst = {0};

  dst.dl_bitrate = dec_float_opt(it, "dl_bitrate");
  dst.ul_bitrate = dec_float_opt(it, "ul_bitrate");
  // dst.mbms_bitrate = dec_float(it, "mbms_bitrate");
  dst.dl_tx = dec_int_opt(it, "dl_tx");
  dst.ul_tx = dec_int_opt(it, "ul_tx");
  dst.dl_err = dec_int_opt(it, "dl_err");
  dst.ul_err = dec_int_opt(it, "ul_err");
  dst.dl_retx = dec_int_opt(it, "dl_retx");
  dst.ul_retx = dec_int_opt(it, "ul_retx");

  dst.dl_sched_users_min = dec_float_opt(it, "dl_sched_users_min");
  dst.dl_sched_users_avg = dec_float_opt(it, "dl_sched_users_avg");
  dst.dl_sched_users_max = dec_float_opt(it, "dl_sched_users_max");
  dst.ul_sched_users_min = dec_float_opt(it, "ul_sched_users_min");
  dst.ul_sched_users_avg = dec_float_opt(it, "ul_sched_users_avg");
  dst.ul_sched_users_max = dec_float_opt(it, "ul_sched_users_max");

  dst.dl_use_min = dec_float_opt(it, "dl_use_min");
  dst.dl_use_max = dec_float_opt(it, "dl_use_max");
  dst.dl_use_avg = dec_float_opt(it, "dl_use_avg");
  dst.ul_use_min = dec_float_opt(it, "ul_use_min");
  dst.ul_use_max = dec_float_opt(it, "ul_use_max");
  dst.ul_use_avg = dec_float_opt(it, "ul_use_avg");

  //dst.ctrl_use_min = dec_float(it, "ctrl_use_min");
  //dst.ctrl_use_avg = dec_float(it, "ctrl_use_avg");
  //dst.ctrl_use_max = dec_float(it, "ctrl_use_max");

  dst.ue_count_min = dec_int_opt(it, "ue_count_min");
  dst.ue_count_max = dec_int_opt(it, "ue_count_max");
  dst.ue_count_avg = dec_int_opt(it, "ue_count_avg");
  dst.ue_active_count_min = dec_int_opt(it, "ue_active_count_min");
  dst.ue_active_count_max = dec_int_opt(it, "ue_active_count_max");
  dst.ue_active_count_avg = dec_int_opt(it, "ue_active_count_avg");
  dst.ue_inactive_count_min = dec_int_opt(it, "ue_inactive_count_min");
  dst.ue_inactive_count_max = dec_int_opt(it, "ue_inactive_count_max");
  dst.ue_inactive_count_avg = dec_int_opt(it, "ue_inactive_count_avg");

  // LTE erab values ignored!!

  dst.drb_count_min = dec_int_opt(it, "drb_count_min");
  dst.drb_count_max = dec_int_opt(it, "drb_count_max");
  dst.drb_count_avg = dec_int_opt(it, "drb_count_avg");
 // dst.dl_gbr_use_min = dec_int(it, "dl_gbr_use_min");
 // dst.dl_gbr_use_max = dec_int(it, "dl_gbr_use_max");
 // dst.dl_gbr_use_avg = dec_int(it, "dl_gbr_use_avg");
 // dst.ul_gbr_use_min = dec_int(it, "ul_gbr_use_min");
 // dst.ul_gbr_use_max = dec_int(it, "ul_gbr_use_max");
 // dst.ul_gbr_use_avgint = dec_int(it, "ul_gbr_use_avgint");

  dst.counter = dec_cnt_msgs_amr(it);

  return dst;
}

cells_ue_lst_amr_t dec_cells_ue_lst_amr(void *it) 
{
  assert(it != NULL);
  cells_ue_lst_amr_t dst = {0};

  dst.cell_id = dec_int(it, "cell_id");

  dst.cqi = dec_int_opt(it, "cqi");
  dst.ri = dec_int_opt(it, "ri");
  dst.dl_bitrate = dec_int_opt(it, "dl_bitrate");
  dst.ul_bitrate = dec_int_opt(it, "ul_bitrate");
  dst.ul_rank = dec_int_opt(it, "ul_rank");
  dst.dl_tx = dec_int_opt(it, "dl_tx");
  dst.ul_tx = dec_int_opt(it, "ul_tx");
  dst.dl_err = dec_int_opt(it, "dl_err");
  dst.ul_err = dec_int_opt(it, "ul_err");
  dst.dl_retx = dec_int_opt(it, "dl_retx");
  dst.ul_retx = dec_int_opt(it, "ul_retx");
  dst.dl_mcs = dec_int_opt(it, "dl_mcs");
  dst.ul_mcs = dec_int_opt(it, "ul_mcs");
  dst.ul_n_layer = dec_int_opt(it, "ul_n_layer");
  dst.turbo_decoder_min = dec_int_opt(it, "turbo_decoder_min");
  dst.turbo_decoder_avg = dec_int_opt(it, "turbo_decoder_avg");
  dst.turbo_decoder_max = dec_int_opt(it, "turbo_decoder_max");
  dst.pusch_snr = dec_int_opt(it, "pusch_snr");
  dst.epre = dec_int_opt(it, "epre");
  dst.ul_phr = dec_int_opt(it, "ul_phr");
  dst.ul_path_loss = dec_int_opt(it, "ul_path_loss");
  dst.p_ue = dec_int_opt(it, "p_ue");
  dst.initial_ta = dec_int_opt(it, "initial_ta");

  return dst;
}

channel_dl_amr_t dec_channel_dl_amr(void* it)
{
  assert(it != NULL);

  channel_dl_amr_t dst = {0}; 

  // Mandatory. Array of float. Noise level table, in dB
  ans_cjson_t ans = find_object(it, "noise_level");

  dst.sz_noise_level = cJSON_GetArraySize(ans.it);
  assert(dst.sz_noise_level != 0 && "Not clear from specs" );
  dst.noise_level = calloc(dst.sz_noise_level, sizeof(float));
  assert(dst.noise_level != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_noise_level; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.noise_level[i] = tmp->valuedouble;
  }

  // Mandatory. Global frequency shift in Hz
  dst.freq_shift = dec_float_opt(it, "freq_shift"); 

  // Mandatory. Array Each object contains the
  // parameters of a path
  
  ans_cjson_t ans_path = find_object(it, "path");

  dst.sz_path = cJSON_GetArraySize(ans_path.it);
  assert(dst.sz_path != 0 && "Not clear from specs" );
  dst.path = calloc(dst.sz_path, sizeof(path_amr_t));
  assert(dst.path != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_path; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans_path.it, i);
    dst.path[i] = dec_path_amr(tmp);
  }

  return dst;
}

connected_mobility_amr_t dec_connected_mobility_amr(void* it)
{
  assert(it != NULL);
  connected_mobility_amr_t dst = {0}; 

  //Mandatory. 
  dst.scell_config = dec_int_opt(it, "scell_config");

  //Mandatory. 
  dst.nr_handover = dec_int_opt(it, "nr_handover");

  //Mandatory.
  dst.nr_cell_redirect = dec_int_opt(it, "nr_cell_redirect");

  //Mandatory.
  dst.eutra_handover = dec_int_opt(it, "eutra_handover");

  //Mandatory.
  dst.eutra_cell_redirect = dec_int_opt(it, "eutra_cell_redirect");

  //Mandatory.
  dst.nr_dc_setup = dec_int_opt(it, "nr_dc_setup");

  //Mandatory.
  dst.mr_dc_release = dec_int_opt(it, "mr_dc_release");

  return dst;
}

cnt_msgs_amr_t dec_cnt_msgs_amr(void* it)
{
  assert(it != NULL);
  cnt_msgs_amr_t dst = {0}; 

  // Global events:
  //Message events: 
  
  //dst.s1_handover_required = dec_int(it, "s1_handover_required"); 
  //dst.s1_handover_command = dec_int(it, "s1_handover_command");  
  //dst.s1_handover_preparation_failure = dec_int(it, "s1_handover_preparation_failure");  
  //dst.s1_handover_request = dec_int(it, "s1_handover_request");  
  //dst.s1_handover_request_ack = dec_int(it, "s1_handover_request_ack");  
  //dst.s1_handover_failure = dec_int(it, "s1_handover_failure");  
  //dst.s1_handover_notify = dec_int(it, "s1_handover_notify");  
  //dst.s1_path_switch_request = dec_int(it, "s1_path_switch_request");  
  //dst.s1_path_switch_request_ack = dec_int(it, "s1_path_switch_request_ack");  
  //dst.s1_path_switch_request_failure = dec_int(it, "s1_path_switch_request_failure");  
  //dst.s1_handover_cancel = dec_int(it, "s1_handover_cancel");  
  //dst.s1_handover_cancel_ack = dec_int(it, "s1_handover_cancel_ack");  
  //dst.s1_erab_setup_request = dec_int(it, "s1_erab_setup_request");  
  //dst.s1_erab_setup_response = dec_int(it, "s1_erab_setup_response");  
  //dst.s1_erab_modify_request = dec_int(it, "s1_erab_modify_request");  
  //dst.s1_erab_modify_response = dec_int(it, "s1_erab_modify_response");  
  //dst.s1_erab_release_command = dec_int(it, "s1_erab_release_command");  
  //dst.s1_erab_release_response = dec_int(it, "s1_erab_release_response");  
  //dst.s1_erab_release_indication = dec_int(it, "s1_erab_release_indication");  
  //dst.s1_initial_context_setup_request = dec_int(it, "s1_initial_context_setup_request");  
  //dst.s1_initial_context_setup_response = dec_int(it, "s1_initial_context_setup_response");  
  //dst.s1_initial_context_setup_failure = dec_int(it, "s1_initial_context_setup_failure");  
  //dst.s1_paging = dec_int(it, "s1_paging");  
  //dst.s1_downlink_nas_transport = dec_int(it, "s1_downlink_nas_transport");  
  //dst.s1_initial_ue_message = dec_int(it, "s1_initial_ue_message");  
  //dst.s1_uplink_nas_transport = dec_int(it, "s1_uplink_nas_transport");  
  //dst.s1_reset = dec_int(it, "s1_reset");  
  //dst.s1_reset_ack = dec_int(it, "s1_reset_ack");  
  //dst.s1_error_indication = dec_int(it, "s1_error_indication");  
  //dst.s1_nas_non_delivery_indication = dec_int(it, "s1_nas_non_delivery_indication");  
  //dst.s1_setup_request = dec_int(it, "s1_setup_request");  
  //dst.s1_setup_response = dec_int(it, "s1_setup_response");  
  //dst.s1_setup_failure = dec_int(it, "s1_setup_failure");  
  //dst.s1_downlink_s1_cdma2000_tunnelling = dec_int(it, "s1_downlink_s1_cdma2000_tunnelling");  
  //dst.s1_uplink_s1_cdma2000_tunnelling = dec_int(it, "s1_uplink_s1_cdma2000_tunnelling");  
  //dst.s1_ue_context_modification_request = dec_int(it, "s1_ue_context_modification_request");  
  //dst.s1_ue_context_modification_response = dec_int(it, "s1_ue_context_modification_response");  
  //dst.s1_ue_context_modification_failure = dec_int(it, "s1_ue_context_modification_failure");  
  //dst.s1_ue_capability_info_indication = dec_int(it, "s1_ue_capability_info_indication");  
  //dst.s1_ue_context_release_request = dec_int(it, "s1_ue_context_release_request");  
  //dst.s1_ue_context_release_command = dec_int(it, "s1_ue_context_release_command");  
  //dst.s1_ue_context_release_complete = dec_int(it, "s1_ue_context_release_complete");  
  //dst.s1_enb_status_transfer = dec_int(it, "s1_enb_status_transfer");  
  //dst.s1_mme_status_transfer = dec_int(it, "s1_mme_status_transfer");  
  //dst.s1_deactivate_trace = dec_int(it, "s1_deactivate_trace");  
  //dst.s1_trace_start = dec_int(it, "s1_trace_start");  
  //dst.s1_trace_failure_indication = dec_int(it, "s1_trace_failure_indication");  
  //dst.s1_enb_configuration_update = dec_int(it, "s1_enb_configuration_update");  
  //dst.s1_enb_configuration_update_ack = dec_int(it, "s1_enb_configuration_update_ack");  
  //dst.s1_enb_configuration_update_failure = dec_int(it, "s1_enb_configuration_update_failure");  
  //dst.s1_mme_configuration_update = dec_int(it, "s1_mme_configuration_update");  
  //dst.s1_mme_configuration_update_ack = dec_int(it, "s1_mme_configuration_update_ack");  
  //dst.s1_mme_configuration_update_failure = dec_int(it, "s1_mme_configuration_update_failure");  
  //dst.s1_location_reporting_control = dec_int(it, "s1_location_reporting_control");  
  //dst.s1_location_reporting_failure_indication = dec_int(it, "s1_location_reporting_failure_indication");  
  //dst.s1_location_report = dec_int(it, "s1_location_report");  
  //dst.s1_overload_start = dec_int(it, "s1_overload_start");  
  //dst.s1_overload_stop = dec_int(it, "s1_overload_stop");  
  //dst.s1_write_replace_warning_request = dec_int(it, "s1_write_replace_warning_request");  
  //dst.s1_write_replace_warning_response = dec_int(it, "s1_write_replace_warning_response");  
  //dst.s1_enb_direct_information_transfer = dec_int(it, "s1_enb_direct_information_transfer");  
  //dst.s1_mme_direct_information_transfer = dec_int(it, "s1_mme_direct_information_transfer");  
  //dst.s1_private_message = dec_int(it, "s1_private_message");  
  //dst.s1_enb_configuration_transfer = dec_int(it, "s1_enb_configuration_transfer");  
  //dst.s1_mme_configuration_transfer = dec_int(it, "s1_mme_configuration_transfer");  
  //dst.s1_cell_traffic_trace = dec_int(it, "s1_cell_traffic_trace");  
  //dst.s1_kill_request = dec_int(it, "s1_kill_request");  
  //dst.s1_kill_response = dec_int(it, "s1_kill_response");  
  //dst.s1_downlink_ue_associated_lppa_transport = dec_int(it, "s1_downlink_ue_associated_lppa_transport");  
  //dst.s1_uplink_ue_associated_lppa_transport = dec_int(it, "s1_uplink_ue_associated_lppa_transport");  
  //dst.s1_downlink_non_ue_associated_lppa_transport = dec_int(it, "s1_downlink_non_ue_associated_lppa_transport");  
  //dst.s1_uplink_non_ue_associated_lppa_transport = dec_int(it, "s1_uplink_non_ue_associated_lppa_transport");  
  //dst.s1_ue_radio_capability_match_request = dec_int(it, "s1_ue_radio_capability_match_request");  
  //dst.s1_ue_radio_capability_match_response = dec_int(it, "s1_ue_radio_capability_match_response");  
  //dst.s1_pws_restart_indication = dec_int(it, "s1_pws_restart_indication");  
  //dst.s1_erab_modification_indication = dec_int(it, "s1_erab_modification_indication");  
  //dst.s1_erab_modification_confirm = dec_int(it, "s1_erab_modification_confirm");  
  //dst.s1_pws_failure_indication = dec_int(it, "s1_pws_failure_indication");  
  //dst.s1_reroute_nas_request = dec_int(it, "s1_reroute_nas_request");  
  //dst.s1_ue_context_modification_indication = dec_int(it, "s1_ue_context_modification_indication");  
  //dst.s1_ue_context_modification_confirm = dec_int(it, "s1_ue_context_modification_confirm");  
  //dst.s1_connection_establishment_indication = dec_int(it, "s1_connection_establishment_indication");  
  //dst.s1_ue_context_suspend_request = dec_int(it, "s1_ue_context_suspend_request");  
  //dst.s1_ue_context_suspend_response = dec_int(it, "s1_ue_context_suspend_response");  
  //dst.s1_ue_context_resume_request = dec_int(it, "s1_ue_context_resume_request");  
  //dst.s1_ue_context_resume_response = dec_int(it, "s1_ue_context_resume_response");  
  //dst.s1_ue_context_resume_failure = dec_int(it, "s1_ue_context_resume_failure");  
  //dst.s1_nas_delivery_indication = dec_int(it, "s1_nas_delivery_indication");  
  //dst.s1_retrieve_ue_information = dec_int(it, "s1_retrieve_ue_information");  
  //dst.s1_ue_information_transfer = dec_int(it, "s1_ue_information_transfer");  
  //dst.s1_enb_cp_relocation_indication = dec_int(it, "s1_enb_cp_relocation_indication");  
  //dst.s1_mme_cp_relocation_indication = dec_int(it, "s1_mme_cp_relocation_indication");  
  //dst.s1_secondary_rat_data_usage_report = dec_int(it, "s1_secondary_rat_data_usage_report");  
  //dst.s1_ue_radio_capability_id_mapping_request = dec_int(it, "s1_ue_radio_capability_id_mapping_request");  
  //dst.s1_ue_radio_capability_id_mapping_response = dec_int(it, "s1_ue_radio_capability_id_mapping_response");  
  //dst.s1_handover_success = dec_int(it, "s1_handover_success");  
  //dst.s1_enb_early_status_transfer = dec_int(it, "s1_enb_early_status_transfer");  
  //dst.s1_mme_early_status_transfer = dec_int(it, "s1_mme_early_status_transfer");  
  //dst.x2_handover_request_sent = dec_int(it, "x2_handover_request_sent");  
  //dst.x2_handover_request_recv = dec_int(it, "x2_handover_request_recv");  
  //dst.x2_handover_request_ack_sent = dec_int(it, "x2_handover_request_ack_sent");  
  //dst.x2_handover_request_ack_recv = dec_int(it, "x2_handover_request_ack_recv");  
  //dst.x2_handover_preparation_failure_sent = dec_int(it, "x2_handover_preparation_failure_sent");  
  //dst.x2_handover_preparation_failure_recv = dec_int(it, "x2_handover_preparation_failure_recv");  
  //dst.x2_handover_cancel_sent = dec_int(it, "x2_handover_cancel_sent");  
  //dst.x2_handover_cancel_recv = dec_int(it, "x2_handover_cancel_recv");  
  //dst.x2_load_indication_sent = dec_int(it, "x2_load_indication_sent");  
  //dst.x2_load_indication_recv = dec_int(it, "x2_load_indication_recv");  
  //dst.x2_error_indication_sent = dec_int(it, "x2_error_indication_sent");  
  //dst.x2_error_indication_recv = dec_int(it, "x2_error_indication_recv");  
  //dst.x2_sn_status_transfer_sent = dec_int(it, "x2_sn_status_transfer_sent");  
  //dst.x2_sn_status_transfer_recv = dec_int(it, "x2_sn_status_transfer_recv");  
  //dst.x2_ue_context_release_sent = dec_int(it, "x2_ue_context_release_sent");  
  //dst.x2_ue_context_release_recv = dec_int(it, "x2_ue_context_release_recv");  
  //dst.x2_setup_request_sent = dec_int(it, "x2_setup_request_sent");  
  //dst.x2_setup_request_recv = dec_int(it, "x2_setup_request_recv");  
  //dst.x2_setup_response_sent = dec_int(it, "x2_setup_response_sent");  
  //dst.x2_setup_response_recv = dec_int(it, "x2_setup_response_recv");  
  //dst.x2_setup_failure_sent = dec_int(it, "x2_setup_failure_sent");  
  //dst.x2_setup_failure_recv = dec_int(it, "x2_setup_failure_recv");  
  //dst.x2_reset_sent = dec_int(it, "x2_reset_sent");  
  //dst.x2_reset_recv = dec_int(it, "x2_reset_recv");  
  //dst.x2_reset_response_sent = dec_int(it, "x2_reset_response_sent");  
  //dst.x2_reset_response_recv = dec_int(it, "x2_reset_response_recv");  
  //dst.x2_enb_configuration_update_sent = dec_int(it, "x2_enb_configuration_update_sent");  
  //dst.x2_enb_configuration_update_recv = dec_int(it, "x2_enb_configuration_update_recv");  
  //dst.x2_enb_configuration_update_ack_sent = dec_int(it, "x2_enb_configuration_update_ack_sent");  
  //dst.x2_enb_configuration_update_ack_recv = dec_int(it, "x2_enb_configuration_update_ack_recv");  
  //dst.x2_enb_configuration_update_failure_sent = dec_int(it, "x2_enb_configuration_update_failure_sent");  
  //dst.x2_enb_configuration_update_failure_recv = dec_int(it, "x2_enb_configuration_update_failure_recv");  
  //dst.x2_resource_status_request_sent = dec_int(it, "x2_resource_status_request_sent");  
  //dst.x2_resource_status_request_recv = dec_int(it, "x2_resource_status_request_recv");  
  //dst.x2_resource_status_response_sent = dec_int(it, "x2_resource_status_response_sent");  
  //dst.x2_resource_status_response_recv = dec_int(it, "x2_resource_status_response_recv");  
  //dst.x2_resource_status_failure_sent = dec_int(it, "x2_resource_status_failure_sent");  
  //dst.x2_resource_status_failure_recv = dec_int(it, "x2_resource_status_failure_recv");  
  //dst.x2_resource_status_update_sent = dec_int(it, "x2_resource_status_update_sent");  
  //dst.x2_resource_status_update_recv = dec_int(it, "x2_resource_status_update_recv");  
  //dst.x2_private_message_sent = dec_int(it, "x2_private_message_sent");  
  //dst.x2_private_message_recv = dec_int(it, "x2_private_message_recv");  
  //dst.x2_mobility_change_request_sent = dec_int(it, "x2_mobility_change_request_sent");  
  //dst.x2_mobility_change_request_recv = dec_int(it, "x2_mobility_change_request_recv");  
  //dst.x2_mobility_change_ack_sent = dec_int(it, "x2_mobility_change_ack_sent");  
  //dst.x2_mobility_change_ack_recv = dec_int(it, "x2_mobility_change_ack_recv");  
  //dst.x2_mobility_change_failure_sent = dec_int(it, "x2_mobility_change_failure_sent");  
  //dst.x2_mobility_change_failure_recv = dec_int(it, "x2_mobility_change_failure_recv");  
  //dst.x2_rlf_indication_sent = dec_int(it, "x2_rlf_indication_sent");  
  //dst.x2_rlf_indication_recv = dec_int(it, "x2_rlf_indication_recv");  
  //dst.x2_handover_report_sent = dec_int(it, "x2_handover_report_sent");  
  //dst.x2_handover_report_recv = dec_int(it, "x2_handover_report_recv");  
  //dst.x2_cell_activation_request_sent = dec_int(it, "x2_cell_activation_request_sent");  
  //dst.x2_cell_activation_request_recv = dec_int(it, "x2_cell_activation_request_recv");  
  //dst.x2_cell_activation_response_sent = dec_int(it, "x2_cell_activation_response_sent");  
  //dst.x2_cell_activation_response_recv = dec_int(it, "x2_cell_activation_response_recv");  
  //dst.x2_cell_activation_failure_sent = dec_int(it, "x2_cell_activation_failure_sent");  
  //dst.x2_cell_activation_failure_recv = dec_int(it, "x2_cell_activation_failure_recv");  
  //dst.x2_release_sent = dec_int(it, "x2_release_sent");  
  //dst.x2_release_recv = dec_int(it, "x2_release_recv");  
  //dst.x2_message_transfer_sent = dec_int(it, "x2_message_transfer_sent");  
  //dst.x2_message_transfer_recv = dec_int(it, "x2_message_transfer_recv");  
  //dst.x2_removal_request_sent = dec_int(it, "x2_removal_request_sent");  
  //dst.x2_removal_request_recv = dec_int(it, "x2_removal_request_recv");  
  //dst.x2_removal_response_sent = dec_int(it, "x2_removal_response_sent");  
  //dst.x2_removal_response_recv = dec_int(it, "x2_removal_response_recv");  
  //dst.x2_removal_failure_sent = dec_int(it, "x2_removal_failure_sent");  
  //dst.x2_removal_failure_recv = dec_int(it, "x2_removal_failure_recv");  
  //dst.x2_senb_addition_request_sent = dec_int(it, "x2_senb_addition_request_sent");  
  //dst.x2_senb_addition_request_recv = dec_int(it, "x2_senb_addition_request_recv");  
  //dst.x2_senb_addition_request_acknowledge_sent = dec_int(it, "x2_senb_addition_request_acknowledge_sent");  
  //dst.x2_senb_addition_request_acknowledge_recv = dec_int(it, "x2_senb_addition_request_acknowledge_recv");  
  //dst.x2_senb_addition_request_reject_sent = dec_int(it, "x2_senb_addition_request_reject_sent");  
  //dst.x2_senb_addition_request_reject_recv = dec_int(it, "x2_senb_addition_request_reject_recv");  
  //dst.x2_senb_reconfiguration_complete_sent = dec_int(it, "x2_senb_reconfiguration_complete_sent");  
  //dst.x2_senb_reconfiguration_complete_recv = dec_int(it, "x2_senb_reconfiguration_complete_recv");  
  //dst.x2_senb_modification_request_sent = dec_int(it, "x2_senb_modification_request_sent");  
  //dst.x2_senb_modification_request_recv = dec_int(it, "x2_senb_modification_request_recv");  
  //dst.x2_senb_modification_request_acknowledge_sent = dec_int(it, "x2_senb_modification_request_acknowledge_sent");  
  //dst.x2_senb_modification_request_acknowledge_recv = dec_int(it, "x2_senb_modification_request_acknowledge_recv");  
  //dst.x2_senb_modification_request_reject_sent = dec_int(it, "x2_senb_modification_request_reject_sent");  
  //dst.x2_senb_modification_request_reject_recv = dec_int(it, "x2_senb_modification_request_reject_recv");  
  //dst.x2_senb_modification_required_sent = dec_int(it, "x2_senb_modification_required_sent");  
  //dst.x2_senb_modification_required_recv = dec_int(it, "x2_senb_modification_required_recv");  
  //dst.x2_senb_modification_confirm_sent = dec_int(it, "x2_senb_modification_confirm_sent");  
  //dst.x2_senb_modification_confirm_recv = dec_int(it, "x2_senb_modification_confirm_recv");  
  //dst.x2_senb_modification_refuse_sent = dec_int(it, "x2_senb_modification_refuse_sent");  
  //dst.x2_senb_modification_refuse_recv = dec_int(it, "x2_senb_modification_refuse_recv");  
  //dst.x2_senb_release_request_sent = dec_int(it, "x2_senb_release_request_sent");  
  //dst.x2_senb_release_request_recv = dec_int(it, "x2_senb_release_request_recv");  
  //dst.x2_senb_release_required_sent = dec_int(it, "x2_senb_release_required_sent");  
  //dst.x2_senb_release_required_recv = dec_int(it, "x2_senb_release_required_recv");  
  //dst.x2_senb_release_confirm_sent = dec_int(it, "x2_senb_release_confirm_sent");  
  //dst.x2_senb_release_confirm_recv = dec_int(it, "x2_senb_release_confirm_recv");  
  //dst.x2_senb_counter_check_request_sent = dec_int(it, "x2_senb_counter_check_request_sent");  
  //dst.x2_senb_counter_check_request_recv = dec_int(it, "x2_senb_counter_check_request_recv");  
  //dst.x2_retrieve_ue_context_request_sent = dec_int(it, "x2_retrieve_ue_context_request_sent");  
  //dst.x2_retrieve_ue_context_request_recv = dec_int(it, "x2_retrieve_ue_context_request_recv");  
  //dst.x2_retrieve_ue_context_response_sent = dec_int(it, "x2_retrieve_ue_context_response_sent");  
  //dst.x2_retrieve_ue_context_response_recv = dec_int(it, "x2_retrieve_ue_context_response_recv");  
  //dst.x2_retrieve_ue_context_failure_sent = dec_int(it, "x2_retrieve_ue_context_failure_sent");  
  //dst.x2_retrieve_ue_context_failure_recv = dec_int(it, "x2_retrieve_ue_context_failure_recv");  
  //dst.x2_sgnb_addition_request_sent = dec_int(it, "x2_sgnb_addition_request_sent");  
  //dst.x2_sgnb_addition_request_recv = dec_int(it, "x2_sgnb_addition_request_recv");  
  //dst.x2_sgnb_addition_request_acknowledge_sent = dec_int(it, "x2_sgnb_addition_request_acknowledge_sent");  
  //dst.x2_sgnb_addition_request_acknowledge_recv = dec_int(it, "x2_sgnb_addition_request_acknowledge_recv");  
  //dst.x2_sgnb_addition_request_reject_sent = dec_int(it, "x2_sgnb_addition_request_reject_sent");  
  //dst.x2_sgnb_addition_request_reject_recv = dec_int(it, "x2_sgnb_addition_request_reject_recv");  
  //dst.x2_sgnb_reconfiguration_complete_sent = dec_int(it, "x2_sgnb_reconfiguration_complete_sent");  
  //dst.x2_sgnb_reconfiguration_complete_recv = dec_int(it, "x2_sgnb_reconfiguration_complete_recv");  
  //dst.x2_sgnb_modification_request_sent = dec_int(it, "x2_sgnb_modification_request_sent");  
  //dst.x2_sgnb_modification_request_recv = dec_int(it, "x2_sgnb_modification_request_recv");  
  //dst.x2_sgnb_modification_request_acknowledge_sent = dec_int(it, "x2_sgnb_modification_request_acknowledge_sent");  
  //dst.x2_sgnb_modification_request_acknowledge_recv = dec_int(it, "x2_sgnb_modification_request_acknowledge_recv");  
  //dst.x2_sgnb_modification_request_reject_sent = dec_int(it, "x2_sgnb_modification_request_reject_sent");  
  //dst.x2_sgnb_modification_request_reject_recv = dec_int(it, "x2_sgnb_modification_request_reject_recv");  
  //dst.x2_sgnb_modification_required_sent = dec_int(it, "x2_sgnb_modification_required_sent");  
  //dst.x2_sgnb_modification_required_recv = dec_int(it, "x2_sgnb_modification_required_recv");  
  //dst.x2_sgnb_modification_confirm_sent = dec_int(it, "x2_sgnb_modification_confirm_sent");  
  //dst.x2_sgnb_modification_confirm_recv = dec_int(it, "x2_sgnb_modification_confirm_recv");  
  //dst.x2_sgnb_modification_refuse_sent = dec_int(it, "x2_sgnb_modification_refuse_sent");  
  //dst.x2_sgnb_modification_refuse_recv = dec_int(it, "x2_sgnb_modification_refuse_recv");  
  //dst.x2_sgnb_release_request_sent = dec_int(it, "x2_sgnb_release_request_sent");  
  //dst.x2_sgnb_release_request_recv = dec_int(it, "x2_sgnb_release_request_recv");  
  //dst.x2_sgnb_release_request_acknowledge_sent = dec_int(it, "x2_sgnb_release_request_acknowledge_sent");  
  //dst.x2_sgnb_release_request_acknowledge_recv = dec_int(it, "x2_sgnb_release_request_acknowledge_recv");  
  //dst.x2_sgnb_release_request_reject_sent = dec_int(it, "x2_sgnb_release_request_reject_sent");  
  //dst.x2_sgnb_release_request_reject_recv = dec_int(it, "x2_sgnb_release_request_reject_recv");  
  //dst.x2_sgnb_release_required_sent = dec_int(it, "x2_sgnb_release_required_sent");  
  //dst.x2_sgnb_release_required_recv = dec_int(it, "x2_sgnb_release_required_recv");  
  //dst.x2_sgnb_release_confirm_sent = dec_int(it, "x2_sgnb_release_confirm_sent");  
  //dst.x2_sgnb_release_confirm_recv = dec_int(it, "x2_sgnb_release_confirm_recv");  
  //dst.x2_sgnb_counter_check_request_sent = dec_int(it, "x2_sgnb_counter_check_request_sent");  
  //dst.x2_sgnb_counter_check_request_recv = dec_int(it, "x2_sgnb_counter_check_request_recv");  
  //dst.x2_sgnb_change_required_sent = dec_int(it, "x2_sgnb_change_required_sent");  
  //dst.x2_sgnb_change_required_recv = dec_int(it, "x2_sgnb_change_required_recv");  
  //dst.x2_sgnb_change_confirm_sent = dec_int(it, "x2_sgnb_change_confirm_sent");  
  //dst.x2_sgnb_change_confirm_recv = dec_int(it, "x2_sgnb_change_confirm_recv");  
  //dst.x2_sgnb_change_refuse_sent = dec_int(it, "x2_sgnb_change_refuse_sent");  
  //dst.x2_sgnb_change_refuse_recv = dec_int(it, "x2_sgnb_change_refuse_recv");  
  //dst.x2_rrc_transfer_sent = dec_int(it, "x2_rrc_transfer_sent");  
  //dst.x2_rrc_transfer_recv = dec_int(it, "x2_rrc_transfer_recv");  
  //dst.x2_endc_x2_setup_request_sent = dec_int(it, "x2_endc_x2_setup_request_sent");  
  //dst.x2_endc_x2_setup_request_recv = dec_int(it, "x2_endc_x2_setup_request_recv");  
  //dst.x2_endc_x2_setup_response_sent = dec_int(it, "x2_endc_x2_setup_response_sent");  
  //dst.x2_endc_x2_setup_response_recv = dec_int(it, "x2_endc_x2_setup_response_recv");  
  //dst.x2_endc_x2_setup_failure_sent = dec_int(it, "x2_endc_x2_setup_failure_sent");  
  //dst.x2_endc_x2_setup_failure_recv = dec_int(it, "x2_endc_x2_setup_failure_recv");  
  //dst.x2_endc_configuration_update_sent = dec_int(it, "x2_endc_configuration_update_sent");  
  //dst.x2_endc_configuration_update_recv = dec_int(it, "x2_endc_configuration_update_recv");  
  //dst.x2_endc_configuration_update_acknowledge_sent = dec_int(it, "x2_endc_configuration_update_acknowledge_sent");  
  //dst.x2_endc_configuration_update_acknowledge_recv = dec_int(it, "x2_endc_configuration_update_acknowledge_recv");  
  //dst.x2_endc_configuration_update_failure_sent = dec_int(it, "x2_endc_configuration_update_failure_sent");  
  //dst.x2_endc_configuration_update_failure_recv = dec_int(it, "x2_endc_configuration_update_failure_recv");  
  //dst.x2_secondary_rat_data_usage_report_sent = dec_int(it, "x2_secondary_rat_data_usage_report_sent");  
  //dst.x2_secondary_rat_data_usage_report_recv = dec_int(it, "x2_secondary_rat_data_usage_report_recv");  
  //dst.x2_endc_cell_activation_request_sent = dec_int(it, "x2_endc_cell_activation_request_sent");  
  //dst.x2_endc_cell_activation_request_recv = dec_int(it, "x2_endc_cell_activation_request_recv");  
  //dst.x2_endc_cell_activation_response_sent = dec_int(it, "x2_endc_cell_activation_response_sent");  
  //dst.x2_endc_cell_activation_response_recv = dec_int(it, "x2_endc_cell_activation_response_recv");  
  //dst.x2_endc_cell_activation_failure_sent = dec_int(it, "x2_endc_cell_activation_failure_sent");  
  //dst.x2_endc_cell_activation_failure_recv = dec_int(it, "x2_endc_cell_activation_failure_recv");  
  //dst.x2_endc_partial_reset_required_sent = dec_int(it, "x2_endc_partial_reset_required_sent");  
  //dst.x2_endc_partial_reset_required_recv = dec_int(it, "x2_endc_partial_reset_required_recv");  
  //dst.x2_endc_partial_reset_confirm_sent = dec_int(it, "x2_endc_partial_reset_confirm_sent");  
  //dst.x2_endc_partial_reset_confirm_recv = dec_int(it, "x2_endc_partial_reset_confirm_recv");  
  //dst.x2_endc_eutran_nr_cell_resource_coordination_request_sent = dec_int(it, "x2_endc_eutran_nr_cell_resource_coordination_request_sent");  
  //dst.x2_endc_eutran_nr_cell_resource_coordination_request_recv = dec_int(it, "x2_endc_eutran_nr_cell_resource_coordination_request_recv");  
  //dst.x2_endc_eutran_nr_cell_resource_coordination_response_sent = dec_int(it, "x2_endc_eutran_nr_cell_resource_coordination_response_sent");  
  //dst.x2_endc_eutran_nr_cell_resource_coordination_response_recv = dec_int(it, "x2_endc_eutran_nr_cell_resource_coordination_response_recv");  
  //dst.x2_sgnb_activity_notification_sent = dec_int(it, "x2_sgnb_activity_notification_sent");  
  //dst.x2_sgnb_activity_notification_recv = dec_int(it, "x2_sgnb_activity_notification_recv");  
  //dst.x2_endc_x2_removal_request_sent = dec_int(it, "x2_endc_x2_removal_request_sent");  
  //dst.x2_endc_x2_removal_request_recv = dec_int(it, "x2_endc_x2_removal_request_recv");  
  //dst.x2_endc_x2_removal_response_sent = dec_int(it, "x2_endc_x2_removal_response_sent");  
  //dst.x2_endc_x2_removal_response_recv = dec_int(it, "x2_endc_x2_removal_response_recv");  
  //dst.x2_endc_x2_removal_failure_sent = dec_int(it, "x2_endc_x2_removal_failure_sent");  
  //dst.x2_endc_x2_removal_failure_recv = dec_int(it, "x2_endc_x2_removal_failure_recv");  
  //dst.x2_data_forwarding_address_indication_sent = dec_int(it, "x2_data_forwarding_address_indication_sent");  
  //dst.x2_data_forwarding_address_indication_recv = dec_int(it, "x2_data_forwarding_address_indication_recv");  
  //dst.x2_gnb_status_indication_sent = dec_int(it, "x2_gnb_status_indication_sent");  
  //dst.x2_gnb_status_indication_recv = dec_int(it, "x2_gnb_status_indication_recv");  
  //dst.x2_deactivate_trace_sent = dec_int(it, "x2_deactivate_trace_sent");  
  //dst.x2_deactivate_trace_recv = dec_int(it, "x2_deactivate_trace_recv");  
  //dst.x2_trace_start_sent = dec_int(it, "x2_trace_start_sent");  
  //dst.x2_trace_start_recv = dec_int(it, "x2_trace_start_recv");  
  //dst.x2_endc_configuration_transfer_sent = dec_int(it, "x2_endc_configuration_transfer_sent");  
  //dst.x2_endc_configuration_transfer_recv = dec_int(it, "x2_endc_configuration_transfer_recv");  
  //dst.x2_handover_success_sent = dec_int(it, "x2_handover_success_sent");  
  //dst.x2_handover_success_recv = dec_int(it, "x2_handover_success_recv");  
  //dst.x2_conditional_handover_cancel_sent = dec_int(it, "x2_conditional_handover_cancel_sent");  
  //dst.x2_conditional_handover_cancel_recv = dec_int(it, "x2_conditional_handover_cancel_recv");  
  //dst.x2_early_status_transfer_sent = dec_int(it, "x2_early_status_transfer_sent");  
  //dst.x2_early_status_transfer_recv = dec_int(it, "x2_early_status_transfer_recv");  
  //dst.x2_cell_traffic_trace_sent = dec_int(it, "x2_cell_traffic_trace_sent");  
  //dst.x2_cell_traffic_trace_recv = dec_int(it, "x2_cell_traffic_trace_recv");  
  //dst.x2_endc_resource_status_update_sent = dec_int(it, "x2_endc_resource_status_update_sent");  
  //dst.x2_endc_resource_status_update_recv = dec_int(it, "x2_endc_resource_status_update_recv");  
  //dst.x2_endc_resource_status_request_sent = dec_int(it, "x2_endc_resource_status_request_sent");  
  //dst.x2_endc_resource_status_request_recv = dec_int(it, "x2_endc_resource_status_request_recv");  
  //dst.x2_endc_resource_status_response_sent = dec_int(it, "x2_endc_resource_status_response_sent");  
  //dst.x2_endc_resource_status_response_recv = dec_int(it, "x2_endc_resource_status_response_recv");  
  //dst.x2_endc_resource_status_failure_sent = dec_int(it, "x2_endc_resource_status_failure_sent");  
  //dst.x2_endc_resource_status_failure_recv = dec_int(it, "x2_endc_resource_status_failure_recv");  
  //dst.x2_f1c_traffic_transfer_sent = dec_int(it, "x2_f1c_traffic_transfer_sent");  
  //dst.x2_f1c_traffic_transfer_recv = dec_int(it, "x2_f1c_traffic_transfer_recv");  
  //dst.x2_ue_radio_capability_id_mapping_request_sent = dec_int(it, "x2_ue_radio_capability_id_mapping_request_sent");  
  //dst.x2_ue_radio_capability_id_mapping_request_recv = dec_int(it, "x2_ue_radio_capability_id_mapping_request_recv");  
  //dst.x2_ue_radio_capability_id_mapping_response_sent = dec_int(it, "x2_ue_radio_capability_id_mapping_response_sent");  
  //dst.x2_ue_radio_capability_id_mapping_response_recv = dec_int(it, "x2_ue_radio_capability_id_mapping_response_recv");  
  //dst.x2_access_and_mobility_indication_sent = dec_int(it, "x2_access_and_mobility_indication_sent");  
  //dst.x2_access_and_mobility_indication_recv = dec_int(it, "x2_access_and_mobility_indication_recv");  
  //dst.x2_cpc_cancel_sent = dec_int(it, "x2_cpc_cancel_sent");  
  //dst.x2_cpc_cancel_recv = dec_int(it, "x2_cpc_cancel_recv");  
  //dst.ng_amf_configuration_update = dec_int(it, "ng_amf_configuration_update");  
  //dst.ng_amf_configuration_update_acknowledge = dec_int(it, "ng_amf_configuration_update_acknowledge");  
  //dst.ng_amf_configuration_update_failure = dec_int(it, "ng_amf_configuration_update_failure");  
  //dst.ng_amf_status_indication = dec_int(it, "ng_amf_status_indication");  
  //dst.ng_cell_traffic_trace = dec_int(it, "ng_cell_traffic_trace");  
  //dst.ng_deactivate_trace = dec_int(it, "ng_deactivate_trace");  
  //dst.ng_downlink_nas_transport = dec_int(it, "ng_downlink_nas_transport");  
  //dst.ng_downlink_non_ue_associated_nrppa_transport = dec_int(it, "ng_downlink_non_ue_associated_nrppa_transport");  
  //dst.ng_downlink_ran_configuration_transfer = dec_int(it, "ng_downlink_ran_configuration_transfer");  
  //dst.ng_downlink_ran_status_transfer = dec_int(it, "ng_downlink_ran_status_transfer");  
  //dst.ng_downlink_ue_associated_nrppa_transport = dec_int(it, "ng_downlink_ue_associated_nrppa_transport");  
  //dst.ng_error_indication = dec_int(it, "ng_error_indication");  
  //dst.ng_handover_cancel = dec_int(it, "ng_handover_cancel");  
  //dst.ng_handover_cancel_acknowledge = dec_int(it, "ng_handover_cancel_acknowledge");  
  //dst.ng_handover_notify = dec_int(it, "ng_handover_notify");  
  //dst.ng_handover_required = dec_int(it, "ng_handover_required");  
  //dst.ng_handover_command = dec_int(it, "ng_handover_command");  
  //dst.ng_handover_preparation_failure = dec_int(it, "ng_handover_preparation_failure");  
  //dst.ng_handover_request = dec_int(it, "ng_handover_request");  
  //dst.ng_handover_request_acknowledge = dec_int(it, "ng_handover_request_acknowledge");  
  //dst.ng_handover_failure = dec_int(it, "ng_handover_failure");  
  //dst.ng_initial_context_setup_request = dec_int(it, "ng_initial_context_setup_request");  
  //dst.ng_initial_context_setup_response = dec_int(it, "ng_initial_context_setup_response");  
  //dst.ng_initial_context_setup_failure = dec_int(it, "ng_initial_context_setup_failure");  
  //dst.ng_initial_ue_message = dec_int(it, "ng_initial_ue_message");  
  //dst.ng_location_reporting_control = dec_int(it, "ng_location_reporting_control");  
  //dst.ng_location_reporting_failure_indication = dec_int(it, "ng_location_reporting_failure_indication");  
  //dst.ng_location_report = dec_int(it, "ng_location_report");  
  //dst.ng_nas_non_delivery_indication = dec_int(it, "ng_nas_non_delivery_indication");  
  //dst.ng_reset = dec_int(it, "ng_reset");  
  //dst.ng_reset_acknowledge = dec_int(it, "ng_reset_acknowledge");  
  //dst.ng_setup_request = dec_int(it, "ng_setup_request");  
  //dst.ng_setup_response = dec_int(it, "ng_setup_response");  
  //dst.ng_setup_failure = dec_int(it, "ng_setup_failure");  
  //dst.ng_overload_start = dec_int(it, "ng_overload_start");  
  //dst.ng_overload_stop = dec_int(it, "ng_overload_stop");  
  //dst.ng_paging = dec_int(it, "ng_paging");  
  //dst.ng_path_switch_request = dec_int(it, "ng_path_switch_request");  
  //dst.ng_path_switch_request_acknowledge = dec_int(it, "ng_path_switch_request_acknowledge");  
  //dst.ng_path_switch_request_failure = dec_int(it, "ng_path_switch_request_failure");  
  //dst.ng_pdu_session_resource_modify_request = dec_int(it, "ng_pdu_session_resource_modify_request");  
  //dst.ng_pdu_session_resource_modify_response = dec_int(it, "ng_pdu_session_resource_modify_response");  
  //dst.ng_pdu_session_resource_modify_indication = dec_int(it, "ng_pdu_session_resource_modify_indication");  
  //dst.ng_pdu_session_resource_modify_confirm = dec_int(it, "ng_pdu_session_resource_modify_confirm");  
  //dst.ng_pdu_session_resource_release_command = dec_int(it, "ng_pdu_session_resource_release_command");  
  //dst.ng_pdu_session_resource_release_response = dec_int(it, "ng_pdu_session_resource_release_response");  
  //dst.ng_pdu_session_resource_setup_request = dec_int(it, "ng_pdu_session_resource_setup_request");  
  //dst.ng_pdu_session_resource_setup_response = dec_int(it, "ng_pdu_session_resource_setup_response");  
  //dst.ng_pdu_session_resource_notify = dec_int(it, "ng_pdu_session_resource_notify");  
  //dst.ng_private_message = dec_int(it, "ng_private_message");  
  //dst.ng_pws_cancel_request = dec_int(it, "ng_pws_cancel_request");  
  //dst.ng_pws_cancel_response = dec_int(it, "ng_pws_cancel_response");  
  //dst.ng_pws_failure_indication = dec_int(it, "ng_pws_failure_indication");  
  //dst.ng_pws_restart_indication = dec_int(it, "ng_pws_restart_indication");  
  //dst.ng_ran_configuration_update = dec_int(it, "ng_ran_configuration_update");  
  //dst.ng_ran_configuration_update_acknowledge = dec_int(it, "ng_ran_configuration_update_acknowledge");  
  //dst.ng_ran_configuration_update_failure = dec_int(it, "ng_ran_configuration_update_failure");  
  //dst.ng_reroute_nas_request = dec_int(it, "ng_reroute_nas_request");  
  //dst.ng_rrc_inactive_transition_report = dec_int(it, "ng_rrc_inactive_transition_report");  
  //dst.ng_trace_failure_indication = dec_int(it, "ng_trace_failure_indication");  
  //dst.ng_trace_start = dec_int(it, "ng_trace_start");  
  //dst.ng_ue_context_modification_request = dec_int(it, "ng_ue_context_modification_request");  
  //dst.ng_ue_context_modification_response = dec_int(it, "ng_ue_context_modification_response");  
  //dst.ng_ue_context_modification_failure = dec_int(it, "ng_ue_context_modification_failure");  
  //dst.ng_ue_context_release_command = dec_int(it, "ng_ue_context_release_command");  
  //dst.ng_ue_context_release_complete = dec_int(it, "ng_ue_context_release_complete");  
  //dst.ng_ue_context_release_request = dec_int(it, "ng_ue_context_release_request");  
  //dst.ng_ue_radio_capability_check_request = dec_int(it, "ng_ue_radio_capability_check_request");  
  //dst.ng_ue_radio_capability_check_response = dec_int(it, "ng_ue_radio_capability_check_response");  
  //dst.ng_ue_radio_capability_info_indication = dec_int(it, "ng_ue_radio_capability_info_indication");  
  //dst.ng_ue_tnla_binding_release_request = dec_int(it, "ng_ue_tnla_binding_release_request");  
  //dst.ng_uplink_nas_transport = dec_int(it, "ng_uplink_nas_transport");  
  //dst.ng_uplink_non_ue_associated_nrppa_transport = dec_int(it, "ng_uplink_non_ue_associated_nrppa_transport");  
  //dst.ng_uplink_ran_configuration_transfer = dec_int(it, "ng_uplink_ran_configuration_transfer");  
  //dst.ng_uplink_ran_status_transfer = dec_int(it, "ng_uplink_ran_status_transfer");  
  //dst.ng_uplink_ue_associated_nrppa_transport = dec_int(it, "ng_uplink_ue_associated_nrppa_transport");  
  //dst.ng_write_replace_warning_request = dec_int(it, "ng_write_replace_warning_request");  
  //dst.ng_write_replace_warning_response = dec_int(it, "ng_write_replace_warning_response");  
  //dst.ng_secondary_rat_data_usage_report = dec_int(it, "ng_secondary_rat_data_usage_report");  
  //dst.ng_uplink_rim_information_transfer = dec_int(it, "ng_uplink_rim_information_transfer");  
  //dst.ng_downlink_rim_information_transfer = dec_int(it, "ng_downlink_rim_information_transfer");  
  //dst.ng_retrieve_ue_information = dec_int(it, "ng_retrieve_ue_information");  
  //dst.ng_ue_information_transfer = dec_int(it, "ng_ue_information_transfer");  
  //dst.ng_ran_cp_relocation_indication = dec_int(it, "ng_ran_cp_relocation_indication");  
  //dst.ng_ue_context_resume_request = dec_int(it, "ng_ue_context_resume_request");  
  //dst.ng_ue_context_resume_response = dec_int(it, "ng_ue_context_resume_response");  
  //dst.ng_ue_context_resume_failure = dec_int(it, "ng_ue_context_resume_failure");  
  //dst.ng_ue_context_suspend_request = dec_int(it, "ng_ue_context_suspend_request");  
  //dst.ng_ue_context_suspend_response = dec_int(it, "ng_ue_context_suspend_response");  
  //dst.ng_ue_context_suspend_failure = dec_int(it, "ng_ue_context_suspend_failure");  
  //dst.ng_ue_radio_capability_id_mapping_request = dec_int(it, "ng_ue_radio_capability_id_mapping_request");  
  //dst.ng_ue_radio_capability_id_mapping_response = dec_int(it, "ng_ue_radio_capability_id_mapping_response");  
  //dst.ng_handover_success = dec_int(it, "ng_handover_success");  
  //dst.ng_uplink_ran_early_status_transfer = dec_int(it, "ng_uplink_ran_early_status_transfer");  
  //dst.ng_downlink_ran_early_status_transfer = dec_int(it, "ng_downlink_ran_early_status_transfer");  
  //dst.ng_amf_cp_relocation_indication = dec_int(it, "ng_amf_cp_relocation_indication");  
  //dst.ng_connection_establishment_indication = dec_int(it, "ng_connection_establishment_indication");  
  //dst.ng_broadcast_session_modification_request = dec_int(it, "ng_broadcast_session_modification_request");  
  //dst.ng_broadcast_session_modification_response = dec_int(it, "ng_broadcast_session_modification_response");  
  //dst.ng_broadcast_session_modification_failure = dec_int(it, "ng_broadcast_session_modification_failure");  
  //dst.ng_broadcast_session_release_request = dec_int(it, "ng_broadcast_session_release_request");  
  //dst.ng_broadcast_session_release_response = dec_int(it, "ng_broadcast_session_release_response");  
  //dst.ng_broadcast_session_setup_request = dec_int(it, "ng_broadcast_session_setup_request");  
  //dst.ng_broadcast_session_setup_response = dec_int(it, "ng_broadcast_session_setup_response");  
  //dst.ng_broadcast_session_setup_failure = dec_int(it, "ng_broadcast_session_setup_failure");  
  //dst.ng_distribution_setup_request = dec_int(it, "ng_distribution_setup_request");  
  //dst.ng_distribution_setup_response = dec_int(it, "ng_distribution_setup_response");  
  //dst.ng_distribution_setup_failure = dec_int(it, "ng_distribution_setup_failure");  
  //dst.ng_distribution_release_request = dec_int(it, "ng_distribution_release_request");  
  //dst.ng_distribution_release_response = dec_int(it, "ng_distribution_release_response");  
  //dst.ng_multicast_session_activation_request = dec_int(it, "ng_multicast_session_activation_request");  
  //dst.ng_multicast_session_activation_response = dec_int(it, "ng_multicast_session_activation_response");  
  //dst.ng_multicast_session_activation_failure = dec_int(it, "ng_multicast_session_activation_failure");  
  //dst.ng_multicast_session_deactivation_request = dec_int(it, "ng_multicast_session_deactivation_request");  
  //dst.ng_multicast_session_deactivation_response = dec_int(it, "ng_multicast_session_deactivation_response");  
  //dst.ng_multicast_session_update_request = dec_int(it, "ng_multicast_session_update_request");  
  //dst.ng_multicast_session_update_response = dec_int(it, "ng_multicast_session_update_response");  
  //dst.ng_multicast_session_update_failure = dec_int(it, "ng_multicast_session_update_failure");  
  //dst.ng_multicast_group_paging = dec_int(it, "ng_multicast_group_paging");  
  //dst.ng_broadcast_session_release_required = dec_int(it, "ng_broadcast_session_release_required");  
  //dst.xn_handover_request_sent = dec_int(it, "xn_handover_request_sent");  
  //dst.xn_handover_request_recv = dec_int(it, "xn_handover_request_recv");  
  //dst.xn_handover_request_acknowledge_sent = dec_int(it, "xn_handover_request_acknowledge_sent");  
  //dst.xn_handover_request_acknowledge_recv = dec_int(it, "xn_handover_request_acknowledge_recv");  
  //dst.xn_handover_preparation_failure_sent = dec_int(it, "xn_handover_preparation_failure_sent");  
  //dst.xn_handover_preparation_failure_recv = dec_int(it, "xn_handover_preparation_failure_recv");  
  //dst.xn_sn_status_transfer_sent = dec_int(it, "xn_sn_status_transfer_sent");  
  //dst.xn_sn_status_transfer_recv = dec_int(it, "xn_sn_status_transfer_recv");  
  //dst.xn_handover_cancel_sent = dec_int(it, "xn_handover_cancel_sent");  
  //dst.xn_handover_cancel_recv = dec_int(it, "xn_handover_cancel_recv");  
  //dst.xn_retrieve_ue_context_sent = dec_int(it, "xn_retrieve_ue_context_sent");  
  //dst.xn_retrieve_ue_context_recv = dec_int(it, "xn_retrieve_ue_context_recv");  
  //dst.xn_retrieve_ue_context_response_sent = dec_int(it, "xn_retrieve_ue_context_response_sent");  
  //dst.xn_retrieve_ue_context_response_recv = dec_int(it, "xn_retrieve_ue_context_response_recv");  
  //dst.xn_retrieve_ue_context_failure_sent = dec_int(it, "xn_retrieve_ue_context_failure_sent");  
  //dst.xn_retrieve_ue_context_failure_recv = dec_int(it, "xn_retrieve_ue_context_failure_recv");  
  //dst.xn_ran_paging_sent = dec_int(it, "xn_ran_paging_sent");  
  //dst.xn_ran_paging_recv = dec_int(it, "xn_ran_paging_recv");  
  //dst.xn_u_address_indication_sent = dec_int(it, "xn_u_address_indication_sent");  
  //dst.xn_u_address_indication_recv = dec_int(it, "xn_u_address_indication_recv");  
  //dst.xn_ue_context_release_sent = dec_int(it, "xn_ue_context_release_sent");  
  //dst.xn_ue_context_release_recv = dec_int(it, "xn_ue_context_release_recv");  
  //dst.xn_s_node_addition_request_sent = dec_int(it, "xn_s_node_addition_request_sent");  
  //dst.xn_s_node_addition_request_recv = dec_int(it, "xn_s_node_addition_request_recv");  
  //dst.xn_s_node_addition_request_acknowledge_sent = dec_int(it, "xn_s_node_addition_request_acknowledge_sent");  
  //dst.xn_s_node_addition_request_acknowledge_recv = dec_int(it, "xn_s_node_addition_request_acknowledge_recv");  
  //dst.xn_s_node_addition_request_reject_sent = dec_int(it, "xn_s_node_addition_request_reject_sent");  
  //dst.xn_s_node_addition_request_reject_recv = dec_int(it, "xn_s_node_addition_request_reject_recv");  
  //dst.xn_s_node_reconfiguration_complete_sent = dec_int(it, "xn_s_node_reconfiguration_complete_sent");  
  //dst.xn_s_node_reconfiguration_complete_recv = dec_int(it, "xn_s_node_reconfiguration_complete_recv");  
  //dst.xn_s_node_modification_request_sent = dec_int(it, "xn_s_node_modification_request_sent");  
  //dst.xn_s_node_modification_request_recv = dec_int(it, "xn_s_node_modification_request_recv");  
  //dst.xn_s_node_modification_request_acknowledge_sent = dec_int(it, "xn_s_node_modification_request_acknowledge_sent");  
  //dst.xn_s_node_modification_request_acknowledge_recv = dec_int(it, "xn_s_node_modification_request_acknowledge_recv");  
  //dst.xn_s_node_modification_request_reject_sent = dec_int(it, "xn_s_node_modification_request_reject_sent");  
  //dst.xn_s_node_modification_request_reject_recv = dec_int(it, "xn_s_node_modification_request_reject_recv");  
  //dst.xn_s_node_modification_required_sent = dec_int(it, "xn_s_node_modification_required_sent");  
  //dst.xn_s_node_modification_required_recv = dec_int(it, "xn_s_node_modification_required_recv");  
  //dst.xn_s_node_modification_confirm_sent = dec_int(it, "xn_s_node_modification_confirm_sent");  
  //dst.xn_s_node_modification_confirm_recv = dec_int(it, "xn_s_node_modification_confirm_recv");  
  //dst.xn_s_node_modification_refuse_sent = dec_int(it, "xn_s_node_modification_refuse_sent");  
  //dst.xn_s_node_modification_refuse_recv = dec_int(it, "xn_s_node_modification_refuse_recv");  
  //dst.xn_s_node_release_request_sent = dec_int(it, "xn_s_node_release_request_sent");  
  //dst.xn_s_node_release_request_recv = dec_int(it, "xn_s_node_release_request_recv");  
  //dst.xn_s_node_release_request_acknowledge_sent = dec_int(it, "xn_s_node_release_request_acknowledge_sent");  
  //dst.xn_s_node_release_request_acknowledge_recv = dec_int(it, "xn_s_node_release_request_acknowledge_recv");  
  //dst.xn_s_node_release_reject_sent = dec_int(it, "xn_s_node_release_reject_sent");  
  //dst.xn_s_node_release_reject_recv = dec_int(it, "xn_s_node_release_reject_recv");  
  //dst.xn_s_node_release_required_sent = dec_int(it, "xn_s_node_release_required_sent");  
  //dst.xn_s_node_release_required_recv = dec_int(it, "xn_s_node_release_required_recv");  
  //dst.xn_s_node_release_confirm_sent = dec_int(it, "xn_s_node_release_confirm_sent");  
  //dst.xn_s_node_release_confirm_recv = dec_int(it, "xn_s_node_release_confirm_recv");  
  //dst.xn_s_node_counter_check_request_sent = dec_int(it, "xn_s_node_counter_check_request_sent");  
  //dst.xn_s_node_counter_check_request_recv = dec_int(it, "xn_s_node_counter_check_request_recv");  
  //dst.xn_s_node_change_required_sent = dec_int(it, "xn_s_node_change_required_sent");  
  //dst.xn_s_node_change_required_recv = dec_int(it, "xn_s_node_change_required_recv");  
  //dst.xn_s_node_change_confirm_sent = dec_int(it, "xn_s_node_change_confirm_sent");  
  //dst.xn_s_node_change_confirm_recv = dec_int(it, "xn_s_node_change_confirm_recv");  
  //dst.xn_s_node_change_refuse_sent = dec_int(it, "xn_s_node_change_refuse_sent");  
  //dst.xn_s_node_change_refuse_recv = dec_int(it, "xn_s_node_change_refuse_recv");  
  //dst.xn_rrc_transfer_sent = dec_int(it, "xn_rrc_transfer_sent");  
  //dst.xn_rrc_transfer_recv = dec_int(it, "xn_rrc_transfer_recv");  
  //dst.xn_removal_request_sent = dec_int(it, "xn_removal_request_sent");  
  //dst.xn_removal_request_recv = dec_int(it, "xn_removal_request_recv");  
  //dst.xn_removal_response_sent = dec_int(it, "xn_removal_response_sent");  
  //dst.xn_removal_response_recv = dec_int(it, "xn_removal_response_recv");  
  //dst.xn_removal_failure_sent = dec_int(it, "xn_removal_failure_sent");  
  //dst.xn_removal_failure_recv = dec_int(it, "xn_removal_failure_recv");  
  //dst.xn_setup_request_sent = dec_int(it, "xn_setup_request_sent");  
  //dst.xn_setup_request_recv = dec_int(it, "xn_setup_request_recv");  
  //dst.xn_setup_response_sent = dec_int(it, "xn_setup_response_sent");  
  //dst.xn_setup_response_recv = dec_int(it, "xn_setup_response_recv");  
  //dst.xn_setup_failure_sent = dec_int(it, "xn_setup_failure_sent");  
  //dst.xn_setup_failure_recv = dec_int(it, "xn_setup_failure_recv");  
  //dst.xn_ng_ran_node_configuration_update_sent = dec_int(it, "xn_ng_ran_node_configuration_update_sent");  
  //dst.xn_ng_ran_node_configuration_update_recv = dec_int(it, "xn_ng_ran_node_configuration_update_recv");  
  //dst.xn_ng_ran_node_configuration_update_acknowledge_sent = dec_int(it, "xn_ng_ran_node_configuration_update_acknowledge_sent");  
  //dst.xn_ng_ran_node_configuration_update_acknowledge_recv = dec_int(it, "xn_ng_ran_node_configuration_update_acknowledge_recv");  
  //dst.xn_ng_ran_node_configuration_update_failure_sent = dec_int(it, "xn_ng_ran_node_configuration_update_failure_sent");  
  //dst.xn_ng_ran_node_configuration_update_failure_recv = dec_int(it, "xn_ng_ran_node_configuration_update_failure_recv");  
  //dst.xn_cell_activation_request_sent = dec_int(it, "xn_cell_activation_request_sent");  
  //dst.xn_cell_activation_request_recv = dec_int(it, "xn_cell_activation_request_recv");  
  //dst.xn_cell_activation_response_sent = dec_int(it, "xn_cell_activation_response_sent");  
  //dst.xn_cell_activation_response_recv = dec_int(it, "xn_cell_activation_response_recv");  
  //dst.xn_cell_activation_failure_sent = dec_int(it, "xn_cell_activation_failure_sent");  
  //dst.xn_cell_activation_failure_recv = dec_int(it, "xn_cell_activation_failure_recv");  
  //dst.xn_reset_request_sent = dec_int(it, "xn_reset_request_sent");  
  //dst.xn_reset_request_recv = dec_int(it, "xn_reset_request_recv");  
  //dst.xn_reset_response_sent = dec_int(it, "xn_reset_response_sent");  
  //dst.xn_reset_response_recv = dec_int(it, "xn_reset_response_recv");  
  //dst.xn_error_indication_sent = dec_int(it, "xn_error_indication_sent");  
  //dst.xn_error_indication_recv = dec_int(it, "xn_error_indication_recv");  
  //dst.xn_private_message_sent = dec_int(it, "xn_private_message_sent");  
  //dst.xn_private_message_recv = dec_int(it, "xn_private_message_recv");  
  //dst.xn_notification_control_indication_sent = dec_int(it, "xn_notification_control_indication_sent");  
  //dst.xn_notification_control_indication_recv = dec_int(it, "xn_notification_control_indication_recv");  
  //dst.xn_activity_notification_sent = dec_int(it, "xn_activity_notification_sent");  
  //dst.xn_activity_notification_recv = dec_int(it, "xn_activity_notification_recv");  
  //dst.xn_eutra_nr_cell_resource_coordination_request_sent = dec_int(it, "xn_eutra_nr_cell_resource_coordination_request_sent");  
  //dst.xn_eutra_nr_cell_resource_coordination_request_recv = dec_int(it, "xn_eutra_nr_cell_resource_coordination_request_recv");  
  //dst.xn_eutra_nr_cell_resource_coordination_response_sent = dec_int(it, "xn_eutra_nr_cell_resource_coordination_response_sent");  
  //dst.xn_eutra_nr_cell_resource_coordination_response_recv = dec_int(it, "xn_eutra_nr_cell_resource_coordination_response_recv");  
  //dst.xn_secondary_rat_data_usage_report_sent = dec_int(it, "xn_secondary_rat_data_usage_report_sent");  
  //dst.xn_secondary_rat_data_usage_report_recv = dec_int(it, "xn_secondary_rat_data_usage_report_recv");  
  //dst.xn_deactivate_trace_sent = dec_int(it, "xn_deactivate_trace_sent");  
  //dst.xn_deactivate_trace_recv = dec_int(it, "xn_deactivate_trace_recv");  
  //dst.xn_trace_start_sent = dec_int(it, "xn_trace_start_sent");  
  //dst.xn_trace_start_recv = dec_int(it, "xn_trace_start_recv");  
  //dst.xn_handover_success_sent = dec_int(it, "xn_handover_success_sent");  
  //dst.xn_handover_success_recv = dec_int(it, "xn_handover_success_recv");  
  //dst.xn_conditional_handover_cancel_sent = dec_int(it, "xn_conditional_handover_cancel_sent");  
  //dst.xn_conditional_handover_cancel_recv = dec_int(it, "xn_conditional_handover_cancel_recv");  
  //dst.xn_early_status_transfer_sent = dec_int(it, "xn_early_status_transfer_sent");  
  //dst.xn_early_status_transfer_recv = dec_int(it, "xn_early_status_transfer_recv");  
  //dst.xn_failure_indication_sent = dec_int(it, "xn_failure_indication_sent");  
  //dst.xn_failure_indication_recv = dec_int(it, "xn_failure_indication_recv");  
  //dst.xn_handover_report_sent = dec_int(it, "xn_handover_report_sent");  
  //dst.xn_handover_report_recv = dec_int(it, "xn_handover_report_recv");  
  //dst.xn_resource_status_request_sent = dec_int(it, "xn_resource_status_request_sent");  
  //dst.xn_resource_status_request_recv = dec_int(it, "xn_resource_status_request_recv");  
  //dst.xn_resource_status_response_sent = dec_int(it, "xn_resource_status_response_sent");  
  //dst.xn_resource_status_response_recv = dec_int(it, "xn_resource_status_response_recv");  
  //dst.xn_resource_status_failure_sent = dec_int(it, "xn_resource_status_failure_sent");  
  //dst.xn_resource_status_failure_recv = dec_int(it, "xn_resource_status_failure_recv");  
  //dst.xn_resource_status_update_sent = dec_int(it, "xn_resource_status_update_sent");  
  //dst.xn_resource_status_update_recv = dec_int(it, "xn_resource_status_update_recv");  
  //dst.xn_mobility_change_request_sent = dec_int(it, "xn_mobility_change_request_sent");  
  //dst.xn_mobility_change_request_recv = dec_int(it, "xn_mobility_change_request_recv");  
  //dst.xn_mobility_change_acknowledge_sent = dec_int(it, "xn_mobility_change_acknowledge_sent");  
  //dst.xn_mobility_change_acknowledge_recv = dec_int(it, "xn_mobility_change_acknowledge_recv");  
  //dst.xn_mobility_change_failure_sent = dec_int(it, "xn_mobility_change_failure_sent");  
  //dst.xn_mobility_change_failure_recv = dec_int(it, "xn_mobility_change_failure_recv");  
  //dst.xn_access_and_mobility_indication_sent = dec_int(it, "xn_access_and_mobility_indication_sent");  
  //dst.xn_access_and_mobility_indication_recv = dec_int(it, "xn_access_and_mobility_indication_recv");  
  //dst.xn_cell_traffic_class_sent = dec_int(it, "xn_cell_traffic_class_sent");  
  //dst.xn_cell_traffic_class_recv = dec_int(it, "xn_cell_traffic_class_recv");  
  //dst.xn_ran_multicast_group_paging_sent = dec_int(it, "xn_ran_multicast_group_paging_sent");  
  //dst.xn_ran_multicast_group_paging_recv = dec_int(it, "xn_ran_multicast_group_paging_recv");  
  //dst.xn_scg_failure_information_report_sent = dec_int(it, "xn_scg_failure_information_report_sent");  
  //dst.xn_scg_failure_information_report_recv = dec_int(it, "xn_scg_failure_information_report_recv");  
  //dst.xn_scg_failure_transfer_sent = dec_int(it, "xn_scg_failure_transfer_sent");  
  //dst.xn_scg_failure_transfer_recv = dec_int(it, "xn_scg_failure_transfer_recv");  
  //dst.xn_f1c_traffic_transfer_sent = dec_int(it, "xn_f1c_traffic_transfer_sent");  
  //dst.xn_f1c_traffic_transfer_recv = dec_int(it, "xn_f1c_traffic_transfer_recv");  
  //dst.xn_iab_transport_migration_management_request_sent = dec_int(it, "xn_iab_transport_migration_management_request_sent");  
  //dst.xn_iab_transport_migration_management_request_recv = dec_int(it, "xn_iab_transport_migration_management_request_recv");  
  //dst.xn_iab_transport_migration_management_response_sent = dec_int(it, "xn_iab_transport_migration_management_response_sent");  
  //dst.xn_iab_transport_migration_management_response_recv = dec_int(it, "xn_iab_transport_migration_management_response_recv");  
  //dst.xn_iab_transport_migration_management_reject_sent = dec_int(it, "xn_iab_transport_migration_management_reject_sent");  
  //dst.xn_iab_transport_migration_management_reject_recv = dec_int(it, "xn_iab_transport_migration_management_reject_recv");  
  //dst.xn_iab_transport_migration_modification_request_sent = dec_int(it, "xn_iab_transport_migration_modification_request_sent");  
  //dst.xn_iab_transport_migration_modification_request_recv = dec_int(it, "xn_iab_transport_migration_modification_request_recv");  
  //dst.xn_iab_transport_migration_modification_response_sent = dec_int(it, "xn_iab_transport_migration_modification_response_sent");  
  //dst.xn_iab_transport_migration_modification_response_recv = dec_int(it, "xn_iab_transport_migration_modification_response_recv");  
  //dst.xn_iab_resource_coordination_request_sent = dec_int(it, "xn_iab_resource_coordination_request_sent");  
  //dst.xn_iab_resource_coordination_request_recv = dec_int(it, "xn_iab_resource_coordination_request_recv");  
  //dst.xn_iab_resource_coordination_response_sent = dec_int(it, "xn_iab_resource_coordination_response_sent");  
  //dst.xn_iab_resource_coordination_response_recv = dec_int(it, "xn_iab_resource_coordination_response_recv");  
  //dst.xn_retrieve_ue_context_confirm_sent = dec_int(it, "xn_retrieve_ue_context_confirm_sent");  
  //dst.xn_retrieve_ue_context_confirm_recv = dec_int(it, "xn_retrieve_ue_context_confirm_recv");  
  //dst.xn_cpc_cancel_sent = dec_int(it, "xn_cpc_cancel_sent");  
  //dst.xn_cpc_cancel_recv = dec_int(it, "xn_cpc_cancel_recv");  
  //dst.xn_partial_ue_context_transfer_sent = dec_int(it, "xn_partial_ue_context_transfer_sent");  
  //dst.xn_partial_ue_context_transfer_recv = dec_int(it, "xn_partial_ue_context_transfer_recv");  
  //dst.xn_partial_ue_context_transfer_acknowledge_sent = dec_int(it, "xn_partial_ue_context_transfer_acknowledge_sent");  
  //dst.xn_partial_ue_context_transfer_acknowledge_recv = dec_int(it, "xn_partial_ue_context_transfer_acknowledge_recv");  
  //dst.xn_partial_ue_context_transfer_failure_sent = dec_int(it, "xn_partial_ue_context_transfer_failure_sent");  
  //dst.xn_partial_ue_context_transfer_failure_recv = dec_int(it, "xn_partial_ue_context_transfer_failure_recv");  
  //dst.m2_session_start_request = dec_int(it, "m2_session_start_request");  
  //dst.m2_session_start_response = dec_int(it, "m2_session_start_response");  
  //dst.m2_session_start_failure = dec_int(it, "m2_session_start_failure");  
  //dst.m2_session_stop_request = dec_int(it, "m2_session_stop_request");  
  //dst.m2_session_stop_response = dec_int(it, "m2_session_stop_response");  
  //dst.m2_mbms_scheduling_information = dec_int(it, "m2_mbms_scheduling_information");  
  //dst.m2_mbms_scheduling_information_response = dec_int(it, "m2_mbms_scheduling_information_response");  
  //dst.m2_error_indication = dec_int(it, "m2_error_indication");  
  //dst.m2_reset = dec_int(it, "m2_reset");  
  //dst.m2_reset_acknowledge = dec_int(it, "m2_reset_acknowledge");  
  //dst.m2_m2_setup_request = dec_int(it, "m2_m2_setup_request");  
  //dst.m2_m2_setup_response = dec_int(it, "m2_m2_setup_response");  
  //dst.m2_m2_setup_failure = dec_int(it, "m2_m2_setup_failure");  
  //dst.m2_enb_configuration_update = dec_int(it, "m2_enb_configuration_update");  
  //dst.m2_enb_configuration_update_acknowledge = dec_int(it, "m2_enb_configuration_update_acknowledge");  
  //dst.m2_enb_configuration_update_failure = dec_int(it, "m2_enb_configuration_update_failure");  
  //dst.m2_mce_configuration_update = dec_int(it, "m2_mce_configuration_update");  
  //dst.m2_mce_configuration_update_acknowledge = dec_int(it, "m2_mce_configuration_update_acknowledge");  
  //dst.m2_mce_configuration_update_failure = dec_int(it, "m2_mce_configuration_update_failure");  
  //dst.m2_private_message = dec_int(it, "m2_private_message");  
  //dst.m2_session_update_request = dec_int(it, "m2_session_update_request");  
  //dst.m2_session_update_response = dec_int(it, "m2_session_update_response");  
  //dst.m2_session_update_failure = dec_int(it, "m2_session_update_failure");  
  //dst.m2_mbms_service_counting_request = dec_int(it, "m2_mbms_service_counting_request");  
  //dst.m2_mbms_service_counting_response = dec_int(it, "m2_mbms_service_counting_response");  
  //dst.m2_mbms_service_counting_failure = dec_int(it, "m2_mbms_service_counting_failure");  
  //dst.m2_mbms_service_counting_results_report = dec_int(it, "m2_mbms_service_counting_results_report");  
  //dst.m2_mbms_overload_notification = dec_int(it, "m2_mbms_overload_notification");  
  //dst.lppa_asn1_decode_error = dec_int(it, "lppa_asn1_decode_error");  
  //dst.lppa_error_indication = dec_int(it, "lppa_error_indication");  
  //dst.lppa_private_message = dec_int(it, "lppa_private_message");  
  //dst.lppa_ecid_measurement_initiation_request = dec_int(it, "lppa_ecid_measurement_initiation_request");  
  //dst.lppa_ecid_measurement_failure_indication = dec_int(it, "lppa_ecid_measurement_failure_indication");  
  //dst.lppa_ecid_measurement_report = dec_int(it, "lppa_ecid_measurement_report");  
  //dst.lppa_ecid_measurement_termination = dec_int(it, "lppa_ecid_measurement_termination");  
  //dst.lppa_otdoa_information_request = dec_int(it, "lppa_otdoa_information_request");  
  //dst.lppa_utdoa_information_request = dec_int(it, "lppa_utdoa_information_request");  
  //dst.lppa_utdoa_information_update = dec_int(it, "lppa_utdoa_information_update");  
  //dst.lppa_assistance_information_control = dec_int(it, "lppa_assistance_information_control");  
  //dst.lppa_assistance_information_feedback = dec_int(it, "lppa_assistance_information_feedback");  
  //dst.lppa_ecid_measurement_initiation_response = dec_int(it, "lppa_ecid_measurement_initiation_response");  
  //dst.lppa_otdoa_information_response = dec_int(it, "lppa_otdoa_information_response");  
  //dst.lppa_utdoa_information_response = dec_int(it, "lppa_utdoa_information_response");  
  //dst.lppa_ecid_measurement_initiation_failure = dec_int(it, "lppa_ecid_measurement_initiation_failure");  
  //dst.lppa_otdoa_information_failure = dec_int(it, "lppa_otdoa_information_failure");  
  //dst.lppa_utdoa_information_failure = dec_int(it, "lppa_utdoa_information_failure");  
  //dst.nrppa_asn1_decode_error = dec_int(it, "nrppa_asn1_decode_error");  
  //dst.nrppa_ecid_measurement_initiation_request = dec_int(it, "nrppa_ecid_measurement_initiation_request");  
  //dst.nrppa_ecid_measurement_failure_indication = dec_int(it, "nrppa_ecid_measurement_failure_indication");  
  //dst.nrppa_ecid_measurement_report = dec_int(it, "nrppa_ecid_measurement_report");  
  //dst.nrppa_ecid_measurement_termination = dec_int(it, "nrppa_ecid_measurement_termination");  
  //dst.nrppa_otdoa_information_exchange = dec_int(it, "nrppa_otdoa_information_exchange");  
  //dst.nrppa_assistance_information_control = dec_int(it, "nrppa_assistance_information_control");  
  //dst.nrppa_assistance_information_feedback = dec_int(it, "nrppa_assistance_information_feedback");  
  //dst.nrppa_error_indication = dec_int(it, "nrppa_error_indication");  
  //dst.nrppa_private_message = dec_int(it, "nrppa_private_message");  
  //dst.nrppa_positioning_information_exchange = dec_int(it, "nrppa_positioning_information_exchange");  
  //dst.nrppa_positioning_information_update = dec_int(it, "nrppa_positioning_information_update");  
  //dst.nrppa_measurement_request = dec_int(it, "nrppa_measurement_request");  
  //dst.nrppa_measurement_report = dec_int(it, "nrppa_measurement_report");  
  //dst.nrppa_measurement_update = dec_int(it, "nrppa_measurement_update");  
  //dst.nrppa_measurement_abort = dec_int(it, "nrppa_measurement_abort");  
  //dst.nrppa_measurement_failure_indication = dec_int(it, "nrppa_measurement_failure_indication");  
  //dst.nrppa_trp_information_exchange = dec_int(it, "nrppa_trp_information_exchange");  
  //dst.nrppa_positioning_activation_request = dec_int(it, "nrppa_positioning_activation_request");  
  //dst.nrppa_positioning_deactivation_request = dec_int(it, "nrppa_positioning_deactivation_request");  
  //dst.nrppa_ecid_measurement_initiation_response = dec_int(it, "nrppa_ecid_measurement_initiation_response");  
  //dst.nrppa_otdoa_information_response = dec_int(it, "nrppa_otdoa_information_response");  
  //dst.nrppa_positioning_information_response = dec_int(it, "nrppa_positioning_information_response");  
  //dst.nrppa_measurement_response = dec_int(it, "nrppa_measurement_response");  
  //dst.nrppa_trp_information_response = dec_int(it, "nrppa_trp_information_response");  
  //dst.nrppa_positioning_activation_response = dec_int(it, "nrppa_positioning_activation_response");  
  //dst.nrppa_ecid_measurement_failure = dec_int(it, "nrppa_ecid_measurement_failure");  
  //dst.nrppa_otdoa_information_failure = dec_int(it, "nrppa_otdoa_information_failure");  
  //dst.nrppa_positioning_information_failure = dec_int(it, "nrppa_positioning_information_failure");  
  //dst.nrppa_measurement_failure = dec_int(it, "nrppa_measurement_failure");  
  //dst.nrppa_trp_information_failure = dec_int(it, "nrppa_trp_information_failure");  
  //dst.nrppa_positioning_activation_failur = dec_int(it, "nrppa_positioning_activation_failur");ee
  //dst.
  ////// # Cell 1 events : 
  ////// Message events : 
  //dst.prach = dec_int(it, "prach");  
  //dst.handover_command = dec_int(it, "handover_command");  
  //dst.handover_preparation = dec_int(it, "handover_preparation");  
  //dst.handover_source_success = dec_int(it, "handover_source_success");  
  //dst.handover_target_success = dec_int(it, "handover_target_success");  
  //dst.nr_scg_reconf_with_sync_success = dec_int(it, "nr_scg_reconf_with_sync_success");  
  //dst.rrc_connection_request = dec_int(it, "rrc_connection_request");  
  //dst.rrc_connection_reject = dec_int(it, "rrc_connection_reject");  
  //dst.rrc_connection_setup = dec_int(it, "rrc_connection_setup");  
  //dst.rrc_connection_setup_complete = dec_int(it, "rrc_connection_setup_complete");  
  //dst.rrc_ul_information_transfer = dec_int(it, "rrc_ul_information_transfer");  
  //dst.rrc_dl_information_transfer = dec_int(it, "rrc_dl_information_transfer");  
  //dst.rrc_security_mode_command = dec_int(it, "rrc_security_mode_command");  
  //dst.rrc_security_mode_complete = dec_int(it, "rrc_security_mode_complete");  
  //dst.rrc_security_mode_failure = dec_int(it, "rrc_security_mode_failure");  
  //dst.rrc_ue_capability_enquiry = dec_int(it, "rrc_ue_capability_enquiry");  
  //dst.rrc_ue_capability_information = dec_int(it, "rrc_ue_capability_information");  
  //dst.rrc_connection_reconfiguration = dec_int(it, "rrc_connection_reconfiguration");  
  //dst.rrc_connection_reconfiguration_complete = dec_int(it, "rrc_connection_reconfiguration_complete");  
  //dst.rrc_connection_reestablishment_request = dec_int(it, "rrc_connection_reestablishment_request");  
  //dst.rrc_connection_reestablishment_reject = dec_int(it, "rrc_connection_reestablishment_reject");  
  //dst.rrc_connection_reestablishment = dec_int(it, "rrc_connection_reestablishment");  
  //dst.rrc_connection_reestablishment_complete = dec_int(it, "rrc_connection_reestablishment_complete");  
  //dst.rrc_measurement_report = dec_int(it, "rrc_measurement_report");  
  //dst.rrc_connection_release = dec_int(it, "rrc_connection_release");  
  //dst.rrc_ue_information_request = dec_int(it, "rrc_ue_information_request");  
  //dst.rrc_ue_information_response = dec_int(it, "rrc_ue_information_response");  
  //dst.rrc_connection_resume_request = dec_int(it, "rrc_connection_resume_request");  
  //dst.rrc_csfb_parameters_response_cdma2000 = dec_int(it, "rrc_csfb_parameters_response_cdma2000");  
  //dst.rrc_handover_from_eutra_preparation_request = dec_int(it, "rrc_handover_from_eutra_preparation_request");  
  //dst.rrc_mobility_from_eutra_command = dec_int(it, "rrc_mobility_from_eutra_command");  
  //dst.rrc_counter_check = dec_int(it, "rrc_counter_check");  
  //dst.rrc_logged_measurement_configuration = dec_int(it, "rrc_logged_measurement_configuration");  
  //dst.rrc_rn_reconfiguration = dec_int(it, "rrc_rn_reconfiguration");  
  //dst.rrc_connection_resume = dec_int(it, "rrc_connection_resume");  
  //dst.rrc_dl_dedicated_message_segment = dec_int(it, "rrc_dl_dedicated_message_segment");  
  //dst.rrc_csfb_parameters_request_cdma2000 = dec_int(it, "rrc_csfb_parameters_request_cdma2000");  
  //dst.rrc_ul_handover_preparation_transfer = dec_int(it, "rrc_ul_handover_preparation_transfer");  
  //dst.rrc_counter_check_response = dec_int(it, "rrc_counter_check_response");  
  //dst.rrc_proximity_indication = dec_int(it, "rrc_proximity_indication");  
  //dst.rrc_rn_reconfiguration_complete = dec_int(it, "rrc_rn_reconfiguration_complete");  
  //dst.rrc_mbms_counting_response = dec_int(it, "rrc_mbms_counting_response");  
  //dst.rrc_inter_freq_rstd_measurement_indication = dec_int(it, "rrc_inter_freq_rstd_measurement_indication");  
  //dst.rrc_ue_assistance_information = dec_int(it, "rrc_ue_assistance_information");  
  //dst.rrc_in_device_coex_indication = dec_int(it, "rrc_in_device_coex_indication");  
  //dst.rrc_mbms_interest_indication = dec_int(it, "rrc_mbms_interest_indication");  
  //dst.rrc_scg_failure_information = dec_int(it, "rrc_scg_failure_information");  
  //dst.rrc_sidelink_ue_information = dec_int(it, "rrc_sidelink_ue_information");  
  //dst.rrc_wlan_connection_status_report = dec_int(it, "rrc_wlan_connection_status_report");  
  //dst.rrc_connection_resume_complete = dec_int(it, "rrc_connection_resume_complete");  
  //dst.rrc_ul_information_transfer_mrdc = dec_int(it, "rrc_ul_information_transfer_mrdc");  
  //dst.rrc_scg_failure_information_nr = dec_int(it, "rrc_scg_failure_information_nr");  
  //dst.rrc_measurement_report_application_layer = dec_int(it, "rrc_measurement_report_application_layer");  
  //dst.rrc_failure_information = dec_int(it, "rrc_failure_information");  
  //dst.rrc_ul_dedicated_message_segment = dec_int(it, "rrc_ul_dedicated_message_segment");  
  //dst.rrc_pur_configuration_request = dec_int(it, "rrc_pur_configuration_request");  
  //dst.rrc_failure_information_r16 = dec_int(it, "rrc_failure_information_r16");  
  //dst.rrc_mcg_failure_information = dec_int(it, "rrc_mcg_failure_information");  
  //dst.rrc_ul_information_transfer_irat = dec_int(it, "rrc_ul_information_transfer_irat");  
  //dst.rrc_early_data_request = dec_int(it, "rrc_early_data_request");  
  //dst.rrc_early_data_complete = dec_int(it, "rrc_early_data_complete");  
  //dst.rrc_paging = dec_int(it, "rrc_paging");  
  //dst.rrc_connection_reestablishment_nb = dec_int(it, "rrc_connection_reestablishment_nb");  
  //dst.rrc_connection_reestablishment_reject_nb = dec_int(it, "rrc_connection_reestablishment_reject_nb");  
  //dst.rrc_connection_reject_nb = dec_int(it, "rrc_connection_reject_nb");  
  //dst.rrc_connection_setup_nb = dec_int(it, "rrc_connection_setup_nb");  
  //dst.rrc_dl_information_transfer_nb = dec_int(it, "rrc_dl_information_transfer_nb");  
  //dst.rrc_connection_reconfiguration_nb = dec_int(it, "rrc_connection_reconfiguration_nb");  
  //dst.rrc_connection_release_nb = dec_int(it, "rrc_connection_release_nb");  
  //dst.rrc_security_mode_command_nb = dec_int(it, "rrc_security_mode_command_nb");  
  //dst.rrc_ue_capability_enquiry_nb = dec_int(it, "rrc_ue_capability_enquiry_nb");  
  //dst.rrc_connection_resume_nb = dec_int(it, "rrc_connection_resume_nb");  
  //dst.rrc_ue_information_request_nb = dec_int(it, "rrc_ue_information_request_nb");  
  //dst.rrc_connection_reestablishment_request_nb = dec_int(it, "rrc_connection_reestablishment_request_nb");  
  //dst.rrc_connection_request_nb = dec_int(it, "rrc_connection_request_nb");  
  //dst.rrc_connection_resume_request_nb = dec_int(it, "rrc_connection_resume_request_nb");  
  //dst.rrc_connection_reconfiguration_complete_nb = dec_int(it, "rrc_connection_reconfiguration_complete_nb");  
  //dst.rrc_connection_reestablishment_complete_nb = dec_int(it, "rrc_connection_reestablishment_complete_nb");  
  //dst.rrc_connection_setup_complete_nb = dec_int(it, "rrc_connection_setup_complete_nb");  
  //dst.rrc_security_mode_complete_nb = dec_int(it, "rrc_security_mode_complete_nb");  
  //dst.rrc_security_mode_failure_nb = dec_int(it, "rrc_security_mode_failure_nb");  
  //dst.rrc_ue_capability_information_nb = dec_int(it, "rrc_ue_capability_information_nb");  
  //dst.rrc_ul_information_transfer_nb = dec_int(it, "rrc_ul_information_transfer_nb");  
  //dst.rrc_connection_resume_complete_nb = dec_int(it, "rrc_connection_resume_complete_nb");  
  //dst.rrc_ue_information_response_nb = dec_int(it, "rrc_ue_information_response_nb");  
  //dst.rrc_pur_configuration_request_nb = dec_int(it, "rrc_pur_configuration_request_nb");  
  //dst.rrc_early_data_request_nb = dec_int(it, "rrc_early_data_request_nb");  
  //dst.rrc_early_data_complete_nb = dec_int(it, "rrc_early_data_complete_nb");  
  //dst.rrc_paging_nb = dec_int(it, "rrc_paging_nb");  
  //dst.nr_rrc_reject = dec_int(it, "nr_rrc_reject");  
  //dst.nr_rrc_setup = dec_int(it, "nr_rrc_setup");  
  //dst.nr_rrc_setup_request = dec_int(it, "nr_rrc_setup_request");  
  //dst.nr_rrc_resume_request = dec_int(it, "nr_rrc_resume_request");  
  //dst.nr_rrc_reestablishment_request = dec_int(it, "nr_rrc_reestablishment_request");  
  //dst.nr_rrc_system_info_request = dec_int(it, "nr_rrc_system_info_request");  
  //dst.nr_rrc_resume_request_1 = dec_int(it, "nr_rrc_resume_request_1");  
  //dst.nr_rrc_reconfiguration = dec_int(it, "nr_rrc_reconfiguration");  
  //dst.nr_rrc_resume = dec_int(it, "nr_rrc_resume");  
  //dst.nr_rrc_release = dec_int(it, "nr_rrc_release");  
  //dst.nr_rrc_reestablishment = dec_int(it, "nr_rrc_reestablishment");  
  //dst.nr_rrc_security_mode_command = dec_int(it, "nr_rrc_security_mode_command");  
  //dst.nr_rrc_dl_information_transfer = dec_int(it, "nr_rrc_dl_information_transfer");  
  //dst.nr_rrc_ue_capability_enquiry = dec_int(it, "nr_rrc_ue_capability_enquiry");  
  //dst.nr_rrc_counter_check = dec_int(it, "nr_rrc_counter_check");  
  //dst.nr_rrc_mobility_from_nr_command = dec_int(it, "nr_rrc_mobility_from_nr_command");  
  //dst.nr_rrc_dl_dedicated_message_segment = dec_int(it, "nr_rrc_dl_dedicated_message_segment");  
  //dst.nr_rrc_ue_information_request = dec_int(it, "nr_rrc_ue_information_request");  
  //dst.nr_rrc_dl_information_transfer_mrdc = dec_int(it, "nr_rrc_dl_information_transfer_mrdc");  
  //dst.nr_rrc_logged_measurement_configuration = dec_int(it, "nr_rrc_logged_measurement_configuration");  
  //dst.nr_rrc_measurement_report = dec_int(it, "nr_rrc_measurement_report");  
  //dst.nr_rrc_reconfiguration_complete = dec_int(it, "nr_rrc_reconfiguration_complete");  
  //dst.nr_rrc_setup_complete = dec_int(it, "nr_rrc_setup_complete");  
  //dst.nr_rrc_reestablishment_complete = dec_int(it, "nr_rrc_reestablishment_complete");  
  //dst.nr_rrc_resume_complete = dec_int(it, "nr_rrc_resume_complete");  
  //dst.nr_rrc_security_mode_complete = dec_int(it, "nr_rrc_security_mode_complete");  
  //dst.nr_rrc_security_mode_failure = dec_int(it, "nr_rrc_security_mode_failure");  
  //dst.nr_rrc_ul_information_transfer = dec_int(it, "nr_rrc_ul_information_transfer");  
  //dst.nr_rrc_location_measurement_indication = dec_int(it, "nr_rrc_location_measurement_indication");  
  //dst.nr_rrc_ue_capability_information = dec_int(it, "nr_rrc_ue_capability_information");  
  //dst.nr_rrc_counter_check_response = dec_int(it, "nr_rrc_counter_check_response");  
  //dst.nr_rrc_ue_assistance_information = dec_int(it, "nr_rrc_ue_assistance_information");  
  //dst.nr_rrc_failure_information = dec_int(it, "nr_rrc_failure_information");  
  //dst.nr_rrc_ul_information_transfer_mrdc = dec_int(it, "nr_rrc_ul_information_transfer_mrdc");  
  //dst.nr_rrc_scg_failure_information = dec_int(it, "nr_rrc_scg_failure_information");  
  //dst.nr_rrc_scg_failure_information_eutra = dec_int(it, "nr_rrc_scg_failure_information_eutra");  
  //dst.nr_rrc_ul_dedicated_message_segment = dec_int(it, "nr_rrc_ul_dedicated_message_segment");  
  //dst.nr_rrc_dedicated_sib_request = dec_int(it, "nr_rrc_dedicated_sib_request");  
  //dst.nr_rrc_mcg_failure_information = dec_int(it, "nr_rrc_mcg_failure_information");  
  //dst.nr_rrc_ue_information_response = dec_int(it, "nr_rrc_ue_information_response");  
  //dst.nr_rrc_sidelink_ue_information_nr = dec_int(it, "nr_rrc_sidelink_ue_information_nr");  
  //dst.nr_rrc_ul_information_transfer_irat = dec_int(it, "nr_rrc_ul_information_transfer_irat");  
  //dst.nr_rrc_iab_other_information = dec_int(it, "nr_rrc_iab_other_information");  
  //dst.nr_rrc_mbs_interest_indication = dec_int(it, "nr_rrc_mbs_interest_indication");  
  //dst.nr_rrc_ue_positioning_assistance_info = dec_int(it, "nr_rrc_ue_positioning_assistance_info");  
  //dst.nr_rrc_measurement_report_app_layer = dec_int(it, "nr_rrc_measurement_report_app_layer");  
  //dst.nr_rrc_pagin = dec_int(it, "nr_rrc_pagin");

  return dst;
}

cpu_amr_t dec_cpu_amr(void* it)
{
  assert(it != NULL);

  cpu_amr_t dst = {0}; 

  // Mandatory. Array of float. Noise level table, in dB
  ans_cjson_t ans = find_object(it, "cpu");

  dst.global = dec_int_opt(ans.it, "global");

  return dst;
}

global_gnb_id_amr_t dec_global_gnb_id_amr(void* it_void)
{
  assert(it_void != NULL);
  global_gnb_id_amr_t dst = {0};


  cJSON const* it = it_void;

  // Mandatory. PLMN
  dst.plmn = dec_string_opt(it, "plmn");

  // Mandatory. gnb_id_bits
  dst.gnb_id_bits = dec_int_opt(it, "gnb_id_bits");

  // Mandatory. gnb_id
  dst.gnb_id = dec_int_opt(it, "gnb_id");

  // Mandatory. gnb_name
  dst.gnb_name = dec_string_opt(it, "gnb_name");

  return dst;
}

layers_amr_t dec_layers_amr(void* it_void)
{
  assert(it_void != NULL);
  cJSON* it = (cJSON*)it_void;

  layers_amr_t dst = {0};
  // Mandatory. Name
  dst.layer_name = strdup(it->string);

  // Mandatory. Level
  dst.level = dec_string_opt(it, "level");

  // Mandatory. Max_size 
  dst.max_size = dec_int_opt(it, "max_size");

  // Mandatory. key
  dst.key = dec_bool_opt(it, "key");
  
  // Mandatory. crypto
  dst.crypto = dec_bool_opt(it, "crypto");
 
  // Mandatory. Payload
  dst.payload = dec_bool_opt(it, "payload");

  // There may be more fields... 

  return dst;
}

logs_amr_t dec_logs_amr(void* it_void)
{
  assert(it_void != NULL);

  cJSON* it = (cJSON*)it_void;
  
  logs_amr_t dst = {0};

  ans_cjson_t ans_logs = find_object(it, "logs");

  ans_cjson_t ans_layers = find_object(ans_logs.it, "layers");

  dst.sz_layers = cJSON_GetArraySize(ans_layers.it);
 
  if(dst.sz_layers > 0){
    dst.layers = calloc(dst.sz_layers, sizeof(layers_amr_t));
  }

  cJSON* tmp_it = ((cJSON*)ans_layers.it)->child; 
  for(size_t i = 0; i < dst.sz_layers; ++i){
    assert(tmp_it != NULL);
        
    dst.layers[i] = dec_layers_amr(tmp_it);
    tmp_it = tmp_it->next;
  }

  // Mandatory. Count
  dst.count = dec_int_opt(ans_logs.it, "count");

  // Optional. Rotate
  // int* rotate;

  // Optional. Path
  // char* path;

  // Mandatory bcch
  // True if BCCH dump is enabled (eNB only)
  // BCCH 
  dst.bcch = dec_bool_opt(ans_logs.it,"bcch");

  // Mandatory mib
  // True if MIB dump is enabled (eNB only).
  dst.mib = dec_bool_opt(ans_logs.it,"mib");

  return dst;
}

ncell_list_amr_t dec_ncell_list_amr(void* it )
{
  assert(it != NULL);
  ncell_list_amr_t dst = {0};

  // Mandatory. "nr" or "eutra"
  dst.rat = dec_string_opt(it, "rat");

  // Optional. 
  //int* dl_earfcn;

  // Optional
  //int* n_id_cell;
  
  // Optional
  //ecgi_amr_t* ecgi;

  // Optional 
  //int* ssb_nr_arfcn;
  ans_cjson_t tmp = find_object(it, "ssb_nr_arfcn");
  if(tmp.it != NULL){
    dst.ssb_nr_arfcn = calloc(1, sizeof(int)); 
    *dst.ssb_nr_arfcn = ((cJSON*)tmp.it)->valueint;
  }

  // Optional
  tmp = find_object(it, "n_id_nrcell");
  if(tmp.it != NULL){
    dst.n_id_nrcell = calloc(1, sizeof(int)); 
    *dst.n_id_nrcell = ((cJSON*)tmp.it)->valueint;
  }

  // Optional
  exp_ncgi_amr_t e = dec_opt_ncgi_amr(it);
  if(e.value == true){
    dst.ncgi = calloc(1, sizeof(ncgi_amr_t));
    assert(dst.ncgi != NULL && "Memory exhausted");
    *dst.ncgi = e.ncgi_amr; 
  }
  
  // Mandatory
  dst.handover_target = dec_bool_opt(it, "handover_target");

  // Mandatory
  dst.cell_redirect_target = dec_bool_opt(it, "cell_redirect_target");

  // Optional. Only applicable to E-UTRA cells.
  // bool* eps_fallback_target;

  // Optional. Only applicable to E-UTRA cells.
  // bool* emergency_fallback_target;

  return dst;
}

ncgi_amr_t dec_ncgi_amr(void* it_void)
{
  assert(it_void != NULL);

  ncgi_amr_t dst = {0};

  ans_cjson_t ans_ncgi = find_object(it_void, "ncgi");
  assert(ans_ncgi.it != NULL);
  cJSON const* it = ans_ncgi.it;

  // NR Cell Identity PLMN.
  dst.plmn= dec_string_opt(it, "plmn");

  // NR Cell Identity (36 bits)
  dst.nci = dec_int_opt(it, "nci");

  return dst;
}

exp_ncgi_amr_t dec_opt_ncgi_amr(void* it_void)
{
  assert(it_void != NULL);

  exp_ncgi_amr_t dst = {.value = false};

  ans_cjson_t ans_ncgi = find_object(it_void, "ncgi");
  if(ans_ncgi.it == NULL)
    return dst;

  dst.value = true;

  assert(ans_ncgi.it != NULL);
  cJSON const* it = ans_ncgi.it;

  // NR Cell Identity PLMN.
  dst.ncgi_amr.plmn= dec_string_opt(it, "plmn");

  // NR Cell Identity (36 bits)
  dst.ncgi_amr.nci = dec_int_opt(it, "nci");

  return dst;
}

nr_cells_amr_t dec_nr_cells_amr(void* it_void)
{
  assert(it_void != NULL);

  nr_cells_amr_t dst = {0}; 
  cJSON const* it = (cJSON const*)it_void;

  // Mandatory. Downlink antenna count
  dst.n_antenna_dl = dec_int_opt(it, "n_antenna_dl");

  // Mandatory. Uplink antenna count 
  dst.n_antenna_ul = dec_int_opt(it, "n_antenna_ul"); 

  // Mandatory. Downlink layer count
  dst.n_layer_dl = dec_int_opt(it, "n_layer_dl");

  // Mandatory. Uplink layer count
  dst.n_layer_ul = dec_int_opt(it, "n_layer_ul");

  // Mandatory. Cell gain in dB.
  dst.gain = dec_float_opt(it, "gain");

  // Mandatory. UL state
  dst.ul_disabled = dec_bool_opt(it, "ul_disabled");

  // Mandatory. RF port number index.
  dst.rf_port = dec_int_opt(it, "rf_port");

  // Optional. Label set in configuration file.
  // char* label;

  // Mandatory. Maximum QAM size used in downlink.
  // 64 or 256
  dst.dl_qam = dec_int_opt(it, "dl_qam");
  assert(dst.dl_qam == 64 || dst.dl_qam == 256); 

  // Mandatory. Maximum QAM size used in uplink
  // 64 or 256
  dst.ul_qam = dec_int_opt(it, "ul_qam");
  assert(dst.ul_qam == 64 || dst.ul_qam == 256); 

  // Mandatory. NR Cell Identity + plmn
  dst.ncgi = dec_ncgi_amr((void*)it);

  // Mandatory. Cell barred status.
  dst.cell_barred = dec_bool_opt(it, "cell_barred");

  // Optional.
  //bool* cell_barred_redcap_1rx; 

  // Optional.
  //bool* cell_barred_redcap_2rx; 

  // Optional.
  //bool* cell_barred_ntn; 

  // Mandatory. Physical cell ID 
  dst.n_id_nrcell = dec_int_opt(it, "n_id_nrcell");

  // Mandatory. NR frequency band indicator
  dst.band = dec_int_opt(it, "band");

  // Mandatory. Integer. Downlink ARFCN
  dst.dl_nr_arfcn = dec_int_opt(it, "dl_nr_arfcn");

  // Mandatory. Uplink ARFCN.
  dst.ul_nr_arfcn= dec_int_opt(it, "ul_nr_arfcn"); 

  // Mandatory. Number of downlink resource blocks
  dst.n_rb_dl = dec_int_opt(it, "n_rb_dl");

  // Mandatory. Number of uplink resource blocks.
  dst.n_rb_ul = dec_int_opt(it, "n_rb_ul");

  // Mandatory. SSB ARFCN.
  dst.ssb_nr_arfcn = dec_int_opt(it, "ssb_nr_arfcn");

  // Mandatory. Downlink sub carrier spacing (15 * 2 ^ mu in KHz)
  dst.dl_mu = dec_int_opt(it, "dl_mu");

  // Mandatory. Uplink sub carrier spacing (15 * 2 ^ mu in KHz)
  dst.ul_mu = dec_int_opt(it, "ul_mu");

  // Mandatory. SSB sub carrier spacing (15 * 2 ^ mu in KHz).
  dst.ssb_mu = dec_int_opt(it, "ssb_mu");

  // Mandatory. Enumeration: FDD, TDD. Operation mode 
  dst.mode = dec_string_opt(it, "mode");

  // Mandatory. Cell PRACH sequence index.
  dst.prach_sequence_index = dec_int_opt(it, "prach_sequence_index");

  // Optional. connected_mobility
 // ans_cjson_t const cm = find_object(it, "connected_mobility" ); 
 // if(cm.it != NULL){
 //   dst.conn_mob = calloc(1, sizeof(connected_mobility_amr_t));
 //   assert(dst.conn_mob != NULL && "Memory exhausted");
 //   *dst.conn_mob = dec_connected_mobility_amr((void*)cm.it);
 // } 

  // Optional. NR secondary cells 
  ans_cjson_t const scl = find_object(it, "scell_list" ); 
  if(scl.it != NULL){
    dst.scell_lst = calloc(1, sizeof(scell_list_amr_t));
    assert(dst.scell_lst != NULL && "Memory exhausted");
    *dst.scell_lst = dec_scell_list_amr((void*)scl.it); 
  }

  // Optional ncell_list
  ans_cjson_t const ncl = find_object(it, "ncell_list"); 
  if(ncl.it != NULL){
    dst.arr_ncell_lst = calloc(1, sizeof(arr_ncell_list_amr_t)); 
    assert(dst.arr_ncell_lst != NULL && "Memory exhausted");
    *dst.arr_ncell_lst = dec_arr_ncell_list_amr((void*)ncl.it);
  }

  // Optional. Dual connectivity 
  // nr_dc_scg_cell_list

  // Mandatory. PLMN list 
  ans_cjson_t plst = find_object(it, "plmn_list");
  assert(plst.it != NULL);
 
  dst.sz_plm_lst = cJSON_GetArraySize(plst.it);
  
  if(dst.sz_plm_lst > 0){
    dst.plmn_lst = calloc(1 , sizeof(plmn_list_amr_t ));
    assert(dst.plmn_lst != NULL && "Memory exhausted");
  }

  cJSON* tmp_it = ((cJSON*)plst.it)->child; 
  for(size_t i = 0; i < dst.sz_plm_lst; ++i){
    assert(tmp_it != NULL);
    dst.plmn_lst[i] = dec_plmn_list_amr(tmp_it);
    tmp_it = tmp_it->next;
  } 

  return dst;
}

nssai_amr_t dec_nssai_amr(void* it)
{
  assert(it != NULL);
  nssai_amr_t dst = {0};

  // Mandatory. Slice Srvice Type [1-255] 
  dst.sst = dec_int_opt(it, "sst"); 
  assert(dst.sst != 0); 

  // Optional. Slice Differentiator [0-0xFFFFFE]
  ans_cjson_t ans = find_object(it, "sd");
  if(ans.it != NULL){
    dst.sd = calloc(1, sizeof(uint32_t));
    assert(dst.sd != NULL && "Memory exhausted");
    *dst.sd = ((cJSON*)ans.it)->valueint; 
    assert(*dst.sd < 0xFFFFFF);
  }

  return dst;
}

path_amr_t dec_path_amr(void* it)
{
  assert(it != NULL);

  path_amr_t dst = {0};

  // Mandatory
  dst.type = dec_string_opt(it, "type");

  // Mandatory. Path delay in ns
  dst.delay = dec_float_opt(it, "delay");

  // Mandatory. Path gain in dB
  dst.gain = dec_float_opt(it, "gain");

  // Mandatory. Frequency shift in Hz
  dst.freq_shift = dec_float_opt(it, "freq_shift");

  // Mandatory. Doppler frequency in Hz
  dst.freq_doppler = dec_float_opt(it, "freq_doppler");

  return dst;
}

plmn_list_amr_t dec_plmn_list_amr(void* it)
{
  assert(it != NULL);

  plmn_list_amr_t dst = {0}; 
 
  // Mandatory.
  // Array of strings. List of PLMNs for this TAC
  ans_cjson_t ans = find_object(it, "plmn_ids");  
  
  dst.sz_plmn_ids = cJSON_GetArraySize(ans.it);
  assert(dst.sz_plmn_ids > 0);
  
  dst.plmn_ids = calloc(dst.sz_plmn_ids, sizeof(char*));
  assert(dst.plmn_ids != NULL && "Memory exhausted");

  for(size_t i = 0; i< dst.sz_plmn_ids; ++i){
    cJSON * tmp = cJSON_GetArrayItem(ans.it, i);
    dst.plmn_ids[i] = strdup(tmp->valuestring);
  }

  // Mandatory. Reserved flag.
  dst.reserved = dec_bool_opt(it, "reserved"); 

  // Mandatory. Tracking Area Code
  dst.tac = dec_int_opt(it, "tac");

  // Optional. RAN Area Code.
  // int* ranac;

  // Array of objects. Mandatory? Optional?
  ans_cjson_t ans_nssai = find_object(it, "nssai");  
  dst.sz_nssai_amr = cJSON_GetArraySize(ans_nssai.it);
  assert(dst.sz_nssai_amr > 0 && "Not clear froms amari specs..." );
  
  dst.nssai = calloc(dst.sz_nssai_amr, sizeof(nssai_amr_t));
  assert(dst.nssai != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_nssai_amr; ++i){
    cJSON * tmp = cJSON_GetArrayItem(ans_nssai.it, i);
    dst.nssai[i] = dec_nssai_amr(tmp);
  }

  return dst;
}

qos_flows_ue_lst_amr_t dec_qos_flows_ue_lst_amr(void* it)
{
  assert(it != NULL);

  qos_flows_ue_lst_amr_t dst = {0};

  dst.pdu_session_id = dec_int_opt(it, "pdu_session_id") ;
  dst.dl_total_bytes = dec_int_opt(it, "dl_total_bytes") ;
  dst.ul_total_bytes = dec_int_opt(it, "ul_total_bytes") ;
  dst.sst = dec_int_opt(it, "sst"); 
  dst.sd = dec_int_opt(it, "sd") ;

  return dst;
}

rf_ports_amr_t dec_rf_ports_amr(void* it)
{
  assert(it != NULL);
  rf_ports_amr_t dst = {0};

  ans_cjson_t ans = find_object(it, "channel_dl" );
  if(ans.it != NULL)
    dst.channel_dl = dec_channel_dl_amr((void*)ans.it); 

  // Mandatory. Uplink frequency shift in Hz
  dst.ul_freq_shift = dec_float_opt(it, "ul_freq_shift");

  // Mandatory. Sample rate in Hz
  dst.sample_rate = dec_float_opt(it, "sample_rate");

  return dst;
}

rf_ports_ms_amr_t dec_rf_ports_ms_amr(void* it)
{
  assert(it != NULL);

  rf_ports_ms_amr_t dst = {0}; 

  dst.min= dec_float_opt(it, "min");
  dst.max= dec_float_opt(it, "max");
  dst.sd= dec_float_opt(it, "sd");

  return dst;
}

rx_channels_amr_t dec_rx_channels_amr(void* it)
{
  assert(it != NULL);
  rx_channels_amr_t dst = {0}; 

  // Mandatory. RF reception gain, in dB
  dst.gain = dec_float_opt(it, "gain");

  // Mandatory. RF reception frequency, in MHz 
  dst.freq = dec_float_opt(it, "freq");

  // Mandatory? Optional? Bug? 2023-12-15
  // int port;

  return dst;
}

scell_list_amr_t dec_scell_list_amr(void* it)
{
  assert(it != NULL);
  scell_list_amr_t dst = {0}; 

  // Mandatory. Cell identifier 
  dst.cell_id = dec_int_opt(it, "cell_id");

  // Mandatory. Indicates if PUSCH transmission is allowed
  dst.ul_allowed = dec_int_opt(it, "ul_allowed");

  return dst;
}

tx_channels_amr_t dec_tx_channels_amr(void* it)
{
  assert(it != NULL);
  tx_channels_amr_t dst = {0};

  // Mandatory RF transmission gain, in dB.
  dst.gain = dec_float_opt(it, "gain"); 

  // Mandatory RF transmission frequency, in MHz.
  dst.freq = dec_float_opt(it, "freq");

  // Mandatory. RF port index.
  dst.port = dec_int_opt(it, "port");

  return dst;
}

ue_lst_amr_t dec_ue_lst_amr(void* it)
{
  assert(it != NULL);

  ue_lst_amr_t dst = {0}; 

  // Mandatory for 5G. RAN UE id. Present for NR UEs.
  dst.ran_ue_id = dec_int_opt(it,"ran_ue_id");

  // Optional integer. AMF UE id. It is present when the UE-
  // associated logical NG-connection is setup.
  // dst.amf_ue_id = dec_int(it,"amf_ue_id");

  // Mandatory
  dst.rnti = dec_int_opt(it,"rnti"); 

  // Mandatory
  // Array of Objects
  ans_cjson_t ans = find_object(it, "cells");
  dst.arr_cells = dec_arr_cells_ue_lst_amr((void*)ans.it);

  // Mandatory. 
  // Array of objects
  ans = find_object(it, "qos_flow_list");
  dst.qos_flows = dec_arr_qos_flows_ue_lst((void*)ans.it);

  return dst;
}

