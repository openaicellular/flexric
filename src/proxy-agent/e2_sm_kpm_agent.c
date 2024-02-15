/* 
 * Layer between flexric E2SM callbacks and WS interface
 * it manages SUBSCRIPTION procedure (indication) and CTRL procedure
 */
#include "e2_sm_kpm_agent.h"
#include <stdio.h>
#include <libwebsockets.h>

#include "sm/kpm_sm/kpm_data_ie_wrapper.h"
#include "../../test/rnd/fill_rnd_data_kpm.h"
#include "util/time_now_us.h"
#include "util/alg_ds/alg/murmur_hash_32.h"
#include "util/byte_array.h"
#include "util/alg_ds/alg/defer.h"
#include "util/alg_ds/ds/assoc_container/assoc_generic.h"

#include "e2_sm_agent.h"
#include "ran_if.h"
#include "proxy_agent.h"
#include "ran_msg_hdlr.h"
#include "ringbuffer.h"
#include "notif_e2_ran.h"

typedef struct {
  size_t amr_ue_idx;
  ue_id_e2sm_t e2sm_ue_id;
} matched_ues_t;

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
 meas_record_lst_t fill_DRB_PdcpSduVolumeDL(amarisoft_ue_stats_t const* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  (void) ran_config;
  assert(ue_stats != NULL);
  //printf(" fill_DRB_PdcpSduVolumeDL\n");
  meas_record_lst_t meas_record = {0};
  meas_record.value = INTEGER_MEAS_VALUE;
  meas_record.int_val = ue_stats->cells[cell_idx].dl_tx*8; // bits// TODO:
  return meas_record;
}

static
 meas_record_lst_t fill_DRB_PdcpSduVolumeUL(amarisoft_ue_stats_t const* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  (void) ran_config;
  assert(ue_stats != NULL);
  //printf(" fill_DRB_PdcpSduVolumeUL\n");
  meas_record_lst_t meas_record = {0};
  meas_record.value = INTEGER_MEAS_VALUE;
  meas_record.int_val = ue_stats->cells[cell_idx].ul_tx * 8; // bits TODO:
  return meas_record;
}

static
 meas_record_lst_t fill_DRB_RlcSduDelayDl(amarisoft_ue_stats_t const* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  (void) ran_config;
  (void) cell_idx;
  assert(ue_stats != NULL);
  //printf("fill_DRB_RlcSduDelayDl \n");
  printf("[E2-AGENT] No implementation fill_DRB_RlcSduDelayDl - Fill random value\n");
  return fill_rnd_float_data();
}

static
 meas_record_lst_t fill_DRB_UEThpDl( amarisoft_ue_stats_t const* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  (void) ran_config;
  assert(ue_stats != NULL);
  //printf(" fill_DRB_UEThpDl\n");

  meas_record_lst_t meas_record = {0};
  meas_record.value = REAL_MEAS_VALUE;
  meas_record.real_val = ue_stats->cells[cell_idx].dl_bitrate/1000; //Kbps // TODO:
  return meas_record;
}

static
 meas_record_lst_t fill_DRB_UEThpUl(amarisoft_ue_stats_t const* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  (void) ran_config;
  (void) cell_idx;
  assert(ue_stats != NULL);
  //printf(" fill_DRB_UEThpUl\n");
  meas_record_lst_t meas_record = {0};
  meas_record.value = REAL_MEAS_VALUE;
  meas_record.real_val = ue_stats->cells[cell_idx].ul_bitrate/1000; // Kbps // TODO:
  return meas_record;
}

static
 meas_record_lst_t fill_RRU_PrbTotDl(amarisoft_ue_stats_t const* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  (void) ran_config;
  (void) cell_idx;
  assert(ue_stats != NULL);
  printf("[E2-AGENT] No implementation fill_RRU_PrbTotDl - Fill random value\n");
  return fill_rnd_int_data();
}

