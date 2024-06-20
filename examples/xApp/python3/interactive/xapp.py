import time
# import pdb
import inspect
import json
from tabulate import tabulate
# import curses
# from curses import wrapper
# from curses.textpad import rectangle, Textbox
# import math
from enum import Enum
import os
import sys
_cur_dir = os.path.dirname(os.path.abspath(__file__))
# print("Current Directory:", cur_dir)
sdk_path = _cur_dir + "/../xapp_sdk/"
sys.path.append(sdk_path)

import xapp_sdk as ric


####################
####  HELP
####################
def print_funcs_list():
    """
    print_funcs_list():
        Print provided functions in this interactive xApp.
    """
    function_names = [name for name, func in globals().items() if callable(func) and func.__module__ == __name__ and not name.startswith('_')]
    print("Available functions:")
    for name in function_names:
        print("- " + name)

def print_funcs_usage(func_name):
    """
    print_funcs_usage(func_name):
        Print given function information.

    Parameters:
        func_name: function name (ex: xapp.init)
    """
    print(func_name.__doc__)


ex_kpm_actions_gnb = ["DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL", "DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl"]
ex_kpm_actions_gnb_du = ["DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl"]
ex_kpm_actions_gnb_cu = ["DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL"]

class _ServiceModelEnum(Enum):
    MAC = "mac_sm"
    SLICE = "slice_sm"
    KPM = "kpm_sm"
ServiceModel: _ServiceModelEnum
ServiceModel = _ServiceModelEnum.SLICE

class _SubTTIEnum(Enum):
    ms1 = ric.Interval_ms_1
    ms2 = ric.Interval_ms_2
    ms5 = ric.Interval_ms_5
    ms10 = ric.Interval_ms_10
    ms100 = ric.Interval_ms_100
    ms1000 = ric.Interval_ms_1000
SubTimeInterval: _SubTTIEnum
SubTimeInterval = _SubTTIEnum.ms10

class _SliceTypeEnum(Enum):
    ADDMOD = "ADDMOD"
    DELETE = "DEL"
    ASSOC_UE = "ASSOC_UE"
SliceType: _SliceTypeEnum
SliceType = _SliceTypeEnum.ADDMOD

####################
#### KPM INDICATION CALLBACK
####################
# Create a callback for KPM which derived it from C++ class kpm_cb
class _KPMCallback(ric.kpm_cb):
    def __init__(self):
        # Inherit C++ _kpm_cb class
        ric.kpm_cb.__init__(self)
    # Create an override C++ method
    def handle(self, ind):
        # if ind.hdr:
        t_now = time.time_ns() / 1000.0
        #     t_kpm = ind.hdr.kpm_ric_ind_hdr_format_1.collectStartTime / 1.0
        #     t_diff = t_now - t_kpm
        #     print(f"KPM Indication tstamp {t_now} diff {t_diff} E2-node type {ind.id.type} nb_id {ind.id.nb_id.nb_id}")
        _kpm_ind_to_dict_json(ind, t_now, ind.id)


####################
####  GLOBAL VALUE
####################
_e2nodes = 0
MAX_E2_NODES = 10
_slice_hndlr = {}
_mac_hndlr = {}
_kpm_hndlr = {}
_slice_cb = 0
_mac_cb = 0
_kpm_cb = 0

####################
####  KPM INDICATION MSG TO JSON
####################

_kpm_stats_struct = {
    "Format" : {},
    "Latency" : {},
    "RAN" : {
        "nb_id" : {},
        "ran_type" : {},
    },
    "UEs" : []
}

global _global_kpm_stats
_global_kpm_stats = [_kpm_stats_struct for i in range(0, MAX_E2_NODES)]

