#include "sm_kpm.h"
#include "../../../test/rnd/fill_rnd_data_kpm.h"
#include "../../../src/util/time_now_us.h"
#include <assert.h>
#include <stdio.h>

#ifdef TEST_AGENT_GNB
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

  return gnb;
}
#endif

#ifdef TEST_AGENT_GNB_DU
static
gnb_du_e2sm_t fill_rnd_gnb_du_data(void)
{
  gnb_du_e2sm_t gnb_du = {0};
  gnb_du.gnb_cu_ue_f1ap = (rand() % 4294967296) + 0;

  gnb_du.ran_ue_id = calloc(1, sizeof(*gnb_du.ran_ue_id));
  *gnb_du.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_du;
}
#endif

#ifdef TEST_AGENT_GNB_CU
static
gnb_cu_up_e2sm_t fill_rnd_gnb_cu_up_data(void)
{
  gnb_cu_up_e2sm_t gnb_cu_up = {0};
  gnb_cu_up.gnb_cu_cp_ue_e1ap = (rand() % 4294967296) + 0;

  gnb_cu_up.ran_ue_id = calloc(1, sizeof(*gnb_cu_up.ran_ue_id));
  *gnb_cu_up.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_cu_up;
}
#endif

#ifdef TEST_AGENT_ENB
static
enb_e2sm_t fill_rnd_enb_data(void)
{
  enb_e2sm_t enb = {0};

  // 6.2.3.26
  // Mandatory
  // MME UE S1AP ID
  enb.mme_ue_s1ap_id = (rand() % 4294967296) + 0;

  // 6.2.3.18
  // Mandatory
  // GUMMEI
  enb.gummei.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  enb.gummei.mme_group_id = (rand() % 2^16) + 0;
  enb.gummei.mme_code = (rand() % 2^8) + 0;

  // 6.2.3.23
  // C-ifDCSetup
  // MeNB UE X2AP ID
  enb.enb_ue_x2ap_id = calloc(1, sizeof(uint16_t));
  assert(enb.enb_ue_x2ap_id != NULL && "Memory exhausted");
  *enb.enb_ue_x2ap_id = (rand() % 4095) + 1;

  // 6.2.3.24
  // C-ifDCSetup
  // MeNB UE X2AP ID Extension
  enb.enb_ue_x2ap_id_extension = calloc(1, sizeof(uint16_t));
  assert(enb.enb_ue_x2ap_id_extension != NULL && "Memory exhausted");
  *enb.enb_ue_x2ap_id_extension = (rand() % 4095) + 1;

  // 6.2.3.9
  // C-ifDCSetup
  // Global eNB ID
  enb.global_enb_id = calloc(1, sizeof(*enb.global_enb_id));
  assert(enb.global_enb_id != NULL && "Memory exhausted");

  enb.global_enb_id->plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};

  enb.global_enb_id->type = MACRO_ENB_TYPE_ID;  // rand()%END_ENB_TYPE_ID;

  switch (enb.global_enb_id->type)
  {
    case MACRO_ENB_TYPE_ID:
      enb.global_enb_id->macro_enb_id = (rand() % 2^20) + 0;
      break;

    case HOME_ENB_TYPE_ID:
      enb.global_enb_id->home_enb_id = (rand() % 2^28) + 0;
      break;

      /* Possible extensions: */
      // case SHORT_MACRO_ENB_TYPE_ID:
      //   enb.global_enb_id->short_macro_enb_id = (rand() % 2^18) + 0;
      //   break;

      // case LONG_MACRO_ENB_TYPE_ID:
      //   enb.global_enb_id->long_macro_enb_id = (rand() % 2^21) + 0;
      //   break;

    default:
      break;
  }

  return enb;
}
#endif

static 
ue_id_e2sm_t fill_rnd_ue_id_data(void)
{
  ue_id_e2sm_t ue_id_data = {0};

#ifdef TEST_AGENT_GNB
    ue_id_data.type = GNB_UE_ID_E2SM;
    ue_id_data.gnb = fill_rnd_gnb_data();
#elif defined(TEST_AGENT_GNB_DU)
    ue_id_data.type = GNB_DU_UE_ID_E2SM;
    ue_id_data.gnb_du = fill_rnd_gnb_du_data();
#elif defined(TEST_AGENT_GNB_CU)
    ue_id_data.type = GNB_CU_UP_UE_ID_E2SM;
    ue_id_data.gnb_cu_up = fill_rnd_gnb_cu_up_data();
#elif defined(TEST_AGENT_ENB)
    ue_id_data.type = ENB_UE_ID_E2SM;
    ue_id_data.enb = fill_rnd_enb_data();
#else
  static_assert(0!=0, "not support TEST_AGENT_RAN_TYPE");
#endif

  return ue_id_data;
}

