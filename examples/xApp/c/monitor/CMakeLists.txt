if(XAPP_DB STREQUAL "MYSQL_XAPP")
	include_directories(${LIBMYSQLCLIENT_INCLUDE_DIRS})
endif()

# xapp_cust_moni
add_executable(xapp_cust_moni
		xapp_cust_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								)

target_link_libraries(xapp_cust_moni
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)

# xapp_oran_moni
add_executable(xapp_oran_moni
		xapp_oran_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								)

target_link_libraries(xapp_oran_moni
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

# xapp_all_moni
add_executable(xapp_all_moni
		xapp_all_moni.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								${CMAKE_SOURCE_DIR}/src/util/e2ap_ngran_type.c
								)

target_link_libraries(xapp_all_moni
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)