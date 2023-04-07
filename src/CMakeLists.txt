if (EMU_AGENT_INSTALL OR UNIT_TEST)
    add_subdirectory(agent)
endif()

if (RAN_WITH_WS)
    add_subdirectory(proxy-agent)
endif()

if (NEAR_RIC_INSTALL OR UNIT_TEST)
    add_subdirectory(ric)
endif()

if (XAPP_PYTHON_INSTALL OR XAPP_C_INSTALL OR UNIT_TEST)
    add_subdirectory(xApp)
endif()

add_subdirectory(sm)
add_subdirectory(util)
add_subdirectory(lib)



