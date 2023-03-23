if(XAPP_DB STREQUAL "MYSQL_XAPP")
	include_directories(${LIBMYSQLCLIENT_INCLUDE_DIRS})
endif()

# xapp_mac_rlc_pdcp_moni
add_executable(xapp_mac_rlc_pdcp_moni
								xapp_mac_rlc_pdcp_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
								)

target_link_libraries(xapp_mac_rlc_pdcp_moni
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)

# xapp_gtp_moni
add_executable(xapp_gtp_moni
								xapp_gtp_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
								)



target_link_libraries(xapp_gtp_moni
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)

# xapp_kpm_moni
add_executable(xapp_kpm_moni
								xapp_kpm_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/util/ngran_type.c
								)

# target_include_directories(xapp_kpm_moni PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})

target_link_libraries(xapp_kpm_moni
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)
# enable below directive if you need to debug asn code
# target_compile_options(xapp_kpm_moni PUBLIC -DASN_EMIT_DEBUG=1)

# xapp_kpm_moni_infinity
add_executable(xapp_kpm_moni_infinity
								xapp_kpm_moni_infinity.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
								)

target_link_libraries(xapp_kpm_moni_infinity
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)

# xapp_moni
add_executable(xapp_moni
								xapp_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
								${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
								${CMAKE_SOURCE_DIR}/src/util/ngran_type.c
								)

target_link_libraries(xapp_moni
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)