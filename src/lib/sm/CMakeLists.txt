add_subdirectory(dec)
add_subdirectory(enc)
add_subdirectory(ie)

######
### KPM
#####


add_library(e2sm_common_ie_obj_kpm OBJECT
                                $<TARGET_OBJECTS:sm_common_ie_obj>
                                $<TARGET_OBJECTS:sm_common_dec_asn_obj_kpm>
                                $<TARGET_OBJECTS:sm_common_enc_asn_obj_kpm>
                                )

######
### RC
#####
add_library(e2sm_common_ie_obj_rc OBJECT
                                $<TARGET_OBJECTS:sm_common_ie_obj>
                                $<TARGET_OBJECTS:sm_common_dec_asn_obj_rc>
                                $<TARGET_OBJECTS:sm_common_enc_asn_obj_rc>
                                )

