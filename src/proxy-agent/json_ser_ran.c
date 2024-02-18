/* 
 * Serialization in json.
 * 
 * For the encoding part, we mainly do not use the third party library json-c as the data structure we need to 
 * encode is most of times very simple.
 * Examples of some json messages to be decoded are available in test/proxy_agent directory
 * 
 * Compliance to Amarisoft API in retrieval data: functions may have difference compliance levels to versions of 
 * Amarisoft API, i.e. up to a certain kind of version of Amarisoft. See "Compliance" section in the documentation 
 * of each function below.
 */
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <libwebsockets.h>

#include "util/conversions.h"
#include "util/alg_ds/alg/defer.h"
#include "lib/e2ap/e2ap_global_node_id_wrapper.h"
#include "sm/mac_sm/mac_sm_id.h"
#include "sm/rc_sm/rc_sm_id.h"
#include "sm/rc_sm/ie/ir/ran_param_list.h"

#include "ran_if.h"
#include "ran_msg_hdlr.h"
#include "ser_ran.h"
#include "notif_e2_ran.h"
#include "ringbuffer.h"
#include "ws_io_ran.h"

static char enc_gbuf[1024];
#define AMARISOFT_SUPPORTED_VERSION "2021-12-17"
static char *LOG_MODULE_STR = "json parsing";

struct amr_fields_t {
  const char *name;         // JSON field name
  enum {AMR_DT_INT,
        AMR_DT_INT64,
        AMR_DT_DOUBLE,
        AMR_DT_BOOL, 
        AMR_DT_STR} dtype;  // JSON datatype. Note that Enumerations mentioned in amarisoft specs can be mapped to JSON strings or integers
  void *dst_container;      // address of destination storage
  bool *dst_container_flag; // destination storage presence flag to signal if the record has been found
};

/* --------------------------------------------------------------------------------------- */

/* Conversion routines from Amarisoft representation (string) to 3GPP one (plmn_t) */
static e2sm_plmn_t conv_plmnstr_to_e2sm_plmnt(const char *src)
{
  e2sm_plmn_t ret;
  const int plmn_val = atoi(src);

  ret.mcc = plmn_val > 99999 ? plmn_val / 1000 : plmn_val / 100;
  ret.mnc = plmn_val > 99999 ? plmn_val % 1000 : plmn_val % 100;
  ret.mnc_digit_len = ret.mnc > 99 ? 3 : 2;

  return ret;
}

/* Conversion routines from Amarisoft representation (string) to 3GPP one (plmn_t) */
static e2ap_plmn_t conv_plmnstr_to_e2ap_plmnt(const char *src)
{
  e2ap_plmn_t ret;
  const int plmn_val = atoi(src);

  ret.mcc = plmn_val > 99999 ? plmn_val / 1000 : plmn_val / 100;
  ret.mnc = plmn_val > 99999 ? plmn_val % 1000 : plmn_val % 100;
  ret.mnc_digit_len = ret.mnc > 99 ? 3 : 2;

  return ret;
}


/* 
 * Extract from the json object VAL all the members declared in table TBL and put them in the storage declared 
 * in TBL associated field. The absence of member is signalled int the associated flag declared in TBL (if different from NULL).
 * Members need to have one of the JSON types: string, number, boolean; json objects, array and null are not 
 * supported yet. In case of member of type string, the storage will be allocated via strdup and referenced in the 
 * storage pointer associated in the table.
 */
