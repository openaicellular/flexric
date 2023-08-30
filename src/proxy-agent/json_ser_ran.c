/* 
 * Serialization in json.
 * 
 * For the encoding part, we mainly do not use the third party library json-c as the data structure we need to encode is most of times very simple.
 */
#include <stdio.h>
#include <json-c/json.h>
#include <libwebsockets.h>

#include "util/conversions.h"
#include "util/alg_ds/alg/defer.h"
#include "lib/e2ap/e2ap_global_node_id_wrapper.h"
#include "sm/mac_sm/mac_sm_id.h"

#include "ran_if.h"
#include "ran_msg_hdlr.h"
#include "ser_ran.h"
#include "notif_e2_ran.h"
#include "ws_io_ran.h"

static char global_buff[1024];
#define AMARISOFT_SUPPORTED_VERSION "2021-12-17"

struct amr_fields_t {
  const char *name;
  enum {AMR_DT_INT, AMR_DT_DOUBLE} dtype;
  void *dst_container;
};
static void parse_amarisoft_fields_unnested (struct json_object *val, int cell_idx, struct amr_fields_t *tbl, size_t tbl_len)
{
  for (size_t i = 0; i<tbl_len;  i++)
    {  
      struct json_object *cell_field_tmp;
      if (!json_object_object_get_ex(val, tbl[i].name, &cell_field_tmp))
       lwsl_debug("WS: indication from cell %d. %s not found\n", cell_idx, tbl[i].name);
      else 
      {
        double *pdouble;
        int *pint;
        switch (tbl[i].dtype)
        {
          case AMR_DT_DOUBLE:
            pdouble = (double *)tbl[i].dst_container;
            *pdouble = json_object_get_double(cell_field_tmp);
            break;
          case AMR_DT_INT:
            pint = (int *)tbl[i].dst_container;
            *pint = json_object_get_int(cell_field_tmp);
            break;
          default:
            assert(0!=0 && "unexpected static datatype declaration\n");
        }
      }
    }
}
static void json_free_ran_msghdr(ran_msghdr_t msg)
{
  if (msg.type != NULL)
    free(msg.type);
}

