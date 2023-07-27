#include "ringbuffer.h"
static ringbuffer_t ringbuffer;

ran_ind_t get_ringbuffer_data(void) 
{
  return ringbuffer.data[0];
}

void put_ringbuffer_data(ran_ind_t ind) 
{
  memcpy (&ringbuffer.data[0], &ind, sizeof(ind));
}
