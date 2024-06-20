add_subdirectory(proxy_agent)
add_subdirectory(agent)
add_subdirectory(ric)
add_subdirectory(xApp)
add_subdirectory(sm)
add_subdirectory(util)
add_subdirectory(lib)

if (UNIT_TEST)
    enable_testing()
endif()

