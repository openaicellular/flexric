set (IR_CCC_SRC
        attribute.c
        act_def_ran_conf.c
        ctrl_out_conf_accepted.c
        ctrl_out_conf_failed.c
        e2sm_ccc_act_def_frmt_1.c
        e2sm_ccc_act_def_frmt_2.c
        e2sm_ccc_ctrl_hdr_frmt_1.c
        e2sm_ccc_ctrl_msg_frmt_1.c
        e2sm_ccc_ctrl_msg_frmt_2.c
        e2sm_ccc_ctrl_out_frmt_1.c
        e2sm_ccc_ctrl_out_frmt_2.c
        e2sm_ccc_ev_trg_frmt_1.c
        e2sm_ccc_ev_trg_frmt_2.c
        e2sm_ccc_ev_trg_frmt_3.c
        e2sm_ccc_ind_hdr_frmt_1.c
        e2sm_ccc_ind_msg_frmt_1.c
        e2sm_ccc_ind_msg_frmt_2.c
  )

add_library(e2sm_ccc_ir_obj OBJECT ${IR_CCC_SRC})
target_include_directories(e2sm_ccc_ir_obj
                            PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                           )

target_compile_options(e2sm_ccc_ir_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter  -fPIC -fvisibility=hidden )


