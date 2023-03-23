#include "ringbuffer.h"
ringbuffer_t ringbuffer;


ws_ind_t get_ringbuffer_data(void) 
{
  return ringbuffer.data[0];
}
void put_ringbuffer_data(ws_ind_t ind) 
{
  memcpy (&ringbuffer.data[0], &ind, sizeof(ind));
}
