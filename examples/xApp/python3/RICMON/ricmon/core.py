from . import xapp_sdk as ric

import time
from .helpers import sm_configs_to_e2_statuses, \
                    sm_configs_to_e2_intervals, \
                    sm_configs_to_sm_filters, \
                    sm_configs_to_access_names, \
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
    def __init__(self, sm_name, e2_metrics, report_checkpoint=0, access_names={}):
        # Call C++ base class constructor
        ric.mac_cb.__init__(self)

        # Call SubscriptionHelper's constructor
        CallbackHelper.__init__(self, sm_name, e2_metrics, report_checkpoint, access_names)

    # Override C++ method: virtual void handle(swig_mac_ind_msg_t a) = 0;
    def handle(self, ind):
        if len(ind.ue_stats) > 0:
            ricmont_ms = time.time_ns()//1_000_000
            tstamp_ms = ind.tstamp//1000

            try:
                # E2-based filtering of messages, plus enqueuing
                e2_id = (ind.id.plmn.mcc, ind.id.plmn.mnc,
                         ind.id.nb_id.nb_id, ind.id.cu_du_id, ind.id.type)
                e2_filters = self.filters[e2_id]
                access_name = 'unknown'
                if e2_id[2] in self.access_names:
                    if self.access_names[e2_id[2]]:
                        access_name = self.access_names[e2_id[2]]

                mac_data = [{
                    '__name__': metric,
                    'ue_id': f"{ue_stat.rnti}",
                    'ue_id_type': 'rnti',
                    'mcc': f"{e2_id[0]}",
                    'mnc': f"{e2_id[1]}",
                    'nb_id': f"{e2_id[2]}",
                    'cu_du_id': e2_id[3],
                    'ran_type': f"{e2_id[4]}",
                    'access_name': access_name,
                    'timestamp': tstamp_ms,
                    'value': getattr(ue_stat, metric),
                    'frame': f"{ue_stat.frame}",
                    'slot': f"{ue_stat.slot}",
                    'ricmonstamp': ricmont_ms
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
    def __init__(self, sm_name, e2_metrics, report_checkpoint=0, access_names={}):
        # Call C++ base class constructor
        ric.pdcp_cb.__init__(self)

        # Call SubscriptionHelper's constructor
        CallbackHelper.__init__(self, sm_name, e2_metrics, report_checkpoint, access_names)

    # Override C++ method: virtual void handle(swig_pdcp_ind_msg_t a) = 0;
    def handle(self, ind):
        if len(ind.rb_stats) > 0:
            ricmont_ms = time.time_ns()//1_000_000
            tstamp_ms = ind.tstamp//1000

            try:
                # E2-based filtering of messages, plus enqueuing
                e2_id = (ind.id.plmn.mcc, ind.id.plmn.mnc,
                         ind.id.nb_id.nb_id, ind.id.cu_du_id, ind.id.type)
                e2_filters = self.filters[e2_id]
                access_name = 'unknown'
                if e2_id[2] in self.access_names:
                    if self.access_names[e2_id[2]]:
                        access_name = self.access_names[e2_id[2]]

                pdcp_data = [{
                    '__name__': f"pdcp_{metric}",
                    'ue_id': f"{rb_stat.rnti}",
                    'ue_id_type': 'rnti',
                    'mcc': f"{e2_id[0]}",
                    'mnc': f"{e2_id[1]}",
                    'nb_id': f"{e2_id[2]}",
                    'cu_du_id': e2_id[3],
                    'ran_type': f"{e2_id[4]}",
                    'access_name': access_name,
                    'timestamp': tstamp_ms,
                    'value': getattr(rb_stat, metric),
                    'ricmonstamp': ricmont_ms
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

class KPMCallback(ric.kpm_cb):
    def __init__(self, access_names={}):
        # Call C++ base class constructor
        ric.kpm_cb.__init__(self)

        self.access_names = access_names

    def _get_ue_id(self, ue_meas_report_lst):
        if ue_meas_report_lst.type == ric.GNB_UE_ID_E2SM:
            return (f"{ue_meas_report_lst.gnb.amf_ue_ngap_id}", 'ue_ngap_id')
        elif ue_meas_report_lst.type == ric.GNB_DU_UE_ID_E2SM:
            return (f"{ue_meas_report_lst.gnb_du.gnb_cu_ue_f1ap}", 'ue_f1ap')
        elif ue_meas_report_lst.type == ric.GNB_CU_UP_UE_ID_E2SM:
            return (f"{ue_meas_report_lst.gnb_cu_up.gnb_cu_cp_ue_e1ap}", 'ue_e1ap')
        else:
            return ('-1', 'unsupported')

    def _get_ue_metrics(self, meas_info_lst):
        return [
            meas_info.meas_type.name
            if meas_info.meas_type.type == ric.NAME_MEAS_TYPE else 'unknown'
            for meas_info in meas_info_lst
        ]

    def _get_metric_value(self, meas_record):
        if meas_record.value == ric.INTEGER_MEAS_VALUE:
            return meas_record.int_val
        elif meas_record.value == ric.REAL_MEAS_VALUE:
            return meas_record.real_val
        else:
            return 0

    # Override C++ method: virtual void handle(swig_kpm_ind_data_t a) = 0;
    def handle(self, ind):
        # ONLY support FORMAT-3 message with HEADER and non-empty MEAS-REPORT
        if ind.hdr and ind.msg.type == ric.FORMAT_3_INDICATION_MESSAGE:
            if len(ind.msg.frm_3.meas_report_per_ue) > 0:
                ricmont_ms = time.time_ns()//1_000_000
                tstamp_ms = ind.hdr.kpm_ric_ind_hdr_format_1.collectStartTime//1000

                try:
                    e2_id = (ind.id.plmn.mcc, ind.id.plmn.mnc,
                            ind.id.nb_id.nb_id, ind.id.cu_du_id, ind.id.type)
                    access_name = 'unknown'
                    if e2_id[2] in self.access_names:
                        if self.access_names[e2_id[2]]:
                            access_name = self.access_names[e2_id[2]]

                    kpm_data = [
                        {
                            '__name__': ue_metrics[idx],
                            'ue_id': ue_id,
                            'ue_id_type': ue_id_type,
                            'mcc': f"{e2_id[0]}",
                            'mnc': f"{e2_id[1]}",
                            'nb_id': f"{e2_id[2]}",
                            'cu_du_id': e2_id[3],
                            'ran_type': f"{e2_id[4]}",
                            'access_name': access_name,
                            'timestamp': tstamp_ms,
                            'value': self._get_metric_value(meas_record),
                            'ricmonstamp': ricmont_ms
                        } for ue_meas in ind.msg.frm_3.meas_report_per_ue
                        for ue_id, ue_id_type, ue_table in [self._get_ue_id(ue_meas.ue_meas_report_lst)
                                                            + (ue_meas.ind_msg_format_1,)]
                        for ue_metrics in [self._get_ue_metrics(ue_table.meas_info_lst)]
                        for meas_data in ue_table.meas_data_lst
                        if (meas_data.incomplete_flag != ric.TRUE_ENUM_VALUE) and
                           (meas_data.meas_record_len == ue_table.meas_info_lst_len)
                        for idx, meas_record in enumerate(meas_data.meas_record_lst)
                    ]

                    global msg_queue
                    msg_queue.put_many_nowait(kpm_data)
                except (ConnectionResetError, BrokenPipeError):
                    print("[KPMCallback] No message_queue to use. Ctrl+C?")
                    return
                except queue.Full:
                    print("[KPMCallback] Queue is full, drop some messages!")
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

    # Get access names from the sm_configs
    access_names = sm_configs_to_access_names(sm_configs)

    # Create shared callbacks between different E2-Nodes
    shared_mac_cb = MACCallback('MAC', sm_filters['MAC'], report_checkpoint, access_names)
    shared_pdcp_cb = PDCPCallback('PDCP', sm_filters['PDCP'], report_checkpoint, access_names)
    shared_kpm_cb = KPMCallback(access_names)

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
             conn.id.cu_du_id,
             conn.id.type): conn.id
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
                    if status_type == 'KPM':
                        status['KPM'] = ric.report_kpm_sm(
                            conns_ids[e2_id], getattr(ric, e2_intervals[e2_id]['KPM']),
                            sm_filters['KPM'][e2_id], shared_kpm_cb
                        )
                    elif status_type != 'is_on':
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
            cp_data_msg = data_msg.copy()
            results.extend(fn(cp_data_msg, OBJ_STORE))
            del cp_data_msg

    return results

def write(idx, is_running, msg_queue, obj_store,
          min_batch_size, session, promscale_url, labels_key='labels'):
    # For reporting Througput & HOL-delay when exiting
    pushes_count, total_samples, total_delay = 0, 0, 0

    # Set the DataFrame schema & pick labels for GROUP BY
    msg_dtype = [('__name__', pl.Utf8), ('ue_id', pl.Utf8), ('ue_id_type', pl.Utf8),
                 ('mcc', pl.Utf8), ('mnc', pl.Utf8),
                 ('nb_id', pl.Utf8), ('cu_du_id', pl.Utf8), ('ran_type', pl.Utf8), ('access_name', pl.Utf8),
                 ('timestamp', pl.UInt64), ('value', pl.Float32), ('ricmonstamp', pl.UInt64)]
    labels_list = ['__name__', 'ue_id', 'ue_id_type',
                   'mcc', 'mnc', 'nb_id', 'cu_du_id', 'ran_type', 'access_name']

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
            # TODO: Should we get_many_nowait()
            # sleep if exception is raised?
            pass
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
            begint_ms = msgs_df['timestamp'][0]
            total_delay += (endt_ms-begint_ms)
            pushes_count += 1
            total_samples += msgs_df.height

            # Reset the DataFrame
            msgs_df = pl.DataFrame({}, schema=msg_dtype)

    print(f"[PUSHER-{idx}] {total_samples} records \
          with AVG(HOL-latency) = {total_delay//pushes_count} (ms).")

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
    signal.signal(signal.SIGTERM, writer_sighandler)

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
            # TODO: Drain the queue with get_many_nowait()
            # pass if exception is raised?
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

def execute(flexapp_conf_path, running_configs, metrics_to_fns):
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
    ric.init(['', '-c', flexapp_conf_path])
    signal.signal(signal.SIGINT, subscriber_sighandler)
    signal.signal(signal.SIGTERM, subscriber_sighandler)

    stats_writer_proc.start()
    subscriber(running_configs['sm_configs'], running_configs['cbreport_every_msgs'])
