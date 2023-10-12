if(XAPP_DB STREQUAL "SQLITE3_XAPP")
  add_library(e42_xapp_db_obj OBJECT 
                                    db.c
                                    $<TARGET_OBJECTS:e2_conf_obj>
                                    sqlite3/sqlite3.c
                                    sqlite3/sqlite3_wrapper.c
                                    $<TARGET_OBJECTS:e2_time_obj>)
  target_compile_definitions(e42_xapp_db_obj PRIVATE ${XAPP_DB} ${E2AP_VERSION} ${KPM_VERSION} )
elseif(XAPP_DB STREQUAL "MYSQL_XAPP")
  find_package(PkgConfig)
  pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)
  foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
	  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
  endforeach()
  add_library(e42_xapp_db_obj OBJECT
                                    db.c
                                    $<TARGET_OBJECTS:e2_conf_obj>
                                    mysql/mysql_wrapper.c
                                    $<TARGET_OBJECTS:e2_time_obj>)
  target_link_libraries(e42_xapp_db_obj PRIVATE ${LIBMYSQLCLIENT_LINK_LIBRARIES})
  target_compile_definitions(e42_xapp_db_obj PRIVATE ${XAPP_DB} ${E2AP_VERSION} ${KPM_VERSION} )
else()
  message(STATUS "skipping DB component for xapp.")

endif()



