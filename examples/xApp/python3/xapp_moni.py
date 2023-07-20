import xapp_sdk as ric
import time
import os
import pdb
from tabulate import tabulate
import signal
import sys

def sig_handler(signum, frame):
    print("Ctrl-C Detect")
    exit(1)


count_max = 100
####################
#### MAC INDICATION CALLBACK
####################
count_mac = 0
aggr_tstamp_mac = 0
#  MACCallback class is defined and derived from C++ class mac_cb
class MACCallback(ric.mac_cb):
    # Define Python class 'constructor'
    def __init__(self):
        # Call C++ base class constructor
        ric.mac_cb.__init__(self)
    # Override C++ method: virtual void handle(swig_mac_ind_msg_t a) = 0;
    def handle(self, ind):
        # Print swig_mac_ind_msg_t
        if len(ind.ue_stats) > 0:
            global count_mac, aggr_tstamp_mac
            count_mac += 1
            t_now = time.time_ns() / 1000.0
            t_mac = ind.tstamp / 1.0
            t_diff = t_now - t_mac
            aggr_tstamp_mac += t_diff
            if count_mac == count_max:
                print(f"MAC Indication avg time diff {aggr_tstamp_mac/count_max} e2 node type {ind.id.type} nb_id {ind.id.nb_id}")
                count_mac = 0
                aggr_tstamp_mac = 0
            # print('MAC rnti = ' + str(ind.ue_stats[0].rnti))

####################
#### RLC INDICATION CALLBACK
####################
count_rlc = 0
aggr_tstamp_rlc = 0
class RLCCallback(ric.rlc_cb):
    # Define Python class 'constructor'
    def __init__(self):
        # Call C++ base class constructor
        ric.rlc_cb.__init__(self)
    # Override C++ method: virtual void handle(swig_rlc_ind_msg_t a) = 0;
    def handle(self, ind):
        # Print swig_rlc_ind_msg_t
        if len(ind.rb_stats) > 0:
            global count_rlc, aggr_tstamp_rlc
            count_rlc += 1
            t_now = time.time_ns() / 1000.0
            t_rlc = ind.tstamp / 1.0
            t_diff = t_now - t_rlc
            aggr_tstamp_rlc += t_diff
            if count_rlc == count_max:
                print('RLC Indication avg time diff = ' + str(aggr_tstamp_rlc/count_max))
                count_rlc = 0
                aggr_tstamp_rlc = 0
            # print('RLC rnti = '+ str(ind.rb_stats[0].rnti))

####################
#### PDCP INDICATION CALLBACK
####################
count_pdcp = 0
aggr_tstamp_pdcp = 0
class PDCPCallback(ric.pdcp_cb):
    # Define Python class 'constructor'
    def __init__(self):
        # Call C++ base class constructor
        ric.pdcp_cb.__init__(self)
    # Override C++ method: virtual void handle(swig_pdcp_ind_msg_t a) = 0;
    def handle(self, ind):
        # Print swig_pdcp_ind_msg_t
        if len(ind.rb_stats) > 0:
            global count_pdcp, aggr_tstamp_pdcp
            count_pdcp += 1
            t_now = time.time_ns() / 1000.0
            t_pdcp = ind.tstamp / 1.0
            t_diff = t_now - t_pdcp
            aggr_tstamp_pdcp += t_diff
            if count_pdcp == count_max:
                print('PDCP Indication avg time diff = ' + str(aggr_tstamp_pdcp/count_max))
                count_pdcp = 0
                aggr_tstamp_pdcp = 0
            # print('PDCP rnti = '+ str(ind.rb_stats[0].rnti))

####################
#### UPDATE CONNECTED E2 NODES
####################
def get_e2_nodes():
    return ric.conn_e2_nodes()

####################
#### SEND SUBSCRIPTION REQUEST
####################
mac_cb = 0
rlc_cb = 0
pdcp_cb = 0
def send_subscription_req(nodes):
    tti = ric.Interval_ms_10
    if nodes.id.type == 2 or nodes.id.type == 7:
        global mac_cb
        global mac_hndlr
        mac_cb = MACCallback()
        hndlr = ric.report_mac_sm(nodes.id, tti, mac_cb)
        mac_hndlr.append(hndlr)
        global rlc_cb
        global rlc_hndlr
        rlc_cb = RLCCallback()
        hndlr = ric.report_rlc_sm(nodes.id, tti, rlc_cb)
        rlc_hndlr.append(hndlr)
    if nodes.id.type == 2 or nodes.id.type == 5:
        global pdcp_cb
        global pdcp_hndlr
        pdcp_cb = PDCPCallback()
        hndlr = ric.report_pdcp_sm(nodes.id, tti, pdcp_cb)
        pdcp_hndlr.append(hndlr)

