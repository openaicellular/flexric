# This is a CMake example for Python
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
CMAKE_POLICY(VERSION 3.8)

PROJECT(XAPP_SDK)

CMAKE_POLICY(SET CMP0078 NEW) 
CMAKE_POLICY(SET CMP0086 NEW) 

FIND_PACKAGE(SWIG 4.1 REQUIRED)

#find_package (Python3 COMPONENTS Interpreter Development)

INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

#SET(CMAKE_SWIG_FLAGS "")
#SET(CMAKE BUILD TYPE Debug)

# libconf
find_path(LibConfig_INCLUDE_DIR libconfig.h++)
find_library(LibConfig_LIBRARY NAMES config++)

# mysql db
if(XAPP_DB STREQUAL "MYSQL_XAPP")
  link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})
endif()

SET_PROPERTY(SOURCE swig_wrapper.i PROPERTY CPLUSPLUS ON)

option(XAPP_PYTHON_SWIG "Build xApps in Python" ON)
message(STATUS "Build xApps in Python: ${XAPP_PYTHON_SWIG}")
if(XAPP_PYTHON_SWIG)
  option(XAPP_PYTHON_ML "Enable AI/ML xApps in Python" OFF)
  message(STATUS "Enable AI/ML xApps in Python: ${XAPP_PYTHON_ML}")
endif()
option(XAPP_GO_SWIG "Build xApp in Golang" OFF)
message(STATUS "Build xApp in Golang: ${XAPP_GO_SWIG}")
option(XAPP_JAVA_SWIG "Build xApp in Java" OFF)
message(STATUS "Build xApp in Java: ${XAPP_JAVA_SWIG}")
option(XAPP_JAVASCRIPT_SWIG "Build xApp in Javascript" OFF)
message(STATUS "uild xApp in Javascript: ${XAPP_JAVASCRIPT_SWIG}")

#set(XAPP_LANG_LIST)

if(KPM_VERSION STREQUAL "KPM_V2")
  set(KPM_VERSION_SRC ../../sm/kpm_sm/kpm_sm_v02.03/ie/kpm_data_ie.c)
elseif(KPM_VERSION STREQUAL "KPM_V3")
  set(KPM_VERSION_SRC ../../sm/kpm_sm/kpm_sm_v03.00/ie/kpm_data_ie.c)
else()
  message(FATAL_ERROR "Unknown KPM version")
endif()

#PYTHON compilation
if(XAPP_PYTHON_SWIG)
  if(XAPP_PYTHON_ML)
    FIND_PACKAGE(Python 3.11 EXACT REQUIRED)
    FIND_PACKAGE(PythonLibs 3.11 EXACT REQUIRED)
  else()
    FIND_PACKAGE(Python 3.8 REQUIRED)
    FIND_PACKAGE(PythonLibs 3.8 REQUIRED)
  endif()
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
  message(STATUS "PYTHON_INCLUDE_PATH ${PYTHON_INCLUDE_DIRS}")
  SWIG_ADD_LIBRARY(xapp_sdk LANGUAGE python SOURCES
      swig_wrapper.i
      swig_wrapper.cpp
      $<TARGET_OBJECTS:e2ap_ran_func_obj>
      $<TARGET_OBJECTS:e2ap_plmn_obj>
      $<TARGET_OBJECTS:e2ap_global_node_id_obj>
      $<TARGET_OBJECTS:e2_conv_obj>
      ../../sm/mac_sm/ie/mac_data_ie.c
      ../../sm/rlc_sm/ie/rlc_data_ie.c
      ../../sm/pdcp_sm/ie/pdcp_data_ie.c
      ../../sm/slice_sm/ie/slice_data_ie.c
      ../../sm/tc_sm/ie/tc_data_ie.c
      ../../sm/gtp_sm/ie/gtp_data_ie.c
      ../../util/byte_array.c
      ../../util/alg_ds/alg/eq_float.c
      ../../util/e2ap_ngran_type.c
      ${KPM_VERSION_SRC}
      $<TARGET_OBJECTS:kpm_ric_info_common_obj>
      $<TARGET_OBJECTS:kpm_ric_info_obj>
      $<TARGET_OBJECTS:e2sm_common_ie_obj>
      $<TARGET_OBJECTS:3gpp_common_ie_obj>
      $<TARGET_OBJECTS:sm_common_ie_obj>
      $<TARGET_OBJECTS:3gpp_derived_ie_obj>
      $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
      $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
      $<TARGET_OBJECTS:sm_common_dec_asn_obj>
      $<TARGET_OBJECTS:sm_common_enc_asn_obj>
      $<TARGET_OBJECTS:sm_common_ie_obj>
      )
endif()

