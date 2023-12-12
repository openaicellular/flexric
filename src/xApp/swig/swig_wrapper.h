#ifndef SWIG_WRAPPER_H
#define SWIG_WRAPPER_H 

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <cstring>
#include <map>

#include "../../lib/e2ap/e2ap_global_node_id_wrapper.h"
#include "../../lib/e2ap/e2ap_plmn_wrapper.h"
#include "../../lib/e2ap/e2ap_ran_function_wrapper.h"
#include "../../lib/3gpp/ie/e2ap_gnb_id.h"

#include "../../sm/mac_sm/ie/mac_data_ie.h"
#include "../../sm/rlc_sm/ie/rlc_data_ie.h"
#include "../../sm/pdcp_sm/ie/pdcp_data_ie.h"
#include "../../sm/slice_sm/ie/slice_data_ie.h"
#include "../../sm/gtp_sm/ie/gtp_data_ie.h"
#include "../../sm/kpm_sm/kpm_data_ie_wrapper.h"

//////////////////////////////////////
// General    
/////////////////////////////////////
#define MAX_NUM_CUST_SM 6
#define MAX_NUM_ORAN_SM 8 // consider diff ran type

struct RanFunction{
  byte_array_t def;
  uint16_t id;
  uint16_t rev;
  // TODO: std::vector<byte_array_t> oid; // optional
};

struct swig_global_e2_node_id_t {
  e2ap_ngran_node_t type;
  e2ap_plmn_t plmn;
  e2ap_gnb_id_t nb_id;
  std::vector<long unsigned int> cu_du_id;
};

struct E2Node {
  swig_global_e2_node_id_t id;
  std::vector<RanFunction> ran_func;
};

typedef struct {
  std::string name;
  std::string time;
} swig_sub_cust_sm_t;

typedef struct {
  std::string name;
  int32_t time;
  int32_t format;
  std::string ran_type;
  int32_t act_len;
  std::vector<std::string> actions;
} swig_sub_oran_sm_t;

struct swig_fr_args_t {
  std::string ip;
  int32_t e42_port;

  std::string conf_file;
  std::string libs_dir;

  int32_t sub_cust_sm_len;
  swig_sub_cust_sm_t sub_cust_sm[MAX_NUM_CUST_SM];
  int32_t sub_oran_sm_len;
  swig_sub_oran_sm_t sub_oran_sm[MAX_NUM_ORAN_SM];
};

std::vector<swig_sub_cust_sm_t> get_cust_sm_conf();

std::vector<swig_sub_oran_sm_t> get_oran_sm_conf();

std::map<std::string, std::string> get_conf(std::string mainkey);

void init(std::vector<std::string>& argv);

bool try_stop(void);

std::vector<E2Node> conn_e2_nodes(void);

