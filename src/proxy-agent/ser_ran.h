/* Serialization interface 
 * Memory management: we take the approach here to use statically allocated datastructures with limits to store output of decoding functions
 */
#ifndef SER_RAN_H
#define SER_RAN_H

#include "ran_msg_hdlr.h"

#include "lib/e2ap/e2ap_global_node_id_wrapper.h"

typedef struct ws_ioloop_event_t ws_ioloop_event_t; // XXX: to investigate. this a forward declaration to skip error of linking ws_io_ran.h. 


#define AMARISOFT_MAX_CELL_NUM           10
#define AMARISOFT_MAX_SECONDARY_CELL_NUM 10
#define AMARISOFT_MAX_UE_NUM             10
#define AMARISOFT_MAX_QOS_NUM            10
#define AMARISOFT_MAX_PLMN_NUM           10
#define AMARISOFT_MAX_NSSAI_NUM          10
#define AMARISOFT_MAX_PLMNID_NUM         10

typedef struct amr_conn_mobility_t {
  bool scell_config;      // Set to true if scell_config is defined.
  bool nr_handover;       // Set to true if nr_handover is defined.
  bool nr_cell_redirect;  // Set to true if nr_cell_redirect is dedined
  bool eutra_handover;    // Set to true if eutra_handover is defined.
  bool eutra_cell_redirect;// Set to true if eutra_cell_redirect is defined.
  bool nr_dc_setup;       // Set to true if nr_dc_setup is defined.
  bool mr_dc_release;     // Set to true if mr_dc_release is defined.
}amr_conn_mobility_t;

typedef struct amr_scell_list_t {
  int   cell_id;
  bool  ul_allowed;
}amr_scell_list_t;

typedef struct amr_ncell_list_t {
    char *rat;
    int dl_earfcn;
    int n_id_cell;
    eutra_cgi_t ecgi;
    int ssb_nr_arfcn;
    int n_id_nrcell;
    nr_cgi_t ncgi;
    bool handover_target;
    bool cell_redirect_target;
    bool eps_fallback_target;
    bool emergency_fallback_target;
}amr_ncell_list_t;

typedef struct amr_en_dc_scg_cell_list_t {
    int cell_id;
}amr_en_dc_scg_cell_list_t;

typedef struct nssai_t {
    int sst;
    int sd;
}nssai_t;

typedef struct nr_plmn_list_t {
    size_t len_plmn_ids;
    char** plmn_ids[AMARISOFT_MAX_PLMNID_NUM];
    bool reserved;
    int tac;
    int ranac;
    size_t len_nssai;
    nssai_t nssai[AMARISOFT_MAX_NSSAI_NUM];
}nr_plmn_list_t;