def _kpm_ind_to_dict_json(ind, t_now, id):
    global _e2nodes
    # find e2 node idx
    n_idx = -1
    for n in _e2nodes:
        if n.id.nb_id.nb_id == id.nb_id.nb_id:
            if n.id.type == id.type:
                n_idx = _e2nodes.index(n)

                break
    if n_idx == -1:
        print("cannot find e2 node idx")
        return

    global _global_kpm_stats
    _global_kpm_stats[n_idx] = {
        "Format" : {},
        "Latency" : {},
        "RAN" : {
            "nb_id" : {},
            "ran_type" : {},
        },
        "UEs" : []
    }

    kpm_stats = _global_kpm_stats[n_idx]

    # e2 node id
    kpm_stats["RAN"]["nb_id"] = id.nb_id.nb_id
    kpm_stats["RAN"]["ran_type"] = _get_ngran_name(id.type)

    # initial
    kpm_dict = kpm_stats

    # header
    if ind.hdr:
        # latency
        t_diff = t_now - ind.hdr.kpm_ric_ind_hdr_format_1.collectStartTime
        kpm_dict.update({"Latency" :  t_diff})

        # format # TODO: different format should map to different json struct
        if ind.msg.type == ric.FORMAT_1_INDICATION_MESSAGE:
            kpm_dict.update({"Format" : 1})
        elif ind.msg.type == ric.FORMAT_3_INDICATION_MESSAGE:
            kpm_dict.update({"Format" : 3})
        else:
            kpm_dict.update({"Format" : "UNKNOWN"})
            print(f"not implement KPM indication format {ind.msg.type}")

        if ind.hdr.kpm_ric_ind_hdr_format_1.fileformat_version:
            kpm_stats["RAN"]["fileformat_version"] = ind.hdr.kpm_ric_ind_hdr_format_1.fileformat_version
        if ind.hdr.kpm_ric_ind_hdr_format_1.sender_name:
            kpm_stats["RAN"]["sender_name"] = ind.hdr.kpm_ric_ind_hdr_format_1.sender_name
        if ind.hdr.kpm_ric_ind_hdr_format_1.sender_type:
            kpm_stats["RAN"]["sender_type"] = ind.hdr.kpm_ric_ind_hdr_format_1.sender_type
        if ind.hdr.kpm_ric_ind_hdr_format_1.vendor_name:
            kpm_stats["RAN"]["vendor_name"] = ind.hdr.kpm_ric_ind_hdr_format_1.vendor_name

    # message
    if kpm_dict["Format"] == 3:
        for index, ue_meas in enumerate(ind.msg.frm_3.meas_report_per_ue):

            ue_dict = {
                "UE_ID" : {
                    "idx" : index,
                    "type" : {},
                },
                "Measurements" : []
            }

            ue_id = ue_dict["UE_ID"]

            ue = ue_meas.ue_meas_report_lst
            if ue.type == ric.GNB_UE_ID_E2SM:
                ue_id.update({"type" : "GNB_UE_ID_E2SM"})
                ue_id["amf_ue_ngap_id"] = ue.gnb.amf_ue_ngap_id
                ue_id["guami.plmn_id.mcc"] = ue.gnb.guami.plmn_id.mcc
                ue_id["guami.plmn_id.mnc"] = ue.gnb.guami.plmn_id.mnc
                ue_id["guami.plmn_id.mnc_digit_len"] = ue.gnb.guami.plmn_id.mnc_digit_len
            elif ue.type == ric.GNB_DU_UE_ID_E2SM:
                ue_id.update({"type" : "GNB_DU_UE_ID_E2SM"})
                ue_id["gnb_cu_ue_f1ap"] = ue.gnb_du.gnb_cu_ue_f1ap
            elif ue.type == ric.GNB_CU_UP_UE_ID_E2SM:
                ue_id.update({"type" : "GNB_CU_UP_UE_ID_E2SM"})
                ue_id["gnb_cu_cp_ue_e1ap"] = ue.gnb_cu_up.gnb_cu_cp_ue_e1ap
            else:
                print("python3: not support ue_id_e2sm type")


            ind_frm1 = ue_meas.ind_msg_format_1
            for idx, meas_data in enumerate(ind_frm1.meas_data_lst):
                tmp_dict = {"idx" : idx}
                if meas_data.incomplete_flag == ric.TRUE_ENUM_VALUE:
                    tmp_dict["incomplete_flag"] = "true"

                tmp_dict["data"] = []
                if meas_data.meas_record_len == ind_frm1.meas_info_lst_len:
                    for meas_record, meas_info in zip(meas_data.meas_record_lst, ind_frm1.meas_info_lst):
                        # print(f"value: {meas_record.value}")
                        # print(f"type: {meas_info.meas_type.type}")
                        print_value = 0
                        if meas_record.value == ric.INTEGER_MEAS_VALUE:
                            print_value = meas_record.int_val
                        elif meas_record.value == ric.REAL_MEAS_VALUE:
                            print_value = meas_record.real_val
                        elif meas_record.value == ric.NO_VALUE_MEAS_VALUE:
                            print_value = meas_record.no_value
                        else:
                            print(f"unknown meas_record")

                        print_name_id = 0
                        if meas_info.meas_type.type == ric.NAME_MEAS_TYPE:
                            print_name_id = meas_info.meas_type.name
                        elif meas_info.meas_type.type == ric.ID_MEAS_TYPE:
                            print_name_id = meas_info.meas_type.id
                        else:
                            print(f"unknown meas info type")
                        meas_dict = {
                            "name/id" : print_name_id,
                            "value" : print_value
                        }
                        tmp_dict["data"].append(meas_dict)
                else:
                    print(f"meas_data.meas_record_len {meas_data.meas_record_len} != ind_frm1.meas_info_lst_len {ind_frm1.meas_info_lst_len}, cannot map value to name")
                ue_dict["Measurements"].append(tmp_dict)

            kpm_dict["UEs"].append(ue_dict)


####################
#### MAC INDICATION CALLBACK
####################
class _MACCallback(ric.mac_cb):
    # Define Python class 'constructor'
    def __init__(self):
        # Call C++ base class constructor
        ric.mac_cb.__init__(self)
    # Override C++ method: virtual void handle(swig_mac_ind_msg_t a) = 0;
    def handle(self, ind):
        # Print swig_mac_ind_msg_t
        if len(ind.ue_stats) > 0:
            t_now = time.time_ns() / 1000.0
            t_mac = ind.tstamp / 1.0
            t_diff = t_now - t_mac
            # print(f"MAC Indication tstamp {t_now} diff {t_diff} e2 node type {ind.id.type} nb_id {ind.id.nb_id.nb_id}")
            # print('MAC rnti = ' + str(ind.ue_stats[0].rnti))

####################
####  SLICE INDICATION MSG TO JSON
####################
_slice_stats_struct = {
    "RAN" : {
        "nb_id" : {},
        "ran_type" : {},
        "dl" : {}
        # TODO: handle the ul slice stats, currently there is no ul slice stats in database(SLICE table)
        # "ul" : {}
    },
    "UE" : {}
}

global _global_slice_stats
_global_slice_stats = [_slice_stats_struct for i in range(0, MAX_E2_NODES)]

