#ifndef SER_RAN_SERVER_H
#define SER_RAN_SERVER_H

#include <stdbool.h>
#include <stddef.h>
#include <json-c/json.h>

typedef enum{
  RAN_CONFIG_RECV,
  RAN_INDICATION_STATS_RECV,
  RAN_INDICATION_UEGET_RECV,
  RAN_CTRL_ACK,
  RAN_REGISTRATION_ACK,
  RAN_AUTHENTICATE_ACK,
  RAN_INDICATION_CONFGET_RECV,
  RAN_READY,
} ran_type_id_t;

// raw json message in text or in binary form coming from client
typedef struct server_msg_t {
  const char *buf;
  size_t      len; // we put len to deal with the case you have binary data instead of text, where a string would suffice.
} server_msg_t;

// decoded message header coming from client
typedef struct {
  char *        type;
  int           msg_id;
  ran_type_id_t typeid;
  bool          error;
} server_msghdr_t;

typedef struct ran_ser_abs_t {
  server_msghdr_t (*get_server_msghdr)(struct json_object *json_obj);
  void (*free_server_msghdr)(server_msghdr_t msg);
  const char* (*encode_ready)(void);
  const char* (*encode_config_get)(struct json_object* str_json_obj);
  const char* (*encode_stats)(struct json_object* str_json_obj);
  const char* (*encode_ue)(struct json_object* str_json_obj);
  const char* (*encode_ctrl_ack)(struct json_object* str_json_obj);
} ran_ser_abs_t;

ran_ser_abs_t *ran_ser_get_instance(void);

#endif