#ifndef ENDPOINT_AMARISOFT_MIR_H
#define ENDPOINT_AMARISOFT_MIR_H 

#include "ws_lib/nopoll/src/nopoll.h"
#include "../../../lib/ep/ws_msg.h"

typedef struct{
  noPollCtx* ctx;
  noPollConn* conn; 
  char addr[16]; // only ipv4 supported
  int port;
  int fd;
  pthread_mutex_t mtx;
} ep_amr_t;

ep_amr_t init_ep_amr(const char* ip, int32_t port);

void stop_ep_amr(ep_amr_t* ep);

void send_ep_amr(ep_amr_t const* ep, const uint8_t* msg, size_t sz);

ws_msg_t recv_ep_amr(ep_amr_t const* ep);

#endif