#GOLANG compilation
if(XAPP_GO_SWIG)
#  list(APPEND XAPP_LANG_LIST "go")
  SWIG_ADD_LIBRARY(xapp_sdk_go LANGUAGE go SOURCES
      swig_wrapper.i
      swig_wrapper.cpp
      $<TARGET_OBJECTS:e2ap_ran_func_obj>
      $<TARGET_OBJECTS:e2ap_plmn_obj>
      $<TARGET_OBJECTS:e2ap_global_node_id_obj>
      $<TARGET_OBJECTS:e2_conv_obj>
      ../../sm/mac_sm/ie/mac_data_ie.c
      ../../sm/rlc_sm/ie/rlc_data_ie.c
      ../../sm/pdcp_sm/ie/pdcp_data_ie.c
      ../../sm/slice_sm/ie/slice_data_ie.c
      ../../sm/tc_sm/ie/tc_data_ie.c
      ../../sm/gtp_sm/ie/gtp_data_ie.c
      ../../util/byte_array.c
      ../../util/alg_ds/alg/eq_float.c
      ../../util/e2ap_ngran_type.c
      ../../
      ${KPM_VERSION_SRC}
      $<TARGET_OBJECTS:kpm_ric_info_common_obj>
      $<TARGET_OBJECTS:kpm_ric_info_obj>
      $<TARGET_OBJECTS:e2sm_common_ie_obj>
      $<TARGET_OBJECTS:3gpp_common_ie_obj>
      $<TARGET_OBJECTS:sm_common_ie_obj>
      $<TARGET_OBJECTS:3gpp_derived_ie_obj>
      $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
      $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
      $<TARGET_OBJECTS:sm_common_dec_asn_obj>
      $<TARGET_OBJECTS:sm_common_enc_asn_obj>
      $<TARGET_OBJECTS:sm_common_ie_obj>
      )
endif()

#PYTHON compilation
if(XAPP_PYTHON_SWIG)

  TARGET_COMPILE_DEFINITIONS(xapp_sdk PRIVATE XAPP_LANG_PYTHON ${E2AP_ENCODING} ${E2AP_VERSION} ${KPM_VERSION})
  SET_PROPERTY(TARGET xapp_sdk PROPERTY SWIG_COMPILE_DEFINITIONS ${E2AP_VERSION} ${KPM_VERSION})
  SWIG_LINK_LIBRARIES(xapp_sdk ${PYTHON_LIBRARIES})
  SWIG_LINK_LIBRARIES(xapp_sdk ${LibConfig_LIBRARY})

  # xapp_sdk
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/_xapp_sdk.so" "${CMAKE_BINARY_DIR}/examples/xApp/python3/xapp_sdk/_xapp_sdk.so")
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/xapp_sdk.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/xapp_sdk/xapp_sdk.py")
  # monitor
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/examples/xApp/python3/monitor/xapp_cust_moni.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/monitor/xapp_cust_moni.py")
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/examples/xApp/python3/monitor/xapp_all_moni.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/monitor/xapp_all_moni.py")
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/examples/xApp/python3/monitor/xapp_oran_moni.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/monitor/xapp_oran_moni.py")
  # control
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/examples/xApp/python3/control/xapp_slice_moni_ctrl.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/control/xapp_slice_moni_ctrl.py")
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/examples/xApp/python3/control/watch_slice_stats" "${CMAKE_BINARY_DIR}/examples/xApp/python3/control/watch_slice_stats")
  # interactive
  add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/examples/xApp/python3/interactive/xapp.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/interactive/xapp.py")



  # I do not like this solution at all but it is not my code ...
  target_compile_options(xapp_sdk PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter )

  #Comment this line to build it from the directory
  ADD_DEPENDENCIES(xapp_sdk e42_xapp_shared)

  target_include_directories(xapp_sdk PRIVATE
      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_INCLUDE_DIRS}>)
  target_include_directories(xapp_sdk PRIVATE ${LibConfig_INCLUDE_DIR})

  SWIG_LINK_LIBRARIES(xapp_sdk
      e42_xapp_shared
      -ldl
      -lsctp
      -lpthread
      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
      )
endif()