static void parse_amarisoft_fields_unnested(const struct json_object *val, struct amr_fields_t *tbl, size_t tbl_len)
{
  for (size_t i = 0; i<tbl_len;  i++)
  {  
    struct json_object *cell_field_tmp;
      
    if (json_object_object_get_ex(val, tbl[i].name, &cell_field_tmp))
    {
      if (tbl[i].dst_container_flag)
        *tbl[i].dst_container_flag = true;
      double *pdouble;
      int *pint;
      int64_t *pint64;
      bool *pbool;
      char **pstr;
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
        case AMR_DT_INT64:
          pint64 = (int64_t *)tbl[i].dst_container;
          *pint64 = json_object_get_int64(cell_field_tmp);
          break;
        case AMR_DT_BOOL:
          pbool = (bool *)tbl[i].dst_container;
          *pbool = json_object_get_boolean(cell_field_tmp);
          break;
        case AMR_DT_STR:
          pstr = (char **)tbl[i].dst_container;
          *pstr = strdup(json_object_get_string(cell_field_tmp));
          break;
        default:
          assert(0!=0 && " unsupported static datatype declaration\n");
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
  // lws logging system uses a hardlimit of 256 chars for truncation, so output below may be truncated.
  lwsl_debug("%s: Dumping json data %s:\n",
              LOG_MODULE_STR,
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
  ret_msg.error = false; // we avoid getting the exact reason of the error as we do not use it

  return ret_msg;
}

/* ----------------------------------------------------------------------------------- 
 *                                       Decoding
 * ----------------------------------------------------------------------------------- */
/*
 * Parsing reply to Amarisoft API command 'stats'. 
 *
 * Compliance: partial to Amarisoft API version 2023.06.10. Some fields have been omitted 
 * as not used by indication message; see the comments inside the function code for its 
 * full list
 * 
 * Perf: see benchmarking in test directory
 */
static bool json_decode_ran_indication_stats(const ran_msg_t *in_msg, ran_ind_t *out)
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

  struct json_object *gtp_tx_bitrate;
  if (json_object_object_get_ex(root_json_obj, "gtp_tx_bitrate", &gtp_tx_bitrate))
  {
    if (out->ran_stats_flags.gtp_tx_bitrate)
      out->ran_stats_flags.gtp_tx_bitrate = true;
    out->ran_stats.gtp_tx_bitrate = json_object_get_double(gtp_tx_bitrate);
  }
  struct json_object *gtp_rx_bitrate;
  if (json_object_object_get_ex(root_json_obj, "gtp_rx_bitrate", &gtp_rx_bitrate))
  {
    if (out->ran_stats_flags.gtp_rx_bitrate)
      out->ran_stats_flags.gtp_rx_bitrate = true;
    out->ran_stats.gtp_rx_bitrate = json_object_get_double(gtp_rx_bitrate);
  }

  struct json_object *cells;
  if (!json_object_object_get_ex(root_json_obj, "cells", &cells))
    return false;
  out->ran_stats.len_cell = json_object_object_length(cells);
  if (out->ran_stats.len_cell <= 0)
    return false;

  int idx = 0;
  json_object_object_foreach(cells, key, val)
  {
    struct amr_fields_t amr_cell_fields_tbl[] = {
      {"dl_bitrate"         , AMR_DT_DOUBLE , &out->ran_stats.cells[idx].dl_bitrate     , &out->ran_stats_flags.cells_stats_flags[idx].dl_bitrate},
      {"ul_bitrate"         , AMR_DT_DOUBLE , &out->ran_stats.cells[idx].ul_bitrate     , &out->ran_stats_flags.cells_stats_flags[idx].ul_bitrate},
      {"mbms_bitrate"       , AMR_DT_DOUBLE , &out->ran_stats.cells[idx].mbms_bitrate   , &out->ran_stats_flags.cells_stats_flags[idx].mbms_bitrate},
      {"ue_count_avg"       , AMR_DT_INT    , &out->ran_stats.cells[idx].ue_count_avg   , &out->ran_stats_flags.cells_stats_flags[idx].ue_count_avg},
      {"dl_tx"              , AMR_DT_INT    , &out->ran_stats.cells[idx].dl_tx          , &out->ran_stats_flags.cells_stats_flags[idx].dl_tx},
      {"ul_tx"              , AMR_DT_INT    , &out->ran_stats.cells[idx].ul_tx          , &out->ran_stats_flags.cells_stats_flags[idx].ul_tx},
      {"dl_err"             , AMR_DT_INT    , &out->ran_stats.cells[idx].dl_err         , &out->ran_stats_flags.cells_stats_flags[idx].dl_err},
      {"ul_err"             , AMR_DT_INT    , &out->ran_stats.cells[idx].ul_err         , &out->ran_stats_flags.cells_stats_flags[idx].ul_err},
      {"dl_retx"            , AMR_DT_INT    , &out->ran_stats.cells[idx].dl_retx        , &out->ran_stats_flags.cells_stats_flags[idx].dl_retx},
      {"ul_retx"            , AMR_DT_INT    , &out->ran_stats.cells[idx].ul_retx        , &out->ran_stats_flags.cells_stats_flags[idx].ul_retx},
      {"dl_sched_users_min" , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_sched_users_min, &out->ran_stats_flags.cells_stats_flags[idx].dl_sched_users_min},
      {"dl_sched_users_avg" , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_sched_users_avg, &out->ran_stats_flags.cells_stats_flags[idx].dl_sched_users_avg},
      {"dl_sched_users_max" , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_sched_users_max, &out->ran_stats_flags.cells_stats_flags[idx].dl_sched_users_max},
      {"ul_sched_users_min" , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_sched_users_min, &out->ran_stats_flags.cells_stats_flags[idx].ul_sched_users_min},
      {"ul_sched_users_avg" , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_sched_users_avg, &out->ran_stats_flags.cells_stats_flags[idx].ul_sched_users_avg},
      {"ul_sched_users_max" , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_sched_users_max, &out->ran_stats_flags.cells_stats_flags[idx].ul_sched_users_max},
      {"dl_use_min"         , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_use_min     , &out->ran_stats_flags.cells_stats_flags[idx].dl_use_min},
      {"dl_use_avg"         , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_use_avg     , &out->ran_stats_flags.cells_stats_flags[idx].dl_use_avg},
      {"dl_use_max"         , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_use_max     , &out->ran_stats_flags.cells_stats_flags[idx].dl_use_max},
      {"ul_use_min"         , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_use_min     , &out->ran_stats_flags.cells_stats_flags[idx].ul_use_min},
      {"ul_use_avg"         , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_use_avg     , &out->ran_stats_flags.cells_stats_flags[idx].ul_use_avg},
      {"ul_use_max"         , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_use_max     , &out->ran_stats_flags.cells_stats_flags[idx].ul_use_max},
      {"ctrl_use_min"       , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ctrl_use_min   , &out->ran_stats_flags.cells_stats_flags[idx].ctrl_use_min},
      {"ctrl_use_avg"       , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ctrl_use_avg   , &out->ran_stats_flags.cells_stats_flags[idx].ctrl_use_avg},
      {"ctrl_use_max"       , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ctrl_use_max   , &out->ran_stats_flags.cells_stats_flags[idx].ctrl_use_max},
      {"ue_count_min"       , AMR_DT_INT    , &out->ran_stats.cells[idx].ue_count_min   , &out->ran_stats_flags.cells_stats_flags[idx].ue_count_min},
      {"ue_count_max"       , AMR_DT_INT    , &out->ran_stats.cells[idx].ue_count_max   , &out->ran_stats_flags.cells_stats_flags[idx].ue_count_max},
      {"ue_count_avg"       , AMR_DT_INT    , &out->ran_stats.cells[idx].ue_count_avg   , &out->ran_stats_flags.cells_stats_flags[idx].ue_count_avg},
      {"erab_count_min"     , AMR_DT_INT    , &out->ran_stats.cells[idx].erab_count_min , &out->ran_stats_flags.cells_stats_flags[idx].erab_count_min},
      {"erab_count_max"     , AMR_DT_INT    , &out->ran_stats.cells[idx].erab_count_max , &out->ran_stats_flags.cells_stats_flags[idx].erab_count_max},
      {"erab_count_avg"     , AMR_DT_INT    , &out->ran_stats.cells[idx].erab_count_avg , &out->ran_stats_flags.cells_stats_flags[idx].erab_count_avg},
      {"drb_count_min"      , AMR_DT_INT    , &out->ran_stats.cells[idx].drb_count_min  , &out->ran_stats_flags.cells_stats_flags[idx].drb_count_min},
      {"drb_count_max"      , AMR_DT_INT    , &out->ran_stats.cells[idx].drb_count_max  , &out->ran_stats_flags.cells_stats_flags[idx].drb_count_max},
      {"drb_count_avg"      , AMR_DT_INT    , &out->ran_stats.cells[idx].drb_count_avg  , &out->ran_stats_flags.cells_stats_flags[idx].drb_count_avg},
      {"dl_gbr_use_min"     , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_gbr_use_min , &out->ran_stats_flags.cells_stats_flags[idx].dl_gbr_use_min},
      {"dl_gbr_use_avg"     , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_gbr_use_avg , &out->ran_stats_flags.cells_stats_flags[idx].dl_gbr_use_avg},
      {"dl_gbr_use_max"     , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].dl_gbr_use_max , &out->ran_stats_flags.cells_stats_flags[idx].dl_gbr_use_max},
      {"ul_gbr_use_min"     , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_gbr_use_min , &out->ran_stats_flags.cells_stats_flags[idx].ul_gbr_use_min},
      {"ul_gbr_use_avg"     , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_gbr_use_avg , &out->ran_stats_flags.cells_stats_flags[idx].ul_gbr_use_avg},
      {"ul_gbr_use_max"     , AMR_DT_DOUBLE,  &out->ran_stats.cells[idx].ul_gbr_use_max , &out->ran_stats_flags.cells_stats_flags[idx].ul_gbr_use_max}
    };

    out->ran_stats.cells[idx].cell_id = atoi(key);
    out->ran_stats_flags.cells_stats_flags[idx].cell_id = true;
    parse_amarisoft_fields_unnested(val, 
                                    amr_cell_fields_tbl, 
                                    sizeof(amr_cell_fields_tbl)/sizeof (struct amr_fields_t));
    
    idx++;
    if (idx > AMARISOFT_MAX_CELL_NUM)
    {
      lwsl_err("%s: hit hard limit on internal buffer\n", LOG_MODULE_STR);
      return false;
    }
  }
  
  // Omitted the following fields: cpu, instance_id, counters, gtp_tx_bitrate, gtp_rx_bitrate, rf_ports, samples, duration

  return true;
}

/* 
 * Parsing reply to Amarisoft API command 'ue_get'
 * 
 * Compliance: full up to Amarisoft API version 2023.06.10 
 * 
 * Perf: see benchmarking in test directory
 */
static bool json_decode_ran_indication_ue_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
  assert((out != NULL || in_msg != NULL) && "programming error\n");

  struct json_tokener *tok = json_tokener_new();
  defer({ json_tokener_free(tok); };);
  struct json_object *root_json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({ json_object_put(root_json_obj); };);

  if (root_json_obj == NULL)
  {
    lwsl_err("%s: error json token: %s\n", LOG_MODULE_STR, json_tokener_error_desc(json_tokener_get_error(tok)));
    return false;
  }
  
  // parse array of objects `ue_list`
  struct json_object *ue_list;
  if (!json_object_object_get_ex(root_json_obj, "ue_list", &ue_list))
    return false;
  out->len_ue_stats = json_object_array_length(ue_list);
  //if (out->len_ue_stats <= 0)
  //  return false;
  // TODO: if we do calloc for ue_stats in here, where should we free the memory

  for (size_t idx = 0; idx < out->len_ue_stats; idx++)
  {
    struct amr_fields_t amr_ue_list_fields_tbl[] = {
        {"time"             , AMR_DT_INT, &out->ue_stats[idx].time            , NULL},
        {"enb_ue_id"        , AMR_DT_INT, &out->ue_stats[idx].enb_ue_id       , NULL},
        {"ran_ue_id"        , AMR_DT_INT, &out->ue_stats[idx].ran_ue_id       , NULL},
        {"mme_ue_id"        , AMR_DT_INT, &out->ue_stats[idx].mme_ue_id       , NULL},
        {"amf_ue_id"        , AMR_DT_INT, &out->ue_stats[idx].amf_ue_id       , NULL},
        {"linked_enb_ue_id" , AMR_DT_INT, &out->ue_stats[idx].linked_enb_ue_id, NULL},
        {"linked_ran_ue_id" , AMR_DT_INT, &out->ue_stats[idx].linked_ran_ue_id, NULL},
        {"rnti"             , AMR_DT_INT, &out->ue_stats[idx].rnti            , NULL}
      };
    parse_amarisoft_fields_unnested(json_object_array_get_idx(ue_list, idx), 
                                    amr_ue_list_fields_tbl, 
                                    sizeof(amr_ue_list_fields_tbl)/sizeof (struct amr_fields_t));

    // parse array of objects `cells`
    struct json_object *cells;
    if (!json_object_object_get_ex(json_object_array_get_idx(ue_list, idx), "cells", &cells))
      return false;
    out->ue_stats[idx].len_cell = json_object_array_length(cells);;
    assert (out->ue_stats[idx].len_cell < AMARISOFT_MAX_CELL_NUM && "hard limit on number of cell hit\n");
    for (size_t j = 0; j < out->ue_stats[idx].len_cell; j++)
    {
      struct amr_fields_t amr_cell_fields_tbl[] = {
        {"cell_id"          , AMR_DT_INT    , &out->ue_stats[idx].cells[j].cell_id          , &out->ue_stats[idx].cell_flags[j].cell_id},
        {"cqi"              , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].cqi              , &out->ue_stats[idx].cell_flags[j].cqi},
        {"ri"               , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].ri               , &out->ue_stats[idx].cell_flags[j].ri},
        {"ul_rank"          , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].ul_rank          , &out->ue_stats[idx].cell_flags[j].ul_rank},
        {"dl_bitrate"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].dl_bitrate       , &out->ue_stats[idx].cell_flags[j].dl_bitrate},
        {"ul_bitrate"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].ul_bitrate       , &out->ue_stats[idx].cell_flags[j].ul_bitrate},
        {"dl_tx"            , AMR_DT_INT    , &out->ue_stats[idx].cells[j].dl_tx            , &out->ue_stats[idx].cell_flags[j].dl_tx},
        {"ul_tx"            , AMR_DT_INT    , &out->ue_stats[idx].cells[j].ul_tx            , &out->ue_stats[idx].cell_flags[j].ul_tx},
        {"dl_retx"          , AMR_DT_INT    , &out->ue_stats[idx].cells[j].dl_retx          , &out->ue_stats[idx].cell_flags[j].dl_retx},
        {"ul_retx"          , AMR_DT_INT    , &out->ue_stats[idx].cells[j].ul_retx          , &out->ue_stats[idx].cell_flags[j].ul_retx},
        {"dl_mcs"           , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].dl_mcs           , &out->ue_stats[idx].cell_flags[j].dl_mcs},
        {"ul_mcs"           , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].ul_mcs           , &out->ue_stats[idx].cell_flags[j].ul_mcs},
        {"ul_n_layer"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].ul_n_layer       , &out->ue_stats[idx].cell_flags[j].ul_n_layer},
        {"turbo_decoder_min", AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].turbo_decoder_min, &out->ue_stats[idx].cell_flags[j].turbo_decoder_min},
        {"turbo_decoder_avg", AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].turbo_decoder_avg, &out->ue_stats[idx].cell_flags[j].turbo_decoder_avg},
        {"turbo_decoder_max", AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].turbo_decoder_max, &out->ue_stats[idx].cell_flags[j].turbo_decoder_max},
        {"pucch1_snr"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].pucch1_snr       , &out->ue_stats[idx].cell_flags[j].pucch1_snr},
        {"pusch_snr"        , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].pusch_snr        , &out->ue_stats[idx].cell_flags[j].pusch_snr},
        {"ul_phr"           , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].ul_phr           , &out->ue_stats[idx].cell_flags[j].ul_phr},
        {"epre"             , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].epre             , &out->ue_stats[idx].cell_flags[j].epre},
        {"ul_path_loss"     , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].ul_path_loss     , &out->ue_stats[idx].cell_flags[j].ul_path_loss},
        {"p_ue"             , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].p_ue             , &out->ue_stats[idx].cell_flags[j].p_ue},
        {"initial_ta"       , AMR_DT_DOUBLE , &out->ue_stats[idx].cells[j].initial_ta       , &out->ue_stats[idx].cell_flags[j].initial_ta}
      };
      parse_amarisoft_fields_unnested(json_object_array_get_idx(cells, j), 
                                      amr_cell_fields_tbl, 
                                      sizeof(amr_cell_fields_tbl)/sizeof (struct amr_fields_t));
    }

    if (idx > AMARISOFT_MAX_UE_NUM)
    {
      lwsl_err("%s: hit hard limit on internal buffer\n", LOG_MODULE_STR);
      return false;
    }
   
  // parse array of objects `erabs` (Set for LTE and NB-IoT UEs)
    struct json_object *erabs;
    if (json_object_object_get_ex(json_object_array_get_idx(ue_list, idx), "erabs", &erabs))
    {
      out->ue_stats[idx].len_erab = json_object_array_length(erabs);
      assert (out->ue_stats[idx].len_erab < AMARISOFT_MAX_QOS_NUM && "hard limit on number of ERABS hit\n");

      for (size_t j = 0; j < out->ue_stats[idx].len_erab; j++)
      {
        struct amr_fields_t amr_erabs_fields_tbl[] = {
          {"erab_id"       , AMR_DT_DOUBLE, &out->ue_stats[idx].erabs[j].erab_id    , &out->ue_stats[idx].erabs_flags[j].erab_id},
          {"qci"           , AMR_DT_DOUBLE, &out->ue_stats[idx].erabs[j].qci        , &out->ue_stats[idx].erabs_flags[j].qci},
          {"dl_total_bytes", AMR_DT_INT, &out->ue_stats[idx].erabs[j].dl_total_bytes, &out->ue_stats[idx].erabs_flags[j].dl_total_bytes},
          {"ul_total_bytes", AMR_DT_INT, &out->ue_stats[idx].erabs[j].ul_total_bytes, &out->ue_stats[idx].erabs_flags[j].ul_total_bytes}
        };
        parse_amarisoft_fields_unnested(json_object_array_get_idx(erabs, j), 
                                        amr_erabs_fields_tbl, 
                                        sizeof(amr_erabs_fields_tbl)/sizeof (struct amr_fields_t));
      }
    }

  // parse array of objects `qos_flows` (Set for NR UEs)
    struct json_object *qos_flows;
    if (json_object_object_get_ex(json_object_array_get_idx(ue_list, idx), "qos_flow_list", &qos_flows))
    {
      out->ue_stats[idx].len_qos_flow = json_object_array_length(qos_flows);
      assert (out->ue_stats[idx].len_qos_flow < AMARISOFT_MAX_QOS_NUM && "hard limit on number of QOS hit\n");

      for (size_t j = 0; j < out->ue_stats[idx].len_qos_flow; j++)
      {
        struct json_object *cur_qos = json_object_array_get_idx(qos_flows, j);
        struct amr_fields_t amr_qos_fields_tbl[] = {
          {"pdu_session_id" , AMR_DT_INT, &out->ue_stats[idx].qos_flows[j].pdu_session_id, &out->ue_stats[idx].qos_flows_flags[j].pdu_session_id},
          {"sst"            , AMR_DT_INT, &out->ue_stats[idx].qos_flows[j].sst           , &out->ue_stats[idx].qos_flows_flags[j].sst},
          {"dl_total_bytes" , AMR_DT_INT64, &out->ue_stats[idx].qos_flows[j].dl_total_bytes, &out->ue_stats[idx].qos_flows_flags[j].dl_total_bytes},
          {"ul_total_bytes" , AMR_DT_INT64, &out->ue_stats[idx].qos_flows[j].ul_total_bytes, &out->ue_stats[idx].qos_flows_flags[j].ul_total_bytes}
        };
        parse_amarisoft_fields_unnested(cur_qos,
                                        amr_qos_fields_tbl, 
                                        sizeof(amr_qos_fields_tbl)/sizeof (struct amr_fields_t));
      }
    }
  }
  return true;
}

