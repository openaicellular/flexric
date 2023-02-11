if (EMU_AGENT_INSTALL)
    add_subdirectory(agent)
    add_subdirectory(proxy-agent)
endif()
if (NEAR_RIC_INSTALL)
    add_subdirectory(ric)
endif()
if (XAPP_PYTHON_INSTALL OR XAPP_C_INSTALL)
    add_subdirectory(xApp)
endif()
add_subdirectory(sm)
add_subdirectory(util)
add_subdirectory(lib)



