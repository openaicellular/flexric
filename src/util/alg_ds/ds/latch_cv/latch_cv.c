#include "latch_cv.h"
#include "../lock_guard/lock_guard.h"
#include <assert.h>

latch_cv_t init_latch_cv(size_t cnt)
{
  assert(cnt > 0);
  
  latch_cv_t dst = {.cnt = cnt }; 
  
  const pthread_mutexattr_t * attr = NULL;
  int rc = pthread_mutex_init(&dst.mtx, attr);
  assert(rc == 0);

  const pthread_condattr_t *attr2 = NULL;
  rc = pthread_cond_init(&dst.cond, attr2);
  assert(rc == 0);

  return dst;
}

void count_down_latch_cv(latch_cv_t* l)
{
  assert(l != NULL);
 
  atomic_fetch_sub(&l->cnt, 1);
  if(l->cnt == 0){
    lock_guard(&l->mtx);
    pthread_cond_signal(&l->cond);
  }
}

void wait_latch_cv(latch_cv_t* l)
{
  assert(l != NULL);

  lock_guard(&l->mtx);
  while(l->cnt != 0){
    pthread_cond_wait(&l->cond, &l->mtx);
  }
}

void free_latch_cv(latch_cv_t* l)
{
  assert(l != NULL);

  int rc = pthread_cond_destroy(&l->cond);
  assert(rc == 0);

  rc = pthread_mutex_destroy(&l->mtx);
  assert(rc == 0);
}

