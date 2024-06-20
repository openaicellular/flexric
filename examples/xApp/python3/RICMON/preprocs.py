from typing import List
from ricmon.metrics import dl_prb_utilization, ul_prb_utilization


# Define my Radio Structure
DL_FRAME = [1, 1, 1, 1, 1, 1, 1, 0.45, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 0.45, 0, 0]
UL_FRAME = [0, 0, 0, 0, 0, 0, 0, 0.3, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0.3, 1, 1]
PRBS_PER_SLOT = 106

# Extend into my preprocessors
def my_dl_prb_utilization(data_msg: dict, OBJ_STORE: dict) -> List[dict]:
    return dl_prb_utilization(data_msg, OBJ_STORE, 
                              "dl_prb_utilization", DL_FRAME, PRBS_PER_SLOT)

def my_ul_prb_utilization(data_msg: dict, OBJ_STORE: dict) -> List[dict]:
    return ul_prb_utilization(data_msg, OBJ_STORE, 
                              "ul_prb_utilization", UL_FRAME, PRBS_PER_SLOT)

# Write my own preprocessor
def my_dummy_preprocessor(data_msg: dict, OBJ_STORE: dict) -> List[dict]:
    data_msg['__name__'] = f"doubled_{data_msg['__name__']}"
    data_msg['value'] *= 2

    return [data_msg]
