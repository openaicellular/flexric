/*
 * generic transport API for connecting agent to RAN
 */
#ifndef RAN_IF_H
#define RAN_IF_H

#include "util/alg_ds/ds/assoc_container/bimap.h"
#include "util/alg_ds/ds/tsn_queue/tsn_queue.h"

#include "io_ran.h"
#include "ser_ran.h"

/* 
 * Four modules compose the RAN interface: 
 *  - msg_handler, 
 *  - I/O, 
 *  - serialization, 
 *  - application logic (i.e KPM Service Model).
 * Communications from/to the RAN interface are: 
 * - notification mechanism with E2 interface 
 * - communication mean with storage for indication data to be perused by E2 interface (push only)
 * Msg_handler, application/SM logic and storage for indication data are not referenced as module resource from the RAN_IF 
 * for the moment; instead they will be called autonomously when needed.
 */
typedef struct ran_if_t
{
  /*  I/O module */
  io_ran_abs_t *io;
  
  /*  Serialization module */
  ran_ser_abs_t *ser;

  /* Communication mean for notification mechanism among RAN and E2 interface.
   * Implemented as a linkedlist for communicating with the I/O module from inside the I/O RAN module itself and from/to the outside world 
   * (i.e. from E2 interface) containing notif_ran_event_t messages
   */
  tsnq_t io_ran_notif_ds;

  /* 
   * Registered Indication events asked by different Service Models subscribed by the external E2 interface. 
   * Meaning of the keys for the bidirectional map is: {left_key:timer_id, right_key:ind_event_t }
   */
  bi_map_t ind_event; 
  
} ran_if_t;


#endif