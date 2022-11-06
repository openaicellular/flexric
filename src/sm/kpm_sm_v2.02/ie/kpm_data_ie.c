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

/* 
 * This file contains free/copy/equals methods implementation for the data structures defined in the associated header file 
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../../util/alg_ds/alg/eq_float.h"
#include "../../../util/byte_array.h"
#include "kpm_data_ie.h"

/* ACTION DEFINITION*/
void free_kpm_action_def(kpm_action_def_t* src) 
{
  assert(src != NULL);

  for (size_t i = 0; i<src->MeasInfo_len; i++)
  {
    for (size_t j = 0; j<src->MeasInfo[i].labelInfo_len; j++)
      free_label_info(&src->MeasInfo[i].labelInfo[j]);
    free(src->MeasInfo[i].labelInfo);
        
    if (src->MeasInfo[i].meas_type == KPM_V2_MEASUREMENT_TYPE_NAME)
      free_byte_array(src->MeasInfo[i].measName);
  }
  
  free(src->MeasInfo);
}

/* INDICATION*/
void free_kpm_ind_hdr(kpm_ind_hdr_t* src)
{
  assert(src != NULL);
  if (src->fileFormatversion != NULL) {
    free_byte_array(*src->fileFormatversion);
    free(src->fileFormatversion);
  }
  if (src->senderName != NULL) {
    free_byte_array(*src->senderName);
    free(src->senderName);
  }
  if (src->senderType != NULL) {
    free_byte_array(*src->senderType);
    free(src->senderType);
  }
  if (src->vendorName != NULL) {
    free_byte_array(*src->vendorName);
    free(src->vendorName);
  }
}

void free_kpm_ind_msg(kpm_ind_msg_t* src) 
{
  assert(src != NULL);
  
  size_t i;

  for (i=0; i<src->MeasData_len; i++)
    free(src->MeasData[i].measRecord);
  free(src->MeasData);
  
  /*OPTIONAL fields below*/
  if (src->MeasInfo != NULL){
    for (i=0; i<src->MeasInfo_len; i++)
    {
      if (src->MeasInfo[i].meas_type ==  KPM_V2_MEASUREMENT_TYPE_NAME)
        free_byte_array(src->MeasInfo[i].measName);
      for (size_t j = 0; j< src->MeasInfo[i].labelInfo_len; j++)
        free_label_info(&src->MeasInfo[i].labelInfo[j]);
      free(src->MeasInfo[i].labelInfo);
    }
    free(src->MeasInfo);
  }

  if (src->granulPeriod != NULL)
    free(src->granulPeriod);
}

static byte_array_t* dup_byte_array(const byte_array_t* src)
{
  if (src == NULL)
    return NULL;

  byte_array_t* dst = malloc(sizeof(*dst));
  assert(dst != NULL && "memory exhausted");
  dst->len = src->len;
  dst->buf = malloc(dst->len * sizeof(uint8_t));
  memcpy(dst->buf, src->buf, src->len);
  return dst;
}

kpm_ind_hdr_t cp_kpm_ind_hdr(kpm_ind_hdr_t const* src)
{
  assert(src != NULL);
  kpm_ind_hdr_t ret = {0};

  ret.collectStartTime = src->collectStartTime;
  
  ret.fileFormatversion = dup_byte_array(src->fileFormatversion);
  ret.senderName = dup_byte_array(src->senderName);
  ret.senderType = dup_byte_array(src->senderType);
  ret.vendorName = dup_byte_array(src->vendorName);
  return ret;
}

kpm_ind_msg_t cp_kpm_ind_msg(kpm_ind_msg_t const* src) {

  assert(src != NULL);
  kpm_ind_msg_t ret = {0};

  if (src->granulPeriod) {
    ret.granulPeriod = malloc (sizeof(ret.granulPeriod));
    *(ret.granulPeriod) = *(src->granulPeriod); 
  }

  if (src->MeasData_len) {
    ret.MeasData_len = src->MeasData_len;
    ret.MeasData = calloc(src->MeasData_len, sizeof(adapter_MeasDataItem_t));
    memcpy (ret.MeasData, src->MeasData, src->MeasData_len * sizeof(adapter_MeasDataItem_t));
    for (size_t i = 0; i<ret.MeasData_len; i++){
      ret.MeasData[i].measRecord = calloc(src->MeasData[i].measRecord_len, sizeof(adapter_MeasRecord_t));
      memcpy (ret.MeasData[i].measRecord, src->MeasData[i].measRecord, src->MeasData[i].measRecord_len * sizeof(adapter_MeasDataItem_t));
    }
  }

  if (src->MeasInfo_len) {
    ret.MeasInfo_len = src->MeasInfo_len;
    ret.MeasInfo = calloc(src->MeasInfo_len, sizeof(MeasInfo_t));
    memcpy (ret.MeasInfo, src->MeasInfo, src->MeasInfo_len * sizeof(MeasInfo_t));
    for (size_t i = 0; i<ret.MeasInfo_len; i++)
    {
      ret.MeasInfo[i].labelInfo_len = src->MeasInfo[i].labelInfo_len;
      ret.MeasInfo[i].labelInfo = calloc(src->MeasInfo[i].labelInfo_len, sizeof(adapter_LabelInfoItem_t));
      memcpy (ret.MeasInfo[i].labelInfo, src->MeasInfo[i].labelInfo, src->MeasInfo[i].labelInfo_len * sizeof(adapter_LabelInfoItem_t));
      for (size_t j = 0; j < src->MeasInfo[i].labelInfo_len; j++)
      {
        ret.MeasInfo[i].meas_type = src->MeasInfo[i].meas_type;
        if (ret.MeasInfo[i].meas_type == KPM_V2_MEASUREMENT_TYPE_NAME)
          ret.MeasInfo[i].measName = copy_byte_array(src->MeasInfo[i].measName);
        else 
          ret.MeasInfo[i].measID = src->MeasInfo[i].measID;
        cp_label_info(&ret.MeasInfo[i].labelInfo[j], &src->MeasInfo[i].labelInfo[j]);
      }  
    }
  }
  
  return ret;
}

