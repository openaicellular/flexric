#ifndef RRC_ENCRYPTIOIN_ASN_H
#define RRC_ENCRYPTIOIN_ASN_H

#include "../../../util/byte_array.h"
#include "../ie/rrc_data_ie.h"

// Used for static polymorphism. 
// See rrc_enc_generic.h file
typedef struct{

} rrc_enc_asn_t;

byte_array_t rrc_enc_event_trigger_asn(rrc_event_trigger_t const* event_trigger);

byte_array_t rrc_enc_action_def_asn(rrc_action_def_t const*);

byte_array_t rrc_enc_ind_hdr_asn(rrc_ind_hdr_t const*); 

byte_array_t rrc_enc_ind_msg_asn(rrc_ind_msg_t const*); 

byte_array_t rrc_enc_call_proc_id_asn(rrc_call_proc_id_t const*); 

byte_array_t rrc_enc_ctrl_hdr_asn(rrc_ctrl_hdr_t const*); 

byte_array_t rrc_enc_ctrl_msg_asn(rrc_ctrl_msg_t const*); 

byte_array_t rrc_enc_ctrl_out_asn(rrc_ctrl_out_t const*); 

byte_array_t rrc_enc_func_def_asn(rrc_func_def_t const*);

#endif

