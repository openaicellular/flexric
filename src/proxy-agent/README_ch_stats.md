# 1. About and a note of warning

We recap here which statistics and configuration values you can extract or set using proxy_agent. This has been tested for Amarisoft RAN version labeled
`2021-12-17`, the only supported for the moment, but parameters are listed for Amarisoft versions from `2021-12-17` to version `2023-06-10`. This is a draft
pending integration as a chapter of a new file README.md and new developments in the code. Note that the data collected or set is not checked for being in the permitted range.  

Various O-RAN standardized Service Models (SM) are available at xApp level to extract these information. they are defined by stardard [1]:
- statistics extraction via Service Model KPM v3.00 using report style number 4. If other report style are asked by the xApp, the programm will provide dummy (simulated) data. 
- parameters setting via Service Model RC (Ran Control) v1.03 or CCC (Cell Configuration and Control). RC is implemented in flexric but its integration work
  with proxy_agent for exposure to the xApp level is ongoing. CCC instead is not yet implemented in flexric.

Some parameters have been omitted for compactness of this document: they will be highlighted in the text below via the ellipsis `...`. 

# 2. List of statistics that can be retrived by Amarisoft RAN 

The parameters will be presented in the form of a tree with each line having the structure: `<param name>: [SM binding] [#comment]`, where:
- param_name: Parameter name in Amarisoft WebSocket API document
- SM binding: can be the name in KPM SM or `TODO` meaning the measure is not yet available to be retrieved by SM KPM but it will, empty space ` ` meaning that
  the parameter is extracted by proxy_agent but not made available to SM indication message by direct specification of the field to be retrieved (it may be
  encapsulated in some other ways, i.e. like gnb_id in E2Setup messages).
- comment: optional comment note 
<br>
For example: 
- `dl_bitrate : DRB.IPThpDl.QCI` 
- `gnb_id_bits : TODO` 

The naming convention to be used by KPM can be found in the specifications 3GPP [2], [3]. The Amarisoft Websocket API document is available in [4]

2.1 Using Amarisoft API function `ue_get`, we can retrieve the following statistics for currently connected UEs:
<pre>
ue_list: 
  enb_ue_id : 
  ran_ue_id : 
  mme_ue_id : TODO # we need this for the kpm indication message
  amf_ue_id : TODO # we need this for the kpm indication message
  linked_enb_ue_id
  linked_ran_ue_id
  rnti: 
  cells:
    cell_id: 
    cqi: TODO
    ri: TODO
    ul_rank: TODO # since Amarisoft version 2023-03-17
    dl_bitrate : DRB.IPThpDl.QCI
    ul_bitrate : DRB.IPThpIl.QCI
    dl_tx : TODO
    ul_tx : TODO
    dl_retx: TODO 
    ul_retx: TODO 
    dl_mcs: TODO 
    ul_mcs: TODO 
    ul_n_layer: TODO # since Amarisoft version 2023-03-17
    turbo_decoder_min: TODO 
    turbo_decoder_avg: TODO 
    turbo_decoder_max: TODO 
    pucch1_snr: TODO 
    pusch_snr: TODO 
    epre: TODO 
    ul_phr: TODO 
    ul_path_loss: TODO 
    p_ue: TODO # since Amarisoft version 2022-03-18
    initial_ta: TODO 
  erabs: # since Amarisoft version 2022-09-16
    erab_id
    qci
    dl_total_bytes: TODO
    ul_total_bytes: TODO
  qos_flows: # since Amarisoft version 2022-09-16
    pdu_session_id
    sst
    dl_total_bytes: TODO
    ul_total_bytes: TODO
</pre>
2.2 Using Amarisoft API `stats`, we can retrieve the following statistics for gnb:
<pre>
cells:
  dl_bitrate: TODO
  ul_bitrate: TODO
  mbms_bitrate: TODO
  dl_tx: TODO
  ul_tx: TODO
  dl_err: TODO # since Amarisoft version 2022-12-16
  ul_err: TODO # since Amarisoft version 2022-12-16
  dl_retx: TODO
  ul_retx: TODO
  dl_sched_users_min: TODO
  dl_sched_users_avg: TODO
  dl_sched_users_max: TODO
  ul_sched_users_min: TODO
  ul_sched_users_avg: TODO
  ul_sched_users_max: TODO
  dl_use_min: TODO
  dl_use_avg: TODO
  dl_use_max: TODO
  ul_use_min: TODO
  ul_use_avg: TODO
  ul_use_max: TODO
  ctrl_use_min: TODO # since Amarisoft version XXXX
  ctrl_use_avg: TODO # since Amarisoft version XXXX
  ctrl_use_max: TODO # since Amarisoft version XXXX
  ue_count_min: TODO
  ue_count_max: TODO
  ue_count_avg: TODO
  erab_count_min: TODO
  erab_count_max: TODO
  erab_count_avg: TODO
  drb_count_max: TODO # since Amarisoft version XXXX
  drb_count_max: TODO # since Amarisoft version XXXX
  drb_count_max: TODO # since Amarisoft version XXXX
  dl_gbr_use_min: TODO
  dl_gbr_use_avg: TODO
  dl_gbr_use_max: TODO
  ul_gbr_use_min: TODO
  ul_gbr_use_avg: TODO
  ul_gbr_use_max TODO