// Equality implemented as euristics for the moment. This function is used in test framework.
// XXX: to be extended 
bool eq_kpm_ind_msg(kpm_ind_msg_t const* m0, kpm_ind_msg_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  bool ret = true;
  if (m0->granulPeriod != NULL)
    ret &= (m0->granulPeriod ==  m1->granulPeriod);
  ret &= (m0->MeasData_len == m1->MeasData_len);
  ret &= (m0->MeasInfo_len == m1->MeasInfo_len);
  for (size_t i =0; i< m0->MeasData_len ; i++)
    ret &= (m0->MeasData[i].measRecord_len == m1->MeasData[i].measRecord_len);
  return ret;
}


void free_kpm_ind_data(kpm_ind_data_t* ind) 
{
  assert(ind != NULL);
  free_kpm_ind_hdr(&ind->hdr);
  free_kpm_ind_msg(&ind->msg);
}

kpm_ind_data_t cp_kpm_ind_data(kpm_ind_data_t const* src) 
{

  assert(src != NULL);
  kpm_ind_data_t ret = {.hdr = cp_kpm_ind_hdr(&src->hdr), .msg = cp_kpm_ind_msg(&src->msg)};
  return ret;
}

/* FUNCTION DEFINITION*/
void free_kpm_func_def(kpm_func_def_t* src)
{
  assert(src != NULL);
  
  free(src->ric_ReportStyle_List); 
  free(src->ric_EventTriggerStyle_List); 
  free(src->ranFunction_Name.ranFunction_Instance);
  free_byte_array(src->ranFunction_Name.Description);
  free_byte_array(src->ranFunction_Name.E2SM_OID);
  free_byte_array(src->ranFunction_Name.ShortName);
}

/* GENERICS: 
 * - adapter_LabelInfoItem_t 
 * ... more to be added in the future
 */

static long* dup_long(const long* src)
{
  if (src == NULL)
    return NULL;
  long* dst = malloc(sizeof(*dst));
  assert(dst != NULL && "memory exhausted");
  *dst = *src;
  return dst;
}

void cp_label_info(adapter_LabelInfoItem_t *dst, adapter_LabelInfoItem_t const *src) 
{
  assert(src != NULL);
  assert(dst != NULL);

  dst->noLabel = dup_long(src->noLabel);
  if (src->plmn_id != NULL) {
    dst->plmn_id = malloc(sizeof(*dst->plmn_id));
    *dst->plmn_id = *src->plmn_id;
  }

  // TO BE COMPLETED with the other fields
}

void free_label_info(adapter_LabelInfoItem_t *l) 
{
  assert(l != NULL);

  if (l->noLabel)
    free (l->noLabel);
  if (l->plmn_id)
    free(l->plmn_id);
 
  if (l->sliceID != NULL) {
    assert(false && "not implemented");
  }
	if (l->fiveQI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qFI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCImax != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCImin != NULL) {
    assert(false && "not implemented");
  }
	if (l->aRPmax != NULL) {
    assert(false && "not implemented");
  }
	if (l->aRPmin != NULL) {
    assert(false && "not implemented");
  }
	if (l->bitrateRange != NULL) {
    assert(false && "not implemented");
  }
	if (l->layerMU_MIMO != NULL) {
    assert(false && "not implemented");
  }
	if (l->sUM != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinX != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinY != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinZ != NULL) {
    assert(false && "not implemented");
  }
	if (l->preLabelOverride != NULL) {
    assert(false && "not implemented");
  }
	if (l->startEndInd != NULL) {
    assert(false && "not implemented");
  }
	if (l->min != NULL) {
    assert(false && "not implemented");
  }
	if (l->max != NULL) {
    assert(false && "not implemented");
  }
	if (l->avg != NULL) {
    assert(false && "not implemented");
  }
}
