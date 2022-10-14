#ifndef RRC_DECRYPTION_FLATBUFFERS_H
#define RRC_DECRYPTION_FLATBUFFERS_H

#include <stddef.h>
#include "../ie/rrc_data_ie.h"


rrc_event_trigger_t rrc_dec_event_trigger_fb(size_t len, uint8_t const ev_tr[len]);

rrc_action_def_t rrc_dec_action_def_fb(size_t len, uint8_t const action_def[len]);

rrc_ind_hdr_t rrc_dec_ind_hdr_fb(size_t len, uint8_t const ind_hdr[len]); 

rrc_ind_msg_t rrc_dec_ind_msg_fb(size_t len, uint8_t const ind_msg[len]); 

rrc_call_proc_id_t rrc_dec_call_proc_id_fb(size_t len, uint8_t const call_proc_id[len]);

rrc_ctrl_hdr_t rrc_dec_ctrl_hdr_fb(size_t len, uint8_t const ctrl_hdr[len]); 

rrc_ctrl_msg_t rrc_dec_ctrl_msg_fb(size_t len, uint8_t const ctrl_msg[len]); 

rrc_ctrl_out_t rrc_dec_ctrl_out_fb(size_t len, uint8_t const ctrl_out[len]); 

rrc_func_def_t rrc_dec_func_def_fb(size_t len, uint8_t const func[len]);

#endif

