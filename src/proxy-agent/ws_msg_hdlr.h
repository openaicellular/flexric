/* interface for handling messages from and to Websockets interface */
#ifndef WS_MSG_HDLR_H 
#define WS_MSG_HDLR_H 

#include <stdio.h>
#include "lib/ap/e2ap_types/common/e2ap_plmn.h"


#include "ws_ran_if.h"
#include "e2_sm_agent.h"
#include "proxy_agent.h"

typedef enum{
      // msgs coming from RAN
      WS_REGISTRATION_ACK,
      WS_AUTHENTICATE_ACK,
      WS_INDICATION_RECV,
      WS_CONFIG_RECV,
      WS_CTRL_ACK,
      WS_READY,
      // msgs for the RAN
      WS_INDICATION_REQ
}ws_type_id_t;

// raw json message in text or in binary form coming from RAN
typedef struct ws_msg_t { 
  char *buf;
  size_t len; // we put len to deal with the case you have binary data instead of text, where a string would suffice.
} ws_msg_t;

// message coming from RAN decoded
typedef struct {
  char *type;
  int msg_id;
  ws_type_id_t typeid;
} ws_msghdr_t;

// internal representation of RAN received messages
// XXX: this is a generic data structure that will be used to map the info we want for being returned via the indication 
// message of various SMs (i.e. KPM/MAC etc..) this data structure has to be SM agnostic
typedef struct ws_ind_t {
  //TBC
} ws_ind_t;

e2_agent_msg_t ws_msg_handle(proxy_agent_t *p, const ws_msg_t* msg);

#endif