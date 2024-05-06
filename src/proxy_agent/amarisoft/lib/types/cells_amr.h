#ifndef CELLS_AMARISOFT_MSG_STAT_H
#define CELLS_AMARISOFT_MSG_STAT_H 

typedef struct{
    dl_bitrate":0,
    ul_bitrate":0,
    dl_tx":0,
    ul_tx":0,
    dl_err":0,
    ul_err":0,
    dl_retx":0,
    ul_retx":0,
    dl_use_min":0,
    dl_use_max":0.412,
    dl_use_avg":0.032,
    ul_use_min":0.078,
    ul_use_max":0.275,
    ul_use_avg":0.111,
    dl_sched_users_min":0,
    dl_sched_users_max":0,
    dl_sched_users_avg":0,
    ul_sched_users_min":0,
    ul_sched_users_max":0,
    ul_sched_users_avg":0,
    ue_count_min":0,
    ue_count_max":0,
    ue_count_avg":0,
    ue_active_count_min":0,
    ue_active_count_max":0,
    ue_active_count_avg":0,
    ue_inactive_count_min":0,
    ue_inactive_count_max":0,
    ue_inactive_count_avg":0,
    drb_count_min":0,
    drb_count_max":0,
    drb_count_avg":0,
    dl_gbr_use_min":0,
    dl_gbr_use_max":0,
    dl_gbr_use_avg":0,
    ul_gbr_use_min":0,
    ul_gbr_use_max":0,
    ul_gbr_use_avg":0,

    cnt_msgs_amr_t count;


}cell_amr_t ;


#endif

