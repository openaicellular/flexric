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

#ifndef FILL_INDICATION_DATA_MIR_H
#define FILL_INDICATION_DATA_MIR_H 

#include "../../../src/sm/mac_sm/ie/mac_data_ie.h"
#include "../../../src/sm/rlc_sm/ie/rlc_data_ie.h"
#include "../../../src/sm/pdcp_sm/ie/pdcp_data_ie.h"
#include "../../../src/sm/rrc_sm/ie/rrc_data_ie.h"
#include "../../../src/sm/slice_sm/ie/slice_data_ie.h"
#include "../../../src/sm/tc_sm/ie/tc_data_ie.h"
#include "../../../src/sm/gtp_sm/ie/gtp_data_ie.h"
#include "../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.h"

void fill_mac_ind_data(mac_ind_data_t* ind);

void fill_rlc_ind_data(rlc_ind_data_t* ind);

void fill_pdcp_ind_data(pdcp_ind_data_t* ind);

void fill_rrc_ind_data(rrc_ind_data_t* ind);

void fill_slice_ind_data(slice_ind_data_t* ind);

void fill_slice_ctrl(slice_ctrl_req_data_t* ctrl);

void fill_tc_ind_data(tc_ind_data_t* ind_msg);

void fill_tc_ctrl(tc_ctrl_req_data_t* ctrl);

void fill_gtp_ind_data(gtp_ind_data_t* ind);
void fill_kpm_ind_data(kpm_ind_data_t* ind);

#endif

