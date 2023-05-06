# RICMon xApp v0.1 - Setup and Tests

**RICMon v0.1** has been tested with `Python 3.8.16` on `Ubuntu 18.04` (`5.4.0-146-generic`), but it should work with any combination of Python 3.8+ and Ubuntu 18.04+.

### A. Setup in 3 simple fast steps
1. Checkout the `ricmon-xapp` branch on the BubbleRAN's FlexRIC repo.
2. Install FlexRIC with the following options:
    * `EMU_AGENT_INSTALL=TRUE`
    * `NEAR_RIC_INSTALL=TRUE`
    * `XAPP_DB=NONE_XAPP`
    * `XAPP_PYTHON_INSTALL=TRUE`
    * `XAPP_TARGET_LANGUAGE=PYTHON_LANG`
3. Then, navigate to `$FLEXRIC_PATH/examples/xApp/python3/xapp_ricmon/` to install the necessary Python packages with the following command:
    * `pip install -r requirements.txt`

### B. 3 simple fast tests for RICMon
Before carrying out these tests, please navigate to `$FLEXRIC_PATH/build/examples/xApp/python3/xapp_ricmon/` and have a look at the `configs.py` file. Then, you can start RICMon as follows:
1. Run `kubectl port-forward -n tobs svc/tobs-promscale --address 172.21.13.12 9201:9201` on `ayo` to port-forward Promscale externally;
2. Start the near-RT RIC;
3. Start OAI (with RF-SIMs or Quectels) or multiple E2-SIMs;
4. Finally, execute `python3.8 run.py`.

**TEST 1**. _RICMon will keep track of what E2 Nodes are connected and subscribe/re-subscribe to them_.

To see this, simply set `subscriber_report_cycles: 100` in `configs.py` and observe how RICMon response to different E2 Nodes going up and down.

**TEST 2**. _In "streaming" mode, RICMon can process up to 9K records with the average HOL-sojourn of ~20ms_.

Firstly, to run at this scale, you may:
1. Run multiple E2-SIMs, e.g., gNB, gNB-CU, gNB-DU;
2. Fill in data for multiple UEs, say 10, per indication message;
3. Subscribe at 1ms interval.

Then, simply set `database_mode: False` and `'stats_writer_report': True` in `configs.py`. Happy observing RICMon handling lots of records in near-real-time.

**TEST 3**. _In "database" mode, RICMon can process up to 9K records with the average HOL-sojourn of ~300ms_.

You can follow the steps of **TEST 2**, but set `database_mode: True` in `configs.py`.

**Remark 1**. You can use any profiling tool to check that RICMon consumes *~2.5 modern CPU cores* and *< 150MiB of RAM*.

**Remark 2**. We use the terms "records" instead of "metrics" because not every data model have _1 metric per record_ as in TimescaleDB.

### C. Shutting down RICMon
The usual `Ctrl+C` is used to close RICMon, however, at the end, you may observe either errors:
1. `Segmentation Fault`, which only happens after `Ctrl+C`, is due to a combination of incompabilities: SWIG-based `handle()` callbacks, Python's Proxy Objects, and RICMon's spawn-based (not fork-based) design.
2. `pure virtual method called...`, which is related to SWIG.

Rest assured that RICMon will always exit gracefully, leaving no zombies on the RIC plus the xApp itself; in fact, all handlers will be unsubscribed and all processes/threads will run until their ends. We just need to manually kill the leftover `spawn_main()` process. In the mid-term, #1 can be fixed by upgrading the version of Python.

Finally, remember to stop port-forwarding Promscale on `ayo`.