#GOLANG compilation
if(XAPP_GO_SWIG)

  TARGET_COMPILE_DEFINITIONS(xapp_sdk_go PRIVATE XAPP_LANG_GO ${E2AP_ENCODING} ${E2AP_VERSION} ${KPM_VERSION})
  # Compile the Swig wrapper for Go
  SET_PROPERTY(TARGET xapp_sdk_go PROPERTY SWIG_COMPILE_DEFINITIONS ${E2AP_VERSION} ${KPM_VERSION})
  SET_PROPERTY(TARGET xapp_sdk_go PROPERTY SWIG_COMPILE_OPTIONS -intgosize 64 -use-shlib -soname xapp_sdk_go.so -w312 -w314)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

  #Check if go.mod exists
  if(EXISTS "${CMAKE_BINARY_DIR}/go.mod")
    MESSAGE(STATUS "go.mod already exists")
  else()
    execute_process(COMMAND go mod init build
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        RESULT_VARIABLE go_mod_init_result
        OUTPUT_VARIABLE go_mod_init_output
        ERROR_VARIABLE go_mod_init_error)
    if(NOT go_mod_init_result EQUAL 0)
      message(FATAL_ERROR "Failed to run 'go mod init' in swig: ${go_mod_init_error}")
    else()
      MESSAGE(STATUS "go.mod init successful from swig")
    endif()
  endif()

  # Copy the necessary files to the build directory
  SWIG_LINK_LIBRARIES(xapp_sdk_go ${GO_LIBRARIES})
  SWIG_LINK_LIBRARIES(xapp_sdk_go ${LibConfig_LIBRARY})

  # Copy go.mod, xapp_sdk.go and libxapp_sdk.so to the build directory
  # These 3 files are necessary to build the xApp
  add_custom_command(TARGET xapp_sdk_go POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/xapp_sdk_go.so" "${CMAKE_BINARY_DIR}/examples/xApp/go/xapp_sdk/libxapp_sdk.so" )

  add_custom_command(TARGET xapp_sdk_go POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/xapp_sdk.go" "${CMAKE_BINARY_DIR}/examples/xApp/go/xapp_sdk/xapp_sdk.go" )

  # Gather all files in examples/xApp/go directory recursively
  file(GLOB_RECURSE GO_FILES RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/examples/xApp/go/*")

  foreach(GO_FILE IN LISTS GO_FILES)
    get_filename_component(GO_FILE_PATH ${GO_FILE} DIRECTORY)

    # Ensure the destination directory exists
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${GO_FILE_PATH}")

    add_custom_command(TARGET xapp_sdk_go POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/${GO_FILE}"
      "${CMAKE_BINARY_DIR}/${GO_FILE}"
      COMMENT "Copying ${GO_FILE}"
    )
  endforeach()

  # This variable will hold the paths to all the Go binaries
  set(GO_BINARIES "")



  # Gather all Go _main.go files in examples/xApp/go directory recursively
  file(GLOB_RECURSE GO_MAIN_FILES RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/examples/xApp/go/*/*_main.go")

  foreach(GO_MAIN_FILE IN LISTS GO_MAIN_FILES)
    get_filename_component(GO_APP_DIR ${GO_MAIN_FILE} DIRECTORY)

    # Extract the file's base name
    get_filename_component(GO_FILE_NAME_WE ${GO_MAIN_FILE} NAME_WE)

    # Remove the _main suffix to get the executable name
    string(REGEX REPLACE "_main$" "" EXECUTABLE_NAME ${GO_FILE_NAME_WE})

    add_custom_command(
      TARGET xapp_sdk_go
      POST_BUILD
      COMMAND go mod tidy
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${GO_APP_DIR}
      COMMENT "Running go mod tidy in ${GO_APP_DIR}"
    )

    # Add "go_" prefix to the executable name
    set(PREFIXED_EXECUTABLE_NAME "go_${EXECUTABLE_NAME}")

    add_custom_command(
      TARGET xapp_sdk_go
      POST_BUILD
      COMMAND go build -o ${PREFIXED_EXECUTABLE_NAME}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${GO_APP_DIR}
      COMMENT "Building Go application in ${GO_APP_DIR}"
    )

    # Add the path to the Go binary to the list of binaries
    list(APPEND GO_BINARIES "${CMAKE_BINARY_DIR}/${GO_APP_DIR}/${PREFIXED_EXECUTABLE_NAME}")
  endforeach()

  set(GO_BINARIES "${GO_BINARIES}" CACHE INTERNAL "List of Go binaries")

  #Comment this line to build it from the directory
  ADD_DEPENDENCIES(xapp_sdk_go e42_xapp_shared)

  target_include_directories(xapp_sdk_go PRIVATE
      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_INCLUDE_DIRS}>)
  target_include_directories(xapp_sdk_go PRIVATE ${LibConfig_INCLUDE_DIR})

  SWIG_LINK_LIBRARIES(xapp_sdk_go
      e42_xapp_shared
      -ldl
      -lsctp
      -lpthread
      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
      )
endif()

#JAVA compilation
if(XAPP_JAVA_SWIG)
 message(FATAL_ERROR "SWIG for JAVA not implemented")
# ADD_COMPILE_DEFINITIONS(XAPP_LANG_JAVA)
# INCLUDE_DIRECTORIES(/usr/lib/jvm/default-java/include)
# INCLUDE_DIRECTORIES(/usr/lib/jvm/default-java/include/linux)
endif()

#JAVASCRIPT compilation
if(XAPP_JAVASCRIPT_SWIG)
  message(FATAL_ERROR "SWIG for JAVASCRIPT not implemented")
#  ADD_COMPILE_DEFINITIONS(XAPP_LANG_JAVASCRIPT)
#  SET_PROPERTY(TARGET xapp_sdk_javascript PROPERTY SWIG_COMPILE_OPTIONS -v8)
#  INCLUDE_DIRECTORIES(/usr/include/nodejs/src)
#  INCLUDE_DIRECTORIES(/usr/include/nodejs/deps/v8/include/)
endif()





