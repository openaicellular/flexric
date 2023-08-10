/* 
 * Layer between flexric E2SM callbacks and WS interface
 * it manages SUBSCRIPTION procedure (indication) and CTRL procedure
 */
#include <stdio.h>

#include "../sm/mac_sm/ie/mac_data_ie.h"
#include "../sm/rlc_sm/ie/rlc_data_ie.h"
#include "../sm/pdcp_sm/ie/pdcp_data_ie.h"
#include "../sm/slice_sm/ie/slice_data_ie.h"
#include "../sm/tc_sm/ie/tc_data_ie.h"
#include "../sm/gtp_sm/ie/gtp_data_ie.h"
#include "../sm/kpm_sm/ie/kpm_data_ie.h"
#include "../src/util/time_now_us.h"

#include "e2_sm_agent.h"
#include "ws_ran_if.h"
#include "proxy_agent.h"

#include "ws_msg_hdlr.h"
#include "ringbuffer.h"

void e2_read_RAN(sm_ag_if_rd_t *data)
{
  assert( data->type == MAC_STATS_V0    ||
          data->type == PDCP_STATS_V0   ||
          data->type == KPM_STATS_V0);
  
  lwsl_debug("[E2 Agent]: reading data from ringbuffer\n");
  ws_ind_t temp = get_ringbuffer_data();
  
  if (data->type == MAC_STATS_V0) {
    lwsl_user("[E2 Agent]: filling MAC SM indication message\n");

    // TODO: need to get ue_count(from amr ws) to define num_ue
    const size_t num_ue = 1;
    mac_ind_msg_t* ind_msg = &data->mac_stats.msg;
    ind_msg->len_ue_stats = num_ue;
    ind_msg->tstamp = time_now_us();
    if (num_ue > 0) {
      ind_msg->ue_stats = calloc(num_ue, sizeof(mac_ue_stats_impl_t));
      assert(ind_msg->ue_stats != NULL && "memory exhausted");
    }
    for (size_t i = 0; i < ind_msg->len_ue_stats; ++i) {
      ind_msg->ue_stats[i].dl_aggr_tbs = temp.cells_stats[0].dl_tx;
      ind_msg->ue_stats[i].ul_aggr_tbs = temp.cells_stats[0].ul_tx;
      //TODO: need to get the other data from amr ws
      /*ind_msg->ue_stats[i].dl_aggr_bytes_sdus = abs(rand()%mod);
      ind_msg->ue_stats[i].ul_aggr_bytes_sdus = abs(rand()%mod);
      ind_msg->ue_stats[i].pusch_snr = 64.0;
      ind_msg->ue_stats[i].pucch_snr = 64.0;
      ind_msg->ue_stats[i].rnti = abs(rand()%mod);
      ind_msg->ue_stats[i].dl_aggr_prb = abs(rand()%mod);
      ind_msg->ue_stats[i].ul_aggr_prb = abs(rand()%mod);
      ind_msg->ue_stats[i].dl_aggr_sdus = abs(rand()%mod);
      ind_msg->ue_stats[i].ul_aggr_sdus= abs(rand()%mod);
      ind_msg->ue_stats[i].dl_aggr_retx_prb= abs(rand()%mod);
      ind_msg->ue_stats[i].ul_aggr_retx_prb= abs(rand()%mod);
      ind_msg->ue_stats[i].wb_cqi= abs(rand()%mod);
      ind_msg->ue_stats[i].dl_mcs1= abs(rand()%mod);
      ind_msg->ue_stats[i].ul_mcs1= abs(rand()%mod);
      ind_msg->ue_stats[i].dl_mcs2= abs(rand()%mod);
      ind_msg->ue_stats[i].ul_mcs2= abs(rand()%mod);
      ind_msg->ue_stats[i].phr= (rand()%64) - 23;
      ind_msg->ue_stats[i].bsr= abs(rand()%mod);
      ind_msg->ue_stats[i].dl_num_harq = numUlHarq;
      for (uint8_t j = 0; j < numDLHarq; j++)
        ind_msg->ue_stats[i].dl_harq[j] = abs(rand()%mod);
      ind_msg->ue_stats[i].ul_num_harq = numUlHarq;
      for (uint8_t j = 0; j < numUlHarq; j++)
        ind_msg->ue_stats[i].ul_harq[j] = abs(rand()%mod);*/
    }
  }
  else if (data->type == PDCP_STATS_V0) {
    lwsl_user("[E2 Agent]: filling PDCP SM indication message\n");
  }
  else if (data->type == KPM_STATS_V0) {
    lwsl_user("[E2 Agent]: filling KPM SM indication message\n");
    kpm_ind_hdr_t* ind_hdr = &data->kpm_stats.hdr;
    int64_t t = time_now_us();
    ind_hdr->collectStartTime = t/1000000;
    ind_hdr->fileFormatversion = NULL;
    ind_hdr->senderName = NULL;
    ind_hdr->senderType = NULL;
    ind_hdr->vendorName = NULL;

    kpm_ind_msg_t* ind_msg = &data->kpm_stats.msg;
    // Convention: high resolution (microseconds) timestamp is encapsulated as first measurement record to ease extraction at xapp level
    const size_t num_ue = temp.n_connected_ue;
    
    if(num_ue <= 0) {
      /* we put only gnb first cell, there is alwasy cell_number 1 in the report from amarisoft even if metrics 
       * are not interesting/useful. See example below:
       *
       *   "message": "stats",
       *   "instance_id": "63e64475",
       *   "cpu": {
       *     "global": 45.92274678111588
       *   },
       *   "cells": {
       *     "1": {
       *       "dl_bitrate": 0,
       *       "ul_bitrate": 0,
       *       "dl_tx": 0,
       *       "ul_tx": 0,
       *       "dl_retx:0
       *     }
       *   }
       */
      ind_msg->MeasData_len = 1; 
      ind_msg->MeasData = calloc(1, sizeof(adapter_MeasDataItem_t));
      assert(ind_msg->MeasData != NULL && "memory exhausted");
      
      ind_msg->MeasData[0].measRecord_len = 2;
      ind_msg->MeasData[0].incompleteFlag = 0;

      ind_msg->MeasData[0].measRecord = calloc(2, sizeof(adapter_MeasRecord_t));
      assert(ind_msg->MeasData[0].measRecord != NULL && "memory exhausted");

      ind_msg->MeasData[0].measRecord[0].type = MeasRecord_real;
      ind_msg->MeasData[0].measRecord[0].real_val = t;
      ind_msg->MeasData[0].measRecord[1].type = MeasRecord_int;
      ind_msg->MeasData[0].measRecord[1].int_val = 0;
      

      ind_msg->MeasInfo_len = 2;
      ind_msg->MeasInfo = calloc(ind_msg->MeasInfo_len, sizeof(MeasInfo_t));
      assert(ind_msg->MeasInfo != NULL && "Memory exhausted" );

      set_kpm_measName(&ind_msg->MeasInfo[0], "timestamp_us");
      set_kpm_measName(&ind_msg->MeasInfo[1], "null");
      
      ind_msg->granulPeriod = NULL;
    } else {
      ind_msg->MeasData_len = num_ue;
      ind_msg->MeasData = calloc(num_ue, sizeof(adapter_MeasDataItem_t));
      assert(ind_msg->MeasData != NULL && "memory exhausted");

      const int num_ms_per_ue = 9;
      /*
      GNB measurements
      
      int   cell_id;
      double dl_bitrate;    
      double ul_bitrate;    
      double ue_count_avg;  
      int   dl_tx;          
      int   ul_tx;          
      int   dl_retx;        
      int   ul_retx;   

      UE measurements
      int    rnti;
      double dl_bitrate;
      double ul_bitrate;
      int    dl_tx;
      int    dl_retx;
      double dl_mcs;
      double ul_mcs; 
      int    phr;
      double cqi;
      */
      for (size_t i = 0; i < num_ue; ++i) {
        ind_msg->MeasData[i].measRecord_len = num_ms_per_ue;
        ind_msg->MeasData[i].incompleteFlag = -1;

        if(num_ms_per_ue > 0) {
          ind_msg->MeasData[i].measRecord = calloc(num_ms_per_ue, sizeof(adapter_MeasRecord_t));
          assert(ind_msg->MeasData[i].measRecord != NULL && "memory exhausted");

          ind_msg->MeasData[i].measRecord[0].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[0].real_val = t;
          ind_msg->MeasData[i].measRecord[1].type = MeasRecord_int;
          ind_msg->MeasData[i].measRecord[1].int_val = temp.ue_stats[i].rnti;
          ind_msg->MeasData[i].measRecord[2].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[2].real_val = temp.ue_stats[i].ul_bitrate;
          ind_msg->MeasData[i].measRecord[3].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[3].real_val = temp.ue_stats[i].dl_bitrate;
          ind_msg->MeasData[i].measRecord[4].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[4].real_val =  (temp.ue_stats[i].dl_tx !=0) ? 
                                                          temp.ue_stats[i].dl_retx/(temp.ue_stats[i].dl_retx + temp.ue_stats[i].dl_tx) : 
                                                          0; // BLER
          ind_msg->MeasData[i].measRecord[5].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[5].real_val = temp.ue_stats[i].dl_mcs;
          ind_msg->MeasData[i].measRecord[6].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[6].real_val = temp.ue_stats[i].ul_mcs;
          ind_msg->MeasData[i].measRecord[7].type = MeasRecord_int;
          ind_msg->MeasData[i].measRecord[7].int_val = temp.ue_stats[i].phr;
          ind_msg->MeasData[i].measRecord[8].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[8].real_val = temp.ue_stats[i].cqi;
        }
      }
      // XXX: missing gnB measurements
      ind_msg->granulPeriod = NULL;

      ind_msg->MeasInfo_len = num_ms_per_ue;
      if (num_ms_per_ue > 0) {
        ind_msg->MeasInfo = calloc(ind_msg->MeasInfo_len, sizeof(MeasInfo_t));
        assert(ind_msg->MeasInfo != NULL && "Memory exhausted" );

        set_kpm_measName(&ind_msg->MeasInfo[0], "timestamp_us");
        set_kpm_measName(&ind_msg->MeasInfo[1], "rnti");
        set_kpm_measName(&ind_msg->MeasInfo[2], "ul_bitrate");
        set_kpm_measName(&ind_msg->MeasInfo[3], "dl_bitrate");
        set_kpm_measName(&ind_msg->MeasInfo[4], "bler");
        set_kpm_measName(&ind_msg->MeasInfo[5], "dl_mcs");
        set_kpm_measName(&ind_msg->MeasInfo[6], "ul_mcs");
        set_kpm_measName(&ind_msg->MeasInfo[7], "phr");
        set_kpm_measName(&ind_msg->MeasInfo[8], "cqi");
      }
    }
  }
  else {
    assert("unsupported data type");
  }
}

