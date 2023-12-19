
#include "../../util/alg_ds/alg/defer.h"
#include "../../util/alg_ds/alg/find.h"
#include "../../util/alg_ds/alg/alg.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

#include "e42_iapp_api.h"
#include "e42_iapp.h"
#include "msg_handler_iapp.h"
#include "iapp_if_generic.h"
#include "xapp_ric_id.h"



#include "../../lib/async_event.h"
#include "../../lib/ep/sctp_msg.h"
#include "../../util/time_now_us.h"

#include <stdio.h>
#include <pthread.h>

e42_iapp_t* init_e42_iapp(const char* addr, int port, near_ric_if_t ric_if)
{
  assert(addr != NULL);
//  assert(ric != NULL);

  printf("[iApp]: Initializing ... \n");

  e42_iapp_t* iapp = calloc(1, sizeof(*iapp));
  assert(iapp != NULL && "Memory exhausted");

  iapp->ric_if = ric_if;

  // Emulator
  start_near_ric_iapp_gen(iapp->ric_if.type);

  printf("[iApp]: nearRT-RIC IP Address = %s, PORT = %d\n", addr, port);
  e2ap_init_ep_iapp(&iapp->ep, addr, port);

  init_asio_iapp(&iapp->io); 

  add_fd_asio_iapp(&iapp->io, iapp->ep.base.fd);

  assert(iapp->io.efd < 1024);

  init_ap(&iapp->ap.base.type);

  assert(iapp->io.efd < 1024);

  iapp->sz_handle_msg = sizeof(iapp->handle_msg)/sizeof(iapp->handle_msg[0]);
  init_handle_msg_iapp(iapp->sz_handle_msg, &iapp->handle_msg);

  assert(iapp->io.efd < 1024);

  init_reg_e2_node(&iapp->e2_nodes);

  assert(iapp->io.efd < 1024);

  init_map_ric_id(&iapp->map_ric_id);


  iapp->xapp_id = 7;

  iapp->stop_token = false;
  iapp->stopped = false;

  return iapp;
}


static inline
bool net_pkt(const e42_iapp_t* iapp, int fd)
{
  assert(iapp != NULL);
  assert(fd > 0);
  return fd == iapp->ep.base.fd;
}

static
void consume_fd(int fd)
{
  assert(fd > 0);
  uint64_t read_buf = 0;
  ssize_t const bytes = read(fd,&read_buf, sizeof(read_buf));
  assert(bytes == sizeof(read_buf));
}

static inline
bool pend_event(e42_iapp_t* iapp, int fd, pending_event_t** p_ev)
{
  assert(iapp != NULL);
  assert(fd > 0);
  assert(*p_ev == NULL);

  assert(0!=0 && "iApp does not have pending events");

/*
  assert(bi_map_size(&iapp->pending) == 1 );

  void* start_it = assoc_front(&iapp->pending.left);
  void* end_it = assoc_end(&iapp->pending.left);

  void* it = find_if(&iapp->pending.left,start_it, end_it, &fd, eq_fd);

  assert(it != end_it);
  *p_ev = assoc_value(&iapp->pending.left ,it);
  return *p_ev != NULL;
  */
  return false;
}

