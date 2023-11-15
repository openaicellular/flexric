from . import xapp_sdk as ric

import json, inspect
from collections import defaultdict
import polars as pl


##########
# PARSER #
##########
# TODO: ADD DESCRIPTION and TYPE HINTS!!!
def parse_configs(path_to_json, preprocs_module):
    with open(path_to_json, 'r') as f:
        run_configs = json.load(f)

    # 1A. Separate preproc_configs
    preproc_configs = run_configs.get('preproc_configs', {})
    del run_configs['preproc_configs']

    # 1B. Mapping metrics to preprocessors
    # TODO: make it work for external (absolute-path-ed) preprocs_module
    preproc_fns = {
        name: getattr(preprocs_module, name)
        for name, obj in inspect.getmembers(preprocs_module)
        if (inspect.isfunction(obj) and name in dir(preprocs_module))
    }
    metrics_preprocs = {
        metric: [preproc_fns[pp_n] for pp_n in preproc_names]
        for metric, preproc_names in preproc_configs.items()
    }

    # 2A. Tuplize E2-ID strings
    def tuplize_e2id_str(e2id_str):
        e2id_tuple = eval(e2id_str)
        return e2id_tuple[:-1] + (getattr(ric, f"e2ap_{e2id_tuple[-1]}"),)

    # 2B. Transform sm_configs into usable form
    sm_configs = run_configs.get('sm_configs', {})
    for _, config in sm_configs.items():
        for e2_id, _ in list(config.items()):
            new_e2_id = tuplize_e2id_str(e2_id)
            config[new_e2_id] = config.pop(e2_id)

    return run_configs, metrics_preprocs


##############
# SUBSCRIBER #
##############
# TODO: ADD THE TYPE HINTS!!!

# Assist subscriber()-process in tracking E2-Nodes' statuses:
# - from user-specified configs (SM-based)
# - to convenient status keeping (E2-based)
def sm_configs_to_e2_statuses(sm_configs):
    e2_statuses = defaultdict(lambda: {'is_on': False})

    for sm, configs in sm_configs.items():
        for e2_id in list(configs.keys()):
            e2_statuses[e2_id][sm] = None

    return e2_statuses

# Assist subscriber()-process in extracting E2-Node's desired intervals:
# - from user-specified configs (SM-based)
# - to per-SM pubsub intervals (E2-based)
def sm_configs_to_e2_intervals(sm_configs):
    e2_intervals = defaultdict(dict)

    for sm, configs in sm_configs.items():
        for e2_id, specs in configs.items():
            e2_intervals[e2_id][sm] = specs['interval']

    return e2_intervals

# Assist subscriber()-process in extracting E2-Node's target metrics:
# - from user-specified configs (SM-based)
# - to filters shared between all E2-Nodes (still SM-based)
def sm_configs_to_sm_filters(sm_configs):
    sm_filters = defaultdict(dict)

    for sm, configs in sm_configs.items():
        for e2_id, specs in configs.items():
            sm_filters[sm][e2_id] = specs['metrics']

    return sm_filters

# Assist ServiceModel callbacks with latency-benchmarked programmable filters:
# - dictionary of (MCC, MNC, NB_ID, CU_DU_ID): ["<target_metric_i>"];
# - report max_latency after a "checkpoint" number of indications
class CallbackHelper:
    def __init__(self, sm_name, e2_metrics, report_checkpoint):
        # Set programmable filters
        self.sm_name = sm_name
        self.filters = e2_metrics

        # Set checkpoint for max_latency reporting
        self.report_checkpoint = report_checkpoint
        self.report_counter = 0
        self.maxlatency_ms = 0

    def report_maxlatency(self, tstamp_ms, ricmont_ms):
        if self.report_counter < self.report_checkpoint:
            self.maxlatency_ms = max(self.maxlatency_ms, ricmont_ms-tstamp_ms)
            self.report_counter += 1
        else:
            print(f"[{self.sm_name}] Max-Latency: {self.maxlatency_ms} (ms).")
            self.report_counter = 0
            self.maxlatency_ms = 0


################
# STATS WRITER #
################
# TODO: Fix bugs (if any) then add type hints
# Efficient grouping with Polars
def ts_grouping(labels_list, msgs_df):
    labels_str = ";".join(labels_list)

    msgs_lf = msgs_df.lazy()
    ts_df = msgs_lf.with_columns(
        pl.concat_str(labels_list, separator=';').alias(labels_str),
        pl.concat_list([pl.col('timestamp'), pl.col('value')]).alias('datapoint')
    ).groupby(labels_str).agg(pl.col('datapoint').alias('samples')).collect()

    return ts_df

# TODO: Fix bugs (if any) then add type hints
# Promscale JSON encoder for TimeSeries
def promscale_jsonize(ts_row, labels_key):
    if (len(ts_row.keys()) == 2) and ('samples' in ts_row):
        labels_str = [k for k in ts_row.keys() if k != 'samples'][0]

        ts_row[labels_key] = {
            label: value for label, value
            in zip(labels_str.split(';'), ts_row[labels_str].split(';'))
        }

        del ts_row[labels_str]
        return ts_row
