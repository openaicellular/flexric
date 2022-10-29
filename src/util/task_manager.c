#include "task_manager.h"

#include <assert.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>


//////////////////////////////////////////////
//////////////////////////////////////////////
////////////////  Seq Ring  //////////
//////////////////////////////////////////////
//////////////////////////////////////////////

typedef struct seq_ring_buf_s
{
  const size_t elt_size;
  uint8_t* array;

  size_t cap;
  uint32_t head;
  uint32_t tail;
} seq_ring_t;

// For working correctly, maintain the default elements to a multiple of 2
#define DEFAULT_ELM 32768 

inline static
uint32_t mask(uint32_t cap, uint32_t val)
{
  return val & (cap-1);
}

inline static
size_t seq_ring_size(seq_ring_t* r)
{
  return r->head - r->tail; 
};

static inline 
bool full(seq_ring_t* r)
{
  return seq_ring_size(r) == r->cap -1;
}

static
void enlarge_buffer(seq_ring_t* r)
{
  assert(r != NULL);
  assert(full(r));

  const uint32_t factor = 2;
  uint8_t* tmp_buffer = calloc(r->cap * factor, r->elt_size);
  assert(tmp_buffer != NULL);

  const uint32_t head_pos = mask(r->cap, r->head);
  const uint32_t tail_pos = mask(r->cap, r->tail);

  if(head_pos > tail_pos){
    memcpy(tmp_buffer, r->array + tail_pos*r->elt_size , (head_pos-tail_pos)*r->elt_size );
  } else {
    memcpy(tmp_buffer, r->array + tail_pos*r->elt_size , (r->cap-tail_pos)*r->elt_size);
    memcpy(tmp_buffer + (r->cap-tail_pos)*r->elt_size, r->array, head_pos*r->elt_size);
  }
  r->cap *= factor;
  free(r->array);
  r->array = tmp_buffer;
  r->tail = 0;
  r->head = r->cap/2 - 1;
}

static
void seq_ring_init(seq_ring_t* r, size_t elt_size)
{
  assert(r != NULL);
  uint8_t* tmp_buffer = calloc(DEFAULT_ELM, elt_size); 
  assert(tmp_buffer != NULL);
  seq_ring_t tmp = {.elt_size = elt_size, .array = tmp_buffer, .head = 0, .tail = 0, .cap = DEFAULT_ELM};
  memcpy(r, &tmp, sizeof(seq_ring_t));

}

static
void* seq_ring_front(seq_ring_t* r)
{
  return r->array + mask(r->cap, r->tail)*r->elt_size;
};

static
void* seq_ring_next(seq_ring_t* r, void* it)
{
  assert(r != NULL);
  assert(it != NULL);
  const uint32_t pos = ((uint8_t*)it - r->array)/r->elt_size;

  if(pos + 1 == r->cap) 
    return r->array;
  return it + r->elt_size;
}

static inline
void* seq_ring_end(seq_ring_t* r)
{
  return r->array + mask(r->cap, r->head)*r->elt_size;
}

static inline
void* seq_ring_at(seq_ring_t* r, uint32_t pos)
{
  return r->array + mask(r->cap, r->tail + pos)*r->elt_size;
}

static inline
int32_t seq_ring_dist(seq_ring_t* r, void* first, void* last)
{
  // Precondition: random access iterators belonging to a sequence.
  assert(r != NULL);
  assert(first != NULL);
  assert(last != NULL);

  const uint32_t last_pos = ((uint8_t*)last - r->array) / r->elt_size; 
  const uint32_t first_pos = ((uint8_t*)first - r->array) / r->elt_size; 
  if(last_pos > first_pos)
    return last_pos - first_pos; 

  return (r->cap - first_pos) + last_pos;
};

static inline
bool seq_ring_equal(void* it_1,void* it_2)
{
  return (uint8_t*)it_1 == (uint8_t*)it_2;  
}

typedef void (*seq_free_func)(void*);

