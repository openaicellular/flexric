#include "conf_file.h"

#include <assert.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <libconfig.h>
#include "alg_ds/alg/defer.h"

#define valid_ipv4 valid_ip
#define MAX_PASSWORD_LENGTH 64

static
const char* default_conf_file = "/usr/local/etc/flexric/ric.conf";

#ifdef PROXY_AGENT
#include <libwebsockets.h>
static
bool valid_logl(int logl)
{
  return logl & ( LLL_ERR    |
                  LLL_WARN   |
                  LLL_NOTICE |
	                LLL_INFO	 |
                  LLL_DEBUG	 |
                  LLL_PARSER |
                  LLL_HEADER |
                  LLL_EXT		 |
                  LLL_CLIENT |
                  LLL_LATENCY|
                  LLL_USER	 |
                  LLL_THREAD
                );


}
#endif
/*
static
bool valid_port(const char *port)
{
  int val = atoi(port);
  return (val > 0 && val < USHRT_MAX);
}


static
bool valid_ip(const char* ip)
{
  assert(ip != NULL);
  if (strlen(ip) < 7) // 8.8.8.8, at least we need 7 characters  
    return false;

  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
  return result != 0;
}

static
char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

static
char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}
*/
static
bool is_directory(const char* path)
{
  assert(path != NULL);

  struct stat sb = {0};
  int const rc = stat(path, &sb); 

  if(rc != 0){
    printf("Error finding path %s \n%s \n ", path, strerror(errno) ); 
    exit(EXIT_FAILURE);
  }

  if(S_ISDIR(sb.st_mode) == true)
    return true;

  return false;
}

static
bool is_regular_file(const char* path)
{
  assert(path != NULL);

  struct stat sb = {0};
  int const rc = stat(path, &sb); 

  if(rc != 0){
    printf("Error finding path %s \n%s \n ", path, strerror(errno) ); 
    exit(EXIT_FAILURE);
  }

  if(S_ISREG(sb.st_mode) == true)
    return true;

  return false;
}

static
void print_usage(void)
{     
  printf("Usage: [options]\n"); 
  printf("\n");
  printf("    General options:\n");
  printf("  -h         : print usage\n");
  printf("  -c         : path to the config file\n");
//  printf("  -p         : path to the shared libs \n");
  printf(
      "\n");
  printf("Ex. -p /usr/local/lib/flexric/ -c /usr/local/etc/flexric/flexric.conf \n");
} 

static
void parse_args(int argc, char* const* argv, fr_args_t* args)
{
  assert(argc > 1 && "No argument passed");
  assert(argv != NULL);
  assert(args != NULL);

  int opt = '?';
  const char *optstring = "hc:p:";
  while((opt = getopt(argc, argv, optstring)) != -1) {
    switch(opt) {
      case 'h':{
                 print_usage();
                 // printf("Usage  to be or not to be... \n" );
                 exit(EXIT_SUCCESS);
               }
      case 'c':  {
                   int const len = strlen(optarg);
                   assert(len < FR_CONF_FILE_LEN - 1);

                   if(!is_regular_file(optarg)){
                     printf("Error: %s is not a regular file \n", optarg);  
                     exit(EXIT_FAILURE);
                   }

                   memset(args->conf_file, '\0', FR_CONF_FILE_LEN);
                   strncpy(args->conf_file, optarg, len );

                   break;
                 }
//      case 'p':{
//                 int const len = strlen(optarg);
//                 assert(len < FR_CONF_FILE_LEN - 1 );
//
//                 if(!is_directory(optarg)){
//                   printf("Error: %s is not a directory \n", optarg);
//                   exit(EXIT_FAILURE);
//                 }
//                 if(optarg[len -1] != '/'){
//                   printf("Error: %s directory should finish with a / , e.g. /usr/local/flexric/  please add it\n", optarg);
//                   exit(EXIT_FAILURE);
//                 }
//
//                 memset(args->libs_dir, '\0', FR_CONF_FILE_LEN);
//                 strncpy(args->libs_dir, optarg, len);
//
//                 break;
//               }
      case '?':{
                 printf("Error: unknown flag %c ??\n ",optopt);
                 print_usage();
                 exit(EXIT_FAILURE);
               }
      default:{
                assert(0!=0 && "Unforeseen code path");
                exit(EXIT_FAILURE);
              }

    }   
  }
}

