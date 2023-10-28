import sys
from . import xapp_sdk as ric

import time
from .helpers import sm_configs_to_e2_statuses, \
                    sm_configs_to_e2_intervals, \
                    sm_configs_to_sm_filters, \
                    CallbackHelper, ts_grouping, promscale_jsonize

from typing import List
import polars as pl
import json, snappy, requests, traceback, queue
from concurrent.futures import ThreadPoolExecutor

import signal
from multiprocessing import set_start_method, Manager, Process
from faster_fifo import Queue

# TODO: Desparately need to be splitted into 2 modules
# TO-REMOVE: for fixing SegFault
# import faulthandler
# faulthandler.enable()


##############
# SUBSCRIBER #
##############
class MACCallback(ric.mac_cb, CallbackHelper):
    # Define Python class 'constructor'
    def __init__(self, sm_name, e2_metrics, report_checkpoint=0):
        # Call C++ base class constructor
        ric.mac_cb.__init__(self)

        # Call SubscriptionHelper's constructor
        CallbackHelper.__init__(self, sm_name, e2_metrics, report_checkpoint)

    # Override C++ method: virtual void handle(swig_mac_ind_msg_t a) = 0;
    def handle(self, ind):
        if len(ind.ue_stats) > 0:
            ricmont_ms = time.time_ns()//1_000_000
            tstamp_ms = ind.tstamp//1000

            try:
                # E2-based filtering of messages, plus enqueuing
                e2_id = (ind.id.plmn.mcc, ind.id.plmn.mnc, 
                         ind.id.nb_id.nb_id, ind.id.cu_du_id)
                e2_filters = self.filters[e2_id]

                mac_data = [{
                    '__name__': metric,
                    'value': getattr(ue_stat, metric),
                    'timestamp': tstamp_ms,
                    'frame': f"{ue_stat.frame}",
                    'slot': f"{ue_stat.slot}",
                    'rnti': f"{ue_stat.rnti}",
                    'ricmonstamp': ricmont_ms,
                    'mcc': f"{e2_id[0]}",
                    'mnc': f"{e2_id[1]}",
                    'nb_id': f"{e2_id[2]}",
                    'cu_du_id': e2_id[3]
                } for ue_stat in ind.ue_stats for metric in e2_filters]

                global msg_queue
                msg_queue.put_many_nowait(mac_data)

                # Report the latencies
                if self.report_checkpoint > 0:
                    self.report_maxlatency(tstamp_ms, ricmont_ms)
            except (ConnectionResetError, BrokenPipeError):
                print("[MACCallback] No message_queue to use. Ctrl+C?")
                return
            except queue.Full:
                print("[MACCallback] Queue is full, drop some messages!")
                return
            except Exception:
                traceback.print_exc()

class PDCPCallback(ric.pdcp_cb, CallbackHelper):
    # Define Python class 'constructor'
    def __init__(self, sm_name, e2_metrics, report_checkpoint=0):
        # Call C++ base class constructor
        ric.pdcp_cb.__init__(self)

        # Call SubscriptionHelper's constructor
        CallbackHelper.__init__(self, sm_name, e2_metrics, report_checkpoint)

    # Override C++ method: virtual void handle(swig_pdcp_ind_msg_t a) = 0;
    def handle(self, ind):
        if len(ind.rb_stats) > 0:
            ricmont_ms = time.time_ns()//1_000_000
            tstamp_ms = ind.tstamp//1000

            try:
                # E2-based filtering of messages, plus enqueuing
                e2_id = (ind.id.plmn.mcc, ind.id.plmn.mnc, 
                         ind.id.nb_id.nb_id, ind.id.cu_du_id)
                e2_filters = self.filters[e2_id]

                pdcp_data = [{
                    '__name__': f"pdcp_{metric}",
                    'value': getattr(rb_stat, metric),
                    'timestamp': tstamp_ms,
                    'rnti': f"{rb_stat.rnti}",
                    'ricmonstamp': ricmont_ms,
                    'mcc': f"{e2_id[0]}",
                    'mnc': f"{e2_id[1]}",
                    'nb_id': f"{e2_id[2]}",
                    'cu_du_id': e2_id[3]
                } for rb_stat in ind.rb_stats for metric in e2_filters]

                global msg_queue
                msg_queue.put_many_nowait(pdcp_data)

                # Report the latencies
                if self.report_checkpoint > 0:
                    self.report_maxlatency(tstamp_ms, ricmont_ms)
            except (ConnectionResetError, BrokenPipeError):
                print("[PDCPCallback] No message_queue to use. Ctrl+C?")
                return
            except queue.Full:
                print("[PDCPCallback] Queue is full, drop some messages!")
                return
            except Exception:
                traceback.print_exc()

