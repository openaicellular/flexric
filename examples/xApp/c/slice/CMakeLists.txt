
add_executable(xapp_slice_moni_ctrl 
             xapp_slice_moni_ctrl.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/rrc_sm/ie/rrc_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_slice_moni_ctrl
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )



