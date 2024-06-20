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


#include "map_xapps_sockaddr.h"

#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../util/alg_ds/alg/alg.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

static inline
void free_sctp_info(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  sctp_info_t* s = (sctp_info_t*)value;
  free(s);
}

static
int cmp_uint16_wrapper(void const* m0_v, void const* m1_v)
{
  assert(m0_v != NULL);
  assert(m1_v != NULL);

  uint16_t* m0 = (uint16_t*)m0_v;
  uint16_t* m1 = (uint16_t*)m1_v;

  if(*m0 < *m1) 
    return 1;

  if(*m0 > *m1) 
    return -1;

  return 0;
}

static
bool eq_uint16_wrapper(void const* m0_v, void const* m1_v)
{
  assert(m0_v != NULL);
  assert(m1_v != NULL);

  uint16_t* m0 = (uint16_t*)m0_v;
  uint16_t* m1 = (uint16_t*)m1_v;

  return *m0 == *m1;
}

static
void free_uin16_wrapper(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  uint16_t* xapp_id = (uint16_t*)value;
  free(xapp_id);
}

void init_map_xapps_sad(map_xapps_sockaddr_t* m)
{
  assert(m != NULL);


//  pthread_mutexattr_t *mtx_attr = NULL;
//#ifdef DEBUG
//  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK;
//#endif
//  int rc = pthread_mutex_init(&m->mtx, mtx_attr);
//  assert(rc == 0);


  pthread_rwlockattr_t attr = {0};
#ifdef DEBUG
  attr = PTHREAD_MUTEX_ERRORCHECK;
#endif
  int const rc = pthread_rwlock_init(&m->rw, &attr);
  assert(rc == 0);

  // left:  key1: uint16_t xapp_id | value: sctp_info_t
  // right: key2: sctp_info_t      | value:  uint16_t xapp_id
  const size_t key_sz_1 = sizeof(uint16_t);
  bi_map_cmp const cmp1 = cmp_uint16_wrapper;
  free_func_t const free1 = free_sctp_info;

  const size_t key_sz_2 = sizeof(sctp_info_t);
  bi_map_cmp const cmp2 = cmp_sctp_info_wrapper;
  free_func_t const free2 = free_uin16_wrapper;

  bi_map_init(&m->bimap , key_sz_1, key_sz_2, cmp1, cmp2, free1, free2);
//  assoc_init(.left, key1_sz, cmp_uint16_wrapper, free_sctp_info);
}

void free_map_xapps_sad(map_xapps_sockaddr_t* m)
{
  assert(m != NULL);

  int const rc = pthread_rwlock_destroy(&m->rw);
  assert(rc == 0);

  bi_map_free(&m->bimap);
  //assoc_free(&m->tree);
}

void add_map_xapps_sad(map_xapps_sockaddr_t* m, uint16_t xapp_id, sctp_info_t* s)
{
  assert(m != NULL);
  assert(s != NULL);

//  lock_guard(&m->mtx);
  int rc = pthread_rwlock_wrlock(&m->rw);
  assert(rc == 0);

  assoc_rb_tree_t* tree = &m->bimap.left;

  void* it = assoc_front(tree);
  void* end = assoc_end(tree);

  it = find_if(tree, it, end, &xapp_id, eq_uint16_wrapper);
  assert(it == end && "xApp ID already registered in the tree");

 // sctp_info_t* info = calloc(1, sizeof(sctp_info_t));
//  assert(info != NULL && "Memory exhausted");
 // *info = *s;

  sctp_info_t const info = *s;
  bi_map_insert(&m->bimap, &xapp_id, sizeof(uint16_t), &info, sizeof(sctp_info_t));

  //assoc_insert(&m->tree, &xapp_id, sizeof(uint16_t), info);

  rc = pthread_rwlock_unlock(&m->rw);
  assert(rc == 0);
}

void rm_map_xapps_sad(map_xapps_sockaddr_t* m, uint16_t xapp_id)
{
  assert(m != NULL);

  assert(0!=0 && "Not tested, it should work");

  int rc = pthread_rwlock_wrlock(&m->rw);
  assert(rc == 0);

  void (*free_sctp_info)(void*) = NULL;
  sctp_info_t* s = bi_map_extract_left(&m->bimap, &xapp_id, sizeof(uint16_t), free_sctp_info);
  free(s);

//  lock_guard(&m->mtx);
//  sctp_info_t* s = assoc_extract(&m->tree, &xapp_id);
//  free(s);

  rc = pthread_rwlock_unlock(&m->rw);
  assert(rc == 0);

}

sctp_info_t find_map_xapps_sad(map_xapps_sockaddr_t* m, uint16_t xapp_id)
{
  assert(m != NULL);

  //lock_guard(&m->mtx);

  int rc = pthread_rwlock_rdlock(&m->rw);
  assert(rc == 0);

  assoc_rb_tree_t* tree = &m->bimap.left;

  void* it = assoc_front(tree);
  void* end = assoc_end(tree);

  it = find_if(tree, it, end, &xapp_id, eq_uint16_wrapper);
  assert(it != end && "xApp ID not found in the tree");

  sctp_info_t s = *(sctp_info_t*)assoc_value(tree, it);

  rc = pthread_rwlock_unlock(&m->rw);
  assert(rc == 0);

  return s;
}

uint16_t find_map_xapps_xid(map_xapps_sockaddr_t* m, sctp_info_t const* s)
{
  assert(m != NULL);
  assert(s != NULL);

  int rc = pthread_rwlock_rdlock(&m->rw);
  assert(rc == 0);

  assoc_rb_tree_t* tree = &m->bimap.right;

  void* it = assoc_front(tree);
  void* end = assoc_end(tree);

  it = find_if(tree, it, end, s, eq_sctp_info_wrapper);
  assert(it != end && "SCTP info not found in the tree");

  uint16_t xapp_id = *(uint16_t*)assoc_value(tree, it);

  rc = pthread_rwlock_unlock(&m->rw);
  assert(rc == 0);

  return xapp_id;
}


size_t get_num_connected_xapps(map_xapps_sockaddr_t* m)
{
  assert(m != NULL);
  return assoc_rb_tree_size(&m->bimap.left);
}
