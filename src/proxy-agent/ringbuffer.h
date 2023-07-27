#ifndef RINGBUFFER_H 
#define RINGBUFFER_H 
/* 
 * implementation of a ring buffer that stores indication messages created by RAN interface when performing reading from RAN endpoint.
 * Ringbuffer will contain data service model agnostic (indication event internal representation: ran_ind_t)
 */

#include "ran_msg_hdlr.h"

typedef struct ringbuffer_t {
  ran_ind_t data[1];
} ringbuffer_t;

ran_ind_t get_ringbuffer_data(void);

void put_ringbuffer_data(ran_ind_t ind);

#endif