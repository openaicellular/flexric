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
#include "util/byte_array.h"
#include "util/alg_ds/alg/defer.h"

#include "e2_sm_agent.h"
#include "ran_if.h"
#include "proxy_agent.h"
#include "ran_msg_hdlr.h"
#include "ringbuffer.h"
#include "notif_e2_ran.h"

static
gnb_e2sm_t fill_gnb_data(const amarisoft_ue_stats_t* ue, global_e2_node_id_t id)
{
  gnb_e2sm_t gnb = {0};

  gnb.amf_ue_ngap_id = ue->amf_ue_id;

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
ue_id_e2sm_t fill_rnd_ue_id_data()
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
meas_record_lst_t fill_meas_value(meas_type_t meas_info_type, const amarisoft_ue_stats_t* ue_stats)
{
  meas_record_lst_t meas_record = {0};
  // Get Meas Info Name from Action Definition
  char* meas_info_name_str = copy_ba_to_str(&meas_info_type.name);
  defer({free(meas_info_name_str); } );

  // Get value based on Meas Info Name
  if (!strcmp(meas_info_name_str, "DRB.UEThpDl")) {
    meas_record.value = REAL_MEAS_VALUE;
    meas_record.real_val = ue_stats->cells[0].dl_bitrate/1000; //Kbps // TODO:
  } else if (!strcmp(meas_info_name_str, "DRB.UEThpUl")) {
    meas_record.value = REAL_MEAS_VALUE;
    meas_record.real_val = ue_stats->cells[0].ul_bitrate/1000; // Kbps // TODO:
  } else if (!strcmp(meas_info_name_str, "DRB.PdcpSduVolumeDL")) {
    meas_record.value = INTEGER_MEAS_VALUE;
    meas_record.real_val = ue_stats->cells[0].dl_tx*8; // bits// TODO:
  } else if (!strcmp(meas_info_name_str, "DRB.PdcpSduVolumeUL")) {
    meas_record.value = INTEGER_MEAS_VALUE;
    meas_record.real_val = ue_stats->cells[0].ul_tx*8; // bits TODO:
  } else {
    // TODO: need metrics from AMR and need mapping name from 3GPP
    printf("not implement value for measurement name %s\n", meas_info_name_str);
    meas_record.value = REAL_MEAS_VALUE;
    meas_record.real_val = 1234;
  }
  return meas_record;
}

static
kpm_ind_msg_format_1_t fill_kpm_ind_msg_frm_1_in_monolithic(const amarisoft_ue_stats_t* ue, const kpm_act_def_format_1_t* act_def_fr1)
{
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
          meas_data->meas_record_lst[j] = fill_meas_value(meas_info_type, ue);
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

typedef struct {
    size_t num_ues;
    amarisoft_ue_stats_t ue_list[AMARISOFT_MAX_UE_NUM];
} matched_ues_t;

static
kpm_ind_msg_format_3_t fill_kpm_ind_msg_frm_3_in_monolithic(const matched_ues_t matched_ues, const kpm_act_def_format_1_t * act_def_fr_1, const ran_ind_t* ws_ind)
{
  assert(act_def_fr_1 != NULL);


  kpm_ind_msg_format_3_t msg_frm_3 = {0};

  // Fill UE Measurement Reports
  msg_frm_3.ue_meas_report_lst_len = matched_ues.num_ues;
  msg_frm_3.meas_report_per_ue = calloc(msg_frm_3.ue_meas_report_lst_len, sizeof(meas_report_per_ue_t));
  assert(msg_frm_3.meas_report_per_ue != NULL && "Memory exhausted");
  for (size_t i = 0; i<msg_frm_3.ue_meas_report_lst_len; i++)
  {
    // Fill UE ID data
    msg_frm_3.meas_report_per_ue[i].ue_meas_report_lst.type = GNB_UE_ID_E2SM;
    msg_frm_3.meas_report_per_ue[i].ue_meas_report_lst.gnb = fill_gnb_data(&matched_ues.ue_list[i], ws_ind->global_e2_node_id);

    // Fill UE related info
    msg_frm_3.meas_report_per_ue[i].ind_msg_format_1 = fill_kpm_ind_msg_frm_1_in_monolithic(&matched_ues.ue_list[i], act_def_fr_1);
  }

  return msg_frm_3;
}

static
matched_ues_t filter_ues_by_s_nssai_in_du_or_monolithic(test_cond_e const condition, int64_t const value, ran_ind_t* ws_ind)
{
  matched_ues_t matched_ues = {.num_ues = 0};

  /* IMPORTANT: in the case of gNB-DU, it is not possible to filter UEs by S-NSSAI as the ngap context is stored in gNB-CU
                instead, we take all connected UEs*/

  // Take MAC info
  for (size_t i = 0; i < ws_ind->len_ue_stats; i++)
  {
    // Filter connected UEs by S-NSSAI test condition to get list of matched UEs
    // note: not possible to filter
    switch (condition)
    {
      case EQUAL_TEST_COND:
      {
        if (ws_ind->ue_stats[i].len_qos_flow > 0) {
          if (ws_ind->ue_stats[i].qos_flows[0].sst == value)
            printf("condition map, find the same sst %ld\n", value);
        }
        matched_ues.ue_list[matched_ues.num_ues] = ws_ind->ue_stats[i];
        matched_ues.num_ues++;
        break;
      }

      default:
        assert(false && "Condition not yet implemented");
    }
  }

  return matched_ues;
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
#if defined KPM_V2
  hdr_frm_1.collectStartTime = t/1000000; // seconds
#elif defined KPM_V3
  hdr_frm_1.collectStartTime = t; // microseconds
#else
  static_assert(0!=0, "Undefined KPM SM Version");
#endif

  hdr_frm_1.fileformat_version = NULL;

  // Check E2 Node NG-RAN Type
  if (E2AP_NODE_IS_MONOLITHIC(ws_ind->global_e2_node_id.type))
  {
    hdr_frm_1.sender_name = calloc(1, sizeof(byte_array_t));
    hdr_frm_1.sender_name->buf = calloc(strlen("AMR-MONO") + 1, sizeof(char));
    memcpy(hdr_frm_1.sender_name->buf, "AMR-MONO", strlen("AMR-MONO"));
    hdr_frm_1.sender_name->len = strlen("AMR-MONO") + 1;

    hdr_frm_1.sender_type = calloc(1, sizeof(byte_array_t));
    hdr_frm_1.sender_type->buf = calloc(strlen("MONO") + 1, sizeof(char));
    memcpy(hdr_frm_1.sender_type->buf, "MONO", strlen("MONO"));
    hdr_frm_1.sender_type->len = strlen("MONO") + 1;
  } else {
    assert(0!=0 && "Unknown node type");
  }

  hdr_frm_1.vendor_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.vendor_name->buf = calloc(strlen("Amarisoft") + 1, sizeof(char));
  memcpy(hdr_frm_1.vendor_name->buf, "Amarisoft", strlen("Amarisoft"));
  hdr_frm_1.vendor_name->len = strlen("Amarisoft") + 1;

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

void read_kpm_sm(void* data)
{
  ran_ind_t ws_ind = get_ringbuffer_data();

  kpm_rd_ind_data_t* kpm = (kpm_rd_ind_data_t*)data;
  kpm_act_def_t const* act_def = kpm->act_def;
  assert(act_def!= NULL && "Cannot be NULL");
  switch (act_def->type) {
    case FORMAT_4_ACTION_DEFINITION: {
      kpm->ind.hdr = kpm_ind_hdr(&ws_ind);

      kpm->ind.msg.type = FORMAT_3_INDICATION_MESSAGE;
      // Filter the UE by the test condition criteria
      kpm_act_def_format_4_t const* frm_4 = &kpm->act_def->frm_4; // 8.2.1.2.4
      for (size_t i = 0; i < frm_4->matching_cond_lst_len; i++) {
        switch (frm_4->matching_cond_lst[i].test_info_lst.test_cond_type) {
          case S_NSSAI_TEST_COND_TYPE: {
            assert(frm_4->matching_cond_lst[i].test_info_lst.S_NSSAI == TRUE_TEST_COND_TYPE && "Must be true");
            assert(frm_4->matching_cond_lst[i].test_info_lst.test_cond != NULL && "Even though is optional..");
            assert(frm_4->matching_cond_lst[i].test_info_lst.int_value != NULL && "Even though is optional..");

            test_cond_e const test_cond = *frm_4->matching_cond_lst[i].test_info_lst.test_cond;
            int64_t const value = *frm_4->matching_cond_lst[i].test_info_lst.int_value;
            // Check E2 Node NG-RAN Type
            if (E2AP_NODE_IS_MONOLITHIC(ws_ind.global_e2_node_id.type)) {
              matched_ues_t matched_ues = filter_ues_by_s_nssai_in_du_or_monolithic(test_cond, value, &ws_ind);
              if (matched_ues.num_ues <= 0)
                goto rnd_data_label;
              kpm->ind.msg.frm_3 = fill_kpm_ind_msg_frm_3_in_monolithic(matched_ues, &frm_4->action_def_format_1, &ws_ind);
            } else {
              assert(false && "NG-RAN Type not implemented");
            }

            break;
          }

          default:
            assert(false && "Unknown Test condition");
        }
      }
      break;
    }
    default: {
      printf("Not supported action definition type %d, fill the dummy indication msg\n", act_def->type);
    rnd_data_label:
      kpm->ind.hdr = fill_rnd_kpm_ind_hdr();
      kpm->ind.msg.frm_3 = fill_rnd_kpm_ind_msg_frm_3();

      break;
    }
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

