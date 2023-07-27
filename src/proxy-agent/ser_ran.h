/* Serialization interface */
#ifndef SER_RAN_H
#define SER_RAN_H

#include "ran_msg_hdlr.h"

#include "lib/ap/e2ap_types/common/e2ap_global_node_id.h"

typedef struct ws_ioloop_event_t ws_ioloop_event_t; // XXX: to investigate. this a forward declaration to skip error of linking ws_io_ran.h. 

typedef struct ran_ser_abs_t {
  ran_msghdr_t (*get_ran_msghdr)(const ran_msg_t *msg);
  void (*free_ran_msghdr)(ran_msghdr_t msg);

  bool (*decode_indication_stats)(const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_indication_ue_get) (const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_indication_config_get)(const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_indication_qos_flow_get) (const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_indication_ue_erab_get) (const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_e2setup)(const ran_msg_t *msg, global_e2_node_id_t *out);
  bool (*decode_ctrl)(const ran_msghdr_t *msg, ctrl_ev_reply_t *out, const ws_ioloop_event_t *sent_ev);

  const char * (*encode_e2setup)(int message_id);
  const char * (*encode_indication)(int message_id, int sm_id);
  const char * (*encode_ctrl)(int message_id, sm_ag_if_wr_t ctrl_msg);

}ran_ser_abs_t; 

ran_ser_abs_t *ran_ser_get_instance(void);

#endif