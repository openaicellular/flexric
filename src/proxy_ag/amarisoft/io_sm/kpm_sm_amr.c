#include "kpm_sm_amr.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../../../util/alg_ds/alg/defer.h"
#include "../../../util/time_now_us.h"
#include "../../../util/e.h"

#include "../../../agent/e2_agent_api.h"
#include "../../../sm/kpm_sm/kpm_data_ie_wrapper.h"
#include "../../../util/alg_ds/alg/murmur_hash_32.c"
#include "../../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../../../util/compare.h"
#include "../../../sm/agent_if/read/sm_ag_if_rd.h"

#include "../lib/ie/msg_stats.h"
#include "../lib/ie/msg_ue_get.h"
#include "../e2_agent_amr_api.h"


typedef struct{
  // Index from msg_ue_get_t  
  size_t idx_ue;
  size_t idx_qos_flows;
  ue_id_e2sm_t ue_id;
} idx_ue_id_e2sm_t;

static
void free_idx_ue_id_e2sm_wrapper(void* it)
{
  assert(it != NULL);
  free_ue_id_e2sm(&((idx_ue_id_e2sm_t*)it)->ue_id);
}

static
meas_record_lst_t fill_DRB_PdcpSduVolumeDL(idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs) 
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  meas_record_lst_t dst = {.value = REAL_MEAS_VALUE}; 

  msg_ue_get_t const* ue_amr = &kpm_msgs->ues;
  // Convert data from bytes to Mbit followed TS 28.552
  size_t const i = ue->idx_ue;
  size_t const j = ue->idx_qos_flows;
  dst.real_val = ue_amr->ue_lst[i].qos_flows.qos_flows[j].dl_total_bytes * 8.0 / 1000.0; 
  printf("dst.real_val %lf \n", dst.real_val );
  return dst;
}

static
meas_record_lst_t fill_DRB_PdcpSduVolumeUL(idx_ue_id_e2sm_t const* ue,  kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  
  meas_record_lst_t dst = {.value = REAL_MEAS_VALUE}; 

  msg_ue_get_t const* ue_amr = &kpm_msgs->ues;
  // Convert data from bytes to Mbit followed TS 28.552
  size_t const i = ue->idx_ue;
  size_t const j = ue->idx_qos_flows;
  dst.real_val = ue_amr->ue_lst[i].qos_flows.qos_flows[j].ul_total_bytes * 8 / 1000.0; 

  return dst;
}

static
meas_record_lst_t fill_DRB_RlcSduDelayDl(idx_ue_id_e2sm_t const* ue,  kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);

  printf("[E2-AGENT] No implementation fill_DRB_RlcSduDelayDl - Fill random value\n");
  meas_record_lst_t dst = {.value = INTEGER_MEAS_VALUE}; 
  dst.int_val = rand()% 1024;
  return dst;
}

static
meas_record_lst_t fill_DRB_UEThpDl( idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  msg_ue_get_t const* ue_amr = &kpm_msgs->ues;

  meas_record_lst_t dst = {0};
  dst.value = REAL_MEAS_VALUE;
  // Mbps
  ue_lst_amr_t const* tmp = &ue_amr->ue_lst[ue->idx_ue];
  assert(tmp->arr_cells.sz > 0 && "Can we have 0??");
  dst.real_val = tmp->arr_cells.cell[0].dl_bitrate / 1000.0;
  return dst;
}

static
meas_record_lst_t fill_DRB_UEThpUl(idx_ue_id_e2sm_t const* ue,  kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  msg_ue_get_t const* ue_amr = &kpm_msgs->ues;

  meas_record_lst_t dst = {0};
  dst.value = REAL_MEAS_VALUE;
  // Mbps
  ue_lst_amr_t const* tmp = &ue_amr->ue_lst[ue->idx_ue];
  assert(tmp->arr_cells.sz > 0 && "Can we have 0??");
  dst.real_val = tmp->arr_cells.cell[0].ul_bitrate / 1000.0;
  return dst;
}

