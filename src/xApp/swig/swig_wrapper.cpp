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
#include "../../sm/slice_sm/slice_sm_id.h"
#include "../../sm/kpm_sm/kpm_sm_id_wrapper.h"
#include "../../util/conf_file.h"


#include <arpa/inet.h>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <libconfig.h++>

static
bool initialized = false;

std::string ByteArrayToString(const byte_array_t* ba) {
  if (ba && ba->buf && ba->len > 0) {
    return std::string(reinterpret_cast<char*>(ba->buf), ba->len);
  } else {
    return "";
  }
}

static
const char* convert_period_to_char(Interval  inter_arg)
{
  if(inter_arg == Interval::ms_1 ){
    return "1_ms";
  } else if (inter_arg == Interval::ms_2) {
    return "2_ms";
  } else if(inter_arg == Interval::ms_5) {
    return "5_ms";
  } else if(inter_arg == Interval::ms_10) {
    return "10_ms";
  } else if(inter_arg == Interval::ms_100) {
    return "100_ms";
  } else if(inter_arg == Interval::ms_1000) {
    return "1000_ms";
  } else {
    assert(0 != 0 && "Unknown type");
  }

}

static
uint64_t convert_period_to_uint(Interval  inter_arg)
{
  // unit: ms
  if(inter_arg == Interval::ms_1 ){
    return 1;
  } else if (inter_arg == Interval::ms_2) {
    return 2;
  } else if(inter_arg == Interval::ms_5) {
    return 5;
  } else if(inter_arg == Interval::ms_10) {
    return 10;
  } else if(inter_arg == Interval::ms_100) {
    return 100;
  } else if(inter_arg == Interval::ms_1000) {
    return 1000;
  } else {
    assert(0 != 0 && "Unknown period_ms");
  }

}

swig_fr_args_t global_swig_args;

std::vector<swig_sub_cust_sm_t> get_cust_sm_conf()
{
  std::vector<swig_sub_cust_sm_t> ret;
  for (int i = 0; i < global_swig_args.sub_cust_sm_len; i++) {
    swig_sub_cust_sm_t tmp;
    tmp.name = global_swig_args.sub_cust_sm[i].name;
    tmp.time = global_swig_args.sub_cust_sm[i].time;
    ret.push_back(tmp);
  }
  return ret;
}

std::vector<swig_sub_oran_sm_t> get_oran_sm_conf()
{
  std::vector<swig_sub_oran_sm_t> ret;
  for (int i = 0; i < global_swig_args.sub_oran_sm_len; i++) {
    swig_sub_oran_sm_t tmp;
    tmp.name = global_swig_args.sub_oran_sm[i].name;
    tmp.time = global_swig_args.sub_oran_sm[i].time;
    tmp.format = global_swig_args.sub_oran_sm[i].format;
    tmp.ran_type = global_swig_args.sub_oran_sm[i].ran_type;
    tmp.act_len = global_swig_args.sub_oran_sm[i].act_len;
    for (int j = 0; j < tmp.act_len; j++) {
      tmp.actions.push_back(global_swig_args.sub_oran_sm[i].actions[j]);
    }
//    printf("[SWIG]: getting action definition from the saving configuration:\n");
//    for (int32_t j = 0; j < tmp.act_len; ++j) {
//      std::cout << tmp.actions[j] << std::endl;
//    }
    ret.push_back(tmp);
  }
  return ret;
}

std::map<std::string, std::string> get_conf(std::string sectionName)
{
  std::map<std::string, std::string> configMap;

  libconfig::Config config;
  try {
    config.readFile(global_swig_args.conf_file.c_str());
    const libconfig::Setting& root = config.getRoot();

    if (!root.exists(sectionName.c_str())) {
      std::cerr << "Error: Section '" << sectionName << "' not found in the configuration file." << std::endl;
      return configMap;
    }

    const libconfig::Setting& section = root[sectionName.c_str()];
    for (int i = 0; i < section.getLength(); ++i) {
      const libconfig::Setting& setting = section[i];
      std::string key = setting.getName();
      std::string value;
      section.lookupValue(key.c_str(), value);
      configMap[key] = value;
      // std::cout << key << ": " << value << std::endl;
    }

  } catch (const libconfig::FileIOException& e) {
    std::cerr << "Error reading the configuration file: " << e.what() << std::endl;
  } catch (const libconfig::ParseException& e) {
    std::cerr << "Error parsing the configuration file at line " << e.getLine() << ": " << e.getError() << std::endl;
  }

  return configMap;
}