static
void load_default_val(fr_args_t* args)
{
  assert(args != NULL);
  assert(strlen(default_conf_file) < FR_CONF_FILE_LEN && "Path too long");

  memset(args, '\0', sizeof(*args));
  strncpy(args->conf_file, default_conf_file, FR_CONF_FILE_LEN);
}

static
void get_component_name(fr_args_t* args, config_t cfg)
{
  const char* name;
  if (!config_lookup_string(&cfg, "Name", &name))
    assert(0!=0 && "Name is required in .conf, ex: Name = \"NearRT_RIC\", Name = \"E2_Node\", or Name = \"xApp\"");

  args->name = malloc(strlen(name) + 1);
  strcpy(args->name, name);
}

static
void get_SM_DIR(fr_args_t* args, config_t cfg)
{
  const char* dir;
  if (!config_lookup_string(&cfg, "SM_DIR", &dir))
    assert(0!=0 && "SM_DIR is required in .conf");

  args->libs_dir = malloc(strlen(dir) + 1);
  strcpy(args->libs_dir, dir);
}

static
void get_NearRT_RIC_IP(fr_args_t* args, config_t cfg)
{
  const char* ip;
  if (!config_lookup_string(&cfg, "NearRT_RIC_IP", &ip))
    assert(0!=0 && "NearRT_RIC_IP is required in .conf");

  args->ip = malloc(strlen(ip) + 1);
  strcpy(args->ip, ip);
}

static
void get_E2_Port(fr_args_t* args, config_t cfg)
{
  int32_t port;
  if (!config_lookup_int(&cfg, "E2_Port", &port))
    assert(0!=0 && "E2_Port is required in .conf");

  args->e2_port = port;
}

static
void get_E42_Port(fr_args_t* args, config_t cfg)
{
  int32_t port;
  if (!config_lookup_int(&cfg, "E42_Port", &port))
    assert(0!=0 && "E42_Port is required in .conf");

  args->e42_port = port;
}