def _slice_ind_to_dict_json(ind, id):
    global _e2nodes
    # find e2 node idx
    n_idx = -1
    for n in _e2nodes:
        if n.id.nb_id.nb_id == id.nb_id.nb_id:
            if n.id.type == id.type:
                n_idx = _e2nodes.index(n)
                break
    if n_idx == -1:
        print("cannot find e2 node idx")
        return

    global _global_slice_stats
    _global_slice_stats[n_idx] = {
        "RAN" : {
            "nb_id" : {},
            "ran_type" : {},
            "dl" : {}
            # TODO: handle the ul slice stats, currently there is no ul slice stats in database(SLICE table)
            # "ul" : {}
        },
        "UE" : {}
    }
    slice_stats = _global_slice_stats[n_idx]

    # RAN - e2 node id
    slice_stats["RAN"]["nb_id"] = id.nb_id.nb_id
    slice_stats["RAN"]["ran_type"] = _get_ngran_name(id.type)
    # RAN - dl
    dl_dict = slice_stats["RAN"]["dl"]
    if ind.slice_stats.dl.len_slices <= 0:
        dl_dict["num_of_slices"] = ind.slice_stats.dl.len_slices
        dl_dict["slice_sched_algo"] = "null"
        dl_dict["ue_sched_algo"] = ind.slice_stats.dl.sched_name[0]
    else:
        dl_dict["num_of_slices"] = ind.slice_stats.dl.len_slices
        dl_dict["slice_sched_algo"] = "null"
        dl_dict["slices"] = []
        slice_algo = ""
        for s in ind.slice_stats.dl.slices:
            if s.params.type == 1: # TODO: convert from int to string, ex: type = 1 -> STATIC
                slice_algo = "STATIC"
            elif s.params.type == 2:
                slice_algo = "NVS"
            elif s.params.type == 4:
                slice_algo = "EDF"
            else:
                slice_algo = "unknown"
            dl_dict.update({"slice_sched_algo" : slice_algo})

            slices_dict = {
                "index" : s.id,
                "label" : s.label[0],
                "ue_sched_algo" : s.sched[0],
            }
            if dl_dict["slice_sched_algo"] == "STATIC":
                slices_dict["slice_algo_params"] = {
                    "pos_low" : s.params.u.sta.pos_low,
                    "pos_high" : s.params.u.sta.pos_high
                }
            elif dl_dict["slice_sched_algo"] == "NVS":
                if s.params.u.nvs.conf == 0: # TODO: convert from int to string, ex: conf = 0 -> RATE
                    slices_dict["slice_algo_params"] = {
                        "type" : "RATE",
                        "mbps_rsvd" : s.params.u.nvs.u.rate.u1.mbps_required,
                        "mbps_ref" : s.params.u.nvs.u.rate.u2.mbps_reference
                    }
                elif s.params.u.nvs.conf == 1: # TODO: convert from int to string, ex: conf = 1 -> CAPACITY
                    slices_dict["slice_algo_params"] = {
                        "type" : "CAPACITY",
                        "pct_rsvd" : s.params.u.nvs.u.capacity.u.pct_reserved
                    }
                else:
                    slices_dict["slice_algo_params"] = {"type" : "unknown"}
            elif dl_dict["slice_sched_algo"] == "EDF":
                slices_dict["slice_algo_params"] = {
                    "deadline" : s.params.u.edf.deadline,
                    "guaranteed_prbs" : s.params.u.edf.guaranteed_prbs,
                    "max_replenish" : s.params.u.edf.max_replenish
                }
            else:
                print("unknown slice algorithm, cannot handle params")
            dl_dict["slices"].append(slices_dict)

    # RAN - ul
    # TODO: handle the ul slice stats, currently there is no ul slice stats in database(SLICE table)
    # ul_dict = slice_stats["RAN"]["ul"]
    # if ind.slice_stats.ul.len_slices <= 0:
    #     dl_dict["num_of_slices"] = ind.slice_stats.ul.len_slices
    #     dl_dict["slice_sched_algo"] = "null"
    #     dl_dict["ue_sched_algo"] = ind.slice_stats.ul.sched_name

    # UE
    ue_dict = slice_stats["UE"]
    if ind.ue_slice_stats.len_ue_slice <= 0:
        ue_dict["num_of_ues"] = ind.ue_slice_stats.len_ue_slice
    else:
        ue_dict["num_of_ues"] = ind.ue_slice_stats.len_ue_slice
        ue_dict["ues"] = []
        for ue_idx, u in enumerate(ind.ue_slice_stats.ues):
            ues_dict = {}
            dl_id = "null"
            if u.dl_id >= 0 and dl_dict["num_of_slices"] > 0:
                dl_id = u.dl_id
            ues_dict = {
                "idx": ue_idx,
                "rnti" : hex(u.rnti),
                "assoc_dl_slice_id" : dl_id
                # TODO: handle the associated ul slice id, currently there is no ul slice id in database(UE_SLICE table)
                # "assoc_ul_slice_id" : ul_id
            }
            ue_dict["ues"].append(ues_dict)

    ind_dict = slice_stats
    ind_json = json.dumps(ind_dict)

    json_fname = "rt_slice_stats_nb_id" + str(id.nb_id.nb_id)+ ".json"
    with open(json_fname, "w") as outfile:
        outfile.write(ind_json)
    # print(ind_dict)



####################
#### SLICE INDICATION CALLBACK
####################
class _SLICECallback(ric.slice_cb):
    # Define Python class 'constructor'
    def __init__(self):
        # Call C++ base class constructor
        ric.slice_cb.__init__(self)
    # Override C++ method: virtual void handle(swig_slice_ind_msg_t a) = 0;
    def handle(self, ind):
        # Print swig_slice_ind_msg_t
        #if (ind.slice_stats.dl.len_slices > 0):
        #     print('SLICE Indication tstamp = ' + str(ind.tstamp))
        #     print('SLICE STATE: len_slices = ' + str(ind.slice_stats.dl.len_slices))
        #     print('SLICE STATE: sched_name = ' + str(ind.slice_stats.dl.sched_name[0]))
        #if (ind.ue_slice_stats.len_ue_slice > 0):
        #    print('UE ASSOC SLICE STATE: len_ue_slice = ' + str(ind.ue_slice_stats.len_ue_slice))
        _slice_ind_to_dict_json(ind, ind.id)

