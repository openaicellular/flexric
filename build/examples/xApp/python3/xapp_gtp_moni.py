import xapp_sdk as ric
import time
import os
import pdb

####################
#### GTP INDICATION CALLBACK
####################

# Create a callback for GTP which derived it from C++ class gtp_cb
class GTPCallback(ric.gtp_cb):
    def __init__(self):
        # Inherit C++ gtp_cb class
        ric.gtp_cb.__init__(self)
    # Create an override C++ method 
    def handle(self, ind):
        if len(ind.gtp_stats) > 0:
            t_now = time.time_ns() / 1000.0
            t_gtp = ind.tstamp / 1.0
            t_diff = t_now - t_gtp
            print('GTP Indication tstamp = ' + str(ind.tstamp) + ' diff = ' + str(t_diff))

####################
####  GENERAL 
####################

ric.init()

conn = ric.conn_e2_nodes()
assert(len(conn) > 0)
for i in range(0, len(conn)):
    print("Global E2 Node [" + str(i) + "]: PLMN MCC = " + str(conn[i].id.plmn.mcc))
    print("Global E2 Node [" + str(i) + "]: PLMN MNC = " + str(conn[i].id.plmn.mnc))

####################
#### GTP INDICATION
####################

gtp_hndlr = []

for i in range(0, len(conn)):
    gtp_cb = GTPCallback()
    hndlr = ric.report_gtp_sm(conn[i].id, ric.Interval_ms_1, gtp_cb)
    gtp_hndlr.append(hndlr)
    time.sleep(1)


time.sleep(10)

### End

for i in range(0, len(gtp_hndlr)):
    ric.rm_report_gtp_sm(gtp_hndlr[i])

# Avoid deadlock. ToDo revise architecture 
while ric.try_stop == 0:
    time.sleep(1)

