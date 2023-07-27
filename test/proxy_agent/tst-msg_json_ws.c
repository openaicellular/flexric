/* test stub for testing encoding/decoding in json. */
#include <json-c/json.h>
#include <libwebsockets.h>
#include <stdio.h>

#include "util/conversions.h"
#include "proxy-agent/ser_ran.h"
#include "proxy-agent/ran_msg_hdlr.h"
#include "util/alg_ds/alg/defer.h"

ran_ind_t expected1, expected2, expected3, expected4;

static void initialize_expected1 (void) 
{
  memset(&expected1, 0,  sizeof(ran_ind_t));
  expected1.cells_stats[0].cell_id = 1;
  expected1.n_connected_ue = 0;
};

static void initialize_expected2 (void) 
{
  memset(&expected2, 0,  sizeof(ran_ind_t));
  expected2.cells_stats[0].cell_id = 1;
  expected2.cells_stats[0].dl_bitrate = 100;
  expected2.cells_stats[0].ul_bitrate = 120;
  expected2.n_connected_ue = 0;
};

static void initialize_expected3 (void) 
{
  memset(&expected3, 0,  sizeof(ran_ind_t));
};

static void initialize_expected4 (void) 
{
  memset(&expected4, 0,  sizeof(ran_ind_t));
  expected4.ue_stats[0].rnti = 17922;
  expected4.ue_stats[0].cell[0].cell_id = 1;
  expected4.ue_stats[0].cell[0].ul_phr = 38;
  expected4.ue_stats[0].cell[0].cqi = 12;
  expected4.n_connected_ue = 1;
};

struct test_t
{                  
  char *json;
  ran_ind_t *expected;
}; 

// test1: 'stats' reply when no ue is connected
// test2: 'ue_get' reply when no ue is connected
// test3: 'ue_get' reply when ue is connected but no stats requested from proxy_agent. This is an error in formulation of the request, stats need to be
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
  return (memcmp(&in, expected, sizeof(in)) == 0) ? true : false;
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
      printf ("FAIL decoding on test(%ld): \"%s\"\n", i, tests[i].json);
      ret_status = EXIT_FAILURE;
      continue;
    }  
    if (cmp_ind_t_msg(out, tests[i].expected) == false) {
      printf ("FAIL value mismatch on test(%ld): \"%s\"\n", i, tests[i].json);
      ret_status = EXIT_FAILURE;
      continue;
    }
  }
  sm_ag_if_wr_t write_msg = {.type = CONTROL_SM_AG_IF_WR,
     .ctrl.type = MAC_CTRL_REQ_V0,
     .ctrl.mac_ctrl.hdr.dummy = 0,
     .ctrl.mac_ctrl.msg = {
        .ran_conf_len = 1}
  };
  write_msg.ctrl.mac_ctrl.msg.ran_conf = calloc(sizeof(mac_conf_t), 1);
  write_msg.ctrl.mac_ctrl.msg.ran_conf[0].rnti = 1;
  write_msg.ctrl.mac_ctrl.msg.ran_conf[0].pusch_mcs = 1;
  write_msg.ctrl.mac_ctrl.msg.ran_conf[0].isset_pusch_mcs = true;
  char *expected = "{\"message\":\"config_set\",\"cells\":[{\"cell_id\":1,\"pusch_mcs\":1}],\"message_id\":\"1\"}";
  const char *p = ser->encode_ctrl(1, write_msg);
  if (strcmp(p, expected)){
    printf ("FAIL encoding CTRL mismatch: Got '%s', expected '%s'\n", p, expected);
    ret_status = EXIT_FAILURE;
  }
  return ret_status;
}
