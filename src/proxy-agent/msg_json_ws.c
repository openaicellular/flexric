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
  * PLMN is a (octet) string encoded in the following way:
  * Digits 0 to 9 encoded 0000 to 1001. The 1111 is used as filler digit.
  * Two digits per octet:
  * - bits 4 to 1 of octet n encoding digit 2n-1
  * - bits 8 to 5 of octet n encoding digit 2n
  * PLMN Identity consists of 3 digits from MCC followed by either:
  * - a filler digit plus 2 digits from MNC (in case of 2 digit MNC) or
  * - 3 digits from MNC (in case of 3 digit MNC).
  * Below a representation (f=filler)
  * 32 ....................... 16 ...........................1
  * | ---- | ---- | ---- | ---- | ---- |  ---- | ---- | ---- |   
  *         [mnc]   mnc     mnc   mnc/f    mcc    mcc     mcc
  
  * Example of json snippet coming from Amarisoft:
  * "global_gnb_id": {
  *      "plmn": 50501",
  *      "gnb_id_bits": 8,
  *      "gnb_id": 3590,
  *      "gnb_name": gnb0000e06"
  * }
  *
  * XXX-TESTS: near RT ric reports: mcc = 530, mnc = 353, mnc_digit_len = 3  for the PLMN 50501. Check if they are correct.
  */

  if (!json_object_object_get_ex(key, "plmn", &plmn))
    return false;
  const char *p = json_object_get_string(plmn);
  if (!p)
    return false;
  /* caveats: struct similar to OCTET STRING, just to be used for the function PLMNID_TO_MCC_MNC()*/
  struct local_plmnid_t {
    const uint8_t *buf;	
    size_t size;	
  };
  struct local_plmnid_t * p_octet = malloc(sizeof(struct local_plmnid_t));
  if (!p_octet)
    return false;
  p_octet->buf = (unsigned char *)p;
  p_octet->size = strlen(p) + 1;
  PLMNID_TO_MCC_MNC(p_octet, out->plmn.mcc, out->plmn.mnc, out->plmn.mnc_digit_len);
  free(p_octet);

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

