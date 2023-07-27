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

#define AMARISOFT_MAX_CELL_NUM 10
#define AMARISOFT_MAX_UE_NUM   10
 
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
  double  dl_use_avg;          // umber between 0 and 1. Average downlink usage ratio, based on number of allocated resource blocks.
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

typedef struct ue_cell_t {
  int    cell_id;
  double cqi;
  double ri;
  double dl_bitrate;
  double ul_bitrate;
  int    dl_tx;
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
  int    ul_phr;
  int    epre;
  int    ul_path_loss;
  int    p_ue;
  int    initial_ta;
}ue_cell_t;

typedef struct amarisoft_uestats_t 
{
  int       rnti; // RNTI will identify each UE
  int       num_cell;
  ue_cell_t cell[AMARISOFT_MAX_CELL_NUM];
}amarisoft_uestats_t;

/* Internal representation of RAN received messages for gNB and ue statistics 
 * This is a generic data structure that will be used to map the info we want for being returned via the indication 
 * message of various SMs (i.e. KPM/MAC etc..). This data structure has to be SM agnostic
 */
typedef struct ran_ind_t {
  amarisoft_cell_t    cells_stats[AMARISOFT_MAX_CELL_NUM];  // gNB stats
  amarisoft_uestats_t ue_stats[AMARISOFT_MAX_UE_NUM];       // ue stats
  int                 n_connected_ue;                       // extracted from ue_stats
} ran_ind_t;


/*  
 * This function will manage messages coming from the RAN endpoint in encoded form and returns 
 * a message response if needed.
 */
next_msg_t ran_msg_handle(const char *buf, size_t len, bi_map_t *sent_msg);


#endif