/* 
 * Parsing reply to Amarisoft API command 'config_get'.
 * Some malloced memory is allocated by this function in OUT parameter if needed that would need to be freed by free_ran_config()
 * It return FALSE if some compulsory fields are not found (version, global_gnb_id, nr_cells), TRUE otherwise
 * 
 * Compliance: partial to Amarisoft API version 2023.06.10. Some fields have been omitted; see the comments inside 
 * this function code for its full list.
 * 
 * Perf: see benchmarking in test directory
 */
static bool json_decode_ran_config_get(const ran_msg_t *in_msg, ran_config_t *out_ran, ran_ind_t *out_ind)
{
  assert((in_msg != NULL) && "programming error\n");
  ran_config_t *out = NULL;
  if (out_ind != NULL){
      out = &out_ind->ran_config;
  } else {
      out = out_ran;
  }

  memset (&out->nr_cells_flag, 0, sizeof (out->nr_cells_flag)); //reset to false all the presence flags

  struct json_tokener *tok = json_tokener_new();
  defer({ json_tokener_free(tok); };);
  struct json_object *root_json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({ json_object_put(root_json_obj); };);

  struct json_object *version;
  if (!json_object_object_get_ex(root_json_obj, "version", &version))
    return false;
  // TODO: Fix API version
  // const char *exp_ver = json_object_get_string(version);
//  if (exp_ver && (strcmp(exp_ver, AMARISOFT_SUPPORTED_VERSION)))
//    lwsl_warn("%s: Amarisoft version mismatch ! Supported one is %s, got instead %s\n",
//              LOG_MODULE_STR,
//              AMARISOFT_SUPPORTED_VERSION,
//              exp_ver);

  // TODO: omitted `logs`
  // TODO: omitted `tai`
  // TODO: omitted `global_enb_id`

  // parse object `global_gnb_id`
  out->global_e2_node_id.cu_du_id = NULL;
  out->global_e2_node_id.nb_id.nb_id = 0;
  out->global_e2_node_id.nb_id.unused = 0;
  out->global_e2_node_id.plmn.mcc = 0;
  out->global_e2_node_id.plmn.mnc = 0;
  out->global_e2_node_id.plmn.mnc_digit_len = 0;
  out->global_e2_node_id.type = e2ap_ngran_gNB;
  struct json_object *item, *plmn, *gnb_id;
  if (!json_object_object_get_ex(root_json_obj, "global_gnb_id", &item))
    return false;
  
  if (!json_object_object_get_ex(item, "plmn", &plmn))
    return false;
  const char *p = json_object_get_string(plmn);
  if (!p)
    return false;
  out->global_e2_node_id.plmn = conv_plmnstr_to_e2ap_plmnt(p);

  if (!json_object_object_get_ex(item, "gnb_id", &gnb_id))
    return false;

  out->global_e2_node_id.nb_id.nb_id = json_object_get_int(gnb_id);
  out->global_e2_node_id.nb_id.unused = 0;

  // TODO: omitted `cells`
  // TODO: omitted `nb_cells`

  // parse object 'nr_cells'
  struct json_object *cells;
  if (!json_object_object_get_ex(root_json_obj, "nr_cells", &cells))
    return false;
  struct json_object* nr_cells = json_object_object_get(root_json_obj, "nr_cells");
  out->len_nr_cell = json_object_object_length(nr_cells);
  assert (out->len_nr_cell < AMARISOFT_MAX_CELL_NUM && "hard limit on number of secondary cells hit\n");

  size_t idx = 0;
  json_object_object_foreach(cells, key, val)
  {
    // parse the plain items
    struct amr_fields_t amr_nr_cell_fields_tbl[] = {
      {"n_antenna_dl"         , AMR_DT_INT    , &out->nr_cells[idx].n_antenna_dl, &out->nr_cells_flag[idx].n_antenna_dl},
      {"n_antenna_ul"         , AMR_DT_INT    , &out->nr_cells[idx].n_antenna_ul, &out->nr_cells_flag[idx].n_antenna_ul},
      {"n_layer_dl"           , AMR_DT_INT    , &out->nr_cells[idx].n_layer_dl  , &out->nr_cells_flag[idx].n_layer_dl},
      {"n_layer_ul"           , AMR_DT_INT    , &out->nr_cells[idx].n_layer_ul  , &out->nr_cells_flag[idx].n_layer_ul},
      {"gain"                 , AMR_DT_DOUBLE , &out->nr_cells[idx].gain        , &out->nr_cells_flag[idx].gain},
      {"ul_disabled"          , AMR_DT_BOOL   , &out->nr_cells[idx].ul_disabled , &out->nr_cells_flag[idx].ul_disabled},
      {"rf_port"              , AMR_DT_INT    , &out->nr_cells[idx].rf_port     , &out->nr_cells_flag[idx].rf_port},
      {"label"                , AMR_DT_STR    , &out->nr_cells[idx].label       , &out->nr_cells_flag[idx].label},
      {"dl_qam"               , AMR_DT_INT    , &out->nr_cells[idx].dl_qam      , &out->nr_cells_flag[idx].dl_qam},
      {"ul_qam"               , AMR_DT_INT    , &out->nr_cells[idx].ul_qam      , &out->nr_cells_flag[idx].ul_qam},
      {"n_id_nrcell"          , AMR_DT_INT    , &out->nr_cells[idx].n_id_nrcell , &out->nr_cells_flag[idx].n_id_nrcell},
      {"band"                 , AMR_DT_INT    , &out->nr_cells[idx].band        , &out->nr_cells_flag[idx].band},
      {"dl_nr_arfcn"          , AMR_DT_INT    , &out->nr_cells[idx].dl_nr_arfcn , &out->nr_cells_flag[idx].dl_nr_arfcn},
      {"ul_nr_arfcn"          , AMR_DT_INT    , &out->nr_cells[idx].ul_nr_arfcn , &out->nr_cells_flag[idx].ul_nr_arfcn},
      {"n_rb_dl"              , AMR_DT_INT    , &out->nr_cells[idx].n_rb_dl     , &out->nr_cells_flag[idx].n_rb_dl},
      {"n_rb_ul"              , AMR_DT_INT    , &out->nr_cells[idx].n_rb_ul     , &out->nr_cells_flag[idx].n_rb_ul},
      {"ssb_nr_arfcn"         , AMR_DT_INT    , &out->nr_cells[idx].ssb_nr_arfcn, &out->nr_cells_flag[idx].ssb_nr_arfcn},
      {"dl_mu"                , AMR_DT_INT    , &out->nr_cells[idx].dl_mu       , &out->nr_cells_flag[idx].dl_mu},
      {"ul_mu"                , AMR_DT_INT    , &out->nr_cells[idx].ul_mu       , &out->nr_cells_flag[idx].ul_mu},
      {"ssb_mu"               , AMR_DT_INT    , &out->nr_cells[idx].ssb_mu      , &out->nr_cells_flag[idx].ssb_mu},
      {"mode"                 , AMR_DT_STR    , &out->nr_cells[idx].mode        , &out->nr_cells_flag[idx].mode},
      {"prach_sequence_index" , AMR_DT_INT    , &out->nr_cells[idx].prach_sequence_index, &out->nr_cells_flag[idx].prach_sequence_index},
    };
    out->nr_cells[idx].cell_id = atoi(key);
    parse_amarisoft_fields_unnested(val, 
                                    amr_nr_cell_fields_tbl, 
                                    sizeof(amr_nr_cell_fields_tbl)/sizeof (struct amr_fields_t));
    
    // TODO: parse object `gbr`

    // parse object `ncgi`
    struct json_object *ncgi_plmn, *ncgi_id;
    if (json_object_object_get_ex(val, "ncgi", &item))
    {
      if (json_object_object_get_ex(item, "plmn", &ncgi_plmn))
        out->nr_cells[idx].ncgi.plmn_id = conv_plmnstr_to_e2sm_plmnt(json_object_get_string(ncgi_plmn));
      if (json_object_object_get_ex(item, "nci", &ncgi_id))
        out->nr_cells[idx].ncgi.nr_cell_id = json_object_get_int(gnb_id); // TODO: this is wrong
    }

    // parse optional object `connected_mobility`
    struct amr_fields_t amr_connected_mobility_tbl[] = {
      {"scell_config"         , AMR_DT_BOOL, &out->nr_cells[idx].connected_mobility.scell_config    , &out->nr_cells_flag[idx].connected_mobility.scell_config},
      {"nr_handover"          , AMR_DT_BOOL, &out->nr_cells[idx].connected_mobility.nr_handover     , &out->nr_cells_flag[idx].connected_mobility.nr_handover},
      {"nr_cell_redirect"     , AMR_DT_BOOL, &out->nr_cells[idx].connected_mobility.nr_cell_redirect, &out->nr_cells_flag[idx].connected_mobility.nr_cell_redirect},
      {"eutra_handover"       , AMR_DT_BOOL, &out->nr_cells[idx].connected_mobility.eutra_handover  , &out->nr_cells_flag[idx].connected_mobility.eutra_handover},
      {"eutra_cell_redirect"  , AMR_DT_BOOL, &out->nr_cells[idx].connected_mobility.eutra_cell_redirect, &out->nr_cells_flag[idx].connected_mobility.eutra_cell_redirect},
      {"nr_dc_setup"          , AMR_DT_BOOL, &out->nr_cells[idx].connected_mobility.nr_dc_setup     , &out->nr_cells_flag[idx].connected_mobility.nr_dc_setup},
      {"mr_dc_release"        , AMR_DT_BOOL, &out->nr_cells[idx].connected_mobility.mr_dc_release   , &out->nr_cells_flag[idx].connected_mobility.mr_dc_release}
    };
    parse_amarisoft_fields_unnested(val, 
                                    amr_connected_mobility_tbl, 
                                    sizeof(amr_connected_mobility_tbl)/sizeof (struct amr_fields_t));

    // parse optional array of objects `scell_list`
    if (json_object_object_get_ex(val, "scell_list", &item))
    {
      out->nr_cells[idx].len_scell = json_object_array_length(item);
      assert (out->nr_cells[idx].len_scell < AMARISOFT_MAX_SECONDARY_CELL_NUM && "hard limit on number of secondary cells hit\n");

      for (size_t scell_idx = 0; scell_idx < out->nr_cells[idx].len_scell; scell_idx++)
      {
        struct json_object *subitem;
        if (json_object_object_get_ex(json_object_array_get_idx(item, idx), "cell_id", &subitem))
          out->nr_cells[idx].scell_list[scell_idx].cell_id = json_object_get_int(subitem);
        if (json_object_object_get_ex(json_object_array_get_idx(item, idx), "ul_allowed", &subitem))
          out->nr_cells[idx].scell_list[scell_idx].ul_allowed = json_object_get_boolean(subitem);
        scell_idx++;
      }
    }  
    
    // parse optional array of objects `ncell_list`
    if (json_object_object_get_ex(val, "ncell_list", &item))
    {
      out->nr_cells[idx].len_ncell = json_object_array_length(item);
      assert (out->nr_cells[idx].len_ncell < AMARISOFT_MAX_SECONDARY_CELL_NUM && "hard limit on number of secondary cells hit\n");

      for (size_t ncell_idx = 0; ncell_idx < out->nr_cells[idx].len_ncell; ncell_idx++)
      {
        struct amr_fields_t amr_ncell_list_tbl[] = {
          {"rat", AMR_DT_STR, &out->nr_cells[idx].ncell_list[ncell_idx].rat, &out->nr_cells_flag[idx].ncell_list[ncell_idx].rat},
          {"dl_earfcn", AMR_DT_INT, &out->nr_cells[idx].ncell_list[ncell_idx].dl_earfcn, &out->nr_cells_flag[idx].ncell_list[ncell_idx].dl_earfcn},
          {"n_id_cell", AMR_DT_INT, &out->nr_cells[idx].ncell_list[ncell_idx].n_id_cell, &out->nr_cells_flag[idx].ncell_list[ncell_idx].n_id_cell},
          // TODO: ecgi
          {"ssb_nr_arfcn", AMR_DT_INT, &out->nr_cells[idx].ncell_list[ncell_idx].ssb_nr_arfcn, &out->nr_cells_flag[idx].ncell_list[ncell_idx].ssb_nr_arfcn},
          {"n_id_nrcell", AMR_DT_INT, &out->nr_cells[idx].ncell_list[ncell_idx].n_id_nrcell, &out->nr_cells_flag[idx].ncell_list[ncell_idx].n_id_nrcell},
          {"handover_target", AMR_DT_BOOL, &out->nr_cells[idx].ncell_list[ncell_idx].handover_target, &out->nr_cells_flag[idx].ncell_list[ncell_idx].handover_target},
          {"cell_redirect_target", AMR_DT_BOOL, &out->nr_cells[idx].ncell_list[ncell_idx].cell_redirect_target, &out->nr_cells_flag[idx].ncell_list[ncell_idx].cell_redirect_target},
          {"eps_fallback_target", AMR_DT_BOOL, &out->nr_cells[idx].ncell_list[ncell_idx].eps_fallback_target, &out->nr_cells_flag[idx].ncell_list[ncell_idx].eps_fallback_target},
          {"emergency_fallback_target", AMR_DT_BOOL, &out->nr_cells[idx].ncell_list[ncell_idx].emergency_fallback_target, &out->nr_cells_flag[idx].ncell_list[ncell_idx].emergency_fallback_target}
          };

        parse_amarisoft_fields_unnested(json_object_array_get_idx(item, ncell_idx), 
                                        amr_ncell_list_tbl, 
                                        sizeof(amr_ncell_list_tbl)/sizeof (struct amr_fields_t));
        struct json_object *cur_ncell = json_object_array_get_idx(item, ncell_idx);
        struct json_object *ncgi, *ncgi_plmn, *ncgi_id;
        if (json_object_object_get_ex(cur_ncell, "ncgi", &ncgi))
        {
          if (json_object_object_get_ex(ncgi, "plmn", &ncgi_plmn))
            out->nr_cells[idx].ncell_list[ncell_idx].ncgi.plmn_id = conv_plmnstr_to_e2sm_plmnt(json_object_get_string(ncgi_plmn));
          if (json_object_object_get_ex(ncgi, "nci", &ncgi_id))
            out->nr_cells[idx].ncell_list[ncell_idx].ncgi.nr_cell_id = json_object_get_int(gnb_id); // TODO: this is wrong
        }
      }
    }  

    // TODO: omitted optional array of objects `nr_dc_scg_cell_list`
    // TODO: omitted array of objects `plmn_list`
    
    idx++;
    if (idx > AMARISOFT_MAX_CELL_NUM)
    {
      lwsl_err("%s: hit hard limit on internal buffer\n", LOG_MODULE_STR);
      return false;
    }
  }
  // TODO: omitted array of objects `rx_channels`
  // TODO: omitted array of objects `tx_channels`
  // TODO: omitted array of objects `rf_ports`

  return true;
  
}

