import xapp_sdk as ric
import time
import pdb
import json

####################
####  SLICE INDICATION MSG TO JSON
####################

def slice_ind_to_dict_json(ind):

    slice_stats = {
        "RAN" : {
            "dl" : {}
            # TODO: handle the ul slice stats, currently there is no ul slice stats in database(SLICE table)
            # "ul" : {}
        },
        "UE" : {}
    }

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
    global assoc_rnti
    ue_dict = slice_stats["UE"]
    if ind.ue_slice_stats.len_ue_slice <= 0:
        ue_dict["num_of_ues"] = ind.ue_slice_stats.len_ue_slice
    else:
        ue_dict["num_of_ues"] = ind.ue_slice_stats.len_ue_slice
        ue_dict["ues"] = []
        for u in ind.ue_slice_stats.ues:
            ues_dict = {}
            dl_id = "null"
            if u.dl_id >= 0 and dl_dict["num_of_slices"] > 0:
                dl_id = u.dl_id
            ues_dict = {
                "rnti" : hex(u.rnti),
                "assoc_dl_slice_id" : dl_id
                # TODO: handle the associated ul slice id, currently there is no ul slice id in database(UE_SLICE table)
                # "assoc_ul_slice_id" : ul_id
            }
            ue_dict["ues"].append(ues_dict)
            assoc_rnti = u.rnti

    ind_dict = slice_stats
    ind_json = json.dumps(ind_dict)

    with open("rt_slice_stats.json", "w") as outfile:
        outfile.write(ind_json)
    # print(ind_dict)

    return json

####################
#### SLICE INDICATION CALLBACK
####################

class SLICECallback(ric.slice_cb):
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
        slice_ind_to_dict_json(ind)

####################
####  SLICE CONTROL FUNCS
####################
def create_slice(slice_params, slice_sched_algo):
    s = ric.fr_slice_t()
    s.id = slice_params["id"]
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
        if slice_params["type"] == "SLICE_SM_NVS_V0_RATE":
            s.params.u.nvs.conf = ric.SLICE_SM_NVS_V0_RATE
            s.params.u.nvs.u.rate.u1.mbps_required = slice_params["slice_algo_params"]["mbps_rsvd"]
            s.params.u.nvs.u.rate.u2.mbps_reference = slice_params["slice_algo_params"]["mbps_ref"]
            # print("ADD NVS DL SLCIE: id", s.id,
            # ", conf", s.params.u.nvs.conf,
            # ", mbps_rsrv", s.params.u.nvs.u.rate.u1.mbps_required,
            # ", mbps_ref", s.params.u.nvs.u.rate.u2.mbps_reference)
        elif slice_params["type"] == "SLICE_SM_NVS_V0_CAPACITY":
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
####  SLICE CONTROL PARAMETER EXAMPLE - ADD SLICE
####################
add_static_slices = {
    "num_slices" : 3,
    "slice_sched_algo" : "STATIC",
    "slices" : [
        {
            "id" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "slice_algo_params" : {"pos_low" : 0, "pos_high" : 2},
        },
        {
            "id" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "slice_algo_params" : {"pos_low" : 3, "pos_high" : 10},
        },
        {
            "id" : 5,
            "label" : "s3",
            "ue_sched_algo" : "PF",
            "slice_algo_params" : {"pos_low" : 11, "pos_high" : 13},
        }
    ]
}

add_nvs_slices_rate = {
    "num_slices" : 2,
    "slice_sched_algo" : "NVS",
    "slices" : [
        {
            "id" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"mbps_rsvd" : 60, "mbps_ref" : 120},
        },
        {
            "id" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"mbps_rsvd" : 60, "mbps_ref" : 120},
        }
    ]
}

add_nvs_slices_cap = {
    "num_slices" : 3,
    "slice_sched_algo" : "NVS",
    "slices" : [
        {
            "id" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"pct_rsvd" : 0.5},
        },
        {
            "id" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"pct_rsvd" : 0.3},
        },
        {
            "id" : 5,
            "label" : "s3",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"pct_rsvd" : 0.2},
        }
    ]
}

add_nvs_slices = {
    "num_slices" : 3,
    "slice_sched_algo" : "NVS",
    "slices" : [
        {
            "id" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_CAPACITY",
            "slice_algo_params" : {"pct_rsvd" : 0.5},
        },
        {
            "id" : 2,
            "label" : "s2",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"mbps_rsvd" : 50, "mbps_ref" : 120},
        },
        {
            "id" : 5,
            "label" : "s3",
            "ue_sched_algo" : "PF",
            "type" : "SLICE_SM_NVS_V0_RATE",
            "slice_algo_params" : {"mbps_rsvd" : 5, "mbps_ref" : 120},
        }
    ]
}

add_edf_slices = {
    "num_slices" : 3,
    "slice_sched_algo" : "EDF",
    "slices" : [
        {
            "id" : 0,
            "label" : "s1",
            "ue_sched_algo" : "PF",
            "slice_algo_params" : {"deadline" : 10, "guaranteed_prbs" : 20, "max_replenish" : 0},
        },
        {
            "id" : 2,
            "label" : "s2",
            "ue_sched_algo" : "RR",
            "slice_algo_params" : {"deadline" : 20, "guaranteed_prbs" : 20, "max_replenish" : 0},
        },
        {
            "id" : 5,
            "label" : "s3",
            "ue_sched_algo" : "MT",
            "slice_algo_params" : {"deadline" : 40, "guaranteed_prbs" : 10, "max_replenish" : 0},
        }
    ]
}

