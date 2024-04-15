#include "pending_event_proxy.h"
#include <assert.h>
#include "../../util/compare.h"
#include "../../util/alg_ds/alg/find.h"

#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

typedef struct{
  int fd;
  int msg_id;
  pending_event_t p;
} fd_pending_event_t;

static
void free_fd_pending_event(void* key, void* val)
{
  assert(key != NULL);
  (void)key;
  
  free(val);
}

void init_pend_ev_prox(pend_ev_prox_t *p)
{
  assert(p != NULL);

  size_t key_sz = sizeof(int);
  assoc_init(&p->tree_msg_id, key_sz, cmp_int, free_fd_pending_event );
  assoc_init(&p->tree_fd, key_sz, cmp_fd, free_fd_pending_event );

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  int rc = pthread_mutex_init(&p->mtx_mi, mtx_attr);
  assert(rc == 0);
  rc = pthread_mutex_init(&p->mtx_fd, mtx_attr);
  assert(rc == 0);
}

void free_pend_ev_prox(pend_ev_prox_t* p)
{
  assert(p != NULL);

  assoc_free(&p->tree_fd);
  assoc_free(&p->tree_msg_id);

  int rc = pthread_mutex_destroy(&p->mtx_mi);
  assert(rc == 0);
  rc = pthread_mutex_destroy(&p->mtx_fd);
  assert(rc == 0);
}

void add_pend_ev_prox(pend_ev_prox_t *p, asio_agent_amr_t *io, int msg_id, pending_event_t ev) 
{
  assert(p != NULL);

  long const wait_ms = 1000;
  int fd = create_timer_ms_asio_agent_amr(io, wait_ms, wait_ms);

  fd_pending_event_t* val0 = calloc(1, sizeof(fd_pending_event_t)); 
  assert(val0 != NULL && "Memory exhausted");

  val0->fd = fd;
  val0->msg_id = msg_id;
  val0->p = ev;

  fd_pending_event_t* val1 = calloc(1, sizeof(fd_pending_event_t)); 
  assert(val1 != NULL && "Memory exhausted");

  val1->fd = fd;
  val1->msg_id = msg_id;
  val1->p = ev;

  {
  lock_guard(&p->mtx_fd);
  assoc_rb_tree_insert(&p->tree_fd, &fd, sizeof(int), val0);
  }
  {
  lock_guard(&p->mtx_mi);
  assoc_rb_tree_insert(&p->tree_msg_id, &msg_id, sizeof(int), val1);
  }
}


void rm_pend_ev_prox(pend_ev_prox_t* p, asio_agent_amr_t* io, int msg_id)
{
  assert(p != NULL);
  fd_pending_event_t* fp = NULL; 
  {
    lock_guard(&p->mtx_mi);
    fp = assoc_rb_tree_extract(&p->tree_msg_id, &msg_id); 
  }

  fd_pending_event_t* fp2 = NULL; 
  {
    lock_guard(&p->mtx_fd);
    fp2 = assoc_rb_tree_extract(&p->tree_fd, &fp->fd); 
  } 

  // Clean the asio
  rm_fd_asio_agent_amr(io, fp->fd);

  free(fp);
  free(fp2);
}

exp_msg_id_t find_pend_ev_prox(pend_ev_prox_t* p, int fd)
{
  exp_msg_id_t dst = {.found = false}; 
  {
    lock_guard(&p->mtx_fd);
    void* f = assoc_front(&p->tree_fd);
    void* l = assoc_end(&p->tree_fd);
    void* it = find_if_rb_tree(&p->tree_fd, f, l, &fd, eq_fd);
    if(it != l){
      dst.found = true;
      fd_pending_event_t* tmp = assoc_value(&p->tree_fd,it);
      dst.msg_id = tmp->msg_id;
    }
  }
  return dst;
}

