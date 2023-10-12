#ifndef FLEXRIC_CONFIGURATION_FILE_H
#define FLEXRIC_CONFIGURATION_FILE_H 

#include <stdint.h>
#include <stdbool.h>
#define FR_CONF_FILE_LEN 128
#define MAX_NUM_CUST_SM 6
#define MAX_NUM_ORAN_SM 8 // consider diff ran type

typedef struct {
    char* name;
    char* time;
} sub_cust_sm_t;

typedef struct {
    char* name;
    int32_t time;
    int32_t format;
    char* ran_type;
    int32_t act_len;
    char** actions;
} sub_oran_sm_t;

typedef struct{
    bool enable;
    char* ip;
    char* dir;
    char* filename;
    char* user;
    char* pass;
} db_params_t;

typedef struct {
    char* ip;
    int32_t port;
    int32_t logl;
} proxy_ran_args_t;

typedef struct {
  char conf_file[FR_CONF_FILE_LEN];
  char* libs_dir;

  char *name;
  char *ip;
  int32_t e2_port;
  int32_t e42_port;

  int32_t sub_cust_sm_len;
  sub_cust_sm_t sub_cust_sm[MAX_NUM_CUST_SM];
  int32_t sub_oran_sm_len;
  sub_oran_sm_t sub_oran_sm[MAX_NUM_ORAN_SM];

  db_params_t db_params;

  proxy_ran_args_t proxy_ran_args;
} fr_args_t;

fr_args_t init_fr_args(int argc, char* argv[]);

void free_fr_args(fr_args_t* args);

#endif

