add_executable(xapp_hw
				hw.c
				${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
				${CMAKE_SOURCE_DIR}/src/util/ngran_type.c
				${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
				${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
				${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
				${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
				${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
				${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
				${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
				)
if(XAPP_DB STREQUAL "MYSQL_XAPP")
	include_directories(${LIBMYSQLCLIENT_INCLUDE_DIRS})
endif()

target_link_libraries(xapp_hw
						PUBLIC
						e42_xapp
						$<TARGET_OBJECTS:e2_time_obj>
						$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
						-pthread
						-lsctp
						-ldl
						)
	