// parsing reply to Amarisoft API command 'qos_flow_get'.
static bool json_decode_ran_indication_qos_flow_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
  assert((out != NULL || in_msg != NULL) && "programming error\n");

  struct json_tokener *tok = json_tokener_new();
  defer({ json_tokener_free(tok); };);
  struct json_object *root_json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
  defer({ json_object_put(root_json_obj); };);

  struct json_object *item;
  // parse optional array of objects `qos_flow_list`
  if (json_object_object_get_ex(root_json_obj, "qos_flow_list", &item))
  {
      out->len_qos_flow_stats = json_object_array_length(item);
      assert (out->len_qos_flow_stats < AMARISOFT_MAX_UE_NUM && "hard limit on number of QOS list hit\n");

      for (size_t qos_flow_idx = 0; qos_flow_idx < out->len_qos_flow_stats; qos_flow_idx++)
      {
        struct amr_fields_t amr_qos_flow_list_tbl[] = {
          {"ran_ue_id", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].ran_ue_id, &out->qos_flow_stats_flags[qos_flow_idx].ran_ue_id},
          {"pdu_session_id", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].pdu_session_id, &out->qos_flow_stats_flags[qos_flow_idx].pdu_session_id},
          {"sst", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].sst, &out->qos_flow_stats_flags[qos_flow_idx].sst},
          {"sd", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].sd, &out->qos_flow_stats_flags[qos_flow_idx].sd},
          {"dl_total_bytes", AMR_DT_INT64, &out->qos_flow_stats[qos_flow_idx].dl_total_bytes, &out->qos_flow_stats_flags[qos_flow_idx].dl_total_bytes},
          {"ul_total_bytes", AMR_DT_INT64, &out->qos_flow_stats[qos_flow_idx].ul_total_bytes, &out->qos_flow_stats_flags[qos_flow_idx].ul_total_bytes},
        };

        parse_amarisoft_fields_unnested(json_object_array_get_idx(item, qos_flow_idx), 
                                        amr_qos_flow_list_tbl, 
                                        sizeof(amr_qos_flow_list_tbl)/sizeof (struct amr_fields_t));
        // Parse array of objects `qfi_list`
        struct json_object *qfi_list;
        if (json_object_object_get_ex(item, "qfi_list", &qfi_list))
        {
          out->qos_flow_stats[qos_flow_idx].len_qfi_list = json_object_array_length(qfi_list);
          assert (out->qos_flow_stats[qos_flow_idx].len_qfi_list < AMARISOFT_MAX_QOS_NUM && "hard limit on number of QOS list hit\n");

          for (size_t qfi_idx = 0; qfi_idx < out->qos_flow_stats[qos_flow_idx].len_qfi_list; qfi_idx++)
          {
              out->qos_flow_stats_flags[qos_flow_idx].qfi_list[qfi_idx].presence = true;
              struct amr_fields_t amr_qos_qfi_list_tbl[] = {
                {"qfi", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].qfi_list[qfi_idx].qfi, &out->qos_flow_stats_flags[qos_flow_idx].qfi_list[qfi_idx].qfi},
                {"5qi", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].qfi_list[qfi_idx]._5gi, &out->qos_flow_stats_flags[qos_flow_idx].qfi_list[qfi_idx]._5gi},
                {"dl_mbr", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].qfi_list[qfi_idx].dl_mbr, &out->qos_flow_stats_flags[qos_flow_idx].qfi_list[qfi_idx].dl_mbr},
                {"dl_gbr", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].qfi_list[qfi_idx].dl_gbr, &out->qos_flow_stats_flags[qos_flow_idx].qfi_list[qfi_idx].dl_gbr},
                {"ul_mbr", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].qfi_list[qfi_idx].ul_mbr, &out->qos_flow_stats_flags[qos_flow_idx].qfi_list[qfi_idx].ul_mbr},
                {"ul_gbr", AMR_DT_INT, &out->qos_flow_stats[qos_flow_idx].qfi_list[qfi_idx].ul_gbr, &out->qos_flow_stats_flags[qos_flow_idx].qfi_list[qfi_idx].ul_gbr}
            };
            parse_amarisoft_fields_unnested(json_object_array_get_idx(qfi_list, qfi_idx), 
                                           amr_qos_qfi_list_tbl, 
                                          sizeof(amr_qos_qfi_list_tbl)/sizeof (struct amr_fields_t));
          }
        }
      }
  }
  return true;
}

