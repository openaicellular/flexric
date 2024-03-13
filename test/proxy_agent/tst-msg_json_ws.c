/* test stub for testing encoding/decoding in json. */
#include <json-c/json.h>
#include <libwebsockets.h>
#include <stdio.h>

#include "proxy-agent/ser_ran.h"
#include "proxy-agent/ran_msg_hdlr.h"
#include "util/alg_ds/alg/defer.h"
#include "sm/rc_sm/ie/ir/ran_param_list.h"
#include "sm/rc_sm/ie/rc_data_ie.h"

ran_ind_t expected1, expected2, expected3, expected4;

ran_config_t global_ran_config;

static void initialize_global_ran_config (void)
{
  memset(&global_ran_config, 0, sizeof(ran_config_t));
  global_ran_config.len_nr_cell = 1;
  global_ran_config.nr_cells[0].ncgi.nr_cell_id = 1;
  global_ran_config.nr_cells[0].n_id_nrcell = 500;
  global_ran_config.nr_cells[0].len_ncell = 1;
  global_ran_config.nr_cells[0].ssb_nr_arfcn = 1;
  // Nested cell
  global_ran_config.nr_cells[0].ncell_list[0].n_id_nrcell = 700;
  global_ran_config.nr_cells[0].ncell_list[0].ncgi.nr_cell_id = 2;
  global_ran_config.nr_cells[0].ncell_list[0].ssb_nr_arfcn = 2;

}

static void initialize_expected1 (void)
{
  memset(&expected1, 0, sizeof(ran_ind_t));
  expected1.ran_stats.cells[0].cell_id = 1;
  expected1.ran_stats.len_cell = 1;
  expected1.ran_stats_flags.cells_stats_flags[0].cell_id = true;
  expected1.ran_stats_flags.cells_stats_flags[0].dl_bitrate = true;
  expected1.ran_stats_flags.cells_stats_flags[0].ul_bitrate = true;
  expected1.ran_stats_flags.cells_stats_flags[0].dl_tx = true;
  expected1.ran_stats_flags.cells_stats_flags[0].ul_tx = true;
  expected1.ran_stats_flags.cells_stats_flags[0].dl_retx = true;
  expected1.ran_stats_flags.cells_stats_flags[0].ul_retx = true;
  expected1.ran_stats_flags.cells_stats_flags[0].dl_sched_users_min = true;
};

static void initialize_expected2 (void) 
{
  memset(&expected2, 0,  sizeof(ran_ind_t));
  expected2.ran_stats.cells[0].cell_id = 1;
  expected2.ran_stats.len_cell = 1;
  expected2.ran_stats_flags.cells_stats_flags[0].cell_id = true;
  expected2.ran_stats.cells[0].dl_bitrate = 100;
  expected2.ran_stats_flags.cells_stats_flags[0].dl_bitrate = true;
  expected2.ran_stats.cells[0].ul_bitrate = 120;
  expected2.ran_stats_flags.cells_stats_flags[0].ul_bitrate = true;
  expected2.ran_stats_flags.cells_stats_flags[0].dl_tx = true;
  expected2.ran_stats_flags.cells_stats_flags[0].ul_tx = true;
  expected2.ran_stats_flags.cells_stats_flags[0].dl_retx = true;
  expected2.ran_stats_flags.cells_stats_flags[0].ul_retx = true;
  expected2.ran_stats_flags.cells_stats_flags[0].dl_sched_users_min = true;
};

static void initialize_expected3 (void) 
{
  memset(&expected3, 0, sizeof(ran_ind_t));
};

