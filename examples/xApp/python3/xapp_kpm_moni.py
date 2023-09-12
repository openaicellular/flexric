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
        now = int(time.time() * 1000000)
        ts = 0
        # MeasData
        if len(ind.MeasData) > 0:
            if len(ind.MeasData[0].measRecord) > 0:
                ts = ind.MeasData[0].measRecord[0].real_val

        # MeasInfo
        for i in range(0, ind.MeasInfo_len):
            meas_infotype = ind.MeasInfo[i].meas_type
            measName = ind.MeasInfo[i].measName
            measID = ind.MeasInfo[i].measID
            measRecord = ind.MeasData[0].measRecord[i].real_val
            meas_recordtype = ind.MeasData[0].measRecord[i].type # need to check why type is equal to 0 when is real
            print(f"KPM ind_msg latency = {now - ts} us, "
                  f"meas_infotype = {meas_infotype}, "
                  f"measName = {measName}, "
                  f"measRecord = {measRecord}, "
                  f"meas_recordtype = {meas_recordtype}, "
                  f"from E2 node type = {ind.id.type} nb_id = {ind.id.nb_id.nb_id}")


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
    hndlr = ric.report_kpm_sm(conn[i].id, ric.Interval_ms_5, kpm_cb)
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