####################
####  SLICE CONTROL FUNCS
####################
def _fill_slice_conf(slice_params, slice_sched_algo):
    s = ric.fr_slice_t()
    s.id = slice_params["index"]
    s.label = slice_params["label"]
    s.len_label = len(slice_params["label"])
    s.sched = slice_params["ue_sched_algo"]
    s.len_sched = len(slice_params["ue_sched_algo"])
    if slice_sched_algo == "STATIC":
        s.params.type = ric.SLICE_ALG_SM_V0_STATIC
        s.params.u.sta.pos_low = slice_params["slice_algo_params"]["pos_low"]
        s.params.u.sta.pos_high = slice_params["slice_algo_params"]["pos_high"]
    elif slice_sched_algo == "NVS":
        s.params.type = ric.SLICE_ALG_SM_V0_NVS
        if slice_params["slice_algo_params"]["type"] == "RATE":
            s.params.u.nvs.conf = ric.SLICE_SM_NVS_V0_RATE
            s.params.u.nvs.u.rate.u1.mbps_required = slice_params["slice_algo_params"]["mbps_rsvd"]
            s.params.u.nvs.u.rate.u2.mbps_reference = slice_params["slice_algo_params"]["mbps_ref"]
            # print("ADD NVS DL SLCIE: id", s.id,
            # ", conf", s.params.u.nvs.conf,
            # ", mbps_rsrv", s.params.u.nvs.u.rate.u1.mbps_required,
            # ", mbps_ref", s.params.u.nvs.u.rate.u2.mbps_reference)
        elif slice_params["slice_algo_params"]["type"] == "CAPACITY":
            s.params.u.nvs.conf = ric.SLICE_SM_NVS_V0_CAPACITY
            s.params.u.nvs.u.capacity.u.pct_reserved = slice_params["slice_algo_params"]["pct_rsvd"]
            # print("ADD NVS DL SLCIE: id", s.id,
            # ", conf", s.params.u.nvs.conf,
            # ", pct_rsvd", s.params.u.nvs.u.capacity.u.pct_reserved)
        else:
            print("Unkown NVS conf")
    elif slice_sched_algo == "EDF":
        s.params.type = ric.SLICE_ALG_SM_V0_EDF
        s.params.u.edf.deadline = slice_params["slice_algo_params"]["deadline"]
        s.params.u.edf.guaranteed_prbs = slice_params["slice_algo_params"]["guaranteed_prbs"]
        s.params.u.edf.max_replenish = slice_params["slice_algo_params"]["max_replenish"]
    else:
        print("Unkown slice algo type")


    return s

####################
####  SLICE CONTROL PARAMETER EXAMPLE - ADD/MOD SLICE
####################
# ex_slice_conf_addmod_static = {
#     "num_of_slices" : 3,
#     "slice_sched_algo" : "STATIC",
#     "slices" : [
#         {
#             "index" : 0,
#             "label" : "s1",
#             "ue_sched_algo" : "PF",
#             "slice_algo_params" : {"pos_low" : 0, "pos_high" : 2},
#         },
#         {
#             "index" : 2,
#             "label" : "s2",
#             "ue_sched_algo" : "PF",
#             "slice_algo_params" : {"pos_low" : 3, "pos_high" : 10},
#         },
#         {
#             "index" : 5,
#             "label" : "s3",
#             "ue_sched_algo" : "PF",
#             "slice_algo_params" : {"pos_low" : 11, "pos_high" : 13},
#         }
#     ]
# }

ex_slice_conf_addmod_nvs_rate2 = {
    "num_of_slices" : 2,
    "slice_sched_algo" : "NVS",
    "slices" : [
        {
            "index" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"type": "RATE", "mbps_rsvd" : 60, "mbps_ref" : 200},
        },
        {
            "index" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"type": "RATE", "mbps_rsvd" : 140, "mbps_ref" : 200},
        }
    ]
}

ex_slice_conf_addmod_nvs_cap2 = {
    "num_of_slices" : 2,
    "slice_sched_algo" : "NVS",
    "slices" : [
        {
            "index" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"type": "CAPACITY", "pct_rsvd" : 0.7},
        },
        {
            "index" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"type": "CAPACITY", "pct_rsvd" : 0.29},
        }
    ]
}

ex_slice_conf_addmod_nvs_cap3 = {
    "num_of_slices" : 3,
    "slice_sched_algo" : "NVS",
    "slices" : [
        {
            "index" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"type": "CAPACITY", "pct_rsvd" : 0.5},
        },
        {
            "index" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"type": "CAPACITY", "pct_rsvd" : 0.3},
        },
        {
            "index" : 5,
            "label" : "s3",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"type": "CAPACITY", "pct_rsvd" : 0.2},
        }
    ]
}

ex_slice_conf_addmod_nvs = {
    "num_of_slices" : 3,
    "slice_sched_algo" : "NVS",
    "slices" : [
        {
            "index" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"type": "CAPACITY", "pct_rsvd" : 0.5},
        },
        {
            "index" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"type": "RATE", "mbps_rsvd" : 30, "mbps_ref" : 100},
        },
        {
            "index" : 5,
            "label" : "s3",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"type": "RATE", "mbps_rsvd" : 60, "mbps_ref" : 100},
        }
    ]
}
#
# ex_slice_conf_addmod_edf = {
#     "num_of_slices" : 3,
#     "slice_sched_algo" : "EDF",
#     "slices" : [
#         {
#             "index" : 0,
#             "label" : "s1",
#             "ue_sched_algo" : "PF",
#             "slice_algo_params" : {"deadline" : 10, "guaranteed_prbs" : 20, "max_replenish" : 0},
#         },
#         {
#             "index" : 2,
#             "label" : "s2",
#             "ue_sched_algo" : "RR",
#             "slice_algo_params" : {"deadline" : 20, "guaranteed_prbs" : 20, "max_replenish" : 0},
#         },
#         {
#             "index" : 5,
#             "label" : "s3",
#             "ue_sched_algo" : "MT",
#             "slice_algo_params" : {"deadline" : 40, "guaranteed_prbs" : 10, "max_replenish" : 0},
#         }
#     ]
# }

ex_slice_conf_addmod_reset = {
    "num_of_slices" : 0
}

####################
####  SLICE CONTROL PARAMETER EXAMPLE - DELETE SLICE
####################
ex_slice_conf_delete = {
    "num_of_slices" : 1,
    "delete_dl_slice_id" : [5]
}

####################
####  SLICE CONTROL PARAMETER EXAMPLE - ASSOC UE SLICE
####################
ex_slice_conf_assoc_ue = {
    "num_of_ues" : 1,
    "ues" : [
        {
            "idx" : 0,
            "assoc_dl_slice_id" : 2
        }
    ]
}

