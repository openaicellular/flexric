#include "msg_handle_amr_ag.h"
#include "msg_dec_amr_ag.h"
#include "ep_amr.h"
#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../../util/alg_ds/alg/murmur_hash_32.c"
#include "../../util/e.h"


#include <assert.h>
#include <string.h>
#include <stdio.h>


sm_ag_if_ans_t write_ctrl_rc_sm_amr(void const* data)
{
  assert(data != NULL);
  sm_ag_if_ans_t ans = {0};

  return ans;
}

void init_rc_sm_amr(void)
{

}

void free_rc_sm_amr(void)
{

}

void read_rc_setup_sm_amr(void* data)
{
  assert(data != NULL);

}

sm_ag_if_ans_t write_subs_rc_sm_amr(void const* data)
{
  assert(data != NULL);
  sm_ag_if_ans_t ans = {0};

  return ans;
}



void msg_handle_amr_ag(e2_agent_amr_t const* ag, msg_amr_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);

  if(msg->type == MSG_READY_AMR_E){
    int const msg_id = ((e2_agent_amr_t*)ag)->msg_id++; 
    send_config_get(&ag->ep, msg_id); 
  } else
  	assert(0 != 0 && "Not implemented");
}

void send_config_get(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[64] = {0}; 
  size_t sz = snprintf(msg, 64, "{\"message\": \"config_get\", \"message_id\": %d }", msg_id );
  assert(sz < 64);

  printf("Sending message \n");
  send_ep_amr(ep, (uint8_t*)msg, sz);
}

void send_msg_stats(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  uint8_t msg[64] = {0}; 
  size_t sz = snprintf((char*)msg, 64, "{\"message\": \"stats\", \"message_id\": %d }", msg_id );
  assert(sz < 64);

  send_ep_amr(ep, msg, sz);
}

void send_msg_ue_get(ep_amr_t const* ep, int msg_id)
{
  assert(ep != NULL);
  assert(msg_id > -1);

  char msg[64] = {0}; 
  size_t sz = snprintf(msg, 64, "{\"message\": \"ue_get\", \"message_id\": %d }", msg_id );
  assert(sz < 64);

  send_ep_amr(ep, (uint8_t*)msg, sz);
}


















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
  assert(0 != 0 && "Debug point");
  assert(data != NULL);
}



bool read_rc_sm_amr(void* data)
{
  assert(0 != 0 && "Debug point");
  assert(data != NULL);
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

