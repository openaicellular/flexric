#include <libwebsockets.h>

#include "ser_ran.h"
#include "../../../src/util/alg_ds/alg/defer.h"

// guess the type of message received from the endpoint
static bool ws_associate_valid_msg_type(server_msghdr_t *hdr)
{
  bool ret = true;

  if (!strcmp(hdr->type, "ready")){
    hdr->typeid = RAN_READY;
  } else if (!strcmp(hdr->type, "register")){
    hdr->typeid = RAN_REGISTRATION_ACK;
  } else if (!strcmp(hdr->type,"authenticate")){
    hdr->typeid = RAN_AUTHENTICATE_ACK;
  } else if (!strcmp(hdr->type, "config_get")){
    hdr->typeid = RAN_CONFIG_RECV;
  } else if (!strcmp(hdr->type, "stats")){
    hdr->typeid = RAN_INDICATION_STATS_RECV;
  } else if (!strcmp(hdr->type, "ue_get")){
    hdr->typeid = RAN_INDICATION_UEGET_RECV;
  } else if (!strcmp(hdr->type, "config_set")){
    hdr->typeid = RAN_CTRL_ACK;
  } else if (!strcmp(hdr->type, "handover")){
    hdr->typeid = RAN_CTRL_ACK;
  } else if (!strcmp(hdr->type, "cell_gain")){
    hdr->typeid = RAN_CTRL_ACK;
  } else{
    ret = false;
  }

  return ret;
}

static const char* handle_ran_config(struct json_object* str_json_obj)
{
  return ran_ser_get_instance()->encode_config_get(str_json_obj);
}

static const char* handle_ran_indication_stats(struct json_object* str_json_obj){
  return ran_ser_get_instance()->encode_stats(str_json_obj);
}

static const char* handle_ran_indication_ue(struct json_object* str_json_obj){
  return ran_ser_get_instance()->encode_ue(str_json_obj);
}

static const char* handle_ran_ctrl_ack(struct json_object* str_json_obj){
  return ran_ser_get_instance()->encode_ctrl_ack(str_json_obj);
}

/* 
 * Table with function pointers to correlate messages received (ranmsg) with actions to be performed as a 
 * consequence (return value)
 */
typedef const char* (*handle_msg_server)(struct json_object* str_json_obj);
static handle_msg_server handle_msg[4] = {
//  // Note that the remaining unspecified seats in this array are initialized automatically to NULL(0)
  [RAN_CONFIG_RECV]             = &handle_ran_config,
  [RAN_INDICATION_STATS_RECV]   = &handle_ran_indication_stats,
  [RAN_INDICATION_UEGET_RECV]   = &handle_ran_indication_ue,
  [RAN_CTRL_ACK]                = &handle_ran_ctrl_ack,
};

const char* ran_msg_handle(struct json_object *json_obj)
{
  server_msghdr_t msghdr = ran_ser_get_instance()->get_server_msghdr(json_obj);
  defer({ran_ser_get_instance()->free_server_msghdr(msghdr); }; );

  lwsl_debug("RAN hdr decoded: (type=%s, message_id=%d)\n",  msghdr.type, msghdr.msg_id);
  if (ws_associate_valid_msg_type(&msghdr) == false) 
  {
    lwsl_err("discarding unexpected received msg type: (%s)\n",  msghdr.type);
    return "";
  }

  return handle_msg[msghdr.typeid](json_obj);
}