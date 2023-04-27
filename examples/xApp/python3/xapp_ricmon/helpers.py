import time, json
import polars as pl


# Set targeted metrics & help identifying if "subscribers" are lagging behind OAI
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
                self.maxdiff_ms = max(self.maxdiff_ms, time.time_ns()//1_000_000 \
                                      - tstamp_ms)
                self.test_counter += 1
            else:
                print(f"[{self.mcc}-{self.mnc}-{self.nb_id}-{self.cu_du_id}: {sm}] \
                       Max-Latency: {self.maxdiff_ms} (ms).")
                self.maxdiff_ms = 0
                self.test_counter = 0


# Efficient grouping with Polars
def ts_grouping(labelset, msgs_df):
    msgs_lf = msgs_df.lazy()

    ts_df = msgs_lf.with_columns(
        pl.concat_list([pl.col('timestamp'), pl.col('value')]).alias('datapoint')
    ).groupby(labelset).agg(pl.col('datapoint').alias('samples')).collect()

    return ts_df


# Custom JSON encoder for TimeSeries
# Remark: not really sensible...
class TimeSeriesEncoder(json.JSONEncoder):
    def default(self, ts):
        if isinstance(ts, dict):
            for attr, value in ts.items():
                if attr != 'samples':
                    ts['labels'][attr] = value
                    del ts[attr]

            return ts

        return json.JSONEncoder.default(self, ts)
