if (PROXY_AGENT)
    add_subdirectory(proxy_agent)
endif()
if (EMU_AGENT)
    add_subdirectory(emulator)
endif()
if (NEAR_RIC)
    add_subdirectory(ric)
endif()
if (XAPP_C)
    add_subdirectory(xApp)
endif()


