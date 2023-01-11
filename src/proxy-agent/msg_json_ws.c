/* encoding/decoding in json. */
#include <json-c/json.h>
#include <stdio.h>
#include <libwebsockets.h>

#include "util/conversions.h"
#include "msg_json_ws.h"
#include "ws_msg_hdlr.h"
static char global_buff[1024];

ws_msghdr_t ws_json_get_msghdr(const ws_msg_t *msg) 
{
  ws_msghdr_t ret_msg = {.type = NULL, .msg_id = -1};
  struct json_tokener *tok = json_tokener_new();
  if (!tok)
    return ret_msg;
  struct json_object *json_obj = json_tokener_parse_ex(tok, msg->buf, msg->len);
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

bool ws_json_decode_indication (const ws_msg_t *msg, ws_ind_t *out) 
{
  (void)msg;
  (void)out;
  assert(0!=0 && "not implemented yet\n");
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
  struct json_object *json_obj = json_tokener_parse_ex(tok, in_msg->buf, in_msg->len);
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
  out->plmn.mcc = plmn_val/100;
  out->plmn.mnc = plmn_val - out->plmn.mcc*100;
  out->plmn.mnc_digit_len = out->plmn.mnc > 99 ? 3 : 2;

  if (!json_object_object_get_ex(key, "gnb_id", &gnb_id))
    return false;
  out->nb_id = json_object_get_int(gnb_id);

  return true;
}

const char * ws_json_encode_indication (int msg_id, int sm_id) 
{ 
  (void)sm_id;
  snprintf(global_buff, sizeof(global_buff), "{\"message\":\"stats\",\"message_id\":\"%d\"}", msg_id);
  return global_buff;
}

const char * ws_json_encode_e2setup(int msg_id) 
{
  snprintf(global_buff, sizeof(global_buff), "{\"message\":\"config_get\",\"message_id\":\"%d\"}", msg_id);
  return global_buff;
}

