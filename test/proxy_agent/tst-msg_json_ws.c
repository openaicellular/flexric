/* test stub for testing encoding/decoding in json. */
#include <json-c/json.h>
#include <stdio.h>

#include "util/conversions.h"
#include "../../src/proxy-agent/msg_json_ws.h"
#include "../../src/proxy-agent/ws_msg_hdlr.h"
#include "util/alg_ds/alg/defer.h"

struct test_t
{                  
  char *json;
  ws_ind_t expected;
}; 

// test1: 'stats' reply when no ue is connected
// test2: 'ue_get' reply when no ue is connected
// test3: 'ue_get' reply when ue is connected but no stats requested form proxy_agent. This is an error in formulation of the request, stats need to be
//        requested always.

static struct test_t tests[] =
  {
    { "{\"message\":\"stats\",\"instance_id\":\"63bd3a95\",\"cpu\":{\"global\":41.87113504887293},\"cells\":{\"1\":{\"dl_bitrate\":0\
  ,\"ul_bitrate\":0,\"dl_tx\":0,\"ul_tx\":0,\"dl_retx\":0,\"ul_retx\":0,\"dl_sched_users_min\":0}}}", 
      { .cells_stats = {
        { .cell_id = 1, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0}
        },
        .ue_stats = {
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0}
        },
        .n_connected_ue = 0
      }
    },
    { "{\"message\":\"ue_get\",\"ue_list\":[],\"message_id\":\"1\",\"time\":1807.759}\"}",
      {
        .cells_stats = {
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0}
        },
        .ue_stats = {
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0}
        },
        .n_connected_ue = 0
      } 
    },
    { "{\"message\":\"ue_get\",\"ue_list\":[{\"ran_ue_id\":2,\"amf_ue_id\":2,\"rnti\":17922,\"cells\":[{\"cell_id\":1,\"cqi\":12,\"ul_phr\":38}]}],\"message_id\":\"1\",\"time\":1807.759}\"}",
      {
        .cells_stats = {
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0},
        { .cell_id = 0, .dl_bitrate = 0, .ul_bitrate = 0, .ue_count_avg = 0, .dl_tx = 0, .ul_tx = 0, .dl_retx = 0, .ul_retx = 0}
        },
        .ue_stats = {
          { .rnti = 17922, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 38, .cqi = 12},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0},
          { .rnti = 0, .dl_bitrate = 0, .ul_bitrate = 0, .dl_tx = 0, .dl_retx = 0, .dl_mcs = 0, .ul_mcs = 0, .phr = 0, .cqi = 0}
        },
        .n_connected_ue = 1
      }
    }
  };


#define ntests (sizeof (tests) / sizeof (tests[0]))

static bool cmp_ind_t_msg (ws_ind_t in, ws_ind_t expected) 
{
  return (memcmp(&in, &expected, sizeof(in)) == 0) ? true : false;
}

int main()
{
  lws_set_log_level(1055, NULL); // XXX: set to debug level,but you probably would like to set from commandline
  int ret_status = EXIT_SUCCESS;
  bool tmp_ret;
  for (size_t i = 0; i < ntests; ++i) 
  {
    ws_msg_t in_msg = {.buf = tests[i].json, .len = strlen(tests[i].json)};
    ws_ind_t out = {0};
    ws_msghdr_t msgdec = ws_json_get_msghdr(&in_msg);
    defer({ws_msghdr_free(msgdec); }; );
    tmp_ret = (strcmp(msgdec.type, "ue_get") == 0) ? 
              ws_json_decode_indication_ue(&in_msg, &out):
              ws_json_decode_indication_gnb(&in_msg, &out);
    if (tmp_ret == false){
      printf ("FAIL decoding: \"%s\"\n", tests[i].json);
      ret_status = EXIT_FAILURE;
      continue;
    }  
    if (cmp_ind_t_msg(out, tests[i].expected) == false) {
      printf ("FAIL value mismatch: \"%s\"\n", tests[i].json);
      ret_status = EXIT_FAILURE;
      continue;
    }
  }
  return ret_status;
}
