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

/*! \file common/ngran_types.h
* \brief Definitions for NGRAN node types
* \author R. Knopp
* \date 2018
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr
* \note
* \warning
*/

#ifndef __E2AP_NGRAN_TYPES_H__
#define __E2AP_NGRAN_TYPES_H__

typedef enum {
  e2ap_ngran_eNB       = 0,
  e2ap_ngran_ng_eNB    = 1,
  e2ap_ngran_gNB       = 2,
  e2ap_ngran_eNB_CU    = 3,
  e2ap_ngran_ng_eNB_CU = 4,
  e2ap_ngran_gNB_CU    = 5,
  e2ap_ngran_eNB_DU    = 6,
  e2ap_ngran_gNB_DU    = 7,
  e2ap_ngran_eNB_MBMS_STA  = 8
} e2ap_ngran_node_t;

#define E2AP_NODE_IS_MONOLITHIC(nOdE_TyPe) ((nOdE_TyPe) == e2ap_ngran_eNB    || (nOdE_TyPe) == e2ap_ngran_ng_eNB    || (nOdE_TyPe) == e2ap_ngran_gNB)
#define E2AP_NODE_IS_CU(nOdE_TyPe)         ((nOdE_TyPe) == e2ap_ngran_eNB_CU || (nOdE_TyPe) == e2ap_ngran_ng_eNB_CU || (nOdE_TyPe) == e2ap_ngran_gNB_CU)
#define E2AP_NODE_IS_DU(nOdE_TyPe)         ((nOdE_TyPe) == e2ap_ngran_eNB_DU || (nOdE_TyPe) == e2ap_ngran_gNB_DU)
#define E2AP_NODE_IS_MBMS(nOdE_TyPe)       ((nOdE_TyPe) == e2ap_ngran_eNB_MBMS_STA)

char* get_e2ap_ngran_name(e2ap_ngran_node_t ran_type);

#endif