def _get_rnti_by_idx(n_idx, ue_idx):
    global _global_slice_stats
    s = _global_slice_stats[n_idx]
    len_ues = s['UE']["num_of_ues"]
    rnti = 0
    for i in range(0, len_ues):
        if s['UE']["ues"][i]["idx"] == ue_idx:
            rnti = s['UE']["ues"][i]["rnti"]
            break
    if rnti == 0:
        print("failed: cannot find rnti by the given UE idx")
    return int(rnti, 16)

def _fill_slice_ctrl_msg(n_idx, ctrl_type, ctrl_msg):
    msg = ric.slice_ctrl_msg_t()
    if ctrl_type == "ADDMOD":
        msg.type = ric.SLICE_CTRL_SM_V0_ADD
        dl = ric.ul_dl_slice_conf_t()
        # TODO: UL SLICE CTRL ADD
        # ul = ric.ul_dl_slice_conf_t()

        # ue_sched_algo can be "RR"(round-robin), "PF"(proportional fair) or "MT"(maximum throughput) and it has to be set in any len_slices
        ue_sched_algo = "PF"
        dl.sched_name = ue_sched_algo
        dl.len_sched_name = len(ue_sched_algo)

        dl.len_slices = ctrl_msg["num_of_slices"]
        slices = ric.slice_array(ctrl_msg["num_of_slices"])
        for i in range(0, ctrl_msg["num_of_slices"]):
            slices[i] = _fill_slice_conf(ctrl_msg["slices"][i], ctrl_msg["slice_sched_algo"])

        dl.slices = slices
        msg.u.add_mod_slice.dl = dl
        # TODO: UL SLICE CTRL ADD
        # msg.u.add_mod_slice.ul = ul;
    elif ctrl_type == "DEL":
        msg.type = ric.SLICE_CTRL_SM_V0_DEL

        msg.u.del_slice.len_dl = ctrl_msg["num_of_slices"]
        del_dl_id = ric.del_dl_array(ctrl_msg["num_of_slices"])
        for i in range(ctrl_msg["num_of_slices"]):
            del_dl_id[i] = ctrl_msg["delete_dl_slice_id"][i]
        # print("DEL DL SLICE: id", del_dl_id)

        # TODO: UL SLCIE CTRL DEL
        msg.u.del_slice.dl = del_dl_id
    elif ctrl_type == "ASSOC_UE":
        msg.type = ric.SLICE_CTRL_SM_V0_UE_SLICE_ASSOC

        msg.u.ue_slice.len_ue_slice = ctrl_msg["num_of_ues"]
        assoc = ric.ue_slice_assoc_array(ctrl_msg["num_of_ues"])
        for i in range(ctrl_msg["num_of_ues"]):
            a = ric.ue_slice_assoc_t()
            a.rnti = _get_rnti_by_idx(n_idx, ctrl_msg["ues"][i]["idx"])
            a.dl_id = ctrl_msg["ues"][i]["assoc_dl_slice_id"]
            # TODO: UL SLICE CTRL ASSOC
            # a.ul_id = 0
            assoc[i] = a
            # print("ASSOC DL SLICE: <rnti:", a.rnti, "(NEED TO FIX)>, id", a.dl_id)
        msg.u.ue_slice.ues = assoc

    return msg

####################
#### CONVERT RAN TYPE TO STRING
####################
def _get_ngran_name(ran_type):
    if ran_type == 0:
        return "ngran_eNB"
    elif ran_type == 2:
        return "ngran_gNB"
    elif ran_type == 5:
        return "ngran_gNB_CU"
    elif ran_type == 7:
        return "ngran_gNB_DU"
    else:
        return "Unknown"

####################
#### UPDATE CONNECTED E2 NODES
####################
def _get_e2_nodes():
    return ric.conn_e2_nodes()

def _gen_e2nodeid_key(id):
    plmn = "PLMN_" + str(id.plmn.mcc) + str(id.plmn.mnc)
    nb_id = "NBID_" + str(id.nb_id.nb_id)
    ran_type = _get_ngran_name(id.type)
    return plmn + "-" + nb_id + "-" + ran_type

####################
####  xAPP INIT
####################
def init(path_to_conf):
    """
    init(path_to_conf):
        Init xApp to setup connection with NearRT-RIC.
    Parameters:
        path_to_conf: configuration for the xApp to setup E42 connection and database.
    """
    # 0. init
    ric.init(['', '-c', path_to_conf])
    # 1. get the length of connected e2 nodes
    global _e2nodes
    _e2nodes = _get_e2_nodes()
    e2nodes_len = len(_e2nodes)
    # while e2nodes_len <= 0:
    #     temp_e2nodes = _get_e2_nodes()
    #     if e2nodes != temp_e2nodes:
    #         print("Update connected E2 nodes")
    #         e2nodes = temp_e2nodes
    #         e2nodes_len = len(e2nodes)
    #     else:
    #         print("No E2 node connects")
    #         time.sleep(1)
    # assert(len(e2nodes) > 0)

    print_e2_nodes()

    # TODO: need to process multi e2 nodes
    # e2node = e2nodes[0]
    # for n in e2nodes:
    #     # 3. subscribe slice sm and mac sm
    #     n_idx = e2nodes.index(n)
    #     global _slice_cb
    #     _slice_cb[n_idx] = SLICECallback()
    #     global _slice_hndlr
    #     hndlr = ric.report_slice_sm(n.id, ric.Interval_ms_10, _slice_cb[n_idx])
    #     _slice_hndlr.append(hndlr)
    #
    #     global _mac_cb
    #     _mac_cb[n_idx] = MACCallback()
    #     global _mac_hndlr
    #     hndlr = ric.report_mac_sm(n.id, ric.Interval_ms_10, _mac_cb[n_idx])
    #     _mac_hndlr.append(hndlr)
    #     time.sleep(2)
    #
    #     # 4. create slices, adding nvs_slices_rate1 by default
    #     msg = fill_slice_ctrl_msg("ADDMOD", conf_nvs_slices_cap2)
    #     ric.control_slice_sm(n.id, msg)

