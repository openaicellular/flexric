## Interacitve xApp

### 0.0 Start from python3 terminal
- import xapp module at the path includes `xapp.py`
```python
$ python3
Python 3.9.13 (main, Aug 25 2022, 23:26:10) 
[GCC 11.2.0] :: Anaconda, Inc. on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import xapp
>>>
```
### 0.1 List of functions
- `xapp.print_funcs_list()`
```python
Available functions:
- print_funcs_list
- print_funcs_usage
- init
- subscribe_sm
- send_slice_ctrl
- print_e2_nodes
- print_slice_stats
- print_slice_stats_json
- print_slice_stats_loop
- print_slice_conf
- print_slice_conf_json
- print_kpm_stats
- print_kpm_stats_ue
- print_kpm_stats_json
```
- `print_funcs_usage()`
```python
>>> xapp.print_funcs_usage(xapp.print_funcs_usage)

    print_funcs_usage(func_name):
    Print given function information.
    
    Parameters:
    func_name: function name (ex: xapp.init)

>>> xapp.print_funcs_usage(xapp.init)

    init():
    Init xApp to setup connection with NearRT-RIC.
```

### 1.0 Start running xApp
- initial xApp to setup E42 connection with NearRT-RIC
```python
>>> xapp.init("path_to_xapp_conf")
```
### 1.1 Print connected E2-Nodes to get index
- show the E2-Nodes information
```
>>> xapp.print_e2_nodes()
+-------+---------+-------+-------+--------------+
|   idx |   nb_id |   mcc |   mnc | ran_type     |
+=======+=========+=======+=======+==============+
|     0 |       2 |   505 |     1 | ngran_gNB_DU |
+-------+---------+-------+-------+--------------+
|     1 |       2 |   505 |     1 | ngran_gNB_CU |
+-------+---------+-------+-------+--------------+
```

### 1.2 Subscribe to Service Model
- subscribe to O-RAN service model
  - For example: the following command will subscribe `xapp.ServiceModel.KPM` on E2-Node index `0` with interval time `xapp.SubTimeInterval.ms1000`, the required action definition is set in `xapp.ex_kpm_actions_gnb_cu`
  ```python
  xapp.subscribe_sm(0, xapp.ServiceModel.KPM, xapp.SubTimeInterval.ms1000, xapp.ex_kpm_actions_gnb_cu)
  ```
- subscribe to customized service model
  - For example: the following command will subscribe `xapp.ServiceModel.SLICE` on E2-Node index `1` with interval time `xapp.SubTimeInterval.ms10`, there is no need of action definition which is set as `0`
    ```python
    xapp.subscribe_sm(1, xapp.ServiceModel.SLICE, xapp.SubTimeInterval.ms10, 0)
    ```

### 1.3 Send slice control message
- write slice configuration, we support the following slice actions:
  - `xapp.SliceType.ADDMOD`
    - configuration format is shown in the exmaple: `xapp.ex_slice_conf_addmod_nvs`, `xapp.ex_slice_conf_addmod_reset`
    - print slice configuration in table:
    ```python
    >>> xapp.print_slice_conf(xapp.SliceType.ADDMOD, xapp.ex_slice_conf_addmod_nvs)
    slice_id  label    slice_sched_algo    slice_algo_param1    slice_algo_param2    slice_algo_param3    ue_sched_algo
    ----------  -------  ------------------  -------------------  -------------------  -------------------  ---------------
           0  s1       NVS-CAPACITY        pct_rsvd:0.5         null                 null                 PF
           2  s2       NVS-RATE            mbps_rsvd:30.0       mbps_ref:100.0       null                 PF
           5  s3       NVS-RATE            mbps_rsvd:60.0       mbps_ref:100.0       null                 PF
    ```
  - `xapp.SliceType.DELETE`
    - configuration format is shown in the exmaple: `xapp.ex_slice_conf_delete`
    - print slice configuration in table:
    ```python
    >>> xapp.print_slice_conf(xapp.SliceType.DELETE, xapp.ex_slice_conf_delete)
    slice_id
    ----------
           5
    ```
  - `xapp.SliceType.ASSOC_UE`
    - configuration format is shown in the exmaple: `xapp.ex_slice_conf_assoc_ue`
    - print slice configuration in table:
    ```python
    xapp.print_slice_conf(xapp.SliceType.ASSOC_UE, xapp.ex_slice_conf_assoc_ue)
    rnti    assoc_slice_id
    ------  ----------------
       0                 2
    ```
  > Note: to print configuration in json foramt can run the function `xapp.print_slice_conf_json()`
- send control message
  
  For example, the following command will send slice control message to E2-Node index `0` with the slice action `xapp.SliceType.ADDMOD` and slice configuration `xapp.ex_slice_conf_addmod_nvs`
  ```python
  xapp.send_slice_ctrl(0, xapp.SliceType.ADDMOD, xapp.ex_slice_conf_addmod_nvs)
  ```

### 1.4 Print stats
Currently, the interacitve xApp only supports to print the stats of KPM SM and SLICE SM.

### 1.5 Stop xApp
- send subscription delete request to the E2-nodes if there are service models subscribed and close the xApp
```
>>> xapp.end()
```