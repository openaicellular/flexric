add_subdirectory(proxy_ag)

if (EMU_AGENT)
    add_subdirectory(agent)
endif()
if (NEAR_RIC)
    add_subdirectory(ric)
endif()
if (XAPP_C)
    add_subdirectory(xApp)
endif()
add_subdirectory(sm)
if (UNIT_TEST)
    enable_testing()
endif()
add_subdirectory(util)
add_subdirectory(lib)



