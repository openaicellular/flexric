#ifndef RAN_MSG_HDLR_H
#define RAN_MSG_HDLR_H

#include "util/alg_ds/ds/assoc_container/bimap.h"

#include "e2_sm_agent.h"

typedef enum{
      // msgs coming from RAN
      RAN_REGISTRATION_ACK,
      RAN_AUTHENTICATE_ACK,
      RAN_INDICATION_UEGET_RECV,
      RAN_INDICATION_STATS_RECV,
      RAN_CONFIG_RECV,
      RAN_CTRL_ACK,
      RAN_READY,
      // msgs for the RAN
      RAN_INDICATION_REQ,
      RAN_CTRL_REQ
} ran_type_id_t; // WARN: keep this datastructure in sync with 'static handle_msg_ran handle_msg' implementation

// raw json message in text or in binary form coming from RAN
typedef struct ran_msg_t { 
  const char *buf;
  size_t      len; // we put len to deal with the case you have binary data instead of text, where a string would suffice.
} ran_msg_t;

// decoded message header coming from RAN
typedef struct {
  char *        type;
  int           msg_id;   /* msg_id is mapped to the event that generated this reply from the RAN via the ran_if->io_loop data structure. 
                           * If msg_id = 0, it means indication requested generated it.
                           */
  ran_type_id_t typeid;
  bool          error;
} ran_msghdr_t;


/*  
 * This function will manage messages coming from the RAN endpoint in encoded form and returns 
 * a message response if needed.
 */
next_msg_t ran_msg_handle(const char *buf, size_t len, bi_map_t *sent_msg);


#endif