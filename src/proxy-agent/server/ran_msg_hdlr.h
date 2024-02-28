#ifndef SERVER_MSG_HDLR_H
#define SERVER_MSG_HDLR_H

#include <stddef.h>
#include <json-c/json.h>

const char* ran_msg_handle(struct json_object *json_obj);

#endif