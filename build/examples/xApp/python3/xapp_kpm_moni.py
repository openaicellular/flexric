import xapp_sdk as ric
import time
import os
import pdb

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
        t_now = time.time() # in second
        t_kpm = ind.tstamp.collectStartTime/1.0
        t_diff = t_now - t_kpm

        #print('latencyy <1 second' if t_diff<1.0 else 'latency =' + str(t_diff))
        #print('KPM Indication collectstarttime = ' + str(ind.tstamp.collectStartTime) + ' diff = ' + str(t_diff))

        word =  ind.kpm_stats.MeasInfo.measName
        val = word.split()
        
        for i in range(ind.kpm_stats.MeasData.measRecord_len):
            print(str(val[i]) + ' = ' + str(ind.kpm_stats.MeasData.MeasRecord[i].int_val))
            
        print("rlcthroughput= " + str(((ind.kpm_stats.MeasData.MeasRecord[2].int_val)-(ind.kpm_stats.MeasData.MeasRecord[1].int_val))/1.0))
        print("pdcpthroughput= " + str(((ind.kpm_stats.MeasData.MeasRecord[4].int_val)-(ind.kpm_stats.MeasData.MeasRecord[3].int_val))/1.0))
        print('-----------------------')

        
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
#### KPM INDICATION
####################

kpm_hndlr = []

for i in range(0, len(conn)):
    kpm_cb = KPMCallback()
    hndlr = ric.report_kpm_sm(conn[i].id, ric.Interval_ms_1, kpm_cb)
    kpm_hndlr.append(hndlr)
    time.sleep(1)
    print(kpm_hndlr)


time.sleep(10)

### End

for i in range(0, len(kpm_hndlr)):
    ric.rm_report_kpm_sm(kpm_hndlr[i])

 


# Avoid deadlock. ToDo revise architecture
while ric.try_stop == 0:
    time.sleep(1)
