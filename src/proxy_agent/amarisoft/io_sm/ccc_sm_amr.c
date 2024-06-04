#include "ccc_sm_amr.h"

#include "../../../agent/e2_agent_api.h"
#include "../../../sm/ccc_sm/ccc_sm_id.h"
#include "../../../util/time_now_us.h"
#include "../../../util/alg_ds/alg/defer.h"
#include "../io_ran/ccc_msgs_amr.h"

#include "../../../util/time_now_us.h"
#include "../../../util/e.h"
#include "../../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../../../util/alg_ds/alg/murmur_hash_32.h"

#include "../e2_agent_amr_api.h"

#include <stdio.h>

typedef enum{
  VALUE_READ = 0,
  VALUE_WRITE = 1,
} val_e;

static
assoc_ht_open_t ht;

/////////////////////////////
// Key value write control rc
/////////////////////////////

typedef struct {
  bool is_accepted;
  union {
    ctrl_out_conf_failed_t failed;
    ctrl_out_conf_accepted_t accepted;
  };
} wr_val_of_attribute_t;

typedef struct {
  val_e type;
  union {
    wr_val_of_attribute_t write;
    ind_msg_ran_conf_t read;
  };
} ans_val_of_attribute_t;

typedef ans_val_of_attribute_t (*fp_ccc_val_attr)(void* const src, val_e type, cell_global_id_t* const cell_global_id);

typedef struct{
  const char* key;
  fp_ccc_val_attr value;
} kv_val_of_attribute_t;

static
e2sm_plmn_t fill_rnd_plmn_id(){
  e2sm_plmn_t dst = (e2sm_plmn_t) {
      .mcc = (rand()%900) + 100,
      .mnc = (rand()%90) + 10,
      .mnc_digit_len = 2
  };

  return dst;
}

static
cell_global_id_t fill_rnd_cell_global_id()
{
  cell_global_id_t dst = {0};

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_RAT_TYPE;
  dst.nr_cgi.plmn_id = fill_rnd_plmn_id();
  dst.nr_cgi.nr_cell_id = rand()% (1UL << 36);

  return dst;
}


static
uint64_t get_cell_local_id(uint64_t nr_cgi){
  // Extract 36 bits from input
  uint64_t res = nr_cgi & ((1UL << 36) - 1);
  // nb_id default number of bits is 28 bits
  // Local UE id is 36 - 28 = 8 left most bits
  // 0xFF: 1111 1111
  return res & 0xFF;
}

/////////////////////////////
//  START read ccc
/////////////////////////////

