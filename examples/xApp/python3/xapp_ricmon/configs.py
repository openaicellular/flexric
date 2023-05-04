CONFIGS = {
    'e2_nodes_configs': {
        (505, 1, 1, 0): {
            'PDCP':  {
                'metrics': ['rxsdu_bytes', 'rxsdu_pkts', 'txsdu_bytes', 'txsdu_pkts'], 
                'interval': 'Interval_ms_10'
            },
            'MAC':  {
                'metrics': [
                    'dl_curr_tbs', 'ul_curr_tbs', 'dl_bler', 'ul_bler', 
                    'dl_mcs1', 'ul_mcs1', 'phr', 'wb_cqi'
                ], 
                'interval': 'Interval_ms_10'
            }
        },
        (505, 1, 2, 21): {
            'PDCP':  {
                'metrics': ['rxsdu_bytes', 'rxsdu_pkts', 'txsdu_bytes', 'txsdu_pkts'], 
                'interval': 'Interval_ms_10'
            }
        },
        (505, 1, 2, 22): {
            'MAC':  {
                'metrics': [
                    'dl_curr_tbs', 'ul_curr_tbs', 'dl_bler', 'ul_bler', 
                    'dl_mcs1', 'ul_mcs1', 'phr', 'wb_cqi'
                ], 
                'interval': 'Interval_ms_10'
            }
        },
        (505, 1, 3584, 0): {
            'PDCP':  {
                'metrics': ['rxsdu_bytes', 'rxsdu_pkts', 'txsdu_bytes', 'txsdu_pkts'], 
                'interval': 'Interval_ms_10'
            },
            'MAC':  {
                'metrics': [
                    'dl_curr_tbs', 'ul_curr_tbs', 'dl_bler', 'ul_bler', 
                    'dl_mcs1', 'ul_mcs1', 'phr', 'wb_cqi'
                ], 
                'interval': 'Interval_ms_10'
            }
        }
    },
    'promscale_url': 'http://172.21.13.12:9201/write',
    'database_mode': True,
    'subscriber_report_cycles': 0,
    'stats_writer_report': False
}