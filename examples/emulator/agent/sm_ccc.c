#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "../../../test/rnd/fill_rnd_data_ccc.h"
#include "../../../src/util/alg_ds/alg/murmur_hash_32.h"
#include "../../../src/util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../../../src/util/alg_ds/alg/defer.h"
#include "../../../src/util/time_now_us.h"
#include "sm_ccc.h"

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
} wr_val_of_attribute;

typedef wr_val_of_attribute (*fp_rc_wr_ctrl)(ctrl_msg_ran_conf_t* const src);

typedef struct{
  const char* key;
  fp_rc_wr_ctrl value;
} kv_val_of_attribute_t;

static
wr_val_of_attribute write_ctrl_o_gnb_du(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
wr_val_of_attribute write_ctrl_o_gnb_cu_cp(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute write_ctrl_o_gnb_cu_up(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute write_ctrl_o_rrm_policy_ratio(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute write_ctrl_nr_cell_cu(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute write_ctrl_nr_cell_du(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}
static
wr_val_of_attribute write_ctrl_ces_management_func(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(0 != 0 && "Not implemented");
}

static
char* bwp_context_to_str(bwp_context_e src){
  switch (src) {
    case UL_BWP_CONTEXT:
      return "UL_BWP_CONTEXT";
    case DL_BWP_CONTEXT:
      return "DL_BWP_CONTEXT";
    case SUL_BWP_CONTEXT:
      return "SUL_BWP_CONTEXT";
    default:
      assert(0 != 0 && "Unknown BWP context");
  }
}

static
wr_val_of_attribute write_ctrl_o_bwp(ctrl_msg_ran_conf_t* const src){
  assert(src != NULL);
  assert(src->old_vals_attributes.values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_BWP);
  assert(src->vals_attributes.values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_BWP);

  wr_val_of_attribute ans = {0};

  printf("O-BWP: %s Start rb %d, number of rbs %d \n",
         bwp_context_to_str(src->vals_attributes.e2sm_ccc_o_bwp.bwp_context),
         src->vals_attributes.e2sm_ccc_o_bwp.start_rb,
         src->vals_attributes.e2sm_ccc_o_bwp.number_of_rbs);

  ans.is_accepted = true;
  ans.accepted.ran_conf_name = cp_str_to_ba("O-BWP");
  ans.accepted.old_atr_val = cp_values_of_attributes(&src->vals_attributes);
  ans.accepted.cur_atr_val = cp_values_of_attributes(&src->old_vals_attributes);

  return ans;
}

// ORAN.WG3.E2SM-CCC-R003-v03.00
// 8.2.2 and 8.2.1
static
const kv_val_of_attribute_t lst_ran_conf_name[] = {
    (kv_val_of_attribute_t){.key = "O-GNBDUFunction", .value = write_ctrl_o_gnb_du},
    (kv_val_of_attribute_t){.key = "O-GNBCUCPFunction", .value = write_ctrl_o_gnb_cu_cp},
    (kv_val_of_attribute_t){.key = "O-GNBCUUPFunction", .value =  write_ctrl_o_gnb_cu_up},
    (kv_val_of_attribute_t){.key = "O-RRMPolicyRatio", .value =  write_ctrl_o_rrm_policy_ratio},
    (kv_val_of_attribute_t){.key = "O-NRCellCU", .value =  write_ctrl_nr_cell_cu},
    (kv_val_of_attribute_t){.key = "O-NRCellDU", .value =  write_ctrl_nr_cell_du},
    (kv_val_of_attribute_t){.key = "O-BWP", .value =  write_ctrl_o_bwp},
    (kv_val_of_attribute_t){.key = "O-CESManagementFunction", .value = write_ctrl_ces_management_func},
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

    fp_rc_wr_ctrl* value = calloc(1, sizeof(fp_rc_wr_ctrl));
    assert(value != NULL && "Memory exhausted");
    *value = lst_ran_conf_name[i].value;
    assoc_insert(&ht, &key, sizeof(char*), value);
  }
  assert(assoc_size(&ht) == nelem);
}

/////////////////////////////
// End Key value write control rc
////////////////////////////len & 3/

void init_ccc_sm(void)
{
  init_wr_ctrl_val_atr();
}

void free_ccc_sm(void)
{
  free_wr_ctrl_val_atr();
}

bool read_ccc_sm(void* data)
{
  assert(data != NULL);
  assert(0!=0 && "Not implemented");
  return true;
}

void read_ccc_setup_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0);
  ccc_e2_setup_t* rc = (ccc_e2_setup_t*)data;
  rc->ran_func_def = fill_ccc_ran_func_def();
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
      start_it = seq_next(src, start_it);
      *dst[index] = cp_ctrl_out_conf_failed((ctrl_out_conf_failed_t *)start_it);
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
      start_it = seq_next(src, start_it);
      *dst[index] = cp_ctrl_out_conf_accepted((ctrl_out_conf_accepted_t*)start_it);
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
      wr_val_of_attribute wr_ans = (*(fp_rc_wr_ctrl *)value)(cur_ran_conf);

      if (wr_ans.is_accepted){
        seq_push_back(&accepted_arr, &wr_ans.accepted, sizeof(ctrl_out_conf_accepted_t));
      }else{
        seq_push_back(&failed_arr, &wr_ans.failed, sizeof(ctrl_out_conf_failed_t));
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

fp_wc_ccc arr_write_ctrl[] =
    {
      node_cfg_ctrl,
      cell_cfg_ctrl
    };

sm_ag_if_ans_t write_ctrl_ccc_sm(void const* data)
{
  assert(data != NULL);
  ccc_ctrl_req_data_t* const ctrl = (ccc_ctrl_req_data_t* const)data;

  assert(ctrl->hdr.format == FORMAT_1_E2SM_CCC_CTRL_HDR);
  // 0: Node configuration and control
  // 1: Cell configuration and control
  return arr_write_ctrl[ctrl->hdr.frmt_1.ric_style_type - 1](ctrl);
}

sm_ag_if_ans_t write_subs_ccc_sm(void const* src)
{
  assert(0!= 0 && "Not implemented");
  assert(src != NULL);
}