typedef struct amarisoft_cell_t 
{ 
  int     cell_id;
  double  dl_bitrate;          // Downlink bitrate in bits per seconds at PHY layer level (Counts acknowledged transmissions).
  double  ul_bitrate;          // Uplink bitrate in bits per seconds at PHY layer level (Counts successful transmissions).
  double  mbms_bitrate;        // Broadcast downlink bitrate in bits per seconds. 
  int     dl_tx;               // Number of downlink transmitted transport blocks (without retransmissions).
  int     ul_tx;               // Number of received uplink transport blocks (without CRC error).
  int     dl_retx;             // Number of downlink retransmitted transport blocks.
  int     ul_retx;             // Number of received uplink transport blocks with CRC errors.
  int     dl_err;              // Number of downlink non transmitted transport blocks (after retransmissions).Since Amarisoft version XXXX
  int     ul_err;              // Number of non received uplink transport blocks (after retransmissions). Since Amarisoft version XXXX
  double  dl_sched_users_min;  // Minimum downlink scheduled users per TTI.
  double  dl_sched_users_avg;  // Average downlink scheduled users per TTI.
  double  dl_sched_users_max;  // Maximum downlink scheduled users per TTI.
  double  ul_sched_users_min;  // Minimum uplink scheduled users per TTI.
  double  ul_sched_users_avg;  // Average uplink scheduled users per TTI.
  double  ul_sched_users_max;  // Maximum uplink scheduled users per TTI.
  double  dl_use_min;          // number between 0 and 1. Minimum downlink usage ratio, based on number of allocated resource blocks.
  double  dl_use_avg;          // Number between 0 and 1. Average downlink usage ratio, based on number of allocated resource blocks.
  double  dl_use_max;          // Number between 0 and 1. Maximum downlink usage ratio, based on number of allocated resource blocks.
  double  ul_use_min;          // Number between 0 and 1. Minimum uplink usage ratio, based on number of allocated resource blocks.
  double  ul_use_avg;          // Number between 0 and 1. Average uplink usage ratio, based on number of allocated resource blocks.
  double  ul_use_max;          // Number between 0 and 1. Maximum uplink usage ratio, based on number of allocated resource blocks.
  double  ctrl_use_min;        // Number between 0 and 1. Minimum control usage ratio, based on number of used CCE.
  double  ctrl_use_avg;        // Number between 0 and 1. Average control usage ratio, based on number of used CCE.
  double  ctrl_use_max;        // Number between 0 and 1. Maximum control usage ratio, based on number of used CCE.
  int     ue_count_min;        // Minimum number of UE contexts.
  int     ue_count_max;        // Maximum number of UE contexts.
  int     ue_count_avg;        // Average number of UE contexts.
  int     erab_count_min;      // Minimum number of established radio bearer. Applicable to LTE or NB-IoT cells.
  int     erab_count_max;      // Maximum number of established radio bearer. Applicable to LTE or NB-IoT cells.
  int     erab_count_avg;      // Average number of established radio bearer. Applicable to LTE or NB-IoT cells.
  int     drb_count_min;       // Minimum number of established radio bearer. Applicable to NR cells.
  int     drb_count_max;       // Maximum number of established radio bearer. Applicable to NR cells.
  int     drb_count_avg;       // Average number of established radio bearer. Applicable to NR cells.
  double  dl_gbr_use_min;      // Optional number. Minimum downlink GBR usage ratio. Not present for NB-IoT cells.
  double  dl_gbr_use_avg;      // Optional number. Average downlink GBR usage ratio. Not present for NB-IoT cells.
  double  dl_gbr_use_max;      // Optional number. Maximum downlink GBR usage ratio. Not present for NB-IoT cells.
  double  ul_gbr_use_min;      // Optional number. Minimum uplink GBR usage ratio. Not present for NB-IoT cells.
  double  ul_gbr_use_avg;      // Optional number. Average uplink GBR usage ratio. Not present for NB-IoT cells.
  double  ul_gbr_use_max;      // Optional number. Maximum uplink GBR usage ratio. Not present for NB-IoT cells.
  
} amarisoft_cell_t;

typedef struct amarisoft_cell_flag_t 
{ 
  bool cell_id;
  bool dl_bitrate;          
  bool ul_bitrate;          
  bool mbms_bitrate;        
  bool dl_tx;               
  bool ul_tx;               
  bool dl_retx;             
  bool ul_retx;             
  bool dl_err;              
  bool ul_err;              
  bool dl_sched_users_min;  
  bool dl_sched_users_avg;  
  bool dl_sched_users_max;  
  bool ul_sched_users_min;  
  bool ul_sched_users_avg;  
  bool ul_sched_users_max;  
  bool dl_use_min;          
  bool dl_use_avg;          
  bool dl_use_max;          
  bool ul_use_min;          
  bool ul_use_avg;          
  bool ul_use_max;          
  bool ctrl_use_min;        
  bool ctrl_use_avg;        
  bool ctrl_use_max;        
  bool ue_count_min;        
  bool ue_count_max;        
  bool ue_count_avg;        
  bool erab_count_min;      
  bool erab_count_max;     
  bool erab_count_avg;     
  bool drb_count_min;      
  bool drb_count_max;      
  bool drb_count_avg;      
  bool dl_gbr_use_min;     
  bool dl_gbr_use_avg;     
  bool dl_gbr_use_max;     
  bool ul_gbr_use_min;     
  bool ul_gbr_use_avg;     
  bool ul_gbr_use_max;     
} amarisoft_cell_flag_t;