static void initialize_expected4 (void) 
{
  memset(&expected4, 0, sizeof(ran_ind_t));
  expected4.ue_stats[0].time = 0;
  expected4.ue_stats[0].enb_ue_id = 0;
  expected4.ue_stats[0].ran_ue_id = 2;
  expected4.ue_stats[0].mme_ue_id = 0;
  expected4.ue_stats[0].amf_ue_id = 2;
  expected4.ue_stats[0].linked_enb_ue_id = 0;
  expected4.ue_stats[0].linked_ran_ue_id = 0;
  expected4.ue_stats[0].rnti = 17922;
  expected4.ue_stats[0].cell_flags[0].cell_id = true;
  expected4.ue_stats[0].cells[0].cell_id = 1;
  expected4.ue_stats[0].cell_flags[0].ul_phr = true;
  expected4.ue_stats[0].cells[0].ul_phr = 38;
  expected4.ue_stats[0].cell_flags[0].cqi = true;
  expected4.ue_stats[0].cells[0].cqi = 12;
  expected4.ue_stats[0].len_cell = 1;
  expected4.len_ue_stats = 1;
};

struct test_t
{                  
  char *json;
  ran_ind_t *expected;
}; 

// test1: 'stats' reply when no ue is connected
// test2: 'stats' reply when ue is connected
// test3: 'ue_get' reply when no ue is connected
// test4: 'ue_get' reply when ue is connected but no stats requested from proxy_agent. This is an error in formulation of the request, stats need to be
//        requested always.

static struct test_t tests[] =
  {
    { "{\"message\":\"stats\",\"instance_id\":\"63bd3a95\",\"cpu\":{\"global\":41.87113504887293},\"cells\":{\"1\":{\"dl_bitrate\":0\
  ,\"ul_bitrate\":0,\"dl_tx\":0,\"ul_tx\":0,\"dl_retx\":0,\"ul_retx\":0,\"dl_sched_users_min\":0}}}", &expected1
    },
    { "{\"message\":\"stats\",\"instance_id\":\"63bd3a95\",\"cpu\":{\"global\":41.87113504887293},\"cells\":{\"1\":{\"dl_bitrate\":100\
  ,\"ul_bitrate\":120,\"dl_tx\":0,\"ul_tx\":0,\"dl_retx\":0,\"ul_retx\":0,\"dl_sched_users_min\":0}}}", &expected2    
    },
    { "{\"message\":\"ue_get\",\"ue_list\":[],\"message_id\":\"1\",\"time\":1807.759}\"}", &expected3
    },
    { "{\"message\":\"ue_get\",\"ue_list\":[{\"ran_ue_id\":2,\"amf_ue_id\":2,\"rnti\":17922,\"cells\":[{\"cell_id\":1,\"cqi\":12,\"ul_phr\":38}]}],\"message_id\":\"1\",\"time\":1807.759}\"}", &expected4
    }
  };


#define ntests (sizeof (tests) / sizeof (tests[0]))

static bool cmp_ind_t_msg (ran_ind_t in, ran_ind_t *expected) 
{
  bool ret = (memcmp(&in, expected, sizeof(in)) == 0) ? true : false;
  if (ret == false)
  {
    printf("Difference was in (");
    if (memcmp( &in.ran_stats,
                &expected->ran_stats,
                sizeof(in.ran_stats) != 0))
      printf("ran_stats");
            
    if (memcmp(&in.ran_stats_flags,
                &expected->ran_stats_flags,
                sizeof(in.ran_stats)) != 0)
      printf(" ran_stats_flags");

    if (memcmp(&in.ue_stats,
                &expected->ue_stats, 
                sizeof(in.ue_stats)) != 0)
      printf(" ue_stats");
                    
    if (in.len_ue_stats != expected->len_ue_stats)
      printf(" n_connected_ue");

    if (in.len_qos_flow_stats != expected->len_qos_flow_stats)
      printf(" qos_flow_stats_len");
     
    if (memcmp(&in.qos_flow_stats,
                &expected->qos_flow_stats, 
                sizeof(in.qos_flow_stats)) != 0)
      printf(" qos_flow_stats");

    if (memcmp(&in.qos_flow_stats_flags,
                &expected->qos_flow_stats_flags, 
                sizeof(in.qos_flow_stats_flags)) != 0)
      printf(" qos_flow_stats_flags");

    printf(")\n");
  }
  return ret;
}

