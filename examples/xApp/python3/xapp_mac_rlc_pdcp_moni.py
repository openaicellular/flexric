import xapp_sdk as ric
import time
import os
import pdb
import csv


####################
#### MAC INDICATION CALLBACK
####################

# MACCallback class is defined and derived from C++ class mac_cb
class MACCallback(ric.mac_cb):
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
            print(f"MAC Indication tstamp {t_now} diff {t_diff} E2-node type {ind.id.type} nb_id {ind.id.nb_id}")
            # print('MAC rnti = ' + str(ind.ue_stats[0].rnti))

####################
#### RLC INDICATION CALLBACK
####################

class RLCCallback(ric.rlc_cb):
    # Define Python class 'constructor'
    def __init__(self):
        # Call C++ base class constructor
        ric.rlc_cb.__init__(self)
    # Override C++ method: virtual void handle(swig_rlc_ind_msg_t a) = 0;
    def handle(self, ind):
        # Print swig_rlc_ind_msg_t
        if len(ind.rb_stats) > 0:
            t_now = time.time_ns() / 1000.0
            t_rlc = ind.tstamp / 1.0
            t_diff= t_now - t_rlc
            print(f"RLC Indication tstamp {t_now} diff {t_diff} E2-node type {ind.id.type} nb_id {ind.id.nb_id}")
            # print('RLC rnti = '+ str(ind.rb_stats[0].rnti))

####################
#### PDCP INDICATION CALLBACK
####################

class PDCPCallback(ric.pdcp_cb):
    # Define Python class 'constructor'
    def __init__(self):
        # Call C++ base class constructor
        ric.pdcp_cb.__init__(self)
   # Override C++ method: virtual void handle(swig_pdcp_ind_msg_t a) = 0;
    def handle(self, ind):
        # Print swig_pdcp_ind_msg_t
        if len(ind.rb_stats) > 0:
            t_now = time.time_ns() / 1000.0
            t_pdcp = ind.tstamp / 1.0
            t_diff = t_now - t_pdcp
            print(f"PDCP Indication tstamp {t_now} diff {t_diff} E2-node type {ind.id.type} nb_id {ind.id.nb_id}")
            # print('PDCP rnti = '+ str(ind.rb_stats[0].rnti))

def send_sub_req(sm_name, id, cb):
    global mac_hndlr
    global rlc_hndlr
    global pdcp_hndlr
    if sm_name == "MAC":
        hndlr = ric.report_mac_sm(id, ric.Interval_ms_1, cb)
        mac_hndlr.append(hndlr)
    elif sm_name == "RLC":
        hndlr = ric.report_rlc_sm(id, ric.Interval_ms_1, cb)
        rlc_hndlr.append(hndlr)
    elif sm_name == "PDCP":
        hndlr = ric.report_pdcp_sm(id, ric.Interval_ms_1, cb)
        pdcp_hndlr.append(hndlr)
    else:
        print("Unknow SM")

####################
####  GENERAL 
####################
if __name__ == '__main__':

    mac_hndlr = []
    rlc_hndlr = []
    pdcp_hndlr = []
    # Start
    ric.init()
    conn = ric.conn_e2_nodes()
    assert(len(conn) > 0)
    for i in range(0, len(conn)):
        print("Global E2 Node [" + str(i) + "]: PLMN MCC = " + str(conn[i].id.plmn.mcc))
        print("Global E2 Node [" + str(i) + "]: PLMN MNC = " + str(conn[i].id.plmn.mnc))

    sm_name = "MAC"
    for i in range(0, len(conn)):
        mac_cb = MACCallback()
        send_sub_req(sm_name, conn[i].id, mac_cb)
    sm_name = "RLC"
    for i in range(0, len(conn)):
        rlc_cb = RLCCallback()
        send_sub_req(sm_name, conn[i].id, rlc_cb)
    sm_name = "PDCP"
    for i in range(0, len(conn)):
        pdcp_cb = PDCPCallback()
        send_sub_req(sm_name, conn[i].id, pdcp_cb)

    time.sleep(10)

    ### End
    for i in range(0, len(mac_hndlr)):
        ric.rm_report_mac_sm(mac_hndlr[i])

    for i in range(0, len(rlc_hndlr)):
        ric.rm_report_rlc_sm(rlc_hndlr[i])

    for i in range(0, len(pdcp_hndlr)):
        ric.rm_report_pdcp_sm(pdcp_hndlr[i])

    # Avoid deadlock. ToDo revise architecture
    while ric.try_stop == 0:
        time.sleep(1)

    print("Test finished")
