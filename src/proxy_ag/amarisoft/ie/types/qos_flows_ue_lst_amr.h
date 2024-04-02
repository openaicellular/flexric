#ifndef QOS_FLOWS_UE_LST_MIR_H
#define QOS_FLOWS_UE_LST_MIR_H

typedef struct{
  int pdu_session_id;
  int dl_total_bytes;
  int ul_total_bytes;
  int sst; 
  int sd;
} qos_flows_ue_lst_amr_t;

qos_flows_ue_lst_amr_t parse_qos_flows_ue_lst_amr(void* it);

#endif
