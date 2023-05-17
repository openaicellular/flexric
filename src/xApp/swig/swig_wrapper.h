#ifndef SWIG_WRAPPER_H
#define SWIG_WRAPPER_H 

#include <string>
#include <memory>
#include <vector>
#include <cstddef>

#include "../../lib/ap/e2ap_types/common/e2ap_global_node_id.h"
#include "../../lib/ap/e2ap_types/common/e2ap_plmn.h"
#include "../../lib/ap/e2ap_types/common/e2ap_ran_function.h"

#include "../../sm/mac_sm/ie/mac_data_ie.h"
#include "../../sm/rlc_sm/ie/rlc_data_ie.h"
#include "../../sm/pdcp_sm/ie/pdcp_data_ie.h"
#include "../../sm/slice_sm/ie/slice_data_ie.h"
#include "../../sm/gtp_sm/ie/gtp_data_ie.h"
#include "../../sm/kpm_sm_v2.02/ie/kpm_data_ie.h"

//////////////////////////////////////
// General    
/////////////////////////////////////

struct RanFunction{
  byte_array_t def;
  uint16_t id;
  uint16_t rev;
  // TODO: std::vector<byte_array_t> oid; // optional
};

struct swig_global_e2_node_id_t {
  ngran_node_t type;
  plmn_t plmn;
  uint32_t nb_id;
  std::vector<long unsigned int> cu_du_id;
};

struct E2Node {
  swig_global_e2_node_id_t id;
  std::vector<RanFunction> ran_func;
};

void init(void); 

bool try_stop(void);

std::vector<E2Node> conn_e2_nodes(void);

enum class Interval {
  ms_1,
  ms_2,
  ms_5,
  ms_10,
};

//////////////////////////////////////
// MAC SM   
/////////////////////////////////////

struct swig_mac_ind_msg_t{
  std::vector<mac_ue_stats_impl_t> ue_stats;
  swig_global_e2_node_id_t id;
  int64_t tstamp;
};

struct mac_cb {
    virtual void handle(swig_mac_ind_msg_t* a) = 0;
    virtual ~mac_cb() {}
};

int report_mac_sm(swig_global_e2_node_id_t* id, Interval inter, mac_cb* handler);

void rm_report_mac_sm(int);

//////////////////////////////////////
// RLC SM   
/////////////////////////////////////

struct swig_rlc_ind_msg_t{
  std::vector<rlc_radio_bearer_stats_t> rb_stats;
  swig_global_e2_node_id_t id;
  int64_t tstamp;
};

struct rlc_cb {
    virtual void handle(swig_rlc_ind_msg_t* a) = 0;
    virtual ~rlc_cb() {}
};

int report_rlc_sm(swig_global_e2_node_id_t* id, Interval inter, rlc_cb* handler);

void rm_report_rlc_sm(int);

//////////////////////////////////////
// PDCP SM   
/////////////////////////////////////

struct swig_pdcp_ind_msg_t{
  std::vector<pdcp_radio_bearer_stats_t> rb_stats;
  swig_global_e2_node_id_t id;
  int64_t tstamp;
};

struct pdcp_cb {
    virtual void handle(swig_pdcp_ind_msg_t* a) = 0;
    virtual ~pdcp_cb() {}
};

int report_pdcp_sm(swig_global_e2_node_id_t* id, Interval inter, pdcp_cb* handler);

void rm_report_pdcp_sm(int);

//////////////////////////////////////
// SLICE SM   
/////////////////////////////////////

typedef struct{
    uint32_t id;

    uint32_t len_label;
    std::vector<std::string> label;

    uint32_t len_sched;
    std::vector<std::string> sched;

    slice_params_t params;
} swig_fr_slice_t ;

typedef struct{
    uint32_t len_slices;
    std::vector<swig_fr_slice_t> slices;

    uint32_t len_sched_name;
    std::vector<std::string> sched_name;
} swig_ul_dl_slice_conf_t ;

typedef struct{
    swig_ul_dl_slice_conf_t dl;
    swig_ul_dl_slice_conf_t ul;
} swig_slice_conf_t ;

typedef struct{
    uint32_t len_ue_slice;
    std::vector<ue_slice_assoc_t> ues;
} swig_ue_slice_conf_t;

struct swig_slice_ind_msg_t{
  swig_slice_conf_t slice_stats;
  swig_ue_slice_conf_t ue_slice_stats;
  swig_global_e2_node_id_t id;
  int64_t tstamp;
};

struct slice_cb {
    virtual void handle(swig_slice_ind_msg_t* a) = 0;
    virtual ~slice_cb() {}
};

int report_slice_sm(swig_global_e2_node_id_t* id, Interval inter, slice_cb* handler);

void rm_report_slice_sm(int);

void control_slice_sm(swig_global_e2_node_id_t* id, slice_ctrl_msg_t* ctrl);

//////////////////////////////////////
// GTP SM   
/////////////////////////////////////

struct swig_gtp_ind_msg_t{
  std::vector<gtp_ngu_t_stats_t> gtp_stats;
  swig_global_e2_node_id_t id;
  int64_t tstamp;
};

struct gtp_cb {
    virtual void handle(swig_gtp_ind_msg_t* a) = 0;
    virtual ~gtp_cb() {}
};

int report_gtp_sm(swig_global_e2_node_id_t* id, Interval inter, gtp_cb* handler);

void rm_report_gtp_sm(int);

//////////////////////////////////////
// KPM SM   
/////////////////////////////////////
enum swig_MeasRecordType {
  MeasRecord_int = 1,
  MeasRecord_real,
  MeasRecord_noval
};

struct swig_adapter_MeasRecord_t {
  swig_MeasRecordType type;
  unsigned long int_val;
  double real_val;
};

struct swig_adapter_MeasDataItem_t {
  size_t measRecord_len; // 1..
  std::vector<swig_adapter_MeasRecord_t> measRecord;
  long incompleteFlag; // OPTIONAL: true(0) value indicates that the measurements record
                       // is not reliable, and we pass this info to ASN. -1 means the flag is not present
};

struct swig_MeasInfo_t {
    meas_type_e meas_type;
    adapter_MeasurementTypeName_t measName;
    adapter_MeasurementTypeID_t measID;
    //std::vector<adapter_LabelInfoItem_t> labelInfo; // Replaced with std::vector
    size_t labelInfo_len;
};


struct swig_kpm_ind_msg_t{
  size_t MeasData_len; // 1..
  std::vector<swig_adapter_MeasDataItem_t> MeasData;

  /*
   * list implemented as an array of length 'MeasInfo_len' containing measurement names like 'DL Transmitted Data Volume' or
   * its corresponding ID. It is the same mechanism used for 'action definition type 1'
   */
  size_t MeasInfo_len;
  std::vector<swig_MeasInfo_t> MeasInfo; // OPTIONAL, MeasInfo_len can be zero

  std::vector<unsigned long> granulPeriod; /* OPTIONAL */

  /*
   * XXX-extensions: add all the info you might find in all the indication format types.
   * Still missing some fields for supporting format_2 and format_3
   */
};

struct kpm_cb {
    virtual void handle(swig_kpm_ind_msg_t* a) = 0;
    virtual ~kpm_cb() {}
};

int report_kpm_sm(global_e2_node_id_t* id, Interval inter, kpm_cb* handler);

void rm_report_kpm_sm(int);

#endif

