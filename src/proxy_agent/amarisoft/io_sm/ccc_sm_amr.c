#include "ccc_sm_amr.h"

#include "../../../sm/ccc_sm/ccc_sm_id.h"
#include "../../../util/alg_ds/alg/defer.h"
#include "../../../util/time_now_us.h"
#include "../e2_agent_amr_api.h"

#include <stdio.h>

void init_ccc_sm_amr(void)
{
  // No allocation needed
}

void free_ccc_sm_amr(void)
{
  // No allocation needed
}

bool read_ccc_sm_amr(void* data)
{
  assert(data != NULL);
  assert(0!=0 && "Not implemented");
  return true;
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

sm_ag_if_ans_t write_ctrl_ccc_sm_amr(void const* data)
{
  assert(data != NULL);
  ccc_ctrl_req_data_t const* ctrl = (ccc_ctrl_req_data_t const*)data;

  if (ctrl->hdr.frmt_1.ric_style_type == 1){
    // Node configuration and control
    assert(0 != 0 && "Not implemented");
  } else if (ctrl->hdr.frmt_1.ric_style_type == 2){
    // Cell configuration and control
    // [1-65535]
    assert(ctrl->msg.frmt_2.sz_ctrl_msg_cell > 0 && ctrl->msg.frmt_2.sz_ctrl_msg_cell < 65536);
    for (size_t i = 0; i < ctrl->msg.frmt_2.sz_ctrl_msg_cell; i++){
      ctrl_msg_cell_t cur_cell = ctrl->msg.frmt_2.ctrl_msg_cell[i];
      printf("Cell configuration and control level with cell id: %ld \n", (uint64_t)cur_cell.cell_global_id.nr_cgi.nr_cell_id);
      for (size_t z = 0; z < cur_cell.sz_ctrl_msg_ran_conf; z++){
        ctrl_msg_ran_conf_t cur_ran_conf = cur_cell.ctrl_msg_ran_conf[z];
        char* ran_conf_name = copy_ba_to_str(&cur_ran_conf.ran_conf_name);
        defer({free(ran_conf_name);});
        switch (cur_ran_conf.vals_attributes.values_of_attributes_type) {
          case VALUES_OF_ATTRIBUTES_O_BWP:
            assert(strcmp(ran_conf_name, "O-BWP") == 0 &&
                   "Ran configuration name is not O-BWP");
            printf("O-BWP: %s Start rb %d, number of rbs %d \n",
                   bwp_context_to_str(cur_ran_conf.vals_attributes.e2sm_ccc_o_bwp.bwp_context),
                   cur_ran_conf.vals_attributes.e2sm_ccc_o_bwp.start_rb,
                   cur_ran_conf.vals_attributes.e2sm_ccc_o_bwp.number_of_rbs);
            break;
          default:
            assert(0 != 0 && "Not implemented");
        }
      }
    }
  }


  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = CCC_V3_0_AGENT_IF_CTRL_ANS_V0;
  return ans;
}

sm_ag_if_ans_t write_subs_ccc_sm_amr(void const* src)
{
  assert(0!= 0 && "Not implemented");
  assert(src != NULL);
}
