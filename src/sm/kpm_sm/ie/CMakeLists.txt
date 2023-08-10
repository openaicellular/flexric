add_subdirectory(kpm_data_ie)

if(KPM_v3_00  STREQUAL "OFF")
  message(STATUS "Generating KPM v2.03 asn1 files:  $/opt/asn1c/bin/asn1c ../src/sm/kpm_sm/ie/e2sm_kpm_v2_03_modified.asn1 -gen-APER -no-gen-OER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps -D ../src/sm/kpm_sm/ie/asn")
  execute_process(COMMAND /opt/asn1c/bin/asn1c ../src/sm/kpm_sm/ie/e2sm_kpm_v2_03_modified.asn1 -gen-APER -no-gen-OER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps -D ../src/sm/kpm_sm/ie/asn)

else()
  message(STATUS "Generating KPM v3.00 asn1 files:  $/opt/asn1c/bin/asn1c ../src/sm/kpm_sm/ie/e2sm_kpm_v3_00_modified.asn1 -gen-APER -no-gen-OER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps -D ../src/sm/kpm_sm/ie/asn")
  execute_process(COMMAND /opt/asn1c/bin/asn1c ../src/sm/kpm_sm/ie/e2sm_kpm_v3_00_modified.asn1 -gen-APER -no-gen-OER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps -D ../src/sm/kpm_sm/ie/asn)

endif()
