from bisect import insort


def _compute_slots_diff(from_frame, from_slot, to_frame, to_slot, frame_structure):
    # Get the correct frames strictly between 2 records
    frames_dist = (to_frame-from_frame) - 1
    if frames_dist <= -2:
        frames_dist += 1024

    if frames_dist == -1:
        # I.e., different slots of the same frame
        return sum(frame_structure[from_slot : to_slot+1])
    else:
        # I.e., slots of different frames
        return sum(frame_structure[from_slot :]) \
                + frames_dist*sum(frame_structure) \
                + sum(frame_structure[0 : to_slot+1])


# [EXPOSED METHODS] TODO: ADD DESCRIPTION && TYPE HINTS
# <DOCS> Accept 'dl_aggr_prb' and 'dl_aggr_retx_prb' raw metrics
def dl_prb_utilization(prb_msg, OBJ_STORE, metric_name, DL_FRAME, SLOT_PRBS):
    ue_key = (prb_msg['mcc'], prb_msg['mnc'],
              prb_msg['nb_id'], prb_msg['cu_du_id'],
              prb_msg['ue_id'], prb_msg['ue_id_type'])
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
               for metric_name in ['dl_aggr_prb', 'dl_aggr_retx_prb']):
            # Count the slots between 2 recorded indications
            dl_slot_diffs = _compute_slots_diff(
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][0]['frame']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][0]['slot']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][1]['frame']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][1]['slot']),
                DL_FRAME
            )

            # Get the used PRBs between 2 recorded indications
            dl_used_prb = (OBJ_STORE[ue_key]['dl_aggr_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['dl_aggr_prb'][0]['value']) + \
                          (OBJ_STORE[ue_key]['dl_aggr_retx_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['dl_aggr_retx_prb'][0]['value'])

            # Pop the former indications, build & return NEW data messages
            dl_res_msg = OBJ_STORE[ue_key]['dl_aggr_prb'].pop(0)
            OBJ_STORE[ue_key]['dl_aggr_retx_prb'].pop(0)
            dl_res_msg['__name__'] = metric_name
            dl_res_msg['value'] = 0 if dl_slot_diffs == 0 \
                else round(dl_used_prb / (dl_slot_diffs*SLOT_PRBS), 2)

            return [dl_res_msg]
        else:
            return []
    except (KeyError, IndexError):
        return []

# [EXPOSED METHODS] TODO: ADD DESCRIPTION && TYPE HINTS
# <DOCS> Accept 'ul_aggr_prb' and 'ul_aggr_retx_prb' raw metrics
def ul_prb_utilization(prb_msg, OBJ_STORE, metric_name, UL_FRAME, SLOT_PRBS):
    ue_key = (prb_msg['mcc'], prb_msg['mnc'],
              prb_msg['nb_id'], prb_msg['cu_du_id'],
              prb_msg['ue_id'], prb_msg['ue_id_type'])
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
               for metric_name in ['ul_aggr_prb', 'ul_aggr_retx_prb']):
            # Count the slots between 2 recorded indications
            ul_slot_diffs = _compute_slots_diff(
                int(OBJ_STORE[ue_key]['ul_aggr_prb'][0]['frame']),
                int(OBJ_STORE[ue_key]['ul_aggr_prb'][0]['slot']),
                int(OBJ_STORE[ue_key]['ul_aggr_prb'][1]['frame']),
                int(OBJ_STORE[ue_key]['ul_aggr_prb'][1]['slot']),
                UL_FRAME
            )

            # Get the used PRBs between 2 recorded indications
            ul_used_prb = (OBJ_STORE[ue_key]['ul_aggr_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['ul_aggr_prb'][0]['value']) + \
                          (OBJ_STORE[ue_key]['ul_aggr_retx_prb'][1]['value'] - \
                           OBJ_STORE[ue_key]['ul_aggr_retx_prb'][0]['value'])

            # Pop the former indications, build & return NEW data messages
            ul_res_msg = OBJ_STORE[ue_key]['ul_aggr_prb'].pop(0)
            OBJ_STORE[ue_key]['ul_aggr_retx_prb'].pop(0)
            ul_res_msg['__name__'] = metric_name
            ul_res_msg['value'] = 0 if ul_slot_diffs == 0 \
                else round(ul_used_prb / (ul_slot_diffs*SLOT_PRBS), 2)

            return [ul_res_msg]
        else:
            return []
    except (KeyError, IndexError):
        return []
