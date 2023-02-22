//#include "FlexRIC.h"
#include "swig_wrapper.h"

#ifdef XAPP_LANG_PYTHON
#include "Python.h"
#endif

#include "../e42_xapp_api.h"

#include "../../sm/mac_sm/mac_sm_id.h"
#include "../../sm/rlc_sm/rlc_sm_id.h"
#include "../../sm/pdcp_sm/pdcp_sm_id.h"
#include "../../sm/gtp_sm/gtp_sm_id.h"
#include "../../sm/kpm_sm_v2.02/kpm_sm_id.h"
#include "../../sm/slice_sm/slice_sm_id.h"

#include "../../util/conf_file.h"


#include <arpa/inet.h>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

static
bool initialized = false;

void init()
{
  assert(initialized == false && "Already initialized!");

  int const argc = 1;
  char** argv = NULL;
  fr_args_t args = init_fr_args(argc, argv);

  initialized = true; 

  init_xapp_api(&args);
}

bool try_stop()
{
  return try_stop_xapp_api();
}

std::vector<E2Node> conn_e2_nodes(void)
{
  e2_node_arr_t arr = e2_nodes_xapp_api();
   

  std::vector<E2Node> x; //(arr.len);

  for(int i = 0; i < arr.len; ++i){

    E2Node tmp;

    e2_node_connected_t const* src = &arr.n[i];
    tmp.id = cp_global_e2_node_id(&src->id); 

    std::vector<ran_function_t> ran_func;//(src->len_rf);

    for(size_t j = 0; j < src->len_rf; ++j){
      ran_function_t rf = cp_ran_function(&src->ack_rf[j]);
      ran_func.push_back(rf);// [j] = rf;
    }
    tmp.ran_func = ran_func;
    x.push_back(tmp);//[i] = tmp;
  }

  free_e2_node_arr(&arr);

  return x;
}

static 
mac_cb* hndlr_mac_cb; 

static
void sm_cb_mac(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == MAC_STATS_V0);
  assert(hndlr_mac_cb != NULL);

  mac_ind_data_t const* data = &rd->mac_stats; 

  swig_mac_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  for(uint32_t i = 0; i < data->msg.len_ue_stats; ++i){
      mac_ue_stats_impl_t tmp = cp_mac_ue_stats_impl(&data->msg.ue_stats[i]) ;
      ind.ue_stats.emplace_back(tmp);
  }

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

    hndlr_mac_cb->handle(&ind);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}

int report_mac_sm(global_e2_node_id_t* id, Interval inter_arg, mac_cb* handler)
{
  assert(id != NULL);
  assert(handler != NULL);

  hndlr_mac_cb = handler;

  inter_xapp_e i;
  if(inter_arg == Interval::ms_1 ){
    i = ms_1;
  } else if (inter_arg == Interval::ms_2) {
    i = ms_2;
  } else if(inter_arg == Interval::ms_5) {
    i = ms_5;
  } else if(inter_arg == Interval::ms_10) {
    i = ms_10;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  sm_ans_xapp_t ans = report_sm_xapp_api(id , SM_MAC_ID, i, sm_cb_mac);
  assert(ans.success == true); 
  return ans.u.handle;
}


void rm_report_mac_sm(int handle)
{

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

//  assert(hndlr_mac_ans.u.handle != 0);
  rm_report_sm_xapp_api(handle);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}


//////////////////////////////////////
// RLC SM   
/////////////////////////////////////

//static
//pthread_t t_rlc;

static 
rlc_cb* hndlr_rlc_cb; 

static
void sm_cb_rlc(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == RLC_STATS_V0);
  assert(hndlr_rlc_cb != NULL);

  rlc_ind_data_t const* data = &rd->rlc_stats; 

  swig_rlc_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  for(uint32_t i = 0; i < data->msg.len; ++i){
    rlc_radio_bearer_stats_t tmp = data->msg.rb[i];
    ind.rb_stats.push_back(tmp);
  }

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

    hndlr_rlc_cb->handle(&ind);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}