static
meas_record_lst_t fill_RRU_PrbTotDl(idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
 
  // Check ran_stats, this measurement is used for cell perspective instead UE perspective
  meas_record_lst_t dst = {0};
  dst.value = REAL_MEAS_VALUE;

  msg_stats_amr_t const* s = &kpm_msgs->stats;
  // This is most probably a bug. 
  // RRU assumed to be the equal to the RAN 
  // and been one
  // Why average ???
  // Copied from previous implementation
  float dl_use_avg =  0.0;
  for(size_t i = 0; i < s->cells.sz; ++i){
    dl_use_avg += s->cells.cells[i].dl_use_avg;
  } 

  dst.real_val = dl_use_avg*100;
  
  return dst;
}

static
meas_record_lst_t fill_RRU_PrbTotUl(idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs) 
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
 
  // Check ran_stats, this measurement is used for cell perspective instead UE perspective
  meas_record_lst_t dst = {0};
  dst.value = REAL_MEAS_VALUE;

  msg_stats_amr_t const* s = &kpm_msgs->stats;
  // This is most probably a bug. 
  // RRU assumed to be the equal to the RAN 
  // and been one
  // Why average ???
  // Copied from previous implementation
  float ul_use_avg = 0.0;
  for(size_t i = 0; i < s->cells.sz; ++i){
    ul_use_avg += s->cells.cells[i].ul_use_avg;
  } 

  dst.real_val = ul_use_avg*100;
  
  return dst;
}

static
meas_record_lst_t fill_WBCQIDist_BinXYZ( idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs) 
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  msg_ue_get_t const* ue_amr = &kpm_msgs->ues;

  // where X represents the index of the CQI value (0 to 15). Y represents the index of rank value (1 to 8), Z represents the index of table value (1 to 4).
  meas_record_lst_t dst = {0};
  dst.value = INTEGER_MEAS_VALUE;
  dst.int_val = ue_amr->ue_lst[ue->idx_ue].arr_cells.cell[0].cqi;
  return dst;
}

static
meas_record_lst_t fill_PDSCHMCSDist_BinXYZ(idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  msg_ue_get_t const* ue_amr = &kpm_msgs->ues;

  // Where X represents the index of rank value (1 to 8), Y represents the index of table value (1 to 4), and Z represents the index of the MCS value (0 to 31).
  meas_record_lst_t dst = {0};
  dst.value = INTEGER_MEAS_VALUE;
  dst.int_val = ue_amr->ue_lst[ue->idx_ue].arr_cells.cell[0].dl_mcs;
  return dst;
}

static
meas_record_lst_t fill_PUSCHMCSDist_BinXYZ(idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  msg_ue_get_t const* ue_amr = &kpm_msgs->ues;

  meas_record_lst_t dst = {0};
  dst.value = INTEGER_MEAS_VALUE;
  dst.int_val = ue_amr->ue_lst[ue->idx_ue].arr_cells.cell[0].dl_mcs;
  return dst;
}

static
double sum_total_epre(msg_ue_get_t const* ue_get)
{
  double epre = 0;
  for(size_t i = 0; i < ue_get->sz; ++i){
    ue_lst_amr_t const* lst = &ue_get->ue_lst[i]; 
    for(size_t j = 0; j < lst->arr_cells.sz; ++j){
      epre += lst->arr_cells.cell[j].epre;
    }
  }
  return epre;
}

static
meas_record_lst_t fill_MeanTxPwr(idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs)
{
  assert(ue != NULL);
  assert(kpm_msgs != NULL);
  msg_stats_amr_t const* stats = &kpm_msgs->stats;
  msg_config_get_amr_t const* cfg = &kpm_msgs->cfg;
  msg_ue_get_t const* ue_get = &kpm_msgs->ues;

  // TS28.552 5.1.1.29.2
  // This measurement is obtained by retaining the mean value of the total carrier power transmitted in the cell within the measurement granularity period.
  // The power includes all radio power transmitted, included common channels, traffic channels, control channels. The value is expressed in dBm.
  double tx_power = 0.0;
  for(size_t i = 0; i < stats->cells.sz; ++i){
    int const cell_id = atoi(stats->cells.names[i]);
    if(cell_id == cfg->arr_nr_cells.nr_cells[i].n_id_nrcell){
      double used_rbs = stats->cells.cells[i].dl_use_avg * cfg->arr_nr_cells.nr_cells[i].n_rb_dl;
      double epre = sum_total_epre(ue_get);
      // Formula copied from previous design
      // probably from here:
      // https://5g-tools.com/epre-energy-per-resource-element-calculator-5g-4g/
      // Not sure about its correctness
      tx_power = epre + 10 * log10(12 * used_rbs);
      break;
    }
  }

  return (meas_record_lst_t){.value = REAL_MEAS_VALUE, .real_val = tx_power};
}