static
void seq_ring_free(seq_ring_t* cb, seq_free_func fp)
{
  assert(cb != NULL);
  if(fp != NULL){
    void* start_it = seq_ring_front(cb);
    void* end_it = seq_ring_end(cb);
    while(start_it != end_it){
      fp(start_it);
      start_it = seq_ring_next(cb, start_it);
    }
  }

  free(cb->array);
}

static
void seq_ring_push_back(seq_ring_t* r, uint8_t* data, size_t len)
{
  assert(r != NULL);
  assert(len == r->elt_size);

  if(full(r))
    enlarge_buffer(r);
  
  const uint32_t pos = mask(r->cap, r->head);
  memcpy(r->array + r->elt_size*pos, data, r->elt_size);
  r->head += 1;
};

static
void seq_ring_erase(seq_ring_t* r, void* it_start, void* it_end)
{
  assert(it_start == r->array + mask(r->cap,r->tail)*r->elt_size && "Only deletions from tail implemented");
  if(it_start == it_end){
    return; // closed semi-open range [ ) 
  }
  const uint32_t start_pos = ((uint8_t*)it_start - r->array)/r->elt_size; 
  const uint32_t end_pos = ((uint8_t*)it_end - r->array)/r->elt_size; 
  assert(end_pos <= r->cap);
  assert(end_pos != start_pos);

  if(end_pos > start_pos){
    memset(it_start, 0, (end_pos - start_pos)*r->elt_size);
    r->tail += end_pos - start_pos;
  } else {
    assert(end_pos < start_pos); 
    memset(it_start, 0, (r->cap - start_pos)*r->elt_size);
    memset(r->array, 0, end_pos*r->elt_size);
    r->tail +=  r->cap - start_pos + end_pos ;
  }
  assert(mask(r->cap, r->tail) == end_pos);
}

static
void seq_ring_move(seq_ring_t* r, void* it, void* end, void* out)
{
  assert(it == r->array + mask(r->cap,r->tail)*r->elt_size && "Only deletions from tail implemented");
  if(it == end){
    return; // closed semi-open range [ ) 
  }
  const uint32_t start_pos = ((uint8_t*)it - r->array)/r->elt_size; 
  const uint32_t end_pos = ((uint8_t*)end - r->array)/r->elt_size; 
  assert(end_pos <= r->cap);
  assert(end_pos != start_pos);

  if(end_pos > start_pos){
    memcpy(out, it, (end_pos - start_pos)*r->elt_size);
    memset(it, 0, (end_pos - start_pos)*r->elt_size);
    r->tail += end_pos - start_pos;
  } else {
    assert(end_pos < start_pos); 
    memcpy(out, it, (r->cap - start_pos)*r->elt_size); 
    memset(it, 0, (r->cap - start_pos)*r->elt_size);

    memcpy(out, r->array, end_pos*r->elt_size); // (r->cap - start_pos)*r->elt_size); 
    memset(r->array, 0, end_pos*r->elt_size);
    r->tail +=  r->cap - start_pos + end_pos ;
  }
  assert(mask(r->cap, r->tail) == end_pos);
}


static
void seq_ring_swap(seq_ring_t* cb, void* it_1, void* it_2)
{
  assert(0!=0 && "Not implemented");
  assert(cb != NULL);
  assert(it_1 != NULL);
  assert(it_2 != NULL);
};


//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////// End Seq Ring  //////////
//////////////////////////////////////////////
//////////////////////////////////////////////




//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////// Notification Queue //////////
//////////////////////////////////////////////
//////////////////////////////////////////////

typedef struct {
  pthread_mutex_t mtx;
  pthread_cond_t cv;
  seq_ring_t r;
  int done;
} not_q_t;

typedef struct{
  task_t t[4096];
  int len;
  bool success;
} ret_try_t;

static
void init_not_q(not_q_t* q)
{
  assert(q != NULL);

  q->done = 0;
  seq_ring_init(&q->r, sizeof(task_t));

  pthread_mutexattr_t attr = {0};
#ifdef _DEBUG
  int const rc_mtx = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  assert(rc_mtx == 0);
#endif
  int rc = pthread_mutex_init(&q->mtx, &attr);
  assert(rc == 0 && "Error while creating the mtx");

  pthread_condattr_t* c_attr = NULL; 
  rc = pthread_cond_init(&q->cv, c_attr);
  assert(rc == 0);
}