####################
#### subscribe_sm
####################
def subscribe_sm(n_idx, enum_sm, tti_enum, action):
    """
    subscribe_sm(n_idx, enum_sm, tti_enum, action):
        Subscribe service model from the specific E2-Node.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
        enum_sm: enum of service model name (support: xapp.ServiceModel.MAC/SLICE/KPM).
        tti_enum: enum of indication message time interval (support: xapp.SubTTI.ms1/ms2/ms5/ms10/ms100/ms1000).
        action: list of action definitions used for KPM SM (ex: ex_kpm_actions_gnb)
    """
    global _e2nodes
    # default tti is 10 ms
    tti = ric.Interval_ms_10
    if tti_enum.value:
        tti = tti_enum.value
    else:
        print("unknown tti")

    sub_sm_str = enum_sm.value
    if sub_sm_str == "mac_sm":
        global _mac_cb
        global _mac_hndlr
        _mac_cb = _MACCallback()
        hndlr = ric.report_mac_sm(_e2nodes[n_idx].id, tti, _mac_cb)
        key = _gen_e2nodeid_key(_e2nodes[n_idx].id)
        _mac_hndlr.setdefault(key, []).append(hndlr)
    elif sub_sm_str == "slice_sm":
        global _slice_cb
        global _slice_hndlr
        _slice_cb = _SLICECallback()
        hndlr = ric.report_slice_sm(_e2nodes[n_idx].id, tti, _slice_cb)
        key = _gen_e2nodeid_key(_e2nodes[n_idx].id)
        _slice_hndlr.setdefault(key, []).append(hndlr)
    elif sub_sm_str == "kpm_sm":
        global _kpm_cb
        global _kpm_hndlr
        _kpm_cb = _KPMCallback()
        hndlr = ric.report_kpm_sm(_e2nodes[n_idx].id, tti, action, _kpm_cb)
        key = _gen_e2nodeid_key(_e2nodes[n_idx].id)
        _kpm_hndlr.setdefault(key, []).append(hndlr)
    else:
        print("unknown sm")

####################
####  SEND SLICE CONTROL MSG
####################
# cmd = "ADDMOD", "DEL", "ASSOC_UE"
# conf = conf_nvs_slices, conf_delete_slices, conf_assoc_ue_slice
def send_slice_ctrl(n_idx, type_enum, conf):
    """
    send_slice_ctrl(n_idx, type_enum, conf):
        Send slice control to the specific E2-Node.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
        type_enum: enum of slice action (support: xapp.SliceType.ADDMOD/DEL/ASSOC_UE).
        conf: slice configuration (ex: xapp.conf_nvs_slices).
    """
    st = time.time_ns() / 1000.0
    cmd = type_enum.value
    msg = _fill_slice_ctrl_msg(n_idx, cmd, conf)
    global _e2nodes
    ric.control_slice_sm(_e2nodes[n_idx].id, msg)
    print(f"[xApp]: Control Loop Latency: ${(time.time_ns() / 1000.0) - st} us")

####################
####  print_e2_nodes
####################
def print_e2_nodes():
    """
    print_e2_nodes():
        Print connected E2-Nodes' stats in table.
    """
    e2nodes_col_names = ["idx", "nb_id", "mcc", "mnc", "ran_type"]
    global _e2nodes
    _e2nodes = _get_e2_nodes()
    e2nodes_data = []
    conn = ric.conn_e2_nodes()
    for i in range(0, len(conn)):
        # TODO: need to fix cu_du_id in swig
        # cu_du_id = -1
        # if conn[i].id.cu_du_id:
        #     cu_du_id = conn[i].id.cu_du_id
        info = [i,
                conn[i].id.nb_id.nb_id,
                conn[i].id.plmn.mcc,
                conn[i].id.plmn.mnc,
                _get_ngran_name(conn[i].id.type)]
        # print(info)
        e2nodes_data.append(info)
    print(tabulate(e2nodes_data, headers=e2nodes_col_names, tablefmt="grid"))
    # print("E2 node : "
    #       "nb_id " + str(e2node.id.nb_id.nb_id) + ",",
    #       "mcc " + str(e2node.id.plmn.mcc) + ",",
    #       "mnc " + str(e2node.id.plmn.mnc) + ",",
    #       "mnc_digit_len " + str(e2node.id.plmn.mnc_digit_len) + ",",
    #       "ran_type " + get_ngran_name(e2node.id.type) + ',',
    #       "cu_du_id " + str(e2node.id.cu_du_id if e2node.id.cu_du_id else -1))