</pre>

2.3 Using Amarisoft API `erab_get`, we can retrieve the following EPS/LTE radio bearer statistics
<pre>
erab_list
  enb_ue_id: TODO
  erab_id: TODO
  qci: TODO
  dl_mbr: TODO
  dl_gbr: TODO
  dl_gbr_re: TODO
  ul_mbr: TODO
  ul_gbr: TODO
  ul_gbr_re: TODO
  dl_total_bytes: TODO
  ul_total_bytes: TODO
</pre>

2.4 Using Amarisoft API `qos_flow_get`, we can retrieve the following 5G radio bearer statistics.
<pre>
qos_flow_list
  ran_ue_id: TODO
  pdu_session_id: TODO
  sst: TODO
  sd: TODO
  qfi_list: 
    qfi: TODO
    5qi: TODO
    dl_mbr: TODO
    dl_gbr: TODO
    ul_mbr : TODO
    ul_gbr: TODO
  dl_total_bytes: TODO
  ul_total_bytes: TODO
</pre>

# 3. List of configuration items that can be retrieved by Amarisoft RAN 

The parameters below could be extracted by RC or CCC Service Model (SMs).

3.1 Using Amarisoft API `config_get`, we can retrieve the following configuration parameters. 

<pre> 
global_enb_id :
  plmn : TODO        # note we need this for the kpm indication message
  enb_id_type : TODO
  enb_id : TODO      # note we need this for the kpm indication message
  enb_name : TODO    # note we need this for the kpm indication header
global_gnb_id :
  plmn : TODO        # note we need this for the kpm indication message
  gnb_id_bits : TODO
  gnb_id : TODO      # note we need this for the kpm indication message
  gnb_name : TODO    # note we need this for the kpm indication header
cells :       # LTE cell definitions
  n_antenna_dl:TODO
  n_antenna_ul:TODO
  n_layer_dl:TODO
  n_layer_ul: TODO
  gain: TODO
  ul_disabled: TODO
  rf_port: TODO
  label: TODO
  dl_qam: TODO
  ul_qam: TODO
  ecgi:  # since Amarisoft version 2022-03-18
    plmn: TODO
    eci: TODO
  n_id_cell: TODO
  n_rb_ul: TODO
  n_rb_dl: TODO
  dl_earfcn: TODO
  ul_earfcn: TODO
  band: TODO
  mode: TODO
  uldl_config: TODO
  sp_config: TODO
  prach_sequence_index:  TODO
  dl_cyclic_prefix: TODO
  ul_cyclic_prefix: TODO
  prach_config_index: TODO
  prach_freq_offset: TODO
  delta_pucch_shift: TODO
  nrb_cqi : TODO
  n_cs_an : TODO
  pucch_allocation: TODO
  pucch_ack_nack_start: TODO
  pucch_reserved_rbs: TODO
  sr_resource_count: TODO
  cqi_resource_count: TODO
  br_sr_resource_count: TODO
  br_cqi_resource_count: TODO
  srs_resources: TODO
  gbr: TODO # since Amarisoft version XXXX
  connected_mobility: TODO # available from Amarisoft version 2022-03-18
  scell_list: TODO
  ncell_list:TODO # since Amarisoft version 2022-03-18
  en_dc_scg_cell_list: TODO
  tac: TODO
  plmn_list: TODO
  plmn_list_5gc: TODO
  
nb_cells : # NB-IoT cell definitions
  n_antenna_dl: TODO
  n_antenna_ul: TODO
  n_layer_dl: TODO
  n_layer_ul: TODO
  gain: TODO
  ul_disabled: TODO
  rf_port : TODO
  label : TODO
  dl_qam : TODO
  ul_qam : TODO
  ecgi: TODO # since Amarisoft version 2022-03-18
  n_id_ncell: TODO
  dl_earfcn: TODO # since Amarisoft version 2023.06.10
  ul_earfcn: TODO # since Amarisoft version 2023.06.10
  band: TODO
  operation_mode: TODO # since Amarisoft version 2023.06.10
  tac:TODO # since Amarisoft version XXXX
  plmn_list:TODO # since Amarisoft version 2023.06.10
  plmn_list_5gc: TODO # since Amarisoft version 2023.06.10