static ran_msghdr_t json_get_ran_msghdr(const ran_msg_t *msg)
{
 
  ran_msghdr_t ret_msg = {.type = NULL, .msg_id = -1};
  struct json_tokener *tok = json_tokener_new();
  defer({ json_tokener_free(tok); };);
  if (!tok)
    return ret_msg;
  struct json_object *json_obj = json_tokener_parse_ex(tok, msg->buf, msg->len);
  defer({ json_object_put(json_obj); };);
  lwsl_debug("Dumping json data %s:\n",
             json_object_to_json_string_ext(json_obj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

  /*
  * structure to be parsed. 
  *  "message": "config_set",
  *  "error": <error message>
  *  "message_id": if != 0, key in io_loop_t map.
  */

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
  
  struct json_object *err; // error field is optional
  ret_msg.error = false;
  if (!json_object_object_get_ex(json_obj, "error", &err))
    return ret_msg;
  ret_msg.error = false; // we discad getting the exact reason of the error as we do not use it

  return ret_msg;
}

/* ----------------------------------------------------------------------------------- 
 *                                       Decoding
 * ----------------------------------------------------------------------------------- */
// parsing Amarisoft API command 'stats'
static bool json_decode_ran_indication_stats(const ran_msg_t *in_msg, ran_ind_t *out)
{
  /* 
   * XXX:TO-IMPROVE: some data can be not found in the reported json for various reasons (i.e. amarisoft version). 
   * In that case, you should find a convention to signal a not set value as for now we only
   * issue a log at debug level for any amarisoft version.
   * Further, you could filter out the warning based on the amarisoft version (ui.e. adding a new version field in 
   * amr_cell_fields_tbl below).
   */

  assert((out != NULL || in_msg != NULL) && "programming error\n");

  struct json_tokener *tok = json_tokener_new();
  defer({ json_tokener_free(tok); };);
  struct json_object *root_json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({ json_object_put(root_json_obj); };);

  if (root_json_obj == NULL)
  {
    lwsl_err("error json token: %s\n", json_tokener_error_desc(json_tokener_get_error(tok)));
    return false;
  }

  /* 
  example of json to be parsed
  "cells": {
     "1": {
       "dl_bitrate": 0,
       "ul_bitrate": 0,
       "dl_tx": 0,
       "ul_tx": 0
    }
  }
   */
  struct json_object *cells;
  if (!json_object_object_get_ex(root_json_obj, "cells", &cells))
    return false;

  int idx = 0;
  json_object_object_foreach(cells, key, val)
  {
    struct amr_fields_t amr_cell_fields_tbl[] = {
      {"dl_bitrate"         , AMR_DT_DOUBLE , &out->cells_stats[idx].dl_bitrate},
      {"ul_bitrate"         , AMR_DT_DOUBLE , &out->cells_stats[idx].ul_bitrate},
      {"mbms_bitrate"       , AMR_DT_DOUBLE , &out->cells_stats[idx].mbms_bitrate},
      {"ue_count_avg"       , AMR_DT_INT    , &out->cells_stats[idx].ue_count_avg}, 
      {"dl_tx"              , AMR_DT_INT    , &out->cells_stats[idx].dl_tx},
      {"ul_tx"              , AMR_DT_INT    , &out->cells_stats[idx].ul_tx}, 
      {"dl_err"             , AMR_DT_INT    , &out->cells_stats[idx].dl_err},
      {"ul_err"             , AMR_DT_INT    , &out->cells_stats[idx].ul_err},
      {"dl_retx"            , AMR_DT_INT    , &out->cells_stats[idx].dl_retx}, 
      {"ul_retx"            , AMR_DT_INT    , &out->cells_stats[idx].ul_retx},
      {"dl_sched_users_min" , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_sched_users_min},
      {"dl_sched_users_avg" , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_sched_users_avg},
      {"dl_sched_users_max" , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_sched_users_max},
      {"ul_sched_users_min" , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_sched_users_min},
      {"ul_sched_users_avg" , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_sched_users_avg},  
      {"ul_sched_users_max" , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_sched_users_max},  
      {"dl_use_min"         , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_use_min},
      {"dl_use_avg"         , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_use_avg},
      {"dl_use_max"         , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_use_max},
      {"ul_use_min"         , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_use_min},
      {"ul_use_avg"         , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_use_avg},
      {"ul_use_max"         , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_use_max},
      {"ctrl_use_min"       , AMR_DT_DOUBLE,  &out->cells_stats[idx].ctrl_use_min},
      {"ctrl_use_avg"       , AMR_DT_DOUBLE,  &out->cells_stats[idx].ctrl_use_avg},
      {"ctrl_use_max"       , AMR_DT_DOUBLE,  &out->cells_stats[idx].ctrl_use_max},
      {"ue_count_min"       , AMR_DT_INT    , &out->cells_stats[idx].ue_count_min},
      {"ue_count_max"       , AMR_DT_INT    , &out->cells_stats[idx].ue_count_max},
      {"ue_count_avg"       , AMR_DT_INT    , &out->cells_stats[idx].ue_count_avg},
      {"erab_count_min"     , AMR_DT_INT    , &out->cells_stats[idx].erab_count_min},
      {"erab_count_max"     , AMR_DT_INT    , &out->cells_stats[idx].erab_count_max},
      {"erab_count_avg"     , AMR_DT_INT    , &out->cells_stats[idx].erab_count_avg},
      {"drb_count_min"      , AMR_DT_INT    , &out->cells_stats[idx].drb_count_min},
      {"drb_count_max"      , AMR_DT_INT    , &out->cells_stats[idx].drb_count_max},
      {"drb_count_avg"      , AMR_DT_INT    , &out->cells_stats[idx].drb_count_avg},
      {"drb_count_avg"      , AMR_DT_INT    , &out->cells_stats[idx].drb_count_avg},
      {"dl_gbr_use_min"     , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_gbr_use_min},
      {"dl_gbr_use_avg"     , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_gbr_use_avg},
      {"dl_gbr_use_max"     , AMR_DT_DOUBLE,  &out->cells_stats[idx].dl_gbr_use_max},
      {"ul_gbr_use_min"     , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_gbr_use_min},
      {"ul_gbr_use_avg"     , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_gbr_use_avg},
      {"ul_gbr_use_max"     , AMR_DT_DOUBLE,  &out->cells_stats[idx].ul_gbr_use_max}
    };

    out->cells_stats[idx].cell_id = atoi(key);
    parse_amarisoft_fields_unnested(val, 
                                    out->cells_stats[idx].cell_id, 
                                    amr_cell_fields_tbl, 
                                    sizeof(amr_cell_fields_tbl)/sizeof (struct amr_fields_t));
    
    idx++;
    if (idx > AMARISOFT_MAX_CELL_NUM)
    {
      lwsl_err("hit hard limit on internal buffer\n");
      return false;
    }
  }

  return true;
}

// parsing Amarisoft API command 'ue_get'
static bool json_decode_ran_indication_ue_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
  assert((out != NULL || in_msg != NULL) && "programming error\n");

  struct json_tokener *tok = json_tokener_new();
  defer({ json_tokener_free(tok); };);
  struct json_object *root_json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({ json_object_put(root_json_obj); };);

  if (root_json_obj == NULL)
  {
    lwsl_err("error json token: %s\n", json_tokener_error_desc(json_tokener_get_error(tok)));
    return false;
  }

  /* example of json to be parsed related to 1 UE connected to 1 CELL 
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

  for (int idx = 0; idx < n; idx++)
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
    out->ue_stats[idx].num_cell = cells_n;
    assert (n < AMARISOFT_MAX_CELL_NUM && "hard limit on number of cell hit\n");
    for (int j = 0; j < cells_n; j++)
    {
      struct amr_fields_t amr_cell_fields_tbl[] = {
        {"cell_id"          , AMR_DT_INT    , &out->ue_stats[idx].cell[j].cell_id},
        {"cqi"              , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].cqi},
        {"ri"               , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].ri},
        {"dl_bitrate"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].dl_bitrate},
        {"ul_bitrate"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].ul_bitrate},
        {"dl_tx"            , AMR_DT_INT    , &out->ue_stats[idx].cell[j].dl_tx},
        {"dl_retx"          , AMR_DT_INT    , &out->ue_stats[idx].cell[j].dl_retx},
        {"ul_retx"          , AMR_DT_INT    , &out->ue_stats[idx].cell[j].ul_retx},
        {"dl_mcs"           , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].dl_mcs},
        {"ul_mcs"           , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].ul_mcs},
        {"ul_n_layer"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].ul_n_layer},
        {"turbo_decoder_min", AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].turbo_decoder_min},
        {"turbo_decoder_avg", AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].turbo_decoder_avg},
        {"turbo_decoder_max", AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].turbo_decoder_max},
        {"pucch1_snr"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].pucch1_snr},
        {"pusch_snr"        , AMR_DT_DOUBLE , &out->ue_stats[idx].cell[j].pusch_snr},
        {"ul_phr"           , AMR_DT_INT    , &out->ue_stats[idx].cell[j].ul_phr},
        {"epre"             , AMR_DT_INT    , &out->ue_stats[idx].cell[j].epre},
        {"ul_path_loss"     , AMR_DT_INT    , &out->ue_stats[idx].cell[j].ul_path_loss},
        {"p_ue"             , AMR_DT_INT    , &out->ue_stats[idx].cell[j].p_ue},
        {"initial_ta"       , AMR_DT_INT    , &out->ue_stats[idx].cell[j].initial_ta}
      };
      parse_amarisoft_fields_unnested(json_object_array_get_idx(cells, j), 
                                      j, 
                                      amr_cell_fields_tbl, 
                                      sizeof(amr_cell_fields_tbl)/sizeof (struct amr_fields_t));
    }

    if (idx > AMARISOFT_MAX_UE_NUM)
    {
      lwsl_err("hit hard limit on internal buffer\n");
      return false;
    }
  }

  return true;
}

// parsing Amarisoft API command 'config_get'
static bool json_decode_ran_indication_config_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
  assert(0!=0 && "not supported");
  //TODO
  assert((out != NULL || in_msg != NULL) && "programming error\n");
  return true;
}

// parsing Amarisoft API command 'qos_flow_get'
static bool json_decode_ran_indication_qos_flow_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
  assert(0!=0 && "not supported");
  //TODO
  assert((out != NULL || in_msg != NULL) && "programming error\n");
  return true;
}

// parsing Amarisoft API command 'erab_get'
static bool json_decode_ran_indication_erab_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
  assert(0!=0 && "not supported");
  //TODO
  assert((out != NULL || in_msg != NULL) && "programming error\n");
  return true;
}

static bool json_decode_ran_e2setup(const ran_msg_t *in_msg, global_e2_node_id_t *out)
{
  assert(out != NULL && "programming error\n");

  out->cu_du_id = NULL;
  out->nb_id = 0;
  out->plmn.mcc = 0;
  out->plmn.mnc = 0;
  out->plmn.mnc_digit_len = 0;
  out->type = e2ap_ngran_gNB;

  struct json_tokener *tok = json_tokener_new();
  defer({ json_tokener_free(tok); };);
  struct json_object *json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({ json_object_put(json_obj); };);
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
  struct json_object *version;
  if (!json_object_object_get_ex(json_obj, "version", &version))
    return false;
  const char *exp_ver = json_object_get_string(version);
  if (exp_ver && (strcmp(exp_ver, AMARISOFT_SUPPORTED_VERSION)))
    lwsl_warn("Amarisoft version mismatch ! Supported one is %s, got instead %s\n", AMARISOFT_SUPPORTED_VERSION, exp_ver);

  if (!json_object_object_get_ex(key, "plmn", &plmn))
    return false;
  const char *p = json_object_get_string(plmn);
  if (!p)
    return false;
  const int plmn_val = atoi(p);
  out->plmn.mcc = plmn_val > 99999 ? plmn_val / 1000 : plmn_val / 100;
  out->plmn.mnc = plmn_val > 99999 ? plmn_val % 1000 : plmn_val % 100;
  out->plmn.mnc_digit_len = out->plmn.mnc > 99 ? 3 : 2;

  if (!json_object_object_get_ex(key, "gnb_id", &gnb_id))
    return false;
  out->nb_id = json_object_get_int(gnb_id);

  return true;
}

static bool json_decode_ctrl(const ran_msghdr_t *in_hdr, ctrl_ev_reply_t *out, const ws_ioloop_event_t *sent_ev)
{
  (void)sent_ev; // we do not use it for now, but good to keep in case you need correlation with data sent
  assert(out != NULL && "programming error\n");

  memset(out, 0, sizeof (ctrl_ev_reply_t));
  assert(out->ans.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
  switch (out->ans.ctrl_out.type)
  {
    case MAC_AGENT_IF_CTRL_ANS_V0:
      out->ans.ctrl_out.mac.ans = (in_hdr->error == true) ?  MAC_CTRL_OUT_KO : MAC_CTRL_OUT_OK;
      out->sm_id = SM_MAC_ID;
      break;
    case RLC_AGENT_IF_CTRL_ANS_V0:
    case PDCP_AGENT_IF_CTRL_ANS_V0:
    case SLICE_AGENT_IF_CTRL_ANS_V0:
    case TC_AGENT_IF_CTRL_ANS_V0:
    case GTP_AGENT_IF_CTRL_ANS_V0:
    default:
      lwsl_err("message unsupported %d\n", out->ans.type);
      assert (0!=0 && "programming error\n");
  }

  return true;
}

/* ----------------------------------------------------------------------------------- 
 *                                       Encoding
 * ----------------------------------------------------------------------------------- */
static const char *json_encode_ran_indication(int msg_id, int sm_id)
{
  (void)sm_id; // unused for the moment
  snprintf(global_buff,
           sizeof(global_buff),
           "[{\"message\":\"stats\",\"message_id\":\"%d\"},{\"message\":\"ue_get\",\"message_id\":\"%d\",\"stats\":1}]",
           msg_id, msg_id);
  return global_buff;
}

static const char *json_encode_ran_e2setup(int msg_id)
{
  snprintf(global_buff, sizeof(global_buff), "{\"message\":\"config_get\",\"message_id\":\"%d\"}", msg_id);
  return global_buff;
}


static const char *json_encode_ctrl_mac(int msg_id, mac_ctrl_req_data_t ctrl_msg)
{ 
   /* 
   * Example of an expected output from this function:
   * '{"message":"config_set","cells":[{cell_id:1,pusch_mcs:1}],"message_id":"1"}'
   *  cells: [
                {
                    "cell_id": 1,
                    "pusch_mcs": val
                }
                ]
    * where val: 
      sets the PUSCH MCS. It is an integer or array of 10 integers in case user wants to provide the PUSCH MCS for each of the 
      10 subframes it is composed of. Range is -1 to 28, where -1 means it not to force the MCS in a given frame/subframe.  
      Note that we will never use the -1 in input as we use instead the boolean isset_pusch_mcs to detect pusch_mcs presence.
      XXX-Limitation: for the moment, we do not check for range consistency in input data.
  */
  
  /* 
   * Caveats: 
   * we can't use json_object_new_array_ext(ctrl_msg.msg.ran_conf_len) as it is implemented in json-c just starting from version 0.15
   * See https://github.com/json-c/json-c/blob/master/ChangeLog
   */
  json_object *config_set_params = json_object_new_array();
  defer({ json_object_put(config_set_params); };);

  for (size_t i=0; i<ctrl_msg.msg.ran_conf_len; i++)
  {
    if (ctrl_msg.msg.ran_conf[i].isset_pusch_mcs)
    {
      json_object *cell = json_object_new_object();  
      if (json_object_object_add(cell, "cell_id", json_object_new_int(ctrl_msg.msg.ran_conf[i].rnti)))
        lwsl_err("error serializing pusch_mcs\n");
      if (json_object_object_add(cell, "pusch_mcs", json_object_new_int(ctrl_msg.msg.ran_conf[i].pusch_mcs)))
        lwsl_err("error serializing pusch_mcs\n");
      (void)json_object_array_add(config_set_params, cell);
    }
  }

  int rc = snprintf(global_buff,
                    sizeof(global_buff), 
                    "{\"message\":\"config_set\",\"cells\":%s,\"message_id\":\"%d\"}", 
                    json_object_to_json_string_ext(config_set_params, JSON_C_TO_STRING_PLAIN), 
                    msg_id); 
  if (rc >= (int)sizeof(global_buff))
  {
    lwsl_err("hit hard limit on internal buffer for command 'config_set'\n");
    return NULL;
  }
  
  return global_buff;
}

static const char *json_encode_ctrl(int msg_id, sm_ag_if_wr_t write_msg) 
{
 /* 
  * caveats: we go only or a modified MAC service model, i.e. like its implementation by CCC, 
  * instead of the SLICE because that one has no parameter that could be mapped into an Amarisoft websocket
  * parameter.
  */
  return (write_msg.type == CONTROL_SM_AG_IF_WR) && (write_msg.ctrl.type == MAC_CTRL_REQ_V0) ?
          json_encode_ctrl_mac(msg_id, write_msg.ctrl.mac_ctrl) :
          NULL; 
}

/*--------------------------------------------- API ----------------------------------------*/
ran_ser_abs_t json_ran_ser = {
    .get_ran_msghdr         = &json_get_ran_msghdr,
    .free_ran_msghdr        = &json_free_ran_msghdr,

    .decode_indication_stats        = &json_decode_ran_indication_stats,
    .decode_indication_ue_get       = &json_decode_ran_indication_ue_get,
    .decode_indication_config_get   = &json_decode_ran_indication_config_get,
    .decode_indication_qos_flow_get = &json_decode_ran_indication_qos_flow_get,
    .decode_indication_ue_erab_get  = &json_decode_ran_indication_erab_get,
    .decode_e2setup         = &json_decode_ran_e2setup,
    .decode_ctrl            = &json_decode_ctrl,

    .encode_e2setup         = &json_encode_ran_e2setup,
    .encode_indication      = &json_encode_ran_indication,    
    .encode_ctrl            = &json_encode_ctrl
};

ran_ser_abs_t *ran_ser_get_instance(void) { return &json_ran_ser; }