####################
#### GET NGRAN TYPE IN STRING
####################
def get_ngran_name(ran_type):
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
#### PRINT E2 NODES IN TABLE
####################
e2nodes_col_names = ["idx", "nb_id", "mcc", "mnc", "ran_type"]
def print_e2_nodes():
    e2nodes_data = []
    conn = ric.conn_e2_nodes()
    for i in range(0, len(conn)):
        # TODO: need to fix cu_du_id in swig
        # cu_du_id = -1
        # if conn[i].id.cu_du_id:
        #     cu_du_id = conn[i].id.cu_du_id
        info = [i,
                conn[i].id.nb_id,
                conn[i].id.plmn.mcc,
                conn[i].id.plmn.mnc,
                get_ngran_name(conn[i].id.type)]
        # print(info)
        e2nodes_data.append(info)
    print(tabulate(e2nodes_data, headers=e2nodes_col_names, tablefmt="grid"))
    # print("E2 node : "
    #       "nb_id " + str(e2node.id.nb_id) + ",",
    #       "mcc " + str(e2node.id.plmn.mcc) + ",",
    #       "mnc " + str(e2node.id.plmn.mnc) + ",",
    #       "mnc_digit_len " + str(e2node.id.plmn.mnc_digit_len) + ",",
    #       "ran_type " + get_ngran_name(e2node.id.type) + ',',
    #       "cu_du_id " + str(e2node.id.cu_du_id if e2node.id.cu_du_id else -1))

####################
#### TUPLE LIST E2 NODES
####################
# turn each list of lists into a list of tuples,
# as tuples are hashable (lists are not) so we can convert the list of tuples into a set of tuples:
def get_e2_nodes_in_tuple_list():
    e2nodes_list = []
    conn = ric.conn_e2_nodes()
    for i in range(0, len(conn)):
        # TODO: need to fix cu_du_id in swig
        # cu_du_id = -1
        # if conn[i].id.cu_du_id:
        #     cu_du_id = conn[i].id.cu_du_id
        info = [conn[i].id.nb_id,
                conn[i].id.plmn.mcc,
                conn[i].id.plmn.mnc,
                conn[i].id.plmn.mnc_digit_len,
                get_ngran_name(conn[i].id.type)]
        # print(info)
        e2nodes_list.append(info)
    tuple_list = [tuple(lst) for lst in e2nodes_list]
    return tuple_list

####################
####  GENERAL
####################

ric.init(sys.argv)
signal.signal(signal.SIGINT, sig_handler)

e2nodes = 0
e2nodes_set = set(map(tuple, get_e2_nodes_in_tuple_list()))
e2nodes_len = len(e2nodes_set)
# print("e2nodes")
# print(e2nodes_set)
while e2nodes_len <= 0:
    temp_e2nodes_set = set(map(tuple, get_e2_nodes_in_tuple_list()))
    # print("temp_e2nodes")
    # print(temp_e2nodes_set)
    if e2nodes_set.symmetric_difference(temp_e2nodes_set):
        print("Update connected E2 nodes")
        e2nodes_set = temp_e2nodes_set
        e2nodes_len = len(e2nodes_set)
    else:
        print("No E2 node connects")
        time.sleep(1)
assert(e2nodes_len > 0)
print_e2_nodes()

while True:
    temp_e2nodes_set = set(map(tuple, get_e2_nodes_in_tuple_list()))
    if e2nodes_set.symmetric_difference(temp_e2nodes_set):
        print("Update connected E2 nodes")
        e2nodes_set = temp_e2nodes_set
        e2nodes_len = len(e2nodes_set)
        print_e2_nodes()

# mac_hndlr = []
# rlc_hndlr = []
# pdcp_hndlr = []
# for i in range(0, len(conn)):
#     send_subscription_req(conn[i])
#     time.sleep(1)
#
# time.sleep(10)
#
# ### End
# for i in range(0, len(mac_hndlr)):
#     ric.rm_report_mac_sm(mac_hndlr[i])
#
# for i in range(0, len(rlc_hndlr)):
#     ric.rm_report_rlc_sm(rlc_hndlr[i])
#
# for i in range(0, len(pdcp_hndlr)):
#     ric.rm_report_pdcp_sm(pdcp_hndlr[i])

# Avoid deadlock. ToDo revise architecture 
while ric.try_stop == 0:
    time.sleep(1)

print("Test finished")
