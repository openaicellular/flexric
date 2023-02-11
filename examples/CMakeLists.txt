if (EMU_AGENT_INSTALL)
    add_subdirectory(emulator)
endif()
if (NEAR_RIC_INSTALL)
    add_subdirectory(ric)
endif()
if (XAPP_PYTHON_INSTALL OR XAPP_C_INSTALL)
    add_subdirectory(xApp)
endif()