static
void get_Sub_SM_List(fr_args_t* args, config_t cfg)
{
  config_setting_t* sub_cust_sm_list = config_lookup(&cfg, "Sub_CUST_SM_List");
  config_setting_t* sub_oran_sm_list = config_lookup(&cfg, "Sub_ORAN_SM_List");
  assert((sub_cust_sm_list!=NULL || sub_oran_sm_list!=NULL) && "Sub_CUST_SM_List or Sub_ORAN_SM_List is required for xApp in .conf");

  if (sub_cust_sm_list != NULL) {
    int count = config_setting_length(sub_cust_sm_list);
    args->sub_cust_sm_len = count;
    assert(args->sub_cust_sm_len > 0 && "sub_cust_sm_len <= 0");
    for (int i = 0; i < count; ++i) {
      config_setting_t* sub_sm_item = config_setting_get_elem(sub_cust_sm_list, i);
      const char* name;
      const char* time;

      if (!config_setting_lookup_string(sub_sm_item, "name", &name) ||
          !config_setting_lookup_string(sub_sm_item, "time", &time))
        assert(0!=0 && "Error parsing name and time in Sub_CUST_SM_List in .conf");

      args->sub_cust_sm[i].name = malloc(strlen(name) + 1);
      strcpy(args->sub_cust_sm[i].name, name);
      args->sub_cust_sm[i].time = malloc(strlen(time) + 1);
      strcpy(args->sub_cust_sm[i].time, time);
//      printf("[LibConf]: Sub_CUST_SM Name: %s, Time: %s\n", args->sub_cust_sm[i].name, args->sub_cust_sm[i].time);
    }
  }

  if (sub_oran_sm_list != NULL) {
    int count = config_setting_length(sub_oran_sm_list);
    args->sub_oran_sm_len = count;
    assert(args->sub_oran_sm_len > 0 && "sub_oran_sm_len <= 0");
    for (int i = 0; i < count; ++i) {
      config_setting_t* sub_sm_item = config_setting_get_elem(sub_oran_sm_list, i);
      const char* name;
      int32_t time;
      int32_t format;
      const char* ran_type;
      config_setting_t *actions_arr = config_setting_get_member(sub_sm_item, "actions");

      if (!config_setting_lookup_string(sub_sm_item, "name", &name) ||
          !config_setting_lookup_int(sub_sm_item, "time", &time) ||
          !config_setting_lookup_int(sub_sm_item, "format", &format) ||
          actions_arr == NULL ||
          !config_setting_lookup_string(sub_sm_item, "ran_type", &ran_type))
        assert(0!=0 && "cannnot find name and time in Sub_ORAN_SM_List in .conf");

      args->sub_oran_sm[i].name = malloc(strlen(name) + 1);
      strcpy(args->sub_oran_sm[i].name, name);
      args->sub_oran_sm[i].time = time;
//      printf("[LibConf]: Sub_ORAN_SM Name: %s, Time: %d\n", args->sub_oran_sm[i].name, args->sub_oran_sm[i].time);

      args->sub_oran_sm[i].format = format;
      args->sub_oran_sm[i].ran_type = malloc(strlen(ran_type) + 1);
      strcpy(args->sub_oran_sm[i].ran_type, ran_type);
      int act_count = config_setting_length(actions_arr);
      args->sub_oran_sm[i].act_len = act_count + 1; // for C xApp, save the latest value as NULL
      args->sub_oran_sm[i].actions = malloc(args->sub_oran_sm[i].act_len * sizeof(char*));
      // printf("[LibConf]: ran type %s, format %d\n", args->sub_oran_sm[i].ran_type, format);
      for (int j = 0; j < args->sub_oran_sm[i].act_len; ++j) {
        if (j == act_count) {
          args->sub_oran_sm[i].actions[j] = NULL;
          continue;
        }

        config_setting_t *action_item = config_setting_get_elem(actions_arr, j);
        const char *action_name;

        if (!config_setting_lookup_string(action_item, "name", &action_name))
          assert(0!=0 && "Error parsing action name in Sub_ORAN_SM_List in .conf");

        args->sub_oran_sm[i].actions[j] = strdup(action_name);
//        printf("%s, ", args->sub_oran_sm[i].actions[j]);
      }
//      printf("\n");
    }
  }
}

static
void get_xApp_DB(fr_args_t* args, config_t cfg)
{
  const config_setting_t *db_settings = config_lookup(&cfg, "xApp_DB");
  assert(db_settings!=NULL && "xApp_DB is required for xApp in .conf");

  if (db_settings != NULL) {
    const char *enable;
    const char *ip;
    const char *dir;
    const char *filename;
#ifdef MYSQL_XAPP
    const char *username;
    const char *password;
#endif
    if (!config_setting_lookup_string(db_settings, "enable", &enable))
      assert(0!=0 && "\"enable\" is required in xApp_DB in .conf");

    if (!strcasecmp(enable, "ON") || !strcasecmp(enable, "1")) {
      args->db_params.enable = true;
    } else {
      args->db_params.enable = false;
    }

    if (!args->db_params.enable)
      return;
    if (!config_setting_lookup_string(db_settings, "ip", &ip) ||
        !config_setting_lookup_string(db_settings, "dir", &dir) ||
          !config_setting_lookup_string(db_settings, "filename", &filename))
      assert(0!=0 && "cannnot find ip, dir, and filename in xApp_DB in .conf");

    args->db_params.ip = malloc(strlen(ip) + 1);
    strcpy(args->db_params.ip, ip);
    args->db_params.dir = malloc(strlen(dir) + 1);
    strcpy(args->db_params.dir, dir);
    args->db_params.filename = malloc(strlen(filename) + 1);
    strcpy(args->db_params.filename, filename);

#ifdef MYSQL_XAPP
    if (!config_setting_lookup_string(db_settings, "username", &username) ||
        !config_setting_lookup_string(db_settings, "password", &password))
      assert(0!=0 && "cannnot find username and password in xApp_DB in .conf");

    args->db_params.user = malloc(strlen(username) + 1);
    strcpy(args->db_params.user, username);
    args->db_params.pass = malloc(strlen(password) + 1);
    strcpy(args->db_params.pass, password);
#endif

  }
}

