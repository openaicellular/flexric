
if(XAPP_DB STREQUAL "SQLITE3_XAPP")

  #  add_subdirectory(sqlite3)
  add_library(e42_xapp_db_obj OBJECT 
                         db.c
                         sqlite3/sqlite3.c
                         sqlite3/sqlite3_wrapper.c
                         ../../util/time_now_us.c
                         )
else()
  message(FATAL_ERROR "Unknown XAPP_DB selected")
endif()

target_compile_definitions(e42_xapp_db_obj PRIVATE ${XAPP_DB} ${E2AP_VERSION} ${KPM_VERSION} )