static
void gen_target_primary_cell_id(seq_ran_param_t* Target_primary_cell_id, uint64_t nr_cgi)
{
    // Target Primary Cell ID, STRUCTURE (Target Primary Cell ID)
    Target_primary_cell_id->ran_param_id = Target_primary_cell_id_8_4_4_1;
    Target_primary_cell_id->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    Target_primary_cell_id->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(Target_primary_cell_id->ran_param_val.strct != NULL && "Memory exhausted");
    Target_primary_cell_id->ran_param_val.strct->sz_ran_param_struct = 1;
    Target_primary_cell_id->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(Target_primary_cell_id->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // CHOICE Target Cell, STRUCTURE (Target Primary Cell ID -> CHOICE Target Cell )
    seq_ran_param_t* CHOICE_target_cell = &Target_primary_cell_id->ran_param_val.strct->ran_param_struct[0];
    CHOICE_target_cell->ran_param_id = CHOICE_target_cell_8_4_4_1;
    CHOICE_target_cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    CHOICE_target_cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(CHOICE_target_cell->ran_param_val.strct != NULL && "Memory exhausted");
    CHOICE_target_cell->ran_param_val.strct->sz_ran_param_struct = 2;
    CHOICE_target_cell->ran_param_val.strct->ran_param_struct = calloc(2, sizeof(seq_ran_param_t));
    assert(CHOICE_target_cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // NR Cell, STRUCTURE (CHOICE target cell -> NR Cell)
    seq_ran_param_t* NR_cell = &CHOICE_target_cell->ran_param_val.strct->ran_param_struct[0];
    NR_cell->ran_param_id = NR_cell_8_4_4_1;
    NR_cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    NR_cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(NR_cell->ran_param_val.strct != NULL && "Memory exhausted");
    NR_cell->ran_param_val.strct->sz_ran_param_struct = 1;
    NR_cell->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(NR_cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // NR CGI, ELEMENT (NR Cell -> NR CGI)
    seq_ran_param_t* NR_cgi = &NR_cell->ran_param_val.strct->ran_param_struct[0];
    NR_cgi->ran_param_id = NR_CGI_8_4_4_1;
    NR_cgi->ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
    NR_cgi->ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
    assert(NR_cgi->ran_param_val.flag_false != NULL && "Memory exhausted");
    // NR CGI IE in TS 38.423 [15] Clause 9.2.2.7
    NR_cgi->ran_param_val.flag_false->type = BIT_STRING_RAN_PARAMETER_VALUE;
    char nr_cgi_val[30];
    sprintf(nr_cgi_val, "%lu", nr_cgi);
    byte_array_t nr_cgi_ba = cp_str_to_ba(nr_cgi_val);
    NR_cgi->ran_param_val.flag_false->bit_str_ran.buf = nr_cgi_ba.buf;
    NR_cgi->ran_param_val.flag_false->bit_str_ran.len = nr_cgi_ba.len;

    // E-ULTRA Cell, STRUCTURE (CHOICE Target Cell -> E-ULTRA cell)
    seq_ran_param_t* E_ultra_cell = &CHOICE_target_cell->ran_param_val.strct->ran_param_struct[1];
    E_ultra_cell->ran_param_id = E_ULTRA_Cell_8_4_4_1;
    E_ultra_cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    E_ultra_cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(E_ultra_cell->ran_param_val.strct != NULL && "Memory exhausted");
    E_ultra_cell->ran_param_val.strct->sz_ran_param_struct = 1;
    E_ultra_cell->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(E_ultra_cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // E-ULTRA CGI, ELEMENT (E-ULTRA Cell -> E-ULTRA CGI)
    seq_ran_param_t* E_ultra_cgi = &E_ultra_cell->ran_param_val.strct->ran_param_struct[0];
    E_ultra_cgi->ran_param_id = E_ULTRA_CGI_8_4_4_1;
    E_ultra_cgi->ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
    E_ultra_cgi->ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
    assert(E_ultra_cgi->ran_param_val.flag_false != NULL && "Memory exhausted");
    // E-UTRA CGI IE in TS 38.423 [15] Clause 9.2.2.8
    E_ultra_cgi->ran_param_val.flag_false->type = BIT_STRING_RAN_PARAMETER_VALUE;
    const char e_utra_cgi_val[] = "8";
    byte_array_t e_ultra_cgi_ba = cp_str_to_ba(e_utra_cgi_val);
    E_ultra_cgi->ran_param_val.flag_false->bit_str_ran.buf = e_ultra_cgi_ba.buf;
    E_ultra_cgi->ran_param_val.flag_false->bit_str_ran.len = e_ultra_cgi_ba.len;

    return;
}

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1_hand_over(uint64_t nr_cgi)
{
    e2sm_rc_ctrl_msg_frmt_1_t dst = {0};

    // 8.4.4.1
    // Target Primary Cell ID, STRUCTURE (len 1)
    // > RAN UE Id, ELEMENT //
    // > CHOICE Target Cell, STRUCTURE (len 2)
    // >> NR Cell, STRUCTURE (len 1)
    // >>> NR CGI, ELEMENT
    // >> E-ULTRA Cell, STRUCTURE (len 1)
    // >>> E-ULTRA CGI, ELEMENT

    dst.sz_ran_param = 1;
    dst.ran_param = calloc(1, sizeof(seq_ran_param_t));
    assert(dst.ran_param != NULL && "Memory exhausted");

    gen_target_primary_cell_id(&dst.ran_param[0], nr_cgi);
    return dst;
}

static
sm_ag_if_wr_ctrl_t create_ctrl_msg(uint64_t ran_ue_id, uint64_t nr_cgi){
  sm_ag_if_wr_ctrl_t ctrl_msg_rc = {
      .type = RAN_CONTROL_CTRL_V1_03,
  };
  ctrl_msg_rc.rc_ctrl.hdr.format = FORMAT_1_E2SM_RC_CTRL_HDR;
  ctrl_msg_rc.rc_ctrl.hdr.frmt_1.ric_style_type = 3;
  ctrl_msg_rc.rc_ctrl.hdr.frmt_1.ctrl_act_id = Handover_control_7_6_4_1;
  ctrl_msg_rc.rc_ctrl.hdr.frmt_1.ue_id.gnb.ran_ue_id = malloc(sizeof(uint64_t));
  *ctrl_msg_rc.rc_ctrl.hdr.frmt_1.ue_id.gnb.ran_ue_id = ran_ue_id; // ran_ue_id
  ctrl_msg_rc.rc_ctrl.msg.format = FORMAT_1_E2SM_RC_CTRL_MSG;
  ctrl_msg_rc.rc_ctrl.msg.frmt_1 = gen_rc_ctrl_msg_frmt_1_hand_over(nr_cgi);

  return ctrl_msg_rc;
}

int main()
{
  lws_set_log_level(1055, NULL); 
  int ret_status = EXIT_SUCCESS;
  bool tmp_ret;
  ran_ser_abs_t *ser = ran_ser_get_instance();
  initialize_expected1();
  initialize_expected2();
  initialize_expected3();
  initialize_expected4();
  initialize_global_ran_config();
  for (size_t i = 0; i < ntests; ++i) 
  {
    ran_msg_t in_msg = {.buf = tests[i].json, .len = strlen(tests[i].json)};
    ran_ind_t out = {0};
    
    ran_msghdr_t msgdec = ser->get_ran_msghdr(&in_msg);
    defer({ser->free_ran_msghdr(msgdec); }; );
    tmp_ret = (strcmp(msgdec.type, "ue_get") == 0) ? 
              ser->decode_indication_ue_get(&in_msg, &out):
              ser->decode_indication_stats(&in_msg, &out);
              
    if (tmp_ret == false){
      printf ("FAIL decoding on test(%ld). Input was:\"%s\"\n", i, tests[i].json);
      ret_status = EXIT_FAILURE;
      continue;
    }  
    if (cmp_ind_t_msg(out, tests[i].expected) == false) {
      printf ("FAIL value mismatch on test(%ld). Input was:\"%s\"\n", i, tests[i].json);
      ret_status = EXIT_FAILURE;
      continue;
    }
  }

  sm_ag_if_wr_ctrl_t ctrl_msg = {
     .type = MAC_CTRL_REQ_V0,
     .mac_ctrl.hdr.dummy = 0,
     .mac_ctrl.msg = {
        .ran_conf_len = 1}
  };

  ctrl_msg.mac_ctrl.msg.ran_conf = calloc(sizeof(mac_conf_t), 1);
  ctrl_msg.mac_ctrl.msg.ran_conf[0].rnti = 1;
  ctrl_msg.mac_ctrl.msg.ran_conf[0].pusch_mcs = 1;
  ctrl_msg.mac_ctrl.msg.ran_conf[0].isset_pusch_mcs = true;
  char *expected = "{\"message\":\"config_set\",\"cells\":[{\"cell_id\":1,\"pusch_mcs\":1}],\"message_id\":\"1\"}";
  const char *p = ser->encode_ctrl(1, ctrl_msg, &global_ran_config);
  if (strcmp(p, expected)){
    printf ("FAIL encoding CTRL mismatch: Got '%s', expected '%s'\n", p, expected);
    ret_status = EXIT_FAILURE;
  }
  free(ctrl_msg.mac_ctrl.msg.ran_conf);

  // Outer cell test
  sm_ag_if_wr_ctrl_t ctrl_msg_rc = create_ctrl_msg(1, 1);

  // Return value depends on cell information from config_get api
  char *expected_rc_ctrl_1 = "{\"message\":\"handover\",\"ran_ue_id\":1,\"pci\":500,\"ssb_nr_arfcn\":1,\"message_id\":\"1\"}";
  const char *p_rc_1 = ser->encode_ctrl(1, ctrl_msg_rc, &global_ran_config);
  if (strcmp(p_rc_1, expected_rc_ctrl_1)){
    printf ("FAIL encoding CTRL RC mismatch: Got '%s', expected '%s'\n", p_rc_1, expected_rc_ctrl_1);
    ret_status = EXIT_FAILURE;
  }
  free(ctrl_msg_rc.rc_ctrl.hdr.frmt_1.ue_id.gnb.ran_ue_id);
  free_e2sm_rc_ctrl_msg_frmt_1(&ctrl_msg_rc.rc_ctrl.msg.frmt_1);

  // inner cell test
  sm_ag_if_wr_ctrl_t ctrl_msg_rc_2 = create_ctrl_msg(1, 2);

  // Return value depends on cell information from config_get api
  char *expected_rc_ctrl_2 = "{\"message\":\"handover\",\"ran_ue_id\":1,\"pci\":700,\"ssb_nr_arfcn\":2,\"message_id\":\"1\"}";
  const char *p_rc_2 = ser->encode_ctrl(1, ctrl_msg_rc_2, &global_ran_config);
  if (strcmp(p_rc_2, expected_rc_ctrl_2)){
    printf ("FAIL encoding CTRL RC mismatch: Got '%s', expected '%s'\n", p_rc_2, expected_rc_ctrl_2);
    ret_status = EXIT_FAILURE;
  }

  free(ctrl_msg_rc_2.rc_ctrl.hdr.frmt_1.ue_id.gnb.ran_ue_id);
  free_e2sm_rc_ctrl_msg_frmt_1(&ctrl_msg_rc_2.rc_ctrl.msg.frmt_1);

  return ret_status;
}
