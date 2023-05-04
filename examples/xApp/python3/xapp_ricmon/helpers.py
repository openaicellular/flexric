import polars as pl


# Set targeted metrics & help identifying if "subscribers" are lagging behind OAI
class SubscriptionHelper:
    def __init__(self, e2_id, target_metrics, test_checkpoint=0):
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

    def process_indication(self, ind_stats, msg_queue, tstamp_ms=0, ricmont_ms=0):
        for stat in ind_stats:
            for metric in self.target_metrics:
                msg_queue.put_nowait({
                    '__name__': metric, 'rnti': f"{stat.rnti}",
                    'nb_id': f"{self.nb_id}", 'cu_du_id': f"{self.cu_du_id}",
                    'mcc': f"{self.mcc}", 'mnc': f"{self.mnc}",
                    'timestamp': tstamp_ms, 'value': getattr(stat, metric),
                    'ricmonstamp': ricmont_ms
                })

    def report_latency(self, sm_name, tstamp_ms, ricmont_ms):
        if self.test_counter < self.test_checkpoint:
            self.maxdiff_ms = max(self.maxdiff_ms, ricmont_ms-tstamp_ms)
            self.test_counter += 1
        else:
            print(f"[{self.mcc}-{self.mnc}-{self.nb_id}-{self.cu_du_id}: {sm_name}] \
                    Max-Latency: {self.maxdiff_ms} (ms).")
            self.maxdiff_ms = 0
            self.test_counter = 0


# Efficient grouping with Polars
def ts_grouping(labels_list, msgs_df):
    labels_str = ";".join(labels_list)

    msgs_lf = msgs_df.lazy()
    ts_df = msgs_lf.with_columns(
        pl.concat_str(labels_list, separator=';').alias(labels_str),
        pl.concat_list([pl.col('timestamp'), pl.col('value')]).alias('datapoint')
    ).groupby(labels_str).agg(pl.col('datapoint').alias('samples')).collect()

    return ts_df


# Promscale JSON encoder for TimeSeries
def promscale_jsonize(ts_row):
    if (len(ts_row.keys()) == 2) and ('samples' in ts_row):
        labels_str = [k for k in ts_row.keys() if k != 'samples'][0]

        ts_row['labels'] = {
            label: value for label, value
            in zip(labels_str.split(';'), ts_row[labels_str].split(';'))
        }

        del ts_row[labels_str]
        return ts_row