sm_ag_if_ans_t e2_write_RAN(sm_ag_if_wr_t const *data)
{
  assert(data != NULL);
  if (data->type == MAC_CTRL_REQ_V0)
  {
    sm_ag_if_ans_t ans = {.type = MAC_AGENT_IF_CTRL_ANS_V0};
    ans.mac.ans = MAC_CTRL_OUT_OK;
    return ans;
  }
  else if (data->type == SLICE_CTRL_REQ_V0)
  {

    slice_ctrl_req_data_t const *slice_req_ctrl = &data->slice_req_ctrl;
    slice_ctrl_msg_t const *msg = &slice_req_ctrl->msg;

    if (msg->type == SLICE_CTRL_SM_V0_ADD)
    {
      printf("[E2 Agent]: SLICE CONTROL ADD rx.\n");
      printf("SLICE CONTROL ADD: unimplemented.\n");
    }
    else if (msg->type == SLICE_CTRL_SM_V0_DEL)
    {
      printf("[E2 Agent]: SLICE CONTROL DEL rx\n");
      printf("SLICE CONTROL DEL: unimplemented.\n");
    }
    else if (msg->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC)
    {
      printf("[E2 Agent]: SLICE CONTROL ASSOC rx\n");
      printf("SLICE CONTROL ASSOC: unimplemented.\n");
    }
    else
    {
      assert(0 != 0 && "Unknown msg_type!");
    }

    sm_ag_if_ans_t ans = {.type = SLICE_AGENT_IF_CTRL_ANS_V0};
    return ans;
  }
  else if (data->type == TC_CTRL_REQ_V0)
  {
    tc_ctrl_req_data_t const *ctrl = &data->tc_req_ctrl;

    tc_ctrl_msg_e const t = ctrl->msg.type;

    assert(t == TC_CTRL_SM_V0_CLS || t == TC_CTRL_SM_V0_PLC || t == TC_CTRL_SM_V0_QUEUE || t == TC_CTRL_SM_V0_SCH || t == TC_CTRL_SM_V0_SHP || t == TC_CTRL_SM_V0_PCR);

    sm_ag_if_ans_t ans = {.type = TC_AGENT_IF_CTRL_ANS_V0};
    return ans;
  }
  else
  {
    assert(0 != 0 && "Not supported function ");
  }
  sm_ag_if_ans_t ans = {0};
  return ans;
}
