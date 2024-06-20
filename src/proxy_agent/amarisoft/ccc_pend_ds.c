#include "ccc_pend_ds.h"
#include <assert.h>
#include "../../util/compare.h"
#include "../../util/alg_ds/alg/find.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

static
void free_func_ccc_sm_pend(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);
  (void)key;
  (void)value;
}

void init_ccc_pend_ds(ccc_pend_ds_t* src)
{
  assert(src != NULL);

  assoc_init(&src->t, sizeof(int), cmp_int, free_func_ccc_sm_pend);
  int rc = pthread_mutex_init(&src->mtx, NULL);
  assert(rc == 0);
}

void free_ccc_pend_ds(ccc_pend_ds_t* src)
{
  assert(src != NULL);

  assoc_free(&src->t);
  int rc = pthread_mutex_destroy(&src->mtx);
  assert(rc == 0);
}

void add_ccc_pend_ds(ccc_pend_ds_t* src, int msg_id, ccc_pend_msg_t* val)
{
  assert(src != NULL);
  assert(msg_id > -1);
  assert(val != NULL);

  lock_guard(&src->mtx);
  assoc_insert(&src->t, &msg_id, sizeof(msg_id), val);
}

ccc_pend_msg_t* extract_ccc_pend_ds(ccc_pend_ds_t* src, int msg_id)
{
  assert(src != NULL);
  assert(msg_id > -1);

  lock_guard(&src->mtx);
  void* f = assoc_rb_tree_front(&src->t);
  void* e = assoc_rb_tree_end(&src->t);
  void* it = find_if_rb_tree(&src->t, f, e, &msg_id, eq_int);
  assert(it != e);
  // k is non-owning pointer. Do nothing
  ccc_pend_msg_t* k = assoc_rb_tree_extract(&src->t, &msg_id);
  return k;
}