void init(std::vector<std::string>& argv)
{
  assert(initialized == false && "Already initialized!");
  // for (const auto& str : argv) {
  //  std::cout << str << std::endl;
  //}

  // Allocate memory for the char** array
  char** c_argv = new char*[argv.size()];
  // Copy each string to the char* array
  for (size_t i = 0; i < argv.size(); ++i) {
    // Allocate memory for the current string
    c_argv[i] = new char[argv[i].size() + 1];

    // Copy the contents of the string
    std::strcpy(c_argv[i], argv[i].c_str());
  }
  // Print the strings
  //  for (size_t i = 0; i < argv.size(); ++i) {
  //    std::cout << c_argv[i] << std::endl;
  //  }

  fr_args_t args = init_fr_args(argv.size(), c_argv);
//  swig_fr_args_t swig_args;
  global_swig_args.ip = args.ip;
  global_swig_args.e42_port = args.e42_port;
  global_swig_args.conf_file = args.conf_file;
  global_swig_args.libs_dir = args.libs_dir;
  global_swig_args.sub_cust_sm_len = args.sub_cust_sm_len;
  for (int32_t i = 0; i < global_swig_args.sub_cust_sm_len; ++i) {
    global_swig_args.sub_cust_sm[i].name = args.sub_cust_sm[i].name;
    global_swig_args.sub_cust_sm[i].time = args.sub_cust_sm[i].time;
  }
  global_swig_args.sub_oran_sm_len = args.sub_oran_sm_len;
  for (int32_t i = 0; i < global_swig_args.sub_oran_sm_len; ++i) {
    global_swig_args.sub_oran_sm[i].name = args.sub_oran_sm[i].name;
    global_swig_args.sub_oran_sm[i].time = args.sub_oran_sm[i].time;
    global_swig_args.sub_oran_sm[i].format = args.sub_oran_sm[i].format;
    global_swig_args.sub_oran_sm[i].ran_type = args.sub_oran_sm[i].ran_type;
    global_swig_args.sub_oran_sm[i].act_len = args.sub_oran_sm[i].act_len - 1;
    for (int32_t j = 0; j < global_swig_args.sub_oran_sm[i].act_len; ++j) {
      global_swig_args.sub_oran_sm[i].actions.push_back(args.sub_oran_sm[i].actions[j]);
    }
//    printf("[SWIG]: generating action definition from the list of required measurement data:\n");
//    for (int32_t j = 0; j < global_swig_args.sub_oran_sm[i].act_len; ++j) {
//      std::cout << global_swig_args.sub_oran_sm[i].actions[j] << std::endl;
//    }
  }

  // Deallocate memory for the char** array
  for (size_t i = 0; i < argv.size(); ++i) {
    delete[] c_argv[i];
  }
  delete[] c_argv;


  initialized = true;

  init_xapp_api(&args);
  free_fr_args(&args);
}

bool try_stop()
{
  return try_stop_xapp_api();
}