static
async_event_t next_async_event_iapp(e42_iapp_t* iapp)
{
  assert(iapp != NULL);

  int const fd = event_asio_iapp(&iapp->io);

  async_event_t e = {.type = UNKNOWN_EVENT,
                     .fd = fd};

  if(fd == -1){ // no event happened. Just for checking the stop_token condition
    e.type = CHECK_STOP_TOKEN_EVENT;
  } else if (net_pkt(iapp, fd) == true){
    e.msg = e2ap_recv_msg_iapp(&iapp->ep);
    if(e.msg.type == SCTP_MSG_NOTIFICATION){
      if(e.msg.notif->sn_header.sn_type == SCTP_ASSOC_CHANGE){
        printf(" SCTP_ASSOC_CHANGE recived \n" );
        e.type = UNKNOWN_EVENT;
        return e;
      }
      e.type = SCTP_CONNECTION_SHUTDOWN_EVENT;
    } else if (e.msg.type == SCTP_MSG_PAYLOAD){
       e.type = SCTP_MSG_ARRIVED_EVENT;
    } else {
      assert(0!=0 && "Unknown type");
    }
/*
  } else if(aind_event(iapp, fd, &e.ai_ev) == true) {
    e.type = APERIODIC_INDICATION_EVENT;

  } else if (ind_event(iapp, fd, &e.i_ev) == true) {
    e.type = INDICATION_EVENT;
    */
  } else if (pend_event(iapp, fd, &e.p_ev) == true){
    e.type = PENDING_EVENT;
  } else {
    assert(0!=0 && "Unknown event happened!");
  }
  return e;
}

static
void gen_e2ap_subs_delete(void const* it, void const* data)
{
  assert(it != NULL);
  assert(data != NULL);

  e2_node_ric_id_t* n = (e2_node_ric_id_t*)it;
  e42_iapp_t* iapp = (e42_iapp_t*)data;

  ric_subscription_delete_request_t dst = {.ric_id = n->ric_id};

  fwd_ric_subscription_request_delete_gen(iapp->ric_if.type, &n->e2_node_id, &dst, notify_msg_iapp_api);
}

static
void rm_if_pending_subs(e42_iapp_t* iapp, uint16_t xapp_id)
{
  assert(iapp != NULL);

  // array of e2_node_ric_id_t
  seq_arr_t arr = find_all_subs_map_ric_id(&iapp->map_ric_id, xapp_id);
  defer({ seq_arr_free(&arr, free_e2_node_ric_id_wrapper); } );

  if(seq_size(&arr) > 0){
    printf("[NEAR-RIC]: Automatically removing pending %lu subscription(s)\n",  seq_size(&arr));
  }

  void* f = seq_front(&arr);
  void* l = seq_end(&arr);
  void* data = iapp;
  for_each_arr(&arr, f, l, gen_e2ap_subs_delete, data);
}

static
void e2_event_loop_iapp(e42_iapp_t* iapp)
{
  assert(iapp != NULL);
  while(iapp->stop_token == false){

    async_event_t e = next_async_event_iapp(iapp);
    assert(e.type != UNKNOWN_EVENT && "Unknown event triggered ");

    switch(e.type){
      case SCTP_MSG_ARRIVED_EVENT:
        {
          defer({free_sctp_msg(&e.msg);});

          e2ap_msg_t msg = e2ap_msg_dec_iapp(&iapp->ap, e.msg.ba);
          defer( { e2ap_msg_free_iapp(&iapp->ap, &msg);} );

          e2ap_msg_t ans = e2ap_msg_handle_iapp(iapp, &msg);
          defer( { e2ap_msg_free_iapp(&iapp->ap, &ans);} );

          if(ans.type == E42_SETUP_RESPONSE){
            const uint16_t xapp_id = ans.u_msgs.e42_stp_resp.xapp_id;
            e2ap_reg_sock_addr_iapp(&iapp->ep, xapp_id, &e.msg.info);;
          }

          if(ans.type != NONE_E2_MSG_TYPE){
            sctp_msg_t sctp_msg = { .info.addr = e.msg.info.addr,
              .info.sri =  e.msg.info.sri,
            };

            sctp_msg.ba = e2ap_msg_enc_iapp(&iapp->ap, &ans);
            defer({ free_sctp_msg(&sctp_msg); } );

            e2ap_send_sctp_msg_iapp(&iapp->ep, &sctp_msg);

            if(ans.type == RIC_SUBSCRIPTION_DELETE_RESPONSE)
              printf("RIC_SUBSCRIPTION_DELETE_RESPONSE sent with size = %ld \n", sctp_msg.ba.len);
            if(ans.type == RIC_INDICATION){
              int64_t now = time_now_us();
              printf("Time diff at iapp after sending = %ld \n", now - msg.tstamp);
            }
          }
          break;
        }
      case PENDING_EVENT:
        {
          printf("[nearRT-RIC] Pending event timeout happened. Communication lost?\n");
          consume_fd(e.fd);
          break;
        }
      case SCTP_CONNECTION_SHUTDOWN_EVENT:
        {
          defer({free_sctp_msg(&e.msg);});
          uint16_t const xapp_id = find_map_xapps_xid(&iapp->ep.xapps, &e.msg.info);
          printf("[NEAR-RIC]: xApp %d disconnected!\n", xapp_id);
          rm_if_pending_subs(iapp, xapp_id);
          break;
        }
      case CHECK_STOP_TOKEN_EVENT:
        {
/*
       socklen_t opt_len = sizeof(struct sctp_status);
       struct sctp_status status = {0};
          //check status
          getsockopt(iapp->ep.base.fd, IPPROTO_SCTP, SCTP_STATUS, &status, &opt_len);
          printf("\nSCTP Status:\n--------\n");
    printf("assoc id  = %d\n", status.sstat_assoc_id);
    printf("state     = %d\n", status.sstat_state);
    printf("instrms   = %d\n", status.sstat_instrms);
    printf("outstrms  = %d\n--------\n\n", status.sstat_outstrms);
*/
          break;
        }
      default:
        {
          assert(0!=0 && "Unknown event happened");
          break;
        }
    }

  }

  e2ap_free_ep_iapp(&iapp->ep);

  iapp->stopped = true;
}


