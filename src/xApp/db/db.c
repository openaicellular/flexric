/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */
#include "mysql/mysql.h"
#include "db.h"
#include "db_generic.h"
#include "../../util/time_now_us.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct{
  global_e2_node_id_t id;
  sm_ag_if_rd_t rd;
} e2_node_ag_if_t;

static
e2_node_ag_if_t static_e2_node_ag_if; 

static
e2_node_ag_if_t static_e2_node_ag_if_10[10]; 

static
e2_node_ag_if_t static_e2_node_ag_if_100[100]; 



static
void* create_val(void* it)
{
  if(it == NULL)
    return NULL;

  memcpy(&static_e2_node_ag_if, it, sizeof(e2_node_ag_if_t));

  return &static_e2_node_ag_if;
}

static 
int val_10 = 0;

static
void* create_val_10(void* it)
{
  if(it == NULL)
    return NULL;

  memcpy(&static_e2_node_ag_if_10[val_10], it, sizeof(e2_node_ag_if_t));

  ++val_10;
  assert(val_10 < 11);
  return &static_e2_node_ag_if_10;
}

static 
int val_100 = 0;

static
void* create_val_100(void* it)
{
  if(it == NULL)
    return NULL;

  memcpy(&static_e2_node_ag_if_100[val_100], it, sizeof(e2_node_ag_if_t));

  ++val_100;
  assert(val_100 < 101);
  return &static_e2_node_ag_if_100;
}


static
void* worker_thread(void* arg)
{
  db_xapp_t* db = (db_xapp_t*)arg;

  while(true){
    e2_node_ag_if_t* data = NULL; 
    size_t sz = size_tsq(&db->q);
    //printf("Current size of the db = %ld \n", sz);
    if(sz > 100){
      sz = 100;
      val_100 = 0;
      data = pop_tsq_100(&db->q, create_val_100); 
    } else if(sz > 10){
      sz = 10;
      val_10 = 0;
      data = pop_tsq_10(&db->q, create_val_10); 
    } else{
      sz = 1;
      data = wait_and_pop_tsq(&db->q, create_val);
    }

    if(data == NULL)
        break;

    for(size_t i = 0; i < sz; ++i){
//      printf("i = %ld\n", i);
//      if(data[i].rd.type == MAC_STATS_V0)
//        printf("Try to write mac data in db\n");
//      else if(data[i].rd.type == RLC_STATS_V0)
//        printf("Try to write rlc data in db\n");
      write_db_mysql(db->handler, &data[i].id, &data[i].rd);

      free_global_e2_node_id(&data[i].id);
      free_sm_ag_if_rd(&data[i].rd);
    }
  }
  db->q.stopped = true;

  return NULL;
}

void mysql_finish_with_error(MYSQL *conn)
{
  fprintf(stderr, "%s\n", mysql_error(conn));
  mysql_close(conn);
  exit(1);
}

void init_db_xapp(db_xapp_t* db, char const* dir, char const* db_name)
{
  assert(db != NULL);
  assert(dir != NULL);
  assert(db_name != NULL);

#ifdef MYSQL_XAPP
  // MYSQL
  //MYSQL* conn = mysql_init(NULL);
  db->handler = mysql_init(NULL);
  // check init
  if (db->handler == NULL)
    mysql_finish_with_error(db->handler);
  // check connection with server
  static char* host = "localhost";
  static char* user = "root";
  static char* pass = "eurecom";
  if(mysql_real_connect(db->handler, host, user, pass, NULL, 0, NULL, 0) == NULL)
    mysql_finish_with_error(db->handler);
  printf("[MySQL]: Connection Successful\n");
  init_db_gen(db->handler, db_name);
#elif defined(SQLITE3_XAPP)
  //SQLite3
  char filename[256] = {0};
  if (strlen(dir) && strlen(db_name)) {
    int n = snprintf(filename, 255, "%s%s", dir, db_name);
    assert(n < 256 && "Overflow");
  }
  printf("Filename = %s \n ", filename);
  init_db_gen(&db->handler, filename);
#endif

  init_tsq(&db->q, sizeof(e2_node_ag_if_t));

  int rc = pthread_create(&db->p, NULL, worker_thread, db);
  assert(rc == 0);
}

static
void free_e2_node_ag_if_wrapper(void* it)
{
  assert(it != NULL);

  e2_node_ag_if_t* d = (e2_node_ag_if_t*)it;
  free_global_e2_node_id(&d->id);
  free_sm_ag_if_rd(&d->rd);
}


void close_db_xapp(db_xapp_t* db)
{
  assert(db != NULL);
  
  free_tsq(&db->q, free_e2_node_ag_if_wrapper);
  pthread_join(db->p, NULL);
  close_db_gen(db->handler);
}

void write_db_xapp(db_xapp_t* db, global_e2_node_id_t const* id, sm_ag_if_rd_t const* rd)
{
  assert(db != NULL);
  assert(rd != NULL);
  assert(id != NULL);

  e2_node_ag_if_t d = { .rd = cp_sm_ag_if_rd(rd) ,
                        .id = cp_global_e2_node_id(id) };
  //size_t sz = size_tsq(&db->q);
  //printf("Before push: Current size of the db = %ld \n", sz);
  push_tsq(&db->q, &d, sizeof(d) );
  //size_t sz2 = size_tsq(&db->q);
  //printf("After push: Current size of the db = %ld \n", sz2);
}

