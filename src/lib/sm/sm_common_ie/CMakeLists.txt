set(SM_COMMON_IE_SRCS
    ue_id.c
    cell_global_id.c
    ran_function_name.c)

include_directories("/usr/local/share/asn1c/")

add_library(sm_common_ie_obj OBJECT ${SM_COMMON_IE_SRCS})



