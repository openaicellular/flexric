if(XAPP_DB STREQUAL "MYSQL_XAPP")
	include_directories(${LIBMYSQLCLIENT_INCLUDE_DIRS})
endif()

# xapp_mac_rlc_pdcp_moni
add_executable(xapp_mac_rlc_pdcp_moni
								xapp_mac_rlc_pdcp_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
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

# xapp_moni
add_executable(xapp_moni
								xapp_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
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