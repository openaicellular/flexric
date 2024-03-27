#ifndef WEBSOCKET_MESSAGE_MIR_H
#define WEBSOCKET_MESSAGE_MIR_H 

#include <stdint.h>
#include <stdlib.h>

typedef struct{
  uint8_t* buf;
  size_t sz;
} ws_msg_t;

#endif