static
assoc_ht_open_t ht;

typedef meas_record_lst_t (*kpm_fp)( idx_ue_id_e2sm_t const* ue, kpm_msgs_amr_t const* kpm_msgs);

typedef struct{
  const char* key;
  kpm_fp value;
} kv_measure_t;

static
const kv_measure_t lst_measure[] = {
  (kv_measure_t){.key = "DRB.PdcpSduVolumeDL", .value = fill_DRB_PdcpSduVolumeDL },
  (kv_measure_t){.key = "DRB.PdcpSduVolumeUL", .value = fill_DRB_PdcpSduVolumeUL },
  (kv_measure_t){.key = "DRB.RlcSduDelayDl", .value =  fill_DRB_RlcSduDelayDl },
  (kv_measure_t){.key = "DRB.UEThpDl", .value =  fill_DRB_UEThpDl },
  (kv_measure_t){.key = "DRB.UEThpUl", .value =  fill_DRB_UEThpUl },
  (kv_measure_t){.key = "RRU.PrbTotDl", .value =  fill_RRU_PrbTotDl },
  (kv_measure_t){.key = "RRU.PrbTotUl", .value =  fill_RRU_PrbTotUl },
  (kv_measure_t){.key = "CARR.WBCQIDist.BinX.BinY.BinZ", .value = fill_WBCQIDist_BinXYZ },
  (kv_measure_t){.key = "CARR.PDSCHMCSDist.BinX.BinY.BinZ", .value = fill_PDSCHMCSDist_BinXYZ },
  (kv_measure_t){.key = "CARR.PUSCHMCSDist.BinX.BinY.BinZ", .value = fill_PUSCHMCSDist_BinXYZ },
  (kv_measure_t){.key = "CARR.MeanTxPwr", .value = fill_MeanTxPwr },

  };
  // 3GPP TS 28.552

static
uint32_t hash_func(const void* key_v)
{
  char* key = *(char**)(key_v);
  static const uint32_t seed = 42;
  return murmur3_32((uint8_t*)key, strlen(key), seed);
}

static
void free_str(void* key, void* value)
{
  free(*(char**)key);
  free(value);
}

static
void free_lst_measurements(void)
{
  assoc_free(&ht);
}

static
void init_lst_measurements(void)
{
  assoc_ht_open_init(&ht, sizeof(char*), cmp_str, free_str, hash_func);

  const size_t nelem = sizeof(lst_measure) / sizeof(lst_measure[0]);
  for(size_t i = 0; i < nelem; ++i){
    const size_t sz = strlen(lst_measure[i].key);
    char* key = calloc(sz + 1, sizeof(char));
    memcpy(key, lst_measure[i].key, sz);

    kpm_fp* value = calloc(1, sizeof(kpm_fp));
    assert(value != NULL && "Memory exhausted");
    *value = lst_measure[i].value;
    assoc_insert(&ht, &key, sizeof(char*), value);
  }
  assert(assoc_size(&ht) == nelem);
}



static
seq_arr_t emulate_ues_fullfilling_pred(test_info_lst_t const*  info)
{
  assert(info != NULL);
  //size_t const num_ues = 16;

  seq_arr_t dst = {0};
  seq_init(&dst, sizeof(ue_id_e2sm_t)); //

  //for(size_t i = 0; i < num_ues; ++i){
  //  bool const select_ue = ue_fullfills_predicate(*info->test_cond, *info->test_cond_value->int_value );
  //  if(select_ue){
  //    ue_id_e2sm_t ue = fill_rnd_ue_id_data();
  //    seq_push_back(&dst, &ue, sizeof(ue_id_e2sm_t));
  //  }
  //}
  return dst;
}




