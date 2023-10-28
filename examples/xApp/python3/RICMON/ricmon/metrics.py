from bisect import insort


####################
# INTERNAL FUNCTIONS
####################
def _compute_slots_diff(from_frame, from_slot, to_frame, to_slot,
                       dl_frame_structure, ul_frame_structure):
    # Get the correct frames strictly between 2 records
    frames_dist = (to_frame-from_frame) - 1
    if frames_dist <= -2:
        frames_dist += 1024

    if frames_dist == -1:
        # I.e., different slots of the same frame
        # => TODO: should be (from_slot+1 --> to_slot)?
        # => return (dl_slots_diff, ul_slots_diff)
        return (
            sum(dl_frame_structure[from_slot : to_slot+1]),
            sum(ul_frame_structure[from_slot : to_slot+1])
        )
    else:
        # I.e., slots of different frames
        # => TODO: should be (from_slot+1 --> 19) + (0 --> to_slot)?
        # => return (dl_slots_diff, ul_slots_diff)
        return (
            sum(dl_frame_structure[from_slot : -1]) \
                + frames_dist*sum(dl_frame_structure) \
                + sum(dl_frame_structure[0 : to_slot+1]),
            sum(ul_frame_structure[from_slot : -1]) \
                + frames_dist*sum(ul_frame_structure) \
                + sum(ul_frame_structure[0 : to_slot+1])
        )

##################
# REUSABLE METHODS
##################
# [DOCS] Each method inside ricmon.metrics should be listed with:
# + A pre-defined list of required metrics, with __name__ set by FlexRIC.
# + Fun-fact: We assume Promscale-structured narrow messages.

# METHOD: ricmon_prb_utilization(dict, dict, List[float], List[float], int) -> List[dict]
# REQUIRE: ['dl_aggr_prb', 'dl_aggr_retx_prb', 'ul_aggr_prb', 'ul_aggr_retx_prb']
def ricmon_prb_utilization(prb_msg, OBJ_STORE, DL_FRAME, UL_FRAME, SLOT_PRBS):
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
            dl_slot_diffs, ul_slot_diffs = _compute_slots_diff(
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][0]['frame']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][0]['slot']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][1]['frame']),
                int(OBJ_STORE[ue_key]['dl_aggr_prb'][1]['slot']),
                DL_FRAME, UL_FRAME
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
                else round(dl_used_prb / (dl_slot_diffs*SLOT_PRBS), 2)

            ul_res_msg = OBJ_STORE[ue_key]['ul_aggr_prb'].pop(0)
            OBJ_STORE[ue_key]['ul_aggr_retx_prb'].pop(0)
            ul_res_msg['__name__'] = 'ul_prb_utilization'
            ul_res_msg['value'] = 0 if ul_slot_diffs == 0 \
                else round(ul_used_prb / (ul_slot_diffs*SLOT_PRBS), 2)

            return [dl_res_msg, ul_res_msg]
        else:
            return []
    except (IndexError, KeyError):
        return []
