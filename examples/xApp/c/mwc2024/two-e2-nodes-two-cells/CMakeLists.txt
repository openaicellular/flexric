# xapp_rc_ho_kpm
add_executable(xapp_rc_ctrl_ho
        xapp_rc_ctrl_ho.c
        ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
)

target_link_libraries(xapp_rc_ctrl_ho
        PUBLIC
        e42_xapp
        $<TARGET_OBJECTS:e2_time_obj>
        $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
        -pthread
        -lsctp
        -ldl
)

# xapp_rc_moni_ho_pre
add_executable(xapp_rc_moni_ho_pre
        xapp_rc_moni_ho_pre.c
        ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
)

target_link_libraries(xapp_rc_moni_ho_pre
        PUBLIC
        e42_xapp
        $<TARGET_OBJECTS:e2_time_obj>
        $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
        -pthread
        -lsctp
        -ldl
)