#ifdef PROXY_AGENT
static
void get_Proxy_Agent(fr_args_t* args, config_t cfg)
{
  const config_setting_t *pa_settings = config_lookup(&cfg, "Proxy_Agent");
  assert(pa_settings!=NULL && "Proxy_Agent is required for E2 Proxy Agent in .conf");

  if (pa_settings != NULL) {
    const char *ip;
    int32_t port;
    int32_t logl;

    if (!config_setting_lookup_string(pa_settings, "ip", &ip) ||
        !config_setting_lookup_int(pa_settings, "port", &port) ||
        !config_setting_lookup_int(pa_settings, "logl", &logl))
      assert(0!=0 && "cannnot find ip, port, and logl in Proxy_Agent in .conf");

    args->proxy_ran_args.ip = malloc(strlen(ip) + 1);
    strcpy(args->proxy_ran_args.ip, ip);
    args->proxy_ran_args.port = port;
    args->proxy_ran_args.logl = logl;
    if (valid_logl(args->proxy_ran_args.logl) == false){
      printf("RAN loglevel invalid = %d. Check the config file\n", logl);
      printf("Allowed levels are a bitwose combination of\n");
      printf("LLL_ERR(%d), LLL_WARN(%d),LLL_NOTICE(%d), LLL_INFO(%d), LLL_DEBUG(%d), LLL_PARSER(%d), LLL_HEADER(%d), LLL_EXT(%d), LLL_CLIENT(%d), LLL_LATENCY(%d), LLL_USER(%d), LLL_THREAD(%d)\n", LLL_ERR, LLL_WARN, LLL_NOTICE, LLL_INFO, LLL_DEBUG, LLL_PARSER, LLL_HEADER, LLL_EXT, LLL_CLIENT, LLL_LATENCY, LLL_USER, LLL_THREAD);
      assert(0!=0);
    }
  }
}
#endif

