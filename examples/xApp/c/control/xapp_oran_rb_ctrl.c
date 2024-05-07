#include "../../../../src/xApp/e42_xapp_api.h"
#include "../../../../src/sm/ccc_sm/ccc_sm_id.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static
cell_global_id_t fill_rnd_cell_global_id()
{
  cell_global_id_t dst = {0};

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_CGI_RAT_TYPE;
  dst.nr_cgi.plmn_id = (e2sm_plmn_t){.mcc = 505, .mnc = 55, .mnc_digit_len = 2};
  dst.nr_cgi.nr_cell_id = 10001;

  return dst;
}

static
e2sm_ccc_o_bwp_t fill_rnd_e2sm_ccc_o_bwp() {
  e2sm_ccc_o_bwp_t dst;

  dst.bwp_context = DL_BWP_CONTEXT;
  dst.start_rb = 0;
  dst.number_of_rbs = 10;

  return dst;
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);
  defer({ free_fr_args(&args); });

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

  ccc_ctrl.hdr.format = FORMAT_1_E2SM_CCC_CTRL_HDR;
  ccc_ctrl.hdr.frmt_1.ric_style_type = 2;
  
  ccc_ctrl.msg.format = FORMAT_2_E2SM_CCC_CTRL_MSG;
  ccc_ctrl.msg.frmt_2.sz_ctrl_msg_cell = 1;
  ccc_ctrl.msg.frmt_2.ctrl_msg_cell = calloc(ccc_ctrl.msg.frmt_2.sz_ctrl_msg_cell, sizeof(ctrl_msg_cell_t));
  assert(ccc_ctrl.msg.frmt_2.ctrl_msg_cell != NULL);
  
  for (size_t i = 0; i < ccc_ctrl.msg.frmt_2.sz_ctrl_msg_cell; i++){
    ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].cell_global_id = fill_rnd_cell_global_id();
    ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].sz_ctrl_msg_ran_conf= 1;
    ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].ctrl_msg_ran_conf = calloc(ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].sz_ctrl_msg_ran_conf, sizeof(ctrl_msg_ran_conf_t));
    assert(ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].ctrl_msg_ran_conf!= NULL && "Memory exhausted");
    for (size_t z = 0; z < ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].sz_ctrl_msg_ran_conf; z++) {
      ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].ctrl_msg_ran_conf[z].ran_conf_name = cp_str_to_ba("O-BWP");
      ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].ctrl_msg_ran_conf[z].vals_attributes.values_of_attributes_type = VALUES_OF_ATTRIBUTES_O_BWP;
      ccc_ctrl.msg.frmt_2.ctrl_msg_cell[i].ctrl_msg_ran_conf[z].vals_attributes.e2sm_ccc_o_bwp = fill_rnd_e2sm_ccc_o_bwp();
    }
  }


  int64_t st = time_now_us();
  for(size_t i =0; i < nodes.len; ++i){
    control_sm_xapp_api(&nodes.n[i].id, SM_CCC_ID, &ccc_ctrl);
  }
  printf("[xApp]: Control Loop Latency: %ld us\n", time_now_us() - st);
  free_ccc_ctrl_req_data(&ccc_ctrl);

  ////////////
  // END CCC
  ////////////

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");

}