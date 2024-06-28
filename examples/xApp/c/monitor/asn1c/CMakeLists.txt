# Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The OpenAirInterface Software Alliance licenses this file to You under
# the OAI Public License, Version 1.1  (the "License"); you may not use this file
# except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.openairinterface.org/?page_id=698
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------------------------------------
# For more information about the OpenAirInterface (OAI) Software Alliance:
#      contact@openairinterface.org
#

set(NR_RRC_VERSION 17 3 0)
include(macros.cmake)
make_version(NR_RRC_cc ${NR_RRC_VERSION})
string(REPLACE ";" "." NR_RRC_RELEASE "${NR_RRC_VERSION}")

if(NR_RRC_RELEASE VERSION_EQUAL "17.3.0")
  include(nr-rrc-17.3.0.cmake)
else()
  message(FATAL_ERROR "unknown NR_RRC_RELEASE ${NR_RRC_RELEASE}")
endif()

find_program(ASN1C_EXEC_PATH asn1c HINTS /opt/asn1c/bin)
set(ASN1C_EXEC ${ASN1C_EXEC_PATH} CACHE FILEPATH "path to asn1c executable")
add_custom_command(OUTPUT ${nr_rrc_source} ${nr_rrc_headers}
  COMMAND ASN1C_PREFIX=NR_ ${ASN1C_EXEC} -pdu=all -fcompound-names -gen-UPER -no-gen-BER -no-gen-JER -no-gen-OER -gen-APER -no-gen-example -findirect-choice -D ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${NR_RRC_GRAMMAR}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NR_RRC_GRAMMAR}
  COMMENT "Generating NR RRC source file from ${CMAKE_CURRENT_SOURCE_DIR}/${NR_RRC_GRAMMAR}"
)

add_library(asn1_nr_rrc_hdrs INTERFACE ${nr_rrc_headers})
target_include_directories(asn1_nr_rrc_hdrs INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")

add_library(asn1_nr_rrc ${nr_rrc_source} ${nr_rrc_headers})
target_link_libraries(asn1_nr_rrc PUBLIC asn1_nr_rrc_hdrs)
target_compile_options(asn1_nr_rrc
                         PRIVATE -DASN_DISABLE_OER_SUPPORT -w
                         PUBLIC -DNR_RRC_VERSION=${NR_RRC_RELEASE})

