/* interface for managing encoding/decoding in json for WS interface*/
#ifndef MSG_JSON_WS_H 
#define MSG_JSON_WS_H

#include "lib/ap/e2ap_types/common/e2ap_global_node_id.h"
#include "ws_msg_hdlr.h"

/******  1. Decoding JSON messages ********/
/* 
 * extract the message type extracted from value associated to json keyword "message" and "message_id"
 * RETURN: a malloced string representing the message type that you will have to free with ws_msghdr_free(), NULL on error 
*/
ws_msghdr_t ws_json_get_msghdr(const ws_msg_t *msg);

void ws_msghdr_free(ws_msghdr_t msg);
/* 
 * extract from a MSG of type json reply to "stats" request, the data neede to fill the gnb statistic in indication message as OUT
 */
bool ws_json_decode_indication_gnb (const ws_msg_t *msg, ws_ind_t *out);
/* 
extract from a MSG of type json reply to "ue_get" request, the data neede to fill the ue statistic in indication message as OUT 
*/
bool ws_json_decode_indication_ue (const ws_msg_t *in_msg, ws_ind_t *out);
/* 
 * parse 'config_get' message to extract the info needed by E2 setup request. 
 * RETURN: false if malformed json, otherwise true
 */
bool ws_json_decode_e2setup (const ws_msg_t *msg, global_e2_node_id_t *out);

/******* 2. Encoding JSON messages *********/
const char * ws_json_encode_e2setup(int msg_id);
const char * ws_json_encode_indication(int msg_id, int sm_id);
#endif