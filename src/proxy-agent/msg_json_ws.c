/* encoding/decoding in json. */
#include <json-c/json.h>
#include <stdio.h>
#include <libwebsockets.h>

#include "util/conversions.h"
#include "util/alg_ds/alg/defer.h"
#include "msg_json_ws.h"
#include "ws_msg_hdlr.h"

static char global_buff[1024];

void ws_msghdr_free(ws_msghdr_t msg) {
  if (msg.type != NULL)
    free (msg.type);
}

ws_msghdr_t ws_json_get_msghdr(const ws_msg_t *msg) 
{
  ws_msghdr_t ret_msg = {.type = NULL, .msg_id = -1};
  struct json_tokener *tok = json_tokener_new();
  defer({json_tokener_free(tok); }; );
  if (!tok)
    return ret_msg;
  struct json_object *json_obj = json_tokener_parse_ex(tok, msg->buf, msg->len);
  defer({json_object_put(json_obj); }; );
  lwsl_debug("Dumping json data %s:\n", 
            json_object_to_json_string_ext(json_obj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
  struct json_object *msg_type;
  if (!json_object_object_get_ex(json_obj, "message", &msg_type))
    return ret_msg;

  const char *p = json_object_get_string(msg_type);
  ret_msg.type = strdup(p);
  struct json_object *msg_id;
  if (!json_object_object_get_ex(json_obj, "message_id", &msg_id))
    return ret_msg;
  p = json_object_get_string(msg_id);
  ret_msg.msg_id = atoi(p);

  return ret_msg;
}

bool ws_json_decode_indication_gnb(const ws_msg_t *in_msg, ws_ind_t *out) 
{
  assert ((out != NULL || in_msg != NULL) && "programming error\n");
  
  struct json_tokener *tok = json_tokener_new();
  defer({json_tokener_free(tok); }; );
  struct json_object *root_json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({json_object_put(root_json_obj); }; );

  if (root_json_obj == NULL){
    lwsl_err("error json token: %s\n", json_tokener_error_desc(json_tokener_get_error(tok)));
    return false;
  }
  
  struct json_object *cells;
  if (!json_object_object_get_ex(root_json_obj, "cells", &cells))
    return false;
  
  int idx=0;
  json_object_object_foreach(cells, key, val)
  { 
    out->cells_stats[idx].cell_id = atoi(key);

    struct json_object *dl_bitrate;
    if (!json_object_object_get_ex(val, "dl_bitrate", &dl_bitrate))
      lwsl_debug("WS: indication from cell %d. dl_bitrate not found\n", out->cells_stats[idx].cell_id);
    else 
      out->cells_stats[idx].dl_bitrate = json_object_get_double(dl_bitrate);
    
    struct json_object *ul_bitrate;
    if (!json_object_object_get_ex(val, "ul_bitrate", &ul_bitrate))
      lwsl_debug("WS: indication from cell %d. ul_bitrate not found\n", out->cells_stats[idx].cell_id);
    else 
      out->cells_stats[idx].ul_bitrate = json_object_get_double(ul_bitrate);
    
    struct json_object *ue_count_avg;
    if (!json_object_object_get_ex(val, "ue_count_avg", &ue_count_avg))
      lwsl_debug("WS: indication from cell %d. ue_count_avg not found\n", out->cells_stats[idx].cell_id);
    else 
      out->cells_stats[idx].ue_count_avg = json_object_get_double(ue_count_avg);

    struct json_object *dl_tx;
    if (!json_object_object_get_ex(val, "dl_tx", &dl_tx))
      lwsl_debug("WS: indication from cell %d. dl_tx not found\n", out->cells_stats[idx].cell_id);
    else 
      out->cells_stats[idx].dl_tx = json_object_get_int(dl_tx);

    struct json_object *ul_tx;
    if (!json_object_object_get_ex(val, "ul_tx", &ul_tx))
      lwsl_debug("WS: indication from cell %d. ul_tx not found\n", out->cells_stats[idx].cell_id);
    else 
      out->cells_stats[idx].ul_tx = json_object_get_int(ul_tx);

    struct json_object *dl_retx;
    if (!json_object_object_get_ex(val, "dl_retx", &dl_retx))
      lwsl_debug("WS: indication from cell %d. dl_retx not found\n", out->cells_stats[idx].cell_id);
    else 
      out->cells_stats[idx].dl_retx = json_object_get_int(dl_retx);

    struct json_object *ul_retx;
    if (!json_object_object_get_ex(val, "ul_retx", &ul_retx))
      lwsl_debug("WS: indication from cell %d. ul_retx not found\n", out->cells_stats[idx].cell_id);
    else 
      out->cells_stats[idx].ul_retx = json_object_get_int(ul_retx);

    idx++;
    if (idx > AMARISOFT_MAX_CELL_NUM) {
      lwsl_err("hit hard limit on internal buffer\n");
      return false;
    }
  }
  
  return true;
}

bool ws_json_decode_indication_ue (const ws_msg_t *in_msg, ws_ind_t *out) 
{
  assert ((out != NULL || in_msg != NULL) && "programming error\n");
  
  struct json_tokener *tok = json_tokener_new();
  defer({json_tokener_free(tok); }; );
  struct json_object *root_json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({json_object_put(root_json_obj); }; );

  if (root_json_obj == NULL){
    lwsl_err("error json token: %s\n", json_tokener_error_desc(json_tokener_get_error(tok)));
    return false;
  }
    
  /* example of 1 ue connected json to be parsed
      "ue_list": [                                                                                                                                                
        {                                                                                                                                                       
            "ran_ue_id": 2,                                                                                                                                     
            "amf_ue_id": 139,
            "rnti": 17925,
            "cells": [
                {
                    "cell_id": 1,
                    "dl_bitrate": 148,
                    "ul_bitrate": 35,
                    "dl_tx": 88,
                    "ul_tx": 17,
                    "dl_retx": 0,
                    "ul_retx": 6,
                    "pusch_snr": 35.4,
                    "epre": -95.5,
                    "ul_phr": 38,
                    "ul_path_loss": 55.2,
                    "dl_mcs": 26,
                    "ul_mcs": 25.5,
                    "turbo_decoder_min": 1,
                    "turbo_decoder_avg": 2.696,
                    "turbo_decoder_max": 5,
                    "cqi": 15,
                    "ri": 1,
                    "initial_ta": 5
                }
              ]                                                                                                                           
        }                                                                                                                                                       
    ],      
  */
  struct json_object *ue_list;
  if (!json_object_object_get_ex(root_json_obj, "ue_list", &ue_list))
    return false;

  int n = json_object_array_length(ue_list);
  out->n_connected_ue = n;

  for (int idx=0; idx<n; idx++)
  {
    struct json_object *rnti;
    if (!json_object_object_get_ex(json_object_array_get_idx(ue_list, idx), "rnti", &rnti))
      lwsl_debug("WS: indication from cell n.%d rnti not found\n", idx);
    else 
      out->ue_stats[idx].rnti = json_object_get_int(rnti);

    struct json_object *cells;
    if (!json_object_object_get_ex(json_object_array_get_idx(ue_list, idx), "cells", &cells))
      return false;
    int cells_n = json_object_array_length(cells);
    for (int j = 0; j<cells_n; j++)
    {
      // XXX: not sure about above assignment like 'out->ue_stats[idx].cqi = json_object_get_int(cqi)' below.
      // it is not clear the association between cells in UE and cells in gnb (1 cell and 1 ue ?) 
      struct json_object *cqi;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "cqi", &cqi))
        lwsl_debug("WS: indication from cell n.%d cqi not found\n", j);
      else 
        out->ue_stats[idx].cqi = json_object_get_int(cqi); 
      
      struct json_object *phr;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "ul_phr", &phr))
        lwsl_debug("WS: indication from cell n.%d ul_phr not found\n", j);
      else 
        out->ue_stats[idx].phr = json_object_get_int(phr); 

      struct json_object *dl_bitrate;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "dl_bitrate", &dl_bitrate))
        lwsl_debug("WS: indication from cell n.%d dl_bitrate not found\n", j);
      else 
        out->ue_stats[idx].dl_bitrate = json_object_get_int(dl_bitrate); 
      
      struct json_object *ul_bitrate;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "ul_bitrate", &ul_bitrate))
        lwsl_debug("WS: indication from cell n.%d ul_bitrate not found\n", j);
      else 
        out->ue_stats[idx].ul_bitrate = json_object_get_int(ul_bitrate); 

      struct json_object *dl_tx;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "dl_tx", &dl_tx))
        lwsl_debug("WS: indication from cell n.%d dl_tx not found\n", j);
      else 
        out->ue_stats[idx].dl_tx = json_object_get_int(dl_tx); 

      struct json_object *dl_retx;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "dl_retx", &dl_retx))
        lwsl_debug("WS: indication from cell n.%d dl_retx not found\n", j);
      else 
        out->ue_stats[idx].dl_retx = json_object_get_int(dl_retx); 

      struct json_object *dl_mcs;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "dl_mcs", &dl_mcs))
        lwsl_debug("WS: indication from cell n.%d dl_mcs not found\n", j);
      else 
        out->ue_stats[idx].dl_mcs = json_object_get_int(dl_mcs); 

      struct json_object *ul_mcs;
      if (!json_object_object_get_ex(json_object_array_get_idx(cells, j), "ul_mcs", &ul_mcs))
        lwsl_debug("WS: indication from cell n.%d ul_mcs not found\n", j);
      else 
        out->ue_stats[idx].ul_mcs = json_object_get_int(ul_mcs); 
    }
    
    if (idx > AMARISOFT_MAX_UE_NUM) {
      lwsl_err("hit hard limit on internal buffer\n");
      return false;
    }
  }

  
  return true;
}




