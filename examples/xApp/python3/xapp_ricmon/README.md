# RICMon xApp v0.1 - Setup and Tests

**RICMon v0.1** has been tested with `Python 3.11.3` on `Ubuntu 18.04` (`5.4.0-150-generic`), but it should work with any combination of Python 3.11+ and Ubuntu 18.04+.

For now, compiling FlexApp using Python3.8 share object and headers is still OK. Yet, we recommend users to reserve Python3.11 share object and headers for future releases.

### A. Setup in 3 simple fast steps
1. Checkout the `ricmon-xapp` branch on the BubbleRAN's FlexRIC repo.
2. Install FlexRIC using the default option.
3. Then, navigate to `$FLEXRIC_PATH/examples/xApp/python3/xapp_ricmon/` to install the necessary Python packages with the following command:
    * `pip install -r requirements.txt`

### B. 4 integration tests for RICMon
Before carrying out these tests, please navigate to `$FLEXRIC_PATH/build/examples/xApp/python3/xapp_ricmon/` and have a look at the `configs.py` file. Then, you can start RICMon as follows:
1. Run `kubectl port-forward -n tobs svc/tobs-promscale --address 172.21.13.12 9201:9201` on `ayo` to port-forward Promscale externally;
2. Start the near-RT RIC;
3. Start OAI (with RF-SIMs or Quectels) or multiple E2-SIMs;
4. Finally, execute `python run.py`.

**TEST 1**. _RICMon will keep track of what E2 Nodes are connected and subscribe/re-subscribe to them_.

To see this, simply have your E2 Nodes going up and down and see how RICMon maintain relevant subscriptions.

**TEST 2**. _RICMon can be programmed to write only a subset of each service-model's metrics at custom intervals, and/or add secondary metrics on top of them_.

To customize each service-model's metrics and intervals, you can modify the `CONFIGS` dictionary inside `configs.py`. To preprocess and add secondary metrics, you can program the `msg_dispatcher` function inside `configs.py`. 

To illustrate RICMon's programming model, i.e., **handling one narrow message per call with object-storage across calls**, we provide a _compute PRB utilization_ example.

Before validating programmability, please:
1. Run `kubectl port-forward -n tobs pods/tobs-timescaledb-0 5432:5432` on `ayo` to port-forward TimescaleDB externally;
2. Open your tunneled (port 5432) database administration tool, e.g., **pgAdmin**, with the following password,
3. `kubectl get secret --namespace tobs tobs-credentials -o jsonpath="{.data.PATRONI_SUPERUSER_PASSWORD}" | base64 --decode`

Happy observing RICMon's progammability with your own SQL queries!

**TEST 3**. _In "streaming" mode, RICMon can process up to 10K records/second with the average HOL-sojourn of around 15ms_.

Firstly, to run at this scale, you should:
1. Run multiple E2-SIMs, e.g., gNB, gNB-CU, gNB-DU;
2. With data filled in for multiple UEs, here we have 10, per indication message.

Then, simply set `database_mode: False` in `configs.py`. Happy observing RICMon handling lots of records in near-real-time!

**TEST 4**. _In "database" mode, RICMon can process up to 10K records/second with the average HOL-sojourn of **under** 300ms_.

You can follow the steps of **TEST 3**, but set `database_mode: True` in `configs.py`. Note that your actual results may vary depend on how you tune `writer_threads_count` and `writer_batch_size`.

**Remark 1**. We use the terms "records" instead of "metrics" because not every data model have _1 metric per record_ as in TimescaleDB.

**Remark 2**. Please first make sure that your `configs.py` does not make FlexApp lags behind. You may measure such delay by turning on `callback_report_checkpoint` in `configs.py`, which will report the delay of each service-model's callback periodically.

**Remark 3**. You can use any profiling tool to check that RICMon consumes *~2.5 modern CPU cores* and *~175MiB of RAM*. Note that this also depends on the number of metrics you expect RICMon to process.

### C. Shutting down RICMon
The usual `Ctrl+C` is used to close RICMon. At the end, you will see that RICMon exits gracefully through:
1. Overriding the default sig-handler of FlexApp, to
2. Have all subscriptions unsubscribed, and
3. All processes/threads stop safely.

Finally, please remember to stop port-forwarding Promscale, and TimescaleDB if applicable, on `ayo`.