fr_args_t init_fr_args(int argc, char* argv[])
{
  assert(argc > -1);

  fr_args_t args = {0};
  load_default_val(&args);
  if(argc > 1){
    assert(argc < 6 && "Only -h -c flags supported");
    assert(argv != NULL);
    parse_args(argc, argv, &args);
  }
  assert(is_regular_file(args.conf_file) == true && ".conf is not a file");

  printf("Setting the config -c file to %s\n",args.conf_file);
//  printf("Setting path -p for the shared libraries to %s\n",args.libs_dir);

  // Use libconf to parse .conf
  config_t cfg;
  config_init(&cfg);

  if (!config_read_file(&cfg, args.conf_file)) {
    fprintf(stderr, "Error reading configuration file: %s\n", config_error_text(&cfg));
    config_destroy(&cfg);
    assert(0!=0);
  }

  // SM_DIR
  get_SM_DIR(&args, cfg);
  printf("[LibConf]: loading service models from SM_DIR: %s\n", args.libs_dir);
  assert(is_directory(args.libs_dir) == true && "SM_DIR is not directory");

  // Name
  get_component_name(&args, cfg);
  printf("[LibConf]: reading configuration for %s\n", args.name);
  // NearRT_RIC_IP
  get_NearRT_RIC_IP(&args, cfg);
  printf("[LibConf]: NearRT_RIC IP: %s\n", args.ip);


  if (!strcasecmp(args.name, "NearRT_RIC")) {
    // E2_Port
    get_E2_Port(&args, cfg);
    printf("[LibConf]: E2_Port Port: %d\n", args.e2_port);
    // E42_Port
    get_E42_Port(&args, cfg);
    printf("[LibConf]: E42_Port Port: %d\n", args.e42_port);
  } else if (!strcasecmp(args.name, "E2_Agent")) {
    // E2_Port
    get_E2_Port(&args, cfg);
    printf("[LibConf]: E2_Port Port: %d\n", args.e2_port);
  } else if (!strcasecmp(args.name, "xApp")) {
    // E42_Port
    get_E42_Port(&args, cfg);
    printf("[LibConf]: E42_Port Port: %d\n", args.e42_port);

    // Sub_SM_List
    get_Sub_SM_List(&args, cfg);
    for (int32_t i = 0; i < args.sub_cust_sm_len; i++)
      printf("[LibConf]: Sub_CUST_SM Name: %s, Time: %s\n", args.sub_cust_sm[i].name, args.sub_cust_sm[i].time);
    for (int32_t i = 0; i < args.sub_oran_sm_len; i++) {
      printf("[LibConf]: Sub_ORAN_SM Name: %s, Time: %d\n", args.sub_oran_sm[i].name, args.sub_oran_sm[i].time);
      printf("[LibConf]: format %d, RAN type %s, actions = ", args.sub_oran_sm[i].format,  args.sub_oran_sm[i].ran_type);
      for (int32_t j = 0; j < args.sub_oran_sm[i].act_len - 1; j++)
        printf("%s ", args.sub_oran_sm[i].actions[j]);
      printf("\n");
    }
#if defined(SQLITE3_XAPP) ||  defined(MYSQL_XAPP)
    // xApp_DB
    get_xApp_DB(&args, cfg);
    if (args.db_params.enable)
      printf("[LibConf]: xApp_DB enable: %d, ip: %s, dir: %s, filename: %s\n", args.db_params.enable, args.db_params.ip,
             args.db_params.dir, args.db_params.filename);
    else
      printf("[LibConf]: xApp_DB enable: %d\n", args.db_params.enable);
#ifdef MYSQL_XAPP
    printf("[LibConf]: xApp_DB user: %s, pass: %s\n", args.db_params.user, args.db_params.pass);
#endif
#endif
  }
#ifdef PROXY_AGENT
  else if (!strcasecmp(args.name, "E2_Proxy_Agent")) {
    // E2_Port
    get_E2_Port(&args, cfg);
    printf("[LibConf]: E2_Port Port: %d\n", args.e2_port);
    // Proxy_Agent
    get_Proxy_Agent(&args, cfg);
    printf("[LibConf]: Proxy_Agent ip: %s, port: %d, logl: %d\n", args.proxy_ran_args.ip, args.proxy_ran_args.port, args.proxy_ran_args.logl);
  }
#endif
  else {
    assert(0!=0 && "unknown args.name");
  }

  config_destroy(&cfg);

  return args;
}

static
void free_db_params(db_params_t* params)
{
  free(params->ip);
  free(params->dir);
  free(params->filename);
#ifdef MYSQL_XAPP
  free(params->user);
  free(params->pass);
#endif
}

void free_fr_args(fr_args_t* args)
{
  assert(args != NULL);

  free(args->libs_dir);

  free(args->name);
  free(args->ip);
  for (int32_t i = 0; i < args->sub_cust_sm_len; i++) {
    free(args->sub_cust_sm[i].name);
    free(args->sub_cust_sm[i].time);
  }
  for (int32_t i = 0; i < args->sub_oran_sm_len; i++) {
    free(args->sub_oran_sm[i].name);
    free(args->sub_oran_sm[i].ran_type);
    for (int32_t j = 0; j < args->sub_oran_sm[i].act_len; j++)
      free(args->sub_oran_sm[i].actions[j]);
  }

  free_db_params(&args->db_params);
}