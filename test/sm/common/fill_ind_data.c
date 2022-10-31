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


#include "fill_ind_data.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static
int64_t time_now_us(void)
{
  struct timespec tms;

  /* The C11 way */
  /* if (! timespec_get(&tms, TIME_UTC))  */

  /* POSIX.1-2008 way */
  if (clock_gettime(CLOCK_REALTIME,&tms)) {
    return -1;
  }
  /* seconds, multiplied with 1 million */
  int64_t micros = tms.tv_sec * 1000000;
  /* Add full microseconds */
  micros += tms.tv_nsec/1000;
  /* round up if necessary */
  if (tms.tv_nsec % 1000 >= 500) {
    ++micros;
  }
  return micros;
}

void fill_mac_ind_data(mac_ind_data_t* ind)
{
  assert(ind != NULL);
  srand(time(0));

  int const mod = 1024;

  mac_ind_msg_t* ind_msg = &ind->msg; 
  
  int const NUM_UES = abs(rand()%5);

  ind_msg->len_ue_stats = NUM_UES;

  ind_msg->tstamp = time_now_us();

  if(NUM_UES > 0){
    ind_msg->ue_stats = calloc(NUM_UES, sizeof(mac_ue_stats_impl_t));
    assert(ind_msg->ue_stats != NULL && "memory exhausted");
  }

  const size_t numDLHarq = 4;
  const size_t numUlHarq = 4;

  for(uint32_t i = 0; i < ind_msg->len_ue_stats; ++i){
    ind_msg->ue_stats[i].dl_aggr_tbs = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_aggr_tbs = abs(rand()%mod);
    ind_msg->ue_stats[i].dl_aggr_bytes_sdus = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_aggr_bytes_sdus = abs(rand()%mod);
    ind_msg->ue_stats[i].pusch_snr = 64.0; //: float = -64;
    ind_msg->ue_stats[i].pucch_snr = 64.0; //: float = -64;
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
    ind_msg->ue_stats[i].phr= (rand()%64) - 23; // 41 abs(rand()%mod);
    ind_msg->ue_stats[i].bsr= abs(rand()%mod);
    ind_msg->ue_stats[i].dl_num_harq = numUlHarq;
    for (uint8_t j = 0; j < numDLHarq; j++)
      ind_msg->ue_stats[i].dl_harq[j] = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_num_harq = numUlHarq;
    for (uint8_t j = 0; j < numUlHarq; j++)
      ind_msg->ue_stats[i].ul_harq[j] = abs(rand()%mod);
  }
}

void fill_kpm_ind_data(kpm_ind_data_t* ind)
{
  assert(ind != NULL);
  
  srand(time(0));

  int64_t t = time_now_us();
  ind->hdr.collectStartTime = t / 1000000; // needs to be truncated to 32 bits to arrive to a resolution of seconds
  ind->hdr.fileFormatversion = NULL;
  ind->hdr.senderName = NULL;
  ind->hdr.senderType = NULL;
  ind->hdr.vendorName = NULL;

  if (rand()%100 == 0)
  {
    adapter_MeasDataItem_t *KPMData = calloc(1, sizeof(adapter_MeasDataItem_t));
    KPMData[0].measRecord_len = 1;
    KPMData[0].incompleteFlag = 0;

    adapter_MeasRecord_t * KPMRecord = calloc(KPMData[0].measRecord_len, sizeof(adapter_MeasRecord_t));
    KPMData[0].measRecord = KPMRecord;
    for (size_t i=0; i<KPMData[0].measRecord_len ; i++){
      KPMRecord[i].type = MeasRecord_int;
      KPMRecord[i].int_val = 0;
    }

    ind->msg.MeasData = KPMData;
    ind->msg.MeasData_len = 1;

    ind->msg.MeasInfo_len = 0;
    ind->msg.MeasInfo = NULL;
    ind->msg.granulPeriod = NULL;

  } else {

    adapter_MeasDataItem_t *KPMData = calloc(1, sizeof(adapter_MeasDataItem_t));
    KPMData[0].measRecord_len = rand()%100 + 1;
    KPMData[0].incompleteFlag =  -1;
    
    adapter_MeasRecord_t * KPMRecord = calloc(KPMData[0].measRecord_len, sizeof(adapter_MeasRecord_t));
    KPMData[0].measRecord = KPMRecord;
    for (size_t i=0; i<KPMData[0].measRecord_len ; i++){
      KPMRecord[i].type = MeasRecord_int;
      KPMRecord[i].int_val = rand();
    }
    

    ind->msg.MeasData = KPMData;
    ind->msg.MeasData_len = 1;

    ind->msg.granulPeriod = NULL;
    
    ind->msg.MeasInfo_len = 2;
    ind->msg.MeasInfo = calloc(ind->msg.MeasInfo_len, sizeof(MeasInfo_t));
    assert(ind->msg.MeasInfo != NULL && "Memory exhausted" );
    
    MeasInfo_t* info1 = &ind->msg.MeasInfo[0];
    assert(info1 != NULL && "memory exhausted");
    info1->meas_type = KPM_V2_MEASUREMENT_TYPE_NAME;
    char* measName = "PrbDlUsage";
    info1->measName.len = strlen(measName) + 1;
    info1->measName.buf = malloc(strlen(measName)+1) ;
    assert(info1->measName.buf != NULL && "memory exhausted");
    memcpy(info1->measName.buf, measName, strlen(measName) );
    info1->measName.buf[strlen(measName)] = '\0';
    info1->labelInfo_len = 1;
    info1->labelInfo = calloc(info1->labelInfo_len, sizeof(adapter_LabelInfoItem_t));
    assert(info1->labelInfo != NULL && "memory exhausted");
    adapter_LabelInfoItem_t* label1 = &info1->labelInfo[0];
    label1->noLabel = calloc(1, sizeof(long));
    assert(label1->noLabel != NULL && "memory exhausted");
    *(label1->noLabel) = 0;

    MeasInfo_t* info2 = &ind->msg.MeasInfo[1];
    assert(info2 != NULL && "memory exhausted");
    info2->meas_type = KPM_V2_MEASUREMENT_TYPE_ID;
    info2->measID = 1L;
    info2->labelInfo_len = 1;
    info2->labelInfo = calloc(info2->labelInfo_len, sizeof(adapter_LabelInfoItem_t));
    assert(info2->labelInfo != NULL && "memory exhausted");
    adapter_LabelInfoItem_t* label2 = &info2->labelInfo[0];
    label2->noLabel = calloc(1, sizeof(long));
    assert(label2->noLabel != NULL && "memory exhausted");
    *(label2->noLabel) = 0;
  }
}