int report_rlc_sm(global_e2_node_id_t* id, Interval inter_arg, rlc_cb* handler)
{

  assert(id != NULL);
  assert(handler != NULL);

  hndlr_rlc_cb = handler;

  inter_xapp_e i;
  if(inter_arg == Interval::ms_1 ){
    i = ms_1;
  } else if (inter_arg == Interval::ms_2) {
    i = ms_2;
  } else if(inter_arg == Interval::ms_5) {
    i = ms_5;
  } else if(inter_arg == Interval::ms_10) {
    i = ms_10;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  sm_ans_xapp_t ans = report_sm_xapp_api(id , SM_RLC_ID, i, sm_cb_rlc);
  assert(ans.success == true); 
  return ans.u.handle;
}

void rm_report_rlc_sm(int handler)
{

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

  rm_report_sm_xapp_api(handler);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}



//////////////////////////////////////
// PDCP 
/////////////////////////////////////

static 
pdcp_cb* hndlr_pdcp_cb; 

static
void sm_cb_pdcp(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == PDCP_STATS_V0);
  assert(hndlr_pdcp_cb != NULL);

  pdcp_ind_data_t const* data = &rd->pdcp_stats; 

  swig_pdcp_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  for(uint32_t i = 0; i < data->msg.len; ++i){
    pdcp_radio_bearer_stats_t tmp = data->msg.rb[i];
    ind.rb_stats.push_back(tmp);
  }

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

    hndlr_pdcp_cb->handle(&ind);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}

int report_pdcp_sm(global_e2_node_id_t* id, Interval inter_arg, pdcp_cb* handler)
{
  assert(id != NULL);
  assert(handler != NULL);

  hndlr_pdcp_cb = handler;

  inter_xapp_e i;
  if(inter_arg == Interval::ms_1 ){
    i = ms_1;
  } else if (inter_arg == Interval::ms_2) {
    i = ms_2;
  } else if(inter_arg == Interval::ms_5) {
    i = ms_5;
  } else if(inter_arg == Interval::ms_10) {
    i = ms_10;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  sm_ans_xapp_t ans = report_sm_xapp_api(id , SM_PDCP_ID, i, sm_cb_pdcp);
  assert(ans.success == true); 
  return ans.u.handle;
}

void rm_report_pdcp_sm(int handler)
{

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

  rm_report_sm_xapp_api(handler);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}

//////////////////////////////////////
// SLICE Indication & Control
/////////////////////////////////////

static
slice_cb* hndlr_slice_cb;

static
void sm_cb_slice(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == SLICE_STATS_V0);
  assert(hndlr_slice_cb != NULL);

  slice_ind_data_t const* data = &rd->slice_stats;

  swig_slice_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;


  ind.slice_stats.dl.len_slices = data->msg.slice_conf.dl.len_slices;
  ind.slice_stats.dl.sched_name.push_back(data->msg.slice_conf.dl.sched_name);
  for (size_t i = 0; i < ind.slice_stats.dl.len_slices; ++i) {
    swig_fr_slice_t tmp;
    tmp.id = data->msg.slice_conf.dl.slices[i].id;
    tmp.label.push_back(data->msg.slice_conf.dl.slices[i].label);
    tmp.sched.push_back(data->msg.slice_conf.dl.slices[i].sched);
    tmp.params = data->msg.slice_conf.dl.slices[i].params;
    ind.slice_stats.dl.slices.emplace_back(tmp);
  }

  ind.ue_slice_stats.len_ue_slice = data->msg.ue_slice_conf.len_ue_slice;
  for (size_t i = 0; i < ind.ue_slice_stats.len_ue_slice; ++i) {
    ue_slice_assoc_t tmp_ue;
    tmp_ue.rnti = data->msg.ue_slice_conf.ues[i].rnti;
    tmp_ue.dl_id = data->msg.ue_slice_conf.ues[i].dl_id;
    tmp_ue.ul_id = data->msg.ue_slice_conf.ues[i].ul_id;
    ind.ue_slice_stats.ues.emplace_back(tmp_ue);
  }

#ifdef XAPP_LANG_PYTHON
  PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

  hndlr_slice_cb->handle(&ind);

#ifdef XAPP_LANG_PYTHON
  PyGILState_Release(gstate);
#endif

}

