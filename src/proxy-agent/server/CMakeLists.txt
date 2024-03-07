find_package(libwebsockets CONFIG REQUIRED)

set(SAMP proxy-server)
set(SRCS main.c)

add_executable(${SAMP}
        ${SRCS}
        json_ser_ran.c
        ran_msg_hdlr.c
)

target_link_libraries(${SAMP} websockets
                        ${LIBWEBSOCKETS_DEP_LIBS}
                        ${LIBJSON-C_LIBRARIES}
                        $<TARGET_OBJECTS:e2ap_ds_obj>
                        $<TARGET_OBJECTS:e2ap_alg_obj>
                        )