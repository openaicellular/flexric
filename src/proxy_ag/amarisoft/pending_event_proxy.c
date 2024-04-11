#include "pending_event_proxy.h"
#include <assert.h>
#include "../../util/compare.h"

#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

static
void free_func_pending_ev(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;
  int* v = (int*)value;
  free(v);
}

void init_pend_ev_prox(pend_ev_prox_t *p)
{
  assert(p != NULL);

  size_t const key_sz = sizeof(int);

  assoc_init(&p->tree, key_sz, cmp_int, free_func_pending_ev);

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  int const rc = pthread_mutex_init(&p->mtx, mtx_attr);
  assert(rc == 0);
}

void free_pend_ev_prox(pend_ev_prox_t* p)
{
  assert(p != NULL);

  assoc_free(&p->tree);

  int const rc = pthread_mutex_destroy(&p->mtx);
  assert(rc == 0);
}

void add_pend_ev_prox(pend_ev_prox_t* p, int msg_id, pending_event_t ev)
{
  assert(p != NULL);

  pending_event_t* ev_heap = calloc(1, sizeof(pending_event_t));
  assert(ev_heap != NULL && "Memory exhausted");
  *ev_heap = ev; 

  lock_guard(&p->mtx);
  assoc_insert(&p->tree, &msg_id, sizeof(msg_id), ev_heap);
}

void rm_pend_ev_prox(pend_ev_prox_t* p, int msg_id)
{
  assert(p != NULL);
  pending_event_t* v = NULL; 
 {
    lock_guard(&p->mtx);
    v = assoc_extract(&p->tree, &msg_id);
 }
 assert(v != NULL);
 free(v);
}