void fill_rlc_ind_data(rlc_ind_data_t* ind)
{
  assert(ind != NULL);

  srand(time(0));

  int const mod = 1024;

  rlc_ind_msg_t* ind_msg = &ind->msg;

  ind_msg->tstamp = time_now_us();

  ind_msg->len = rand()%4;
  if(ind_msg->len > 0 ){
    ind_msg->rb = calloc(ind_msg->len, sizeof(rlc_radio_bearer_stats_t) );
    assert(ind_msg->rb != NULL);
  }

  for(uint32_t i = 0; i < ind_msg->len; ++i){
    rlc_radio_bearer_stats_t* rb = &ind_msg->rb[i];

    rb->txpdu_pkts=abs(rand()%mod) ;         /* aggregated number of transmitted RLC PDUs */
    rb->txpdu_bytes=abs(rand()%mod);        /* aggregated amount of transmitted bytes in RLC PDUs */
    /* TODO? */
    rb->txpdu_wt_ms=abs(rand()%mod);      /* aggregated head-of-line tx packet waiting time to be transmitted (i.e. send to the MAC layer) */
    rb->txpdu_dd_pkts=abs(rand()%mod);      /* aggregated number of dropped or discarded tx packets by RLC */
    rb->txpdu_dd_bytes=abs(rand()%mod);     /* aggregated amount of bytes dropped or discarded tx packets by RLC */
    rb->txpdu_retx_pkts=abs(rand()%mod);    /* aggregated number of tx pdus/pkts to be re-transmitted (only applicable to RLC AM) */
    rb->txpdu_retx_bytes=abs(rand()%mod);   /* aggregated amount of bytes to be re-transmitted (only applicable to RLC AM) */
    rb->txpdu_segmented=abs(rand()%mod);    /* aggregated number of segmentations */
    rb->txpdu_status_pkts=abs(rand()%mod);  /* aggregated number of tx status pdus/pkts (only applicable to RLC AM) */
    rb->txpdu_status_bytes=abs(rand()%mod); /* aggregated amount of tx status bytes  (only applicable to RLC AM) */
    /* TODO? */
    rb->txbuf_occ_bytes=abs(rand()%mod);    /* current tx buffer occupancy in terms of amount of bytes (average: NOT IMPLEMENTED) */
    /* TODO? */
    rb->txbuf_occ_pkts=abs(rand()%mod);     /* current tx buffer occupancy in terms of number of packets (average: NOT IMPLEMENTED) */
    /* txbuf_wd_ms: the time window for which the txbuf  occupancy value is obtained - NOT IMPLEMENTED */

    /* RX */
    rb->rxpdu_pkts=abs(rand()%mod);         /* aggregated number of received RLC PDUs */
    rb->rxpdu_bytes=abs(rand()%mod);        /* amount of bytes received by the RLC */
    rb->rxpdu_dup_pkts=abs(rand()%mod);     /* aggregated number of duplicate packets */
    rb->rxpdu_dup_bytes=abs(rand()%mod);    /* aggregated amount of duplicated bytes */
    rb->rxpdu_dd_pkts=abs(rand()%mod);      /* aggregated number of rx packets dropped or discarded by RLC */
    rb->rxpdu_dd_bytes=abs(rand()%mod);     /* aggregated amount of rx bytes dropped or discarded by RLC */
    rb->rxpdu_ow_pkts=abs(rand()%mod);      /* aggregated number of out of window received RLC pdu */
    rb->rxpdu_ow_bytes=abs(rand()%mod);     /* aggregated number of out of window bytes received RLC pdu */
    rb->rxpdu_status_pkts=abs(rand()%mod);  /* aggregated number of rx status pdus/pkts (only applicable to RLC AM) */
    rb->rxpdu_status_bytes=abs(rand()%mod); /* aggregated amount of rx status bytes  (only applicable to RLC AM) */
    /* rxpdu_rotout_ms: flag indicating rx reordering  timeout in ms - NOT IMPLEMENTED */
    /* rxpdu_potout_ms: flag indicating the poll retransmit time out in ms - NOT IMPLEMENTED */
    /* rxpdu_sptout_ms: flag indicating status prohibit timeout in ms - NOT IMPLEMENTED */
    /* TODO? */
    rb->rxbuf_occ_bytes=abs(rand()%mod);    /* current rx buffer occupancy in terms of amount of bytes (average: NOT IMPLEMENTED) */
    /* TODO? */
    rb->rxbuf_occ_pkts=abs(rand()%mod);     /* current rx buffer occupancy in terms of number of packets (average: NOT IMPLEMENTED) */

    /* SDU stats */
    /* TX */
    rb->txsdu_pkts=abs(rand()%mod);         /* number of SDUs delivered */
    rb->txsdu_bytes=abs(rand()%mod);        /* number of bytes of SDUs delivered */

    /* RX */
    rb->rxsdu_pkts=abs(rand()%mod);         /* number of SDUs received */
    rb->rxsdu_bytes=abs(rand()%mod);        /* number of bytes of SDUs received */
    rb->rxsdu_dd_pkts=abs(rand()%mod);      /* number of dropped or discarded SDUs */
    rb->rxsdu_dd_bytes=abs(rand()%mod);     /* number of bytes of SDUs dropped or discarded */

    rb->rnti=abs(rand()%mod);
    rb->mode=abs(rand()%3);               /* 0: RLC AM, 1: RLC UM, 2: RLC TM */
    rb->rbid=abs(rand()%16);

  }
}