def subscriber(sm_configs, report_checkpoint):
    # From sm_configs to E2-Nodes' status book
    # {(e2_id): {'is_on': T/F, '<SM>': HandlerObject}}
    e2_statuses = sm_configs_to_e2_statuses(sm_configs)

    # From sm_configs to E2-based per-SM intervals
    # {(e2_id): {'<SM>': 'interval'}}
    e2_intervals = sm_configs_to_e2_intervals(sm_configs)

    # From sm_configs to SM-based programmable filters (for all E2-Nodes)
    # {'<SM>': {(e2_id): ['<metric_i>']}}
    sm_filters = sm_configs_to_sm_filters(sm_configs)

    # Create shared callbacks between different E2-Nodes
    shared_mac_cb = MACCallback('MAC', sm_filters['MAC'], 
                                report_checkpoint)
    shared_pdcp_cb = PDCPCallback('PDCP', sm_filters['PDCP'], 
                                  report_checkpoint)

    # Now, let us handle multiple E2-Node subscriptions
    global is_running
    while True:
        # [Hack] Temporary fix memory leak!
        time.sleep(1)

        # First: Extract the list of "ON" conns_ids
        conns = ric.conn_e2_nodes()
        conns_ids = {
            (conn.id.plmn.mcc,
             conn.id.plmn.mnc,
             conn.id.nb_id.nb_id,
             conn.id.cu_du_id): conn.id
            for conn in conns
        }

        # Second: Drop handlers or re-subscribe to each known E2-Nodes
        # I.e., ignore E2-Nodes that are not in e2_statuses
        for e2_id, status in e2_statuses.items():
            # Case-1: E2-Node is marked as "ON" but is now "OFF"
            if status['is_on'] and (not e2_id in conns_ids):
                status['is_on'] = False
                for status_type in list(status.keys()):
                    if status_type != 'is_on':
                        # TODO: Fix unsubscribe a dead E2-Node at FlexRIC
                        status[status_type] = None
                print(f"[RICMon] Dropped handlers of E2-Node: {e2_id}!")

            # Case-2: E2-Node is marked as "OFF" but is now "ON"
            if (not status['is_on']) and (e2_id in conns_ids):

                status['is_on'] = True
                for status_type in list(status.keys()):
                    if status_type != 'is_on':
                        status[status_type] = getattr(
                            ric, f"report_{status_type.lower()}_sm"
                        )(
                            conns_ids[e2_id],
                            getattr(ric, e2_intervals[e2_id][status_type]),
                            locals()[f"shared_{status_type.lower()}_cb"]
                        )
                print(f"[RICMon] Subscribed to E2-Node: {e2_id}!")

        # Thirdly: Check if Ctrl+C has been pressed => breaking the loop
        try:
            if not is_running.value:
                break
        except EOFError:
            break

    # When exiting, unsubscribe handlers (if existed)
    print('[Ctrl+C] Unsubscribing...')
    for _, status in e2_statuses.items():
        status['is_on'] = False
        for status_type in list(status.keys()):
            if status_type != 'is_on':
                if status[status_type]:
                    getattr(
                        ric, f"rm_report_{status_type.lower()}_sm"
                    )(status[status_type])
                    status[status_type] = None

    # Stopping the SDK...
    print('[Ctrl+C] Stopping the SDK...')
    while ric.try_stop == 0:
        time.sleep(0.01)

    # Stopping the StatsWriter
    print('[Ctrl+C] Stopping the StatsWriter...')
    global stats_writer_proc, manager
    stats_writer_proc.join()
    manager.shutdown()


################
# STATS WRITER #
################
def _msg_dispatcher(data_msg: dict, OBJ_STORE: dict) -> List[dict]:
    global preproc_logics

    results = []
    metric_name = data_msg['__name__']
    if metric_name in preproc_logics:
        for fn in preproc_logics[metric_name]:
            data_msg_copy = data_msg.copy()
            results.extend(fn(data_msg_copy, OBJ_STORE))

    return results

