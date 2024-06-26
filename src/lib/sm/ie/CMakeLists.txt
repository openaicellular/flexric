set(SM_COMMON_IE_SRCS
    ue_id.c
    cell_global_id.c
    ran_function_name.c)

add_library(sm_common_ie_obj OBJECT ${SM_COMMON_IE_SRCS})

target_compile_options(sm_common_ie_obj PRIVATE "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(sm_common_ie_obj PRIVATE "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(sm_common_ie_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC -fvisibility=hidden)