// parsing reply to Amarisoft API command 'erab_get'
static bool json_decode_ran_indication_erab_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
  assert(0!=0 && "not supported");
  //TODO
  assert((out != NULL || in_msg != NULL) && "programming error\n");

  return true;

}

static bool json_decode_ran_e2setup(const ran_msg_t *in_msg, ran_config_t *out)
{
  bool ret = json_decode_ran_config_get(in_msg, out, NULL);

//  if (ret == true)
//    *out = cp_global_e2_node_id(&conf.global_e2_node_id);
//
//  free_ran_config(&conf);
  
  return ret;
}

/*
 * Parsing reply to Amarisoft API command 'config_get'
 */
static bool json_decode_ran_indication_config_get(const ran_msg_t *in_msg, ran_ind_t *out)
{
    // TODO: Free config_get
    bool ret = json_decode_ran_config_get(in_msg, NULL, out);

    return ret;
}

static bool json_decode_ctrl(const ran_msghdr_t *in_hdr, ctrl_ev_reply_t *out, const ws_ioloop_event_t *sent_ev)
{
  (void)sent_ev; // we do not use it for now, but good to keep in case you need correlation with data sent
  assert(out != NULL && "programming error\n");

  memset(out, 0, sizeof (ctrl_ev_reply_t));
  assert(out->ans.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
    switch (out->ans.ctrl_out.type) {
        case RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0:
            // TODO: Finish the control out
            out->ans.ctrl_out.rc.format = FORMAT_1_E2SM_RC_CTRL_OUT; // 7.6.4.5
            out->sm_id = SM_RC_ID;
            break;
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
            lwsl_err("%s: message unsupported %d\n", LOG_MODULE_STR, out->ans.type);
            assert (0!=0 && "programming error\n");
    }
  return true;
}

/* ----------------------------------------------------------------------------------- 
 *                                       Encoding
 * ----------------------------------------------------------------------------------- */
static const char *json_encode_ran_indication(int msg_id, int sm_id, double initial_delay)
{
  (void)sm_id; // unused for the moment
  snprintf(enc_gbuf,
           sizeof(enc_gbuf),
           "[{\"message\":\"stats\",\"message_id\":\"%d\",\"initial_delay\": %.1f},"
           "{\"message\":\"ue_get\",\"message_id\":\"%d\",\"stats\":1},"
           "{\"message\":\"config_get\",\"message_id\":\"%d\"}]",
           msg_id, initial_delay, msg_id, msg_id);
  return enc_gbuf;
}

static const char *json_encode_ran_e2setup(int msg_id)
{
  snprintf(enc_gbuf, sizeof(enc_gbuf), "{\"message\":\"config_get\",\"message_id\":\"%d\"}", msg_id);
  return enc_gbuf;
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
        lwsl_err("%s: error serializing pusch_mcs\n", LOG_MODULE_STR);
      if (json_object_object_add(cell, "pusch_mcs", json_object_new_int(ctrl_msg.msg.ran_conf[i].pusch_mcs)))
        lwsl_err("%s: error serializing pusch_mcs\n", LOG_MODULE_STR);
      (void)json_object_array_add(config_set_params, cell);
    }
  }

  int rc = snprintf(enc_gbuf,
                    sizeof(enc_gbuf), 
                    "{\"message\":\"config_set\",\"cells\":%s,\"message_id\":\"%d\"}", 
                    json_object_to_json_string_ext(config_set_params, JSON_C_TO_STRING_PLAIN), 
                    msg_id); 
  if (rc >= (int)sizeof(enc_gbuf))
  {
    lwsl_err("%s: hit hard limit on internal buffer for command 'config_set'\n", LOG_MODULE_STR);
    return NULL;
  }
  
  return enc_gbuf;
}

