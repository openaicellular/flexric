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


#include "reg_e2_nodes.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../util/alg_ds/alg/find.h"
#include "../../util/alg_ds/alg/alg.h" 
#include "../../util/compare.h"
#include "../../xApp/e2_node_arr_xapp.h"
#include "../../lib/e2ap/e2ap_global_node_id_wrapper.h"


#include <assert.h>
#include <pthread.h>

#ifdef E2AP_V1
#elif defined (E2AP_V2) || defined(E2AP_V3)
static inline
void free_e2ap_node_component_config_add_it(void* it)
{
  assert(it != NULL);
  e2ap_node_component_config_add_t* cca  = (e2ap_node_component_config_add_t*)it;
  free_e2ap_node_component_config_add(cca);
}
#endif


void free_pair_rf_cca(pair_rf_cca_t* src)
{
  assert(src != NULL);

  seq_arr_t* ran_func = &src->ran_func;
  seq_free_func f = free_ran_function_wrapper; 
  seq_arr_free(ran_func, f);

#ifdef E2AP_V1
#elif defined (E2AP_V2) || defined(E2AP_V3)
  seq_arr_t* cca = &src->comp_conf_add;
  seq_free_func f2 = free_e2ap_node_component_config_add_it; 
  seq_arr_free(cca, f2);
#endif
}

static inline
void free_e2_nodes(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  // Key
  global_e2_node_id_t* e2node = (global_e2_node_id_t*) key;
  free_global_e2_node_id(e2node);

  // Value
  pair_rf_cca_t* pair = (pair_rf_cca_t*)value;
  free_pair_rf_cca(pair);
  free(pair);
}


void init_reg_e2_node(reg_e2_nodes_t* i)
{
  assert(i != NULL);

  assoc_init(&i->node_to_rf, sizeof(global_e2_node_id_t), cmp_global_e2_node_id_wrapper, free_e2_nodes);

  pthread_mutexattr_t* attr = NULL;
#ifdef DEBUG
  *attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  int const rc = pthread_mutex_init(&i->mtx, attr);
  assert(rc == 0);
}

void free_reg_e2_node(reg_e2_nodes_t* i)
{
  assert(i != NULL);
  assoc_free(&i->node_to_rf);

  int const rc = pthread_mutex_destroy(&i->mtx);
  assert(rc == 0);
}

#ifdef E2AP_V1
void add_reg_e2_node_v1(reg_e2_nodes_t* i, global_e2_node_id_t const* id, size_t len_rf, ran_function_t const* ran_func)
{
  assert(i != NULL);
  assert(id != NULL);
  assert(len_rf > 0);
  assert(ran_func != NULL);

  pair_rf_cca_t* rf_cca = calloc(1, sizeof(pair_rf_cca_t));
  assert(rf_cca != NULL && "memory exhausted");

  // RAN Function
  seq_arr_t* arr_rf = &rf_cca->ran_func; 
  seq_init(arr_rf, sizeof(ran_function_t));

  for(size_t i = 0; i < len_rf; ++i){
    ran_function_t tmp = cp_ran_function(&ran_func[i]);
    seq_push_back(arr_rf, &tmp, sizeof(ran_function_t));
  }

  lock_guard(&i->mtx);

  if(assoc_size(&i->node_to_rf) > 0){
    void* it_node = assoc_front(&i->node_to_rf);
    void* end_node = assoc_end(&i->node_to_rf);

    it_node = find_if(&i->node_to_rf, it_node, end_node, id, eq_global_e2_node_id_wrapper );
    assert(it_node == end_node && "Trying to add an already existing E2 Node");
  }

  global_e2_node_id_t cp_id = cp_global_e2_node_id(id); 
  assoc_insert(&i->node_to_rf, &cp_id, sizeof(global_e2_node_id_t), rf_cca);
}
#elif defined (E2AP_V2) || defined(E2AP_V3)
void add_reg_e2_node(reg_e2_nodes_t* i, global_e2_node_id_t const* id, size_t len_rf, ran_function_t const* ran_func, size_t len_cca, e2ap_node_component_config_add_t const* cca)
{
  assert(i != NULL);
  assert(id != NULL);
  assert(len_rf > 0);
  assert(ran_func != NULL);
  assert(len_cca > 0);
  assert(cca != NULL);

  pair_rf_cca_t* rf_cca = calloc(1, sizeof(pair_rf_cca_t));
  assert(rf_cca != NULL && "memory exhausted");

  // RAN Function
  seq_arr_t* arr_rf = &rf_cca->ran_func; 
  seq_init(arr_rf, sizeof(ran_function_t));

  for(size_t i = 0; i < len_rf; ++i){
    ran_function_t tmp = cp_ran_function(&ran_func[i]);
    seq_push_back(arr_rf, &tmp, sizeof(ran_function_t));
  }

  // Component configuration Add
  seq_arr_t* arr_cca = &rf_cca->comp_conf_add; 
  seq_init(arr_cca, sizeof(e2ap_node_component_config_add_t));
  for(size_t i = 0; i < len_cca; ++i){
    e2ap_node_component_config_add_t tmp = cp_e2ap_node_component_config_add(&cca[i]);  
    seq_push_back(arr_cca, &tmp, sizeof(e2ap_node_component_config_add_t));
  }

  lock_guard(&i->mtx);

  if(assoc_size(&i->node_to_rf) > 0){
    void* it_node = assoc_front(&i->node_to_rf);
    void* end_node = assoc_end(&i->node_to_rf);

    it_node = find_if(&i->node_to_rf, it_node, end_node, id, eq_global_e2_node_id_wrapper );
    assert(it_node == end_node && "Trying to add an already existing E2 Node");
  }

//  if(it_node != end_node) {
//    seq_arr_t* old_arr = assoc_extract(&i->node_to_rf,(global_e2_node_id_t*)id);
//    seq_free(old_arr, free_ran_function_wrapper);
//    free(old_arr);
//  }

  global_e2_node_id_t cp_id = cp_global_e2_node_id(id); 
  assoc_insert(&i->node_to_rf, &cp_id, sizeof(global_e2_node_id_t), rf_cca);

//  void* it_n = assoc_front(&i->node_to_rf);
//  seq_arr_t* arr_tmp = assoc_value(&i->node_to_rf, it_n); 
//  assert(seq_size(arr_tmp) == 2 && "Only for current test valid");
}

