#include "ep_amr.h"
#include <assert.h>
#include <stdbool.h>
#include "../../util/alg_ds/alg/defer.h"

static
bool wait_conn_ready_or_timeout(noPollConn* conn)
{

  // Not working. Why?
  //const int timeout_sec = 5;
  //rc = nopoll_conn_wait_until_connection_ready(dst.conn, timeout_sec); 
  //assert(rc == 1 && "Connection ready timeout error");

  for (int i = 0; i < 128; ++i) {
    bool conn_ready = nopoll_conn_is_ready(conn);
    if(conn_ready == true) 
      return true; 
    int const ten_ms = 10000;  
    usleep(ten_ms);
  }
  // Time out
  return false;
}

ep_amr_t init_ep_amr(const char* ip, int32_t port)
{
  assert(ip != NULL && "Invalid Amarisoft RAN ip address");
  assert(port > 0 && "Invalid Amarisoft RAN port");

  ep_amr_t dst = {0}; 

  // Context
  dst.ctx = nopoll_ctx_new();
  assert(dst.ctx != NULL && "Error when creating a new nopoll context" );

  //const char ip[] = "127.0.0.1";
  //const char port[] = "9001";
  char port_str[10] = {0};
  int rc = snprintf(port_str, 10, "%d", port);
  assert(rc < 10 && "Bug in port number?");

  const char* name = NULL;
  const char* get_url = NULL;
  const char* proto = NULL;
  const char* origin = NULL;

  for(;;){
    // Part of the websocket handshake message is sent when calling nopoll_conn_new, 
    // thus it needs to be in loop. Sucks.
    dst.conn = nopoll_conn_new(dst.ctx, ip , port_str, name, get_url, proto, origin);
    assert(dst.conn != NULL && "Connection error");
    bool conn_ready = wait_conn_ready_or_timeout(dst.conn);

    if(conn_ready == true) 
      break;

    printf("[PROXY]: Websocket connection with %s %s was NOT successfull.\n Trying again.\n", ip, port_str);
    // Free the conn handler and try again
    int status = -1;
    const char reason[] = "Timeout waiting connection";
    nopoll_conn_close_ext(dst.conn, status, reason, strlen(reason));
  }

  dst.fd = nopoll_conn_socket(dst.conn);
  assert(dst.fd != -1 && "Error while getting the nopoll socket descriptor");

  printf("[PROXY]: Websocket handshake with %s %s SUCESSFULLY completed\n", ip, port_str);
  fflush(stdout);

  return dst; 
}

void send_ep_amr(ep_amr_t const* ep, const uint8_t* msg, size_t sz)
{
  assert(ep != NULL);
  assert(msg != NULL);
  assert(sz > 0);
  size_t const sz_sent = nopoll_conn_send_text(ep->conn, (char*)msg, sz);
  assert(sz_sent == sz);
}

ws_msg_t recv_ep_amr(ep_amr_t const* ep)
{
  noPollMsg* msg = nopoll_conn_get_msg(ep->conn);
  assert(msg != NULL);
  defer({ nopoll_msg_unref(msg); } );

  uint8_t const* payload = nopoll_msg_get_payload(msg);
  size_t const sz_payload = strlen((char*)payload);

  ws_msg_t dst = {.sz = sz_payload};
  dst.buf = calloc(dst.sz, sizeof(uint8_t));
  assert(dst.buf != NULL && "Memory exhausted");
  memcpy(dst.buf, payload, sz_payload);
  return dst;
}

void stop_ep_amr(ep_amr_t* ep)
{
  // conn
  int status = -1;
  const char reason[] = "Bye!";
  nopoll_conn_close_ext(ep->conn, status, reason, strlen(reason));

  // ctx
  nopoll_ctx_unref(ep->ctx);
}