nr_cells: # New Radio cell definitions
  n_antenna_dl: TODO
  n_antenna_ul: TODO
  n_layer_dl: TODO
  n_layer_ul: TODO
  gain: TODO
  ul_disabled: TODO
  rf_port: TODO
  label: TODO
  dl_qam: TODO
  ul_qam: TODO
  ncgi: TODO # since Amarisoft version 2022-03-18
  n_id_nrcell: TODO
  band: TODO
  dl_nr_arfcn: TODO
  ul_nr_arfcn: TODO
  n_rb_dl: TODO
  n_rb_ul: TODO
  ssb_nr_arfcn: TODO
  dl_mu: TODO
  ul_mu: TODO
  ssb_mu: TODO
  mode: TODO
  prach_sequence_index: TODO
  connected_mobility: TODO # since Amarisoft version 2022-03-18
  scell_list: TODO  # NewRadio secondary cells definitions
  ncell_list: TODO # since Amarisoft version 2022-03-18
  nr_dc_scg_cell_list: TODO # since Amarisoft version 2022-03-18
  plmn_list: TODO
rx_channels:
  gains: TODO
  freq: TODO
tx_channels:
  gains: TODO
  freq: TODO
  port: TODO
rf_ports: # channel simulator parameters per RF port if channel simulator is enabled.
  channel_dl:
    noise_level: TODO
    freq_shift: TODO # since Amarisoft version 2022-09-16
    path: TODO
  ul_freq_shift: TODO # since Amarisoft version 2022-12-16
  sample_rate: TODO # since Amarisoft version XXXX
</pre>
# 4. List of configuration parameters that can be set in Amarisoft RAN

The parameters below could be set by RC or CCC Service Model (SMs). Investigation on the mapping between Amarisoft and the SMs is in progress.

4.1 Using Amarisoft API `handover`, we can trigger an handover with the following parameters:
<pre>
ran_ue_id: TODO
pci: TODO
dl_earfcn: TODO
ssb_nr_arfcn: TODO
type: TODO
</pre>

4.2 Using Amarisoft API `config_set`, we can set the following configuration parameters:
<pre>
cells:
  pusch_mcs: TODO
  pusch_fixed_rb_alloc: TODO
  pusch_fixed_rb_start: TODO
  pusch_fixed_l_crb: TODO
  pusch_fixed_rb_forced: TODO
  pusch_multi_cluster: TODO
  pusch_max_mcs: TODO
  pusch_max_its: TODO
  force_full_bsr: TODO
  force_dl_schedule: TODO
  pdsch_mcs: TODO
  pdsch_mcs_from_cqi: TODO
  pdsch_fixed_rb_alloc: TODO
  pdsch_fixed_rb_start: TODO
  pdsch_fixed_l_crb: TODO
  rach_ignore_count: TODO
  mac_crnti_ce_ignore_count: TODO
  dummy_ue_contention_resolution_id: TODO
  rrc_procedure_filter: TODO
  rrc_cnx_reject_waitTime: TODO
  rrc_cnx_reject_extWaitTime: TODO
  rrc_cnx_reject_deprioritisation: TODO
  rrc_cnx_release_extWaitTime: TODO
  rrc_cnx_release_waitTime_5gc: TODO
  rrc_cnx_release_extWaitTime_CPdata: TODO
  rrc_reject_waitTime: TODO
  rrc_release_waitTime: TODO
  rrc_release_deprioritisation: TODO
  gbr_congested: TODO
  ignore_gbr_congestion: TODO
  ue_cap_rat_type: TODO
  requested_eutra_freq_bands: TODO
  requested_eutra_max_ccs_dl: TODO
  requested_eutra_max_ccs_ul: TODO
  request_reduced_format: TODO
  request_eutra_reduced_int_non_cont_comb: TODO
  requested_freq_bands_nr_mrdc: TODO
    ...
  requested_freq_bands_nr: TODO
  mr_dc_request_nr_dc: TODO
  rrc_ul_segmentation_support: TODO
  single_ue_cap_enquiry: TODO
  inactivity_timer: TODO
  br_ue: TODO
    ...
  coverage_levels: TODO # for NB-IoT cells
  preemptive_ul_grant:: TODO
  forced_ri: TODO
  forced_cqi: TODO
  forced_meas_gap_offset: TODO
  pusch_fer: TODO
  pdsch_fer: TODO
  npusch_fer: TODO
  npdsch_fer: TODO
eps_fallback_preferred_method: TODO
eps_fallback_fast_return_preferred_method: TODO
emergency_fallback_preferred_method: TODO
srb3_support: TODO
scg_failure_informtion_behavior: TODO
cell_barred: TODO
rf_ports:: TODO
  channel_dl: TODO
    ...
  ul_freq_shift: TODO

</pre>

# References

- [1] O-RAN Service model standard O-RAN.WG3.E2SM-R003-v03.00.docx
- [2] 3GPP standard for LTE measurements TS 32.425
- [3] 3GPP standard for 5G measurements TS 28.552
- [4] Amarisoft Websocket API docs lteenb-2021-12-17.pdf or lteenb-2023-06-10.pdf


