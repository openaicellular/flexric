/* 
 * test stub for testing performances in decoding json datastructure
 *
 * TOFIX: there is a bug in path handling of structure test that forces the user to run 
 * the executable from the build directory to be able to find correctly the file paths.
 */
#include <json-c/json.h>
#include <libwebsockets.h>
#include <stdio.h>
#include <sys/time.h>
#include <libgen.h>

#include "proxy-agent/ser_ran.h"
#include "proxy-agent/ran_msg_hdlr.h"
#include "util/alg_ds/alg/defer.h"

struct test_t {
  char *path;
  enum {decode_config_get, decode_stats, decode_ue_get, decode_qos_flow_get} op; 
  enum {LOG_YES, LOG_NO} log;
  suseconds_t microsec;
  bool status;
} tests[] = {
  {"../../../test/proxy_agent/config_get.2021.12.17.json", decode_config_get , LOG_YES , 0, true}, 
  {"../../../test/proxy_agent/config_get.2021.12.17.json", decode_config_get , LOG_NO  , 0, true}, 
  {"../../../test/proxy_agent/config_get.2022.09.16.json", decode_config_get , LOG_YES , 0, true}, 
  {"../../../test/proxy_agent/config_get.2022.09.16.json", decode_config_get , LOG_NO  , 0, true}, 
  {"../../../test/proxy_agent/stats.json"                , decode_stats      , LOG_YES , 0, true}, 
  {"../../../test/proxy_agent/stats.json"                , decode_stats      , LOG_NO  , 0, true}, 
  {"../../../test/proxy_agent/ue_get.json"               , decode_ue_get     , LOG_YES , 0, true},
  {"../../../test/proxy_agent/ue_get.json"               , decode_ue_get     , LOG_NO  , 0, true},
  {"../../../test/proxy_agent/qos_flow_get.json"         , decode_qos_flow_get,LOG_NO  , 0, true} 
  
};
char *opstr (int op) 
{
  switch (op){
    case decode_config_get:
      return "decode_config_get";
    case decode_stats:
      return "decode_stats";
    case decode_ue_get:
      return "decode_ue_get";
    case decode_qos_flow_get:
      return "decode_qos_flow_get";
  }
  assert (0!=0 && "Programming error\n");
}
#define ntests (sizeof (tests) / sizeof (tests[0]))

int main()
{
  
  int ret_status = EXIT_SUCCESS;
  bool tmp_ret;

  const size_t nrec = 100;
  ran_ser_abs_t *ser = ran_ser_get_instance();

  for (size_t t = 0; t < ntests; ++t) 
  {
    if (tests[t].log == LOG_YES) 
      lws_set_log_level(1055, NULL); 
    else 
      lws_set_log_level(0, NULL); 
    json_object *root = json_object_from_file(tests[t].path);
    if (root == NULL) {
      printf("Unable to retrieve file:%s\n", tests[t].path);
      return EXIT_FAILURE;
    }
    const char * tststr = json_object_to_json_string(root);
    if (tststr == NULL) {
      printf("json formatting error\n");
      return EXIT_FAILURE;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < nrec; ++i) 
    {
      ran_msg_t in_msg = {.buf = tststr, .len = strlen(tststr)};
      ran_msghdr_t msgdec = ser->get_ran_msghdr(&in_msg);
      defer({ser->free_ran_msghdr(msgdec); }; );
      ran_config_t config_out = {0};  
      ran_ind_t ind_out = {0};
      switch (tests[t].op)
      {
        case decode_config_get:          
          tmp_ret = ser->decode_config_get(&in_msg, &config_out);
          free_ran_config(&config_out);
          break;
        case decode_stats:
          tmp_ret = ser->decode_indication_stats(&in_msg, &ind_out);
          break;
        case decode_ue_get:
          tmp_ret = ser->decode_indication_ue_get(&in_msg, &ind_out);
          break;
        case decode_qos_flow_get:
          tmp_ret = ser->decode_indication_qos_flow_get(&in_msg, &ind_out);
          break;
        default:
          assert (0!=0 && "Programming error\n");
      }
      if (tmp_ret == false){
         printf ("FAIL decoding record %ld\n", i);
         tests[t].status = false;
         continue;
      }  
    }
    
    gettimeofday(&end, NULL);
    tests[t].microsec = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    json_object_put(root);  // free mem
  }

  /* report writing */
  printf("Test results: --------------------------------------------------\n");
  for (size_t t = 0; t < ntests; ++t) 
  {
    printf("Test n.%ld: %s\n", t, (tests[t].status == false) ? "FAILED": "SUCCESS");
    if (tests[t].status == false)
      ret_status = EXIT_FAILURE;
  }
  
  printf("stats averaged over 100 records ----------------------------------------------------------\n");
  for (size_t t = 0; t < ntests; ++t) 
  {    
    char buffer[80];
    time_t ora;
    strftime(buffer, sizeof (buffer),"%x - %I:%M%p", localtime(&ora));
    printf("%s: time taken to parse the %s record coming from file %s with logs %s is %ld μs.\n",
            buffer,
            opstr(tests[t].op),  
            basename(tests[t].path),  
            (tests[t].log == LOG_YES) ? "ON": "OFF",
            tests[t].microsec/nrec
            );
  }
  printf("-----------------------------------------------------------------\n");
  printf ("Total tests status %s\n", (ret_status == EXIT_FAILURE)? "FAILED": "SUCCESS");
  printf ("You may want to add manually these results to test/proxy_agent/benchmarking.txt\n");

  return ret_status;
}

