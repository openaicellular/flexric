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


#ifndef SM_READ_INTERFACE_H
#define SM_READ_INTERFACE_H

// Interface between the SM and the agent/server. 
// The SM can call the functions here defined and implemented on the RAN/server to read data.

#include "../../mac_sm/ie/mac_data_ie.h"
#include "../../rlc_sm/ie/rlc_data_ie.h"
#include "../../pdcp_sm/ie/pdcp_data_ie.h"
#include "../../rrc_sm/ie/rrc_data_ie.h"
#include "../../slice_sm/ie/slice_data_ie.h"
#include "../../tc_sm/ie/tc_data_ie.h"
#include "../../gtp_sm/ie/gtp_data_ie.h"
#include "../../kpm_sm_v2.02/ie/kpm_data_ie.h"


typedef enum{
  MAC_STATS_V0,
  RLC_STATS_V0,
  PDCP_STATS_V0,
  RRC_STATS_V0,
  SLICE_STATS_V0,
  TC_STATS_V0,
  GTP_STATS_V0, 
  KPM_STATS_V0, 
  SM_AGENT_IF_READ_V0_END,
} sm_ag_if_rd_e;

// Do not change the order of data in the struct
typedef struct{
  union {
    mac_ind_data_t mac_stats;
    rlc_ind_data_t rlc_stats;
    pdcp_ind_data_t pdcp_stats;
    rrc_ind_data_t rrc_stats;
    slice_ind_data_t slice_stats;
    tc_ind_data_t tc_stats;
    gtp_ind_data_t gtp_stats;
    kpm_ind_data_t kpm_stats;
  };
  sm_ag_if_rd_e type;
} sm_ag_if_rd_t;


void free_sm_ag_if_rd(sm_ag_if_rd_t* d);

sm_ag_if_rd_t cp_sm_ag_if_rd(sm_ag_if_rd_t const* d);

#endif

