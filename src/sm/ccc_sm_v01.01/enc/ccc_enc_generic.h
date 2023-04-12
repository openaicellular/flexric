#ifndef CCC_ENCRYPTION_GENERIC
#define CCC_ENCRYPTION_GENERIC 

// #include "ccc_enc_json.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v03.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////

#define ccc_enc_event_trigger(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_event_trigger_json,\
                           default: ccc_enc_event_trigger_json) (U)

#define ccc_enc_action_def(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_action_def_json, \
                           default:  ccc_enc_action_def_json) (U)

#define ccc_enc_ind_hdr(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_ind_hdr_json, \
                           default:  ccc_enc_ind_hdr_json) (U)

#define ccc_enc_ind_msg(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_ind_msg_json, \
                           default:  ccc_enc_ind_msg_json) (U)

#define ccc_enc_ctrl_hdr(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_ctrl_hdr_json, \
                           default:  ccc_enc_ctrl_hdr_json) (U)

#define ccc_enc_ctrl_msg(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_ctrl_msg_json, \
                           default:  ccc_enc_ctrl_msg_json) (U)

#define ccc_enc_ctrl_out(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_ctrl_out_json, \
                           default:  ccc_enc_ctrl_out_json) (U)

#define ccc_enc_func_def(T,U) _Generic ((T), \
                           ccc_enc_json_t*: ccc_enc_func_def_json, \
                           default:  ccc_enc_func_def_json) (U)

#endif
