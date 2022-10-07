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



#include "stdout.h"
#include "ric/iApps/../../sm/mac_sm/ie/mac_data_ie.h"    // for mac_ind_msg_t
#include "ric/iApps/../../sm/pdcp_sm/ie/pdcp_data_ie.h"  // for pdcp_ind_msg_t
#include "ric/iApps/../../sm/rlc_sm/ie/rlc_data_ie.h"    // for rlc_ind_msg_t
#include "ric/iApps/../../sm/rrc_sm/ie/rrc_data_ie.h"    // for rrc_ind_msg_t
#include "string_parser.h"                               // for to_string_ma..

#include "../../util/time_now_us.h"

#include <assert.h>                                      // for assert
#include <stdint.h>                                      // for uint32_t
#include <stdio.h>                                       // for NULL, fputs
#include <stdlib.h>                                      // for atexit
#include <time.h>

static
FILE *fp = NULL;

const char* file_path = "log.txt";


static
void close_fp(void)
{
  assert(fp != NULL);

  int const rc = fclose(fp);
  assert(rc == 0);
}

static
void init_fp(FILE** fp, const char* path)
{
  assert(*fp == NULL);
  assert(path != NULL);

//  const char* append = "a"; 
  const char* write = "w"; 

  *fp = fopen(path, write);
  assert(*fp != NULL);

  atexit(close_fp);
}


static
void print_mac_stats(mac_ind_msg_t const* msg )
{
  assert(msg != NULL);

  if(fp == NULL)
    init_fp(&fp, file_path);

  int64_t now = time_now_us();
  printf("Time diff at iApp = %ld \n", now - msg->tstamp);

  for(uint32_t i = 0; i < msg->len_ue_stats; ++i){
    char stats[1024] = {0};
    to_string_mac_ue_stats(&msg->ue_stats[i], msg->tstamp, stats, 1024);
    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }
}


static
void print_rlc_stats(rlc_ind_msg_t const* rlc)
{
  assert(rlc != NULL);

  if(fp == NULL)
    init_fp(&fp, file_path);

  for(uint32_t i = 0; i < rlc->len; ++i){
    char stats[1024] = {0};
    to_string_rlc_rb(&rlc->rb[i], rlc->tstamp , stats , 1024);

    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }
}

static
void print_pdcp_stats(pdcp_ind_msg_t const* pdcp)
{
  assert(pdcp != NULL);

  if(fp == NULL)
    init_fp(&fp, file_path);

  for(uint32_t i = 0; i < pdcp->len; ++i){

    char stats[512] = {0};
    pdcp_radio_bearer_stats_t* rb = &pdcp->rb[i];

    to_string_pdcp_rb(rb, pdcp->tstamp, stats, 512);
    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }
}

static
void print_slice_stats(slice_ind_msg_t const* slice)
{
  assert(slice != NULL);

  if(fp == NULL)
    init_fp(&fp, file_path);

  char stats[512] = {0};
  to_string_slice(slice, slice->tstamp, stats, 512); 

  int const rc = fputs(stats , fp);
  // Edit: The C99 standard §7.19.1.3 states:
  // The macros are [...]
  // EOF which expands to an integer constant expression, 
  // with type int and a negative value, that is returned by 
  // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
  assert(rc > -1);

}

static
void print_gtp_stats(gtp_ind_msg_t const* gtp)
{
  assert(gtp != NULL);
  if(fp == NULL)
  init_fp(&fp, file_path);

  for(uint32_t i = 0; i < gtp->len; ++i){
    char stats[1024] = {0};
    to_string_gtp_ngu(&gtp->ngut[i], gtp->tstamp , stats , 1024);

    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }

}