static
 meas_record_lst_t fill_RRU_PrbTotUl(amarisoft_ue_stats_t const* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  (void) ran_config;
  (void) cell_idx;
  assert(ue_stats != NULL);
  printf("[E2-AGENT] No implementation fill_RRU_PrbTotUl - Fill random value\n");
  return fill_rnd_int_data();
}

static
assoc_ht_open_t ht;

typedef meas_record_lst_t (*kpm_fp)(const amarisoft_ue_stats_t* ue_stats, const ran_config_t* ran_config, const size_t cell_idx);

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
gnb_e2sm_t fill_gnb_data(const amarisoft_ue_stats_t* ue, global_e2_node_id_t id)
{
  gnb_e2sm_t gnb = {0};

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  gnb.amf_ue_ngap_id = ue->amf_ue_id;

  // Mandatory
  //GUAMI 6.2.3.17
  gnb.guami.plmn_id = (e2sm_plmn_t) {.mcc = id.plmn.mcc, .mnc = id.plmn.mnc, .mnc_digit_len = id.plmn.mnc_digit_len};
  // TODO: need metrics from AMR
  gnb.guami.amf_region_id = (rand() % 2^8) + 0;
  // TODO: need metrics from AMR
  gnb.guami.amf_set_id = (rand() % 2^10) + 0;
  // TODO: need metrics from AMR
  gnb.guami.amf_ptr = (rand() % 2^6) + 0;

  gnb.ran_ue_id = (uint64_t*)malloc(sizeof(uint64_t));
  *(gnb.ran_ue_id) = (uint64_t)ue->ran_ue_id;
  return gnb;
}

static
ue_id_e2sm_t fill_rnd_ue_id_data(void)
{
  ue_id_e2sm_t ue_id_data = {0};
  ue_id_data.type = GNB_UE_ID_E2SM;
  ue_id_data.gnb.amf_ue_ngap_id = (rand() % 2^40) + 0;
  ue_id_data.gnb.guami.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  ue_id_data.gnb.guami.amf_region_id = (rand() % 2^8) + 0;
  ue_id_data.gnb.guami.amf_set_id = (rand() % 2^10) + 0;
  ue_id_data.gnb.guami.amf_ptr = (rand() % 2^6) + 0;
  ue_id_data.gnb.ran_ue_id = (uint64_t*)malloc(sizeof(uint64_t));
  *(ue_id_data.gnb.ran_ue_id) = 123;
  return ue_id_data;
}

static
meas_info_format_1_lst_t * fill_kpm_meas_info_frm_1(const size_t len, const kpm_act_def_format_1_t * act_def_fr_1)
{
  meas_info_format_1_lst_t* meas_info_lst = calloc(len, sizeof(meas_info_format_1_lst_t));
  assert(meas_info_lst != NULL && "Memory exhausted" );

  // Get measInfo from action definition
  for (size_t i = 0; i < len; i++)
  {
    // Measurement Type
    meas_info_lst[i].meas_type.type = act_def_fr_1->meas_info_lst[i].meas_type.type;
    // Measurement Name
    if (act_def_fr_1->meas_info_lst[i].meas_type.type == NAME_MEAS_TYPE) {
      meas_info_lst[i].meas_type.name.buf = calloc(act_def_fr_1->meas_info_lst[i].meas_type.name.len, sizeof(uint8_t));
      memcpy(meas_info_lst[i].meas_type.name.buf, act_def_fr_1->meas_info_lst[i].meas_type.name.buf, act_def_fr_1->meas_info_lst[i].meas_type.name.len);
      meas_info_lst[i].meas_type.name.len = act_def_fr_1->meas_info_lst[i].meas_type.name.len;
    } else {
      meas_info_lst[i].meas_type.id = act_def_fr_1->meas_info_lst[i].meas_type.id;
    }


    // Label Information
    meas_info_lst[i].label_info_lst_len = 1;
    meas_info_lst[i].label_info_lst = calloc(meas_info_lst[i].label_info_lst_len, sizeof(label_info_lst_t));
    assert(meas_info_lst[i].label_info_lst != NULL && "Memory exhausted" );

    for (size_t j = 0; j < meas_info_lst[i].label_info_lst_len; j++) {
      meas_info_lst[i].label_info_lst[j].noLabel = malloc(sizeof(enum_value_e));
      *meas_info_lst[i].label_info_lst[j].noLabel = TRUE_ENUM_VALUE;
    }
  }

  return meas_info_lst;
}

