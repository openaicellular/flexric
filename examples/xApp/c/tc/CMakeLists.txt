
include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})

add_executable(xapp_tc_all
                xapp_tc_all.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_include_directories(xapp_tc_all PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})

target_link_libraries(xapp_tc_all
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      ${LIBMYSQLCLIENT_LIBRARIES}
                      )

add_executable(xapp_tc_codel
                xapp_tc_codel.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_include_directories(xapp_tc_codel PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})

target_link_libraries(xapp_tc_codel
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      ${LIBMYSQLCLIENT_LIBRARIES}
                      )

add_executable(xapp_tc_partition
               xapp_tc_partition.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_include_directories(xapp_tc_partition PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})

target_link_libraries(xapp_tc_partition
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                       ${LIBMYSQLCLIENT_LIBRARIES}
                      )

add_executable(xapp_tc_ecn
                xapp_tc_ecn.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_include_directories(xapp_tc_ecn PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})

target_link_libraries(xapp_tc_ecn
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      ${LIBMYSQLCLIENT_LIBRARIES}
                      )

add_executable(xapp_tc_osi_codel
               xapp_tc_osi_codel.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../../../src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                )

target_include_directories(xapp_tc_osi_codel PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})

target_link_libraries(xapp_tc_osi_codel
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                       ${LIBMYSQLCLIENT_LIBRARIES}
                      )