def write(idx, is_running, msg_queue, obj_store,
          min_batch_size, session, promscale_url, labels_key='labels'):
    # For reporting Througput & HOL-sojourn when exiting
    pushes_count, total_samples, total_sojourn = 0, 0, 0

    # Set the DataFrame schema & pick labels for GROUP BY
    msg_dtype = [('__name__', pl.Utf8), ('rnti', pl.Utf8), 
                 ('nb_id', pl.Utf8), ('cu_du_id', pl.Utf8), 
                 ('mcc', pl.Utf8), ('mnc', pl.Utf8),
                 ('timestamp', pl.UInt64), ('value', pl.Float32), 
                 ('ricmonstamp', pl.UInt64)]
    labels_list = ['__name__', 'rnti', 'nb_id', 'cu_du_id', 'mcc', 'mnc']

    msgs_df = pl.DataFrame({}, schema=msg_dtype)
    while True:
        try:
            msgs = msg_queue.get_many(timeout=1)
            preproc_msgs = [p_msg 
                            for msg in msgs
                            for p_msg in _msg_dispatcher(msg, obj_store)]
            msgs.extend(preproc_msgs)

            msgs_df.vstack(pl.DataFrame(msgs, schema=msg_dtype), in_place=True)
        except queue.Empty:
            print(f"[PUSHER-{idx}] Empty queue!")
        except Exception:
            traceback.print_exc()

        try:
            if not is_running.value:
                break
        except EOFError:
            break

        if msgs_df.height >= min_batch_size:
            # GROUP into TimeSeries using the picked labels
            msgs_df.rechunk()
            ts_df = ts_grouping(labels_list, msgs_df)

            # SNAPPY Compression
            payload = "".join([json.dumps(promscale_jsonize(ts, labels_key))
                               for ts in ts_df.iter_rows(named=True)])
            snappy_payload = snappy.compress(payload.encode('utf-8'))

            # POST to a Promscale (exception-prone)
            try:
                response = session.post(promscale_url, 
                                        data=snappy_payload, timeout=1)
                if response.status_code != 200:
                    print(f"\n\t[PUSHER-{idx}] Prosmcale ERROR returned!\n")
            except requests.exceptions.Timeout:
                print(f"\n\t[PUSHER-{idx}] Pushing to Promscale timeout!\n")
            except Exception:
                traceback.print_exc()
            finally:
                response.close()

            # BENCHMARK
            endt_ms = time.time_ns()//1_000_000
            ricmont_ms = msgs_df['ricmonstamp'][0]
            total_sojourn += (endt_ms-ricmont_ms)
            pushes_count += 1
            total_samples += msgs_df.height

            # Reset the DataFrame
            msgs_df = pl.DataFrame({}, schema=msg_dtype)

    print(f"[PUSHER-{idx}] {total_samples} records \
          with AVG(HOL-sojourn) = {total_sojourn//pushes_count} (ms).")

def stats_writer(is_running, msg_queue, metrics_preprocs, promscale_url, is_db,
                 num_threads=2, batch_size=16):
    # GLOBAl (per process) variable for preprocessing logics
    global preproc_logics
    preproc_logics = metrics_preprocs

    # Session (connection pool) shared between threads
    session = requests.Session()
    session.mount('http://', requests.adapters.HTTPAdapter(
        max_retries=0, pool_block=True,
        pool_connections=1, pool_maxsize=num_threads*2)
    )
    session.headers.update({'Content-Type': 'application/json', 
                            'Content-Encoding': 'snappy'})

    # [Trick] A do-nothing sig-handler for the "spawned" StatsWriter process
    def writer_sighandler(signal, frame):
        pass
    signal.signal(signal.SIGINT, writer_sighandler)

    # ThreadPool-based writing to PromScale
    OBJECT_STORE = {}
    labels_key = 'labels' if is_db else 'tests'
    with ThreadPoolExecutor(num_threads) as executor:
        for idx in range(num_threads):
            executor.submit(write, idx, is_running, msg_queue, OBJECT_STORE,
                            batch_size, session, promscale_url, 
                            labels_key=labels_key)

    # When exit, draining the queues
    try:
        print("[Ctrl+C] Clearing the shared queues...")
        while not msg_queue.empty():
            msg_queue.get()
    except Exception:
        traceback.print_exc()
    finally:
        session.close()


########
# MAIN #
########
def subscriber_sighandler(signal, frame):
    global is_running
    is_running.value = False

def execute(running_configs, metrics_to_fns):
    set_start_method('spawn')

    global manager, is_running, msg_queue, stats_writer_proc
    manager = Manager()
    is_running = manager.Value(bool, True)
    msg_queue = Queue(max_size_bytes=running_configs['queue_size_bytes'])
    stats_writer_proc = Process(target=stats_writer,
                                args=(is_running, msg_queue, metrics_to_fns,
                                      running_configs['promscale_url'],
                                      running_configs['database_mode']),
                                kwargs={'num_threads': \
                                        running_configs['writer_threads_count'],
                                        'batch_size': \
                                        running_configs['writer_batch_size']})

    # Move ric.init() out to override FlexApp sig-handler
    ric.init(sys.argv)
    signal.signal(signal.SIGINT, subscriber_sighandler)

    stats_writer_proc.start()
    subscriber(running_configs['sm_configs'], running_configs['cbreport_every_msgs'])
