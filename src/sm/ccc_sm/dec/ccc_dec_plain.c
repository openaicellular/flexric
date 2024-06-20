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
/*
 * Decoding interface for plain text encoding Information Elements in SM-KPM
 */


#include "ccc_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

e2sm_ccc_event_trigger_t ccc_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  assert(0!=0 && "Not implemented");
  assert(ev_tr != NULL);
}

e2sm_ccc_action_def_t ccc_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
}

e2sm_ccc_ind_hdr_t ccc_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(0!=0 && "Not implemented");
  assert(ind_hdr != NULL);
}

e2sm_ccc_ind_msg_t ccc_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{
  assert(0!=0 && "Not implemented");
  assert(ind_msg != NULL);
}

e2sm_ccc_cpid_t ccc_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
}

e2sm_ccc_ctrl_hdr_t ccc_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(0!=0 && "Not implemented");
  assert(ctrl_hdr!= NULL);
}

e2sm_ccc_ctrl_msg_t ccc_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  assert(0!=0 && "Not implemented");
  assert(ctrl_msg!= NULL);
}

e2sm_ccc_ctrl_out_t ccc_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len])
{
  assert(0!=0 && "Not implemented");
  assert(ctrl_out!= NULL);
}

e2sm_ccc_func_def_t ccc_dec_func_def_plain(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(func_def != NULL);
}