#endif

size_t sz_reg_e2_node(reg_e2_nodes_t* n)
{
  assert(n != NULL);

  lock_guard(&n->mtx);
  return assoc_size(&n->node_to_rf);
}


assoc_rb_tree_t cp_reg_e2_node(reg_e2_nodes_t* n)
{
  assert(n != 0);

  // key:global_e2_node_id_t | value: seq_arr_t* of ran_function_t
  assoc_rb_tree_t ans = {0};
  assoc_init(&ans, sizeof(global_e2_node_id_t), cmp_global_e2_node_id_wrapper, free_e2_nodes);

  lock_guard(&n->mtx);

  void* it_node = assoc_front(&n->node_to_rf);
  void* end_node = assoc_end(&n->node_to_rf);

  while(it_node != end_node){
    global_e2_node_id_t* tmp = assoc_key(&n->node_to_rf, it_node );
    global_e2_node_id_t cp_tmp = cp_global_e2_node_id(tmp);

    pair_rf_cca_t* rf_cca = assoc_value(&n->node_to_rf, it_node);
    pair_rf_cca_t* new_rf_cca = calloc(1, sizeof(pair_rf_cca_t));
    assert(new_rf_cca != NULL);

    seq_init(&new_rf_cca->ran_func, sizeof(ran_function_t));
    seq_arr_t* src_rf = &rf_cca->ran_func;
    seq_arr_t* dst_rf = &new_rf_cca->ran_func;

    for(void* it = seq_front(src_rf); it != seq_end(src_rf); it = seq_next(src_rf,it)  ){
      ran_function_t const* r = (ran_function_t const*)it; 
      ran_function_t cp = cp_ran_function(r);
      seq_push_back(dst_rf, &cp, sizeof(ran_function_t));
    }

#ifdef E2AP_V1
#elif defined (E2AP_V2) || defined(E2AP_V3)
    seq_init(&new_rf_cca->comp_conf_add, sizeof(e2ap_node_component_config_add_t));
    seq_arr_t* src_cca = &rf_cca->comp_conf_add;
    seq_arr_t* dst_cca = &new_rf_cca->comp_conf_add;

    for(void* it = seq_front(src_cca); it != seq_end(src_cca); it = seq_next(src_cca,it)  ){
      e2ap_node_component_config_add_t const* cca = (e2ap_node_component_config_add_t*)it; 
      e2ap_node_component_config_add_t cp = cp_e2ap_node_component_config_add(cca);
      seq_push_back(dst_cca, &cp, sizeof(e2ap_node_component_config_add_t));
    }
#endif


/*
    seq_arr_t* arr = assoc_value(&n->node_to_rf, it);
    seq_arr_t* new_arr = calloc(1, sizeof(seq_arr_t)); 
    assert(new_arr != NULL);
    seq_init(new_arr, sizeof(ran_function_t*));
    void* it_arr = seq_front(arr);
    void* end_arr = seq_end(arr);
    while(it_arr != end_arr){
      ran_function_t* r = *(ran_function_t**)it_arr; 
      ran_function_t* tmp_r = calloc(1, sizeof(ran_function_t));
      assert(tmp_r != NULL);
      *tmp_r = cp_ran_function(r);
      seq_push_back(new_arr, &tmp_r, sizeof(ran_function_t*));

      it_arr = seq_next(arr, it_arr);
    }
*/

    assoc_insert(&ans, &cp_tmp, sizeof(global_e2_node_id_t), new_rf_cca);

    it_node = assoc_next(&n->node_to_rf, it_node);
  }

  assert(assoc_size(&ans) == assoc_size(&n->node_to_rf) );

  return ans;
}

