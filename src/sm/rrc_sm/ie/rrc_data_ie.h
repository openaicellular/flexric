#ifndef RRC_DATA_INFORMATION_ELEMENTS_H
#define RRC_DATA_INFORMATION_ELEMENTS_H

/*
 * 9 Information Elements (IE) , RIC Event Trigger Definition, RIC Action Definition, RIC Indication Header, RIC Indication Message, RIC Call Process ID, RIC Control Header, RIC Control Message, RIC Control Outcome and RAN Function Definition defined by ORAN-WG3.E2SM-v01.00.00 at Section 5
 */

#include <stdbool.h>
#include <stdint.h>


//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

typedef struct {
  uint32_t ms;
} rrc_event_trigger_t;

void free_rrc_event_trigger(rrc_event_trigger_t* src); 

rrc_event_trigger_t cp_rrc_event_trigger( rrc_event_trigger_t* src);

bool eq_rrc_event_trigger(rrc_event_trigger_t* m0, rrc_event_trigger_t* m1);

//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

typedef struct {
  uint32_t dummy;  
} rrc_action_def_t;

void free_rrc_action_def(rrc_action_def_t* src); 

rrc_action_def_t cp_rrc_action_def(rrc_action_def_t* src);

bool eq_rrc_action_def(rrc_event_trigger_t* m0,  rrc_event_trigger_t* m1);

//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////

typedef struct{
  uint32_t dummy;  
} rrc_ind_hdr_t;

void free_rrc_ind_hdr(rrc_ind_hdr_t* src); 

rrc_ind_hdr_t cp_rrc_ind_hdr(rrc_ind_hdr_t* src);

bool eq_rrc_ind_hdr(rrc_ind_hdr_t* m0, rrc_ind_hdr_t* m1);

//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////

typedef struct{
  uint32_t rnti;
  uint8_t  rbid;
  uint8_t  DRB_active[8];
  uint8_t  StatusRrc;
  uint8_t  setup_pdu_session; /* Total number of pdu session already setup in the list */
  uint8_t  nb_of_pdusessions; /* Number of pdu session to be setup in the list */
  uint8_t  ue_rrc_inactivity_timer;
} rrc_ue_stats_impl_t;

rrc_ue_stats_impl_t cp_rrc_ue_stats_impl(rrc_ue_stats_impl_t const* src);

typedef struct {
  uint32_t len_ue_stats;
  rrc_ue_stats_impl_t* ue_stats; 
  int64_t tstamp;
} rrc_ind_msg_t;

void free_rrc_ind_msg(rrc_ind_msg_t* msg);

rrc_ind_msg_t cp_rrc_ind_msg(rrc_ind_msg_t* src);

bool eq_rrc_ind_msg(rrc_ind_msg_t* m0, rrc_ind_msg_t* m1); 

//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

typedef struct {
  uint32_t dummy;
} rrc_call_proc_id_t;

void free_rrc_call_proc_id( rrc_call_proc_id_t* src); 

rrc_call_proc_id_t cp_rrc_call_proc_id( rrc_call_proc_id_t* src);

bool eq_rrc_call_proc_id(rrc_call_proc_id_t* m0, rrc_call_proc_id_t* m1);


//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////


typedef struct {
  uint32_t dummy;
} rrc_ctrl_hdr_t;

void free_rrc_ctrl_hdr( rrc_ctrl_hdr_t* src); 

rrc_ctrl_hdr_t cp_rrc_ctrl_hdr(rrc_ctrl_hdr_t* src);

bool eq_rrc_ctrl_hdr(rrc_ctrl_hdr_t* m0, rrc_ctrl_hdr_t* m1);

//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////

typedef struct {
  uint32_t action;
} rrc_ctrl_msg_t;

void free_rrc_ctrl_msg( rrc_ctrl_msg_t* src); 

rrc_ctrl_msg_t cp_rrc_ctrl_msg(rrc_ctrl_msg_t* src);

bool eq_rrc_ctrl_msg(rrc_ctrl_msg_t* m0, rrc_ctrl_msg_t* m1);

//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

typedef enum{
  RRC_CTRL_OUT_OK,


  RRC_CTRL_OUT_END
} rrc_ctrl_out_e;

typedef struct {
  rrc_ctrl_out_e ans;  
} rrc_ctrl_out_t;

void free_rrc_ctrl_out(rrc_ctrl_out_t* src); 

rrc_ctrl_out_t cp_rrc_ctrl_out(rrc_ctrl_out_t* src);

bool eq_rrc_ctrl_out(rrc_ctrl_out_t* m0, rrc_ctrl_out_t* m1);


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

typedef struct {
  uint32_t dummy;
} rrc_func_def_t;

void free_rrc_func_def( rrc_func_def_t* src); 

rrc_func_def_t cp_rrc_func_def(rrc_func_def_t* src);

bool eq_rrc_func_def(rrc_func_def_t* m0, rrc_func_def_t* m1);

/////////////////////////////////////////////////
//////////////////////////////////////////////////
/////////////////////////////////////////////////


/*
 * O-RAN defined 5 Procedures: RIC Subscription, RIC Indication, RIC Control, E2 Setup and RIC Service Update 
 * */


///////////////
/// RIC Subscription
///////////////

typedef struct{
  rrc_event_trigger_t et; 
  rrc_action_def_t* ad;
} rrc_sub_data_t;


///////////////
// RIC Indication
///////////////

typedef struct{
  rrc_ind_hdr_t hdr;
  rrc_ind_msg_t msg;
  rrc_call_proc_id_t* proc_id;
} rrc_ind_data_t;

///////////////
// RIC Control
///////////////

typedef struct{
  rrc_ctrl_hdr_t hdr;
  rrc_ctrl_msg_t msg;
} rrc_ctrl_req_data_t;

typedef struct{
  rrc_ctrl_out_t* out;
} rrc_ctrl_out_data_t;

///////////////
// E2 Setup
///////////////

typedef struct{
  rrc_func_def_t func_def;
} rrc_e2_setup_data_t;

///////////////
// RIC Service Update
///////////////

typedef struct{
  rrc_func_def_t func_def;
} rrc_ric_service_update_t;

#endif

