set(JSON_CCC_SRC
        cJSON.c
        list.c
        parse_cjson.c
        ric_indication_header.c
        ric_action_definition.c
)

add_library(e2sm_ccc_json_obj OBJECT ${JSON_CCC_SRC})
target_include_directories(e2sm_ccc_json_obj
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
# For using sem in list.c

#target_compile_options(e2sm_ccc_json_obj PUBLIC "-DASN_DISABLE_OER_SUPPORT")
#target_compile_options(e2sm_ccc_json_obj PUBLIC "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(e2sm_ccc_json_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC -fvisibility=hidden )