static
meas_record_lst_t fill_meas_value(meas_type_t meas_info_type, const amarisoft_ue_stats_t* ue_stats, const ran_config_t* ran_config, const size_t cell_idx)
{
  meas_record_lst_t meas_record = {0};
  // Get Meas Info Name from Action Definition
  const void* key = meas_info_type.name.buf;

  void* value = assoc_ht_open_value(&ht, &key);
  meas_record = (*(kpm_fp*)value)(ue_stats, ran_config, cell_idx);

  return meas_record;
}

static
kpm_ind_msg_format_1_t fill_kpm_ind_msg_frm_1_in_monolithic(const amarisoft_ue_stats_t* ue, const kpm_act_def_format_1_t* act_def_fr1, const ran_ind_t* ws_ind)
{
  (void)ws_ind;
  kpm_ind_msg_format_1_t msg_frm_1 = {0};

  // Measurement Data
  uint32_t num_cells = ue->len_cell;
  msg_frm_1.meas_data_lst_len = num_cells;
  msg_frm_1.meas_data_lst = calloc(msg_frm_1.meas_data_lst_len, sizeof(*msg_frm_1.meas_data_lst));
  assert(msg_frm_1.meas_data_lst != NULL && "Memory exhausted" );

  size_t const rec_data_len = act_def_fr1->meas_info_lst_len; // Recoding Data Length
  for (size_t i = 0; i < msg_frm_1.meas_data_lst_len; i++){
    meas_data_lst_t* meas_data = &msg_frm_1.meas_data_lst[i];
    // ue_cell_t ue_cell_stats = ue.cells[i];
    // Measurement Record
    meas_data->meas_record_len = rec_data_len;
    meas_data->meas_record_lst = calloc(meas_data->meas_record_len, sizeof(meas_record_lst_t));
    assert(meas_data->meas_record_lst != NULL && "Memory exhausted" );
    for (size_t j = 0; j < msg_frm_1.meas_data_lst[i].meas_record_len; j++) {
      meas_type_t meas_info_type = act_def_fr1->meas_info_lst[j].meas_type;
      switch (act_def_fr1->meas_info_lst[j].meas_type.type)
      {
        case NAME_MEAS_TYPE:
        {
          meas_data->meas_record_lst[j] = fill_meas_value(meas_info_type, ue, &ws_ind->ran_config, i);
          break;
        }
        case ID_MEAS_TYPE:
          assert(false && "ID Measurement Type not yet implemented");
          break;

        default:
          assert(false && "Measurement Type not recognized");
          break;
      }
    }
  }

  // Measurement Information - OPTIONAL
  msg_frm_1.meas_info_lst_len = act_def_fr1->meas_info_lst_len;
  msg_frm_1.meas_info_lst = fill_kpm_meas_info_frm_1(msg_frm_1.meas_info_lst_len, act_def_fr1);

  return msg_frm_1;
}