static
void free_not_q(not_q_t* q, void (*clean)(void*) )
{
  assert(q != NULL);
  assert(q->done == 1);

  seq_ring_free(&q->r, clean);

  int rc = pthread_mutex_destroy(&q->mtx);
  assert(rc == 0);

  rc = pthread_cond_destroy(&q->cv);
  assert(rc == 0);
}

static
bool try_push_not_q(not_q_t* q, task_t t)
{
  assert(q != NULL);
  assert(q->done == 0 || q->done ==1);
  assert(t.func != NULL);
  assert(t.args != NULL);

  if(pthread_mutex_trylock(&q->mtx ) != 0)
    return false;

  seq_ring_push_back(&q->r, (uint8_t*)&t, sizeof(task_t));

  int rc = pthread_mutex_unlock(&q->mtx);
  assert(rc == 0);

  pthread_cond_signal(&q->cv);

  return true;
}

static
void push_not_q(not_q_t* q, task_t t)
{
  assert(q != NULL);
  assert(q->done == 0 || q->done ==1);
  assert(t.func != NULL);
  //assert(t.args != NULL);

  int rc = pthread_mutex_lock(&q->mtx);
  assert(rc == 0);

  seq_ring_push_back(&q->r, (void*)&t, sizeof(task_t));

  pthread_mutex_unlock(&q->mtx);

  pthread_cond_signal(&q->cv);
}

static
ret_try_t try_pop_not_q(not_q_t* q)
{
  assert(q != NULL);

  ret_try_t  ret = {.success = false}; 

  int rc = pthread_mutex_trylock(&q->mtx);
  assert(rc == 0 || rc == EBUSY);

  if(rc == EBUSY)
    return ret;

  assert(q->done == 0 || q->done ==1);

  size_t sz = seq_ring_size(&q->r); 
  if(sz == 0){
    rc = pthread_mutex_unlock(&q->mtx);
    assert(rc == 0);

    return ret;
  }

  ret.len = sz < 4096 ? sz : 4096;
  for(int i = 0; i < ret.len; ++i){
  void* it = seq_ring_at(&q->r, i);
    memcpy(&ret.t[i], it, sizeof(task_t)); 
  }

//   void* it = seq_ring_front(&q->r);
//    assert(it != seq_ring_end(&q->r));
//    memcpy(&ret.t, it, sizeof(task_t)); 

  seq_ring_erase(&q->r, seq_ring_front(&q->r), seq_ring_at(&q->r, ret.len) );


//  seq_ring_move(&q->r, seq_ring_front(&q->r), seq_ring_at(&q->r, ret.len), ret.t);

  assert(sz == ret.len + seq_ring_size(&q->r)); 

  rc = pthread_mutex_unlock(&q->mtx);
  assert(rc == 0);
  ret.success = true; 

  return ret;
}

static
bool pop_not_q(not_q_t* q, ret_try_t* out)
{
  assert(q != NULL);
  assert(out != NULL);

  pthread_mutex_lock(&q->mtx);
  assert(q->done == 0 || q->done ==1);

  while(seq_ring_size(&q->r) == 0 && q->done == 0)
    pthread_cond_wait(&q->cv , &q->mtx);

  if(q->done == 1){
    int rc = pthread_mutex_unlock(&q->mtx);
    assert(rc == 0);
    return false;
  }

  const size_t sz =  seq_ring_size(&q->r);

  out->len = sz < 4096 ? sz : 4096; 
  for(int i =0; i < out->len; ++i){
     void* it = seq_ring_at(&q->r, i);
    assert(it != seq_ring_end(&q->r));
    memcpy(&out->t[i], it, sizeof(task_t)); 
  }

//   void* it = seq_ring_at(&q->r, 0);
//    assert(it != seq_ring_end(&q->r));
//    memcpy(&out->t, it, sizeof(task_t)); 

  seq_ring_erase(&q->r, seq_ring_front(&q->r), seq_ring_at(&q->r, out->len));

//  seq_ring_move(&q->r, seq_ring_front(&q->r), seq_ring_at(&q->r, out->len), out->t);


  int rc = pthread_mutex_unlock(&q->mtx);
  assert(rc == 0);

  return true;
}