####################
####  print_slice_stats
####################
def print_slice_stats(n_idx):
    """
    print_slice_stats(n_idx):
        Print slice stats from the specific E2-Node in table.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
    """
    slice_stats_col_names = ["nb_id", "ran_type", "slice_id", "label", "slice_sched_algo", "slice_algo_param1", "slice_algo_param2", "slice_algo_param3", "ue_sched_algo"]
    ue_stats_col_names = ["idx", "rnti", "assoc_slice_id"]
    global _global_slice_stats
    s = _global_slice_stats[n_idx]
    # RAN
    slice_stats_table = []
    nb_id = s["RAN"]["nb_id"]
    ran_type = s["RAN"]["ran_type"]
    len_slices = s["RAN"]["dl"]["num_of_slices"]
    for i in range(0, len_slices):
        param = []
        for key in s["RAN"]["dl"]["slices"][i]["slice_algo_params"].keys():
            param.append(key)

        slice_algo = s["RAN"]["dl"]["slice_sched_algo"]
        info = []
        if slice_algo == "STATIC":
            info = [nb_id,
                    ran_type,
                    s["RAN"]["dl"]["slices"][i]["index"],
                    s["RAN"]["dl"]["slices"][i]["label"],
                    slice_algo,
                    str(param[0]) + ":" + str(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[0]]),
                    str(param[1]) + ":" + str(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[1]]),
                    "null",
                    s["RAN"]["dl"]["slices"][i]["ue_sched_algo"]]
        elif slice_algo == "NVS":
            nvs_type = s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[0]]
            if nvs_type == "RATE":
                mbps_rsvd = float("{:.2f}".format(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[1]]))
                mbps_ref = float("{:.2f}".format(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[2]]))
                info = [nb_id,
                        ran_type,
                        s["RAN"]["dl"]["slices"][i]["index"],
                        s["RAN"]["dl"]["slices"][i]["label"],
                        str(slice_algo) + "-" + str(nvs_type),
                        str(param[1]) + ":" + str(mbps_rsvd),
                        str(param[2]) + ":" + str(mbps_ref),
                        "null",
                        s["RAN"]["dl"]["slices"][i]["ue_sched_algo"]]
            elif nvs_type == "CAPACITY":
                pct_rsvd = float("{:.2f}".format(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[1]]))
                info = [nb_id,
                        ran_type,
                        s["RAN"]["dl"]["slices"][i]["index"],
                        s["RAN"]["dl"]["slices"][i]["label"],
                        str(slice_algo) + "-" + str(nvs_type),
                        str(param[1]) + ":" + str(pct_rsvd),
                        "null",
                        "null",
                        s["RAN"]["dl"]["slices"][i]["ue_sched_algo"]]
        elif slice_algo == "EDF":
            info = [nb_id,
                    ran_type,
                    s["RAN"]["dl"]["slices"][i]["index"],
                    s["RAN"]["dl"]["slices"][i]["label"],
                    str(slice_algo),
                    str(param[0]) + ":" + str(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[0]]),
                    str(param[1]) + ":" + str(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[1]]),
                    str(param[2]) + ":" + str(s["RAN"]["dl"]["slices"][i]["slice_algo_params"][param[2]]),
                    s["RAN"]["dl"]["slices"][i]["ue_sched_algo"]]
        if len(info) > 0:
            slice_stats_table.append(info)
    if len_slices == 0:
        info = [nb_id, ran_type]
        slice_stats_table.append(info)

    # UE
    ue_slice_stats_table = []
    len_ues = s['UE']["num_of_ues"]
    for i in range(0, len_ues):
        info = [s['UE']["ues"][i]["idx"],
                s['UE']["ues"][i]["rnti"],
                s['UE']["ues"][i]["assoc_dl_slice_id"]]
        if len(info) > 0:
            ue_slice_stats_table.append(info)

    print(tabulate(slice_stats_table, headers=slice_stats_col_names, tablefmt="grid"))
    print(tabulate(ue_slice_stats_table, headers=ue_stats_col_names, tablefmt="grid"))

####################
####  print_slice_stats_json
####################
def print_slice_stats_json(n_idx):
    """
    print_slice_stats_json(n_idx):
        Print slice stats from the specific E2-Node in JSON.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
    """
    global _global_slice_stats
    s = _global_slice_stats[n_idx]
    json_formatted_str = json.dumps(s, indent=2)
    print(json_formatted_str)

####################
####  print_slice_stats_loop
####################
def print_slice_stats_loop(n_idx, n_loop):
    """
    print_slice_stats_loop(n_idx, n_loop):
        Print slice stats from the specific E2-Node in table for N seconds.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
        n_loop: N seconds.
    """
    global len_table_str
    for i in range(0, n_loop):
        os.system('cls' if os.name == 'nt' else 'clear')
        print_slice_stats(n_idx)
        time.sleep(1)

####################
####  print_slice_conf
####################
def print_slice_conf(type_enum, conf):
    """
    print_slice_conf(type_enum, conf):
        Print slice configuration in table.

    Parameters:
        type_enum: enum of slice action (support: xapp.SliceType.ADDMOD/DEL/ASSOC_UE).
        conf: slice configuration (ex: xapp.ex_slice_conf_addmod_nvs).
    """
    mod_slice_conf_col_names = ["slice_id", "label", "slice_sched_algo", "slice_algo_param1", "slice_algo_param2", "slice_algo_param3", "ue_sched_algo"]
    del_slice_conf_col_names = ["slice_id"]
    assoc_ue_conf_col_names = ["idx", "assoc_slice_id"]
    type = type_enum.value
    if type == "ADDMOD":
        # RAN
        mod_slice_conf_table = []
        len_slices = conf["num_of_slices"]
        for i in range(0, len_slices):
            param = []
            for key in conf["slices"][i]["slice_algo_params"].keys():
                param.append(key)

            slice_algo = conf["slice_sched_algo"]
            info = []
            if slice_algo == "STATIC":
                info = [conf["slices"][i]["index"],
                        conf["slices"][i]["label"],
                        slice_algo,
                        str(param[0]) + ":" + str(conf["slices"][i]["slice_algo_params"][param[0]]),
                        str(param[1]) + ":" + str(conf["slices"][i]["slice_algo_params"][param[1]]),
                        "null",
                        conf["slices"][i]["ue_sched_algo"]]
            elif slice_algo == "NVS":
                nvs_type = conf["slices"][i]["slice_algo_params"][param[0]]
                if nvs_type == "RATE":
                    mbps_rsvd = float("{:.2f}".format(conf["slices"][i]["slice_algo_params"][param[1]]))
                    mbps_ref = float("{:.2f}".format(conf["slices"][i]["slice_algo_params"][param[2]]))
                    info = [conf["slices"][i]["index"],
                            conf["slices"][i]["label"],
                            str(slice_algo) + "-" + str(nvs_type),
                            str(param[1]) + ":" + str(mbps_rsvd),
                            str(param[2]) + ":" + str(mbps_ref),
                            "null",
                            conf["slices"][i]["ue_sched_algo"]]
                elif nvs_type == "CAPACITY":
                    pct_rsvd = float("{:.2f}".format(conf["slices"][i]["slice_algo_params"][param[1]]))
                    info = [conf["slices"][i]["index"],
                            conf["slices"][i]["label"],
                            str(slice_algo) + "-" + str(nvs_type),
                            str(param[1]) + ":" + str(pct_rsvd),
                            "null",
                            "null",
                            conf["slices"][i]["ue_sched_algo"]]
            elif slice_algo == "EDF":
                info = [conf["slices"][i]["index"],
                        conf["slices"][i]["label"],
                        str(slice_algo),
                        str(param[0]) + ":" + str(conf["slices"][i]["slice_algo_params"][param[0]]),
                        str(param[1]) + ":" + str(conf["slices"][i]["slice_algo_params"][param[1]]),
                        str(param[2]) + ":" + str(conf["slices"][i]["slice_algo_params"][param[2]]),
                        conf["slices"][i]["ue_sched_algo"]]
            if len(info) > 0:
                mod_slice_conf_table.append(info)
        print(tabulate(mod_slice_conf_table, headers=mod_slice_conf_col_names))
    elif type == "DEL":
        del_slice_conf_table = []
        len_slices = conf["num_of_slices"]
        for i in range(0, len_slices):
            info = [conf["delete_dl_slice_id"][i]]
            if len(info) > 0:
                del_slice_conf_table.append(info)
        print(tabulate(del_slice_conf_table, headers=del_slice_conf_col_names))
    elif type == "ASSOC_UE":
        # UE
        assoc_ue_slice_conf_table = []
        len_ues = conf["num_of_ues"]
        for i in range(0, len_ues):
            info = [conf["ues"][i]["idx"],
                    conf["ues"][i]["assoc_dl_slice_id"]]
            if len(info) > 0:
                assoc_ue_slice_conf_table.append(info)
        print(tabulate(assoc_ue_slice_conf_table, headers=assoc_ue_conf_col_names))