static kpm_ind_msg_format_1_t fill_rnd_kpm_ind_msg_frm_1(void)
{
  kpm_ind_msg_format_1_t msg_frm_1 = {0};

  // Measurement Data
  msg_frm_1.meas_data_lst_len = 1;  // (rand() % 65535) + 1;
  msg_frm_1.meas_data_lst = calloc(msg_frm_1.meas_data_lst_len, sizeof(*msg_frm_1.meas_data_lst));
  assert(msg_frm_1.meas_data_lst != NULL && "Memory exhausted" );

  for (size_t i = 0; i < msg_frm_1.meas_data_lst_len; i++)
  {
    // Incomplete Flag
    msg_frm_1.meas_data_lst[i].incomplete_flag = calloc(1, sizeof(enum_value_e));
    assert(msg_frm_1.meas_data_lst[i].incomplete_flag != NULL && "Memory exhausted");
    *msg_frm_1.meas_data_lst[i].incomplete_flag = TRUE_ENUM_VALUE;

    // Measurement Record
    msg_frm_1.meas_data_lst[i].meas_record_len = 1;
    msg_frm_1.meas_data_lst[i].meas_record_lst = calloc(msg_frm_1.meas_data_lst[i].meas_record_len, sizeof(meas_record_lst_t));
    assert(msg_frm_1.meas_data_lst[i].meas_record_lst != NULL && "Memory exhausted" );

    for (size_t j = 0; j < msg_frm_1.meas_data_lst[i].meas_record_len; j++)
    {
      msg_frm_1.meas_data_lst[i].meas_record_lst[j].value = REAL_MEAS_VALUE;
      msg_frm_1.meas_data_lst[i].meas_record_lst[j].real_val = time_now_us();
    }
  }

  // Measurement Information - OPTIONAL
  msg_frm_1.meas_info_lst_len = 1;
  msg_frm_1.meas_info_lst = calloc(msg_frm_1.meas_info_lst_len, sizeof(meas_info_format_1_lst_t));
  assert(msg_frm_1.meas_info_lst != NULL && "Memory exhausted" );

  for (size_t i = 0; i < msg_frm_1.meas_info_lst_len; i++)
  {
    // Measurement Type
    char* s = "timestamp";
    msg_frm_1.meas_info_lst[i].meas_type.name.len = strlen(s) + 1;
    msg_frm_1.meas_info_lst[i].meas_type.name.buf = malloc(strlen(s) + 1);
    assert(msg_frm_1.meas_info_lst[i].meas_type.name.buf != NULL && "memory exhausted");
    memcpy(msg_frm_1.meas_info_lst[i].meas_type.name.buf, s, strlen(s));
    msg_frm_1.meas_info_lst[i].meas_type.name.buf[strlen(s)] = '\0';

    // Label Information
    msg_frm_1.meas_info_lst[i].label_info_lst_len = 1;
    msg_frm_1.meas_info_lst[i].label_info_lst = calloc(msg_frm_1.meas_info_lst[i].label_info_lst_len, sizeof(label_info_lst_t));
    assert(msg_frm_1.meas_info_lst[i].label_info_lst != NULL && "Memory exhausted" );

    for (size_t j = 0; j < msg_frm_1.meas_info_lst[i].label_info_lst_len; j++)
    {
      msg_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = malloc(sizeof(enum_value_e));
      *msg_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = TRUE_ENUM_VALUE;
    }
  }

  return msg_frm_1;
}

static
kpm_ind_msg_format_3_t fill_kpm_ind_msg_frm_3_in_monolithic(seq_arr_t const* ues, const kpm_act_def_format_1_t * act_def_fr_1, const ran_ind_t* ws_ind)
{
  assert(act_def_fr_1 != NULL);

  kpm_ind_msg_format_3_t msg_frm_3 = {0};

  const size_t sz = seq_size((seq_arr_t*)ues);

  // Fill UE Measurement Reports
  msg_frm_3.ue_meas_report_lst_len = sz;
  if (sz > 0){
    msg_frm_3.meas_report_per_ue = calloc(sz, sizeof(meas_report_per_ue_t ));
    assert(msg_frm_3.meas_report_per_ue != NULL && "Memory exhausted");
  }

  void* it = seq_front((seq_arr_t*)ues);
  for(size_t i = 0; i < sz; ++i){
    matched_ues_t const* ue = (matched_ues_t const*)it;

    // Fill UE ID data
    msg_frm_3.meas_report_per_ue[i].ue_meas_report_lst = cp_ue_id_e2sm(&ue->e2sm_ue_id);

    // Fill UE related info
    msg_frm_3.meas_report_per_ue[i].ind_msg_format_1 = fill_kpm_ind_msg_frm_1_in_monolithic(&ws_ind->ue_stats[ue->amr_ue_idx], act_def_fr_1, ws_ind);

    it = seq_next((seq_arr_t*)ues, it);
  }

  return msg_frm_3;
}