// This sucks
static
sm_ran_function_def_t mv_rd_e2_setup(sm_ag_if_rd_e2setup_t const* src)
{
  assert(src != NULL);
  sm_ran_function_def_t dst = {0};

  if(src->type == MAC_AGENT_IF_E2_SETUP_ANS_V0 ){
    dst.type = MAC_RAN_FUNC_DEF_E; 
    dst.mac = src->mac.func_def;
  } else if(src->type == RLC_AGENT_IF_E2_SETUP_ANS_V0){
    dst.type = RLC_RAN_FUNC_DEF_E; 
    dst.rlc = src->rlc.func_def;
  } else if(src->type == PDCP_AGENT_IF_E2_SETUP_ANS_V0){
    dst.type = PDCP_RAN_FUNC_DEF_E; 
    dst.pdcp = src->pdcp.func_def;
  } else if(src->type == SLICE_AGENT_IF_E2_SETUP_ANS_V0){
    dst.type = SLICE_RAN_FUNC_DEF_E; 
    dst.slice = src->slice.func_def;
  } else if(src->type == TC_AGENT_IF_E2_SETUP_ANS_V0){
    dst.type = TC_RAN_FUNC_DEF_E; 
    dst.tc = src->tc.func_def;
  } else if(src->type == GTP_AGENT_IF_E2_SETUP_ANS_V0){
    dst.type = GTP_RAN_FUNC_DEF_E; 
    dst.gtp = src->gtp.func_def;
  } else if(src->type == KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0){
    dst.type = KPM_RAN_FUNC_DEF_E ; 
    dst.kpm = src->kpm.ran_func_def;
  } else if(src->type == RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0){
    dst.type = RC_RAN_FUNC_DEF_E; 
    dst.rc = src->rc.ran_func_def;
  } else {
    assert(0!=0 && "Unknown types");
  }

  return dst;
}

static
sm_ran_function_t dec_ran_func(ran_function_t const* rf_c, plugin_ric_t const* plg_ric_c)
{
  assert(rf_c != NULL);
  assert(plg_ric_c != NULL);

  sm_ran_function_t dst = {.id = rf_c->id, .rev = rf_c->rev };
#ifdef E2AP_V1
  if(rf_c->oid != NULL){
    dst.oid = calloc(1, sizeof(byte_array_t));
    assert(dst.oid != NULL && "Memory exhausted");
    *dst.oid = copy_byte_array(*rf_c->oid);
  }
#elif defined(E2AP_V2) || defined( E2AP_V3)
  dst.oid = copy_byte_array(rf_c->oid);
#endif

  ran_function_t* rf = (ran_function_t*)rf_c;
  plugin_ric_t* plg_ric = (plugin_ric_t*)plg_ric_c;

  void *f = assoc_front(&plg_ric->sm_ds);
  void *l = assoc_end(&plg_ric->sm_ds);
  // key: ran_func_id, value: sm_ric_t*
  void *it = find_if(&plg_ric->sm_ds, f, l, &rf->id, eq_ran_func_id);
  assert(it != l && "RAN Function not found");

  sm_ric_t* sm = assoc_value(&plg_ric->sm_ds, it); 

  sm_e2_setup_data_t const e2_setup_data = {.len_rfd = rf->defn.len, .ran_fun_def = rf->defn.buf }; 
  sm_ag_if_rd_e2setup_t const stp = sm->proc.on_e2_setup(sm, &e2_setup_data);

  // Move ownership
  dst.defn = mv_rd_e2_setup(&stp);

  return dst;
}

#if defined(E2AP_V2) || defined(E2AP_V3)
 
typedef struct{
  e2ap_node_component_config_add_t* cca;
  size_t len;
} span_cca_t;

span_cca_t cp_cca(seq_arr_t* cca_arr)
{
  assert(cca_arr != NULL);
  span_cca_t dst = {0};

  assert(cca_arr->elt_size == sizeof(e2ap_node_component_config_add_t));
  const size_t sz_cca = seq_size(cca_arr);
  assert(sz_cca > 0 && sz_cca < 256);
  dst.len = sz_cca;
  if(sz_cca > 0){
    dst.cca = calloc(sz_cca, sizeof(e2ap_node_component_config_add_t));
    assert(dst.cca != NULL && "memory exhausted");
  }
  for(size_t j = 0; j < sz_cca; ++j){
    e2ap_node_component_config_add_t* c = (e2ap_node_component_config_add_t*)seq_at(cca_arr, j);
    dst.cca[j] = cp_e2ap_node_component_config_add(c);
  }
  return dst;
}