static 
kpm_ind_msg_format_1_t fill_rnd_kpm_ind_msg_frm_1(kpm_act_def_format_1_t act_def_fr1)
{
  kpm_ind_msg_format_1_t msg_frm_1 = {0};

  // Measurement Data
  uint32_t num_drbs = 2;
  msg_frm_1.meas_data_lst_len = num_drbs;  // (rand() % 65535) + 1;
  msg_frm_1.meas_data_lst = calloc(msg_frm_1.meas_data_lst_len, sizeof(*msg_frm_1.meas_data_lst));
  assert(msg_frm_1.meas_data_lst != NULL && "Memory exhausted" );

  size_t const rec_data_len = act_def_fr1.meas_info_lst_len; // Recoding Data Length
  for (size_t i = 0; i < msg_frm_1.meas_data_lst_len; i++){
    if (rand() % 2) {
      // Incomplete Flag
      msg_frm_1.meas_data_lst[i].incomplete_flag = calloc(1, sizeof(enum_value_e));
      assert(msg_frm_1.meas_data_lst[i].incomplete_flag != NULL && "Memory exhausted");
      *msg_frm_1.meas_data_lst[i].incomplete_flag = TRUE_ENUM_VALUE;
    }

    // Measurement Record
    msg_frm_1.meas_data_lst[i].meas_record_len = rec_data_len;
    msg_frm_1.meas_data_lst[i].meas_record_lst = calloc(msg_frm_1.meas_data_lst[i].meas_record_len, sizeof(meas_record_lst_t));
    assert(msg_frm_1.meas_data_lst[i].meas_record_lst != NULL && "Memory exhausted" );

    for (size_t j = 0; j < msg_frm_1.meas_data_lst[i].meas_record_len; j++) {
      msg_frm_1.meas_data_lst[i].meas_record_lst[j].value = rand()%END_MEAS_VALUE;
      if (msg_frm_1.meas_data_lst[i].meas_record_lst[j].value == REAL_MEAS_VALUE) {
        msg_frm_1.meas_data_lst[i].meas_record_lst[j].real_val = (rand() % 256) + 0.1;
      } else if (msg_frm_1.meas_data_lst[i].meas_record_lst[j].value == INTEGER_MEAS_VALUE) {
        msg_frm_1.meas_data_lst[i].meas_record_lst[j].int_val = (rand() % 256);
      } else {
        msg_frm_1.meas_data_lst[i].meas_record_lst[j].no_value = NULL;
      }
    }
  }

  // Measurement Information - OPTIONAL
  msg_frm_1.meas_info_lst_len = rec_data_len;
  msg_frm_1.meas_info_lst = calloc(msg_frm_1.meas_info_lst_len, sizeof(meas_info_format_1_lst_t));
  assert(msg_frm_1.meas_info_lst != NULL && "Memory exhausted" );

  for (size_t i = 0; i < msg_frm_1.meas_info_lst_len; i++) {
    // Measurement Type
    msg_frm_1.meas_info_lst[i].meas_type.type = act_def_fr1.meas_info_lst[i].meas_type.type;
    // Measurement Name
    if (act_def_fr1.meas_info_lst[i].meas_type.type == NAME_MEAS_TYPE) {
      msg_frm_1.meas_info_lst[i].meas_type.name.buf = calloc(act_def_fr1.meas_info_lst[i].meas_type.name.len, sizeof(uint8_t));
      memcpy(msg_frm_1.meas_info_lst[i].meas_type.name.buf, act_def_fr1.meas_info_lst[i].meas_type.name.buf, act_def_fr1.meas_info_lst[i].meas_type.name.len);
      msg_frm_1.meas_info_lst[i].meas_type.name.len = act_def_fr1.meas_info_lst[i].meas_type.name.len;
    } else {
      msg_frm_1.meas_info_lst[i].meas_type.id = act_def_fr1.meas_info_lst[i].meas_type.id;
    }

    // Label Information
    msg_frm_1.meas_info_lst[i].label_info_lst_len = 1;
    msg_frm_1.meas_info_lst[i].label_info_lst = calloc(msg_frm_1.meas_info_lst[i].label_info_lst_len, sizeof(label_info_lst_t));
    assert(msg_frm_1.meas_info_lst[i].label_info_lst != NULL && "Memory exhausted" );

    for (size_t j = 0; j < msg_frm_1.meas_info_lst[i].label_info_lst_len; j++) {
      msg_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = malloc(sizeof(enum_value_e));
      *msg_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = TRUE_ENUM_VALUE;
    }
  }

  // Granularity Period - OPTIONAL: (1..4294967295)
  msg_frm_1.gran_period_ms = calloc(1, sizeof(*msg_frm_1.gran_period_ms));
  assert(msg_frm_1.gran_period_ms != NULL && "Memory exhausted");
  *msg_frm_1.gran_period_ms = act_def_fr1.gran_period_ms;

  return msg_frm_1;
}