static
kpm_ind_msg_format_3_t fill_rnd_kpm_ind_msg_frm_3()
{
  kpm_ind_msg_format_3_t msg_frm_3 = {0};

  // Fill UE Measurement Reports
  msg_frm_3.ue_meas_report_lst_len = 1;
  msg_frm_3.meas_report_per_ue = calloc(msg_frm_3.ue_meas_report_lst_len, sizeof(meas_report_per_ue_t));
  assert(msg_frm_3.meas_report_per_ue != NULL && "Memory exhausted");
  // Fill UE ID
  msg_frm_3.meas_report_per_ue[0].ue_meas_report_lst = fill_rnd_ue_id_data();
  // Fill UE related info
  msg_frm_3.meas_report_per_ue[0].ind_msg_format_1 = fill_rnd_kpm_ind_msg_frm_1();

  return msg_frm_3;
}

static
kpm_ric_ind_hdr_format_1_t kpm_ind_hdr_frm_1(ran_ind_t* ws_ind)
{
  kpm_ric_ind_hdr_format_1_t hdr_frm_1 = {0};

  int64_t const t = time_now_us();
#if defined(KPM_V2_01) || defined (KPM_V2_03)
  hdr_frm_1.collectStartTime = t/1000000; // seconds
#elif defined(KPM_V3_00)
  hdr_frm_1.collectStartTime = t; // microseconds
#else
  static_assert(0!=0, "Unknown KPM version");
#endif

  hdr_frm_1.fileformat_version = NULL;

  // Check E2 Node NG-RAN Type
  if (E2AP_NODE_IS_MONOLITHIC(ws_ind->global_e2_node_id.type))
  {
    hdr_frm_1.sender_name = calloc(1, sizeof(byte_array_t));
    hdr_frm_1.sender_name->buf = calloc(strlen("AMR-MONO") + 1, sizeof(uint8_t));
    memcpy(hdr_frm_1.sender_name->buf, "AMR-MONO", strlen("AMR-MONO"));
    hdr_frm_1.sender_name->len = strlen("AMR-MONO");

    hdr_frm_1.sender_type = calloc(1, sizeof(byte_array_t));
    hdr_frm_1.sender_type->buf = calloc(strlen("MONO") + 1, sizeof(uint8_t));
    memcpy(hdr_frm_1.sender_type->buf, "MONO", strlen("MONO"));
    hdr_frm_1.sender_type->len = strlen("MONO");
  } else {
    assert(0!=0 && "Unknown node type");
  }

  hdr_frm_1.vendor_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.vendor_name->buf = calloc(strlen("Amarisoft") + 1, sizeof(uint8_t));
  memcpy(hdr_frm_1.vendor_name->buf, "Amarisoft", strlen("Amarisoft"));
  hdr_frm_1.vendor_name->len = strlen("Amarisoft");

  return hdr_frm_1;
}

