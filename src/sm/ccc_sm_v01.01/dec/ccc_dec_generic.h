#ifndef CCC_DECRYPTION_GENERIC
#define CCC_DECRYPTION_GENERIC 

// #include "ccc_dec_json.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v03.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////

#define ccc_dec_event_trigger(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_event_trigger_json, \
                           default: ccc_dec_event_trigger_json) (U,V)
                          
#define ccc_dec_action_def(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_action_def_json, \
                           default: ccc_dec_action_def_json) (U,V)

#define ccc_dec_ind_hdr(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_ind_hdr_json, \
                           default:  ccc_dec_ind_hdr_json) (U,V)

#define ccc_dec_ind_msg(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_ind_msg_json, \
                           default:  ccc_dec_ind_msg_json) (U,V)

#define ccc_dec_ctrl_hdr(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_ctrl_hdr_json, \
                           default:  ccc_dec_ctrl_hdr_json) (U,V)

#define ccc_dec_ctrl_msg(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_ctrl_msg_json, \
                           default:  ccc_dec_ctrl_msg_json) (U,V)

#define ccc_dec_ctrl_out(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_ctrl_out_json, \
                           default:  ccc_dec_ctrl_out_json) (U,V)

#define ccc_dec_func_def(T,U,V) _Generic ((T), \
                           ccc_dec_json_t*: ccc_dec_func_def_json, \
                           default:  ccc_dec_func_def_json) (U,V)
          



#endif
