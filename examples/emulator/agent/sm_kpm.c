#include "sm_kpm.h"
#include "../../../test/rnd/fill_rnd_data_kpm.h"
#include "../../../src/util/time_now_us.h"
#include "../../../src/util/alg_ds/alg/murmur_hash_32.h"
#include "../../../src/util/alg_ds/ds/assoc_container/assoc_generic.h"

#include <assert.h>
#include <stdio.h>
#include <pthread.h>

static
meas_record_lst_t fill_rnd_int_data(void)
{
  meas_record_lst_t dst = {0};

  dst.value = INTEGER_MEAS_VALUE;
  dst.int_val = rand()%1024;

  //printf("measurement record value int %d \n", dst.meas_data_lst[i].meas_record_lst[0].int_val);                                                            //

  return dst;
}

static
meas_record_lst_t fill_rnd_float_data(void)
{
  meas_record_lst_t dst = {0};

  // Only 1 supported. It could change according to granularity period
  dst.value = REAL_MEAS_VALUE;
  dst.real_val = 5.0 + rand()%1024 /(double)((rand()%2048) + 1);
  // printf("measurement record value %f \n", dst.meas_data_lst[i].meas_record_lst[0].real_val);

  return dst;
}

static
 meas_record_lst_t fill_DRB_PdcpSduVolumeDL(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  //printf(" fill_DRB_PdcpSduVolumeDL\n");
  return fill_rnd_int_data();
}

static
 meas_record_lst_t fill_DRB_PdcpSduVolumeUL(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  //printf(" fill_DRB_PdcpSduVolumeUL\n");
  return fill_rnd_int_data();
}

static
 meas_record_lst_t fill_DRB_RlcSduDelayDl(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  //printf("fill_DRB_RlcSduDelayDl \n");
  return fill_rnd_float_data();
}

static
 meas_record_lst_t fill_DRB_UEThpDl( ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  //printf(" fill_DRB_UEThpDl\n");
  return fill_rnd_float_data();
}

static
 meas_record_lst_t fill_DRB_UEThpUl(ue_id_e2sm_t const* ue )
{
  assert(ue != NULL);
  //printf(" fill_DRB_UEThpUl\n");
  return fill_rnd_float_data();
}

static
 meas_record_lst_t fill_RRU_PrbTotDl(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  //printf(" fill_RRU_PrbTotDl\n");
  return fill_rnd_int_data();
}

static
 meas_record_lst_t fill_RRU_PrbTotUl(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);
  //printf(" fill_RRU_PrbTotUl\n");
  return fill_rnd_int_data();
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
  (kv_measure_t){.key = "DRB.UEThpDl", .value =  fill_DRB_UEThpDl },
  (kv_measure_t){.key = "DRB.UEThpUl", .value =  fill_DRB_UEThpUl },
  (kv_measure_t){.key = "RRU.PrbTotDl", .value =  fill_RRU_PrbTotDl },
  (kv_measure_t){.key = "RRU.PrbTotUl", .value =  fill_RRU_PrbTotUl },
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
gnb_cu_up_e2sm_t fill_rnd_gnb_cu_up_data(void)
{
  gnb_cu_up_e2sm_t gnb_cu_up = {0};

  // 6.2.3.20
  gnb_cu_up.gnb_cu_cp_ue_e1ap = rand();

  // 6.2.3.25, OPTIONAL
  gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(gnb.ran_ue_id != NULL && "Memory exhausted");
  *gnb.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_cu_up;
}

static
gnb_du_e2sm_t fill_rnd_gnb_du_data(void)
{
  gnb_du_e2sm_t gnb_du = {0};

  // 6.2.3.21
  gnb_du.gnb_cu_ue_f1ap = rand();

  // 6.2.3.25, OPTIONAL
  gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(gnb.ran_ue_id != NULL && "Memory exhausted");
  *gnb.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_du;
}

