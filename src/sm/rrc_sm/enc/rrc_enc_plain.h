#ifndef RRC_ENCRYPTION_PLAIN_H
#define RRC_ENCRYPTION_PLAIN_H 

#include "../../../util/byte_array.h"
#include "../ie/rrc_data_ie.h"


// Used for static polymorphism. 
// View rrc_enc_generic file
typedef struct{

} rrc_enc_plain_t;


byte_array_t rrc_enc_event_trigger_plain(rrc_event_trigger_t const* event_trigger);

byte_array_t rrc_enc_action_def_plain(rrc_action_def_t const*);

byte_array_t rrc_enc_ind_hdr_plain(rrc_ind_hdr_t const*); 

byte_array_t rrc_enc_ind_msg_plain(rrc_ind_msg_t const*); 

byte_array_t rrc_enc_call_proc_id_plain(rrc_call_proc_id_t const*); 

byte_array_t rrc_enc_ctrl_hdr_plain(rrc_ctrl_hdr_t const*); 

byte_array_t rrc_enc_ctrl_msg_plain(rrc_ctrl_msg_t const*); 

byte_array_t rrc_enc_ctrl_out_plain(rrc_ctrl_out_t const*); 

byte_array_t rrc_enc_func_def_plain(rrc_func_def_t const*);

#endif