static
kpm_ind_hdr_t kpm_ind_hdr(ran_ind_t* ws_ind)
{
  kpm_ind_hdr_t hdr = {0};

  hdr.type = FORMAT_1_INDICATION_HEADER;
  hdr.kpm_ric_ind_hdr_format_1 = kpm_ind_hdr_frm_1(ws_ind);

  return hdr;
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
  seq_init(&dst, sizeof(matched_ues_t )); //

  for(size_t i = 0; i < num_ues; ++i){
    bool const select_ue = ue_fullfills_predicate(*info->test_cond, *info->test_cond_value->int_value );
    if(select_ue){
//      ue_id_e2sm_t ue = fill_rnd_ue_id_data();
      matched_ues_t ue = {0};
      ue.e2sm_ue_id = fill_rnd_ue_id_data();
      ue.amr_ue_idx = i;
      seq_push_back(&dst, &ue, sizeof(matched_ues_t ));
    }
  }
  return dst;
}
static
bool amarisoft_ue_fullfills_predicate(test_cond_e cond, int64_t value, amarisoft_ue_stats_t ue_stats)
{
  /* IMPORTANT: in the case of gNB-DU, it is not possible to filter UEs by S-NSSAI as the ngap context is stored in gNB-CU
              instead, we take all connected UEs*/

  // Take MAC info
  assert(cond == EQUAL_TEST_COND
         || cond == GREATERTHAN_TEST_COND
         || cond == CONTAINS_TEST_COND
         || cond == PRESENT_TEST_COND
  );

  switch (cond)
  {
    case EQUAL_TEST_COND:
      if (ue_stats.len_qos_flow > 0) {
        if (ue_stats.qos_flows[0].sst == value)
          printf("condition map, find the same sst %ld\n", value);
      }
      return true;
    case GREATERTHAN_TEST_COND:
      if (ue_stats.len_qos_flow > 0) {
        if (ue_stats.qos_flows[0].sst == value)
          printf("condition map, find the same sst %ld\n", value);
      }
      return true;
    default:
      printf("[E2-agent] Condition not yet implemented\n");
      return false;
  }
}

static
seq_arr_t amarisoft_ues_fullfilling_pred(test_info_lst_t const*  info, ran_ind_t* ws_ind)
{
  assert(info != NULL);

  seq_arr_t dst = {0};
  seq_init(&dst, sizeof(matched_ues_t)); //

  for(size_t i = 0; i < ws_ind->len_ue_stats; ++i){
    amarisoft_ue_stats_t ue_stats = ws_ind->ue_stats[i];
    bool const select_ue = amarisoft_ue_fullfills_predicate(*info->test_cond, *info->test_cond_value->int_value, ue_stats);
    if(select_ue){
      matched_ues_t ue = {0};
      ue.amr_ue_idx = i;
      ue.e2sm_ue_id.type = GNB_UE_ID_E2SM;
      ue.e2sm_ue_id.gnb = fill_gnb_data(&ue_stats, ws_ind->global_e2_node_id);
      seq_push_back(&dst, &ue, sizeof(matched_ues_t ));
    }
  }
  return dst;
}


static
seq_arr_t match_gbr_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == GBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_ambr_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == AMBR_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_isstat_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == IsStat_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_iscatm_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == IsCatM_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrp_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == DL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_dl_rsrq_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == DL_RSRQ_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}


static
seq_arr_t match_ul_rsrp_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == UL_RSRP_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_cqi_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  assert(info != NULL);
  assert(info->test_cond_type == CQI_TEST_COND_TYPE);
  assert(info->test_cond != NULL && "Even though is optional..");
  assert(info->test_cond_value != NULL && "Even though is optional..");

  // Check E2 Node NG-RAN Type
  if (E2AP_NODE_IS_MONOLITHIC(ws_ind.global_e2_node_id.type)) {
    return amarisoft_ues_fullfilling_pred(info, &ws_ind);
  } else {
    assert(false && "NG-RAN Type not implemented");
  }
}

static
seq_arr_t match_fiveqi_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  (void) ws_ind;
  assert(info != NULL);
  assert(info->test_cond_type == fiveQI_TEST_COND_TYPE);

  return emulate_ues_fullfilling_pred(info);
}