static
gnb_e2sm_t fill_rnd_gnb_data(void)
{
  gnb_e2sm_t gnb = {0};

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  gnb.amf_ue_ngap_id = 112358132134; // % 2^40;

  // Mandatory
  //GUAMI 6.2.3.17 
  gnb.guami.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  
  gnb.guami.amf_region_id = (rand() % 2^8) + 0;
  gnb.guami.amf_set_id = (rand() % 2^10) + 0;
  gnb.guami.amf_ptr = (rand() % 2^6) + 0;

  gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(gnb.ran_ue_id != NULL && "Memory exhausted");
  *gnb.ran_ue_id = (rand() % 2^64) + 0;

  // gNB-CU UE F1AP ID List
  // C-ifCUDUseparated
  if (TEST_AGENT_RAN_TYPE == ngran_gNB_CU){
    gnb.gnb_cu_ue_f1ap_lst_len = 1;
    gnb.gnb_cu_ue_f1ap_lst = calloc(gnb.gnb_cu_ue_f1ap_lst_len, sizeof(uint32_t));
    assert(gnb.gnb_cu_ue_f1ap_lst != NULL && "Memory exhausted");

    gnb.gnb_cu_ue_f1ap_lst[0] = rand();
  }


  return gnb;
}

static 
ue_id_e2sm_t fill_rnd_ue_id_data(void)
{
  ue_id_e2sm_t ue_id_data = {0};

  if (TEST_AGENT_RAN_TYPE == ngran_gNB || TEST_AGENT_RAN_TYPE == ngran_gNB_CU){
    ue_id_data.type = GNB_UE_ID_E2SM;
    ue_id_data.gnb = fill_rnd_gnb_data();
  }
  else if (TEST_AGENT_RAN_TYPE == ngran_gNB_DU)
  {
    ue_id_data.type = GNB_DU_UE_ID_E2SM;
    ue_id_data.gnb_du = fill_rnd_gnb_du_data();
  }
  else if (TEST_AGENT_RAN_TYPE == ngran_gNB_CUUP)
  {
    ue_id_data.type = GNB_CU_UP_UE_ID_E2SM;
    ue_id_data.gnb_cu_up = fill_rnd_gnb_cu_up_data();
  }
  else
  {
    assert(false && "NG-RAN Type not yet implemented");
  }



  return ue_id_data;
}

static 
kpm_ric_ind_hdr_format_1_t fill_rnd_kpm_ind_hdr_frm_1(void)
{
  kpm_ric_ind_hdr_format_1_t hdr_frm_1 = {0};

int64_t t = time_now_us();
#ifdef KPM_V2_01 || KPM_V2_03
  hdr_frm_1.collectStartTime = t / 1000000; // needs to be truncated to 32 bits to arrive to a resolution of seconds
#elifdef KPM_V3_00
  hdr_frm_1.collectStartTime = t;
#else
  static_assert(0!=0, "Unknown KPM version");
#endif

  hdr_frm_1.fileformat_version = NULL;
  
  hdr_frm_1.sender_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_name->buf = calloc(strlen("My OAI-MONO") + 1, sizeof(uint8_t));
  memcpy(hdr_frm_1.sender_name->buf, "My OAI-MONO", strlen("My OAI-MONO"));
  hdr_frm_1.sender_name->len = strlen("My OAI-MONO");
  
  hdr_frm_1.sender_type = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_type->buf = calloc(strlen("MONO") + 1, sizeof(uint8_t));
  memcpy(hdr_frm_1.sender_type->buf, "MONO", strlen("MONO"));
  hdr_frm_1.sender_type->len = strlen("MONO");
  
  hdr_frm_1.vendor_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.vendor_name->buf = calloc(strlen("OAI") + 1, sizeof(uint8_t));
  memcpy(hdr_frm_1.vendor_name->buf, "OAI", strlen("OAI"));
  hdr_frm_1.vendor_name->len = strlen("OAI");

  return hdr_frm_1;
}

static
int dummy_cnt = 0;
// Dummy function. It emulates a function where the UE fullfills (or not)
// the condition
static
bool ue_fullfills_predicate(test_cond_e cond, int64_t value)
{
  assert(cond == EQUAL_TEST_COND
      || cond == GREATERTHAN_TEST_COND
      || cond == CONTAINS_TEST_COND
      || cond == PRESENT_TEST_COND
      );
  assert(value > -1 && "Assuming this for testing");

  dummy_cnt++;
  if(dummy_cnt > 32 && dummy_cnt < 4*32){
    //printf("[KPM-SM]: Emulating no UEs matching condition\n");
    return false;
  }

  return rand()%2;
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
seq_arr_t match_gbr_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == GBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_ambr_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == AMBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_isstat_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == IsStat_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_iscatm_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == IsCatM_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrp_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == DL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrq_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == DL_RSRQ_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}