reset_slices = {
    "num_slices" : 0
}

####################
####  SLICE CONTROL PARAMETER EXAMPLE - DELETE SLICE
####################
delete_slices = {
    "num_dl_slices" : 1,
    "delete_dl_slice_id" : [5]
}

####################
####  SLICE CONTROL PARAMETER EXAMPLE - ASSOC UE SLICE
####################
assoc_ue_slice = {
    "num_ues" : 1,
    "ues" : [
        {
            "rnti" : 0, # TODO: get rnti from slice_ind_to_dict_json()
            "assoc_dl_slice_id" : 2
        }
    ]
}


def fill_slice_ctrl_msg(ctrl_type, ctrl_msg):
    msg = ric.slice_ctrl_msg_t()
    if (ctrl_type == "ADDMOD"):
        msg.type = ric.SLICE_CTRL_SM_V0_ADD
        dl = ric.ul_dl_slice_conf_t()
        # TODO: UL SLICE CTRL ADD
        # ul = ric.ul_dl_slice_conf_t()

        # ue_sched_algo can be "RR"(round-robin), "PF"(proportional fair) or "MT"(maximum throughput) and it has to be set in any len_slices
        ue_sched_algo = "PF"
        dl.sched_name = ue_sched_algo
        dl.len_sched_name = len(ue_sched_algo)

        dl.len_slices = ctrl_msg["num_slices"]
        slices = ric.slice_array(ctrl_msg["num_slices"])
        for i in range(0, ctrl_msg["num_slices"]):
            slices[i] = create_slice(ctrl_msg["slices"][i], ctrl_msg["slice_sched_algo"])

        dl.slices = slices
        msg.u.add_mod_slice.dl = dl
        # TODO: UL SLICE CTRL ADD
        # msg.u.add_mod_slice.ul = ul;
    elif (ctrl_type == "DEL"):
        msg.type = ric.SLICE_CTRL_SM_V0_DEL

        msg.u.del_slice.len_dl = ctrl_msg["num_dl_slices"]
        del_dl_id = ric.del_dl_array(ctrl_msg["num_dl_slices"])
        for i in range(ctrl_msg["num_dl_slices"]):
            del_dl_id[i] = ctrl_msg["delete_dl_slice_id"][i]
        # print("DEL DL SLICE: id", del_dl_id)

        # TODO: UL SLCIE CTRL DEL
        msg.u.del_slice.dl = del_dl_id
    elif (ctrl_type == "ASSOC_UE_SLICE"):
        msg.type = ric.SLICE_CTRL_SM_V0_UE_SLICE_ASSOC

        msg.u.ue_slice.len_ue_slice = ctrl_msg["num_ues"]
        assoc = ric.ue_slice_assoc_array(ctrl_msg["num_ues"])
        for i in range(ctrl_msg["num_ues"]):
            a = ric.ue_slice_assoc_t()
            a.rnti = assoc_rnti # TODO: assign the rnti after get the indication msg from slice_ind_to_dict_json()
            a.dl_id = ctrl_msg["ues"][i]["assoc_dl_slice_id"]
            # TODO: UL SLICE CTRL ASSOC
            # a.ul_id = 0
            assoc[i] = a
            # print("ASSOC DL SLICE: <rnti:", a.rnti, "(NEED TO FIX)>, id", a.dl_id)
        msg.u.ue_slice.ues = assoc

    return msg


####################
####  GENERAL
####################

ric.init()

conn = ric.conn_e2_nodes()
assert(len(conn) > 0)

node_idx = 0
#for i in range(0, len(conn)):
#    if conn[i].id.plmn.mcc == 1:
#        node_idx = i

#print("Global E2 Node [" + str(node_idx) + "]: PLMN MCC = " + str(conn[node_idx].id.plmn.mcc))
#print("Global E2 Node [" + str(node_idx) + "]: PLMN MNC = " + str(conn[node_idx].id.plmn.mnc))

####################
#### SLICE INDICATION
####################

slice_cb = SLICECallback()
hndlr = ric.report_slice_sm(conn[node_idx].id, ric.Interval_ms_5, slice_cb)
time.sleep(5)

####################
####  SLICE CTRL ADD
####################

msg = fill_slice_ctrl_msg("ADDMOD", add_static_slices)
ric.control_slice_sm(conn[node_idx].id, msg)
time.sleep(20)

####################
####  SLICE CTRL ASSOC
####################

msg = fill_slice_ctrl_msg("ASSOC_UE_SLICE", assoc_ue_slice)
ric.control_slice_sm(conn[node_idx].id, msg)
time.sleep(20)

####################
####  SLICE CTRL DEL
####################

msg = fill_slice_ctrl_msg("DEL", delete_slices)
ric.control_slice_sm(conn[node_idx].id, msg)
time.sleep(10)

####################
####  SLICE CTRL RESET
####################

msg = fill_slice_ctrl_msg("ADDMOD", reset_slices)
ric.control_slice_sm(conn[node_idx].id, msg)
time.sleep(5)

with open("rt_slice_stats.json", "w") as outfile:
    outfile.write(json.dumps({}))

### End
ric.rm_report_slice_sm(hndlr)

# Avoid deadlock. ToDo revise architecture 
while ric.try_stop == 0:
    time.sleep(1)

print('Test finished' )