typedef struct amarisoft_ran_stats_flags_t {
  amarisoft_cell_flag_t cells_stats_flags[AMARISOFT_MAX_CELL_NUM];  // gNB stats presence flags
  bool gtp_tx_bitrate;
  bool gtp_rx_bitrate;
  // TODO: rf_ports
} amarisoft_ran_stats_flags_t;

typedef struct amarisoft_ran_stats_t {
  size_t len_cell;
  amarisoft_cell_t      cells[AMARISOFT_MAX_CELL_NUM];        // gNB stats
  double gtp_tx_bitrate;
  double gtp_rx_bitrate;
  // TODO: rf_ports
} amarisoft_ran_stats_t;

typedef struct ue_cell_t {
  int    cell_id;
  double cqi;
  double ri;
  double ul_rank;
  double dl_bitrate;
  double ul_bitrate;
  int    dl_tx;
  int    ul_tx;
  int    dl_retx;
  int    ul_retx;
  double dl_mcs;
  double ul_mcs; 
  double ul_n_layer;
  double turbo_decoder_min;
  double turbo_decoder_avg;
  double turbo_decoder_max;
  double pucch1_snr;
  double pusch_snr;
  double ul_phr;
  double epre;
  double ul_path_loss;
  double p_ue;
  double initial_ta;
} ue_cell_t;

typedef struct ue_cell_flag_t {
  bool cell_id;
  bool cqi;
  bool ri;
  bool ul_rank;
  bool dl_bitrate;
  bool ul_bitrate;
  bool dl_tx;
  bool ul_tx;
  bool dl_retx;
  bool ul_retx;
  bool dl_mcs;
  bool ul_mcs; 
  bool ul_n_layer;
  bool turbo_decoder_min;
  bool turbo_decoder_avg;
  bool turbo_decoder_max;
  bool pucch1_snr;
  bool pusch_snr;
  bool ul_phr;
  bool epre;
  bool ul_path_loss;
  bool p_ue;
  bool initial_ta;
} ue_cell_flag_t;


typedef struct amr_erabs_t {
  double erab_id;
  double qci;
  int dl_total_bytes;
  int ul_total_bytes;
} amr_erabs_t;

typedef struct amr_erabs_flags_t {
  bool erab_id;  
  bool qci;
  bool dl_total_bytes;
  bool ul_total_bytes;
} amr_erabs_flags_t;

typedef struct amr_qos_flow_t {
  int pdu_session_id;
  int sst;
  int64_t dl_total_bytes;
  int64_t ul_total_bytes;
} amr_qos_flow_t;

typedef struct amr_qos_flow_flag_t {
  bool pdu_session_id;
  bool sst;
  bool dl_total_bytes;
  bool ul_total_bytes;
} amr_qos_flow_flag_t;

typedef struct qfi_list_t {
    int qfi;
    int _5gi;
    int dl_mbr;
    int dl_gbr;
    int ul_mbr;
    int ul_gbr;
} qfi_list_t;

typedef struct amr_qos_flow_stats_t {
  int ran_ue_id;
  int pdu_session_id;
  int sd;
  int sst;
  size_t len_qfi_list;
  qfi_list_t qfi_list[AMARISOFT_MAX_QOS_NUM]; // Array of objects. List of QoS Flows associated with this bearer.
  int64_t dl_total_bytes;
  int64_t ul_total_bytes;
} amr_qos_flow_stats_t;

