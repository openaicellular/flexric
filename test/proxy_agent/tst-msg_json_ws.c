/* test stub for testing encoding/decoding in json. */
#include <json-c/json.h>
#include <libwebsockets.h>
#include <stdio.h>

#include "proxy-agent/ser_ran.h"
#include "proxy-agent/ran_msg_hdlr.h"
#include "util/alg_ds/alg/defer.h"

ran_ind_t expected1, expected2, expected3, expected4;

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
  const char *p = ser->encode_ctrl(1, ctrl_msg);
  if (strcmp(p, expected)){
    printf ("FAIL encoding CTRL mismatch: Got '%s', expected '%s'\n", p, expected);
    ret_status = EXIT_FAILURE;
  }
  free(ctrl_msg.mac_ctrl.msg.ran_conf);

  return ret_status;
}
