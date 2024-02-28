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

#include "ser_ran.h"

static char* LOG_MODULE_STR = "JSON-LOGGING";
static char enc_gbuf[1024 * 32]; //

struct test_t {
  char *path;
} tests[] = {
  {"/home/khai/work/flexric/test/proxy_agent/config_get.2023.02.27.json"},
  {"/home/khai/work/flexric/test/proxy_agent/stats.json"},
  {"/home/khai/work/flexric/test/proxy_agent/ue_get.json"},
  {"/home/khai/work/flexric/test/proxy_agent/qos_flow_get.json"}
};

/* -----------------------------------------------------------------------------------
 *                                       Encoding
 * ----------------------------------------------------------------------------------- */
static const char *json_encode_config_get(struct json_object* str_json_obj)
{
  struct json_object *msg_id;
  struct json_object* ret_json_obj = json_object_from_file(tests[0].path);

  // Get message_id from source
  if (!json_object_object_get_ex(str_json_obj, "message_id", &msg_id))
    return "";

  // Add new string return json obj
  json_object* mess_id_value = json_object_new_string(json_object_get_string(msg_id));
  int rc = json_object_object_add(ret_json_obj, "message_id", mess_id_value);
  if (rc != 0)
    return "";

  const char* value = json_object_get_string(ret_json_obj);
  strcpy(enc_gbuf, value);

  // Free the return json obj including the mess_id_value
  json_object_put(ret_json_obj);
  return enc_gbuf;
}

static const char *json_encode_stats(struct json_object* str_json_obj)
{
  struct json_object *msg_id;
  struct json_object* ret_json_obj = json_object_from_file(tests[1].path);

  if (!json_object_object_get_ex(str_json_obj, "message_id", &msg_id)){
    const char* value = json_object_get_string(ret_json_obj);
    strcpy(enc_gbuf, value);
  } else {
    // Add new string return json obj
    json_object* mess_id_value = json_object_new_string(json_object_get_string(msg_id));
    int rc = json_object_object_add(ret_json_obj, "message_id", mess_id_value);
    if (rc != 0)
      return "";

    const char* value = json_object_get_string(ret_json_obj);

    strcpy(enc_gbuf, value);
  }

  // Free the return json obj including the mess_id_value
  json_object_put(ret_json_obj);
  return enc_gbuf;
}

static const char *json_encode_ue(struct json_object* str_json_obj)
{
  struct json_object *msg_id;
  struct json_object* ret_json_obj = json_object_from_file(tests[2].path);

  if (!json_object_object_get_ex(str_json_obj, "message_id", &msg_id)){
    const char* value = json_object_get_string(ret_json_obj);
    strcpy(enc_gbuf, value);
  } else {
    // Add new string return json obj
    json_object* mess_id_value = json_object_new_string(json_object_get_string(msg_id));
    int rc = json_object_object_add(ret_json_obj, "message_id", mess_id_value);
    if (rc != 0)
      return "";

    const char* value = json_object_get_string(ret_json_obj);

    strcpy(enc_gbuf, value);
  }

  // Free the return json obj including the mess_id_value
  json_object_put(ret_json_obj);
  return enc_gbuf;
}

static const char *json_encode_ready()
{
  snprintf(enc_gbuf,
           sizeof(enc_gbuf),
           "{\n"
           "    \"message\": \"ready\",\n"
           "    \"type\": \"ENB\",\n"
           "    \"name\": \"ENB\",\n"
           "    \"version\": \"2023-12-15\",\n"
           "    \"time\": 1080.884,\n"
           "    \"utc\": 1708452761.997\n"
           "}"
           );
  return enc_gbuf;
}

static const char *json_encode_ran_ctrl_ack(struct json_object* str_json_obj)
{
  struct json_object *msg_id;
  if (!json_object_object_get_ex(str_json_obj, "message_id", &msg_id))
    return "";
  snprintf(enc_gbuf,
           sizeof(enc_gbuf),
           "{\n"
           "    \"message_id\": \"%s\",\n"
           "    \"message\": \"handover\",\n"
           "    \"time\": 1080.884,\n"
           "    \"utc\": 1708452761.997\n"
           "}",
           json_object_get_string(msg_id)
  );
  return enc_gbuf;
}

static void json_free_server_msghdr(server_msghdr_t msg)
{
  if (msg.type != NULL)
    free(msg.type);
}

static server_msghdr_t get_server_msghdr(struct json_object* json_obj)
{
  server_msghdr_t ret_msg = {.type = NULL, .msg_id = -1};

  // lws logging system uses a hardlimit of 256 chars for truncation, so output below may be truncated.
  lwsl_err("%s: Dumping json data %s:\n",
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

/*--------------------------------------------- API ----------------------------------------*/
ran_ser_abs_t json_ran_ser = {
    .get_server_msghdr                 = &get_server_msghdr,
    .free_server_msghdr                = &json_free_server_msghdr,

    .encode_ready                      = &json_encode_ready,
    .encode_config_get                 = &json_encode_config_get,
    .encode_ue                         = &json_encode_ue,
    .encode_stats                      = &json_encode_stats,
    .encode_ctrl_ack                   = &json_encode_ran_ctrl_ack
};

ran_ser_abs_t *ran_ser_get_instance(void) { return &json_ran_ser; }