#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "db_params.h"
#include "util/time_now_us.h"

void get_db_params(fr_args_t const* args, db_params_t* db_params)
{
  // printf("Set DB_ENABLE = %s, use DB for this xApp\n", db_enable);
  // Get DB directory
  db_params->dir = get_conf_db_dir(args);
  assert(strlen(db_params->dir) < 1024 && "String too large");
  if (!strlen(db_params->dir)) sprintf(db_params->dir, "/tmp/");
  printf("[MySQL/SQLite3]: use .conf DB dir: %s\n", db_params->dir);

  // Get DB name
  db_params->db_name = get_conf_db_name(args);
  assert(strlen(db_params->db_name) < 1024 && "String too large");
  if (!strlen(db_params->db_name)) sprintf(db_params->db_name, "xapp_db_%ld", time_now_us());
  printf("[MySQL/SQLite3]: use default DB name: %s\n", db_params->db_name);

  // Get DB ip address
  db_params->ip = get_conf_db_ip(args);
  assert(strlen(db_params->ip) < 24 && "String too large");
  if (!strlen(db_params->ip)) sprintf(db_params->ip, "localhost");
  printf("[MySQL/SQLite3]: use .conf DB ip: %s\n", db_params->ip);

# ifdef MYSQL_XAPP
  // Get DB username
    db_params->user = get_conf_db_user(args);
    assert(strlen(db_params->user) < 1024 && "DB-username too long");
    if (!strlen(db_params->user)) sprintf(db_params->user, "xapp");
    printf("[MySQL]: use .conf DB username: %s\n", db_params->user);

    // Get DB password
    db_params->pass = get_conf_db_pass(args);
    assert(strlen(db_params->pass) < 1024 && "DB-password too long");
    if (!strlen(db_params->pass)) sprintf(db_params->pass, "eurecom");
    printf("[MySQL]: use .conf DB password: %s\n", db_params->pass);
#endif
}

void free_db_params(db_params_t* params)
{
  free(params->ip);
  free(params->dir);
  free(params->db_name);
# ifdef MYSQL_XAPP
  free(params->user);
  free(params->pass);
#endif
}