static
seq_arr_t match_ul_rsrp_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == UL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_cqi_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == CQI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_fiveqi_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == fiveQI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_qci_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == QCI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

  static
seq_arr_t match_s_nssai_test_cond_type(test_info_lst_t const* info)
{
  assert(info != NULL);
  assert(info->test_cond_type == S_NSSAI_TEST_COND_TYPE);
  assert(info->test_cond_value->type == INTEGER_TEST_COND_VALUE);

  return emulate_ues_fullfilling_pred(info);
}

typedef seq_arr_t (*fp_arr_cond)(test_info_lst_t const* info);

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
seq_arr_t matching_ues(matching_condition_format_4_lst_t const* cond, size_t len)
{
  assert(cond != NULL && "Condition equal NULL");
  assert(len == 1 && "Only one condition supported");

  seq_arr_t dst = match_cond_arr[cond->test_info_lst.test_cond_type](&cond->test_info_lst);

  return dst;
}

static
kpm_ind_msg_format_1_t collect_measurements(ue_id_e2sm_t const* ue, meas_info_format_1_lst_t const* lst, size_t len)
{
  assert(ue != NULL);
  assert(lst != NULL);
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
    dst.meas_data_lst[0].meas_record_lst[i] = (*(kpm_fp*)value)(ue);
  }
  return dst;
}

static
kpm_ind_msg_format_3_t subscription_info(seq_arr_t const* ues, kpm_act_def_format_1_t const* act_def)
{
  assert(act_def != NULL);

  kpm_ind_msg_format_3_t dst = {0};

  const size_t sz = seq_size((seq_arr_t*)ues);
  dst.ue_meas_report_lst_len = sz;
  if(sz > 0){
    dst.meas_report_per_ue = calloc(sz, sizeof(meas_report_per_ue_t));
    assert(dst.meas_report_per_ue != NULL && "Memory exhausted");
  }

  void* it = seq_front((seq_arr_t*)ues);
  for(size_t i = 0; i < sz; ++i){
    ue_id_e2sm_t const* ue = (ue_id_e2sm_t const*)it;

    dst.meas_report_per_ue[i].ue_meas_report_lst = cp_ue_id_e2sm(ue);
    dst.meas_report_per_ue[i].ind_msg_format_1 = collect_measurements(ue, act_def->meas_info_lst, act_def->meas_info_lst_len);

    // We ignore the remaining fields from act_def by the moment
    // for simplicity

    it = seq_next((seq_arr_t*)ues, it);
  }

  return dst;
}

static
void free_ue_id_e2sm_wrapper(void* it)
{
  assert(it != NULL);
  free_ue_id_e2sm((ue_id_e2sm_t*)it);
}

void init_kpm_sm(void)
{
  init_lst_measurements();
}

void free_kpm_sm(void)
{
  free_lst_measurements();
}

bool read_kpm_sm(void* data)
{
  assert(data != NULL);

  kpm_rd_ind_data_t* kpm = (kpm_rd_ind_data_t*)data;
  assert(kpm->act_def!= NULL && "Cannot be NULL");

  if(kpm->act_def->type == FORMAT_4_ACTION_DEFINITION){
    kpm_act_def_format_4_t const* frm_4 = &kpm->act_def->frm_4 ;  // 8.2.1.2.4
    // Matching UEs
    seq_arr_t match_ues = matching_ues(frm_4->matching_cond_lst, frm_4->matching_cond_lst_len);
    // If no UEs match the condition, do not send data to the nearRT-RIC
    if(seq_size(&match_ues) == 0){
      seq_arr_free(&match_ues, free_ue_id_e2sm_wrapper);
      return false;
    }

    // Subscription Information
    kpm_ind_msg_format_3_t info = subscription_info(&match_ues, &frm_4->action_def_format_1);

    // Header
    kpm->ind.hdr.type =  FORMAT_1_INDICATION_HEADER;
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

void read_kpm_setup_sm(void* e2ap)
{
  assert(e2ap != NULL);
//  assert(e2ap->type == KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0);

  kpm_e2_setup_t* kpm = (kpm_e2_setup_t*)(e2ap);
  kpm->ran_func_def = fill_rnd_kpm_ran_func_def();
}

sm_ag_if_ans_t write_ctrl_kpm_sm(void const* src)
{
  assert(0 !=0 && "Not supported");
  (void)src;
  sm_ag_if_ans_t ans = {0};
  return ans;
}