enum class Interval {
  ms_1,
  ms_2,
  ms_5,
  ms_10,
  ms_100,
  ms_1000,
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
typedef struct {
    meas_value_e value;
    uint32_t   int_val;  // [0..4294967295]
    double	   real_val;
    std::string      no_value; // define as void* in C
} swig_meas_record_lst_t;


typedef struct {
    size_t                    meas_record_len;  // [1, 65535]
    std::vector<swig_meas_record_lst_t>        meas_record_lst;
    enum_value_e              incomplete_flag;  // OPTIONAL, Indicates that the measurements record is not reliable.
} swig_meas_data_lst_t;

typedef enum {
    NAME_MEAS_TYPE,
    ID_MEAS_TYPE,

    END_MEAS_TYPE
} swig_meas_type_e;

typedef struct {
    swig_meas_type_e type;
    size_t name_len;
    std::string name; // 8.3.9
    uint16_t id; // 8.3.10
} swig_meas_type_t;

//typedef struct{
//    enum_value_e  	    *noLabel;			/* OPTIONAL */
//    e2sm_plmn_t         *plmn_id;   		/* OPTIONAL */
//    S_NSSAI_t       	  *sliceID;			/* OPTIONAL */
//    uint8_t            	  *fiveQI;			/* OPTIONAL */
//    uint8_t               *qFI;				/* OPTIONAL, INTEGER (0..63, …) */
//    uint8_t        	      *qCI;	    		/* OPTIONAL */
//    uint8_t        	      *qCImax;			/* OPTIONAL */
//    uint8_t        	      *qCImin;			/* OPTIONAL */
//    uint8_t	              *aRPmax;			/* OPTIONAL, INTEGER (1.. 15, …) */
//    uint8_t	              *aRPmin;			/* OPTIONAL, INTEGER (1.. 15, …) */
//    uint16_t	          *bitrateRange;	/* OPTIONAL */
//    uint16_t	          *layerMU_MIMO;	/* OPTIONAL */
//    enum_value_e  	      *sUM;	    		/* OPTIONAL */
//    uint16_t              *distBinX;		/* OPTIONAL */
//    uint16_t	          *distBinY;		/* OPTIONAL */
//    uint16_t	          *distBinZ;		/* OPTIONAL */
//    enum_value_e  	      *preLabelOverride;	/* OPTIONAL */
//    start_end_ind_e	      *startEndInd;		/* OPTIONAL */
//    enum_value_e  	      *min;	    		/* OPTIONAL */
//    enum_value_e  	      *max;	    		/* OPTIONAL */
//    enum_value_e          *avg;	    		/* OPTIONAL */
//    uint16_t  			  *ssbIndex;		/* OPTIONAL */
//    uint16_t  			  *nonGoB_beamformModeIndex;  /* OPTIONAL */
//    uint8_t   			  *mimoModeIndex;    /* OPTIONAL, 1 = SU-MIMO, 2 = MU-MIMO  ask Mikel */
//} swig_label_info_lst_t;

typedef struct{
    swig_meas_type_t meas_type;
    size_t label_info_lst_len;  // [1, 2147483647]
//    std::vector<label_info_lst_t> label_info_lst;  // 8.3.11
} swig_meas_info_format_1_lst_t;

typedef struct {
    size_t                    meas_data_lst_len; // [1, 65535]
    std::vector<swig_meas_data_lst_t>          meas_data_lst;

    size_t                    meas_info_lst_len; // [0, 65535]
    std::vector<swig_meas_info_format_1_lst_t> meas_info_lst;    // OPTIONAL, meas_info_lst_len can be zero

    std::vector<unsigned long>  gran_period_ms;  // 8.3.8  -  OPTIONAL

} swig_kpm_ind_msg_format_1_t;
//
//typedef struct {
//    meas_type_t meas_type;
//
//    size_t matching_cond_lst_len;  // [1, 32768]
//    std::vector<matching_condition_format_3_lst_t> matching_cond_lst;
//
//    size_t ue_id_matched_lst_len;  // [0, 65535]
//    std::vector<ue_id_e2sm_t> ue_id_matched_lst;
//
//    size_t ue_id_gran_period_lst_len;  // [0, 65535]
//    std::vector<ue_id_gran_period_lst_t> ue_id_gran_period_lst;  // not yet implemented in ASN.1 - possible extension
//
//} meas_info_cond_ue_lst_t;
//
//typedef struct {
//    size_t                    meas_data_lst_len; // [1, 65535]
//    std::vector<swig_meas_data_lst_t>          meas_data_lst;
//
//    size_t                    meas_info_cond_ue_lst_len;  // [1, 65535]
//    std::vector<swig_meas_info_cond_ue_lst_t>  meas_info_cond_ue_lst;
//
//    std::vector<unsigned long>  gran_period_ms;  // 8.3.8  -  OPTIONAL  (1..4294967295)
//
//} swig_kpm_ind_msg_format_2_t;
//
typedef struct {
    ue_id_e2sm_e type;
    gnb_e2sm_t gnb;
    gnb_du_e2sm_t  gnb_du;
    gnb_cu_up_e2sm_t  gnb_cu_up;
    ng_enb_e2sm_t ng_enb;
    ng_enb_du_e2sm_t  ng_enb_du;
    en_gnb_e2sm_t  en_gnb;
    enb_e2sm_t enb;
} swig_ue_id_e2sm_t;

typedef struct {
    swig_ue_id_e2sm_t ue_meas_report_lst;  // 8.3.24
    swig_kpm_ind_msg_format_1_t ind_msg_format_1;  // 8.2.1.4.1; measurement data per ue
} swig_meas_report_per_ue_t;


typedef struct {
    size_t ue_meas_report_lst_len;  // [1, 65535]
    std::vector<swig_meas_report_per_ue_t> meas_report_per_ue;
} swig_kpm_ind_msg_format_3_t;

typedef struct{
    format_ind_msg_e type;
    swig_kpm_ind_msg_format_1_t frm_1;  // 8.2.1.4.1
//    swig_kpm_ind_msg_format_2_t frm_2;  // 8.2.1.4.2
    swig_kpm_ind_msg_format_3_t frm_3;  // 8.2.1.4.3
} swig_kpm_ind_msg_t;

typedef struct {
    uint64_t collectStartTime;  // 8.3.12
    size_t len_fileformat_version;
    std::string fileformat_version;  /* OPTIONAL */
    size_t len_sender_name;
    std::string sender_name;         /* OPTIONAL */
    size_t len_sender_type;
    std::string sender_type;         /* OPTIONAL */
    size_t len_vendor_name;
    std::string vendor_name;         /* OPTIONAL */
} swig_kpm_ric_ind_hdr_format_1_t;

typedef struct {
    format_ind_hdr_e type;
    swig_kpm_ric_ind_hdr_format_1_t kpm_ric_ind_hdr_format_1;
} swig_kpm_ind_hdr_t;

typedef struct {
    swig_kpm_ind_hdr_t hdr;
    swig_kpm_ind_msg_t msg;
    swig_global_e2_node_id_t id;
} swig_kpm_ind_data_t;

struct kpm_cb {
    virtual void handle(swig_kpm_ind_data_t* a) = 0;
    virtual ~kpm_cb() {}
};

//typedef struct{
//    size_t meas_info_lst_len;  // [1, 65535]
//    std::vector<swig_meas_info_format_1_lst_t> meas_info_lst;
//
//    uint32_t gran_period_ms;  // 8.3.8 [0, 4294967295]
//
//    std::vector<cell_global_id_t> cell_global_id; // 8.3.20 - OPTIONAL
//
//    size_t meas_bin_range_info_lst_len; // [0, 65535]
//    std::vector<meas_bin_range_info_lst_t> meas_bin_info_lst;
//
//} swig_kpm_act_def_format_1_t ;
//
//typedef struct {
//    size_t matching_cond_lst_len;  // [1, 32768]
//    std::vector<matching_condition_format_4_lst_t> matching_cond_lst;
//    kpm_act_def_format_1_t action_def_format_1;  // 8.2.1.2.1
//} swig_kpm_act_def_format_4_t;
//
//typedef struct{
//    format_action_def_e type;
//    swig_kpm_act_def_format_1_t frm_1;  // 8.2.1.2.1
////    kpm_act_def_format_2_t frm_2;  // 8.2.1.2.2
////    kpm_act_def_format_3_t frm_3;  // 8.2.1.2.3
//    swig_kpm_act_def_format_4_t frm_4;  // 8.2.1.2.4
////    kpm_act_def_format_5_t frm_5;  // 8.2.1.2.5
//} swig_kpm_act_def_t;

int report_kpm_sm(swig_global_e2_node_id_t* id, Interval inter, std::vector<std::string>& action, kpm_cb* handler);

void rm_report_kpm_sm(int);

#endif

