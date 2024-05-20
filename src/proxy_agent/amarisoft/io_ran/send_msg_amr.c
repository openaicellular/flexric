#include "send_msg_amr.h"
#include <assert.h>

void send_config_get(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[64] = {0}; 
  size_t sz = snprintf(msg, 64, "{\"message\": \"config_get\", \"message_id\": %d }", msg_id );
  assert(sz < 64);

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
  size_t sz = snprintf(msg, 64, "{\"message\": \"ue_get\", \"message_id\": %d, \"stats\": 1}", msg_id );
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

void send_ho(ep_amr_t const* ep, int msg_id, uint64_t pci, uint64_t ran_ue_id, size_t ssb_nr_arfcn)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[256] = {0}; 
  size_t sz = snprintf(msg, 256, "{\"message\": \"handover\", \"message_id\": %d, \"ran_ue_id\":%ld, \"pci\":%ld, \"ssb_nr_arfcn\": %ld }", msg_id, ran_ue_id, pci, ssb_nr_arfcn);
  assert(sz < 256);

  printf("Sending HO %s \n", msg);

  send_ep_amr(ep, (uint8_t*)msg, sz);
}

void send_config_set_rb_control_dl(ep_amr_t const* ep, int msg_id, uint64_t cell_id, uint64_t pdsch_fixed_rb_start, uint64_t pdsch_fixed_l_crb){
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[256] = {0};
  size_t sz = snprintf(msg, 256, "{\"message\": \"config_set\", \"message_id\": %d,\"cells\":{\"%ld\": { \"pdsch_fixed_rb_start\" : %ld, \"pdsch_fixed_l_crb\": %ld, \"pdsch_fixed_rb_alloc\":true }} }", msg_id, cell_id, pdsch_fixed_rb_start, pdsch_fixed_l_crb);
  assert(sz < 256);

  printf("Sending config set set rb control dl %s \n", msg);

  send_ep_amr(ep, (uint8_t*)msg, sz);
}

void send_config_set_rb_control_ul(ep_amr_t const* ep, int msg_id, uint64_t cell_id, uint64_t pusch_fixed_rb_start, uint64_t pusch_fixed_l_crb){
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[256] = {0};
  size_t sz = snprintf(msg, 256, "{\"message\": \"config_set\", \"message_id\": %d,\"cells\":{\"%ld\": { \"pusch_fixed_rb_start\" : %ld, \"pusch_fixed_l_crb\": %ld, \"pusch_fixed_rb_alloc\":true }} }", msg_id, cell_id, pusch_fixed_rb_start, pusch_fixed_l_crb);
  assert(sz < 256);

  printf("Sending config set rb control ul %s \n", msg);

  send_ep_amr(ep, (uint8_t*)msg, sz);
}