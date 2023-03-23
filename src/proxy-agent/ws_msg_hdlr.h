/* interface for handling messages from and to Websockets interface */
#ifndef WS_MSG_HDLR_H 
#define WS_MSG_HDLR_H 

#include <stdio.h>
#include "lib/ap/e2ap_types/common/e2ap_plmn.h"


#include "ws_ran_if.h"
#include "e2_sm_agent.h"
#include "proxy_agent.h"

typedef enum{
      // msgs coming from RAN
      WS_REGISTRATION_ACK,
      WS_AUTHENTICATE_ACK,
      WS_INDICATION_RECV,
      WS_CONFIG_RECV,
      WS_CTRL_ACK,
      WS_READY,
      // msgs for the RAN
      WS_INDICATION_REQ
}ws_type_id_t;

// raw json message in text or in binary form coming from RAN
typedef struct ws_msg_t { 
  char *buf;
  size_t len; // we put len to deal with the case you have binary data instead of text, where a string would suffice.
} ws_msg_t;

// message coming from RAN decoded
typedef struct {
  char *type;
  int msg_id;
  ws_type_id_t typeid;
} ws_msghdr_t;

#define AMARISOFT_MAX_CELL_NUM 10
#define AMARISOFT_MAX_UE_NUM   10
 
typedef struct amarisoft_cell_t {
  // XXX: I doubt on some datatypes below where spec of Amarisoft says it is float. To me should be integer (i.e. ue_count). to be tested
  int   cell_id;
  double dl_bitrate;         // Downlink bitrate in bits per seconds at PHY layer level (Counts acknowledged transmissions).
  double ul_bitrate;         // Uplink bitrate in bits per seconds at PHY layer level (Counts successful transmissions).
  double ue_count_avg;       // average number of connected UE 
  int   dl_tx;              // Number of downlink transmitted transport blocks (without retransmissions).
  int   ul_tx;              // Number of received uplink transport blocks (without CRC error).
  int   dl_retx;            // Number of downlink retransmitted transport blocks.
  int   ul_retx;            // Number of received uplink transport blocks with CRC errors.
  // TBC: etc....more to add following spec
}amarisoft_cell_t;

typedef struct amarisoft_uestats_t 
{
  // XXX: I doubt on some datatypes below where spec of Amarisoft says it is float. To me should be integer/to be tested
  int    rnti; // RNTI will identify each UE
  double dl_bitrate;
  double ul_bitrate;
  int    dl_tx;
  int    dl_retx;
  double dl_mcs;
  double ul_mcs; 
  int    phr;
  double cqi;
// TBC: etc....more to add following spec
} amarisoft_uestats_t;

/* Internal representation of RAN received messages for gNB and ue statistics 
 * This is a generic data structure that will be used to map the info we want for being returned via the indication 
 * message of various SMs (i.e. KPM/MAC etc..). This data structure has to be SM agnostic
 */
typedef struct ws_ind_t {
  amarisoft_cell_t    cells_stats[AMARISOFT_MAX_CELL_NUM];  // gNB stats
  amarisoft_uestats_t ue_stats[AMARISOFT_MAX_UE_NUM];       // ue stats
  int                 n_connected_ue;                       // extracted from ue_stats
} ws_ind_t;

e2_agent_msg_t ws_msg_handle(proxy_agent_t *p, const ws_msg_t* msg);

#endif