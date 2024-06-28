add_subdirectory(db)
if(XAPP_MULTILANGUAGE)
  add_subdirectory(swig)
endif()

if(BUILDING_LIBRARY STREQUAL "STATIC")
  set(XAPP_BLD_LIB  "STATIC")
elseif(BUILDING_LIBRARY STREQUAL "DYNAMIC")
  set(XAPP_BLD_LIB  "SHARED")
else()
  message(FATAL_ERROR "Unknown type")
endif()


if(KPM_VERSION STREQUAL "KPM_V2_01")
  set(KPM_VERSION_SRC ../sm/kpm_sm/kpm_sm_v02.01/ie/kpm_data_ie.c)
elseif(KPM_VERSION STREQUAL "KPM_V2_03")
  set(KPM_VERSION_SRC ../sm/kpm_sm/kpm_sm_v02.03/ie/kpm_data_ie.c)
elseif(KPM_VERSION STREQUAL "KPM_V3_00")
  set(KPM_VERSION_SRC ../sm/kpm_sm/kpm_sm_v03.00/ie/kpm_data_ie.c)
else()
  message(FATAL_ERROR "Unknown KPM version")
endif()


set(XAPP_SRC 
  e2ap_xapp.c
  asio_xapp.c
  endpoint_xapp.c
  msg_handler_xapp.c
  msg_generator_xapp.c
  e42_xapp.c
  e42_xapp_api.c
  plugin_agent.c
  plugin_ric.c
  pending_event_xapp.c
  sync_ui.c
  act_proc.c
  msg_dispatcher_xapp.c

  e2_node_arr_xapp.c
  e2_node_connected_xapp.c
  sm_ran_function.c
  sm_ran_function_def.c

  ../sm/agent_if/read/sm_ag_if_rd.c
 
  $<TARGET_OBJECTS:e2_conv_obj>
  $<TARGET_OBJECTS:e2ap_ep_obj> 
  $<TARGET_OBJECTS:e2ap_ap_obj>
  $<TARGET_OBJECTS:msg_hand_obj>
  $<TARGET_OBJECTS:e2ap_ds_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:e2_conf_obj>
  $<TARGET_OBJECTS:e2ap_msg_enc_obj>
  $<TARGET_OBJECTS:e2ap_msg_dec_obj>
  $<TARGET_OBJECTS:e2ap_msg_free_obj>
  $<TARGET_OBJECTS:e2ap_types_obj> 
  $<TARGET_OBJECTS:pending_events_obj>
  $<TARGET_OBJECTS:e42_xapp_db_obj>

  $<TARGET_OBJECTS:sm_common_ie_obj>
  $<TARGET_OBJECTS:3gpp_derived_ie_obj>

  # KPM SM
  ${KPM_VERSION_SRC}
  ../../test/rnd/fill_rnd_data_kpm.c
  $<TARGET_OBJECTS:kpm_ric_info_common_obj>
  $<TARGET_OBJECTS:kpm_ric_info_obj>

  # RAN Control SM
  ../sm/rc_sm/ie/rc_data_ie.c
  ../../test/rnd/fill_rnd_data_rc.c
  $<TARGET_OBJECTS:e2sm_rc_ir_obj>
  
  ../util/ngran_types.c
  ../sm/mac_sm/ie/mac_data_ie.c
  ../sm/rlc_sm/ie/rlc_data_ie.c
  ../sm/pdcp_sm/ie/pdcp_data_ie.c
  ../sm/slice_sm/ie/slice_data_ie.c
  ../sm/tc_sm/ie/tc_data_ie.c
  ../sm/gtp_sm/ie/gtp_data_ie.c

  )

if(E2AP_ENCODING STREQUAL "ASN")
  add_library(e42_xapp 
            ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )

          add_library(e42_xapp_shared SHARED # STATIC or SHARED
            ${XAPP_SRC}
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            $<TARGET_OBJECTS:e2_conv_obj>
            )

	 target_compile_options(e42_xapp_shared PRIVATE "-DASN_DISABLE_OER_SUPPORT" -fPIC)

if(E2AP_VERSION STREQUAL "E2AP_V1")
	target_include_directories(e42_xapp_shared PRIVATE "../lib/e2ap/v1_01/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
	target_include_directories(e42_xapp_shared PRIVATE "../lib/e2ap/v2_03/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V3")
	target_include_directories(e42_xapp_shared PRIVATE "../lib/e2ap/v3_01/ie/asn")
else()
  message(FATAL_ERROR "E2AP unknown version")
endif()

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS" )
  add_library(e42_xapp ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            )

  add_library(e42_xapp_shared ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            )

endif()

target_compile_definitions(e42_xapp PUBLIC ${E2AP_ENCODING} ${E2AP_VERSION} ${XAPP_DB} ${KPM_VERSION})
target_compile_definitions(e42_xapp_shared PUBLIC ${E2AP_ENCODING} ${E2AP_VERSION} ${XAPP_DB}  ${KPM_VERSION})

add_definitions(-DXAPP_DB_DIR="${XAPP_DB_DIR}")

#string(TIMESTAMP NOW "%Y-%m-%dT%H:%M:%SZ")
#string(APPEND XAPP_DB_DIR ${NOW} )
#string(APPEND XAPP_DB_DIR .sqlite3 )
#add_definitions(-DXAPP_DB_DIR="${TODAY}")