bool ws_json_decode_e2setup (const ws_msg_t *in_msg, global_e2_node_id_t *out) 
{ 
  assert (out != NULL && "programming error\n");

  out->cu_du_id = NULL;
  out->nb_id = 0;
  out->plmn.mcc = 0;
  out->plmn.mnc = 0;
  out->plmn.mnc_digit_len = 0;
  out->type = ngran_gNB;

  struct json_tokener *tok = json_tokener_new();
  defer({json_tokener_free(tok); }; );
  struct json_object *json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({json_object_put(json_obj); }; );
  struct json_object *key, *plmn, *gnb_id;
  if (!json_object_object_get_ex(json_obj, "global_gnb_id", &key))
    return false;
  /* 
  * Example of json snippet coming from Amarisoft:
  * "global_gnb_id": {
  *      "plmn": 50501",
  *      "gnb_id_bits": 8,
  *      "gnb_id": 3590,
  *      "gnb_name": gnb0000e06"
  * }
  *
  */

  if (!json_object_object_get_ex(key, "plmn", &plmn))
    return false;
  const char *p = json_object_get_string(plmn);
  if (!p)
    return false;
  const int plmn_val = atoi(p);
  out->plmn.mcc = plmn_val > 99999 ? plmn_val/1000 : plmn_val/100;
  out->plmn.mnc = plmn_val > 99999 ? plmn_val%1000 : plmn_val%100;
  out->plmn.mnc_digit_len = out->plmn.mnc > 99 ? 3 : 2;

  if (!json_object_object_get_ex(key, "gnb_id", &gnb_id))
    return false;
  out->nb_id = json_object_get_int(gnb_id);

  return true;
}

const char * ws_json_encode_indication (int msg_id, int sm_id) 
{ 
  (void)sm_id;
  // snprintf(global_buff, sizeof(global_buff), "{\"message\":\"stats\",\"message_id\":\"%d\"}", msg_id);
  snprintf(global_buff, 
          sizeof(global_buff), 
          "[{\"message\":\"stats\",\"message_id\":\"%d\"},{\"message\":\"ue_get\",\"message_id\":\"%d\",\"stats\":1}]", 
          msg_id, msg_id);
  return global_buff;
}

const char * ws_json_encode_e2setup(int msg_id) 
{
  snprintf(global_buff, sizeof(global_buff), "{\"message\":\"config_get\",\"message_id\":\"%d\"}", msg_id);
  return global_buff;
}

