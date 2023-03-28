import xapp_sdk as ric

import time

import json, snappy, requests, traceback, queue
from concurrent.futures import ThreadPoolExecutor

import signal
from multiprocessing import Process, Queue


##############
# SUBSCRIBER #
##############
class SubscriptionHelper:
    def __init__(self, e2_id, target_metrics=[], test_checkpoint=0):
        # Inject E2's metadata
        self.mcc = e2_id[0]
        self.mnc = e2_id[1]
        self.nb_id = e2_id[2]
        self.cu_du_id = e2_id[3]

        # Inject targeted metrics
        self.target_metrics = target_metrics

        # For message latency reporting
        self.test_checkpoint = test_checkpoint
        self.test_counter = 0
        self.maxdiff_ms = 0

    def report_latency(self, sm, tstamp_ms):
        if self.test_checkpoint > 0:
            if self.test_counter < self.test_checkpoint:
                self.maxdiff_ms = max(self.maxdiff_ms, time.time_ns()//1_000_000 - tstamp_ms)
                self.test_counter += 1
            else:
                print(f"[{self.mcc}-{self.mnc}-{self.nb_id}-{self.cu_du_id}: {sm}] \
                      MaxLatency: {self.maxdiff_ms} (ms).")
                self.maxdiff_ms = 0
                self.test_counter = 0

class MACCallback(ric.mac_cb, SubscriptionHelper):
    # Define Python class 'constructor'
    def __init__(self, e2_id, target_metrics, test_checkpoint):
        # Call C++ base class constructor
        ric.mac_cb.__init__(self)

        # Call SubscriptionHelper's constructor
        SubscriptionHelper.__init__(self, e2_id, target_metrics, test_checkpoint)

    # Override C++ method: virtual void handle(swig_mac_ind_msg_t a) = 0;
    def handle(self, ind):
        if len(ind.ue_stats) > 0:
            tstamp_ms = ind.tstamp // 1000

            try:
                global msg_queue
                for ue_stat in ind.ue_stats:
                    for metric in self.target_metrics:
                        msg_queue.put_nowait({
                            'labels': {'__name__': metric, 'rnti': f"{ue_stat.rnti}", 
                                       'nb_id': f"{self.nb_id}", 'cu_du_id': f"{self.cu_du_id}", 
                                       'mcc': f"{self.mcc}", 'mnc': f"{self.mnc}"},
                            'sample': [tstamp_ms, getattr(ue_stat, metric)]
                        })

                # Report the latencies
                self.report_latency('MAC', tstamp_ms)
            except Exception as e:
                print("[MAC-callback] Exception caught:", e)

class PDCPCallback(ric.pdcp_cb, SubscriptionHelper):
    # Define Python class 'constructor'
    def __init__(self, e2_id, target_metrics, test_checkpoint):
        # Call C++ base class constructor
        ric.pdcp_cb.__init__(self)

        # Call SubscriptionHelper's constructor
        SubscriptionHelper.__init__(self, e2_id, target_metrics, test_checkpoint)

    # Override C++ method: virtual void handle(swig_pdcp_ind_msg_t a) = 0;
    def handle(self, ind):
        if len(ind.rb_stats) > 0:
            tstamp_ms = ind.tstamp // 1000

            try:
                global msg_queue
                for rb_stat in ind.rb_stats:
                    for metric in self.target_metrics:
                        msg_queue.put_nowait({
                            'labels': {'__name__': f"pdcp_{metric}", 'rnti': f"{rb_stat.rnti}", 
                                       'nb_id': f"{self.nb_id}", 'cu_du_id': f"{self.cu_du_id}", 
                                       'mcc': f"{self.mcc}", 'mnc': f"{self.mnc}"},
                            'sample': [tstamp_ms, getattr(rb_stat, metric)]
                        })

                # Report the latencies
                self.report_latency('PDCP', tstamp_ms)
            except Exception as e:
                print("[PDCP-callback] Exception caught:", e)

def subscriber(test_checkpoint=0):
    # Targeted metrics and intervals for each Service Model
    sm_book = {
        'PDCP': {'metrics': ['rxsdu_bytes', 'txsdu_bytes'], 
                 'interval': 'Interval_ms_5'},
        'MAC':  {'metrics': ['dl_curr_tbs', 'ul_curr_tbs', 'dl_bler', 'ul_bler', 
                             'dl_mcs1', 'ul_mcs1', 'phr', 'wb_cqi'], 
                 'interval': 'Interval_ms_10'}
    }

    # E2-Node dictionary for status, subscriptions, and callback-handlers
    e2_nodes_book = {
        (505, 1, 1, 0):     {'is_on': False, 'PDCP': [True, None, None], 'MAC': [True, None, None]},
        (505, 1, 2, 21):    {'is_on': False, 'PDCP': [True, None, None], 'MAC': [False]},
        (505, 1, 2, 22):    {'is_on': False, 'PDCP': [False],            'MAC': [True, None, None]},
        (505, 1, 3584, 0):  {'is_on': False, 'PDCP': [True, None, None], 'MAC': [True, None, None]}
    }

    # For reporting cycle times during testing
    lastcycle_ms, maxcycle_ms = 0, 0
    test_counter = 0

    ric.init()
    global is_running
    while is_running:
        conns = ric.conn_e2_nodes()
        conns_ids = {
            (conn.id.plmn.mcc, conn.id.plmn.mnc, conn.id.nb_id, conn.id.cu_du_id): conn.id
            for conn in conns
        }

        # [REMARK] ignore E2-Node that is NOT in the book!
        for e2_id, e2_sub in e2_nodes_book.items():
            if e2_sub['is_on'] and (not e2_id in conns_ids):
                e2_sub['is_on'] = False

                for sm in ['PDCP', 'MAC']:
                    if e2_sub[sm][0]:
                        # Cannot unsubscribe from a dead E2-Node
                        # => TODO: Fix at FlexRIC
                        e2_sub[sm][2] = None

                print(f"[RICMon] Drop handlers of E2-Node: {e2_id}!")

            if (not e2_sub['is_on']) and (e2_id in conns_ids):
                e2_sub['is_on'] = True

                for sm in ['PDCP', 'MAC']:
                    if e2_sub[sm][0]:
                        if e2_sub[sm][1] is None:
                            e2_sub[sm][1] = globals()[f"{sm}Callback"](
                                                e2_id, target_metrics=sm_book[sm]['metrics'], 
                                                test_checkpoint=test_checkpoint
                                            )
                        e2_sub[sm][2] = getattr(ric, f"report_{sm.lower()}_sm")(
                                            conns_ids[e2_id], 
                                            getattr(ric, sm_book[sm]['interval']), 
                                            e2_sub[sm][1]
                                        )

                print(f"[RICMon] Subscribed to E2-Node: {e2_id}!")

        # Report the cycle times
        if test_checkpoint > 0:
            if test_counter != 0:
                maxcycle_ms = max(maxcycle_ms, time.time_ns()//1_000_000 - lastcycle_ms)
            lastcycle_ms = time.time_ns()//1_000_000

            if test_counter < test_checkpoint*1000:
                test_counter += 1
            else:
                print(f"[RICMon] conn_e2_nodes(): {list(conns_ids.keys())};")
                print(f"\tmax-cycled every: {maxcycle_ms} (ms).")
                test_counter = 0
                maxcycle_ms = 0

    # When exiting, unsubscribe all existing handlers
    print('[Ctrl+C] Stopping the xApp...')
    for _, e2_sub in e2_nodes_book.items():
        e2_sub['is_on'] = False

        for sm in ['PDCP', 'MAC']:
            if e2_sub[sm][0]:
                e2_sub[sm][1] = None

                if e2_sub[sm][2]:
                    getattr(ric, f"rm_report_{sm.lower()}_sm")(e2_sub[sm][2])
                    e2_sub[sm][2] = None

    # ... and close the shared Queue
    global msg_queue
    msg_queue.close()

    # Stopping the SDK...
    print('[Ctrl+C] Stopping the SDK...')
    while ric.try_stop == 0:
        time.sleep(0.01)


##########
# WRITER #
##########
def publisher(idx, session, tasks):
    promscale_url = 'http://172.21.13.12:9201/write'

    # Hash-based MAP
    for t in tasks:
        t['hash'] = '-'.join(t['labels'].values())

    # Hash-based SHUFFLE
    hashed_dict = {}
    for t in tasks:
        t_key = t['hash']
        if not (t_key in hashed_dict):
            hashed_dict[t_key] = {'labels': t['labels'], 'samples': [t['sample']]}
        else:
            hashed_dict[t_key]['samples'].append(t['sample'])

    # Snappy Compression
    payload = ''.join([json.dumps(d) for d in hashed_dict.values()])
    snappy_payload = snappy.compress(payload.encode('utf-8'))

    # POST to Promscale (exception-prone)
    try:
        response = session.post(promscale_url, data=snappy_payload, timeout=1)
        if response.status_code != 200:
            print(f"\n\t[PUSHER-{idx}] ERROR returned by Promscale!!!\n")
    except requests.exceptions.Timeout:
        print(f"\n\t[PUSHER-{idx}] Pushing to Promscale takes too long!!!\n")
    except Exception:
        traceback.print_exc()
    finally:
        response.close()

def stats_writer(is_test=False):
    # Set #threads and batch_size
    num_threads = 8
    batch_size = 256

    # Session (connection pool) sharing
    session = requests.Session()
    session.mount('http://', requests.adapters.HTTPAdapter(max_retries=0, pool_block=True,
                  pool_connections=2, pool_maxsize=num_threads))
    session.headers.update({'Content-Type': 'application/json', 'Content-Encoding': 'snappy'})

    global is_running, msg_queue
    while is_running:
        if msg_queue.qsize() >= num_threads*batch_size:
            try:
                messages = [msg_queue.get(timeout=1) for _ in range(num_threads*batch_size)]
            except queue.Empty as error:
                print("[Ctrl+C?] Not enough messages...")
                break

            if is_test:
                begint_ms = messages[0]['sample'][0]

            # SPLIT before Map-Shuffle
            # TODO: Use multiple external thread-pools instead.
            with ThreadPoolExecutor(num_threads) as executor:
                for idx in range(num_threads):
                    executor.submit(publisher, idx, session, 
                                    messages[idx*batch_size : (idx+1)*batch_size])

            if is_test:
                endt_ms = time.time_ns() // 1_000_000
                print(f"[RICMon] Max-delay of {num_threads*batch_size} messages: \
                      {endt_ms-begint_ms} (ms).")

    print("[Ctrl+C] Clearing the shared queues...")
    while not msg_queue.empty():
        msg_queue.get()

    session.close()


########
# MAIN #
########
def signal_handler(signal, frame):
    global is_running
    is_running = False

if __name__ == '__main__':
    global is_running, msg_queue
    is_running = True
    msg_queue = Queue()

    signal.signal(signal.SIGINT, signal_handler)

    stats_writer_proc = Process(target=stats_writer, kwargs={'is_test': False})
    subscriber_proc = Process(target=subscriber, kwargs={'test_checkpoint': 0})

    stats_writer_proc.start()
    subscriber_proc.start()

    subscriber_proc.join()
    stats_writer_proc.join()
