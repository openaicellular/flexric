#ifndef RRC_DECRYPTION_GENERIC
#define RRC_DECRYPTION_GENERIC 

#include "rrc_dec_asn.h"
#include "rrc_dec_fb.h"
#include "rrc_dec_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define rrc_dec_event_trigger(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_event_trigger_plain, \
                           rrc_enc_asn_t*: rrc_dec_event_trigger_asn,\
                           rrc_enc_fb_t*: rrc_dec_event_trigger_fb,\
                           default: rrc_dec_event_trigger_plain) (U,V)

#define rrc_dec_action_def(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_action_def_plain, \
                           rrc_enc_asn_t*: rrc_dec_action_def_asn, \
                           rrc_enc_fb_t*: rrc_dec_action_def_fb, \
                           default:  rrc_dec_action_def_plain) (U,V)

#define rrc_dec_ind_hdr(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_ind_hdr_plain , \
                           rrc_enc_asn_t*: rrc_dec_ind_hdr_asn, \
                           rrc_enc_fb_t*: rrc_dec_ind_hdr_fb, \
                           default:  rrc_dec_ind_hdr_plain) (U,V)

#define rrc_dec_ind_msg(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_ind_msg_plain , \
                           rrc_enc_asn_t*: rrc_dec_ind_msg_asn, \
                           rrc_enc_fb_t*: rrc_dec_ind_msg_fb, \
                           default:  rrc_dec_ind_msg_plain) (U,V)

#define rrc_dec_call_proc_id(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_call_proc_id_plain , \
                           rrc_enc_asn_t*: rrc_dec_call_proc_id_asn, \
                           rrc_enc_fb_t*: rrc_dec_call_proc_id_fb, \
                           default:  rrc_dec_call_proc_id_plain) (U,V)

#define rrc_dec_ctrl_hdr(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_ctrl_hdr_plain , \
                           rrc_enc_asn_t*: rrc_dec_ctrl_hdr_asn, \
                           rrc_enc_fb_t*: rrc_dec_ctrl_hdr_fb, \
                           default: rrc_dec_ctrl_hdr_plain) (U,V)

#define rrc_dec_ctrl_msg(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_ctrl_msg_plain , \
                           rrc_enc_asn_t*: rrc_dec_ctrl_msg_asn, \
                           rrc_enc_fb_t*: rrc_dec_ctrl_msg_fb, \
                           default:  rrc_dec_ctrl_msg_plain) (U,V)

#define rrc_dec_ctrl_out(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_ctrl_out_plain , \
                           rrc_enc_asn_t*: rrc_dec_ctrl_out_asn, \
                           rrc_enc_fb_t*: rrc_dec_ctrl_out_fb, \
                           default:  rrc_dec_ctrl_out_plain) (U,V)

#define rrc_dec_func_def(T,U,V) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_dec_func_def_plain, \
                           rrc_enc_asn_t*: rrc_dec_func_def_asn, \
                           rrc_enc_fb_t*:  rrc_dec_func_def_fb, \
                           default:  rrc_dec_func_def_plain) (U,V)

#endif