typedef struct amr_qos_flow_stats_flags_t
{
  bool ran_ue_id;
  bool pdu_session_id;
  bool sst;
  bool sd; //Optional, Slice Differentiator.
  bool len_qfi_list;
  struct {
    bool qfi;
    bool _5gi;
    bool dl_mbr;// optional, available only if QoS Flow is GBR
    bool dl_gbr;// optional, available only if QoS Flow is GBR
    bool ul_mbr;// optional, available only if QoS Flow is GBR
    bool ul_gbr;// optional, available only if QoS Flow is GBR
    bool presence;
  } qfi_list[AMARISOFT_MAX_QOS_NUM]; // Array of objects. List of QoS Flows associated with this bearer.
  bool dl_total_bytes;
  bool ul_total_bytes;
} amr_qos_flow_stats_flags_t;

typedef struct amarisoft_ue_stats_t
{
  int                   time;
  int                   enb_ue_id;  // optional: A value of 0 means it is not present.
  int                   ran_ue_id;  // optional: A value of 0 means it is not present.
  int                   mme_ue_id;  // optional: A value of 0 means it is not present.
  int                   amf_ue_id;  // optional: A value of 0 means it is not present.
  int                   linked_enb_ue_id;// optional: A value of 0 means it is not present.
  int                   linked_ran_ue_id;// optional: A value of 0 means it is not present.
  int                   rnti;       // RNTI will identify each UE. A value of 0 means it is not present.
  size_t                len_cell;
  ue_cell_t             cells[AMARISOFT_MAX_CELL_NUM];
  ue_cell_flag_t        cell_flags[AMARISOFT_MAX_CELL_NUM];     // cell stats presence flag
  size_t                len_erab;
  amr_erabs_t           erabs[AMARISOFT_MAX_QOS_NUM];
  amr_erabs_flags_t     erabs_flags[AMARISOFT_MAX_QOS_NUM];
  size_t                len_qos_flow;
  amr_qos_flow_t        qos_flows[AMARISOFT_MAX_QOS_NUM];       // QOS statistics (for NR UEs)
  amr_qos_flow_flag_t   qos_flows_flags[AMARISOFT_MAX_QOS_NUM]; 
} amarisoft_ue_stats_t;

typedef enum {
    nr_dl_qam_64,
    nr_dl_qam_256
} nr_dl_qam_e;

typedef enum {
    nr_ul_qam_64,
    nr_ul_qam_256
} nr_ul_qam_e;

typedef struct nr_cell_conf_t {
  int cell_id;
  int n_antenna_dl;
  int n_antenna_ul;
  int n_layer_dl;
  int n_layer_ul;
  double gain;
  bool ul_disabled;
  int rf_port;
  char *label;
  nr_dl_qam_e dl_qam;
  nr_ul_qam_e ul_qam;
  nr_cgi_t ncgi;
  int n_id_nrcell;
  int band;
  int dl_nr_arfcn;
  int ul_nr_arfcn;
  int n_rb_dl;
  int n_rb_ul;
  int ssb_nr_arfcn;
  int dl_mu;
  int ul_mu;
  int ssb_mu;
  char *mode;
  int prach_sequence_index;
  amr_conn_mobility_t connected_mobility;
  size_t len_scell;
  amr_scell_list_t scell_list[AMARISOFT_MAX_SECONDARY_CELL_NUM];
  size_t len_ncell;
  amr_ncell_list_t ncell_list[AMARISOFT_MAX_SECONDARY_CELL_NUM]; // Optional array of objects listing the configured E-UTRA or NR cells
  size_t len_nr_dc_scg_cell;
  amr_en_dc_scg_cell_list_t nr_dc_scg_cell_list[AMARISOFT_MAX_SECONDARY_CELL_NUM];
  size_t len_plmn_list;
  nr_plmn_list_t plmn_list[AMARISOFT_MAX_PLMN_NUM];
} nr_cell_conf_t;

