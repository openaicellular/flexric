
add_executable(xapp_mac_rlc_pdcp_moni 
		xapp_mac_rlc_pdcp_moni.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_mac_rlc_pdcp_moni
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      -I/usr/local/mysql/include
                      -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -ldl -lssl -lcrypto -lresolv -lm -lrt
                      )


add_executable(xapp_gtp_moni 
		xapp_gtp_moni.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_gtp_moni
		PUBLIC
		e42_xapp
		-pthread
		-lsctp
		-ldl
		-I/usr/local/mysql/include
		-L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -ldl -lssl -lcrypto -lresolv -lm -lrt
		)

add_executable(xapp_kpm_moni 
                xapp_kpm_moni.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                )

target_link_libraries(xapp_kpm_moni
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      -I/usr/local/mysql/include
                      -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -ldl -lssl -lcrypto -lresolv -lm -lrt
                      )
target_compile_options(xapp_kpm_moni PUBLIC -DASN_EMIT_DEBUG=1)

add_executable(xapp_kpm_moni_infinity
                      xapp_kpm_moni_infinity.c
                      ../../../../src/util/alg_ds/alg/defer.c
                      ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                      ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                      ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                      ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                      ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                      ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                      ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                      )

target_link_libraries(xapp_kpm_moni_infinity
                            PUBLIC
                            e42_xapp
                            -pthread
                            -lsctp
                            -ldl
                            -I/usr/local/mysql/include
                            -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -ldl -lssl -lcrypto -lresolv -lm -lrt
                            )

add_executable(xapp_moni
		xapp_moni.c
		../../../../src/util/alg_ds/alg/defer.c
		../../../../src/sm/mac_sm/ie/mac_data_ie.c
		../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
		../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
		../../../../src/sm/slice_sm/ie/slice_data_ie.c
		../../../../src/sm/tc_sm/ie/tc_data_ie.c
		../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
		../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
		../../../../src/util/ngran_type.c
		)

target_link_libraries(xapp_moni
		PUBLIC
		e42_xapp
		-pthread
		-lsctp
		-ldl
		-I/usr/local/mysql/include
		-L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -ldl -lssl -lcrypto -lresolv -lm -lrt
		)