void fill_pdcp_ind_data(pdcp_ind_data_t* ind)
{
  assert(ind != NULL);

  srand(time(0));

  pdcp_ind_msg_t* ind_msg = &ind->msg; 

  ind_msg->len = abs(rand()%8);
  ind_msg->tstamp = time_now_us();

  if(ind_msg->len > 0){
    ind_msg->rb = calloc(ind_msg->len, sizeof(pdcp_radio_bearer_stats_t));
    assert(ind_msg->rb != NULL && "Memory exhausted!");
  }

  for(uint32_t i = 0; i < ind_msg->len; ++i){
    pdcp_radio_bearer_stats_t* rb = &ind_msg->rb[i];

    int const mod = 1024;
    rb->txpdu_bytes = abs(rand()%mod);    /* aggregated bytes of tx packets */
    rb->txpdu_pkts = rb->txpdu_bytes != 0 ? rb->txpdu_bytes - rand()%rb->txpdu_bytes : 0;     /* aggregated number of tx packets */
    rb->txpdu_sn= abs(rand()%mod);       /* current sequence number of last tx packet (or TX_NEXT) */

    rb->rxpdu_bytes = abs(rand()%mod);    /* aggregated bytes of rx packets */
    rb->rxpdu_pkts = rb->rxpdu_bytes != 0 ? rb->rxpdu_bytes - rand()%rb->rxpdu_bytes : 0;     /* aggregated number of rx packets */
    rb->rxpdu_sn= abs(rand()%mod);       /* current sequence number of last rx packet (or  RX_NEXT) */
    rb->rxpdu_oo_pkts= abs(rand()%mod);       /* aggregated number of out-of-order rx pkts  (or RX_REORD) */
    rb->rxpdu_oo_bytes= abs(rand()%mod); /* aggregated amount of out-of-order rx bytes */
    rb->rxpdu_dd_pkts= abs(rand()%mod);  /* aggregated number of duplicated discarded packets (or dropped packets because of other reasons such as integrity failure) (or RX_DELIV) */
    rb->rxpdu_dd_bytes= abs(rand()%mod); /* aggregated amount of discarded packets' bytes */
    rb->rxpdu_ro_count= abs(rand()%mod); /* this state variable indicates the COUNT value following the COUNT value associated with the PDCP Data PDU which triggered t-Reordering. (RX_REORD) */
    rb->txsdu_bytes = abs(rand()%mod);    /* number of bytes of SDUs delivered */
    rb->txsdu_pkts = rb->txsdu_bytes != 0 ? rb->txsdu_bytes - rand()% rb->txsdu_bytes : 0;     /* number of SDUs delivered */

    rb->rxsdu_bytes = abs(rand()%mod);    /* number of bytes of SDUs received */
    rb->rxsdu_pkts =  rb->rxsdu_bytes != 0 ? rb->rxsdu_bytes - rand()%rb->rxsdu_bytes : 0;     /* number of SDUs received */

    rb->rnti= abs(rand()%mod);
    rb->mode= abs(rand()%3);               /* 0: PDCP AM, 1: PDCP UM, 2: PDCP TM */
    rb->rbid= abs(rand()%11);
  }
}

static
void fill_static_slice(static_slice_t* sta)
{
  assert(sta != NULL);

  sta->pos_high = abs(rand()%25);
  sta->pos_low = abs(rand()%25);
}


static
void fill_nvs_slice(nvs_slice_t* nvs)
{
  assert(nvs != NULL);

  const uint32_t type = abs(rand() % SLICE_SM_NVS_V0_END);

  if(type == SLICE_SM_NVS_V0_RATE ){
    nvs->conf = SLICE_SM_NVS_V0_RATE; 
    nvs->u.rate.u2.mbps_reference = 0.8; 
//      10.0*((float)rand()/(float)RAND_MAX); 
    nvs->u.rate.u1.mbps_required = 10.0;
    //*((float)rand()/(float)RAND_MAX); 
  } else if(type ==SLICE_SM_NVS_V0_CAPACITY ){
    nvs->conf = SLICE_SM_NVS_V0_CAPACITY; 
    nvs->u.capacity.u.pct_reserved = 15.0;
    //*((float)rand()/(float)RAND_MAX);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

static
void fill_scn19_slice(scn19_slice_t* scn19)
{
  assert(scn19 != NULL);

  const uint32_t type = abs(rand()% SLICE_SCN19_SM_V0_END);

  if(type == SLICE_SCN19_SM_V0_DYNAMIC ){
    scn19->conf = SLICE_SCN19_SM_V0_DYNAMIC ;
    scn19->u.dynamic.u2.mbps_reference = 10.0 * fabs((float)rand()/(float)RAND_MAX); 
    scn19->u.dynamic.u1.mbps_required = 8.0 * fabs((float)rand()/(float)RAND_MAX); 
  } else if(type == SLICE_SCN19_SM_V0_FIXED ) {
    scn19->conf = SLICE_SCN19_SM_V0_FIXED; 
    scn19->u.fixed.pos_high = abs(rand()%14);
    scn19->u.fixed.pos_low = abs(rand()%10);
  } else if(type ==SLICE_SCN19_SM_V0_ON_DEMAND){
    scn19->conf = SLICE_SCN19_SM_V0_ON_DEMAND;
//    scn19->u.on_demand.log_delta_byte = abs(rand()%121);
    scn19->u.on_demand.log_delta = 1.0 * fabs((float)rand()/RAND_MAX);
    scn19->u.on_demand.tau = abs(rand()%256);
    scn19->u.on_demand.pct_reserved = fabs((float)rand()/(float)RAND_MAX);
  } else {
    assert(0 != 0 && "Unknown type!!");
  }

}

static 
void fill_edf_slice(edf_slice_t* edf)
{
  assert(edf != NULL);

  int mod = 32;
  edf->deadline = abs(rand()%mod);
  edf->guaranteed_prbs = abs(rand()%mod);
  edf->max_replenish = abs(rand()%mod);

  edf->len_over = abs(rand()%mod);

  if(edf->len_over > 0){
    edf->over = calloc(edf->len_over, sizeof(uint32_t));
    assert(edf->over != NULL && "Memory exhausted");
  }

  for(uint32_t i = 0; i < edf->len_over; ++i){
    edf->over[i] = abs(rand()%mod);
  }
}

static
void fill_ul_dl_slice(ul_dl_slice_conf_t* slice)
{
  assert(slice != NULL);

  char const* name = "MY SLICE";
  slice->len_sched_name = strlen(name);
  slice->sched_name = malloc(strlen(name));
  assert(slice->sched_name != NULL && "memory exhausted");
  memcpy(slice->sched_name, name, strlen(name));

  slice->len_slices = abs(rand()%4);

  if(slice->len_slices > 0){
    slice->slices = calloc(slice->len_slices, sizeof(fr_slice_t));
    assert(slice->slices != NULL && "memory exhausted");
  }

  for(uint32_t i = 0; i < slice->len_slices; ++i){
    slice->slices[i].id = abs(rand()%1024);
    fr_slice_t* s = &slice->slices[i];

    const char* label = "This is my label";
    s->len_label = strlen(label);
    s->label = malloc(s->len_label);
    assert(s->label != NULL && "Memory exhausted");
    memcpy(s->label, label, s->len_label );

    const char* sched_str = "Scheduler string";
    s->len_sched = strlen(sched_str); 
    s->sched = malloc(s->len_sched);
    assert(s->sched != NULL && "Memory exhausted");
    memcpy(s->sched, sched_str, s->len_sched);

    uint32_t type = abs(rand()% SLICE_ALG_SM_V0_END);

    if(type == SLICE_ALG_SM_V0_NONE || SLICE_ALG_SM_V0_SCN19)
      type = SLICE_ALG_SM_V0_STATIC; 


    if(type == SLICE_ALG_SM_V0_NONE ){
      s->params.type =SLICE_ALG_SM_V0_NONE; 
    } else if (type == SLICE_ALG_SM_V0_STATIC ){
      s->params.type = SLICE_ALG_SM_V0_STATIC; 
      fill_static_slice(&s->params.u.sta);
    } else if (type == SLICE_ALG_SM_V0_NVS){
      s->params.type =  SLICE_ALG_SM_V0_NVS; 
      fill_nvs_slice(&s->params.u.nvs);
    } else if (type == SLICE_ALG_SM_V0_SCN19) {
      s->params.type = SLICE_ALG_SM_V0_SCN19; 
      fill_scn19_slice(&s->params.u.scn19);
    } else if (type == SLICE_ALG_SM_V0_EDF){
      s->params.type =  SLICE_ALG_SM_V0_EDF; 
      fill_edf_slice(&s->params.u.edf);
    } else {
      assert(0 != 0 && "Unknown type encountered");
    }
  }
}

static
void fill_slice_conf(slice_conf_t* conf)
{
  assert(conf != NULL);

  fill_ul_dl_slice(&conf->ul);
  fill_ul_dl_slice(&conf->dl);
}

static
void fill_ue_slice_conf(ue_slice_conf_t* conf)
{
  assert(conf != NULL);
  conf->len_ue_slice = abs(rand()%10);
  if(conf->len_ue_slice > 0){
    conf->ues = calloc(conf->len_ue_slice, sizeof(ue_slice_assoc_t));
    assert(conf->ues != NULL && "memory exhausted");
  }

  for(uint32_t i = 0; i < conf->len_ue_slice; ++i){
    conf->ues[i].rnti = abs(rand()%1024);  
    conf->ues[i].dl_id = abs(rand()%16); 
    conf->ues[i].ul_id = abs(rand()%16); 
  }

}

static
void fill_slice_del(del_slice_conf_t* conf)
{
  assert(conf != NULL);

  uint32_t const len_dl = rand()%5;
  conf->len_dl = len_dl;
  if (conf->len_dl > 0) {
    conf->dl = calloc(len_dl, sizeof(uint32_t));
    assert(conf->dl != NULL && "memory exhausted");
  }
  for (uint32_t i = 0; i < conf->len_dl; ++i)
    conf->dl[i] = abs(rand()%16);

  uint32_t const len_ul = rand()%5;
  conf->len_ul = len_ul;
  if (conf->len_ul > 0) {
    conf->ul = calloc(len_ul, sizeof(uint32_t));
    assert(conf->ul != NULL && "memory exhausted");
  }
  for (uint32_t i = 0; i < conf->len_ul; ++i)
    conf->ul[i] = abs(rand()%16);

}

void fill_slice_ind_data(slice_ind_data_t* ind_msg)
{
  assert(ind_msg != NULL);

  srand(time(0));

  fill_slice_conf(&ind_msg->msg.slice_conf);
  fill_ue_slice_conf(&ind_msg->msg.ue_slice_conf);
  ind_msg->msg.tstamp = time_now_us();
}

void fill_slice_ctrl(slice_ctrl_req_data_t* ctrl)
{
   assert(ctrl != NULL);

   uint32_t type = rand()%SLICE_CTRL_SM_V0_END;
   ctrl->msg.type = type;

   if(type == SLICE_CTRL_SM_V0_ADD){
     fill_slice_conf(&ctrl->msg.u.add_mod_slice);
     //printf("SLICE_CTRL_SM_V0_ADD \n");
   } else if (type == SLICE_CTRL_SM_V0_DEL){
     fill_slice_del(&ctrl->msg.u.del_slice);
     //printf("SLICE_CTRL_SM_V0_DEL \n");
   } else if (type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
     fill_ue_slice_conf(&ctrl->msg.u.ue_slice); 
     //printf("SLICE_CTRL_SM_V0_MOD \n");
   } else {
      assert(0!=0 && "Unknown type");
   }
}


static
void fill_tc_mtr(tc_mtr_t* mtr)
{
  assert(mtr != NULL);

  mtr->time_window_ms = rand() % 124;
  mtr->bnd_flt = 16.3;
}

static
void fill_tc_sch(tc_sch_t* sch)
{
  assert(sch != NULL);
  sch->type = rand()%TC_SCHED_END;
  if(sch->type == TC_SCHED_RR){
//    sch->rr = ;
  } else if(sch->type == TC_SCHED_PRIO){
    sch->prio.len_q_prio = rand()%2;
    sch->prio.q_prio = calloc(sch->prio.len_q_prio, sizeof(sch->prio.q_prio) ); 
    assert(sch->prio.q_prio != NULL && "Memory exhausted");
    for(size_t i = 0; i < sch->prio.len_q_prio; ++i){
      sch->prio.q_prio[i] = rand()%16;
    }
  } else {
    assert(0!=0 && "Unknown scheduler type");
  }
}

static
void fill_tc_pcr(tc_pcr_t* pcr)
{
  assert(pcr != NULL);
  pcr->type = rand()% TC_PCR_END;
  
  if(pcr->type == TC_PCR_DUMMY){
    pcr->id = rand()%16;
    fill_tc_mtr(&pcr->mtr);   
  } else if(pcr->type == TC_PCR_5G_BDP){
    pcr->id = rand()%16;
    fill_tc_mtr(&pcr->mtr);   
  } else {
    assert(0 !=0 && "Unknown pacer");
  }
}

static
void fill_l3(L3_filter_t* l3)
{
  assert(l3 != NULL);
  l3->src_addr = rand()% 4096*1024;
  l3->dst_addr = rand()% 4096*1024;
}

static
void fill_l4(L4_filter_t* l4 )
{
  assert(l4 != NULL);

  l4->src_port = rand()%1024;
  l4->dst_port = rand()%1024;
  l4->protocol = 17;
}

static
void fill_l7(L7_filter_t* l7)
{
  assert(l7 != NULL);

}

static
void fill_tc_cls(tc_cls_t* cls)
{
  assert(cls != NULL);
  cls->type = rand()%TC_CLS_END; 

  if(cls->type == TC_CLS_RR){
    cls->rr.dummy = rand()%16;
  } else if(cls->type == TC_CLS_OSI){

    cls->osi.len = rand()%8;
    if(cls->osi.len > 0){
      cls->osi.flt = calloc(cls->osi.len, sizeof(tc_cls_osi_filter_t) );
      assert(cls->osi.flt != NULL && "memory exhausted");
    }

    for(size_t i = 0; i < cls->osi.len; ++i){
      cls->osi.flt[i].id = rand()%128;
      fill_l3(&cls->osi.flt[i].l3);
      fill_l4(&cls->osi.flt[i].l4);
      fill_l7(&cls->osi.flt[i].l7);
      cls->osi.flt[i].dst_queue = rand()%8;
    }

  } else if(cls->type == TC_CLS_STO){
    cls->sto.dummy = rand()%16;
  } else {
    assert("Unknown classifier type");
  }

}

static
void fill_tc_drp(tc_drp_t* drp)
{
  assert(drp != NULL);
  drp->dropped_pkts = rand()%1000;
}

static
void fill_tc_mrk(tc_mrk_t* mrk)
{
  mrk->marked_pkts = rand()%8;
}

static
void fill_tc_queue(tc_queue_t* q)
{
  assert(q != NULL);

  q->type = rand()%TC_QUEUE_END;
  q->id = rand()%16;

  if(q->type == TC_QUEUE_FIFO){
    q->fifo.bytes = rand()%10000;
    q->fifo.pkts = rand()%10000;
    q->fifo.bytes_fwd = rand()%10000;
    q->fifo.pkts_fwd = rand()%10000;
    q->fifo.avg_sojourn_time = 100.5;
    q->fifo.last_sojourn_time = rand()%100;
    fill_tc_drp(&q->fifo.drp);

  } else if(q->type == TC_QUEUE_CODEL){
    q->codel.bytes = rand()%10000;
    q->codel.pkts = rand()%10000;
    q->codel.bytes_fwd = rand()%10000;
    q->codel.pkts_fwd = rand()%10000;
    q->codel.avg_sojourn_time = 100.5;
    q->codel.last_sojourn_time = rand()%100;
    fill_tc_drp(&q->codel.drp);
  } else if(q->type == TC_QUEUE_ECN_CODEL){
    q->ecn.bytes = rand()%10000;
    q->ecn.pkts = rand()%10000;
    q->ecn.bytes_fwd = rand()%10000;
    q->ecn.pkts_fwd = rand()%10000;
    q->ecn.avg_sojourn_time = 100.5;
    q->ecn.last_sojourn_time = rand()%100;
    fill_tc_mrk(&q->ecn.mrk);
  } else {
    assert(0!=0 && "Unknown queue type");
  }
}

static
void fill_tc_shp(tc_shp_t* s)
{
  assert(s != NULL);

  s->id = rand()%16;
  s->active = rand()%2;
  fill_tc_mtr(&s->mtr);
}

static
void fill_tc_plc(tc_plc_t* p)
{
  assert(p != NULL);

  p->id = rand() % 1024;
  fill_tc_mtr(&p->mtr);
  fill_tc_drp(&p->drp);
  fill_tc_mrk(&p->mrk);
  p->max_rate_kbps = 124.5;
  p->active = rand()%2;
  p->dst_id = rand()%16;
  p->dev_id = rand()%16;
}


static
void fill_tc_shp_q_plc(tc_ind_msg_t* msg)
{
  assert(msg != NULL); 
  msg->len_q = rand()%16;

  msg->q = calloc(msg->len_q, sizeof(tc_queue_t));
  assert(msg->q != NULL && "Memory exhausted");
  
  msg->shp = calloc(msg->len_q, sizeof(tc_shp_t));
  assert(msg->shp != NULL && "Memory exhausted");

  msg->plc = calloc(msg->len_q, sizeof(tc_plc_t));
  assert(msg->plc != NULL && "Memory exhausted");

  for(size_t i = 0; i < msg->len_q; ++i){
    fill_tc_queue(&msg->q[i]);
    fill_tc_shp(&msg->shp[i]);
    fill_tc_plc(&msg->plc[i]);
  }

}



void fill_tc_ind_data(tc_ind_data_t* ind_msg)
{
  assert(ind_msg != NULL);

  srand(time(0));

  ind_msg->msg.tstamp = time_now_us();

  fill_tc_sch(&ind_msg->msg.sch);
  fill_tc_pcr(&ind_msg->msg.pcr);
  fill_tc_cls(&ind_msg->msg.cls);
  fill_tc_shp_q_plc(&ind_msg->msg);

}


static
void fill_tc_ctrl_cls_add(tc_add_ctrl_cls_t* add)
{
  assert(add != NULL);

  tc_cls_e type = rand() % TC_CLS_END;
  add->type = type;

  if(type == TC_CLS_RR){
    add->rr.dummy = rand()%16;
  } else if(type == TC_CLS_OSI){
    add->osi.dst_queue = rand()%16;
    fill_l3(&add->osi.l3);
    fill_l4(&add->osi.l4);
    fill_l7(&add->osi.l7);
  } else if(type == TC_CLS_STO){
    add->sto.dummy = rand()%16;
  } else {
    assert(0!=0 && "Unknown classifier type");    
  }
}

static
void fill_tc_ctrl_cls_del(tc_del_ctrl_cls_t* del)
{
  assert(del != NULL);

  tc_cls_e type = rand() % TC_CLS_END;
  del->type = type;

  if(type == TC_CLS_RR){
    del->rr.dummy = rand()%16;
  } else if(type == TC_CLS_OSI){
    del->osi.filter_id = rand()%256;
  } else if(type == TC_CLS_STO){
    del->sto.dummy = rand()%16;
  } else {
    assert(0!=0 && "Unknown classifier type");    
  }

}

static
void fill_tc_ctrl_cls_mod(tc_mod_ctrl_cls_t* mod)
{
  assert(mod != NULL);

  tc_cls_e type = rand() % TC_CLS_END;
  mod->type = type;

  if(type == TC_CLS_RR){
    mod->rr.dummy = rand()%16;
  } else if(type == TC_CLS_OSI){
    mod->osi.filter.id = rand()%256;
    mod->osi.filter.dst_queue = rand()%256;
    fill_l3(&mod->osi.filter.l3) ;
    fill_l4(&mod->osi.filter.l4) ;
    fill_l7(&mod->osi.filter.l7) ;
  } else if(type == TC_CLS_STO){
    mod->sto.dummy = rand()%16;
  } else {
    assert(0!=0 && "Unknown classifier type");    
  }
}

static
void fill_tc_ctrl_cls(tc_ctrl_cls_t* cls)
{
  assert(cls != NULL);

  cls->act = rand()%TC_CTRL_ACTION_SM_V0_END;

  if(cls->act == TC_CTRL_ACTION_SM_V0_ADD){
    fill_tc_ctrl_cls_add(&cls->add);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_DEL){
    fill_tc_ctrl_cls_del(&cls->del);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_MOD){
    fill_tc_ctrl_cls_mod(&cls->mod);
  } else {
    assert(0!=0 && "Unknown action type" ); 
  }


/*
  uint32_t type = rand() % TC_CLS_END;

  cls->type = type;

  if(type == TC_CLS_RR){
    cls->rr.dummy = rand()%16;
  } else if (type == TC_CLS_OSI){
    cls->osi.dummy = rand()%16;
  } else if (type == TC_CLS_STO){
    cls->sto.dummy = rand()%16;
  } else {
    assert("Unknown classifier type"); 
  }
*/


}

static
void fill_tc_ctrl_plc(tc_ctrl_plc_t* plc)
{
  assert(plc != NULL);

  tc_ctrl_act_e act = rand() % TC_CTRL_ACTION_SM_V0_END;
  plc->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
    plc->add.dummy = rand()%16;  
  } else if(act == TC_CTRL_ACTION_SM_V0_DEL){
    plc->del.id = rand()%8;
  } else if(act == TC_CTRL_ACTION_SM_V0_MOD){
    plc->mod.id = rand()%16;
    plc->mod.drop_rate_kbps = rand()%16;
    plc->mod.dev_id = rand()%16 ;
    plc->mod.dev_rate_kbps = rand()% 4096;
    plc->mod.active = rand()%2;
  } else {
    assert(0!=0 && "Unknown action type");
  }
}

static
void fill_tc_ctrl_queue_fifo( tc_ctrl_queue_fifo_t* fifo)
{
  assert(fifo != NULL);
  fifo->dummy = rand()%128;
}

static
void fill_tc_ctrl_queue_codel(tc_ctrl_queue_codel_t* codel)
{
  assert(codel != NULL);

  codel->interval_ms = rand()%256 + 100; 
  codel-> target_ms = rand()%10 + 5;
}

static
void fill_tc_ctrl_queue_ecn_codel(tc_ctrl_queue_ecn_codel_t* ecn)
{
  assert(ecn != NULL);

  ecn->interval_ms = rand()%256 + 100; 
  ecn->target_ms = rand()%10 + 5;
}

static
void fill_tc_ctrl_queue_add(tc_add_ctrl_queue_t* add)
{
  assert(add != NULL);

  add->type = rand() % TC_QUEUE_END;

  if(add->type == TC_QUEUE_FIFO ){
    fill_tc_ctrl_queue_fifo(&add->fifo);
  } else if(add->type == TC_QUEUE_CODEL){
    fill_tc_ctrl_queue_codel(&add->codel);
  } else if(add->type == TC_QUEUE_ECN_CODEL){
    fill_tc_ctrl_queue_ecn_codel(&add->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }
}

static
void fill_tc_ctrl_queue_del(tc_del_ctrl_queue_t* del)
{
  assert(del != NULL);

  del->id = rand() % 10;
  del->type = rand() % TC_QUEUE_END;
}

static
void fill_tc_ctrl_queue_mod(tc_mod_ctrl_queue_t* mod)
{
  assert(mod != NULL);

  mod->id = rand() % 10;
  mod->type = rand() % TC_QUEUE_END;

  if(mod->type == TC_QUEUE_FIFO ){
    fill_tc_ctrl_queue_fifo(&mod->fifo);
  } else if(mod->type == TC_QUEUE_CODEL){
    fill_tc_ctrl_queue_codel(&mod->codel);
  } else if(mod->type == TC_QUEUE_ECN_CODEL){
    fill_tc_ctrl_queue_ecn_codel(&mod->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }

}


static
void fill_tc_ctrl_queue(tc_ctrl_queue_t* q)
{
  assert(q != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  q->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
     fill_tc_ctrl_queue_add(&q->add);
  } else if (act == TC_CTRL_ACTION_SM_V0_DEL){ 
     fill_tc_ctrl_queue_del(&q->del);
  } else if (act == TC_CTRL_ACTION_SM_V0_MOD){
     fill_tc_ctrl_queue_mod(&q->mod);
  } else {
    assert(0!=0 && "Unknown actio type");
  }

/*
  tc_queue_e type = rand() % TC_QUEUE_END;
  q->type = type;
  q->id = rand() % 16;

  if(type == TC_QUEUE_FIFO){
    fill_tc_ctrl_queue_fifo(&q->fifo);
  } else if(type == TC_QUEUE_CODEL){
    fill_tc_ctrl_queue_codel(&q->codel);
  } else {
    assert(0!=0 && "Unknown queue type");
  }
*/

}

static
void fill_tc_ctrl_sch_add(tc_add_ctrl_sch_t* add)
{
  assert(add != NULL);

  add->dummy = rand()%16;
}

static
void fill_tc_ctrl_sch_del(tc_del_ctrl_sch_t* del)
{
  assert(del != NULL);

  del->dummy = rand()%16;
}

static
void fill_tc_ctrl_sch_mod(tc_mod_ctrl_sch_t* mod)
{
  assert(mod != NULL);

  tc_sch_e type = rand()%TC_SCHED_END;
  mod->type = type;

  if(type == TC_SCHED_RR){
    mod->rr.dummy = rand()%8;
  } else if(type == TC_SCHED_PRIO ){

  mod->prio.len_q_prio = rand()%8;
  if(mod->prio.len_q_prio > 0){
    mod->prio.q_prio = calloc(mod->prio.len_q_prio, sizeof(uint32_t));
    assert(mod->prio.q_prio != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < mod->prio.len_q_prio; ++i){
    mod->prio.q_prio[i] = rand()%16; 
  }

  } else {
    assert(0!=0 && "scheduler type unknown");
  }

}


static
void fill_tc_ctrl_sch(tc_ctrl_sch_t* sch)
{
  assert(sch != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  sch->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
     fill_tc_ctrl_sch_add(&sch->add);
  } else if (act == TC_CTRL_ACTION_SM_V0_DEL){ 
     fill_tc_ctrl_sch_del(&sch->del);
  } else if (act == TC_CTRL_ACTION_SM_V0_MOD){
     fill_tc_ctrl_sch_mod(&sch->mod);
  } else {
    assert(0!=0 && "Unknown actio type");
  }
/*
  sch->type = rand()%TC_SCHED_END;

  if(sch->type == TC_SCHED_RR){

  } else if(sch->type == TC_SCHED_PRIO){

  } else {
    assert(0!=0 && "Unknown scheduler type");
  }
*/

}

static
void fill_tc_mod_ctrl_shp(tc_mod_ctrl_shp_t* mod)
{
  assert(mod != NULL);

  mod->id = rand()%128;
  mod->time_window_ms = rand()%128; 
  mod->max_rate_kbps = rand()%1024;
  mod->active = rand()%2;
} 

static
void fill_tc_ctrl_shp(tc_ctrl_shp_t* shp)
{
  assert(shp != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  shp->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
    shp->add.dummy = rand()%16; 
  } else if(act == TC_CTRL_ACTION_SM_V0_DEL){
    shp->del.id = rand()%16; 
  } else if(act == TC_CTRL_ACTION_SM_V0_MOD){
     fill_tc_mod_ctrl_shp(&shp->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }
}

static
void fill_tc_mod_ctrl_pcr(tc_mod_ctrl_pcr_t* mod)
{
  assert(mod != NULL);

  tc_pcr_e type = rand()% TC_PCR_END;
  mod->type = type;

  if(type == TC_PCR_DUMMY){
    mod->dummy.dummy = rand()%16;
  } else if(type == TC_PCR_5G_BDP) {
    mod->bdp. drb_sz = rand()% 2048;
    mod->bdp.tstamp = 10000 + rand()% 2048;
  } else {
    assert(0!=0 && "unknown type");
  }

}


static
void fill_tc_ctrl_pcr(tc_ctrl_pcr_t* pcr)
{
  assert(pcr != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  pcr->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
    pcr->add.dummy = rand()%32;
  } else if(act == TC_CTRL_ACTION_SM_V0_DEL){
    pcr->del.dummy = rand()%32;
  } else if(act == TC_CTRL_ACTION_SM_V0_MOD){
    fill_tc_mod_ctrl_pcr(&pcr->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }

}

void fill_tc_ctrl(tc_ctrl_req_data_t* ctrl)
{
  assert(ctrl != NULL);

  ctrl->hdr.dummy = rand() % 16;

  uint32_t const type = rand()%TC_CTRL_SM_V0_END;
  ctrl->msg.type = type;

  if(type == TC_CTRL_SM_V0_CLS){
    fill_tc_ctrl_cls(&ctrl->msg.cls);
  } else if(type == TC_CTRL_SM_V0_PLC) {
    fill_tc_ctrl_plc(&ctrl->msg.plc);
  } else if(type == TC_CTRL_SM_V0_QUEUE){
    fill_tc_ctrl_queue(&ctrl->msg.q);
  } else if(type == TC_CTRL_SM_V0_SCH){
    fill_tc_ctrl_sch(&ctrl->msg.sch);
  } else if(type == TC_CTRL_SM_V0_SHP){
    fill_tc_ctrl_shp(&ctrl->msg.shp);
  } else if(type == TC_CTRL_SM_V0_PCR){
    fill_tc_ctrl_pcr(&ctrl->msg.pcr);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

void fill_gtp_ind_data(gtp_ind_data_t* ind){
  assert(ind != NULL);

  srand(time(0));

  int const mod = 1024;

  // Get indication message
  gtp_ind_msg_t* ind_msg = &ind->msg;
  
  // Set time now  
  ind_msg->tstamp = time_now_us();

  // Set random number of messages  
  ind_msg->len = rand()%4;
  if(ind_msg->len > 0 ){  
    ind_msg->ngut = calloc(ind_msg->len, sizeof(gtp_ngu_t_stats_t) );
    assert(ind_msg->ngut != NULL);
  }
    
  for(uint32_t i = 0; i < ind_msg->len; ++i){
    gtp_ngu_t_stats_t* ngut = &ind_msg->ngut[i];
      
    // Fill dummy data in your data structure  
    ngut->rnti=abs(rand()%mod) ;         
    ngut->qfi=abs(rand()%mod);
    ngut->teidgnb=abs(rand()%mod);
    ngut->teidupf=abs(rand()%mod);
  }
}
