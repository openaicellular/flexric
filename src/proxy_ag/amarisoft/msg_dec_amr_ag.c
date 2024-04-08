#include "msg_dec_amr_ag.h"

#include <assert.h>
#include <string.h>

#include "ie/dec/dec_config_get.h" 
#include "ie/dec/dec_msg_stats.h" 
#include "ie/dec/dec_msg_ue_get.h"

#include "../../util/byte_array.h"
#include "../../util/alg_ds/alg/string/search_naive.h"

typedef struct{
  msg_amr_e type;  
  char* buf;
  size_t len; 
} needle_msg_type_t ;

static
const needle_msg_type_t arr_msgs[3] = {
  {.type = MSG_CONFIG_GET_AMR_E , .buf = "\"config_get\"" , .len = 12},
  {.type = MSG_STATS_AMR_E, .buf = "\"stats\"", .len = 7},
  {.type = MSG_UE_GET_E, .buf = "\"ue_get\"", .len = 8},
};

static
msg_amr_e find_msg_type(const char* src)
{
  const char needle[] = "\"message\"";
  char *pos = strstr(src, needle);
  assert(pos != NULL && "message is mandatory in json file!");

  for(size_t i = 0; i < sizeof(arr_msgs)/sizeof(arr_msgs[0]); ++i){
     char* v = search_naive(arr_msgs[i].len, (char*)arr_msgs[i].buf, 64, pos);
     if(v != NULL) 
       return arr_msgs[i].type;
  }

  assert(0 != 0 && "Unknown message type");
  
  return END_MSG_AMR_E;
}

msg_amr_t msg_dec_amr_ag(ws_msg_t const* src)
{
  assert(src != NULL);

  msg_amr_t dst = {0}; 

  dst.type = find_msg_type((const char*)src->buf);

  if(dst.type == MSG_CONFIG_GET_AMR_E){
    dec_config_get_amr((char*)src->buf, &dst.config);
  } else if(dst.type == MSG_STATS_AMR_E){
    dec_msg_stats_amr((char*)src->buf, &dst.stats);
  } else if(dst.type == MSG_UE_GET_E){
    dec_msg_ue_get_amr((char*)src->buf, &dst.ue);
  } else {
    assert(0 !=0 && "Unknown message type");
  }

  return dst;
}

