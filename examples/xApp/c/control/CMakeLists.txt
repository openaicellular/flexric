if(XAPP_DB STREQUAL "MYSQL_XAPP")
	include_directories(${LIBMYSQLCLIENT_INCLUDE_DIRS})
endif()

# xapp_mac_ctrl
add_executable(xapp_mac_ctrl
								xapp_mac_ctrl.c
								${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
								)

target_link_libraries(xapp_mac_ctrl
											PUBLIC
											e42_xapp
											$<TARGET_OBJECTS:e2_time_obj>
											$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
											-pthread
											-lsctp
											-ldl
											)

# xapp_slice_moni_ctrl
add_executable(xapp_slice_moni_ctrl
		xapp_slice_moni_ctrl.c
		${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
)

target_link_libraries(xapp_slice_moni_ctrl
		PUBLIC
		e42_xapp
		$<TARGET_OBJECTS:e2_time_obj>
		$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
		-pthread
		-lsctp
		-ldl
)

# xapp_oran_ctrl
add_executable(xapp_oran_ctrl
		xapp_oran_ctrl.c
		${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
		)

target_link_libraries(xapp_oran_ctrl
		PUBLIC
		e42_xapp
		$<TARGET_OBJECTS:e2_time_obj>
		$<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
		-pthread
		-lsctp
		-ldl
		)

