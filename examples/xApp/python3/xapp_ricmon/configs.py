from bisect import insort
from typing import List

CONFIGS = {
    'sm_configs': {
        'PDCP': {
            (505, 1, 1, "0"): {
                'metrics': ['rxsdu_bytes', 'rxsdu_pkts', 'txsdu_bytes', 'txsdu_pkts'],
                'interval': 'Interval_ms_10'
            },
            (505, 1, 2, "21"): {
                'metrics': ['rxsdu_bytes', 'rxpdu_bytes'],
                'interval': 'Interval_ms_5'
            }
        },
        'MAC': {
            (505, 1, 1, "0"): {
                'metrics': [
                    'dl_curr_tbs', 'ul_curr_tbs', 'dl_bler', 'ul_bler',
                    'dl_mcs1', 'ul_mcs1', 'phr', 'wb_cqi'
                ],
                'interval': 'Interval_ms_10'
            },
            (505, 1, 2, "22"): {
                # these are aggregated metrics => needs PRB-utilization
                'metrics': ['dl_aggr_prb', 'dl_aggr_retx_prb',
                            'ul_aggr_prb', 'ul_aggr_retx_prb'],
                'interval': 'Interval_ms_10'
            },
            (505, 1, 3584, "0"): {
                # these are aggregated metrics => needs PRB-utilization
                'metrics': ['dl_aggr_prb', 'dl_aggr_retx_prb',
                            'ul_aggr_prb', 'ul_aggr_retx_prb'],
                'interval': 'Interval_ms_5'
            }
        }
    },
    'writer_threads_count': 2,
    'writer_batch_size': 16,
    'promscale_url': 'http://172.21.13.12:9201/write',
    'database_mode': True,

    # Use to check if FlexRIC is lagging behind:
    # - set to: (1000/pubsub_interval) * num_of_E2_senders
    'callback_report_checkpoint': 0
}


# [RADIO STRUCTURE]
# TODO: Check with ChiehChun
DL_FRAME = [1, 1, 1, 1, 1, 1, 1, 0.55, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 0.55, 0, 0]
UL_FRAME = [0, 0, 0, 0, 0, 0, 0, 0.45, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0.45, 1, 1]
PRBS_PER_SLOT = 106

def compute_slots_diff(from_frame, from_slot, to_frame, to_slot):
    # Get the correct frames strictly between 2 records
    frames_dist = (to_frame-from_frame) - 1
    if frames_dist <= -2:
        frames_dist += 1024

    if frames_dist == -1:
        # I.e., different slots of the same frame
        # => TODO: should be (from_slot+1 --> to_slot)
        # => return (dl_slots_diff, ul_slots_diff)
        return (
            sum(DL_FRAME[from_slot : to_slot+1]),
            sum(UL_FRAME[from_slot : to_slot+1])
        )
    else:
        # I.e., slots of different frames
        # => TODO: should be (from_slot+1 --> 19) + (0 --> to_slot)
        # => return (dl_slots_diff, ul_slots_diff)
        return (
            sum(DL_FRAME[from_slot : -1]) \
                + frames_dist*sum(DL_FRAME) \
                + sum(DL_FRAME[0 : to_slot+1]),
            sum(UL_FRAME[from_slot : -1]) \
                + frames_dist*sum(UL_FRAME) \
                + sum(UL_FRAME[0 : to_slot+1])
        )

def compute_prb_utilization(prb_msg, OBJ_STORE):
    ue_key = (prb_msg['mcc'], prb_msg['mnc'],
              prb_msg['nb_id'], prb_msg['cu_du_id'], 
              prb_msg['rnti'])
    prb_metric = prb_msg['__name__']

    # 1. Add to this UE's message-dict
    if not (ue_key in OBJ_STORE):
        OBJ_STORE[ue_key] = { prb_metric: [prb_msg] }
        return []
    elif not (prb_metric in OBJ_STORE[ue_key]):
        OBJ_STORE[ue_key][prb_metric] = [prb_msg]
        return []
    else:
        # Since prb_messages are processed by multiple threads,
        # this insort must be done to ensure sequential computations.
        insort(OBJ_STORE[ue_key][prb_metric],
               prb_msg, key=lambda x: x['value'])

    # 2. Check if this UE has sufficient data to compute prb-utilization
    try:
        if all(len(OBJ_STORE[ue_key][metric_name]) >= 2
            for metric_name in ['dl_aggr_prb', 'dl_aggr_retx_prb', 
                                'ul_aggr_prb', 'ul_aggr_retx_prb']):
            # Count the slots between 2 recorded indications
            dl_slot_diffs, ul_slot_diffs = compute_slots_diff(
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][0]['frame']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][0]['slot']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][1]['frame']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][1]['slot'])
            )

            # Get the used PRBs between 2 recorded indications
            dl_used_prb = (OBJ_STORE[ue_key]['dl_aggr_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['dl_aggr_prb'][0]['value']) + \
                          (OBJ_STORE[ue_key]['dl_aggr_retx_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['dl_aggr_retx_prb'][0]['value'])
            ul_used_prb = (OBJ_STORE[ue_key]['ul_aggr_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['ul_aggr_prb'][0]['value']) + \
                          (OBJ_STORE[ue_key]['ul_aggr_retx_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['ul_aggr_retx_prb'][0]['value'])

            # Pop the former indications, build & return NEW data messages
            dl_res_msg = OBJ_STORE[ue_key]['dl_aggr_prb'].pop(0)
            OBJ_STORE[ue_key]['dl_aggr_retx_prb'].pop(0)
            dl_res_msg['__name__'] = 'dl_prb_utilization'
            dl_res_msg['value'] = 0 if dl_slot_diffs == 0 \
                else round(dl_used_prb / (dl_slot_diffs*PRBS_PER_SLOT), 2)

            ul_res_msg = OBJ_STORE[ue_key]['ul_aggr_prb'].pop(0)
            OBJ_STORE[ue_key]['ul_aggr_retx_prb'].pop(0)
            ul_res_msg['__name__'] = 'ul_prb_utilization'
            ul_res_msg['value'] = 0 if ul_slot_diffs == 0 \
                else round(ul_used_prb / (ul_slot_diffs*PRBS_PER_SLOT), 2)

            return [dl_res_msg, ul_res_msg]
        else:
            return []
    except KeyError:
        return []

def msg_dispatcher(data_msg: dict, OBJ_STORE: dict) -> List[dict]:
    if data_msg['__name__'] in ['dl_aggr_prb', 'dl_aggr_retx_prb', 
                                'ul_aggr_prb', 'ul_aggr_retx_prb']:
        return compute_prb_utilization(data_msg, OBJ_STORE)
    else:
        return []
