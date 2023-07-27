/* RAN I/O generic interface*/
#ifndef IO_RAN_H
#define IO_RAN_H

#include <stdint.h>
#include <stddef.h>

#include "e2_sm_agent.h"
#include "util/alg_ds/ds/assoc_container/bimap.h"

// list of generic error codes returned by io_ran_abs_t interface
typedef enum ran_errcodes_t {
  ERR_RAN_IO_NONE = 0,
  ERR_RAN_INIT_FAILURE, // RAN IO module not correctly instanciated
  ERR_RAN_GEN_FAILURE,  // Generic failure
  ERR_RAN_CONN_FAILURE, 
  ERR_RAN_SEND_DATA
  // XXX: complete this with new SM requirements
} ran_errcodes_t;

// external declarations that will be resolved by implementors with a new definition in c files.
// XXX: io_ran_priv_data_t should be made private by design and opaque. 
typedef struct io_ran_priv_data_t io_ran_priv_data_t; 
typedef struct io_ran_conf_t      io_ran_conf_t;    

// forward declaration
typedef struct ws_ioloop_event_t  ws_ioloop_event_t;
typedef struct io_ran_abs_t       io_ran_abs_t; 

/* 
 * io_ran_get_instance() instanciates and initializes RAN I/O module memory etc. Implemented with singleton pattern.
 * param[in] conf       : I/O module configuration
 * param[in] set_read_cb: user defined synchronous read operation on a stream of data coming directly from the RAN. 
 *                        Data can be encoded (i.e. in json). Its polling timer in milliseconds is set by open() 
 *                        callback of the RAN interface. Params buf/len contain the retrieved data for further processing
 */
io_ran_abs_t *io_ran_get_instance(struct io_ran_conf_t conf,
                                  next_msg_t (*set_read_cb)(const char *buf, size_t len, bi_map_t *sent_msg_list));

// io_ran_abs_t: abstract interface for RAN I/O module
typedef struct io_ran_abs_t 
{   
  /* 
   * open the endpoint (i.e. file, socket connection etc..) source of the stream of data. If successfull, you can start 
   * receive `events` from the endpoint in an asynchronous way, that is, no via polling. The data received (normally, indication data), 
   * instead, will be retrieved by this I/O module at regular interval specified by MS parameter when a specific condition 
   * (subscription message received) appears at RAN interface. Other data can be received asynchronously in response to commands 
   * sent to the RAN (i.e. E2 Setup, CTRL message)
   */
  ran_errcodes_t (*open)(int ms);

  // write a message directly to the RAN. msg_type is a hint at the content that is on buf.
  ran_errcodes_t (*write_to_ran)(const ws_ioloop_event_t *ev, const char *buf, size_t len);

  void (*destroy)(void);

  // Send a notification to the I/O module from user
  ran_errcodes_t (*send_notif)();
  
  io_ran_priv_data_t *priv_data;

} io_ran_abs_t;

#endif