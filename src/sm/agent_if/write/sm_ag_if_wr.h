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


#ifndef E2AP_SM_WR_IF_H
#define E2AP_SM_WR_IF_H 

#include "subscribe_timer.h"
#include "../../pdcp_sm/ie/pdcp_data_ie.h"
#include "../../rlc_sm/ie/rlc_data_ie.h"
#include "../../mac_sm/ie/mac_data_ie.h"
#include "../../slice_sm/ie/slice_data_ie.h"
#include "../../tc_sm/ie/tc_data_ie.h"
#include "../../gtp_sm/ie/gtp_data_ie.h"
#include "../../kpm_sm_v2.02/ie/kpm_data_ie.h"

typedef enum{
  SUBSCRIBE_TIMER = 0,
  MAC_CTRL_REQ_V0 = 1,
  RLC_CTRL_REQ_V0 = 2,
  PDCP_CTRL_REQ_V0 = 3,
  SLICE_CTRL_REQ_V0 = 4,
  TC_CTRL_REQ_V0 = 5,
  GTP_CTRL_REQ_V0 = 6,
  SM_AGENT_IF_WRITE_V0_END,
} sm_ag_if_wr_e;

typedef struct {
  union{
    subscribe_timer_t sub_timer;  
    mac_ctrl_req_data_t mac_ctrl;
    rlc_ctrl_req_data_t rlc_ctrl;
    pdcp_ctrl_req_data_t pdcp_req_ctrl;
    slice_ctrl_req_data_t slice_req_ctrl;
    tc_ctrl_req_data_t tc_req_ctrl;
    gtp_ctrl_req_data_t gtp_ctrl;
  }; 
  sm_ag_if_wr_e type;
} sm_ag_if_wr_t;

#endif

