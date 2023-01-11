/* test stub for testing encoding/decoding in json. */
#include <json-c/json.h>
#include <stdio.h>

#include "util/conversions.h"
#include "../../src/proxy-agent/msg_json_ws.h"
#include "../../src/proxy-agent/ws_msg_hdlr.h"
static bool cmp_ind_t_msg (ws_ind_t in, ws_ind_t expected) 
{
  assert (0!=0 && "Not yet implemented\n");
}

int main()
{
  char *ind_buf = "{\"message\":\"stats\",\"instance_id\":\"63bd3a95\",\"cpu\":{\"global\":41.87113504887293},\"cells\":{\"1\":{\"dl_bitrate\":0\
  ,\"ul_bitrate\":0,\"dl_tx\":0,\"ul_tx\":0,\"dl_retx\":0,\"ul_retx\":0,\"dl_sched_users_min\":0}}}";
  ws_msg_t in_msg = {.buf = ind_buf, .len = strlen(ind_buf)};
  ws_ind_t expected = { .cells_stats[0].cell_id=1, 
                        .cells_stats[0].dl_bitrate=0, 
                        .cells_stats[0].ul_bitrate = 0, 
                        .cells_stats[0].ue_count_avg = 0,
                        .cells_stats[0].dl_tx = 0,
                        .cells_stats[0].ul_tx = 0,
                        .cells_stats[0].dl_retx = 0,
                        .cells_stats[0].ul_retx = 0
                        };
  ws_ind_t out;

  if (ws_json_decode_indication(&in_msg, &out) == false)
    return EXIT_FAILURE;
  if (cmp_ind_t_msg(out, expected) == false)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