static 
kpm_ind_msg_format_3_t fill_kpm_ind_msg_frm_3_sta(kpm_act_def_format_1_t act_def_fr1)
{
  kpm_ind_msg_format_3_t msg_frm_3 = {0};

  uint32_t num_ues = 3;
  msg_frm_3.ue_meas_report_lst_len = num_ues;  // (rand() % 65535) + 1;

  msg_frm_3.meas_report_per_ue = calloc(msg_frm_3.ue_meas_report_lst_len, sizeof(meas_report_per_ue_t));
  assert(msg_frm_3.meas_report_per_ue != NULL && "Memory exhausted");

  for (size_t i = 0; i < msg_frm_3.ue_meas_report_lst_len; i++)
  {
    msg_frm_3.meas_report_per_ue[i].ue_meas_report_lst = fill_rnd_ue_id_data();
    msg_frm_3.meas_report_per_ue[i].ind_msg_format_1 = fill_rnd_kpm_ind_msg_frm_1(act_def_fr1);
  }

  return msg_frm_3;
}

static 
kpm_ric_ind_hdr_format_1_t fill_rnd_kpm_ind_hdr_frm_1(void)
{
  kpm_ric_ind_hdr_format_1_t hdr_frm_1 = {0};

  int64_t t = time_now_us();
#ifdef KPM_V2
  hdr_frm_1.collectStartTime = t / 1000000; // needs to be truncated to 32 bits to arrive to a resolution of seconds
#elif defined(KPM_V3)
  hdr_frm_1.collectStartTime = t;
#else
  static_assert(0!=0, "Unknown KPM version");
#endif

  hdr_frm_1.fileformat_version = NULL;
  
  hdr_frm_1.sender_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_name->buf = calloc(strlen("My E2-Node") + 1, sizeof(char));
  memcpy(hdr_frm_1.sender_name->buf, "My E2-Node", strlen("My E2-Node"));
  hdr_frm_1.sender_name->len = strlen("My E2-Node") + 1;

  hdr_frm_1.sender_type = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_type->buf = calloc(strlen("E2-Node") + 1, sizeof(char));
  memcpy(hdr_frm_1.sender_type->buf, "E2-Node", strlen("E2-Node"));
  hdr_frm_1.sender_type->len = strlen("E2-Node") + 1;

  hdr_frm_1.vendor_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.vendor_name->buf = calloc(strlen("Unknown") + 1, sizeof(char));
  memcpy(hdr_frm_1.vendor_name->buf, "Unknown", strlen("Unknown"));
  hdr_frm_1.vendor_name->len = strlen("Unknown") + 1;

  return hdr_frm_1;
}

static
kpm_ind_hdr_t fill_kpm_ind_hdr_sta(void)
{
  kpm_ind_hdr_t hdr = {0};

  hdr.type = FORMAT_1_INDICATION_HEADER;
  hdr.kpm_ric_ind_hdr_format_1 = fill_rnd_kpm_ind_hdr_frm_1();

  return hdr;
}

void read_kpm_sm(void* data)
{
  assert(data != NULL);
  //assert(data->type == KPM_STATS_V3_0);

  kpm_rd_ind_data_t* kpm = (kpm_rd_ind_data_t*)data;

  assert(kpm->act_def!= NULL && "Cannot be NULL");
  if (kpm->act_def->type == FORMAT_1_ACTION_DEFINITION) {
    kpm->ind.hdr = fill_kpm_ind_hdr_sta();
    kpm->ind.msg.type = FORMAT_1_INDICATION_MESSAGE;
    kpm->ind.msg.frm_1 = fill_rnd_kpm_ind_msg_frm_1(kpm->act_def->frm_1);
  } else if (kpm->act_def->type == FORMAT_4_ACTION_DEFINITION) {

    if(kpm->act_def->frm_4.matching_cond_lst[0].test_info_lst.test_cond_type == CQI_TEST_COND_TYPE
        && *kpm->act_def->frm_4.matching_cond_lst[0].test_info_lst.test_cond == GREATERTHAN_TEST_COND){
      printf("Matching condition: UEs with CQI greater than %ld \n", *kpm->act_def->frm_4.matching_cond_lst[0].test_info_lst.int_value );
    }
    for (size_t i = 0; i < kpm->act_def->frm_4.action_def_format_1.meas_info_lst_len; i++)
      printf("Parameter to report: %s \n", kpm->act_def->frm_4.action_def_format_1.meas_info_lst[i].meas_type.name.buf);

    kpm->ind.hdr = fill_kpm_ind_hdr_sta(); 
    // 7.8 Supported RIC Styles and E2SM IE Formats
    // Format 4 corresponds to indication message 3
    kpm->ind.msg.type = FORMAT_3_INDICATION_MESSAGE;
    kpm->ind.msg.frm_3 = fill_kpm_ind_msg_frm_3_sta(kpm->act_def->frm_4.action_def_format_1);
  } else {
     kpm->ind.hdr = fill_rnd_kpm_ind_hdr();
     kpm->ind.msg = fill_rnd_kpm_ind_msg();
  }
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