int report_slice_sm(global_e2_node_id_t* id, Interval inter_arg, slice_cb* handler)
{
  assert( id != NULL);
  (void)inter_arg;
  assert(handler != NULL);

  hndlr_slice_cb = handler;

  inter_xapp_e i;
  if(inter_arg == Interval::ms_1 ){
    i = ms_1;
  } else if (inter_arg == Interval::ms_2) {
    i = ms_2;
  } else if(inter_arg == Interval::ms_5) {
    i = ms_5;
  } else if(inter_arg == Interval::ms_10) {
    i = ms_10;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  sm_ans_xapp_t ans = report_sm_xapp_api(id , SM_SLICE_ID, i, sm_cb_slice);
  assert(ans.success == true);
  return ans.u.handle;
}

void rm_report_slice_sm(int handler)
{

#ifdef XAPP_LANG_PYTHON
  PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

  rm_report_sm_xapp_api(handler);

#ifdef XAPP_LANG_PYTHON
  PyGILState_Release(gstate);
#endif

}

void control_slice_sm(global_e2_node_id_t* id, slice_ctrl_msg_t* ctrl)
{
  assert(id != NULL);
  assert(ctrl != NULL);

  if(ctrl->type == SLICE_CTRL_SM_V0_ADD){
    slice_conf_t* s_conf = &ctrl->u.add_mod_slice;
    assert(s_conf->dl.sched_name != NULL);

    if (s_conf->dl.len_slices == 0)
      std::cout << "RESET DL SLICE, algo = NONE" << '\n';
    for(size_t i =0; i < s_conf->dl.len_slices; ++i) {
      fr_slice_t *s = &s_conf->dl.slices[i];
      assert(s->len_sched != 0);
      assert(s->sched != NULL);
      slice_params_t *p = &s->params;
      if (p->type == SLICE_ALG_SM_V0_STATIC) {
        static_slice_t *sta_sli = &p->u.sta;
        std::cout << "ADD STATIC DL SLICE: id " << s->id <<
                  ", label " << s->label <<
                  ", pos_low " << sta_sli->pos_low <<
                  ", pos_high " << sta_sli->pos_high << '\n';
      } else if (p->type == SLICE_ALG_SM_V0_NVS) {
        nvs_slice_t *nvs_sli = &p->u.nvs;
        if (nvs_sli->conf == SLICE_SM_NVS_V0_RATE)
          std::cout << "ADD NVS DL SLICE: id " << s->id <<
                    ", label " << s->label <<
                    ", conf " << nvs_sli->conf << "(rate)" <<
                    ", mbps_required " << nvs_sli->u.rate.u1.mbps_required <<
                    ", mbps_reference " << nvs_sli->u.rate.u2.mbps_reference << '\n';
        else if (nvs_sli->conf == SLICE_SM_NVS_V0_CAPACITY)
          std::cout << "ADD NVS DL SLICE: id " << s->id <<
                    ", label " << s->label <<
                    ", conf " << nvs_sli->conf << "(capacity)" <<
                    ", pct_reserved " << nvs_sli->u.capacity.u.pct_reserved << '\n';
        else assert(0 != 0 && "Unknow NVS conf");
      } else if (p->type == SLICE_ALG_SM_V0_EDF) {
        edf_slice_t *edf_sli = &p->u.edf;
        std::cout << "ADD EDF DL SLICE: id " << s->id <<
                  ", label " << s->label <<
                  ", deadline " << edf_sli->deadline <<
                  ", guaranteed_prbs " << edf_sli->guaranteed_prbs <<
                  ", max_replenish " << edf_sli->max_replenish << '\n';
      } else assert(0 != 0 && "Unknow slice algo type");
    }
  } else if(ctrl->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
    for (size_t i = 0; i <  ctrl->u.ue_slice.len_ue_slice; ++i)
      std::cout << "ASSOC DL SLICE: rnti " << std::hex << ctrl->u.ue_slice.ues[i].rnti <<
                ", to slice id " << ctrl->u.ue_slice.ues[i].dl_id << '\n';
  } else if (ctrl->type == SLICE_CTRL_SM_V0_DEL) {
    del_slice_conf_t* d_conf = &ctrl->u.del_slice;
    for (size_t i = 0; i <  d_conf->len_dl; ++i)
      std::cout << "DEL DL SLICE: id " << d_conf->dl[i] << '\n';
    // TODO: UL

  } else {
    assert(0!=0 && "not foreseen case");
  }

  sm_ag_if_wr_t wr;
  wr.type = SLICE_CTRL_REQ_V0;
  wr.slice_req_ctrl.msg = cp_slice_ctrl_msg(ctrl);

  control_sm_xapp_api(id, SM_SLICE_ID,  &wr);
}

//////////////////////////////////////
// GTP SM   
/////////////////////////////////////

static 
gtp_cb* hndlr_gtp_cb; 

static
void sm_cb_gtp(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == GTP_STATS_V0);
  assert(hndlr_gtp_cb != NULL);

  gtp_ind_data_t const* data = &rd->gtp_stats; 

  swig_gtp_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  for(uint32_t i = 0; i < data->msg.len; ++i){
    gtp_ngu_t_stats_t tmp = data->msg.ngut[i];
    ind.gtp_stats.push_back(tmp);
  }

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

    hndlr_gtp_cb->handle(&ind);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}

