CONFIGS = {
    'sm_configs': {
        'PDCP': {
            (505, 1, 1, 0): {
                'metrics': ['rxsdu_bytes', 'rxsdu_pkts', 'txsdu_bytes', 'txsdu_pkts'],
                'interval': 'Interval_ms_10'
            },
            (505, 1, 2, 21): {
                # these are aggregated metrics => needs UPF-utilization
                'metrics': ['rxsdu_bytes', 'rxpdu_bytes'],
                'interval': 'Interval_ms_5'
            }
        },
        'MAC': {
            (505, 1, 1, 0): {
                'metrics': [
                    'dl_curr_tbs', 'ul_curr_tbs', 'dl_bler', 'ul_bler',
                    'dl_mcs1', 'ul_mcs1', 'phr', 'wb_cqi'
                ],
                'interval': 'Interval_ms_10'
            },
            (505, 1, 2, 22): {
                # these are aggregated metrics => needs PRB-utilization
                'metrics': ['dl_aggr_prb', 'dl_aggr_retx_prb',
                            'ul_aggr_prb', 'ul_aggr_retx_prb'],
                'interval': 'Interval_ms_5'
            }
        }
    },
    'promscale_url': 'http://172.21.13.12:9201/write',
    'database_mode': True,

    # Use to check if FlexRIC is lagging behind:
    # - set to: (1000/pubsub_interval) * num_of_E2_senders
    'callback_report_checkpoint': 0
}