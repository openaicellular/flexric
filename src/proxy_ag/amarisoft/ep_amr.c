#include "ep_amr.h"
#include <assert.h>

ep_amr_t init_ep_amr(void)
{
  ep_amr_t dst = {0}; 

  // Context
  dst.ctx = nopoll_ctx_new();
  assert(dst.ctx != NULL && "Error when creating a new nopoll context" );

  const char ip[] = "127.0.0.1";
  const char port[] = "9001";
  const char* name = NULL;
  const char* get_url = NULL;
  const char* proto = NULL;
  const char* origin = NULL;
 
  // Connection
  dst.conn = nopoll_conn_new(dst.ctx, ip , port, name, get_url, proto, origin);
  assert(dst.conn != NULL && "Connection error");

  const int timeout_sec = 5;
  int rc = nopoll_conn_wait_until_connection_ready(dst.conn, timeout_sec); 
  assert(rc == 1 && "Connection ready timeout error");

  *(int*)&dst.fd = nopoll_conn_socket(dst.conn);
  assert(dst.fd != -1 && "Error while getting the nopoll socket descriptor");

  printf("[PROXY AGENT]: Connection ready");

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


  noPollCtx * ctx = nopoll_ctx_new ();
  if (! ctx) {
   return -1;
  }
  noPollConn * conn = nopoll_conn_new (ctx, "localhost", "9001", NULL, NULL, NULL, NULL);
  if (! nopoll_conn_is_ok (conn)) {
   return -1;
  }
  if (! nopoll_conn_wait_until_connection_ready (conn, 5)) {
    return -1;
  }
  int sfd = nopoll_conn_socket(conn);
  assert(sfd != -1);
  int efd = init_epoll();
  add_fd_epoll(efd, sfd);
  const int maxevents = 1;
  struct epoll_event events[maxevents];
  const int timeout_ms = 10000;
  for(size_t i = 0; i < 128; ++i){
    const int events_ready = epoll_wait(efd, events, maxevents, timeout_ms);
    printf("Events ready %d \n", events_ready);
    if(events_ready == 0) {
      printf("Timeout without client data, closing \n");
      break;
    }
    for(int i = 0; i < events_ready; ++i){
      printf(" events_ready %d \n", events_ready);
      assert((events[i].events & EPOLLERR) == 0);
      const int cur_fd = events[i].data.fd;
      assert(cur_fd == sfd);
      noPollMsg* msg = nopoll_conn_get_msg(conn);
      assert(msg != NULL);
      const unsigned char* payload = nopoll_msg_get_payload(msg);
      printf("%s \n", payload);
      nopoll_msg_unref(msg);
    }
    const char msg[] = "\{\"message\": \"config_get\"}";
    size_t const sz = strlen(msg);
  if (nopoll_conn_send_text (conn, msg, sz) != sz) {
          assert(0 != 0);
  }
  }
  nopoll_ctx_unref (ctx);
  return 0;
  noPollCtx * ctx = nopoll_ctx_new ();
  if (! ctx) {
   return -1;
  }
  noPollConn * conn = nopoll_conn_new (ctx, "localhost", "9001", NULL, NULL, NULL, NULL);
  if (! nopoll_conn_is_ok (conn)) {
   return -1;
  }
  if (! nopoll_conn_wait_until_connection_ready (conn, 5)) {
    return -1;
  }
  int sfd = nopoll_conn_socket(conn);
  assert(sfd != -1);
  int efd = init_epoll();
  add_fd_epoll(efd, sfd);
  const int maxevents = 1;
  struct epoll_event events[maxevents];
  const int timeout_ms = 10000;
  for(size_t i = 0; i < 128; ++i){
    const int events_ready = epoll_wait(efd, events, maxevents, timeout_ms);
    printf("Events ready %d \n", events_ready);
    if(events_ready == 0) {
      printf("Timeout without client data, closing \n");
      break;
    }
    for(int i = 0; i < events_ready; ++i){
      printf(" events_ready %d \n", events_ready);
      assert((events[i].events & EPOLLERR) == 0);
      const int cur_fd = events[i].data.fd;
      assert(cur_fd == sfd);
      noPollMsg* msg = nopoll_conn_get_msg(conn);
      assert(msg != NULL);
      const unsigned char* payload = nopoll_msg_get_payload(msg);
      printf("%s \n", payload);
      nopoll_msg_unref(msg);
    }
    const char msg[] = "\{\"message\": \"config_get\"}";
    size_t const sz = strlen(msg);
  if (nopoll_conn_send_text (conn, msg, sz) != sz) {
          assert(0 != 0);
  }
  }
  nopoll_ctx_unref (ctx);
  return 0;