static
void done_not_q(not_q_t* q)
{
  assert(q != NULL);

  int rc = pthread_mutex_lock(&q->mtx);
  assert(rc == 0);
  
  q->done = 1;

  rc = pthread_mutex_unlock(&q->mtx);
  assert(rc == 0);

  rc = pthread_cond_signal(&q->cv);
  assert(rc == 0);
}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////// End Notification Queue //////////
//////////////////////////////////////////////
//////////////////////////////////////////////



//////////////////////////////////////////////
//////////////////////////////////////////////
/////////// Task Manager /////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////


typedef struct{
  task_manager_t* man;
  int idx;
} task_thread_args_t;

static
void* worker_thread(void* arg)
{
  assert(arg != NULL);

  task_thread_args_t* args = (task_thread_args_t*)arg; 
  int const idx = args->idx;
  task_manager_t* man = args->man;

  uint32_t const len = man->len_thr;
  int const num_it = 3*(man->len_thr + idx); 

  not_q_t* q_arr = (not_q_t*)man->q_arr;
  ret_try_t ret; 
  for(;;){
    for(int i = idx; i < num_it; ++i){
      ret = try_pop_not_q(&q_arr[i%len]);
      if(ret.success == true){
        break;
      } 
    }

    if(ret.success == false && pop_not_q(&q_arr[idx], &ret) == false)
      break;
    
    for(int i =0; i < ret.len; ++i )
      ret.t[i].func(ret.t[i].args); 
  }

  free(args);
  return NULL;
}

void init_task_manager(task_manager_t* man, uint32_t num_threads)
{
  assert(man != NULL);
  assert(num_threads > 0 && num_threads < 33 && "Do you have zero or more than 32 processors??");

  man->q_arr = calloc(num_threads, sizeof(not_q_t));
  assert(man->q_arr != NULL && "Memory exhausted");
   
  not_q_t* q_arr = (not_q_t*)man->q_arr;

  for(uint32_t i = 0; i < num_threads; ++i){
    init_not_q(&q_arr[i]);   
  }

  man->t_arr = calloc(num_threads, sizeof(pthread_t));
  assert(man->t_arr != NULL && "Memory exhausted" );
  man->len_thr = num_threads;

  for(uint32_t i = 0; i < num_threads; ++i){
    task_thread_args_t* args = malloc(sizeof(task_thread_args_t) ); 
    args->idx = i;
    args->man = man;

    int rc = pthread_create(&man->t_arr[i], NULL, worker_thread, args);
    assert(rc == 0);
  }

  man->index = 0;
}

void free_task_manager(task_manager_t* man, void (*clean)(void*))
{
  not_q_t* q_arr = (not_q_t*)man->q_arr;
  for(uint32_t i = 0; i < man->len_thr; ++i){
    done_not_q(&q_arr[i]);
  }

  for(uint32_t i = 0; i < man->len_thr; ++i){
    pthread_join(man->t_arr[i], NULL); 
  }

  for(uint32_t i = 0; i < man->len_thr; ++i){
    free_not_q(&q_arr[i], clean); 
  }

  free(man->q_arr);

  free(man->t_arr);
}

void async_task_manager(task_manager_t* man, task_t t)
{
  assert(man != NULL);
  assert(t.func != NULL);
  //assert(t.args != NULL);

  uint64_t const index = man->index++;
//  atomic_fetch_add_explicit(&man->index, 1, memory_order_relaxed);

  not_q_t* q_arr = (not_q_t*)man->q_arr;
  for(uint32_t i = 0; i < man->len_thr; ++i){
    if(try_push_not_q(&q_arr[(i+index) % man->len_thr], t)){
      return;
    }
  }

  push_not_q(&q_arr[index%man->len_thr], t);
}