####################
####  print_slice_conf_json
####################
def print_slice_conf_json(conf):
    """
    print_slice_conf_json(conf):
        Print the slice configuration in JSON.

    Parameters:
        conf: slice configuration (ex: xapp.ex_slice_conf_addmod_nvs).
    """
    json_formatted_str = json.dumps(conf, indent=2)
    print(json_formatted_str)

####################
####  print_kpm_stats
####################
# len_table_str = 0
def print_kpm_stats(n_idx):
    """
    print_kpm_stats(n_idx):
        Print KPM stats (UE_ID_E2SM) for all the UEs in table.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
    """
    global _global_kpm_stats
    kpm_stats = _global_kpm_stats[n_idx]
    col_data = []
    col_name = ["Format", "Latency"]
    for ue in kpm_stats["UEs"]:
        tmp = [kpm_stats["Format"], kpm_stats["Latency"]]
        # UEs
        for ue_id_key in ue["UE_ID"].keys():
            if kpm_stats["UEs"].index(ue) == 0:
                col_name.append(ue_id_key)
            tmp.append(ue["UE_ID"][ue_id_key])
        col_data.append(tmp)
    print(tabulate(col_data, headers=col_name, tablefmt="grid"))

####################
####  print_kpm_stats_ue
####################
def print_kpm_stats_ue(n_idx, ue_idx):
    """
    print_kpm_stats_ue(n_idx, ue_idx):
        Print KPM stats (Measurement Records) for the specific UE in table.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
        ue_idx: index of the UE, you can get the index by calling print_kpm_stats().
    """
    global _global_kpm_stats
    kpm_stats = _global_kpm_stats[n_idx]
    # RAN
    col_data = []
    col_name = ["Format", "Latency", "idx"]
    for data in kpm_stats["UEs"][ue_idx]["Measurements"]:
        tmp = [kpm_stats["Format"], kpm_stats["Latency"], kpm_stats["UEs"][ue_idx]["UE_ID"]["idx"]]

        for meas_key in data.keys():
            if meas_key == "data":
                for name_id_value in data["data"]:
                    col_name.append(name_id_value["name/id"])
                    tmp.append(name_id_value["value"])
        col_data.append(tmp)
    print(tabulate(col_data, headers=col_name, tablefmt="grid"))

#     global len_table_str
#     print_table = tabulate(kpm_stats_table, headers=kpm_ind_col_names, tablefmt="grid")
#     table_str = str(print_table).ljust(len(print_table))
#     len_table_str = len(print_table)
#     print(table_str, end="\r")
#     print("\n")

####################
####  print_kpm_stats_json
####################
def print_kpm_stats_json(n_idx):
    """
    print_kpm_stats_json(n_idx):
        Print KPM stats from the specific E2-Node in JSON.

    Parameters:
        n_idx: index of the connected E2-Node, you can get the index by calling print_e2_nodes().
    """
    global _global_kpm_stats
    k = _global_kpm_stats[n_idx]
    json_formatted_str = json.dumps(k, indent=2)
    print(json_formatted_str)

# def print_kpm_stats_loop(n_idx, n_loop):
#     global len_table_str
#     for i in range(0, n_loop):
#         os.system('cls' if os.name == 'nt' else 'clear')
#         print_kpm_stats(n_idx)
#         time.sleep(1)

####################
####  END
####################
def end():
    """
    end():
        Stop xApp.
    """
    global _slice_hndlr
    global _mac_hndlr
    global _kpm_hndlr
    conn = ric.conn_e2_nodes()
    for n in conn:
        key = _gen_e2nodeid_key(n.id)
        if key in _mac_hndlr:
            for i in range(0, len(_mac_hndlr[key])):
                ric.rm_report_mac_sm(_mac_hndlr[key][i])
        if key in _slice_hndlr:
            for i in range(0, len(_slice_hndlr[key])):
                ric.rm_report_slice_sm(_slice_hndlr[key][i])
        if key in _kpm_hndlr:
            for i in range(0, len(_kpm_hndlr[key])):
                ric.rm_report_kpm_sm(_kpm_hndlr[key][i])

    while ric.try_stop == 0:
        time.sleep(1)
    print('Test finished')