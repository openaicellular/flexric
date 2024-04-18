#include "io_proxy_ag.h"
#include "kpm_sm_amr.h"
#include "rc_sm_amr.h"
#include "../../../lib/e2ap/e2ap_node_component_config_add_wrapper.h"



static
e2ap_node_component_config_add_t fill_ngap_e2ap_node_component_config_add_amr(void)
{
  e2ap_node_component_config_add_t dst = {0};

  // Mandatory
  // 9.2.26
  dst.e2_node_comp_interface_type = NG_E2AP_NODE_COMP_INTERFACE_TYPE;
  // Bug!! Optional in the standard, mandatory in ASN.1
  // 9.2.32
  dst.e2_node_comp_id.type = NG_E2AP_NODE_COMP_INTERFACE_TYPE;
 
  const char ng_msg[] = "Dummy message";
  dst.e2_node_comp_id.ng_amf_name = cp_str_to_ba(ng_msg); 
 
  // Mandatory
  // 9.2.27
  const char req[] = "NGAP Request Message sent";
  const char res[] = "NGAP Response Message reveived";

  dst.e2_node_comp_conf.request = cp_str_to_ba(req); 
  dst.e2_node_comp_conf.response = cp_str_to_ba(res); 
  return dst;
}

static
void read_setup_ran_proxy(void* data, const e2ap_ngran_node_t node_type)
{
  assert(data != NULL);
  arr_node_component_config_add_t* dst = (arr_node_component_config_add_t*)data;
  // Value assigned by CMakeLists.txt
  assert(node_type == e2ap_ngran_gNB); 
    dst->len_cca = 1;
    dst->cca = calloc(1, sizeof(e2ap_node_component_config_add_t));
    assert(dst->cca != NULL);
    // NGAP
    dst->cca[0] = fill_ngap_e2ap_node_component_config_add_amr();
}

static
void init_read_ind_tbl_proxy(read_ind_fp (*read_ind_tbl)[SM_AGENT_IF_READ_V0_END])
{
  (*read_ind_tbl)[MAC_STATS_V0] = NULL;
  (*read_ind_tbl)[RLC_STATS_V0] = NULL;
  (*read_ind_tbl)[PDCP_STATS_V0] = NULL;
  (*read_ind_tbl)[SLICE_STATS_V0] = NULL;
  (*read_ind_tbl)[TC_STATS_V0] = NULL;
  (*read_ind_tbl)[GTP_STATS_V0] = NULL;
  (*read_ind_tbl)[KPM_STATS_V3_0] = read_kpm_sm_amr;
  (*read_ind_tbl)[RAN_CTRL_STATS_V1_03] = read_rc_sm_amr;
}

static
void init_read_setup_tbl_proxy(read_e2_setup_fp (*read_setup_tbl)[SM_AGENT_IF_E2_SETUP_ANS_V0_END])
{
  (*read_setup_tbl)[MAC_AGENT_IF_E2_SETUP_ANS_V0] = NULL;
  (*read_setup_tbl)[RLC_AGENT_IF_E2_SETUP_ANS_V0] = NULL;
  (*read_setup_tbl)[PDCP_AGENT_IF_E2_SETUP_ANS_V0] = NULL;
  (*read_setup_tbl)[SLICE_AGENT_IF_E2_SETUP_ANS_V0] = NULL;
  (*read_setup_tbl)[TC_AGENT_IF_E2_SETUP_ANS_V0] = NULL;
  (*read_setup_tbl)[GTP_AGENT_IF_E2_SETUP_ANS_V0] = NULL;
  (*read_setup_tbl)[KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0] = read_kpm_setup_sm_amr;
  (*read_setup_tbl)[RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0] = read_rc_setup_sm_amr;
}

static
void init_write_ctrl_proxy(write_ctrl_fp (*write_ctrl_tbl)[SM_AGENT_IF_WRITE_CTRL_V0_END])
{
  (*write_ctrl_tbl)[MAC_CTRL_REQ_V0] = NULL;
  (*write_ctrl_tbl)[RLC_CTRL_REQ_V0] = NULL;
  (*write_ctrl_tbl)[PDCP_CTRL_REQ_V0] = NULL;
  (*write_ctrl_tbl)[SLICE_CTRL_REQ_V0] = NULL;
  (*write_ctrl_tbl)[TC_CTRL_REQ_V0] = NULL;
  (*write_ctrl_tbl)[GTP_CTRL_REQ_V0] = NULL;
  (*write_ctrl_tbl)[RAN_CONTROL_CTRL_V1_03] = write_ctrl_rc_sm_amr;
}

static
void init_write_subs_proxy(write_subs_fp (*write_subs_tbl)[SM_AGENT_IF_WRITE_SUBS_V0_END])
{
  (*write_subs_tbl)[MAC_SUBS_V0] = NULL;
  (*write_subs_tbl)[RLC_SUBS_V0] = NULL;
  (*write_subs_tbl)[PDCP_SUBS_V0] = NULL;
  (*write_subs_tbl)[SLICE_SUBS_V0] = NULL;
  (*write_subs_tbl)[TC_SUBS_V0] = NULL;
  (*write_subs_tbl)[GTP_SUBS_V0] = NULL;
  (*write_subs_tbl)[KPM_SUBS_V3_0] = NULL;
  (*write_subs_tbl)[RAN_CTRL_SUBS_V1_03] = write_subs_rc_sm_amr;
}

static
void init_sm_proxy(void)
{
  init_kpm_sm_amr();
  init_rc_sm_amr();
}

void init_io_proxy_ag(sm_io_ag_ran_t* io)
{
  assert(io != NULL);

  init_read_ind_tbl_proxy(&io->read_ind_tbl);
  init_read_setup_tbl_proxy(&io->read_setup_tbl);
#if defined(E2AP_V2) || defined(E2AP_V3)
  io->read_setup_ran = read_setup_ran_proxy;
#endif
  init_write_ctrl_proxy(&io->write_ctrl_tbl);
  init_write_subs_proxy(&io->write_subs_tbl);

  init_sm_proxy();
}

void free_io_proxy_ag(sm_io_ag_ran_t* io)
{
  assert(io != NULL);
  free_kpm_sm_amr();
}

