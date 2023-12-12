####################################
# E2AP Encoding and Decoding tests
###################################

if(E2AP_ENCODING STREQUAL "ASN")

####################
# E2AP ASN Encoding
####################

  add_executable(test_e2ap_enc_dec_asn test_e2ap_enc_dec_asn.c)
  target_link_libraries(test_e2ap_enc_dec_asn
                        PUBLIC 
                        e2_agent
                        $<TARGET_OBJECTS:e2ap_ie_obj>
                        )

  target_compile_options(test_e2ap_enc_dec_asn PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)
  target_compile_definitions(test_e2ap_enc_dec_asn PRIVATE ${E2AP_VERSION})

  target_include_directories(test_e2ap_enc_dec_asn PRIVATE "../../../../src/lib/e2ap/v2_03/ie/asn/")
  #  target_include_directories(test_e2ap_enc_dec_asn PRIVATE "../../../src/lib/ap/ie/asn/")


###########################
# E2AP Flatbuffers Encoding
###########################

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "FB not implemented")
else()
  message(FATAL_ERROR "Unknown E2AP encoding type")
endif()