int report_gtp_sm(global_e2_node_id_t* id, Interval inter_arg, gtp_cb* handler)
{
  assert(id != NULL);
  assert(handler != NULL);

  hndlr_gtp_cb = handler;

  inter_xapp_e i;
  if(inter_arg == Interval::ms_1 ){
    i = ms_1;
  } else if (inter_arg == Interval::ms_2) {
    i = ms_2;
  } else if(inter_arg == Interval::ms_5) {
    i = ms_5;
  } else if(inter_arg == Interval::ms_10) {
    i = ms_10;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  sm_ans_xapp_t ans = report_sm_xapp_api(id , SM_GTP_ID, i, sm_cb_gtp);
  assert(ans.success == true); 
  return ans.u.handle;
}

void rm_report_gtp_sm(int handler)
{

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

  rm_report_sm_xapp_api(handler);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}

//////////////////////////////////////
// KPM
/////////////////////////////////////

static
kpm_cb* hndlr_kpm_cb;

static

void sm_cb_kpm(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == KPM_STATS_V0);
  assert(hndlr_kpm_cb != NULL);

  kpm_ind_data_t const* data = &rd->kpm_stats;
  swig_kpm_ind_md_t ind;
  ind.tstamp.collectStartTime = data->hdr.collectStartTime;
  ind.kpm_stats.MeasData.measRecord_len=data->msg.MeasData->measRecord_len;
  ind.kpm_stats.MeasInfo_len=data->msg.MeasInfo_len;


 
for(size_t i = 0; i <data->msg.MeasInfo_len; i++){
  

    char str1[sizeof(data->msg.MeasInfo[i].measName.buf)+1];
    char str2[15];
    size_t bt = sizeof(data->msg.MeasInfo[i].measName);
    memcpy(str1,data->msg.MeasInfo[i].measName.buf, bt);
    strcpy(str2, str1);
    ind.kpm_stats.MeasInfo.measName.append(str2);
    ind.kpm_stats.MeasInfo.measName.append(" ");

} 

 for(uint32_t i = 0; i < data->msg.MeasData->measRecord_len; ++i){
    adapter_MeasRecord_t tmp = data->msg.MeasData->measRecord[i];
    ind.kpm_stats.MeasData.MeasRecord.push_back(tmp);
  }



#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

    hndlr_kpm_cb->handle(&ind);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif
}

int report_kpm_sm(global_e2_node_id_t* id, Interval inter_arg, kpm_cb* handler)
{

  assert(id != NULL);
  assert(handler != NULL);

  hndlr_kpm_cb = handler;

  inter_xapp_e i;
  if(inter_arg == Interval::ms_1 ){
    i = ms_1;
  } else if (inter_arg == Interval::ms_2) {
    i = ms_2;
  } else if(inter_arg == Interval::ms_5) {
    i = ms_5;
  } else if(inter_arg == Interval::ms_10) {
    i = ms_10;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  sm_ans_xapp_t ans = report_sm_xapp_api(id , SM_KPM_ID, i, sm_cb_kpm);
  assert(ans.success == true);
  return ans.u.handle;

}

void rm_report_kpm_sm(int handler)
{

#ifdef XAPP_LANG_PYTHON
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
#endif

  rm_report_sm_xapp_api(handler);

#ifdef XAPP_LANG_PYTHON
    PyGILState_Release(gstate);
#endif

}