std::vector<E2Node> conn_e2_nodes(void)
{
  e2_node_arr_t arr = e2_nodes_xapp_api();

  std::vector<E2Node> x;

  for(int i = 0; i < arr.len; ++i){

    E2Node tmp;

    e2_node_connected_t const* src = &arr.n[i];

    tmp.id.type = src->id.type;
    tmp.id.plmn.mcc = src->id.plmn.mcc;
    tmp.id.plmn.mnc = src->id.plmn.mnc;
    tmp.id.plmn.mnc_digit_len = src->id.plmn.mnc_digit_len;
    tmp.id.nb_id = src->id.nb_id;
    size_t cuduid_idx = 0;
    if (src->id.cu_du_id) {
      while (src->id.cu_du_id[cuduid_idx]) {
        tmp.id.cu_du_id.push_back(src->id.cu_du_id[cuduid_idx]);
        cuduid_idx++;
      }
    }

    std::vector<RanFunction> ran_func;//(src->len_rf);

    for(size_t j = 0; j < src->len_rf; ++j){
      ran_function_t rf = cp_ran_function(&src->ack_rf[j]);

      RanFunction tmp_ran;

      tmp_ran.id = rf.id;
      tmp_ran.rev = rf.rev;
      tmp_ran.def = rf.def;
      // TODO: oid

      ran_func.push_back(tmp_ran);// [j] = rf;
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
void sm_cb_mac(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == MAC_STATS_V0);
  assert(hndlr_mac_cb != NULL);

  mac_ind_data_t const* data = &rd->ind.mac;

  swig_mac_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  ind.id.type = e2_node->type;
  ind.id.plmn.mcc = e2_node->plmn.mcc;
  ind.id.plmn.mnc = e2_node->plmn.mnc;
  ind.id.plmn.mnc_digit_len = e2_node->plmn.mnc_digit_len;
  ind.id.nb_id.nb_id = e2_node->nb_id.nb_id;
  ind.id.nb_id.unused = e2_node->nb_id.unused;
  size_t cuduid_idx = 0;
  if (e2_node->cu_du_id) {
    while (e2_node->cu_du_id[cuduid_idx]) {
      ind.id.cu_du_id.push_back(e2_node->cu_du_id[cuduid_idx]);
      cuduid_idx++;
    }
  }

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

int report_mac_sm(swig_global_e2_node_id_t* id, Interval inter_arg, mac_cb* handler)
{
  assert(id != NULL);
  assert(handler != NULL);

  hndlr_mac_cb = handler;

  const char* period = convert_period_to_char(inter_arg);

  global_e2_node_id_t* e2node_id = (global_e2_node_id_t*)id;
  sm_ans_xapp_t ans = report_sm_xapp_api(e2node_id, SM_MAC_ID, (void*)period, sm_cb_mac);
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
void sm_cb_rlc(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == RLC_STATS_V0);
  assert(hndlr_rlc_cb != NULL);

  rlc_ind_data_t const* data = &rd->ind.rlc;

  swig_rlc_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  ind.id.type = e2_node->type;
  ind.id.plmn.mcc = e2_node->plmn.mcc;
  ind.id.plmn.mnc = e2_node->plmn.mnc;
  ind.id.plmn.mnc_digit_len = e2_node->plmn.mnc_digit_len;
  ind.id.nb_id.nb_id = e2_node->nb_id.nb_id;
  ind.id.nb_id.unused = e2_node->nb_id.unused;
  size_t cuduid_idx = 0;
  if (e2_node->cu_du_id) {
    while (e2_node->cu_du_id[cuduid_idx]) {
      ind.id.cu_du_id.push_back(e2_node->cu_du_id[cuduid_idx]);
      cuduid_idx++;
    }
  }

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

int report_rlc_sm(swig_global_e2_node_id_t* id, Interval inter_arg, rlc_cb* handler)
{

  assert(id != NULL);
  assert(handler != NULL);

  hndlr_rlc_cb = handler;

  const char* period = convert_period_to_char(inter_arg);

  global_e2_node_id_t* e2node_id = (global_e2_node_id_t*)id;
  sm_ans_xapp_t ans = report_sm_xapp_api(e2node_id , SM_RLC_ID, (void*)period, sm_cb_rlc);
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
void sm_cb_pdcp(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == PDCP_STATS_V0);
  assert(hndlr_pdcp_cb != NULL);

  pdcp_ind_data_t const* data = &rd->ind.pdcp;

  swig_pdcp_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  ind.id.type = e2_node->type;
  ind.id.plmn.mcc = e2_node->plmn.mcc;
  ind.id.plmn.mnc = e2_node->plmn.mnc;
  ind.id.plmn.mnc_digit_len = e2_node->plmn.mnc_digit_len;
  ind.id.nb_id.nb_id = e2_node->nb_id.nb_id;
  ind.id.nb_id.unused = e2_node->nb_id.unused;
  size_t cuduid_idx = 0;
  if (e2_node->cu_du_id) {
    while (e2_node->cu_du_id[cuduid_idx]) {
      ind.id.cu_du_id.push_back(e2_node->cu_du_id[cuduid_idx]);
      cuduid_idx++;
    }
  }

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

int report_pdcp_sm(swig_global_e2_node_id_t* id, Interval inter_arg, pdcp_cb* handler)
{
  assert(id != NULL);
  assert(handler != NULL);

  hndlr_pdcp_cb = handler;

  const char* period = convert_period_to_char(inter_arg);

  global_e2_node_id_t* e2node_id = (global_e2_node_id_t*)id;
  sm_ans_xapp_t ans = report_sm_xapp_api(e2node_id , SM_PDCP_ID, (void*)period, sm_cb_pdcp);
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
void sm_cb_slice(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == SLICE_STATS_V0);
  assert(hndlr_slice_cb != NULL);

  slice_ind_data_t const* data = &rd->ind.slice;

  swig_slice_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  ind.id.type = e2_node->type;
  ind.id.plmn.mcc = e2_node->plmn.mcc;
  ind.id.plmn.mnc = e2_node->plmn.mnc;
  ind.id.plmn.mnc_digit_len = e2_node->plmn.mnc_digit_len;
  ind.id.nb_id.nb_id = e2_node->nb_id.nb_id;
  ind.id.nb_id.unused = e2_node->nb_id.unused;
  size_t cuduid_idx = 0;
  if (e2_node->cu_du_id) {
    while (e2_node->cu_du_id[cuduid_idx]) {
      ind.id.cu_du_id.push_back(e2_node->cu_du_id[cuduid_idx]);
      cuduid_idx++;
    }
  }


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

int report_slice_sm(swig_global_e2_node_id_t* id, Interval inter_arg, slice_cb* handler)
{
  assert( id != NULL);
  (void)inter_arg;
  assert(handler != NULL);

  hndlr_slice_cb = handler;

  const char* period = convert_period_to_char(inter_arg);

  global_e2_node_id_t* e2node_id = (global_e2_node_id_t*)id;
  sm_ans_xapp_t ans = report_sm_xapp_api(e2node_id, SM_SLICE_ID, (void*)period, sm_cb_slice);
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

void control_slice_sm(swig_global_e2_node_id_t* id, slice_ctrl_msg_t* ctrl)
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
  wr.type = CONTROL_SM_AG_IF_WR;
  wr.ctrl.type = SLICE_CTRL_REQ_V0;
  wr.ctrl.slice_req_ctrl.msg = cp_slice_ctrl_msg(ctrl);

  global_e2_node_id_t* e2node_id = (global_e2_node_id_t*)id;
  control_sm_xapp_api(e2node_id, SM_SLICE_ID,  &wr);
}

//////////////////////////////////////
// GTP SM   
/////////////////////////////////////

static 
gtp_cb* hndlr_gtp_cb; 

static
void sm_cb_gtp(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == GTP_STATS_V0);
  assert(hndlr_gtp_cb != NULL);

  gtp_ind_data_t const* data = &rd->ind.gtp;

  swig_gtp_ind_msg_t ind;
  ind.tstamp = data->msg.tstamp;

  ind.id.type = e2_node->type;
  ind.id.plmn.mcc = e2_node->plmn.mcc;
  ind.id.plmn.mnc = e2_node->plmn.mnc;
  ind.id.plmn.mnc_digit_len = e2_node->plmn.mnc_digit_len;
  ind.id.nb_id = e2_node->nb_id;
  size_t cuduid_idx = 0;
  if (e2_node->cu_du_id) {
    while (e2_node->cu_du_id[cuduid_idx]) {
      ind.id.cu_du_id.push_back(e2_node->cu_du_id[cuduid_idx]);
      cuduid_idx++;
    }
  }

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

int report_gtp_sm(swig_global_e2_node_id_t* id, Interval inter_arg, gtp_cb* handler)
{
  assert(id != NULL);
  assert(handler != NULL);

  hndlr_gtp_cb = handler;

  const char* period = convert_period_to_char(inter_arg);

  global_e2_node_id_t* e2node_id = (global_e2_node_id_t*)id;
  sm_ans_xapp_t ans = report_sm_xapp_api(e2node_id , SM_GTP_ID, (void*)period, sm_cb_gtp);
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
// KPM SM
/////////////////////////////////////

static
kpm_cb* hndlr_kpm_cb;

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0);
  assert(hndlr_kpm_cb != NULL);

  kpm_ind_data_t const* data = &rd->ind.kpm.ind;

  swig_kpm_ind_data_t swig_data;

  swig_data.id.type = e2_node->type;
  swig_data.id.plmn.mcc = e2_node->plmn.mcc;
  swig_data.id.plmn.mnc = e2_node->plmn.mnc;
  swig_data.id.plmn.mnc_digit_len = e2_node->plmn.mnc_digit_len;
  swig_data.id.nb_id = e2_node->nb_id;
  size_t cuduid_idx = 0;
  if (e2_node->cu_du_id) {
    while (e2_node->cu_du_id[cuduid_idx]) {
      swig_data.id.cu_du_id.push_back(e2_node->cu_du_id[cuduid_idx]);
      cuduid_idx++;
    }
  }

  // Header
  swig_data.hdr.type = data->hdr.type;
  kpm_ric_ind_hdr_format_1_t const* hdr_frm1 = &data->hdr.kpm_ric_ind_hdr_format_1;
  swig_data.hdr.kpm_ric_ind_hdr_format_1.collectStartTime = hdr_frm1->collectStartTime;
  if (hdr_frm1->fileformat_version) {
    swig_data.hdr.kpm_ric_ind_hdr_format_1.len_fileformat_version = hdr_frm1->fileformat_version->len;
    swig_data.hdr.kpm_ric_ind_hdr_format_1.fileformat_version = ByteArrayToString(hdr_frm1->fileformat_version);
  }
  if (hdr_frm1->sender_name) {
    swig_data.hdr.kpm_ric_ind_hdr_format_1.len_sender_name = hdr_frm1->sender_name->len;
    swig_data.hdr.kpm_ric_ind_hdr_format_1.sender_name = ByteArrayToString(hdr_frm1->sender_name);
  }
  if (hdr_frm1->sender_type) {
    swig_data.hdr.kpm_ric_ind_hdr_format_1.len_sender_type = hdr_frm1->sender_type->len;
    swig_data.hdr.kpm_ric_ind_hdr_format_1.sender_type = ByteArrayToString(hdr_frm1->fileformat_version);
  }
  if (hdr_frm1->vendor_name) {
    swig_data.hdr.kpm_ric_ind_hdr_format_1.len_vendor_name = hdr_frm1->vendor_name->len;
    swig_data.hdr.kpm_ric_ind_hdr_format_1.vendor_name = ByteArrayToString(hdr_frm1->vendor_name);
  }

  // Message
  swig_data.msg.type = data->msg.type;
  if (swig_data.msg.type == FORMAT_1_INDICATION_MESSAGE) {
    kpm_ind_msg_format_1_t const* msg = &data->msg.frm_1;
    // swig_meas_data_lst_t
    swig_data.msg.frm_1.meas_data_lst_len = msg->meas_data_lst_len;
    for (size_t j = 0; j < swig_data.msg.frm_1.meas_data_lst_len; j++) {
      swig_meas_data_lst_t tmp_data;
      meas_data_lst_t const* meas_data = &msg->meas_data_lst[j];
      tmp_data.meas_record_len = meas_data->meas_record_len;
      for (size_t m = 0; m < tmp_data.meas_record_len; m++) {
        swig_meas_record_lst_t tmp_record;
        meas_record_lst_t const* meas_record = &meas_data->meas_record_lst[m];
        tmp_record.value = meas_record->value;
        if (tmp_record.value == INTEGER_MEAS_VALUE) {
          tmp_record.int_val = meas_record->int_val;
        } else if (tmp_record.value == REAL_MEAS_VALUE) {
          tmp_record.real_val = meas_record->real_val;
        } else if (tmp_record.value == NO_VALUE_MEAS_VALUE) {
          tmp_record.no_value = "NULL";
        } else {
          assert(0 != 0 && "Unknown meas record");
        }

        tmp_data.meas_record_lst.emplace_back(tmp_record);
      }
      // incomplete_flag
      tmp_data.incomplete_flag = END_ENUM_VALUE;
      if (meas_data->incomplete_flag && *meas_data->incomplete_flag == TRUE_ENUM_VALUE) {
        tmp_data.incomplete_flag = *meas_data->incomplete_flag;
      }

      swig_data.msg.frm_1.meas_data_lst.emplace_back(tmp_data);
    }

    // swig_meas_info_format_1_lst_t
    swig_data.msg.frm_1.meas_info_lst_len = msg->meas_info_lst_len;
    for (size_t j = 0; j < swig_data.msg.frm_1.meas_info_lst_len; j++) {
      swig_meas_info_format_1_lst_t tmp_info;
      meas_info_format_1_lst_t const* meas_info = &msg->meas_info_lst[j];
      if (meas_info->meas_type.type == meas_type_t::NAME_MEAS_TYPE) {
        tmp_info.meas_type.type = swig_meas_type_e::NAME_MEAS_TYPE;
        tmp_info.meas_type.name_len = meas_info->meas_type.name.len;
        tmp_info.meas_type.name = ByteArrayToString(&meas_info->meas_type.name);
      } else if (meas_info->meas_type.type == meas_type_t::ID_MEAS_TYPE) {
        tmp_info.meas_type.type = swig_meas_type_e::ID_MEAS_TYPE;
        tmp_info.meas_type.id = meas_info->meas_type.id;
      } else {
        assert(0!=0 && "Unknown meas type");
      }

      swig_data.msg.frm_1.meas_info_lst.emplace_back(tmp_info);
    }

    // gran_period_ms
    size_t idx = 0;
    if (msg->gran_period_ms) {
      while (msg->gran_period_ms[idx]) {
        swig_data.msg.frm_1.gran_period_ms.push_back(msg->gran_period_ms[idx]);
        idx++;
      }
    }

  } else if (swig_data.msg.type == FORMAT_3_INDICATION_MESSAGE) {
    kpm_ind_msg_format_3_t const* msg = &data->msg.frm_3;
    swig_data.msg.frm_3.ue_meas_report_lst_len = msg->ue_meas_report_lst_len;
    for (size_t i = 0; i < swig_data.msg.frm_3.ue_meas_report_lst_len; i++) { // swig_meas_report_per_ue_t
      swig_meas_report_per_ue_t tmp;
      meas_report_per_ue_t const* meas_ue = &msg->meas_report_per_ue[i];
      // swig_ue_id_e2sm_t
      tmp.ue_meas_report_lst.type = meas_ue->ue_meas_report_lst.type;
      if (tmp.ue_meas_report_lst.type == GNB_UE_ID_E2SM) {
        tmp.ue_meas_report_lst.gnb.amf_ue_ngap_id = meas_ue->ue_meas_report_lst.gnb.amf_ue_ngap_id;
        tmp.ue_meas_report_lst.gnb.guami.plmn_id.mcc = meas_ue->ue_meas_report_lst.gnb.guami.plmn_id.mcc;
        tmp.ue_meas_report_lst.gnb.guami.plmn_id.mnc = meas_ue->ue_meas_report_lst.gnb.guami.plmn_id.mnc;
        tmp.ue_meas_report_lst.gnb.guami.plmn_id.mnc_digit_len = meas_ue->ue_meas_report_lst.gnb.guami.plmn_id.mnc_digit_len;
        tmp.ue_meas_report_lst.gnb.guami.amf_region_id = meas_ue->ue_meas_report_lst.gnb.guami.amf_region_id;
        tmp.ue_meas_report_lst.gnb.guami.amf_set_id = meas_ue->ue_meas_report_lst.gnb.guami.amf_set_id;
        tmp.ue_meas_report_lst.gnb.guami.amf_ptr = meas_ue->ue_meas_report_lst.gnb.guami.amf_ptr;
        tmp.ue_meas_report_lst.gnb.gnb_cu_ue_f1ap_lst_len = meas_ue->ue_meas_report_lst.gnb.gnb_cu_ue_f1ap_lst_len;
        // TODO: copy uint32_t *gnb_cu_ue_f1ap_lst
        tmp.ue_meas_report_lst.gnb.gnb_cu_cp_ue_e1ap_lst_len = meas_ue->ue_meas_report_lst.gnb.gnb_cu_cp_ue_e1ap_lst_len;
        // TODO: copy uint32_t *gnb_cu_cp_ue_e1ap_lst
        // TODO: copy uint64_t *ran_ue_id ...
      } else if (tmp.ue_meas_report_lst.type == GNB_DU_UE_ID_E2SM) {
        tmp.ue_meas_report_lst.gnb_du.gnb_cu_ue_f1ap = meas_ue->ue_meas_report_lst.gnb_du.gnb_cu_ue_f1ap;
      } else if (tmp.ue_meas_report_lst.type == GNB_CU_UP_UE_ID_E2SM) {
        tmp.ue_meas_report_lst.gnb_cu_up.gnb_cu_cp_ue_e1ap = meas_ue->ue_meas_report_lst.gnb_cu_up.gnb_cu_cp_ue_e1ap;
      } else {
        assert(0!=0 && "Not support UE ID type, do not handle swig_data.msg to callback");
      }

      // swig_kpm_ind_msg_format_1_t
      tmp.ind_msg_format_1.meas_data_lst_len = meas_ue->ind_msg_format_1.meas_data_lst_len;
      for (size_t j = 0; j < tmp.ind_msg_format_1.meas_data_lst_len; j++) { // swig_meas_data_lst_t
        swig_meas_data_lst_t tmp_data;
        meas_data_lst_t const* meas_data = &meas_ue->ind_msg_format_1.meas_data_lst[j];
        tmp_data.meas_record_len = meas_data->meas_record_len;
        for (size_t m = 0; m < tmp_data.meas_record_len; m++) {
          swig_meas_record_lst_t tmp_record;
          meas_record_lst_t const* meas_record = &meas_data->meas_record_lst[m];
          tmp_record.value = meas_record->value;
          if (tmp_record.value == INTEGER_MEAS_VALUE) {
            tmp_record.int_val = meas_record->int_val;
          } else if (tmp_record.value == REAL_MEAS_VALUE) {
            tmp_record.real_val = meas_record->real_val;
          } else if (tmp_record.value == NO_VALUE_MEAS_VALUE) {
            tmp_record.no_value = "NULL";
          } else {
            assert(0 != 0 && "Unknown meas record");
          }

          tmp_data.meas_record_lst.emplace_back(tmp_record);
        }
        // incomplete_flag
        tmp_data.incomplete_flag = END_ENUM_VALUE;
        if (meas_data->incomplete_flag && *meas_data->incomplete_flag == TRUE_ENUM_VALUE) {
          tmp_data.incomplete_flag = *meas_data->incomplete_flag;
        }

        tmp.ind_msg_format_1.meas_data_lst.emplace_back(tmp_data);
      }

      tmp.ind_msg_format_1.meas_info_lst_len = meas_ue->ind_msg_format_1.meas_info_lst_len;
      for (size_t j = 0; j < tmp.ind_msg_format_1.meas_info_lst_len; j++) { // swig_meas_info_format_1_lst_t
        swig_meas_info_format_1_lst_t tmp_info;
        meas_info_format_1_lst_t const* meas_info = &meas_ue->ind_msg_format_1.meas_info_lst[j];
        if (meas_info->meas_type.type == meas_type_t::NAME_MEAS_TYPE) {
          tmp_info.meas_type.type = swig_meas_type_e::NAME_MEAS_TYPE;
          tmp_info.meas_type.name_len = meas_info->meas_type.name.len;
          tmp_info.meas_type.name = ByteArrayToString(&meas_info->meas_type.name);
        } else if (meas_info->meas_type.type == meas_type_t::ID_MEAS_TYPE) {
          tmp_info.meas_type.type = swig_meas_type_e::ID_MEAS_TYPE;
          tmp_info.meas_type.id = meas_info->meas_type.id;
        } else {
          assert(0!=0 && "Unknown meas type");
        }

        tmp.ind_msg_format_1.meas_info_lst.emplace_back(tmp_info);
      }

      // gran_period_ms
      size_t idx = 0;
      if (meas_ue->ind_msg_format_1.gran_period_ms) {
        while (meas_ue->ind_msg_format_1.gran_period_ms[idx]) {
          tmp.ind_msg_format_1.gran_period_ms.push_back(meas_ue->ind_msg_format_1.gran_period_ms[idx]);
          idx++;
        }
      }


      swig_data.msg.frm_3.meas_report_per_ue.emplace_back(tmp);
    }

  } else {
    printf("Not support indication message format %d, do not handle swig_data.msg to callback\n", swig_data.msg.type);
  }
#ifdef XAPP_LANG_PYTHON
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
#endif

  hndlr_kpm_cb->handle(&swig_data);

#ifdef XAPP_LANG_PYTHON
  PyGILState_Release(gstate);
#endif
}

static
meas_info_format_1_lst_t gen_meas_info_format_1_lst(const char* action)
{
  meas_info_format_1_lst_t dst;

  dst.meas_type.type = meas_type_t::NAME_MEAS_TYPE;
  // ETSI TS 128 552
  dst.meas_type.name = cp_str_to_ba(action);

  dst.label_info_lst_len = 1;
  dst.label_info_lst = static_cast<label_info_lst_t*>(calloc(1, sizeof(label_info_lst_t)));
  assert(dst.label_info_lst != NULL && "Memory exhausted");
  dst.label_info_lst[0].noLabel = static_cast<enum_value_e*>(calloc(1, sizeof(enum_value_e)));
  assert(dst.label_info_lst[0].noLabel != NULL && "Memory exhausted");
  *dst.label_info_lst[0].noLabel = TRUE_ENUM_VALUE;

  return dst;
}

static
kpm_act_def_format_1_t gen_act_def_frmt_1(const char** action)
{
  kpm_act_def_format_1_t dst;

  dst.gran_period_ms = 1000;

  // [1, 65535]
  size_t count = 0;
  while (action[count] != NULL) {
    count++;
  }
  dst.meas_info_lst_len = count;
  dst.meas_info_lst = static_cast<meas_info_format_1_lst_t*>(calloc(count, sizeof(meas_info_format_1_lst_t)));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_info_lst_len; i++) {
    dst.meas_info_lst[i] = gen_meas_info_format_1_lst(action[i]);
  }

  dst.cell_global_id = NULL;
  dst.meas_bin_range_info_lst_len = 0;
  dst.meas_bin_info_lst = NULL;


  return dst;
}

static
kpm_act_def_format_4_t gen_act_def_frmt_4(const char** action)
{
  kpm_act_def_format_4_t dst;

  // [1, 32768]
  dst.matching_cond_lst_len = 1;

  dst.matching_cond_lst = static_cast<matching_condition_format_4_lst_t*>(calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t)));
  assert(dst.matching_cond_lst != NULL && "Memory exhausted");

  // Filter connected UEs by S-NSSAI criteria
  dst.matching_cond_lst[0].test_info_lst.test_cond_type = S_NSSAI_TEST_COND_TYPE;
  dst.matching_cond_lst[0].test_info_lst.S_NSSAI = TRUE_TEST_COND_TYPE;

  dst.matching_cond_lst[0].test_info_lst.test_cond = static_cast<test_cond_e*>(calloc(1, sizeof(test_cond_e)));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond = EQUAL_TEST_COND;

  dst.matching_cond_lst[0].test_info_lst.test_cond_value = static_cast<test_cond_value_e*>(calloc(1, sizeof(test_cond_value_e)));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond_value != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond_value =  INTEGER_TEST_COND_VALUE;
  dst.matching_cond_lst[0].test_info_lst.int_value = static_cast<int64_t*>(malloc(sizeof(int64_t)));
  assert(dst.matching_cond_lst[0].test_info_lst.int_value != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.int_value = 1;

  printf("[xApp]: Filter UEs by S-NSSAI criteria where SST = %lu\n", *dst.matching_cond_lst[0].test_info_lst.int_value);

  // Action definition Format 1
  dst.action_def_format_1 = gen_act_def_frmt_1(action);  // 8.2.1.2.1

  return dst;
}

static
kpm_act_def_t gen_act_def(const char** act, format_action_def_e act_frm)
{
  kpm_act_def_t dst;

  if (act_frm == FORMAT_1_ACTION_DEFINITION) {
    dst.type = FORMAT_1_ACTION_DEFINITION;
    dst.frm_1 = gen_act_def_frmt_1(act);
  } else if (act_frm == FORMAT_4_ACTION_DEFINITION) {
    dst.type = FORMAT_4_ACTION_DEFINITION;
    dst.frm_4 = gen_act_def_frmt_4(act);
  } else {
    assert(0!=0 && "not support action definition type");
  }

  return dst;
}

static
kpm_event_trigger_def_t gen_ev_trig(uint64_t period)
{
  kpm_event_trigger_def_t dst;

  dst.type = FORMAT_1_RIC_EVENT_TRIGGER;
  dst.kpm_ric_event_trigger_format_1.report_period_ms = period;

  return dst;
}

int report_kpm_sm(swig_global_e2_node_id_t* id, Interval inter_arg, std::vector<std::string>& action, kpm_cb* handler)
{
  assert(id != NULL);
  assert(handler != NULL);

  hndlr_kpm_cb = handler;

  char** c_action = new char*[action.size() + 1];
  for (size_t i = 0; i < action.size() + 1; ++i) {
    if (i == action.size())
      c_action[i] = NULL;
    else {
      c_action[i] = new char[strlen(action[i].c_str()) + 1];
      strcpy(c_action[i], action[i].c_str());
    }

  }
  printf("[xApp]: generating action definition from the list of required measurement data:\n");
  for (size_t i = 0; i < action.size() + 1; ++i) {
    std::cout << c_action[i] << std::endl;
  }

  kpm_sub_data_t kpm_sub = {};

  // KPM Event Trigger
  const uint64_t period_ms = convert_period_to_uint(inter_arg);
  kpm_sub.ev_trg_def = gen_ev_trig(period_ms);
  printf("[xApp]: reporting period = %lu [ms]\n", period_ms);

  // KPM Action Definition
  kpm_sub.sz_ad = 1;
  kpm_sub.ad = static_cast<kpm_act_def_t*>(calloc(1, sizeof(kpm_act_def_t)));
  assert(kpm_sub.ad != NULL && "Memory exhausted");

  format_action_def_e act_type = FORMAT_4_ACTION_DEFINITION; // act_def.type

  *kpm_sub.ad = gen_act_def(const_cast<const char**>(c_action), act_type);
  // TODO: need to fix, initialize kpm_sub with 0 value

//  switch (id.type)
//  {
//    case e2ap_ngran_gNB: ;
//      const char *act_gnb[] = {"DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL", "DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl", NULL}; // 3GPP TS 28.552
//      *kpm_sub.ad = gen_act_def(act_gnb, act_type);
//      break;
//
//    case e2ap_ngran_gNB_CU: ;
//      const char *act_gnb_cu[] = {"DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL", NULL}; // 3GPP TS 28.552
//      *kpm_sub.ad = gen_act_def(act_gnb_cu, act_type);
//      break;
//
//    case e2ap_ngran_gNB_DU: ;
//      const char *act_gnb_du[] = {"DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl", NULL}; // 3GPP TS 28.552
//      *kpm_sub.ad = gen_act_def(act_gnb_du, act_type);
//      break;
//
//    default:
//      assert(false && "NG-RAN Type not yet implemented");
//  }

  global_e2_node_id_t* e2node_id = (global_e2_node_id_t*)id;
  sm_ans_xapp_t ans = report_sm_xapp_api(e2node_id, SM_KPM_ID, &kpm_sub, sm_cb_kpm);
  assert(ans.success == true);

  // Deallocate memory for the char** array
  for (size_t i = 0; i < action.size() + 1; ++i) {
    delete[] c_action[i];
  }
  delete[] c_action;
//  free_kpm_sub_data(&kpm_sub);

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
