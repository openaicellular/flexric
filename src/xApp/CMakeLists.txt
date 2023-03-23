add_subdirectory(db)
add_subdirectory(swig)

if(BUILDING_LIBRARY STREQUAL "STATIC")
  set(XAPP_BLD_LIB  "STATIC")
elseif(BUILDING_LIBRARY STREQUAL "DYNAMIC")
  set(XAPP_BLD_LIB  "SHARED")
else()
  message(FATAL_ERROR "Unknown type")
endif()

if(NOT (XAPP_DB STREQUAL "NONE_XAPP"))
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
            ../sm/agent_if/read/sm_ag_if_rd.c
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
            $<TARGET_OBJECTS:e42_xapp_db_obj>)
else()
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
            ../sm/agent_if/read/sm_ag_if_rd.c
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
            $<TARGET_OBJECTS:pending_events_obj>)
endif()

if(E2AP_ENCODING STREQUAL "ASN")
  add_library(e42_xapp ${XAPP_BLD_LIB}# STATIC or SHARED
            ${XAPP_SRC}
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )

          add_library(e42_xapp_shared SHARED # STATIC or SHARED
            ${XAPP_SRC}
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )


elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS" )
  add_library(e42_xapp ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            )

  add_library(e42_xapp_shared ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            )

endif()

target_compile_definitions(e42_xapp PRIVATE ${E2AP_ENCODING} ${XAPP_DB} )
target_compile_definitions(e42_xapp_shared PRIVATE ${E2AP_ENCODING} ${XAPP_DB} )

add_definitions(-DXAPP_DB_DIR="${XAPP_DB_DIR}")

#string(TIMESTAMP NOW "%Y-%m-%dT%H:%M:%SZ")
#string(APPEND XAPP_DB_DIR ${NOW} )
#string(APPEND XAPP_DB_DIR .sqlite3 )
#add_definitions(-DXAPP_DB_DIR="${TODAY}")