// Blocking call
void start_e42_iapp(e42_iapp_t* iapp)
{
  assert(iapp != NULL);
  assert(iapp->io. efd < 1024);

  e2_event_loop_iapp(iapp);
}

void free_e42_iapp(e42_iapp_t* iapp)
{
  assert( iapp != NULL);

  iapp->stop_token = true;
  while(iapp->stopped == false){
    sleep(1);
  }

  // Emulator
  //stop_near_ric_iapp_gen(iapp->ric_if.type);

  free_reg_e2_node(&iapp->e2_nodes);

  free_map_ric_id(&iapp->map_ric_id);

  free(iapp);
}


#ifdef E2AP_V1
void add_e2_node_iapp_v1(e42_iapp_t* i, global_e2_node_id_t* id, size_t len, ran_function_t const ran_func[len])
{
  assert(i != NULL);
  assert(id != NULL);
  assert(len > 0);
  assert(ran_func != NULL);

  add_reg_e2_node_v1(&i->e2_nodes,id, len, ran_func);
}
#else
void add_e2_node_iapp(e42_iapp_t* i, global_e2_node_id_t* id, size_t len, ran_function_t const ran_func[len], size_t len_cca, e2ap_node_component_config_add_t const* cca)
{
  assert(i != NULL);
  assert(id != NULL);
  assert(len > 0);
  assert(ran_func != NULL);

  add_reg_e2_node(&i->e2_nodes,id, len, ran_func, len_cca, cca);
}
#endif

void rm_e2_node_iapp(e42_iapp_t* i, global_e2_node_id_t* id)
{
  assert(i != NULL);
  assert(id != NULL);

  rm_reg_e2_node(&i->e2_nodes,id);

  e2ap_msg_t msg = {.type = E2_SETUP_RESPONSE};
  notify_msg_iapp(i, &msg);
}

void notify_msg_iapp(e42_iapp_t* iapp, e2ap_msg_t const* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_INDICATION 
      || msg->type == RIC_SUBSCRIPTION_RESPONSE 
      || msg->type == RIC_SUBSCRIPTION_DELETE_RESPONSE
      || msg->type == RIC_CONTROL_ACKNOWLEDGE
      || msg->type == E2_SETUP_RESPONSE);


  e2ap_msg_t ans = e2ap_msg_handle_iapp(iapp, msg);
  defer({ e2ap_msg_free_iapp(&iapp->ap, &ans); } );

  assert(ans.type ==  NONE_E2_MSG_TYPE );
}

