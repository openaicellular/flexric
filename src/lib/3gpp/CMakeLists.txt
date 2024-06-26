add_subdirectory(dec)
add_subdirectory(enc)
add_subdirectory(ie)

######
### KPM
#####
add_library(3gpp_common_ie_obj_kpm OBJECT
                                $<TARGET_OBJECTS:3gpp_derived_ie_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj_kpm>
                                $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj_kpm>
                                )
######
### RC
#####

add_library(3gpp_common_ie_obj_rc OBJECT
                                $<TARGET_OBJECTS:3gpp_derived_ie_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj_rc>
                                $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj_rc>
                                )

