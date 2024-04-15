#include "send_msg_amr.h"
#include <assert.h>

void send_config_get(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[64] = {0}; 
  size_t sz = snprintf(msg, 64, "{\"message\": \"config_get\", \"message_id\": %d }", msg_id );
  assert(sz < 64);

  printf("Sending message \n");
  send_ep_amr(ep, (uint8_t*)msg, sz);
}

void send_msg_stats(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  uint8_t msg[64] = {0}; 
  size_t sz = snprintf((char*)msg, 64, "{\"message\": \"stats\", \"message_id\": %d }", msg_id );
  assert(sz < 64);

  send_ep_amr(ep, msg, sz);
}

void send_msg_ue_get(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[64] = {0}; 
  size_t sz = snprintf(msg, 64, "{\"message\": \"ue_get\", \"message_id\": %d }", msg_id );
  assert(sz < 64);

  send_ep_amr(ep, (uint8_t*)msg, sz);
}

void send_msg_stats_ue_get(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[128] = {0}; 
  size_t sz = snprintf(msg, 128, "[{\"message\": \"ue_get\", \"message_id\": %d }, {\"message\": \"ue_get\", \"message_id\": %d } ]", msg_id, msg_id);
  assert(sz < 128);

  send_ep_amr(ep, (uint8_t*)msg, sz);
}