#endif

e2_node_arr_t generate_e2_node_arr(reg_e2_nodes_t* n)
{ 
  assoc_rb_tree_t t = cp_reg_e2_node(n);
  defer({ assoc_free(&t); }; );

  e2_node_arr_t dst = {0};
  dst.len = assoc_size(&t);

  if(dst.len > 0){
    dst.n = calloc(dst.len, sizeof(e2_node_connected_t) );
    assert(dst.n != NULL && "Memory exhausted" );
  }

  uint32_t i = 0;
  void* it = assoc_front(&t);
  void* end = assoc_end(&t);
  while(it != end){

    e2_node_connected_t* n = &dst.n[i];

    global_e2_node_id_t* tmp_id = assoc_key(&t, it);        
    n->id = cp_global_e2_node_id(tmp_id);

    pair_rf_cca_t* rf_cca = assoc_value(&t, it);

#ifdef E2AP_V1
#elif defined(E2AP_V2) || defined(E2AP_V3)
    span_cca_t span = cp_cca(&rf_cca->comp_conf_add);
    n->cca = span.cca;
    n->len_cca = span.len;
#else
    static_assert(0 != 0, "Unknown E2AP version");
#endif

    {
      seq_arr_t* rf_arr = &rf_cca->ran_func; 
      assert(rf_arr->elt_size == sizeof(ran_function_t));

      const size_t sz = seq_size(rf_arr);
      n->len_rf = sz;
      if(sz > 0){
        n->ack_rf = calloc(sz, sizeof(ran_function_t));
        assert(n->ack_rf != NULL && "memory exhausted");
      }

      for(size_t j = 0; j < sz; ++j){
        ran_function_t const* r = (ran_function_t*)seq_at(rf_arr, j);
        n->ack_rf[j] = cp_ran_function(r);
      }
    }

    i += 1;
    it = assoc_next(&t, it);
  }

  return dst;
}

e2_node_arr_xapp_t generate_e2_node_arr_xapp(reg_e2_nodes_t* n, plugin_ric_t const* plg_ric)
{ 
  assoc_rb_tree_t t = cp_reg_e2_node(n);
  defer({ assoc_free(&t); }; );

  e2_node_arr_xapp_t dst = {0};
  dst.len = assoc_size(&t);

  if(dst.len > 0){
    dst.n = calloc(dst.len, sizeof(e2_node_connected_xapp_t));
    assert(dst.n != NULL && "Memory exhausted" );
  }

  uint32_t i = 0;
  void* it = assoc_front(&t);
  void* end = assoc_end(&t);
  while(it != end){

    e2_node_connected_xapp_t* n = &dst.n[i];

    global_e2_node_id_t* tmp_id = assoc_key(&t, it);        
    n->id = cp_global_e2_node_id(tmp_id);

    pair_rf_cca_t* rf_cca = assoc_value(&t, it);

#ifdef E2AP_V1
#elif defined(E2AP_V2) || defined(E2AP_V3)
    span_cca_t span = cp_cca(&rf_cca->comp_conf_add);
    n->cca = span.cca;
    n->len_cca = span.len;
#else
    static_assert(0 != 0, "Unknown E2AP version");
#endif

    {
      seq_arr_t* rf_arr = &rf_cca->ran_func; 
      assert(rf_arr->elt_size == sizeof(ran_function_t));

      const size_t sz = seq_size(rf_arr);
      n->len_rf = sz;
      if(sz > 0){
        n->rf = calloc(sz, sizeof(sm_ran_function_t));
        assert(n->rf != NULL && "memory exhausted");
      }

      for(size_t j = 0; j < sz; ++j){
        ran_function_t const* r = (ran_function_t*)seq_at(rf_arr, j);
        n->rf[j] = dec_ran_func(r, plg_ric);
      }
    }

    i += 1;
    it = assoc_next(&t, it);
  }

  return dst;
}

void rm_reg_e2_node(reg_e2_nodes_t* n, global_e2_node_id_t const* id)
{
  assert(n != NULL);
  assert(id != NULL);

  printf("[NEAR-RIC]: Removing E2 Node MCC %d MNC %d NB_ID %u \n", id->plmn.mcc, id->plmn.mnc, id->nb_id.nb_id);

  {
    lock_guard(&n->mtx);

    void* it = assoc_front(&n->node_to_rf);
    void* end = assoc_end(&n->node_to_rf);
    it = find_if(&n->node_to_rf, it, end, id, eq_global_e2_node_id_wrapper );
    assert(it != end && "Not registed e2 Node passed");

    // Remove the iterator, calling the free function passed when init the rb  
    assoc_rb_tree_free_it(&n->node_to_rf, it);
  }
}