static const char *json_encode_ctrl_rc(int msg_id, rc_ctrl_req_data_t ctrl_msg)
{
    char* pci;
    if (ctrl_msg.hdr.format == FORMAT_1_E2SM_RC_CTRL_HDR){
        if(ctrl_msg.hdr.frmt_1.ric_style_type == 3 && ctrl_msg.hdr.frmt_1.ctrl_act_id == Handover_control_7_6_4_1){
            e2sm_rc_ctrl_msg_frmt_1_t const *msg = &ctrl_msg.msg.frmt_1;
            assert(msg->sz_ran_param == 1 && "not support msg->sz_ran_param != 1");

            seq_ran_param_t *target_primary_cell_id = &msg->ran_param[0];
            assert(target_primary_cell_id->ran_param_id == Target_primary_cell_id_8_4_4_1 &&
                   "Wrong Target_primary_cell_id id ");
            assert(target_primary_cell_id->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE &&
                   "wrong Target_primary_cell_id type");
            assert(target_primary_cell_id->ran_param_val.strct != NULL &&
                   "NULL target_primary_cell_id->ran_param_val.strct");
            assert(target_primary_cell_id->ran_param_val.strct->sz_ran_param_struct == 1 &&
                   "wrong target_primary_cell_id->ran_param_val.strct->sz_ran_param_struct");
            assert(target_primary_cell_id->ran_param_val.strct->ran_param_struct != NULL &&
                   "NULL target_primary_cell_id->ran_param_val.strct->ran_param_struct");

            seq_ran_param_t *choice_target_cell = &target_primary_cell_id->ran_param_val.strct->ran_param_struct[0];
            assert(choice_target_cell->ran_param_id == CHOICE_target_cell_8_4_4_1 && "wrong CHOICE_target_cell id");
            assert(choice_target_cell->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE &&
                   "wrong CHOICE_target_cell type");
            assert(choice_target_cell->ran_param_val.strct != NULL && "NULL CHOICE_target_cell->ran_param_val.strct");
            assert(choice_target_cell->ran_param_val.strct->sz_ran_param_struct == 2 &&
                   "wrong CHOICE_target_cell->ran_param_val.strct->sz_ran_param_struct");
            assert(choice_target_cell->ran_param_val.strct->ran_param_struct != NULL &&
                   "NULL CHOICE_target_cell->ran_param_val.strct->ran_param_struct");

            seq_ran_param_t *cell = &choice_target_cell->ran_param_val.strct->ran_param_struct[0];
            // TODO: Add 4g cell id
//            if (cell->ran_param_id == NR_cell_8_4_4_1) {
            assert(cell->ran_param_id == NR_cell_8_4_4_1 && "wrong NR_cell id");
            assert(cell->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE && "wrong NR_cell type");
            assert(cell->ran_param_val.strct != NULL && "NULL nr_cell->ran_param_val.strct");
            assert(cell->ran_param_val.strct->sz_ran_param_struct == 1 &&
                   "wrong NR_cell->ran_param_val.strct->sz_ran_param_struct");
            assert(cell->ran_param_val.strct->ran_param_struct != NULL &&
                   "NULL NR_cell->ran_param_val.strct->ran_param_struct");

            seq_ran_param_t *nr_cgi = &cell->ran_param_val.strct->ran_param_struct[0];
            assert(nr_cgi->ran_param_id == NR_CGI_8_4_4_1 && "wrong NR_CGI id");
            assert(nr_cgi->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE &&
                   "wrong NR_CGI type");
            assert(nr_cgi->ran_param_val.flag_false != NULL && "NULL NR_CGI->ran_param_val.flag_false");
            assert(nr_cgi->ran_param_val.flag_false->type == BIT_STRING_RAN_PARAMETER_VALUE &&
                   "wrong NR_CGI->ran_param_val.flag_false type");
            pci = copy_ba_to_str(&nr_cgi->ran_param_val.flag_false->bit_str_ran);
//            } else {
//                assert(cell->ran_param_id == E_ULTRA_Cell_8_4_4_1 && "wrong id");
//                assert(cell->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE && "wrong e_ultra_cell type");
//                assert(cell->ran_param_val.strct != NULL && "NULL e_ultra_cell->ran_param_val.strct");
//                assert(cell->ran_param_val.strct->sz_ran_param_struct == 1 &&
//                       "wrong e_ultra_cell->ran_param_val.strct->sz_ran_param_struct");
//                assert(cell->ran_param_val.strct->ran_param_struct != NULL &&
//                       "NULL e_ultra_cell->ran_param_val.strct->ran_param_struct");
//
//                seq_ran_param_t *e_ultra_cgi = &cell->ran_param_val.strct->ran_param_struct[0];
//                assert(e_ultra_cgi->ran_param_id == E_ULTRA_CGI_8_4_4_1 && "wrong E_ULTRA_CGI id");
//                assert(e_ultra_cgi->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE &&
//                       "wrong E_ULTRA_CGI type");
//                assert(e_ultra_cgi->ran_param_val.flag_false != NULL && "NULL E_ULTRA_CGI->ran_param_val.flag_false");
//                assert(e_ultra_cgi->ran_param_val.flag_false->type == BIT_STRING_RAN_PARAMETER_VALUE &&
//                       "wrong E_ULTRA_CGI->ran_param_val.flag_false type");
//                pci = copy_ba_to_str(&e_ultra_cgi->ran_param_val.flag_false->bit_str_ran);
//            }

            ue_id_e2sm_t ue_id = ctrl_msg.hdr.frmt_1.ue_id;
            assert(ue_id.type == GNB_UE_ID_E2SM && "Wrong ue_id_e2sm type");
            //assert(ue_id.gnb.ran_ue_id != NULL && "NULL GNB_RAN_UE_ID");
            int rc = 0;
            if (ue_id.gnb.ran_ue_id != NULL) {
              rc = snprintf(enc_gbuf,
                           sizeof(enc_gbuf),
                           "{\"message\":\"handover\",\"ran_ue_id\":%ld,\"pci\":%s,\"message_id\":\"%d\"}",
                           *ue_id.gnb.ran_ue_id, pci,
                           msg_id);
              if (rc >= (int)sizeof(enc_gbuf))
              {
                lwsl_err("hit hard limit on internal buffer for command 'handover'\n");
                return NULL;
              }
            } else {
              // decrease cell gain specific cell
              // let UEs to handover existing cell/gnb by themselves
              ran_ind_t ws_ind = get_ringbuffer_data();
              assert(ws_ind.ran_config.len_nr_cell == 1 && "only support to turn off one cell's gain");
              int n_id_nrcell = atoi(pci);
              if (ws_ind.ran_config.nr_cells[0].n_id_nrcell == n_id_nrcell) {
                int cell_id = ws_ind.ran_config.nr_cells[0].cell_id;
                rc = snprintf(enc_gbuf,
                              sizeof(enc_gbuf),
                              "{\"message\":\"cell_gain\",\"cell_id\":%d,\"gain\":-200,\"message_id\":\"%d\"}",
                              cell_id, msg_id);
              } else {
                printf("Cannot find PCI %d\n", n_id_nrcell);
              }
              if (rc >= (int)sizeof(enc_gbuf))
              {
                lwsl_err("hit hard limit on internal buffer for command 'cell_gain'\n");
                return NULL;
              }
            }

            return enc_gbuf;
        }
    }
    return NULL;
}

