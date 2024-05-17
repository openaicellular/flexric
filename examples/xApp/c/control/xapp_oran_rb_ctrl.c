#include "../../../../src/xApp/e42_xapp_api.h"
#include "../../../../src/sm/ccc_sm/ccc_sm_id.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static
uint32_t start_rb = 0;

static
uint32_t number_of_rbs = 0;

static
cell_global_id_t fill_rnd_cell_global_id()
{
  cell_global_id_t dst = {0};

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_RAT_TYPE;
  dst.nr_cgi.plmn_id = (e2sm_plmn_t){.mcc = 505, .mnc = 55, .mnc_digit_len = 2};
  dst.nr_cgi.nr_cell_id = 10001;

  return dst;
}

static
e2sm_ccc_o_bwp_t fill_e2sm_ccc_o_bwp() 
{
  e2sm_ccc_o_bwp_t dst = {0};

  dst.bwp_context = DL_BWP_CONTEXT;
  dst.start_rb = start_rb;
  dst.number_of_rbs = number_of_rbs;

  return dst;
}


// CONTROL Service Style 2: Cell level configuration and control
static
e2sm_ccc_ctrl_hdr_t gen_hdr(void)
{
  e2sm_ccc_ctrl_hdr_t dst = {.format = FORMAT_1_E2SM_CCC_CTRL_HDR}; 
  dst.frmt_1.ric_style_type = 2;

  return dst;
}

static
ctrl_msg_ran_conf_t fill_ctrl_msg_ran_conf()
{
  ctrl_msg_ran_conf_t dst = {0}; 

  dst.ran_conf_name = cp_str_to_ba("O-BWP");
  dst.vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_BWP;
  dst.vals_attributes.e2sm_ccc_o_bwp = fill_e2sm_ccc_o_bwp();
  // TODO: Add old value of attributes because its mandatory
  return dst;
}

static
ctrl_msg_cell_t fill_ctrl_msg_cell()
{
  ctrl_msg_cell_t dst = {0};

  dst.cell_global_id = fill_rnd_cell_global_id();
  dst.sz_ctrl_msg_ran_conf = 1;
  dst.ctrl_msg_ran_conf = calloc(dst.sz_ctrl_msg_ran_conf, sizeof(ctrl_msg_ran_conf_t));
  assert(dst.ctrl_msg_ran_conf!= NULL && "Memory exhausted");
  dst.ctrl_msg_ran_conf[0] = fill_ctrl_msg_ran_conf();

  return dst;
}

static
e2sm_ccc_ctrl_msg_t gen_msg(void)
{
  e2sm_ccc_ctrl_msg_t dst = {0}; 

  // Cell Level
  dst.format = FORMAT_2_E2SM_CCC_CTRL_MSG;
  dst.frmt_2.sz_ctrl_msg_cell = 1;
  dst.frmt_2.ctrl_msg_cell = calloc(dst.frmt_2.sz_ctrl_msg_cell, sizeof(ctrl_msg_cell_t));
  assert(dst.frmt_2.ctrl_msg_cell != NULL);
  dst.frmt_2.ctrl_msg_cell[0] = fill_ctrl_msg_cell();

  return dst;
}


int main(int argc, char *argv[])
{
  assert(argc > 2 && "Require rb start and number of rbs");
  fr_args_t args = init_fr_args(argc, argv);
  defer({ free_fr_args(&args); });
  for (int i = argc - 1; i > 0; i--){
    if(i == argc - 1)
      number_of_rbs = atoi(argv[i]);
    if(i == argc - 2)
      start_rb = atoi(argv[i]);
    if(i < argc - 2)
      break;
  }

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_xapp_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr_xapp(&nodes); });
  assert(nodes.len > 0);
  printf("Connected E2 nodes = %d\n", nodes.len);

  ////////////
  // START CCC
  ////////////

  // CCC Control
  // CONTROL Service Style 2: Cell level configuration and control
  // E2SM-CCC Control Header Format 1
  // E2SM-CCC Control Message Format 2

  ccc_ctrl_req_data_t ccc_ctrl = {0};
  ccc_ctrl.hdr = gen_hdr();
  ccc_ctrl.msg = gen_msg();

  int64_t const t0 = time_now_us();
  for (size_t i = 0; i < nodes.len; ++i) {
    control_sm_xapp_api(&nodes.n[i].id, SM_CCC_ID, &ccc_ctrl);
  }
  printf("[xApp]: Control Loop Latency: %ld us\n", time_now_us() - t0);
  free_ccc_ctrl_req_data(&ccc_ctrl);

  ////////////
  // END CCC
  ////////////

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");

}