typedef struct nr_cell_conf_flag_t {
  bool cell_id;
  bool n_antenna_dl;
  bool n_antenna_ul;
  bool n_layer_dl;
  bool n_layer_ul;
  bool gain;
  bool ul_disabled;
  bool rf_port;
  bool label;
  bool dl_qam;
  bool ul_qam;
  bool n_id_nrcell;
  bool band;
  bool dl_nr_arfcn;
  bool ul_nr_arfcn;
  bool n_rb_dl;
  bool n_rb_ul;
  bool ssb_nr_arfcn;
  bool dl_mu;
  bool ul_mu;
  bool ssb_mu;
  bool mode; 
  bool prach_sequence_index;
  bool ncgi;
  struct {
    bool scell_config;
    bool nr_handover;
    bool nr_cell_redirect;
    bool eutra_handover;
    bool eutra_cell_redirect;             
    bool nr_dc_setup;   
    bool mr_dc_release; 
  } connected_mobility;
  struct {
    bool cell_id;
    bool ul_allowed;
  } scell_list[AMARISOFT_MAX_SECONDARY_CELL_NUM];
  struct{
    bool rat;
    bool dl_earfcn;
    bool n_id_cell;
    bool ecgi;
    bool ssb_nr_arfcn;
    bool n_id_nrcell;
    bool ncgi;
    bool handover_target;
    bool cell_redirect_target;
    bool eps_fallback_target;
    bool emergency_fallback_target;
  } ncell_list[AMARISOFT_MAX_SECONDARY_CELL_NUM];
} nr_cell_conf_flag_t;

typedef struct ran_config_t 
{
  // amarisoft api: config_get
  global_e2_node_id_t global_e2_node_id;
  size_t len_nr_cell;
  nr_cell_conf_t nr_cells[AMARISOFT_MAX_CELL_NUM];
  nr_cell_conf_flag_t nr_cells_flag[AMARISOFT_MAX_CELL_NUM]; // presence flags
} ran_config_t;

/* Internal representation of RAN received messages for gNB and ue statistics
 * This is a generic data structure that will be used to map the info we want for being returned via the indication
 * message of various SMs (i.e. KPM/MAC etc..). This data structure has to be SM agnostic
 */
typedef struct ran_ind_t {
    // amarisoft api: config_get
    global_e2_node_id_t global_e2_node_id;
    ran_config_t ran_config;
    // amarisoft api: stats
    amarisoft_ran_stats_t ran_stats;
    amarisoft_ran_stats_flags_t ran_stats_flags;
    // amarisoft api: ue_get
    size_t len_ue_stats;
    amarisoft_ue_stats_t  ue_stats[AMARISOFT_MAX_UE_NUM];
    // amarisoft api: qos_flow_get
    size_t len_qos_flow_stats;
    amr_qos_flow_stats_t     qos_flow_stats[AMARISOFT_MAX_UE_NUM];
    amr_qos_flow_stats_flags_t qos_flow_stats_flags[AMARISOFT_MAX_UE_NUM];
} ran_ind_t;

// free memory associated to ran_config_t datastructure
void free_ran_config(ran_config_t *conf);


typedef struct ran_ser_abs_t {
  ran_msghdr_t (*get_ran_msghdr)(const ran_msg_t *msg);
  void (*free_ran_msghdr)(ran_msghdr_t msg);

  bool (*decode_indication_stats)(const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_indication_ue_get) (const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_indication_config_get) (const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_config_get)(const ran_msg_t *in_msg, ran_config_t *out_ran, ran_ind_t *out_ind);
  bool (*decode_indication_qos_flow_get) (const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_indication_ue_erab_get) (const ran_msg_t *in_msg, ran_ind_t *out);
  bool (*decode_e2setup)(const ran_msg_t *msg, ran_config_t *out);
  bool (*decode_ctrl)(const ran_msghdr_t *msg, ctrl_ev_reply_t *out, const ws_ioloop_event_t *sent_ev);

  const char * (*encode_e2setup)(int message_id);
  const char * (*encode_indication)(int message_id, int sm_id, double initial_delay);
  const char * (*encode_ctrl)(int message_id, const sm_ag_if_wr_ctrl_t ctrl_msg, const ran_config_t* ran_config);

}ran_ser_abs_t; 

ran_ser_abs_t *ran_ser_get_instance(void);

#endif