// Copy code to encode ctrl
static const char *json_encode_ctrl(int msg_id, sm_ag_if_wr_ctrl_t write_msg)
{
 /* 
  * caveats: we go only or a modified MAC service model, i.e. like its implementation by CCC, 
  * instead of the SLICE because that one has no parameter that could be mapped into an Amarisoft websocket
  * parameter.
  */
    switch (write_msg.type) {
        case MAC_CTRL_REQ_V0:
            return json_encode_ctrl_mac(msg_id, write_msg.mac_ctrl);
        case RAN_CONTROL_CTRL_V1_03:
            return json_encode_ctrl_rc(msg_id, write_msg.rc_ctrl);
        default:
            return NULL;
    }
}

/*--------------------------------------------- API ----------------------------------------*/
ran_ser_abs_t json_ran_ser = {
    .get_ran_msghdr                 = &json_get_ran_msghdr,
    .free_ran_msghdr                = &json_free_ran_msghdr,

    .decode_indication_stats        = &json_decode_ran_indication_stats,
    .decode_indication_ue_get       = &json_decode_ran_indication_ue_get,
    .decode_indication_config_get   = &json_decode_ran_indication_config_get,
    .decode_config_get              = &json_decode_ran_config_get,
    .decode_indication_qos_flow_get = &json_decode_ran_indication_qos_flow_get,
    .decode_indication_ue_erab_get  = &json_decode_ran_indication_erab_get,
    .decode_e2setup                 = &json_decode_ran_e2setup,
    .decode_ctrl                    = &json_decode_ctrl,

    .encode_e2setup                 = &json_encode_ran_e2setup,
    .encode_indication              = &json_encode_ran_indication,    
    .encode_ctrl                    = &json_encode_ctrl
};

ran_ser_abs_t *ran_ser_get_instance(void) { return &json_ran_ser; }

void free_ran_config(ran_config_t *conf) 
{
  free_global_e2_node_id(&conf->global_e2_node_id);
  for (size_t idx = 0; idx < conf->len_nr_cell; idx++)
  {

    if(conf->nr_cells[idx].mode) {
      free(conf->nr_cells[idx].mode);
      conf->nr_cells[idx].mode = NULL;
    }
    
    if (conf->nr_cells[idx].label){
      free(conf->nr_cells[idx].label);
      conf->nr_cells[idx].label = NULL;
    }

    for (size_t i = 0; i < conf->nr_cells[idx].len_ncell; i++){
      if (conf->nr_cells[idx].ncell_list[i].rat) {
        free(conf->nr_cells[idx].ncell_list[i].rat);
        conf->nr_cells[idx].ncell_list[i].rat=NULL;
      }
    }
  }
}