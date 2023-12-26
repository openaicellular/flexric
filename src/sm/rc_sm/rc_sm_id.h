/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */


#ifndef RAN_CTRL_SERVICE_MODEL_ID_H
#define RAN_CTRL_SERVICE_MODEL_ID_H 


/*
 * Service Model ID needed for the agent as well as for the ric to ensure that they match. 
 */

#include <stdint.h>

typedef enum{
    DRX_parameter_configuration_7_6_3_1 = 1,
    SR_periodicity_configuration_7_6_3_1 = 2,
    SPS_parameters_configuration_7_6_3_1 = 3,
    Configured_grant_control_7_6_3_1 = 4,
    CQI_table_configuration_7_6_3_1 = 5,
    Slice_level_PRB_quotal_7_6_3_1 = 6,
} rc_ctrl_service_style_2_act_id_e;

typedef enum {
    RRM_Policy_Ratio_List_8_4_3_6 = 1,
    RRM_Policy_Ratio_Group_8_4_3_6 = 2,
    RRM_Policy_8_4_3_6 = 3,
    RRM_Policy_Member_List_8_4_3_6 = 4,
    RRM_Policy_Member_8_4_3_6 = 5,
    PLMN_Identity_8_4_3_6 = 6,
    S_NSSAI_8_4_3_6 = 7,
    SST_8_4_3_6 = 8,
    SD_8_4_3_6 = 9,
    Min_PRB_Policy_Ratio_8_4_3_6 = 10,
    Max_PRB_Policy_Ratio_8_4_3_6 = 11,
    Dedicated_PRB_Policy_Ratio_8_4_3_6 = 12,
} slice_level_PRB_quota_param_id_e;

typedef enum{
    Handover_control_7_6_4_1 = 1,
    Conditional_handover_control_7_6_4_1 = 2,
    DAPS_7_6_4_1 = 3,
} rc_ctrl_service_style_3_act_id_e;

typedef enum{
    Amarisoft_ran_ue_id = 1,
    Target_primary_cell_id_8_4_4_1 = 2,
    CHOICE_target_cell_8_4_4_1 = 3,
    NR_cell_8_4_4_1 = 4,
    NR_CGI_8_4_4_1 = 5,
    E_ULTRA_Cell_8_4_4_1 = 6,
    E_ULTRA_CGI_8_4_4_1 = 7,
} handover_control_param_id_e;

static
const uint16_t SM_RC_ID = 3; 

static
const uint16_t SM_RC_REV = 1; 

__attribute__((unused)) static
const char SM_RAN_CTRL_SHORT_NAME[] = "ORAN-E2SM-RC-v01.0";

//iso(1) identified-organization(3)
//dod(6) internet(1) private(4)
//enterprise(1) 53148 e2(1)
// version1 (1) e2sm(2) e2sm-RC-
// IEs (3)

__attribute__((unused)) static
const char SM_RAN_CTRL_OID[] = "1.3.6.1.4.1.53148.1.1.2.3"; 

__attribute__((unused)) static
const char SM_RAN_CTRL_DESCRIPTION[] = "RAN Control";

#endif

