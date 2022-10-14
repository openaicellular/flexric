#ifndef RRC_ENCRYPTION_GENERIC
#define RRC_ENCRYPTION_GENERIC 

#include "rrc_enc_asn.h"
#include "rrc_enc_fb.h"
#include "rrc_enc_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define rrc_enc_event_trigger(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_event_trigger_plain, \
                           rrc_enc_asn_t*: rrc_enc_event_trigger_asn,\
                           rrc_enc_fb_t*: rrc_enc_event_trigger_fb,\
                           default: rrc_enc_event_trigger_plain) (U)

#define rrc_enc_action_def(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_action_def_plain, \
                           rrc_enc_asn_t*: rrc_enc_action_def_asn, \
                           rrc_enc_fb_t*: rrc_enc_action_def_fb, \
                           default:  rrc_enc_action_def_plain) (U)

#define rrc_enc_ind_hdr(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_ind_hdr_plain , \
                           rrc_enc_asn_t*: rrc_enc_ind_hdr_asn, \
                           rrc_enc_fb_t*: rrc_enc_ind_hdr_fb, \
                           default:  rrc_enc_ind_hdr_plain) (U)

#define rrc_enc_ind_msg(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_ind_msg_plain , \
                           rrc_enc_asn_t*: rrc_enc_ind_msg_asn, \
                           rrc_enc_fb_t*: rrc_enc_ind_msg_fb, \
                           default:  rrc_enc_ind_msg_plain) (U)

#define rrc_enc_call_proc_id(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_call_proc_id_plain , \
                           rrc_enc_asn_t*: rrc_enc_call_proc_id_asn, \
                           rrc_enc_fb_t*: rrc_enc_call_proc_id_fb, \
                           default:  rrc_enc_call_proc_id_plain) (U)

#define rrc_enc_ctrl_hdr(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_ctrl_hdr_plain , \
                           rrc_enc_asn_t*: rrc_enc_ctrl_hdr_asn, \
                           rrc_enc_fb_t*: rrc_enc_ctrl_hdr_fb, \
                           default:  rrc_enc_ctrl_hdr_plain) (U)

#define rrc_enc_ctrl_msg(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_ctrl_msg_plain , \
                           rrc_enc_asn_t*: rrc_enc_ctrl_msg_asn, \
                           rrc_enc_fb_t*: rrc_enc_ctrl_msg_fb, \
                           default:  rrc_enc_ctrl_msg_plain) (U)

#define rrc_enc_ctrl_out(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_ctrl_out_plain , \
                           rrc_enc_asn_t*: rrc_enc_ctrl_out_asn, \
                           rrc_enc_fb_t*: rrc_enc_ctrl_out_fb, \
                           default:  rrc_enc_ctrl_out_plain) (U)

#define rrc_enc_func_def(T,U) _Generic ((T), \
                           rrc_enc_plain_t*: rrc_enc_func_def_plain, \
                           rrc_enc_asn_t*: rrc_enc_func_def_asn, \
                           rrc_enc_fb_t*:  rrc_enc_func_def_fb, \
                           default:  rrc_enc_func_def_plain) (U)

#endif