static
seq_arr_t match_gbr_test_cond_type(test_info_lst_t const* info,  kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == GBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_ambr_test_cond_type(test_info_lst_t const* info,  kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == AMBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_isstat_test_cond_type(test_info_lst_t const* info,  kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == IsStat_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_iscatm_test_cond_type(test_info_lst_t const* info, kpm_msgs_amr_t const* msgs) 
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == IsCatM_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrp_test_cond_type(test_info_lst_t const* info,  kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == DL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrq_test_cond_type(test_info_lst_t const* info, kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == DL_RSRQ_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}


static
seq_arr_t match_ul_rsrp_test_cond_type(test_info_lst_t const* info,  kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == UL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_cqi_test_cond_type(test_info_lst_t const* info,  kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == CQI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_fiveqi_test_cond_type(test_info_lst_t const* info,kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == fiveQI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_qci_test_cond_type(test_info_lst_t const* info, kpm_msgs_amr_t const* msgs) 
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == QCI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
uint32_t create_nssai(uint32_t sd, uint32_t sst)
{
  uint32_t nssai = 0;
  memcpy(&((uint8_t*)&nssai)[1] ,&sd, 3); 
  memcpy(((uint8_t*)&nssai),&sst, 1); 
  return nssai;
}

static
guami_t fill_guami(ue_lst_amr_t const* src)
{
  assert(src != NULL);
  guami_t dst = {0};

  // Mandatory
  // PLMN Identity 6.2.3.1
  dst.plmn_id = plmn_agent_amr_api();

  // Mandatory
  // AMF Region ID BIT STRING (SIZE(8))
  // openair3/NGAP/ngap_gNB_defs.h
  // uint8_t amf_region_id;

  // Mandatory
  //  AMF Set ID BIT STRING (SIZE(10))
  //uint16_t amf_set_id:10;

  // Mandatory
  // AMF Pointer BIT STRING (SIZE(6))
  // uint16_t amf_ptr:6;

  return dst;
}

static
gnb_e2sm_t fill_gnb(ue_lst_amr_t const* src)
{
  assert(src != NULL);
  gnb_e2sm_t dst = {0}; 

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  // Defined in TS 38.413 [6] (NGAP) clause 9.3.3.1.
  // Defined in TS 38.401 [2].
  // fill with openair3/NGAP/ngap_gNB_ue_context.h:61
  dst.amf_ue_ngap_id = src->amf_ue_id ; // [0, 2^40 ]

  // Mandatory
  // GUAMI 6.2.3.17 
  dst.guami = fill_guami(src);

  // gNB-CU UE F1AP ID List
  // C-ifCUDUseparated 
  //size_t gnb_cu_ue_f1ap_lst_len;  // [1,4]
  //uint32_t *gnb_cu_ue_f1ap_lst;

  //gNB-CU-CP UE E1AP ID List
  //C-ifCPUPseparated 
  //size_t gnb_cu_cp_ue_e1ap_lst_len;  // [1, 65535]
  //uint32_t *gnb_cu_cp_ue_e1ap_lst;

  // RAN UE ID
  // Optional
  // 6.2.3.25
  // OCTET STRING (SIZE (8))
  // Defined in TS 38.473 (F1AP) 
  // clause 9.2.2.1
  // UE CONTEXT SETUP REQUEST
  dst.ran_ue_id = calloc(1, sizeof(uint64_t)); 
  assert(dst.ran_ue_id != NULL && "Memory exhausted");
  *dst.ran_ue_id = src->ran_ue_id; 

  // M-NG-RAN node UE XnAP ID
  // C- ifDCSetup
  // 6.2.3.19
  // uint32_t *ng_ran_node_ue_xnap_id;

  // Global gNB ID
  // 6.2.3.3
  // Optional
  // This IE shall not be used. Global NG-RAN Node ID IE shall replace this IE 
  // global_gnb_id_t *global_gnb_id;

  // Global NG-RAN Node ID
  // C-ifDCSetup
  // 6.2.3.2
  // global_ng_ran_node_id_t *global_ng_ran_node_id;

  return dst;
}

static
bool lt(int64_t const* m0, int64_t const* m1)  
{
  return *m0 < *m1;
}

static
bool gt(int64_t const* m0, int64_t const* m1)  
{
  return *m0 > *m1;
}

static
bool eq(int64_t const* m0, int64_t const* m1)  
{
  return *m0 == *m1;
}

typedef bool (*fp_test_cond)(int64_t const* m0, int64_t const* m1);

fp_test_cond arr_fp_test_cond[3] = {
    eq,
    gt,
    lt,
};

static
seq_arr_t match_s_nssai_test_cond_type(test_info_lst_t const* info, kpm_msgs_amr_t const* msgs)
{
  assert(info != NULL);
  assert(msgs != NULL);
  assert(info->test_cond_type == S_NSSAI_TEST_COND_TYPE);
  assert(info->test_cond_value->type == INTEGER_TEST_COND_VALUE);

  msg_ue_get_t const* ues = &msgs->ues;

  assert(info->test_cond != NULL && *info->test_cond < CONTAINS_TEST_COND && "Only less, greater, equal");

  seq_arr_t dst = {0};
  seq_init(&dst, sizeof(idx_ue_id_e2sm_t)); //

  for(size_t i = 0; i < ues->sz; ++i){
    ue_lst_amr_t const* ue = &ues->ue_lst[i]; 
    for(size_t j = 0; j < ue->qos_flows.sz; ++j){
      qos_flows_ue_lst_amr_t const* flows = &ue->qos_flows.qos_flows[j]; 
      int64_t const nssai = create_nssai(flows->sd, flows->sst); 
      fp_test_cond f = arr_fp_test_cond[*info->test_cond];
      if(f(&nssai, info->test_cond_value->int_value)){
        // Only GNB supported
        idx_ue_id_e2sm_t idx_ue_id = {.idx_ue = i, .idx_qos_flows = j};
        idx_ue_id.ue_id.type = GNB_UE_ID_E2SM;
        idx_ue_id.ue_id.gnb = fill_gnb(ue);

        seq_push_back(&dst, &idx_ue_id, sizeof(idx_ue_id_e2sm_t));
      } else {
        printf("[PROXY]: nsaai not matching %ld \n", nssai);
      }
    }
  }

  return dst;
}

typedef seq_arr_t (*fp_arr_cond)(test_info_lst_t const* info, kpm_msgs_amr_t const*  );

static
fp_arr_cond match_cond_arr[END_TEST_COND_TYPE_KPM_V2_01] = {
    match_gbr_test_cond_type,
    match_ambr_test_cond_type,
    match_isstat_test_cond_type,
    match_iscatm_test_cond_type,
    match_dl_rsrp_test_cond_type,
    match_dl_rsrq_test_cond_type,
    match_ul_rsrp_test_cond_type,
    match_cqi_test_cond_type,
    match_fiveqi_test_cond_type,
    match_qci_test_cond_type,
    match_s_nssai_test_cond_type,
};

static
seq_arr_t matching_ues_amr(matching_condition_format_4_lst_t const* cond, size_t len, kpm_msgs_amr_t const* msgs)
{
  assert(cond != NULL && "Condition equal NULL");
  assert(len == 1 && "Only one condition supported");
  printf("Condition type %d \n",cond->test_info_lst.test_cond_type);
  seq_arr_t dst = match_cond_arr[cond->test_info_lst.test_cond_type](&cond->test_info_lst, msgs);

  return dst;
}



// KPM SM
void init_kpm_sm_amr(void)
{
  init_lst_measurements();
}

void free_kpm_sm_amr(void)
{
  free_lst_measurements();
}

static
kpm_ind_msg_format_1_t collect_measurements(idx_ue_id_e2sm_t const* ue, meas_info_format_1_lst_t const* lst, size_t len, kpm_msgs_amr_t const* kpm)
{
  assert(ue != NULL);
  assert(lst != NULL);
  assert(kpm != NULL);
  assert(len > 0 && len < 65536);

  kpm_ind_msg_format_1_t dst = {0};
  // Value depending on the (period subscription) / (granularity period)
  // currently only 1 supported
  dst.meas_data_lst_len = 1;
  dst.meas_data_lst = calloc(dst.meas_data_lst_len, sizeof(meas_data_lst_t));
  assert(dst.meas_data_lst != NULL && "Memory exhausted");

  dst.meas_data_lst[0].meas_record_len = len;
  dst.meas_data_lst[0].meas_record_lst = calloc(len, sizeof(meas_record_lst_t));
  assert(dst.meas_data_lst[0].meas_record_lst != NULL && "Memory exhausted");

  dst.meas_info_lst_len = len;
  dst.meas_info_lst = calloc(len, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < len; ++i) {
    assert(lst[i].meas_type.type == NAME_MEAS_TYPE && "Only NAME supported");
    dst.meas_info_lst[i] = cp_meas_info_format_1_lst(&lst[i]);

    const void* key = lst[i].meas_type.name.buf;
    // Get the value pointer from the key i.e., the function to be called
    // for the key that represents a measurement e.g., fill_DRB_PdcpSduVolumeDL
    void* value = assoc_ht_open_value(&ht, &key);
    assert(value != NULL && "Not registered name used as key");
    // Get the measurement (e.g., DRB_PdcpSduVolumeDL) for the UE
    dst.meas_data_lst[0].meas_record_lst[i] = (*(kpm_fp*)value)(ue, kpm);
  }
  return dst;
}


static
kpm_ind_msg_format_3_t subscription_info(seq_arr_t const* ues, kpm_act_def_format_1_t const* act_def, kpm_msgs_amr_t const* kpm)
{
  assert(act_def != NULL);
  assert(kpm != NULL);

  kpm_ind_msg_format_3_t dst = {0};

  const size_t sz = seq_size((seq_arr_t*)ues);
  dst.ue_meas_report_lst_len = sz;
  if(sz > 0){
    dst.meas_report_per_ue = calloc(sz, sizeof(meas_report_per_ue_t));
    assert(dst.meas_report_per_ue != NULL && "Memory exhausted");
  }

  void* it = seq_front((seq_arr_t*)ues);
  for(size_t i = 0; i < sz; ++i){
    idx_ue_id_e2sm_t const* ue = (idx_ue_id_e2sm_t const*)it;

    dst.meas_report_per_ue[i].ue_meas_report_lst = cp_ue_id_e2sm(&ue->ue_id);
    dst.meas_report_per_ue[i].ind_msg_format_1 = collect_measurements(ue, act_def->meas_info_lst, act_def->meas_info_lst_len, kpm);

    // We ignore the remaining fields from act_def by the moment
    // for simplicity

    it = seq_next((seq_arr_t*)ues, it);
  }

  return dst;
}

static 
kpm_ric_ind_hdr_format_1_t fill_rnd_kpm_ind_hdr_frm_1(void)
{
  kpm_ric_ind_hdr_format_1_t hdr_frm_1 = {0};

int64_t t = time_now_us();
#if defined(KPM_V2_01) || defined (KPM_V2_03)
  hdr_frm_1.collectStartTime = t / 1000000; // needs to be truncated to 32 bits to arrive to a resolution of seconds
#elif defined(KPM_V3_00)
  hdr_frm_1.collectStartTime = t;
#else
  static_assert(0!=0, "Unknown KPM version");
#endif

  hdr_frm_1.fileformat_version = NULL;
  
  hdr_frm_1.sender_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_name->buf = calloc(strlen("My Amarisoft MONO") + 1, sizeof(uint8_t));
  memcpy(hdr_frm_1.sender_name->buf, "My Amarisoft-MONO", strlen("My Amarisoft-MONO"));
  hdr_frm_1.sender_name->len = strlen("My  Amarisoft-MONO");
  
  hdr_frm_1.sender_type = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_type->buf = calloc(strlen("MONO") + 1, sizeof(uint8_t));
  memcpy(hdr_frm_1.sender_type->buf, "MONO", strlen("MONO"));
  hdr_frm_1.sender_type->len = strlen("MONO");
  
  hdr_frm_1.vendor_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.vendor_name->buf = calloc(strlen("Amarisoft") + 1, sizeof(uint8_t));
  memcpy(hdr_frm_1.vendor_name->buf, "Amarisoft", strlen("Amarisoft"));
  hdr_frm_1.vendor_name->len = strlen("Amarisoft");

  return hdr_frm_1;
}


bool read_kpm_sm_amr(void* data)
{
  assert(data != NULL);
  kpm_rd_ind_data_t* kpm = (kpm_rd_ind_data_t*)data;
  assert(kpm->act_def!= NULL && "Cannot be NULL");

  kpm_msgs_amr_t kpm_msg = {0};
  defer({ free_kpm_msgs_amr(&kpm_msg); });

  fill_msg_kpm_sm_api(&kpm_msg);

  if(kpm->act_def->type == FORMAT_4_ACTION_DEFINITION){
    kpm_act_def_format_4_t const* frm_4 = &kpm->act_def->frm_4 ;  // 8.2.1.2.4
    
    // Matching UEs
    seq_arr_t match_ues = matching_ues_amr(frm_4->matching_cond_lst, frm_4->matching_cond_lst_len, &kpm_msg);
    defer({ seq_arr_free(&match_ues, free_idx_ue_id_e2sm_wrapper); });

    // If no UEs match the condition, do not send data to the nearRT-RIC
    if(seq_size(&match_ues) == 0){
      return false;
    }

    // Subscription Information
    kpm_ind_msg_format_3_t info = subscription_info(&match_ues, &frm_4->action_def_format_1, &kpm_msg);

    // Header
    kpm->ind.hdr.type = FORMAT_1_INDICATION_HEADER;
    kpm->ind.hdr.kpm_ric_ind_hdr_format_1 = fill_rnd_kpm_ind_hdr_frm_1();
    // Message
    // 7.8 Supported RIC Styles and E2SM IE Formats
    // Format 4 corresponds to indication message 3
    kpm->ind.msg.type = FORMAT_3_INDICATION_MESSAGE;
    kpm->ind.msg.frm_3 = info;

  } else {
     assert(0 != 0 && "Unknown action definition "); 
     //kpm->ind.hdr = fill_rnd_kpm_ind_hdr();
     //kpm->ind.msg = fill_rnd_kpm_ind_msg();
  }
  
  return true;
}

static
ric_report_style_item_t fill_ric_report_style_item(void)
{
  ric_report_style_item_t dst = {0};

  // 8.3.3
  dst.report_style_type = STYLE_4_RIC_SERVICE_REPORT;

  // 8.3.4
  const char style_name[] = "BubbleRAN style name";
  dst.report_style_name = cp_str_to_ba(style_name);

  // 8.3.5
  dst.act_def_format_type = FORMAT_4_ACTION_DEFINITION;

  const size_t sz = sizeof(lst_measure) / sizeof(lst_measure[0]);
  
  // [1, 65535]
  dst.meas_info_for_action_lst_len = sz;
  dst.meas_info_for_action_lst = ecalloc(sz, sizeof(meas_info_for_action_lst_t));

  for(size_t i = 0; i < sz; ++i){
    dst.meas_info_for_action_lst[i].name = cp_str_to_ba(lst_measure[i].key);
  }

  // 8.3.5
  dst.ind_hdr_format_type = FORMAT_1_INDICATION_HEADER;
  dst.ind_msg_format_type = FORMAT_3_INDICATION_MESSAGE;

  return dst;
}

static
kpm_ran_function_def_t fill_kpm_ran_func_def(void)
{
  kpm_ran_function_def_t dst = {0};

  // RAN Function name is already filled by the kpm_sm_agent.c
  dst.sz_ric_event_trigger_style_list = 0;
  dst.ric_event_trigger_style_list = 0;

  dst.sz_ric_report_style_list = 1;
  dst.ric_report_style_list = ecalloc(dst.sz_ric_report_style_list, sizeof(ric_report_style_item_t ));

  dst.ric_report_style_list[0] = fill_ric_report_style_item();

  return dst;
}

void read_kpm_setup_sm_amr(void* e2ap)
{
  assert(e2ap != NULL);

  kpm_e2_setup_t* kpm = (kpm_e2_setup_t*)(e2ap);
  // Let's fill the RAN Function Definition with currently supported measurements
  kpm->ran_func_def = fill_kpm_ran_func_def();
}

