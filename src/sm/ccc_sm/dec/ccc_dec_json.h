/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */


#ifndef CCC_DECRYPTION_JSON_H
#define CCC_DECRYPTION_JSON_H

#include <stddef.h>
#include "../ie/ccc_data_ie.h"


e2sm_ccc_event_trigger_t ccc_dec_event_trigger_json(size_t len, uint8_t const ev_tr[len]);

e2sm_ccc_action_def_t ccc_dec_action_def_json(size_t len, uint8_t const action_def[len]);

e2sm_ccc_ind_hdr_t ccc_dec_ind_hdr_json(size_t len, uint8_t const ind_hdr[len]); 

e2sm_ccc_ind_msg_t ccc_dec_ind_msg_json(size_t len, uint8_t const ind_msg[len]); 

e2sm_ccc_cpid_t ccc_dec_cpid_json(size_t len, uint8_t const call_proc_id[len]);

e2sm_ccc_ctrl_hdr_t ccc_dec_ctrl_hdr_json(size_t len, uint8_t const ctrl_hdr[len]); 

e2sm_ccc_ctrl_msg_t ccc_dec_ctrl_msg_json(size_t len, uint8_t const ctrl_msg[len]); 

e2sm_ccc_ctrl_out_t ccc_dec_ctrl_out_json(size_t len, uint8_t const ctrl_out[len]); 

e2sm_ccc_func_def_t ccc_dec_func_def_json(size_t len, uint8_t const func_def[len]);

#endif