void print_kpm_stats(kpm_ind_data_t const* kpm)
{
  assert(kpm != NULL);
  char stats[1024] = {0};
  int max = 1024;

  if(fp == NULL)
    init_fp(&fp, file_path);

  for(size_t i = 0; i < kpm->msg.MeasData_len; i++){
    adapter_MeasDataItem_t* curMeasData = &kpm->msg.MeasData[i];

    if (i == 0 && kpm->msg.granulPeriod){
      int rc = snprintf(stats, max,  "kpm_stats: "
                      "tstamp=%u"
                      ",granulPeriod=%lu"
                      ",kpm_MeasData"
                      ",kpm->MeasData_len=%zu"
                      , *(kpm->hdr.collectStartTime.buf)
                      , *(kpm->msg.granulPeriod)
                      , kpm->msg.MeasData_len
                      );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }else if(i == 0 && kpm->msg.granulPeriod == NULL){
      int rc = snprintf(stats, max,  "kpm_stats: "
                      "tstamp=%u"
                      ",granulPeriod="
                      ",kpm_MeasData"
                      ",kpm->MeasData_len=%zu"
                      , *(kpm->hdr.collectStartTime.buf)
                      , kpm->msg.MeasData_len
                      );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }

    memset(stats, 0, sizeof(stats));
    int rc = snprintf(stats, max,
                      ",kpm_measData[%zu]"
                      ",MeasData->incompleteFlag=%ld"
                      ",MeasData->measRecord_len=%zu"
                      , i
                      , curMeasData->incompleteFlag
                      , curMeasData->measRecord_len
                      );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    rc = fputs(stats , fp);
    assert(rc > -1);

    for(size_t j = 0; j < curMeasData->measRecord_len; j++){
      adapter_MeasRecord_t* curMeasRecord = &(curMeasData->measRecord[j]);
      memset(stats, 0, sizeof(stats));
      to_string_kpm_measRecord(curMeasRecord, stats, max);
      int rc = fputs(stats , fp);
      assert(rc > -1);
    }
  }

  for(size_t i = 0; i < kpm->msg.MeasInfo_len; i++){
    MeasInfo_t* curMeasInfo = &kpm->msg.MeasInfo[i];
    if (i == 0){
      memset(stats, 0, sizeof(stats));
      int rc = snprintf(stats, max,
                      ",kpm_MeasInfo"
                      ",kpm->MeasInfo_len=%zu",
                      i
                      );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }

    if (curMeasInfo->measType == MeasurementType_ID){
      memset(stats, 0, sizeof(stats));
      int rc = snprintf(stats, max,
                        ",MeasInfo[%zu]"
                        ",measType=%d"
                        ",measID=%ld"
                        , i
                        , curMeasInfo->measType
                        , curMeasInfo->measID
                        );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    } else if (curMeasInfo->measType == MeasurementType_NAME){
      memset(stats, 0, sizeof(stats));
      int rc = snprintf(stats, max,
                        ",MeasInfo[%zu]"
                        ",measType=%d"
                        ",measName->len=%zu"
                        ",measName->buf=%u"
                        , i
                        , curMeasInfo->measType
                        , curMeasInfo->measName.len
                        , *curMeasInfo->measName.buf
                        );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }

    for(size_t j = 0; j < curMeasInfo->labelInfo_len; ++j){
      adapter_LabelInfoList_t* curLabelInfo = &curMeasInfo->labelInfo[j];
      memset(stats, 0, sizeof(stats));
      to_string_kpm_labelInfo(curLabelInfo, stats, max);
      int rc = fputs(stats , fp);
      assert(rc > -1);
    }
  }
}

void notify_stdout_listener(sm_ag_if_rd_t const* data)
{
  assert(data != NULL);
  if(data->type == MAC_STATS_V0)  
    print_mac_stats(&data->mac_stats.msg);
  else if (data->type == RLC_STATS_V0)
    print_rlc_stats(&data->rlc_stats.msg);
  else if (data->type == PDCP_STATS_V0)
    print_pdcp_stats(&data->pdcp_stats.msg);
  else if (data->type == SLICE_STATS_V0)
    print_slice_stats(&data->slice_stats.msg);
  else if (data->type == GTP_STATS_V0)
    print_gtp_stats(&data->gtp_stats.msg);
  else if (data->type == KPM_STATS_V0)
    print_kpm_stats(&data->kpm_stats);
  else
    assert(0!= 0);
}

