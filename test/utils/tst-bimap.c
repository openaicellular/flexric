#include <stdio.h>

#include "lib/ind_event.h"
#include "util/compare.h"
#include "util/alg_ds/ds/assoc_container/bimap.h"

static inline void free_ind_event(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  ind_event_t* ev = (ind_event_t* )value;
  free(ev);
}

static inline
void free_key(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  int* fd = (int* )value;
  free(fd);
}

static void init_indication_event(bi_map_t *ind_event)
{
  size_t key_sz_fd = sizeof(int);
  size_t key_sz_ind = sizeof(ind_event_t);

  bi_map_init(ind_event, key_sz_fd, key_sz_ind, cmp_fd, cmp_ind_event, free_ind_event, free_key);
}

int main(){
  int ret = 0;

  bi_map_t ind_event;
  init_indication_event(&ind_event);

  int timer_id = 147;
  ind_event_t ev = {
    .ric_id = {0},
    .sm = NULL,
    .action_id = 0
  };
  bi_map_insert(&ind_event, &timer_id, sizeof(timer_id), &ev, sizeof(ind_event_t));
  ind_event_t *ev_ext = bi_map_extract_left(&ind_event, &timer_id, sizeof(timer_id));
  if (memcmp(ev_ext, &ev, sizeof(ev))){
    printf("got ric_id=(%d, %d, %d), sm=%p, action_id =%d. Expected all zeros\n", 
            ev_ext->ric_id.ran_func_id,
            ev_ext->ric_id.ric_inst_id,
            ev_ext->ric_id.ric_req_id,
            ev_ext->sm,
            ev_ext->action_id);
    ret = 1;
  }
  free(ev_ext);
  bi_map_free(&ind_event);
  return ret;
}

