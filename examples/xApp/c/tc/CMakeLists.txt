if(XAPP_DB STREQUAL "MYSQL_XAPP")
	include_directories(${LIBMYSQLCLIENT_INCLUDE_DIRS})
	link_libraries(${LIBMYSQLCLIENT_LIBRARIES})
endif()

add_executable(xapp_tc_all
                xapp_tc_all.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                ${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_tc_all
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_codel
                xapp_tc_codel.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                ${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_tc_codel
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_partition
               xapp_tc_partition.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                ${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_tc_partition
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                       )

add_executable(xapp_tc_ecn
                xapp_tc_ecn.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                ${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_tc_ecn
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_osi_codel
               xapp_tc_osi_codel.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                ${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_link_libraries(xapp_tc_osi_codel
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

