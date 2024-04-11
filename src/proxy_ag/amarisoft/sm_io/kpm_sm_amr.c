#include "kpm_sm_amr.h"
#include <assert.h>
#include <stdio.h>

#include "../../../util/alg_ds/alg/defer.h"
#include "../../../util/time_now_us.h"
#include "../../../util/e.h"

#include "../../../agent/e2_agent_api.h"
#include "../../../sm/kpm_sm/kpm_data_ie_wrapper.h"
#include "../../../util/alg_ds/alg/murmur_hash_32.c"
#include "../../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../../../sm/agent_if/read/sm_ag_if_rd.h"
#include "../ie/msg_stats.h"
#include "../ie/msg_ue_get.h"

#include "../e2_agent_amr_api.h"

static
meas_record_lst_t fill_DRB_PdcpSduVolumeDL(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  meas_record_lst_t dst = {.value = INTEGER_MEAS_VALUE}; 
  dst.int_val = 1021;
  return dst;
}

static
meas_record_lst_t fill_DRB_PdcpSduVolumeUL(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  meas_record_lst_t dst = {.value = INTEGER_MEAS_VALUE}; 
  dst.int_val = 1021;
  return dst;

}

static
meas_record_lst_t fill_DRB_RlcSduDelayDl(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  meas_record_lst_t dst = {.value = INTEGER_MEAS_VALUE}; 
  dst.int_val = 1021;
  return dst;
}

static
assoc_ht_open_t ht;

typedef meas_record_lst_t (*kpm_fp)(ue_id_e2sm_t const* ue);

typedef struct{
  const char* key;
  kpm_fp value;
} kv_measure_t;

static
const kv_measure_t lst_measure[] = {
  (kv_measure_t){.key = "DRB.PdcpSduVolumeDL", .value = fill_DRB_PdcpSduVolumeDL },
  (kv_measure_t){.key = "DRB.PdcpSduVolumeUL", .value = fill_DRB_PdcpSduVolumeUL },
  (kv_measure_t){.key = "DRB.RlcSduDelayDl", .value =  fill_DRB_RlcSduDelayDl },
//  (kv_measure_t){.key = "DRB.UEThpDl", .value =  fill_DRB_UEThpDl },
//  (kv_measure_t){.key = "DRB.UEThpUl", .value =  fill_DRB_UEThpUl },
//  (kv_measure_t){.key = "RRU.PrbTotDl", .value =  fill_RRU_PrbTotDl },
//  (kv_measure_t){.key = "RRU.PrbTotUl", .value =  fill_RRU_PrbTotUl },
//  (kv_measure_t){.key = "CARR.WBCQIDist.BinX.BinY.BinZ", .value = fill_WBCQIDist_BinXYZ },
//  (kv_measure_t){.key = "CARR.PDSCHMCSDist.BinX.BinY.BinZ", .value = fill_PDSCHMCSDist_BinXYZ },
//  (kv_measure_t){.key = "CARR.PUSCHMCSDist.BinX.BinY.BinZ", .value = fill_PUSCHMCSDist_BinXYZ },
//  (kv_measure_t){.key = "CARR.MeanTxPwr", .value = fill_MeanTxPwr },
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
bool cmp_str(const void* a, const void* b)
{
  char* a_str = *(char**)(a);
  char* b_str = *(char**)(b);

  int const ret = strcmp(a_str, b_str);
  return ret == 0;
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
  size_t const num_ues = 16;

  seq_arr_t dst = {0};
  seq_init(&dst, sizeof(ue_id_e2sm_t)); //

  for(size_t i = 0; i < num_ues; ++i){
    bool const select_ue = ue_fullfills_predicate(*info->test_cond, *info->test_cond_value->int_value );
    if(select_ue){
      ue_id_e2sm_t ue = fill_rnd_ue_id_data();
      seq_push_back(&dst, &ue, sizeof(ue_id_e2sm_t));
    }
  }
  return dst;
}




static
seq_arr_t match_gbr_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == GBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_ambr_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == AMBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_isstat_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == IsStat_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_iscatm_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == IsCatM_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrp_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == DL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrq_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == DL_RSRQ_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}


static
seq_arr_t match_ul_rsrp_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == UL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_cqi_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == CQI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_fiveqi_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == fiveQI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_qci_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == QCI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_s_nssai_test_cond_type(test_info_lst_t const* info, msg_ue_get_t const* ues)
{
  assert(info != NULL);
  assert(ues != NULL);
  assert(info->test_cond_type == S_NSSAI_TEST_COND_TYPE);
  assert(info->test_cond_value->type == INTEGER_TEST_COND_VALUE);

  return emulate_ues_fullfilling_pred(info);
}

typedef seq_arr_t (*fp_arr_cond)(test_info_lst_t const* info,msg_ue_get_t const*  );

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
seq_arr_t matching_ues_amr(matching_condition_format_4_lst_t const* cond, size_t len, msg_ue_get_t const* ues)
{
  assert(cond != NULL && "Condition equal NULL");
  assert(len == 1 && "Only one condition supported");

  seq_arr_t dst = match_cond_arr[cond->test_info_lst.test_cond_type](&cond->test_info_lst, ues);

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

bool read_kpm_sm_amr(void* data)
{
  assert(data != NULL);
  kpm_rd_ind_data_t* kpm = (kpm_rd_ind_data_t*)data;
  assert(kpm->act_def!= NULL && "Cannot be NULL");

  msg_stats_amr_t msg_stats = {0};
  defer({ free_msg_stats_amr(&msg_stats); });
  msg_ue_get_t msg_ues = {0};
  defer({ free_msg_ue_get(&msg_ues); });

  int64_t t0 = time_now_us();
  fill_msg_kpm_sm_api(&msg_stats, &msg_ues);
  int64_t t1 = time_now_us();

  printf("Elapsed time %ld\n", t1 - t0);
  fflush(stdout);

  if(kpm->act_def->type == FORMAT_4_ACTION_DEFINITION){
    kpm_act_def_format_4_t const* frm_4 = &kpm->act_def->frm_4 ;  // 8.2.1.2.4
    assert(0 != 0 && "Debug point"); 
  }
    // Matching UEs
    seq_arr_t match_ues = matching_ues_amr(frm_4->matching_cond_lst, frm_4->matching_cond_lst_len, msg_ues);
    // If no UEs match the condition, do not send data to the nearRT-RIC
    if(seq_size(&match_ues) == 0){
      seq_arr_free(&match_ues, free_ue_id_e2sm_wrapper);
      return false;
    }

    // Subscription Information
    kpm_ind_msg_format_3_t info = subscription_info(&match_ues, &frm_4->action_def_format_1);

    // Header
    kpm->ind.hdr.type = FORMAT_1_INDICATION_HEADER;
    kpm->ind.hdr.kpm_ric_ind_hdr_format_1 = fill_rnd_kpm_ind_hdr_frm_1();
    // Message
    // 7.8 Supported RIC Styles and E2SM IE Formats
    // Format 4 corresponds to indication message 3
    kpm->ind.msg.type = FORMAT_3_INDICATION_MESSAGE;
    kpm->ind.msg.frm_3 = info;

    seq_arr_free(&match_ues, free_ue_id_e2sm_wrapper);
  } else {
     kpm->ind.hdr = fill_rnd_kpm_ind_hdr();
     kpm->ind.msg = fill_rnd_kpm_ind_msg();
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




