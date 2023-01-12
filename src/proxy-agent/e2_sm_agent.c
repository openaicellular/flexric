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
#include "../sm/kpm_sm_v2.02/ie/kpm_data_ie.h"
#include "../src/util/time_now_us.h"

#include "e2_sm_agent.h"
#include "ws_ran_if.h"
#include "proxy_agent.h"

// TBC: using fake date as workaround until we are ready with real ringbuffer implementation
#include "ws_msg_hdlr.h"
#include "../test/sm/common/fill_ind_data.h"
#include "ringbuffer.h"

#define get_ringbuffer_kpm_data fill_kpm_ind_data
// static void get_ringbuffer_kpm_data(kpm_ind_data_t *kpm_stats) {

//   ws_ind_t ind = get_ringbuffer_data();
// }
/* 
 * We arrive here when the timer set by subscription procedure in agent/msg_handler_agent.c:e2ap_handle_subscription_request_agent()
 * expires. 
 * XXX-BUG: you have a race condition with the WS timer. You should wait reading data until the first timer is expired. check thes issue 
 * descripion in ws_ran_if.c:loop_callback() use case LWS_CALLBACK_TIMER 
 */
void e2_read_RAN(sm_ag_if_rd_t *data)
{
  assert( data->type == MAC_STATS_V0    ||
          data->type == PDCP_STATS_V0   ||
          data->type == KPM_STATS_V0);

  lwsl_user("[E2 Agent]: reading data from ringbuffer\n");
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
    // TODO: need to get ue_count(from amr ws) to define num_ue
    const size_t num_ue = 1;
    ind_msg->MeasData_len = num_ue;
    if(num_ue <= 0) {

      ind_msg->MeasData = calloc(1, sizeof(adapter_MeasDataItem_t));
      assert(ind_msg->MeasData != NULL && "memory exhausted");

      ind_msg->MeasData[0].measRecord_len = 1;
      ind_msg->MeasData[0].incompleteFlag = 0;

      ind_msg->MeasData[0].measRecord = calloc(1, sizeof(adapter_MeasRecord_t));
      assert(ind_msg->MeasData[0].measRecord != NULL && "memory exhausted");

      ind_msg->MeasData[0].measRecord[0].type = MeasRecord_int;
      ind_msg->MeasData[0].measRecord[0].int_val = 0;

      ind_msg->MeasInfo_len = 0;
      ind_msg->MeasInfo = NULL;
      ind_msg->granulPeriod = NULL;
    } else {
      ind_msg->MeasData = calloc(num_ue, sizeof(adapter_MeasDataItem_t));
      assert(ind_msg->MeasData != NULL && "memory exhausted");

      const int num_ms = 2;
      for (size_t i = 0; i < num_ue; ++i) {
        ind_msg->MeasData[i].measRecord_len = num_ms;
        ind_msg->MeasData[i].incompleteFlag = -1;

        if(num_ms > 0) {
          ind_msg->MeasData[i].measRecord = calloc(num_ms, sizeof(adapter_MeasRecord_t));
          assert(ind_msg->MeasData[i].measRecord != NULL && "memory exhausted");

          ind_msg->MeasData[i].measRecord[0].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[0].real_val = temp.cells_stats[0].dl_bitrate;
          ind_msg->MeasData[i].measRecord[1].type = MeasRecord_real;
          ind_msg->MeasData[i].measRecord[1].real_val = temp.cells_stats[0].ul_bitrate;
        }
      }
      ind_msg->granulPeriod = NULL;

      ind_msg->MeasInfo_len = num_ms;
      if (num_ms > 0) {
        ind_msg->MeasInfo = calloc(num_ms, sizeof(MeasInfo_t));
        assert(ind_msg->MeasInfo != NULL && "Memory exhausted" );

        // measinfo 1
        ind_msg->MeasInfo[0].meas_type = KPM_V2_MEASUREMENT_TYPE_NAME;
        char* measName1 = "DL_BITRATE";
        ind_msg->MeasInfo[0].measName.len = strlen(measName1) + 1;
        ind_msg->MeasInfo[0].measName.buf = malloc(strlen(measName1) + 1) ;
        assert(ind_msg->MeasInfo[0].measName.buf != NULL && "memory exhausted");
        memcpy(ind_msg->MeasInfo[0].measName.buf, measName1, strlen(measName1));
        ind_msg->MeasInfo[0].measName.buf[strlen(measName1)] = '\0';

        ind_msg->MeasInfo[0].labelInfo_len = 1;
        ind_msg->MeasInfo[0].labelInfo = calloc(ind_msg->MeasInfo[0].labelInfo_len, sizeof(adapter_LabelInfoItem_t));
        assert(ind_msg->MeasInfo[0].labelInfo != NULL && "memory exhausted");
        ind_msg->MeasInfo[0].labelInfo[0].noLabel = calloc(1, sizeof(long));
        assert( ind_msg->MeasInfo[0].labelInfo[0].noLabel != NULL && "memory exhausted");
        *(ind_msg->MeasInfo[0].labelInfo[0].noLabel) = 0;

        // measinfo 2
        ind_msg->MeasInfo[1].meas_type = KPM_V2_MEASUREMENT_TYPE_NAME;
        char* measName2 = "UL_BITRATE";
        ind_msg->MeasInfo[1].measName.len = strlen(measName2) + 1;
        ind_msg->MeasInfo[1].measName.buf = malloc(strlen(measName2) + 1) ;
        assert(ind_msg->MeasInfo[1].measName.buf != NULL && "memory exhausted");
        memcpy(ind_msg->MeasInfo[1].measName.buf, measName2, strlen(measName2));
        ind_msg->MeasInfo[1].measName.buf[strlen(measName2)] = '\0';

        ind_msg->MeasInfo[1].labelInfo_len = 1;
        ind_msg->MeasInfo[1].labelInfo = calloc(ind_msg->MeasInfo[0].labelInfo_len, sizeof(adapter_LabelInfoItem_t));
        assert(ind_msg->MeasInfo[1].labelInfo != NULL && "memory exhausted");
        ind_msg->MeasInfo[1].labelInfo[0].noLabel = calloc(1, sizeof(long));
        assert( ind_msg->MeasInfo[1].labelInfo[0].noLabel != NULL && "memory exhausted");
        *(ind_msg->MeasInfo[1].labelInfo[0].noLabel) = 0;
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
