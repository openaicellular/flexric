#ifndef RRC_ENCRYPTION_FLATBUFFERS_H
#define RRC_ENCRYPTION_FLATBUFFERS_H

#include "../../../util/byte_array.h"
#include "../ie/rrc_data_ie.h"


// Used for static polymorphism. 
// View rrc_enc_generic file
typedef struct{

} rrc_enc_fb_t;

byte_array_t rrc_enc_event_trigger_fb(rrc_event_trigger_t const* event_trigger);

byte_array_t rrc_enc_action_def_fb(rrc_action_def_t const*);

byte_array_t rrc_enc_ind_hdr_fb(rrc_ind_hdr_t const*); 

byte_array_t rrc_enc_ind_msg_fb(rrc_ind_msg_t const*); 

byte_array_t rrc_enc_call_proc_id_fb(rrc_call_proc_id_t const*); 

byte_array_t rrc_enc_ctrl_hdr_fb(rrc_ctrl_hdr_t const*); 

byte_array_t rrc_enc_ctrl_msg_fb(rrc_ctrl_msg_t const*); 

byte_array_t rrc_enc_ctrl_out_fb(rrc_ctrl_out_t const*); 

byte_array_t rrc_enc_func_def_fb(rrc_func_def_t const*);


#endif

