add_executable(xapp_hw
				hw.c
				${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
				${CMAKE_SOURCE_DIR}/src/util/e2ap_ngran_type.c
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
