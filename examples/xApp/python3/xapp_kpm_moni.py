import xapp_sdk as ric
import time
import os
import pdb
import time
import signal
import sys

exit_flag = False

####################
#### KPM INDICATION CALLBACK
####################

# Create a callback for KPM which derived it from C++ class kpm_cb
class KPMCallback(ric.kpm_cb):
    def __init__(self):
        # Inherit C++ kpm_cb class
        ric.kpm_cb.__init__(self)
    # Create an override C++ method 
    def handle(self, ind):
        if ind.hdr:
            t_now = time.time_ns() / 1000.0
            t_kpm = ind.hdr.kpm_ric_ind_hdr_format_1.collectStartTime / 1.0
            t_diff = t_now - t_kpm
            print(f"KPM Indication tstamp {t_now} diff {t_diff} E2-node type {ind.id.type} nb_id {ind.id.nb_id.nb_id}")
            if ind.hdr.kpm_ric_ind_hdr_format_1.fileformat_version:
                print(f"fileformat_version {ind.hdr.kpm_ric_ind_hdr_format_1.fileformat_version}")
            if ind.hdr.kpm_ric_ind_hdr_format_1.sender_name:
                print(f"sender_name {ind.hdr.kpm_ric_ind_hdr_format_1.sender_name}")
            if ind.hdr.kpm_ric_ind_hdr_format_1.sender_type:
                print(f"sender_type {ind.hdr.kpm_ric_ind_hdr_format_1.sender_type}")
            if ind.hdr.kpm_ric_ind_hdr_format_1.vendor_name:
                print(f"vendor_name {ind.hdr.kpm_ric_ind_hdr_format_1.vendor_name}")



####################
####  GENERAL 
####################

ric.init(sys.argv)

conn = ric.conn_e2_nodes()
assert(len(conn) > 0)

print("Connected E2 nodes =", len(conn))
for i in range(0, len(conn)):
    print("Global E2 Node [" + str(i) + "]: PLMN MCC = " + str(conn[i].id.plmn.mcc))
    print("Global E2 Node [" + str(i) + "]: PLMN MNC = " + str(conn[i].id.plmn.mnc))


####################
#### KPM INDICATION
####################

kpm_hndlr = []

for i in range(0, len(conn)):
    kpm_cb = KPMCallback()
    action = ["DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL", "DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl"]
    hndlr = ric.report_kpm_sm(conn[i].id, ric.Interval_ms_1000, action, kpm_cb)
    kpm_hndlr.append(hndlr)
    time.sleep(1)


time.sleep(10)

### End

for i in range(0, len(kpm_hndlr)):
    ric.rm_report_kpm_sm(kpm_hndlr[i])

# Avoid deadlock. ToDo revise architecture 
while ric.try_stop == 0:
    time.sleep(1)

print("Test xApp run SUCCESSFULLY")