static
seq_arr_t match_qci_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  assert(info != NULL);
  assert(info->test_cond_type == CQI_TEST_COND_TYPE);
  assert(info->test_cond != NULL && "Even though is optional..");
  assert(info->test_cond_value != NULL && "Even though is optional..");

  // Check E2 Node NG-RAN Type
  if (E2AP_NODE_IS_MONOLITHIC(ws_ind.global_e2_node_id.type)) {
    return amarisoft_ues_fullfilling_pred(info, &ws_ind);
  } else {
    assert(false && "NG-RAN Type not implemented");
  }
}

static
seq_arr_t match_s_nssai_test_cond_type(test_info_lst_t const* info, ran_ind_t ws_ind)
{
  assert(info != NULL);
  assert(info->test_cond_type == S_NSSAI_TEST_COND_TYPE);
  assert(info->test_cond != NULL && "Even though is optional..");
  assert(info->test_cond_value != NULL && "Even though is optional..");

  // Check E2 Node NG-RAN Type
  if (E2AP_NODE_IS_MONOLITHIC(ws_ind.global_e2_node_id.type)) {
    return amarisoft_ues_fullfilling_pred(info, &ws_ind);
  } else {
    assert(false && "NG-RAN Type not implemented");
  }
}

typedef seq_arr_t (*fp_arr_cond)(test_info_lst_t const* info, ran_ind_t ws_ind);

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
seq_arr_t matching_ues(matching_condition_format_4_lst_t const* cond, size_t len, ran_ind_t ws_ind)
{
  assert(cond != NULL && "Condition equal NULL");
  assert(len == 1 && "Only one condition supported");

  seq_arr_t dst = match_cond_arr[cond->test_info_lst.test_cond_type](&cond->test_info_lst, ws_ind);

  return dst;
}

static
void free_matched_ues(matched_ues_t* it){
  assert(it != NULL);
  free_ue_id_e2sm(&it->e2sm_ue_id);
}

static
void free_ue_id_e2sm_wrapper(void* it)
{
  assert(it != NULL);
  free_matched_ues((matched_ues_t*) it);
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
  ran_ind_t ws_ind = get_ringbuffer_data();

  kpm_rd_ind_data_t* kpm = (kpm_rd_ind_data_t*)data;
  kpm_act_def_t const* act_def = kpm->act_def;
  assert(act_def!= NULL && "Cannot be NULL");
  switch (act_def->type) {
    case FORMAT_4_ACTION_DEFINITION:
      kpm->ind.hdr = kpm_ind_hdr(&ws_ind);

      kpm->ind.msg.type = FORMAT_3_INDICATION_MESSAGE;
      // Filter the UE by the test condition criteria
      kpm_act_def_format_4_t const* frm_4 = &kpm->act_def->frm_4; // 8.2.1.2.4
      // Matching UEs
      seq_arr_t match_ues = matching_ues(frm_4->matching_cond_lst, frm_4->matching_cond_lst_len, ws_ind);
//      printf("[E2-AGENT] Number of match ues: %ld \n", seq_size(&match_ues));
      // If no UEs match the condition, do not send data to the nearRT-RIC
      if(seq_size(&match_ues) == 0){
        seq_arr_free(&match_ues, free_ue_id_e2sm_wrapper);
        return false;
      }

      kpm_ind_msg_format_3_t info = fill_kpm_ind_msg_frm_3_in_monolithic(&match_ues, &frm_4->action_def_format_1, &ws_ind);
      // Message
      // 7.8 Supported RIC Styles and E2SM IE Formats
      // Format 4 corresponds to indication message 3
      kpm->ind.msg.type = FORMAT_3_INDICATION_MESSAGE;
      kpm->ind.msg.frm_3 = info;
      seq_arr_free(&match_ues, free_ue_id_e2sm_wrapper);
      break;
    default: {
      printf("Not supported action definition type %d, fill the dummy indication msg\n", act_def->type);
      kpm->ind.hdr = fill_rnd_kpm_ind_hdr();
      kpm->ind.msg.frm_3 = fill_rnd_kpm_ind_msg_frm_3();
      break;
    }
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

