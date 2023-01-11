#ifndef RINGBUFFER_H 
#define RINGBUFFER_H 
#include "ws_msg_hdlr.h"
/* 
 * implementation of a ring buffer that stores indication messages created by WS interface when performing reading from RAN.
 * Ringbuffer will contain data service model agnostic (indication event internal representation: ws_ind_t)
 * XXX: It is probably already done by Mikel, I will try to reuse it.
 */
// in microseconds
#define RING_TIMER_TIMEOUT 1000 
typedef struct ringbuffer_t {
ws_ind_t data[1];
} ringbuffer_t;
ws_ind_t get_ringbuffer_data(void);
void put_ringbuffer_data(ws_ind_t ind);

#endif