static
ind_msg_ran_conf_t read_o_gnb_du(act_def_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
ind_msg_ran_conf_t read_o_gnb_cu_cp(act_def_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
ind_msg_ran_conf_t read_o_gnb_cu_up(act_def_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
ind_msg_ran_conf_t read_o_rrm_policy_ratio(act_def_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
ind_msg_ran_conf_t read_nr_cell_cu(act_def_ran_conf_t* const src){
  assert(src != NULL);
  ind_msg_ran_conf_t dst = {0};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);
  dst.change_type = rand()%END_CHANGE_TYPE;
  dst.vals_attributes = calloc(1, sizeof(values_of_attributes_t));
  assert(dst.vals_attributes != NULL);
  // TODO: Fill with calling config_get
//  *dst.vals_attributes = fill_values_of_attributes(VALUES_OF_ATTRIBUTES_O_NRCellCU);
//  if (rand()%2 == 1){
//    dst.old_vals_attributes = calloc(1, sizeof(values_of_attributes_t));
//    assert(dst.old_vals_attributes != NULL);
//    *dst.old_vals_attributes = fill_values_of_attributes(VALUES_OF_ATTRIBUTES_O_NRCellCU);
//  }

  return dst;
}
static
ind_msg_ran_conf_t read_nr_cell_du(act_def_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
ind_msg_ran_conf_t read_ces_management_func(act_def_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
ind_msg_ran_conf_t read_o_bwp(act_def_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

/////////////////////////////
//  END write control ccc
/////////////////////////////

/////////////////////////////
//  START write control ccc
/////////////////////////////

static
wr_val_of_attribute_t write_ctrl_o_gnb_du(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
wr_val_of_attribute_t write_ctrl_o_gnb_cu_cp(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute_t write_ctrl_o_gnb_cu_up(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute_t write_ctrl_o_rrm_policy_ratio(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute_t write_ctrl_nr_cell_cu(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
wr_val_of_attribute_t write_ctrl_nr_cell_du(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
wr_val_of_attribute_t write_ctrl_ces_management_func(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
wr_val_of_attribute_t write_ctrl_o_bwp(ctrl_msg_ran_conf_t* const src, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  assert(src->old_vals_attributes->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_BWP);
  assert(src->vals_attributes->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_BWP);

  wr_val_of_attribute_t ans = {0};

  // Wait for the answer
  ccc_msgs_amr_t msg = {0};
  defer({free_ccc_msgs_amr(&msg);});
  switch (src->vals_attributes->e2sm_ccc_o_bwp.bwp_context) {
    case UL_BWP_CONTEXT:
      config_set_rb_ctrl_ul_ccc_sm_api(get_cell_local_id(cell_global_id->nr_cgi.nr_cell_id),
                                       src->vals_attributes->e2sm_ccc_o_bwp.start_rb,
                                       src->vals_attributes->e2sm_ccc_o_bwp.number_of_rbs,
                                       &msg);
      break;
    case DL_BWP_CONTEXT:
      config_set_rb_ctrl_dl_ccc_sm_api(get_cell_local_id(cell_global_id->nr_cgi.nr_cell_id),
                                       src->vals_attributes->e2sm_ccc_o_bwp.start_rb,
                                       src->vals_attributes->e2sm_ccc_o_bwp.number_of_rbs,
                                       &msg);
      break;
    default:
      assert(0 != 0 && "Not support current bwp context");
  }

  if(msg.cfg_set.error == NULL){
    ans.is_accepted = true;
    ans.accepted.ran_conf_name = cp_str_to_ba("O-BWP");

    // Mandatory
    ans.accepted.old_atr_val = calloc(1, sizeof(values_of_attributes_t));
    assert(ans.accepted.old_atr_val != NULL);
    *ans.accepted.old_atr_val = cp_values_of_attributes(src->old_vals_attributes);

    // Mandatory
    ans.accepted.cur_atr_val = calloc(1, sizeof(values_of_attributes_t));
    assert(ans.accepted.cur_atr_val != NULL);
    *ans.accepted.cur_atr_val = cp_values_of_attributes(src->vals_attributes);
  } else {
    printf("[PROXY-AGENT]: Config set rb control error: %s\n", msg.cfg_set.error);
    ans.is_accepted = false;
    ans.failed.ran_conf_name = cp_str_to_ba("O-BWP");

    // Mandatory
    ans.failed.old_atr_val = calloc(1, sizeof(values_of_attributes_t));
    assert(ans.failed.old_atr_val != NULL);
    *ans.failed.old_atr_val = cp_values_of_attributes(src->old_vals_attributes);

    // Mandatory
    ans.failed.req_atr_val = calloc(1, sizeof(values_of_attributes_t));
    assert(ans.failed.req_atr_val != NULL);
    *ans.failed.req_atr_val = cp_values_of_attributes(src->vals_attributes);
    ans.failed.cause = CAUSE_UNSPECIFIED;
  }

  return ans;
}

/////////////////////////////
//  END write control ccc
/////////////////////////////

static
ans_val_of_attribute_t o_gnb_du_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  (void) cell_global_id;
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_o_gnb_du((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_o_gnb_du((ctrl_msg_ran_conf_t*) src);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}

static
ans_val_of_attribute_t o_gnb_cu_cp_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  (void) cell_global_id;
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_o_gnb_cu_cp((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_o_gnb_cu_cp((ctrl_msg_ran_conf_t*) src);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}

static
ans_val_of_attribute_t o_gnb_cu_up_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  (void) cell_global_id;
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_o_gnb_cu_up((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_o_gnb_cu_up((ctrl_msg_ran_conf_t*) src);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}

static
ans_val_of_attribute_t o_rrm_policy_ratio_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  (void) cell_global_id;
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_o_rrm_policy_ratio((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_o_rrm_policy_ratio((ctrl_msg_ran_conf_t*) src);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}

static
ans_val_of_attribute_t nr_cell_cu_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  (void) cell_global_id;
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_nr_cell_cu((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_nr_cell_cu((ctrl_msg_ran_conf_t*) src);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}

static
ans_val_of_attribute_t nr_cell_du_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  (void) cell_global_id;
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_nr_cell_du((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_nr_cell_du((ctrl_msg_ran_conf_t*) src);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}
static
ans_val_of_attribute_t ces_management_func_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id){
  assert(src != NULL);
  (void) cell_global_id;
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_ces_management_func((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_ces_management_func((ctrl_msg_ran_conf_t*) src);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}

static
ans_val_of_attribute_t o_bwp_wrapper(void* const src, val_e type, cell_global_id_t* const cell_global_id) {
  assert(src != NULL);
  ans_val_of_attribute_t dst = {.type = type};
  switch (type) {
    case VALUE_READ:
      dst.read = read_o_bwp((act_def_ran_conf_t*) src);
      break;
    case VALUE_WRITE:
      dst.write =  write_ctrl_o_bwp((ctrl_msg_ran_conf_t*) src, cell_global_id);
      break;
    default:
      assert(0 != 0 && "Unknown type");
  }
  return dst;
}

// ORAN.WG3.E2SM-CCC-R003-v03.00
// 8.2.2 and 8.2.1
static
const kv_val_of_attribute_t lst_ran_conf_name[] = {
    (kv_val_of_attribute_t){.key = "O-GNBDUFunction", .value = o_gnb_du_wrapper},
    (kv_val_of_attribute_t){.key = "O-GNBCUCPFunction", .value = o_gnb_cu_cp_wrapper},
    (kv_val_of_attribute_t){.key = "O-GNBCUUPFunction", .value =  o_gnb_cu_up_wrapper},
    (kv_val_of_attribute_t){.key = "O-RRMPolicyRatio", .value =  o_rrm_policy_ratio_wrapper},
    (kv_val_of_attribute_t){.key = "O-NRCellCU", .value =  nr_cell_cu_wrapper},
    (kv_val_of_attribute_t){.key = "O-NRCellDU", .value =  nr_cell_du_wrapper},
    (kv_val_of_attribute_t){.key = "O-BWP", .value =  o_bwp_wrapper},
    (kv_val_of_attribute_t){.key = "O-CESManagementFunction", .value = ces_management_func_wrapper},
};

static
uint32_t hash_func(const void* key_v){
  // Def need a null termination
  char* key = *(char**)(key_v);
  static const uint32_t seed = 42;
  return murmur3_32((uint8_t*)key, strlen(key) + 1, seed);
}

static
bool cmp_str(const void* a, const void* b){
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
void free_wr_ctrl_val_atr(void)
{
  assoc_free(&ht);
}

static
void init_wr_ctrl_val_atr(void)
{
  assoc_ht_open_init(&ht, sizeof(char*), cmp_str, free_str, hash_func);

  const size_t nelem = sizeof(lst_ran_conf_name) / sizeof(lst_ran_conf_name[0]);
  for(size_t i = 0; i < nelem; ++i){
    const size_t sz = strlen(lst_ran_conf_name[i].key);
    char* key = calloc(sz + 1, sizeof(char));
    memcpy(key, lst_ran_conf_name[i].key, sz);

    fp_ccc_val_attr* value = calloc(1, sizeof(fp_ccc_val_attr));
    assert(value != NULL && "Memory exhausted");
    *value = lst_ran_conf_name[i].value;
    assoc_insert(&ht, &key, sizeof(char*), value);
  }
  assert(assoc_size(&ht) == nelem);
}

/////////////////////////////
// End Key value write control rc
////////////////////////////len & 3/

void init_ccc_sm_amr(void)
{
  init_wr_ctrl_val_atr();
}

void free_ccc_sm_amr(void)
{
  free_wr_ctrl_val_atr();
}

bool node_cfg_read(ccc_rd_ind_data_t* const ccc_read){
  assert(ccc_read != NULL);
  assert(0 != 0 && "Not implemented");
}

bool cell_cfg_read(ccc_rd_ind_data_t* const ccc_read){
  assert(ccc_read != NULL);
  assert(ccc_read->act_def->format == FORMAT_2_E2SM_CCC_ACT_DEF);

  e2sm_ccc_act_def_frmt_2_t const* act_def = &ccc_read->act_def->frmt_2;

  ccc_read->ind.msg.format = FORMAT_2_E2SM_CCC_IND_MSG;
  ccc_read->ind.msg.frmt_2.sz_ind_msg_cell_report = act_def->sz_act_def_cell_report;
  ccc_read->ind.msg.frmt_2.ind_msg_cell_report = calloc(act_def->sz_act_def_cell_report, sizeof(ind_msg_cell_report_t));
  assert(ccc_read->ind.msg.frmt_2.ind_msg_cell_report != NULL);

  // [1 - 1024]
  assert(act_def->sz_act_def_cell_report > 0 && act_def->sz_act_def_cell_report < 1025);
  for (size_t i = 0; i < act_def->sz_act_def_cell_report; i++){
    // [1 - 1024]
    assert(act_def->act_def_cell_report[i].sz_act_def_ran_conf > 0);
    assert(act_def->act_def_cell_report[i].sz_act_def_ran_conf < 1025);
    ccc_read->ind.msg.frmt_2.ind_msg_cell_report[i].cell_global_id = fill_rnd_cell_global_id();
    ccc_read->ind.msg.frmt_2.ind_msg_cell_report[i].sz_ind_msg_ran_conf = act_def->act_def_cell_report[i].sz_act_def_ran_conf;
    ccc_read->ind.msg.frmt_2.ind_msg_cell_report[i].ind_msg_ran_conf = calloc(act_def->act_def_cell_report[i].sz_act_def_ran_conf, sizeof(ind_msg_ran_conf_t));
    assert(ccc_read->ind.msg.frmt_2.ind_msg_cell_report[i].ind_msg_ran_conf != NULL);

    for (size_t j = 0; j < act_def->act_def_cell_report[i].sz_act_def_ran_conf; j++) {
      act_def_ran_conf_t* const cur_ran_conf = &act_def->act_def_cell_report[i].act_def_ran_conf[j];
      void* key = copy_ba_to_str(&cur_ran_conf->ran_conf_name);
      defer({free(key);});

      // Get the value pointer from the key i.e., the function to be called
      // for the key that represents a ran configuration name e.g., O-BWP
      void* value = assoc_ht_open_value(&ht, &key);
      assert(value != NULL && "Not registered name used as key");
      // Get the control outcome (e.g., O-BWP) for the control output
      ans_val_of_attribute_t rd_ans = (*(fp_ccc_val_attr*)value)(cur_ran_conf, VALUE_READ, NULL);
      assert(rd_ans.type == VALUE_READ);

      ccc_read->ind.msg.frmt_2.ind_msg_cell_report[i].ind_msg_ran_conf[j] = rd_ans.read;
    }
  }

  ccc_read->ind.hdr.format = FORMAT_1_E2SM_CCC_IND_HDR;
  ccc_read->ind.hdr.frmt_1.ind_reason = IND_REASON_PERIODIC;
  uint64_t time = time_now_us();
  sprintf(ccc_read->ind.hdr.frmt_1.event_time, "%ld", time);

  return true;
}

typedef bool (*fp_rd_ccc)(ccc_rd_ind_data_t* const ccc_read);

fp_rd_ccc arr_read[] =
    {
        node_cfg_read,
        cell_cfg_read
    };

bool read_ccc_sm_amr(void* data)
{
  assert(data != NULL);

  ccc_rd_ind_data_t* const ccc_ind = (ccc_rd_ind_data_t* const)data;
  // 0: Node configuration and control
  // 1: Cell configuration and control
  return arr_read[ccc_ind->act_def->format](ccc_ind);
}

void read_ccc_setup_sm_amr(void* data)
{
  assert(data != NULL);
  ccc_e2_setup_t* rc = (ccc_e2_setup_t*)data;

  ran_function_name_t dst = {0};

  // RAN Function Short Name
  // Mandatory
  // PrintableString [1-150]
  dst.name.buf = calloc(strlen(SM_CCC_SHORT_NAME) + 1, sizeof(uint8_t));
  memcpy(dst.name.buf, SM_CCC_SHORT_NAME, strlen(SM_CCC_SHORT_NAME));
  dst.name.len = strlen(SM_CCC_SHORT_NAME);

  // RAN Function Service Model OID
  // Mandatory
  // PrintableString [1-1000]

  //iso(1) identified-organization(3)
  //dod(6) internet(1) private(4)
  //enterprise(1) 53148 e2(1)
  // version1 (1) e2sm(2) e2sm-CCC-IEs (4)
  dst.oid.buf = calloc(strlen(SM_CCC_OID) + 1, sizeof(uint8_t));
  memcpy(dst.oid.buf, SM_CCC_OID, strlen(SM_CCC_OID));
  dst.oid.len = strlen(SM_CCC_OID);

  // RAN Function Description
  // Mandatory
  // PrintableString [1- 150]
  //RAN function RC “RAN Control” performs the following
  //functionalities:
  //- Exposure of RAN control and UE context related
  //information.
  //- Modification and initiation of RAN control related call
  //processes and messages
  //- Execution of policies that may result in change of
  //RAN control behavior

  dst.description.buf = calloc(strlen(SM_CCC_DESCRIPTION) + 1, sizeof(uint8_t));
  memcpy(dst.description.buf, SM_CCC_DESCRIPTION, strlen(SM_CCC_DESCRIPTION));
  dst.description.len = strlen(SM_CCC_DESCRIPTION);

  // RAN Function Instance
  // Optional
  // INTEGER
//    long* instance;	/* OPTIONAL: it is suggested to be used when E2 Node declares
//                                multiple RAN Function ID supporting the same  E2SM specification   ask Mikel */
  rc->ran_func_def.name = dst;
}

void free_ctrl_out_conf_failed_wrapper(void* src){
  assert(src != NULL);
  free_ctrl_out_conf_failed(src);
}

void free_ctrl_out_conf_accepted_wrapper(void* src){
  assert(src != NULL);
  free_ctrl_out_conf_accepted(src);
}

void get_ctrl_out_conf_failed(ctrl_out_conf_failed_t** dst, seq_arr_t* const src){
  assert(src != NULL);
  size_t len = seq_size(src);

  if (len > 0){
    size_t index = 0;
    *dst = calloc(len, sizeof(ctrl_out_conf_failed_t));
    assert(*dst != NULL);
    void* start_it = seq_front(src);
    void* end_it = seq_end(src);
    while(start_it != end_it){
      *dst[index] = cp_ctrl_out_conf_failed((ctrl_out_conf_failed_t *)start_it);
      start_it = seq_next(src, start_it);
      index++;
    }
  }
}

void get_ctrl_out_conf_accepted(ctrl_out_conf_accepted_t** dst, seq_arr_t* const src){
  assert(src != NULL);
  size_t len = seq_size(src);

  if (len > 0){
    size_t index = 0;
    *dst = calloc(len, sizeof(ctrl_out_conf_accepted_t));
    assert(*dst != NULL);
    void* start_it = seq_front(src);
    void* end_it = seq_end(src);
    while(start_it != end_it){
      *dst[index] = cp_ctrl_out_conf_accepted((ctrl_out_conf_accepted_t*)start_it);
      start_it = seq_next(src, start_it);
      index++;
    }
  }
}

static
sm_ag_if_ans_t node_cfg_ctrl(ccc_ctrl_req_data_t* const ccc_ctrl)
{
  assert(ccc_ctrl != NULL);
  assert(0 != 0 && "Not implemented");
}

static
sm_ag_if_ans_t cell_cfg_ctrl(ccc_ctrl_req_data_t* const ctrl)
{
  assert(ctrl != NULL);

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = CCC_V3_0_AGENT_IF_CTRL_ANS_V0;
  ans.ctrl_out.ccc.format = FORMAT_2_E2SM_CCC_CTRL_OUT;

  assert(ctrl->msg.frmt_2.sz_ctrl_msg_cell > 0 && ctrl->msg.frmt_2.sz_ctrl_msg_cell < 65536);
  e2sm_ccc_ctrl_out_frmt_2_t* frmt_2 = &ans.ctrl_out.ccc.frmt_2;
  frmt_2->sz_ctrl_out_cell = ctrl->msg.frmt_2.sz_ctrl_msg_cell;
  frmt_2->ctrl_out_cell = calloc(frmt_2->sz_ctrl_out_cell, sizeof(ctrl_out_cell_t));
  assert(frmt_2->ctrl_out_cell != NULL);

  for (size_t i = 0; i < ctrl->msg.frmt_2.sz_ctrl_msg_cell; i++){
    ctrl_msg_cell_t* const cur_cell = &ctrl->msg.frmt_2.ctrl_msg_cell[i];
    printf("Cell configuration and control level with cell id: %ld \n", (uint64_t)cur_cell->cell_global_id.nr_cgi.nr_cell_id);
    frmt_2->ctrl_out_cell[i].cell_global_id = cp_cell_global_id(&cur_cell->cell_global_id);
    // Initialize dynamic array
    seq_arr_t accepted_arr = {0};
    seq_arr_t failed_arr = {0};
    seq_init(&accepted_arr, sizeof(ctrl_out_conf_accepted_t));
    seq_init(&failed_arr, sizeof(ctrl_out_conf_failed_t));

    for (size_t z = 0; z < cur_cell->sz_ctrl_msg_ran_conf; z++){
      ctrl_msg_ran_conf_t* const cur_ran_conf = &cur_cell->ctrl_msg_ran_conf[z];
      void* key = copy_ba_to_str(&cur_ran_conf->ran_conf_name);
      defer({free(key);});

      // Get the value pointer from the key i.e., the function to be called
      // for the key that represents a ran configuration name e.g., O-BWP
      void* value = assoc_ht_open_value(&ht, &key);
      assert(value != NULL && "Not registered name used as key");
      // Get the control outcome (e.g., O-BWP) for the control output
      ans_val_of_attribute_t wr_ans = (*(fp_ccc_val_attr*)value)(cur_ran_conf, VALUE_WRITE, &cur_cell->cell_global_id);
      assert(wr_ans.type == VALUE_WRITE);

      if (wr_ans.write.is_accepted){
        seq_push_back(&accepted_arr, &wr_ans.write.accepted, sizeof(ctrl_out_conf_accepted_t));
      }else{
        seq_push_back(&failed_arr, &wr_ans.write.failed, sizeof(ctrl_out_conf_failed_t));
      }
    }

    frmt_2->ctrl_out_cell[i].sz_ctrl_out_conf_accepted = seq_size(&accepted_arr);
    get_ctrl_out_conf_accepted(&frmt_2->ctrl_out_cell[i].ctrl_out_conf_accepted, &accepted_arr);
    frmt_2->ctrl_out_cell[i].sz_ctrl_out_conf_failed = seq_size(&failed_arr);
    get_ctrl_out_conf_failed(&frmt_2->ctrl_out_cell[i].ctrl_out_conf_failed, &failed_arr);

    seq_free(&accepted_arr, free_ctrl_out_conf_accepted_wrapper);
    seq_free(&failed_arr, free_ctrl_out_conf_failed_wrapper);
  }

  return ans;
}

typedef sm_ag_if_ans_t (*fp_wc_ccc)(ccc_ctrl_req_data_t* const ccc_ctrl);

static
fp_wc_ccc arr_write_ctrl[] =
    {
      node_cfg_ctrl,
      cell_cfg_ctrl
    };

sm_ag_if_ans_t write_ctrl_ccc_sm_amr(void const* data)
{
  assert(data != NULL);
  ccc_ctrl_req_data_t* const ctrl = (ccc_ctrl_req_data_t* const)data;

  assert(ctrl->hdr.format == FORMAT_1_E2SM_CCC_CTRL_HDR);
  // 0: Node configuration and control
  // 1: Cell configuration and control
  return arr_write_ctrl[ctrl->hdr.frmt_1.ric_style_type - 1](ctrl);
}

sm_ag_if_ans_t write_subs_ccc_sm_amr(void const* src)
{
  assert(0!= 0 && "Not implemented");
  assert(src != NULL);
}
