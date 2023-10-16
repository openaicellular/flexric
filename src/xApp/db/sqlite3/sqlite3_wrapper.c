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

#include "sqlite3_wrapper.h"
#include "../../../util/time_now_us.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static
void create_table(sqlite3* db, char* sql)
{
  char* err_msg = NULL;
  int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  assert(rc == SQLITE_OK && "Error while creating the DB. Check the err_msg string for further info" );
}

static
void create_mac_ue_table(sqlite3* db)
{
  assert(db != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  char* sql_mac = "DROP TABLE IF EXISTS MAC_UE;"
  "CREATE TABLE MAC_UE(tstamp INT CHECK(tstamp > 0)," 
                       "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                       "e2node_mcc INT,"
                       "e2node_mnc INT,"
                       "e2node_mnc_digit_len INT,"
                       "e2node_nb_id INT,"
                       "e2node_cu_du_id TEXT,"
                       "frame INT,"
                       "slot INT,"
                       "dl_aggr_tbs INT CHECK(dl_aggr_tbs  >= 0 AND dl_aggr_tbs < 18446744073709551615),"
                       "ul_aggr_tbs INT CHECK(ul_aggr_tbs  >= 0 AND ul_aggr_tbs < 18446744073709551615),"
                       "dl_aggr_bytes_sdus  INT CHECK(dl_aggr_bytes_sdus  >= 0 AND dl_aggr_bytes_sdus < 18446744073709551615),"
                       "ul_aggr_bytes_sdus INT CHECK(ul_aggr_bytes_sdus >= 0 AND ul_aggr_bytes_sdus < 18446744073709551615),"
                       "dl_curr_tbs INT CHECK(dl_curr_tbs >= 0 AND dl_curr_tbs < 18446744073709551615),"
                       "ul_curr_tbs INT CHECK(ul_curr_tbs >= 0 AND ul_curr_tbs < 18446744073709551615),"
                       "dl_sched_rb INT CHECK(dl_sched_rb >= 0 AND dl_sched_rb < 18446744073709551615),"
                       "ul_sched_rb INT CHECK(ul_sched_rb >= 0 AND ul_sched_rb < 18446744073709551615),"
                       "pusch_snr REAL CHECK(pusch_snr == NULL OR (pusch_snr >= 0 AND pusch_snr < 4294967296)),"
                       "pucch_snr REAL CHECK(pucch_snr == NULL OR (pucch_snr >= 0 AND pucch_snr < 4294967296)) ,"
                       "rnti INT  CHECK(rnti  >= 0 AND rnti < 4294967296 ),"
                       "dl_aggr_prb INT  CHECK(dl_aggr_prb >= 0 AND dl_aggr_prb < 4294967296 ),"
                       "ul_aggr_prb INT  CHECK(ul_aggr_prb >= 0 AND ul_aggr_prb < 4294967296 ),"
                       "dl_aggr_sdus INT  CHECK(dl_aggr_sdus >= 0 AND dl_aggr_sdus < 4294967296 ),"
                       "ul_aggr_sdus INT  CHECK(ul_aggr_sdus >= 0 AND ul_aggr_sdus < 4294967296 ),"
                       "dl_aggr_retx_prb  INT  CHECK(dl_aggr_retx_prb >= 0 AND dl_aggr_retx_prb < 4294967296 ),"
                       "ul_aggr_retx_prb  INT  CHECK(ul_aggr_retx_prb >= 0 AND ul_aggr_retx_prb < 4294967296 ),"
                       "wb_cqi INT  CHECK(wb_cqi >= 0 AND wb_cqi < 256 ),"
                       "dl_mcs1 INT  CHECK(dl_mcs1>= 0 AND dl_mcs1 < 256),"
                       "ul_mcs1  INT CHECK(ul_mcs1 >= 0 AND ul_mcs1 < 256),"
                       "dl_mcs2  INT CHECK(dl_mcs2 >= 0 AND dl_mcs2 < 256),"
                       "ul_mcs2 INT CHECK(ul_mcs2 >= 0 AND ul_mcs2 < 256),"
                       "phr INT CHECK(phr > -24 AND  phr < 41)," // −23 dB to +40 dB
                       "bsr INT CHECK(bsr >= 0 AND  bsr < 4294967296),"
                       "dl_bler REAL CHECK(dl_bler  >= 0 AND dl_bler < 4294967296),"
                       "ul_bler REAL CHECK(ul_bler  >= 0 AND ul_bler < 4294967296),"
                       "dl_num_harq INT CHECK(dl_num_harq >= 0 AND  dl_num_harq < 5),"
                       "dl_harq_round0 INT CHECK(dl_harq_round0 >= 0 AND  dl_harq_round0 < 4294967296),"
                       "dl_harq_round1 INT CHECK(dl_harq_round1 >= 0 AND  dl_harq_round1 < 4294967296),"
                       "dl_harq_round2 INT CHECK(dl_harq_round2 >= 0 AND  dl_harq_round2 < 4294967296),"
                       "dl_harq_round3 INT CHECK(dl_harq_round3 >= 0 AND  dl_harq_round3 < 4294967296),"
                       "dlsch_errors INT CHECK(dlsch_errors >= 0 AND  dlsch_errors < 4294967296),"
                       "ul_num_harq INT CHECK(ul_num_harq >= 0 AND  ul_num_harq < 5),"
                       "ul_harq_round0 INT CHECK(ul_harq_round0 >= 0 AND  ul_harq_round0 < 4294967296),"
                       "ul_harq_round1 INT CHECK(ul_harq_round1 >= 0 AND  ul_harq_round1 < 4294967296),"
                       "ul_harq_round2 INT CHECK(ul_harq_round2 >= 0 AND  ul_harq_round2 < 4294967296),"
                       "ul_harq_round3 INT CHECK(ul_harq_round3 >= 0 AND  ul_harq_round3 < 4294967296),"
                       "ulsch_errors INT CHECK(ulsch_errors >= 0 AND  ulsch_errors < 4294967296)"
                       ");";

  create_table(db, sql_mac);
}

static
void create_rlc_bearer_table(sqlite3* db)
{
  assert(db != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  char* sql_rlc = "DROP TABLE IF EXISTS RLC_bearer;"
  "CREATE TABLE RLC_bearer(tstamp INT CHECK(tstamp > 0)," 
                            "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                            "e2node_mcc INT,"
                            "e2node_mnc INT,"
                            "e2node_mnc_digit_len INT,"
                            "e2node_nb_id INT,"
                            "e2node_cu_du_id TEXT,"
                            "txpdu_pkts INT CHECK(txpdu_pkts >= 0 AND txpdu_pkts < 4294967296)," // 1 << 32 = 4294967296 
                            "txpdu_bytes INT CHECK(txpdu_bytes >= 0 AND  txpdu_bytes < 4294967296 ),"\
                            "txpdu_wt_ms  INT CHECK(txpdu_wt_ms  >= 0 AND  txpdu_wt_ms  < 4294967296 ),"\
                            "txpdu_dd_pkts INT CHECK(txpdu_dd_pkts >= 0 AND  txpdu_dd_pkts < 4294967296 ) ,"\
                            "txpdu_dd_bytes INT  CHECK(txpdu_dd_bytes >= 0 AND  txpdu_dd_bytes < 4294967296 ),"\
                            "txpdu_retx_pkts INT CHECK(txpdu_retx_pkts >= 0 AND  txpdu_retx_pkts < 4294967296 ) ,"\
                            "txpdu_retx_bytes INT  CHECK(txpdu_retx_bytes >= 0 AND  txpdu_retx_bytes < 4294967296 ),"\
                            "txpdu_segmented INT  CHECK(txpdu_segmented >= 0 AND  txpdu_segmented < 4294967296 ),"\
                            "txpdu_status_pkts INT  CHECK(txpdu_status_pkts >= 0 AND  txpdu_status_pkts < 4294967296 ),"\
                            "txpdu_status_bytes INT  CHECK(txpdu_status_bytes >= 0 AND  txpdu_status_bytes < 4294967296 ),"\
                            "txbuf_occ_bytes INT  CHECK(txbuf_occ_bytes >= 0 AND  txbuf_occ_bytes < 4294967296 ),"\
                            "txbuf_occ_pkts INT  CHECK(txbuf_occ_pkts >= 0 AND  txbuf_occ_pkts < 4294967296 ),"\
                            "rxpdu_pkts INT  CHECK(rxpdu_pkts >= 0 AND  rxpdu_pkts < 4294967296 ),"\
                            "rxpdu_bytes INT  CHECK(rxpdu_bytes >= 0 AND  rxpdu_bytes < 4294967296 ),"\
                            "rxpdu_dup_pkts INT CHECK(rxpdu_dup_pkts >= 0 AND  rxpdu_dup_pkts < 4294967296 ),"\
                            "rxpdu_dup_bytes INT CHECK(rxpdu_dup_bytes >= 0 AND  rxpdu_dup_bytes < 4294967296 ),"\
                            "rxpdu_dd_pkts INT CHECK(rxpdu_dd_pkts >= 0 AND  rxpdu_dd_pkts < 4294967296 ),"\
                            "rxpdu_dd_bytes INT CHECK(rxpdu_dd_bytes >= 0 AND  rxpdu_dd_bytes < 4294967296 ),"\
                            "rxpdu_ow_pkts INT CHECK(rxpdu_ow_pkts >= 0 AND  rxpdu_ow_pkts < 4294967296 ),"\
                            "rxpdu_ow_bytes INT CHECK(rxpdu_ow_bytes >= 0 AND  rxpdu_ow_bytes < 4294967296 ),"\
                            "rxpdu_status_pkts INT CHECK(rxpdu_status_pkts >= 0 AND  rxpdu_status_pkts < 4294967296 ),"\
                            "rxpdu_status_bytes INT CHECK(rxpdu_status_bytes >= 0 AND  rxpdu_status_bytes < 4294967296 ),"\
                            "rxbuf_occ_bytes INT CHECK(rxbuf_occ_bytes >= 0 AND  rxbuf_occ_bytes < 4294967296 ),"\
                            "rxbuf_occ_pkts INT CHECK(rxbuf_occ_pkts >= 0 AND  rxbuf_occ_pkts < 4294967296 ),"\
                            "txsdu_pkts INT CHECK(txsdu_pkts >= 0 AND  txsdu_pkts < 4294967296 ),"\
                            "txsdu_bytes INT CHECK(txsdu_bytes >= 0 AND  txsdu_bytes < 4294967296 ),"\
                            "txsdu_avg_time_to_tx REAL CHECK(txsdu_avg_time_to_tx >= 0 AND  txsdu_avg_time_to_tx < 4294967296 ),"\
                            "txsdu_wt_us INT CHECK(txsdu_wt_us >= 0 AND  txsdu_wt_us < 4294967296 ),"\
                            "rxsdu_pkts INT CHECK(rxsdu_pkts >= 0 AND  rxsdu_pkts < 4294967296 ),"\
                            "rxsdu_bytes INT CHECK(rxsdu_bytes >= 0 AND  rxsdu_bytes < 4294967296 ),"\
                            "rxsdu_dd_pkts INT CHECK(rxsdu_dd_pkts >= 0 AND  rxsdu_dd_pkts < 4294967296 ),"\
                            "rxsdu_dd_bytes INT CHECK(rxsdu_dd_bytes >= 0 AND  rxsdu_dd_bytes < 4294967296 ),"\
                            "rnti INT CHECK(rnti >= 0 AND  rnti < 4294967296 ),"\
                            "mode INT CHECK(mode >= 0 AND  mode < 3),"\
                            "rbid INT CHECK(rbid >= 0 AND  rbid < 16)"
                            ");";

  create_table(db, sql_rlc);
}

static
void create_pdcp_bearer_table(sqlite3* db)
{
  assert(db != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  char* sql_pdcp = "DROP TABLE IF EXISTS PDCP_bearer;"
  "CREATE TABLE PDCP_bearer(tstamp INT CHECK(tstamp > 0)," 
                       "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                       "e2node_mcc INT,"
                       "e2node_mnc INT,"
                       "e2node_mnc_digit_len INT,"
                       "e2node_nb_id INT,"
                       "e2node_cu_du_id TEXT,"
                       "txpdu_pkts INT CHECK(txpdu_pkts  >= 0 AND txpdu_pkts  < 4294967296),"
                       "txpdu_bytes INT CHECK(txpdu_bytes >=0 AND txpdu_bytes < 4294967296 ),"
                       "txpdu_sn INT CHECK(txpdu_sn >=0 AND txpdu_sn < 4294967296 ),"
                       "rxpdu_pkts INT CHECK(rxpdu_pkts >=0 AND rxpdu_pkts < 4294967296 ),"
                       "rxpdu_bytes INT CHECK(rxpdu_bytes >=0 AND rxpdu_bytes < 4294967296 ),"
                       "rxpdu_sn  INT CHECK(rxpdu_sn  >= 0 AND rxpdu_sn  < 4294967296 ) ,"
                       "rxpdu_oo_pkts INT  CHECK(rxpdu_oo_pkts >= 0 AND rxpdu_oo_pkts < 4294967296 ),"
                       "rxpdu_oo_bytes INT  CHECK(rxpdu_oo_bytes >= 0 AND rxpdu_oo_bytes < 4294967296 ),"
                       "rxpdu_dd_pkts INT  CHECK(rxpdu_dd_pkts >= 0 AND rxpdu_dd_pkts < 4294967296 ),"
                       "rxpdu_dd_bytes INT  CHECK(rxpdu_dd_bytes >= 0 AND rxpdu_dd_bytes < 4294967296 ),"
                       "rxpdu_ro_count INT  CHECK(rxpdu_ro_count >= 0 AND rxpdu_ro_count < 4294967296 ),"
                       "txsdu_pkts INT  CHECK(txsdu_pkts >= 0 AND txsdu_pkts < 4294967296 ),"
                       "txsdu_bytes INT  CHECK(txsdu_bytes >= 0 AND txsdu_bytes <4294967296 ),"
                       "rxsdu_pkts INT  CHECK(rxsdu_pkts >= 0 AND rxsdu_pkts <4294967296),"
                       "rxsdu_bytes INT CHECK(rxsdu_bytes >= 0 AND rxsdu_bytes <4294967296),"
                       "rnti INT CHECK(rnti >= 0 AND rnti <4294967296),"
                       "mode INT CHECK(mode >= 0 AND mode < 4294967296),"
                       "rbid INT CHECK(rbid >= 0 AND rbid < 4294967296)" // −23 dB to +40 dB 
                       ");";
  create_table(db, sql_pdcp);
}

static
void create_slice_table(sqlite3* db)
{
  assert(db != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  char* sql_slice = "DROP TABLE IF EXISTS SLICE;"
                    "CREATE TABLE SLICE("\
                    "tstamp INT CHECK(tstamp > 0),"\
                    "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"\
                    "e2node_mcc INT,"\
                    "e2node_mnc INT,"\
                    "e2node_mnc_digit_len INT,"\
                    "e2node_nb_id INT,"\
                    "e2node_cu_du_id TEXT,"\
                    "len_slices INT CHECK(len_slices  >= 0 AND len_slices < 4),"\
                    "sched_name TEXT,"\
                    "id INT CHECK(id >=0 AND id < 4294967296),"\
                    "label TEXT,"\
                    "type TEXT,"\
                    "type_conf TEXT,"\
                    "sched TEXT,"\
                    "type_param0 REAL CHECK(type_param0 == NULL OR (type_param0 >= 0 AND type_param0 < 4294967296)),"\
                    "type_param1 REAL CHECK(type_param1 == NULL OR (type_param1 >= 0 AND type_param1 < 4294967296)),"\
                    "type_param2 REAL CHECK(type_param2 == NULL OR (type_param2 >= 0 AND type_param2 < 4294967296))"
                    ");";
  create_table(db, sql_slice);
}

static
void create_ue_slice_table(sqlite3* db)
{
  assert(db != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  char* sql_ue_slice = "DROP TABLE IF EXISTS UE_SLICE;"
                    "CREATE TABLE UE_SLICE("\
                    "tstamp INT CHECK(tstamp > 0),"\
                    "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"\
                    "e2node_mcc INT,"\
                    "e2node_mnc INT,"\
                    "e2node_mnc_digit_len INT,"\
                    "e2node_nb_id INT,"\
                    "e2node_cu_du_id TEXT,"\
                    "len_ue_slice INT CHECK(len_ue_slice  >= 0 AND len_ue_slice  < 4294967296),"\
                    "rnti INT CHECK(rnti == -1 OR (rnti >= 0 AND rnti < 65535)),"\
                    "dl_id INT CHECK(dl_id == -1 OR (dl_id >= 0 AND dl_id < 4294967296))"
                    ");";
  create_table(db, sql_ue_slice);
}

static
void create_gtp_table(sqlite3* db)
{
  assert(db != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  char* sql_gtp = "DROP TABLE IF EXISTS GTP_NGUT;"
  "CREATE TABLE GTP_NGUT(tstamp INT CHECK(tstamp > 0)," 
                            "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                            "e2node_mcc INT,"
                            "e2node_mnc INT,"
                            "e2node_mnc_digit_len INT,"
                            "e2node_nb_id INT,"
                            "e2node_cu_du_id TEXT,"
                            "teidgnb INT ," // 1 << 32 = 4294967296 
                            "rnti INT ,"
                            "qfi  INT ,"
                            "teidupf INT "
                            ");";

  create_table(db, sql_gtp);
}
void create_kpm_table(sqlite3* db) {
  assert(db != NULL);

  // kpm_ric_ind_hdr_format_1_t
  char* sql_kpm_hdr = "DROP TABLE IF EXISTS KPM_HDR;"
                      "CREATE TABLE KPM_HDR(tstamp INT,"
                      "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                      "e2node_mcc INT,"
                      "e2node_mnc INT,"
                      "e2node_mnc_digit_len INT,"
                      "e2node_nb_id INT,"
                      "e2node_cu_du_id TEXT,"
                      "format INT,"
                      "collectStartTime INT,"
                      "fileformat_version TEXT,"
                      "sender_name TEXT,"
                      "sender_type TEXT,"
                      "vendor_name TEXT"
                      ");";
  create_table(db, sql_kpm_hdr);

  // kpm_ind_msg_format_1_t: meas_data_lst_t + gran_period_ms
  // kpm_ind_msg_format_3_t
  char *sql_kpm_meas_data = "DROP TABLE IF EXISTS KPM_IND_MEAS_DATA;"
                            "CREATE TABLE KPM_IND_MEAS_DATA(tstamp INT,"
                            "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                            "e2node_mcc INT,"
                            "e2node_mnc INT,"
                            "e2node_mnc_digit_len INT,"
                            "e2node_nb_id INT,"
                            "e2node_cu_du_id TEXT,"
                            "ue_meas_report_idx INT,"
                            "ric_ind_format INT,"
                            "meas_data_len INT,"
                            "meas_data_idx INT,"
                            "meas_record_len INT,"
                            "meas_record_idx INT,"
                            "incompleteFlag TEXT,"
                            "gran_period_ms INT,"
                            "meas_value_type TEXT,"
                            "meas_value_int INT,"
                            "meas_value_real REAL,"
                            "meas_value_no TEXT"
                            ");";
  create_table(db, sql_kpm_meas_data);

  // kpm_ind_msg_format_1_t: meas_info_format_1_lst_t
  // kpm_ind_msg_format_3_t
  char *sql_kpm_meas_info_frm1 = "DROP TABLE IF EXISTS KPM_IND_MEAS_INFO;"
                                 "CREATE TABLE KPM_IND_MEAS_INFO(tstamp INT,"
                                 "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                                 "e2node_mcc INT,"
                                 "e2node_mnc INT,"
                                 "e2node_mnc_digit_len INT,"
                                 "e2node_nb_id INT,"
                                 "e2node_cu_du_id TEXT,"
                                 "ue_meas_report_idx INT,"
                                 "ric_ind_format INT,"
                                 "meas_info_len INT,"
                                 "meas_info_idx INT,"
                                 "meas_type TEXT,"
                                 "meas_name TEXT,"
                                 "meas_id INT,"
                                 "label_info_len INT,"
                                 "label_info_idx INT,"
                                 "noLabel BOOLEAN CHECK(noLabel IN (0,1)),"
                                 "plmn_id_mcc INT CHECK(plmn_id_mcc >= 0 AND plmn_id_mcc < 255),"
                                 "plmn_id_mnc INT CHECK(plmn_id_mnc >= 0 AND plmn_id_mnc < 255),"
                                 "plmn_id_mnc_digit_len INT CHECK(plmn_id_mnc_digit_len >= 0 AND plmn_id_mnc_digit_len < 255),"
                                 "sliceID_sST INT CHECK(sliceID_sST >= 0 AND sliceID_sST < 255),"
                                 "sliceID_sD TEXT,"
                                 "fiveQI INT CHECK(fiveQI >= 0 AND fiveQI < 255),"
                                 "qFI INT CHECK(qFI >= 0 AND qFI < 255),"
                                 "qCI INT CHECK(qCI >= 0 AND qCI < 255),"
                                 "qCImax INT CHECK(qCImax >= 0 AND qCImax < 255),"
                                 "qCImin INT CHECK(qCImin >= 0 AND qCImin < 255),"
                                 "aRPmax INT CHECK(aRPmax >= 0 AND aRPmax < 255),"
                                 "aRPmin INT CHECK(aRPmin >= 0 AND aRPmin < 255),"
                                 "bitrateRange INT CHECK(bitrateRange >= 0 AND bitrateRange < 32767),"
                                 "layerMU_MIMO INT CHECK(layerMU_MIMO >= 0 AND layerMU_MIMO < 32767),"
                                 "sUM BOOLEAN CHECK(sUM IN (0,1)),"
                                 "distBinX INT CHECK(distBinX >= 0 AND distBinX < 255),"
                                 "distBinY INT CHECK(distBinY >= 0 AND distBinY < 255),"
                                 "distBinZ INT CHECK(distBinZ >= 0 AND distBinZ < 255),"
                                 "preLabelOverride BOOLEAN CHECK(preLabelOverride IN (0,1)),"
                                 "startEndInd INT,"
                                 "min BOOLEAN CHECK(min IN (0,1)),"
                                 "max BOOLEAN CHECK(max IN (0,1)),"
                                 "avg BOOLEAN CHECK(avg IN (0,1)),"
                                 "ssbIndex INT CHECK(ssbIndex >= 0 AND ssbIndex < 32767),"
                                 "nonGoB_beamformModeIndex INT CHECK(nonGoB_beamformModeIndex >= 0 AND nonGoB_beamformModeIndex < 32767),"
                                 "mimoModeIndex INT CHECK(mimoModeIndex >= 0 AND mimoModeIndex < 255)"
                                 ");";
  create_table(db, sql_kpm_meas_info_frm1);

  // kpm_ind_msg_format_1_t: meas_data_lst_t + meas_type_t
  // kpm_ind_msg_format_3_t
  char *sql_kpm_meas_data_info = "DROP TABLE IF EXISTS KPM_IND_MEAS_DATA_INFO;"
                                 "CREATE TABLE KPM_IND_MEAS_DATA_INFO(tstamp INT,"
                                 "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                                 "e2node_mcc INT,"
                                 "e2node_mnc INT,"
                                 "e2node_mnc_digit_len INT,"
                                 "e2node_nb_id INT,"
                                 "e2node_cu_du_id TEXT,"
                                 "ue_meas_report_idx INT,"
                                 "ric_ind_format INT,"
                                 "meas_data_len INT,"
                                 "meas_data_idx INT,"
                                 "meas_record_len INT,"
                                 "meas_record_idx INT,"
                                 "incompleteFlag INT,"
                                 "meas_info_len INT,"
                                 "meas_info_idx INT,"
                                 "gran_period_ms INT,"
                                 "meas_type TEXT," // To map the value to the measurement name or id
                                 "meas_name TEXT,"
                                 "meas_id INT,"
                                 "meas_value_type TEXT,"
                                 "meas_value_int INT,"
                                 "meas_value_real REAL,"
                                 "meas_value_no TEXT"
                                 ");";
  create_table(db, sql_kpm_meas_data_info);

  // kpm_ind_msg_format_2_t: meas_info_cond_ue_lst_t
  // TODO

  // kpm_ind_msg_format_3_t: ue_id_e2sm_t
  char *sql_kpm_ue_id_e2sm = "DROP TABLE IF EXISTS KPM_IND_UE_ID_E2SM;"
                             "CREATE TABLE KPM_IND_UE_ID_E2SM(tstamp INT,"
                             "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                             "e2node_mcc INT,"
                             "e2node_mnc INT,"
                             "e2node_mnc_digit_len INT,"
                             "e2node_nb_id INT,"
                             "e2node_cu_du_id TEXT,"
                             "ric_ind_format INT,"
                             "ue_id_e2sm_type TEXT,"
                             "guami_plmn_id_mcc INT,"
                             "guami_plmn_id_mnc INT,"
                             "guami_plmn_id_mnc_digit_len INT,"
                             /// gnb.h ///
                             "amf_ue_ngap_id INT,"
                             "guami_amf_region_id INT,"
                             "guami_amf_set_id INT,"
                             "guami_amf_ptr INT,"
                             "gnb_cu_ue_f1ap_lst_len INT,"
                             "gnb_cu_ue_f1ap_lst INT,"
                             "gnb_cu_cp_ue_e1ap_lst_len INT,"
                             "gnb_cu_cp_ue_e1ap_lst INT,"
                             "ran_ue_id INT,"
                             "ng_ran_node_ue_xnap_id INT,"
                             /// enb.h ///
                             "mme_ue_s1ap_id INT,"
                             "guami_mme_group_id INT,"
                             "guami_mme_code INT,"
                             "enb_ue_x2ap_id INT,"
                             "enb_ue_x2ap_id_extension INT"
                             ");";
  create_table(db, sql_kpm_ue_id_e2sm);
}

static
void insert_db(sqlite3* db, char const* sql)
{
  assert(db != NULL);
  assert(sql != NULL);

  char* err_msg = NULL;
  int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  if (rc != SQLITE_OK){
    printf("Error while inserting into the DB: %s\n", err_msg);
    assert(1!=1);
  }
}


static
int to_sql_string_mac_ue(global_e2_node_id_t const* id, mac_ue_stats_impl_t* stats, int64_t tstamp, char* out, size_t out_len)
{
  assert(stats != NULL);       
  assert(out != NULL);
  const size_t max = 1024;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  int rc = snprintf(out, max, 
      "INSERT INTO MAC_UE VALUES("
      "%ld,"//tstamp
      "%d," //ngran_node  
      "%d," //mcc
      "%d," //mnc
      "%d," //mnc_digit_len   
      "%d," //nb_id 
      "'%s'," //cu_du_id
      "%d," //frame
      "%d," //slot
      "%lu,"//dl_aggr_tbs     
      "%lu,"//ul_aggr_tbs     
      "%lu," //dl_aggr_bytes_sdus   
      "%lu,"//ul_aggr_bytes_sdus       
      "%lu," //dl_curr_tbs
      "%lu," //ul_curr_tbs
      "%lu," //dl_sched_rb
      "%lu," //ul_sched_rb
      "%g,"// pusch_snr        
      "%g,"//  pucch_snr      
      "%u,"// rnti            
      "%u,"//dl_aggr_prb      
      "%u,"//  ul_aggr_prb    
      "%u,"//  dl_aggr_sdus   
      "%u,"//  ul_aggr_sdus   
      "%u,"// dl_aggr_retx_prb
      "%u,"// ul_aggr_retx_prb
      "%u,"// wb_cqi
      "%u,"// dl_mcs1
      "%u,"// ul_mcs1
      "%u,"// dl_mcs2         
      "%u,"// ul_mcs2         
      "%d,"// phr
      "%u,"// bsr
      "%f," // dl_bler
      "%f," // ul_bler
      "%d," // dl_num_harq
      "%u," // dl_harq[0]
      "%u," // dl_harq[1]
      "%u," // dl_harq[2]
      "%u," // dl_harq[3]
      "%u," // dlsch_errors
      "%d," // ul_num_harq
      "%u," // ul_harq[0]
      "%u," // ul_harq[1]
      "%u," // ul_harq[2]
      "%u," // ul_harq[3]
      "%u" // ulsch_errors
      ");"
      ,tstamp                
      ,id->type
      ,id->plmn.mcc
      ,id->plmn.mnc
      ,id->plmn.mnc_digit_len
      ,id->nb_id.nb_id 
      ,id->cu_du_id ? c_cu_du_id : c_null
      ,stats->frame
      ,stats->slot
      ,stats->dl_aggr_tbs    
      ,stats->ul_aggr_tbs    
      ,stats->dl_aggr_bytes_sdus      
      ,stats->ul_aggr_bytes_sdus      
      ,stats->dl_curr_tbs
      ,stats->ul_curr_tbs
      ,stats->dl_sched_rb
      ,stats->ul_sched_rb
      ,stats->pusch_snr      
      ,stats->pucch_snr      
      ,stats->rnti 
      ,stats->dl_aggr_prb    
      ,stats->ul_aggr_prb    
      ,stats->dl_aggr_sdus   
      ,stats->ul_aggr_sdus   
      ,stats->dl_aggr_retx_prb        
      ,stats->ul_aggr_retx_prb
      ,stats->wb_cqi         
      ,stats->dl_mcs1        
      ,stats->ul_mcs1        
      ,stats->dl_mcs2        
      ,stats->ul_mcs2        
      ,stats->phr            
      ,stats->bsr
      ,stats->dl_bler
      ,stats->ul_bler
      ,stats->dl_num_harq
      ,stats->dl_harq[0]
      ,stats->dl_harq[1]
      ,stats->dl_harq[2]
      ,stats->dl_harq[3]
      ,stats->dl_harq[4]
      ,stats->ul_num_harq
      ,stats->ul_harq[0]
      ,stats->ul_harq[1]
      ,stats->ul_harq[2]
      ,stats->ul_harq[3]
      ,stats->ul_harq[4]
      );
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_sql_string_rlc_rb(global_e2_node_id_t const* id,rlc_radio_bearer_stats_t* rlc, int64_t tstamp, char* out, size_t out_len)
{
  assert(rlc != NULL);
  assert(out != NULL);
  const size_t max = 1024;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  int const rc = snprintf(out, max,
        "INSERT INTO RLC_bearer VALUES("
        "%ld,"// tstamp
        "%d," //ngran_node  
        "%d," //mcc
        "%d," //mnc
        "%d," //mnc_digit_len   
        "%d," //nb_id 
        "'%s'," //cu_du_id
        "%u," //rlc->txpdu_pkts
        "%u," //rlc->txpdu_bytes
        "%u," //rlc->txpdu_wt_ms 
        "%u," //rlc->txpdu_dd_pkts
        "%u," //rlc->txpdu_dd_bytes 
        "%u," //rlc->txpdu_retx_pkts
        "%u," //rlc->txpdu_retx_bytes
        "%u," //rlc->txpdu_segmented
        "%u," //rlc->txpdu_status_pkts
        "%u," //rlc->txpdu_status_bytes
        "%u," //rlc->txbuf_occ_bytes
        "%u," //rlc->txbuf_occ_pkts
        "%u," //rlc->rxpdu_pkts
        "%u," //rlc->rxpdu_bytes
        "%u," //rlc->rxpdu_dup_pkts
        "%u," //rlc->rxpdu_dup_bytes
        "%u," //rlc->rxpdu_dd_pkts
        "%u," //rlc->rxpdu_dd_bytes
        "%u," //rlc->rxpdu_ow_pkts
        "%u," //rlc->rxpdu_ow_bytes
        "%u," //rlc->rxpdu_status_pkts
        "%u," //rlc->rxpdu_status_bytes
        "%u," //rlc->rxbuf_occ_bytes
        "%u," //rlc->rxbuf_occ_pkts
        "%u," //rlc->txsdu_pkts
        "%lu," //rlc->txsdu_bytes
        "%.2f," //rlc->txsdu_avg_time_to_tx
        "%u," //rlc->txsdu_wt_us
        "%u," //rlc->rxsdu_pkts
        "%lu," //rlc->rxsdu_bytes
        "%u," //rlc->rxsdu_dd_pkts
        "%u," //rlc->rxsdu_dd_bytes
        "%u," //rlc->rnti
        "%u,"  //rlc->mode
        "%u"  //rlc->rbid 
        ");"
        , tstamp
        , id->type
        , id->plmn.mcc
        , id->plmn.mnc
        , id->plmn.mnc_digit_len
        , id->nb_id.nb_id 
        , id->cu_du_id ? c_cu_du_id : c_null
        , rlc->txpdu_pkts
        , rlc->txpdu_bytes
        , rlc->txpdu_wt_ms
        , rlc->txpdu_dd_pkts
        , rlc->txpdu_dd_bytes
        , rlc->txpdu_retx_pkts
        , rlc->txpdu_retx_bytes
        , rlc->txpdu_segmented
        , rlc->txpdu_status_pkts
        , rlc->txpdu_status_bytes
        , rlc->txbuf_occ_bytes
        , rlc->txbuf_occ_pkts
        , rlc->rxpdu_pkts
        , rlc->rxpdu_bytes
        , rlc->rxpdu_dup_pkts
        , rlc->rxpdu_dup_bytes
        , rlc->rxpdu_dd_pkts
        , rlc->rxpdu_dd_bytes
        , rlc->rxpdu_ow_pkts
        , rlc->rxpdu_ow_bytes
        , rlc->rxpdu_status_pkts
        , rlc->rxpdu_status_bytes
        , rlc->rxbuf_occ_bytes
        , rlc->rxbuf_occ_pkts
        , rlc->txsdu_pkts
        , rlc->txsdu_bytes
        , rlc->txsdu_avg_time_to_tx
        , rlc->txsdu_wt_us
        , rlc->rxsdu_pkts
        , rlc->rxsdu_bytes
        , rlc->rxsdu_dd_pkts
        , rlc->rxsdu_dd_bytes
        , rlc->rnti
        , rlc->mode
        , rlc->rbid
        );
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}


static
int to_sql_string_pdcp_rb(global_e2_node_id_t const* id, pdcp_radio_bearer_stats_t* pdcp, int64_t tstamp, char* out, size_t out_len)
{                              
  assert(pdcp != NULL);        
  assert(out != NULL);
  const size_t max = 512;      
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  int rc = snprintf(out, out_len, 
      "INSERT INTO PDCP_bearer VALUES("
        "%ld," //tstamp         
        "%d," //ngran_node  
        "%d," //mcc
        "%d," //mnc
        "%d," //mnc_digit_len   
        "%d," //nb_id 
        "'%s'," //cu_du_id
        "%u," //txpdu_pkts       
        "%u," //txpdu_bytes     
        "%u," // txpdu_sn   
        "%u," //rxpdu_pkts      
        "%u," //rxpdu_bytes     
        "%u,"     //rxpdu_sn    
        "%u," //rxpdu_oo_pkts   
        "%u," //rxpdu_oo_bytes  
        "%u,"   //rxpdu_dd_pkts 
        "%u,"  //rxpdu_dd_bytes 
        "%u," //rxpdu_ro_count  
        "%u,"//txsdu_pkts
        "%u," //txsdu_bytes     
        "%u,"//rxsdu_pkts
        "%u," //rxsdu_bytes     
        "%u," //rnti            
        "%u," //mode            
        "%u" //rbid
        ");"
        , tstamp               
        , id->type
        , id->plmn.mcc
        , id->plmn.mnc
        , id->plmn.mnc_digit_len
        , id->nb_id.nb_id 
        , id->cu_du_id ? c_cu_du_id : c_null
        , pdcp->txpdu_pkts     
        , pdcp->txpdu_bytes    
        , pdcp->txpdu_sn       
        , pdcp->rxpdu_pkts     
        , pdcp->rxpdu_bytes    
        , pdcp->rxpdu_sn       
        , pdcp->rxpdu_oo_pkts  
        , pdcp->rxpdu_oo_bytes 
        , pdcp->rxpdu_dd_pkts  
        , pdcp->rxpdu_dd_bytes 
        , pdcp->rxpdu_ro_count 
        , pdcp->txsdu_pkts     
        , pdcp->txsdu_bytes    
        , pdcp->rxsdu_pkts     
        , pdcp->rxsdu_bytes    
        , pdcp->rnti           
        , pdcp->mode           
        , pdcp->rbid           
        );                     
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_sql_string_ue_slice_rb(global_e2_node_id_t const* id, ue_slice_conf_t const* ues, ue_slice_assoc_t const* u, int64_t tstamp, char* out, size_t out_len)
{
  assert(ues != NULL);
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  int rc = 0;
  if (u == NULL) {
    rc = snprintf(out, out_len,
                  "INSERT INTO UE_SLICE VALUES("
                  "%ld,"   // tstamp
                  "%d,"    // ngran_node
                  "%d,"    // mcc
                  "%d,"    // mnc
                  "%d,"    // mnc_digit_len
                  "%d,"    // nb_id
                  "'%s',"  // cu_du_id
                  "%d,"    // dl->len_ue_slices
                  "%d,"    // ues[i]->rnti
                  "%d"     // ues[i]->dl_id
                  ");"
                  , tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id
                  , id->cu_du_id ? c_cu_du_id : c_null
                  , ues->len_ue_slice, -1, -1);
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return rc;
  }

  rc = snprintf(out, out_len,
                "INSERT INTO UE_SLICE VALUES("
                "%ld,"   // tstamp
                "%d,"    // ngran_node
                "%d,"    // mcc
                "%d,"    // mnc
                "%d,"    // mnc_digit_len
                "%d,"    // nb_id
                "'%s',"  // cu_du_id
                "%d,"    // dl->len_ue_slices
                "%d,"    // ues[i]->rnti
                "%d"     // ues[i]->dl_id
                ");"
                , tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id
                , id->cu_du_id ? c_cu_du_id : c_null
                , ues->len_ue_slice, u->rnti, u->dl_id);
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_sql_string_slice_rb(global_e2_node_id_t const* id, ul_dl_slice_conf_t const* slices, fr_slice_t const* s, int64_t tstamp, char* out, size_t out_len)
{
  assert(slices != NULL);
  assert(out != NULL);
  const size_t max = 1024;
  assert(out_len >= max);

  char* c_null = NULL;
  char sched_name[50];
  sched_name[0] = '\0';
  strncat(sched_name, slices->sched_name, slices->len_sched_name);

  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  int rc = 0;
  if (s == NULL) {
    rc = snprintf(out, out_len,
                  "INSERT INTO SLICE VALUES("
                  "%ld,"   // tstamp
                  "%d,"    // ngran_node
                  "%d,"    // mcc
                  "%d,"    // mnc
                  "%d,"    // mnc_digit_len
                  "%d,"    // nb_id
                  "'%s',"  // cu_du_id
                  "%d,"    // dl->len_slices
                  "'%s',"  // dl->sched_name
                  "%u,"    // dl->slice[i].id
                  "'%s',"  // dl->slice[i].label
                  "'%s',"  // dl->slice[i]->params.type
                  "'%s',"  // dl->slice[i]->params.u.nvs.conf
                  "'%s',"  // dl->slice[i].sched
                  "%.2f,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u1.mbps_required/nvs.u.capacity.u.pct_reserved/edf.deadline
                  "%.2f,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u2.mbps_reference/edf.guaranteed_prbs
                  "%.2f"  // dl->slice[i]->params.u.edf.max_replenish
                  ");"
                  , tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id
                  , id->cu_du_id ? c_cu_du_id : c_null
                  , 0, sched_name, 0, c_null, c_null, c_null, c_null, 0.00, 0.00, 0.00);
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return rc;
  }

  char label[50];
  label[0] = '\0';
  strncat(label, s->label, s->len_label);
  char params_type[10];
  params_type[0] = '\0';
  char params_type_conf[10];
  params_type_conf[0] = '\0';
  char sched[50];
  sched[0] = '\0';
  strncat(sched, s->sched, s->len_sched);
  if (s->params.type == SLICE_ALG_SM_V0_STATIC) {
    strcat(params_type, "STATIC");
    rc = snprintf(out, out_len,
                  "INSERT INTO SLICE VALUES("
                  "%ld,"   // tstamp
                  "%d,"    // ngran_node
                  "%d,"    // mcc
                  "%d,"    // mnc
                  "%d,"    // mnc_digit_len
                  "%d,"    // nb_id
                  "'%s',"  // cu_du_id
                  "%d,"    // dl->len_slices
                  "'%s',"  // dl->sched_name
                  "%u,"    // dl->slice[i].id
                  "'%s',"  // dl->slice[i].label
                  "'%s',"  // dl->slice[i]->params.type
                  "'%s',"  // dl->slice[i]->params.u.nvs.conf
                  "'%s',"  // dl->slice[i].sched
                  "%d,"    // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u1.mbps_required/nvs.u.capacity.u.pct_reserved/edf.deadline
                  "%d,"    // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u2.mbps_reference/edf.guaranteed_prbs
                  "%.2f"  // dl->slice[i]->params.u.edf.max_replenish
                  ");"
                  , tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id
                  , id->cu_du_id ? c_cu_du_id : c_null
                  , slices->len_slices, c_null
                  , s->id, label, params_type, c_null, sched
                  , s->params.u.sta.pos_low, s->params.u.sta.pos_high, 0.00);
  } else if (s->params.type == SLICE_ALG_SM_V0_NVS) {
    strcat(params_type, "NVS");
    if (s->params.u.nvs.conf == SLICE_SM_NVS_V0_RATE) {
      strcat(params_type_conf, "RATE");
      rc = snprintf(out, out_len,
                    "INSERT INTO SLICE VALUES("
                    "%ld,"   // tstamp
                    "%d,"    // ngran_node
                    "%d,"    // mcc
                    "%d,"    // mnc
                    "%d,"    // mnc_digit_len
                    "%d,"    // nb_id
                    "'%s',"  // cu_du_id
                    "%d,"    // dl->len_slices
                    "'%s',"  // dl->sched_name
                    "%u,"    // dl->slice[i].id
                    "'%s',"  // dl->slice[i].label
                    "'%s',"  // dl->slice[i]->params.type
                    "'%s',"  // dl->slice[i]->params.u.nvs.conf
                    "'%s',"  // dl->slice[i].sched
                    "%.2f,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u1.mbps_required/nvs.u.capacity.u.pct_reserved/edf.deadline
                    "%.2f,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u2.mbps_reference/edf.guaranteed_prbs
                    "%.2f"  // dl->slice[i]->params.u.edf.max_replenish
                    ");"
                    , tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id
                    , id->cu_du_id ? c_cu_du_id : c_null
                    , slices->len_slices, c_null
                    , s->id, label, params_type, params_type_conf, sched
                    , s->params.u.nvs.u.rate.u1.mbps_required, s->params.u.nvs.u.rate.u2.mbps_reference, 0.00);
      } else if (s->params.u.nvs.conf == SLICE_SM_NVS_V0_CAPACITY) {
        strcat(params_type_conf, "CAPACITY");
        rc = snprintf(out, out_len,
                      "INSERT INTO SLICE VALUES("
                      "%ld,"   // tstamp
                      "%d,"    // ngran_node
                      "%d,"    // mcc
                      "%d,"    // mnc
                      "%d,"    // mnc_digit_len
                      "%d,"    // nb_id
                      "'%s',"  // cu_du_id
                      "%d,"    // dl->len_slices
                      "'%s',"  // dl->sched_name
                      "%u,"    // dl->slice[i].id
                      "'%s',"  // dl->slice[i].label
                      "'%s',"  // dl->slice[i]->params.type
                      "'%s',"  // dl->slice[i]->params.u.nvs.conf
                      "'%s',"  // dl->slice[i].sched
                      "%.2f,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u1.mbps_required/nvs.u.capacity.u.pct_reserved/edf.deadline
                      "%.2f,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u2.mbps_reference/edf.guaranteed_prbs
                      "%.2f"  // dl->slice[i]->params.u.edf.max_replenish
                      ");"
                      , tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id
                      , id->cu_du_id ? c_cu_du_id : c_null
                      , slices->len_slices, c_null
                      , s->id, label, params_type, params_type_conf, sched
                      , s->params.u.nvs.u.capacity.u.pct_reserved, 0.00, 0.00);
      }
  } else if (s->params.type == SLICE_ALG_SM_V0_EDF) {
    strcat(params_type, "EDF");
    rc = snprintf(out, out_len,
                  "INSERT INTO SLICE VALUES("
                  "%ld,"   // tstamp
                  "%d,"    // ngran_node
                  "%d,"    // mcc
                  "%d,"    // mnc
                  "%d,"    // mnc_digit_len
                  "%d,"    // nb_id
                  "'%s',"  // cu_du_id
                  "%d,"    // dl->len_slices
                  "'%s',"  // dl->sched_name
                  "%u,"    // dl->slice[i].id
                  "'%s',"  // dl->slice[i].label
                  "'%s',"  // dl->slice[i]->params.type
                  "'%s',"  // dl->slice[i]->params.u.nvs.conf
                  "'%s',"  // dl->slice[i].sched
                  "%d,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u1.mbps_required/nvs.u.capacity.u.pct_reserved/edf.deadline
                  "%d,"  // dl->slice[i]->params.u.sta.pos_high/nvs.u.rate.u2.mbps_reference/edf.guaranteed_prbs
                  "%d"  // dl->slice[i]->params.u.edf.max_replenish
                  ");"
                  , tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id
                  , id->cu_du_id ? c_cu_du_id : c_null
                  , slices->len_slices, c_null
                  , s->id, label, params_type, c_null, sched
                  , s->params.u.edf.deadline
                  , s->params.u.edf.guaranteed_prbs
                  , s->params.u.edf.max_replenish);
  }
  assert(rc < (int) max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_sql_string_gtp_NGUT(global_e2_node_id_t const* id,gtp_ngu_t_stats_t* gtp, int64_t tstamp, char* out, size_t out_len)
{
  assert(gtp != NULL);
  assert(out != NULL);
  const size_t max = 1024;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  int const rc = snprintf(out, max,
        "INSERT INTO GTP_NGUT VALUES("
        "%ld," //tstamp         
        "%d," //ngran_node  
        "%d," //mcc
        "%d," //mnc
        "%d," //mnc_digit_len   
        "%d," //nb_id 
        "'%s'," //cu_du_id
        "%u," //teidgnb    
        "%u," //rnti
        "%u," // qfi   
        "%u" //teidupf
        ");"
        , tstamp               
        , id->type
        , id->plmn.mcc
        , id->plmn.mnc
        , id->plmn.mnc_digit_len
        , id->nb_id.nb_id
        , id->cu_du_id ? c_cu_du_id : c_null
        , gtp->teidgnb
        , gtp->rnti   
        , gtp->qfi    
        , gtp->teidupf    
        );      
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
void to_mysql_string_kpm_hdr(global_e2_node_id_t const* id,
                             format_ind_hdr_e const format,
                             kpm_ric_ind_hdr_format_1_t const hdr,
                             char* out,
                             size_t out_len)
{
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  uint64_t const timestamp = hdr.collectStartTime;
  const char* fileformat_version_str = hdr.fileformat_version ? (char*)hdr.fileformat_version->buf : "NULL";
  const char* sender_name_str = hdr.sender_name ? (char*)hdr.sender_name->buf : "NULL";
  const char* sender_type_str = hdr.sender_type ? (char*)hdr.sender_type->buf : "NULL";
  const char* vendor_name_str = hdr.vendor_name ? (char*)hdr.vendor_name->buf : "NULL";

  int const rc = snprintf(out, max,
                          "INSERT INTO KPM_HDR VALUES("
                          "%lu,"   //tstamp
                          "%d,"    //ngran_node
                          "%d,"    //mcc
                          "%d,"    //mnc
                          "%d,"    //mnc_digit_len
                          "%d,"    //nb_id
                          "'%s',"  //cu_du_id
                          "%d,"    //format
                          "%lu,"   // collectStartTime
                          "'%s',"  // fileformat_version
                          "'%s',"  // sender_name
                          "'%s',"  // sender_type
                          "'%s'"   // vendor_name
                          ");"
                          ,timestamp
                          ,id->type
                          ,id->plmn.mcc
                          ,id->plmn.mnc
                          ,id->plmn.mnc_digit_len
                          ,id->nb_id.nb_id
                          ,id->cu_du_id ? c_cu_du_id : c_null
                          ,format + 1
                          ,hdr.collectStartTime
                          ,fileformat_version_str
                          ,sender_name_str
                          ,sender_type_str
                          ,vendor_name_str
                          );
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return;
}

typedef struct {
    size_t meas_data_len;
    size_t meas_data_idx;
    size_t meas_record_len;
    size_t meas_record_idx;
    meas_record_lst_t meas_record;
    size_t meas_info_len;
    size_t meas_info_idx;
    meas_type_t meas_type;
    size_t label_info_len;
    size_t label_info_idx;
} sql_str_kpm_ind_frm1_t;


static
void to_sql_string_kpm_meas_data(global_e2_node_id_t const* id,
                                 sql_str_kpm_ind_frm1_t const sql_str_kpm,
                                 uint32_t* const gran_period_ms,
                                 enum_value_e* const incomplete_flag,
                                 format_ind_msg_e const ric_ind_frmt,
                                 uint64_t const timestamp,
                                 size_t const ue_meas_report_idx,
                                 char* out,
                                 size_t out_len)
{
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  uint32_t int_value = 0;
  double real_value = 0;
  bool no_value = false;
  char c_value_type[26];
  if (sql_str_kpm.meas_record.value == INTEGER_MEAS_VALUE) {
    int_value = sql_str_kpm.meas_record.int_val;
    int rc = snprintf(c_value_type, 26, "INTEGER_MEAS_VALUE");
    assert(rc < (int) max && "Not enough space in the char array to write all the data");

    int const rcc = snprintf(out, max,
                             "INSERT INTO KPM_IND_MEAS_DATA VALUES("
                             "%lu,"   //tstamp
                             "%d,"    //ngran_node
                             "%d,"    //mcc
                             "%d,"    //mnc
                             "%d,"    //mnc_digit_len
                             "%d,"    //nb_id
                             "'%s',"  //cu_du_id
                             "%ld,"   //ue_meas_report_idx
                             "%d,"    //format
                             "%ld,"   //meas_data_len
                             "%ld,"   //meas_data_idx
                             "%ld,"   //meas_record_len
                             "%ld,"   //meas_record_idx
                             "'%s',"  //incompleteFlag
                             "%u,"    //gran_period_ms
                             "'%s',"  //meas_value_type
                             "%d,"    //meas_value_int
                             "%f,"    //meas_value_real
                             "'%s'"   //meas_value_no
                             ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ue_meas_report_idx
                            ,ric_ind_frmt + 1
                            ,sql_str_kpm.meas_data_len
                            ,sql_str_kpm.meas_data_idx
                            ,sql_str_kpm.meas_record_len
                            ,sql_str_kpm.meas_record_idx
                            ,incomplete_flag ? "TRUE":"FALSE"
                            ,gran_period_ms ? *gran_period_ms : 0
                            ,c_value_type
                            ,int_value
                            ,real_value
                            ,no_value ? "TRUE":"FALSE"
                            );
    assert(rcc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else if (sql_str_kpm.meas_record.value == REAL_MEAS_VALUE) {
    real_value = sql_str_kpm.meas_record.real_val;
    int rc = snprintf(c_value_type, 26, "REAL_MEAS_VALUE");
    assert(rc < (int) max && "Not enough space in the char array to write all the data");

    int const rcc = snprintf(out, max,
                             "INSERT INTO KPM_IND_MEAS_DATA VALUES("
                             "%lu,"   //tstamp
                             "%d,"    //ngran_node
                             "%d,"    //mcc
                             "%d,"    //mnc
                             "%d,"    //mnc_digit_len
                             "%d,"    //nb_id
                             "'%s',"  //cu_du_id
                             "%ld,"   //ue_meas_report_idx
                             "%d,"    //format
                             "%ld,"   //meas_data_len
                             "%ld,"   //meas_data_idx
                             "%ld,"   //meas_record_len
                             "%ld,"   //meas_record_idx
                             "'%s',"  //incompleteFlag
                             "%u,"    //gran_period_ms
                             "'%s',"  //meas_value_type
                             "%d,"    //meas_value_int
                             "%f,"    //meas_value_real
                             "'%s'"   //meas_value_no
                             ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ue_meas_report_idx
                            ,ric_ind_frmt + 1
                            ,sql_str_kpm.meas_data_len
                            ,sql_str_kpm.meas_data_idx
                            ,sql_str_kpm.meas_record_len
                            ,sql_str_kpm.meas_record_idx
                            ,incomplete_flag ? "TRUE":"FALSE"
                            ,gran_period_ms ? *gran_period_ms : 0
                            ,c_value_type
                            ,int_value
                            ,real_value
                            ,no_value ? "TRUE":"FALSE"
                            );
    assert(rcc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else {
    no_value = true;
    int rc = snprintf(c_value_type, 26, "NO_VALUE_MEAS_VALUE");
    assert(rc < (int) max && "Not enough space in the char array to write all the data");

    int const rcc = snprintf(out, max,
                             "INSERT INTO KPM_IND_MEAS_DATA VALUES("
                             "%lu,"   //tstamp
                             "%d,"    //ngran_node
                             "%d,"    //mcc
                             "%d,"    //mnc
                             "%d,"    //mnc_digit_len
                             "%d,"    //nb_id
                             "'%s',"  //cu_du_id
                             "%ld,"   //ue_meas_report_idx
                             "%d,"    //format
                             "%ld,"   //meas_data_len
                             "%ld,"   //meas_data_idx
                             "%ld,"   //meas_record_len
                             "%ld,"   //meas_record_idx
                             "'%s',"  //incompleteFlag
                             "%u,"    //gran_period_ms
                             "'%s',"  //meas_value_type
                             "%d,"    //meas_value_int
                             "%f,"    //meas_value_real
                             "'%s'"   //meas_value_no
                             ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ue_meas_report_idx
                            ,ric_ind_frmt + 1
                            ,sql_str_kpm.meas_data_len
                            ,sql_str_kpm.meas_data_idx
                            ,sql_str_kpm.meas_record_len
                            ,sql_str_kpm.meas_record_idx
                            ,incomplete_flag ? "TRUE":"FALSE"
                            ,gran_period_ms ? *gran_period_ms : 0
                            ,c_value_type
                            ,int_value
                            ,real_value
                            ,no_value ? "TRUE":"FALSE"
                            );
    assert(rcc < (int)max && "Not enough space in the char array to write all the data");
    return;
  }
  assert(0!=0 && "Bad input data. Nothing for SQL to be created");
}

static
void to_sql_string_kpm_meas_info(global_e2_node_id_t const* id,
                                 sql_str_kpm_ind_frm1_t const sql_str_kpm,
                                 label_info_lst_t const label_info,
                                 format_ind_msg_e const ric_ind_frmt,
                                 uint64_t const timestamp,
                                 size_t const ue_meas_report_idx,
                                 char* out,
                                 size_t out_len)
{
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char *c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  if (sql_str_kpm.meas_type.type == NAME_MEAS_TYPE) {
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_MEAS_INFO VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%ld,"   //ue_meas_report_idx
                            "%d,"    //format
                            "%ld,"   //meas_info_len
                            "%ld,"   //meas_info_idx
                            "'%s',"  //meas_type
                            "'%s',"  //meas_name
                            "-1,"    //meas_id
                            "%ld,"   //label_info_len
                            "%ld,"   //label_info_idx
                            "'%s',"  //noLabel
                            "%d,"    //plmn_id_mcc
                            "%d,"    //plmn_id_mnc
                            "%d,"    //plmn_id_mnc_digit_len
                            "%d,"    //sliceID_sST
                            "'%s',"  //sliceID_sD
                            "%d,"    //fiveQI
                            "%d,"    //qFI
                            "%d,"    //qCI
                            "%d,"    //qCImax
                            "%d,"    //qCImin
                            "%d,"    //aRPmax
                            "%d,"    //aRPmin
                            "%d,"    //bitrateRange
                            "%d,"    //layerMU_MIMO
                            "'%s',"  //sUM
                            "%d,"    //distBinX
                            "%d,"    //distBinY
                            "%d,"    //distBinZ
                            "'%s',"  //preLabelOverride
                            "%d,"    //startEndInd
                            "'%s',"  //min
                            "'%s',"  //max
                            "'%s',"  //avg
                            "%d,"    //ssbIndex
                            "%d,"    //nonGoB_beamformModeIndex
                            "%d"     //mimoModeIndex
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ue_meas_report_idx
                            ,ric_ind_frmt + 1
                            ,sql_str_kpm.meas_info_len
                            ,sql_str_kpm.meas_info_idx
                            ,"NAME_MEAS_TYPE"
                            ,sql_str_kpm.meas_type.name.buf
                            ,sql_str_kpm.label_info_len
                            ,sql_str_kpm.label_info_idx
                            ,label_info.noLabel ? "TRUE":"FALSE"
                            ,label_info.plmn_id->mcc
                            ,label_info.plmn_id->mnc
                            ,label_info.plmn_id->mnc_digit_len
                            ,label_info.sliceID->sST
                            ,*label_info.sliceID->sD
                            ,*label_info.fiveQI
                            ,*label_info.qFI
                            ,*label_info.qCI
                            ,*label_info.qCImax
                            ,*label_info.qCImin
                            ,*label_info.aRPmax
                            ,*label_info.aRPmin
                            ,*label_info.bitrateRange
                            ,*label_info.layerMU_MIMO
                            ,*label_info.sUM ? "TRUE":"FALSE"
                            ,*label_info.distBinX
                            ,*label_info.distBinY
                            ,*label_info.distBinZ
                            ,*label_info.preLabelOverride ? "TRUE":"FALSE"
                            ,*label_info.startEndInd
                            ,*label_info.min ? "TRUE":"FALSE"
                            ,*label_info.max ? "TRUE":"FALSE"
                            ,*label_info.avg ? "TRUE":"FALSE"
                            ,*label_info.ssbIndex
                            ,*label_info.nonGoB_beamformModeIndex
                            ,*label_info.mimoModeIndex
                            );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else if (sql_str_kpm.meas_type.type == ID_MEAS_TYPE) {
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_MEAS_INFO VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%ld,"   //ue_meas_report_idx
                            "%d,"    //format
                            "%ld,"   //meas_info_len
                            "%ld,"   //meas_info_idx
                            "'%s',"  //meas_type
                            "'%s',"  //meas_name
                            "%d,"    //meas_id
                            "%ld,"   //label_info_len
                            "%ld,"   //label_info_idx
                            "'%s',"  //noLabel
                            "%d,"    //plmn_id_mcc
                            "%d,"    //plmn_id_mnc
                            "%d,"    //plmn_id_mnc_digit_len
                            "%d,"    //sliceID_sST
                            "'%s',"  //sliceID_sD
                            "%d,"    //fiveQI
                            "%d,"    //qFI
                            "%d,"    //qCI
                            "%d,"    //qCImax
                            "%d,"    //qCImin
                            "%d,"    //aRPmax
                            "%d,"    //aRPmin
                            "%d,"    //bitrateRange
                            "%d,"    //layerMU_MIMO
                            "'%s',"  //sUM
                            "%d,"    //distBinX
                            "%d,"    //distBinY
                            "%d,"    //distBinZ
                            "'%s',"  //preLabelOverride
                            "%d,"    //startEndInd
                            "'%s',"  //min
                            "'%s',"  //max
                            "'%s',"  //avg
                            "%d,"    //ssbIndex
                            "%d,"    //nonGoB_beamformModeIndex
                            "%d"     //mimoModeIndex
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ue_meas_report_idx
                            ,ric_ind_frmt + 1
                            ,sql_str_kpm.meas_info_len
                            ,sql_str_kpm.meas_info_idx
                            ,"ID_MEAS_TYPE"
                            ,"NULL"
                            ,sql_str_kpm.meas_type.id
                            ,sql_str_kpm.label_info_len
                            ,sql_str_kpm.label_info_idx
                            ,label_info.noLabel ? "TRUE":"FALSE"
                            ,label_info.plmn_id->mcc
                            ,label_info.plmn_id->mnc
                            ,label_info.plmn_id->mnc_digit_len
                            ,label_info.sliceID->sST
                            ,*label_info.sliceID->sD
                            ,*label_info.fiveQI
                            ,*label_info.qFI
                            ,*label_info.qCI
                            ,*label_info.qCImax
                            ,*label_info.qCImin
                            ,*label_info.aRPmax
                            ,*label_info.aRPmin
                            ,*label_info.bitrateRange
                            ,*label_info.layerMU_MIMO
                            ,*label_info.sUM ? "TRUE":"FALSE"
                            ,*label_info.distBinX
                            ,*label_info.distBinY
                            ,*label_info.distBinZ
                            ,*label_info.preLabelOverride ? "TRUE":"FALSE"
                            ,*label_info.startEndInd
                            ,*label_info.min ? "TRUE":"FALSE"
                            ,*label_info.max ? "TRUE":"FALSE"
                            ,*label_info.avg ? "TRUE":"FALSE"
                            ,*label_info.ssbIndex
                            ,*label_info.nonGoB_beamformModeIndex
                            ,*label_info.mimoModeIndex
                            );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else {
    assert(0!=0 && "unknown meas type\n");
  }

  assert(0!=0 && "Bad input data. Nothing for SQL to be created");
}

static
void to_sql_string_kpm_meas_data_info(global_e2_node_id_t const* id,
                                      sql_str_kpm_ind_frm1_t const sql_str_kpm,
                                      uint32_t* const gran_period_ms,
                                      enum_value_e* const incomplete_flag,
                                      format_ind_msg_e const ric_ind_frmt,
                                      uint64_t const timestamp,
                                      size_t const ue_meas_report_idx,
                                      char* out,
                                      size_t out_len)
{
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  uint32_t int_value = 0;
  double real_value = 0;
  bool no_value = false;
  char c_value_type[26];
  if (sql_str_kpm.meas_record.value == INTEGER_MEAS_VALUE) {
    int_value = sql_str_kpm.meas_record.int_val;
    int rc = snprintf(c_value_type, 26, "INTEGER_MEAS_VALUE");
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  } else if (sql_str_kpm.meas_record.value == REAL_MEAS_VALUE) {
    real_value = sql_str_kpm.meas_record.real_val;
    int rc = snprintf(c_value_type, 26, "REAL_MEAS_VALUE");
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  } else {
    no_value = true;
    int rc = snprintf(c_value_type, 26, "NO_VALUE_MEAS_VALUE");
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  if (sql_str_kpm.meas_type.type == NAME_MEAS_TYPE) {
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_MEAS_DATA_INFO VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%ld,"   //ue_meas_report_idx
                            "%d,"    //format
                            "%ld,"   //meas_data_len
                            "%ld,"   //meas_data_idx
                            "%ld,"   //meas_record_len
                            "%ld,"   //meas_record_idx
                            "'%s',"  //incompleteFlag
                            "%ld,"   //meas_info_len
                            "%ld,"   //meas_info_idx
                            "%u,"    //gran_period_ms
                            "'%s',"  //meas_type
                            "'%s',"  //meas_name
                            "-1,"    //meas_id
                            "'%s',"  //meas_value_type
                            "%d,"    //meas_value_int
                            "%f,"    //meas_value_real
                            "'%s'"   //meas_value_no
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ue_meas_report_idx
                            ,ric_ind_frmt + 1
                            ,sql_str_kpm.meas_data_len
                            ,sql_str_kpm.meas_data_idx
                            ,sql_str_kpm.meas_record_len
                            ,sql_str_kpm.meas_record_idx
                            ,incomplete_flag ? "TRUE":"FALSE"
                            ,sql_str_kpm.meas_info_len
                            ,sql_str_kpm.meas_info_idx
                            ,gran_period_ms ? *gran_period_ms : 0
                            ,"NAME_MEAS_TYPE"
                            ,sql_str_kpm.meas_type.name.buf
                            ,c_value_type
                            ,int_value
                            ,real_value
                            ,no_value ? "TRUE":"FALSE"
                            );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else if (sql_str_kpm.meas_type.type == ID_MEAS_TYPE) {
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_MEAS_DATA_INFO VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%ld,"   //ue_meas_report_idx
                            "%d,"    //format
                            "%ld,"   //meas_data_len
                            "%ld,"   //meas_data_idx
                            "%ld,"   //meas_record_len
                            "%ld,"   //meas_record_idx
                            "'%s',"  //incompleteFlag
                            "%ld,"   //meas_info_len
                            "%ld,"   //meas_info_idx
                            "%u,"    //gran_period_ms
                            "'%s',"  //meas_type
                            "'%s',"  //meas_name
                            "%d,"    //meas_id
                            "'%s',"  //meas_value_type
                            "%d,"    //meas_value_int
                            "%f,"    //meas_value_real
                            "'%s'"   //meas_value_no
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ue_meas_report_idx
                            ,ric_ind_frmt + 1
                            ,sql_str_kpm.meas_data_len
                            ,sql_str_kpm.meas_data_idx
                            ,sql_str_kpm.meas_record_len
                            ,sql_str_kpm.meas_record_idx
                            ,incomplete_flag ? "TRUE":"FALSE"
                            ,sql_str_kpm.meas_info_len
                            ,sql_str_kpm.meas_info_idx
                            ,gran_period_ms ? *gran_period_ms : 0
                            ,"NAME_ID_TYPE"
                            ,"NULL"
                            ,sql_str_kpm.meas_type.id
                            ,c_value_type
                            ,int_value
                            ,real_value
                            ,no_value ? "TRUE":"FALSE"
                            );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else {
    assert(0!=0 && "unknown meas type\n");
  }

  assert(0!=0 && "Bad input data. Nothing for SQL to be created");
}

static
void to_sql_string_kpm_ue_id_e2sm(global_e2_node_id_t const* id,
                                  ue_id_e2sm_t const ue_id_e2sm,
                                  format_ind_msg_e const ric_ind_frmt,
                                  ue_id_e2sm_e const ue_id_e2sm_type,
                                  uint64_t const timestamp,
                                  char* out,
                                  size_t out_len)
{
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char *c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  if (ue_id_e2sm_type == GNB_UE_ID_E2SM) {
    gnb_e2sm_t gnb = ue_id_e2sm.gnb;
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_UE_ID_E2SM VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%d,"    //format
                            "'%s',"  //ue_id_e2sm_type
                            "%d,"    //guami_plmn_id_mcc
                            "%d,"    //guami_plmn_id_mnc
                            "%d,"    //guami_plmn_id_mnc_digit_len
                            /// gnb.h ///
                            "%ld,"   //amf_ue_ngap_id
                            "%d,"    //guami_amf_region_id
                            "%d,"    //amf_set_id
                            "%d,"    //amf_ptr
                            "%ld,"   //gnb_cu_ue_f1ap_lst_len
                            "%d,"    //gnb_cu_ue_f1ap_lst
                            "%ld,"   //gnb_cu_cp_ue_e1ap_lst_len
                            "%d,"    //gnb_cu_cp_ue_e1ap_lst
                            "%lu,"   //ran_ue_id
                            "%u,"    //ng_ran_node_ue_xnap_id
                            /// enb.h ///
                            "%d,"    //mme_ue_s1ap_id
                            "%d,"    //guami_mme_group_id
                            "%d,"    //guami_mme_code
                            "%d,"    //enb_ue_x2ap_id
                            "%d"     //enb_ue_x2ap_id_extension
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ric_ind_frmt + 1
                            ,"GNB_UE_ID_E2SM"
                            ,gnb.guami.plmn_id.mcc
                            ,gnb.guami.plmn_id.mnc
                            ,gnb.guami.plmn_id.mnc_digit_len
                            ,gnb.amf_ue_ngap_id
                            ,gnb.guami.amf_region_id
                            ,gnb.guami.amf_set_id
                            ,gnb.guami.amf_ptr
                            ,gnb.gnb_cu_ue_f1ap_lst_len
                            ,gnb.gnb_cu_ue_f1ap_lst_len > 0 ? *gnb.gnb_cu_ue_f1ap_lst : 0
                            ,gnb.gnb_cu_cp_ue_e1ap_lst_len
                            ,gnb.gnb_cu_cp_ue_e1ap_lst_len > 0 ? *gnb.gnb_cu_cp_ue_e1ap_lst : 0
                            ,gnb.ran_ue_id ? *gnb.ran_ue_id : 0
                            ,gnb.ng_ran_node_ue_xnap_id ? *gnb.ng_ran_node_ue_xnap_id : 0
                            ,-1
                            ,-1
                            ,-1
                            ,-1
                            ,-1
                            );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else if (ue_id_e2sm_type == GNB_DU_UE_ID_E2SM) {
    gnb_du_e2sm_t gnb_du = ue_id_e2sm.gnb_du;
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_UE_ID_E2SM VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%d,"    //format
                            "'%s',"  //ue_id_e2sm_type
                            "%d,"    //guami_plmn_id_mcc
                            "%d,"    //guami_plmn_id_mnc
                            "%d,"    //guami_plmn_id_mnc_digit_len
                            /// gnb.h ///
                            "%ld,"   //amf_ue_ngap_id
                            "%d,"    //guami_amf_region_id
                            "%d,"    //amf_set_id
                            "%d,"    //amf_ptr
                            "%ld,"   //gnb_cu_ue_f1ap_lst_len
                            "%d,"    //gnb_cu_ue_f1ap_lst
                            "%ld,"   //gnb_cu_cp_ue_e1ap_lst_len
                            "%d,"    //gnb_cu_cp_ue_e1ap_lst
                            "%lu,"   //ran_ue_id
                            "%u,"    //ng_ran_node_ue_xnap_id
                            /// enb.h ///
                            "%d,"    //mme_ue_s1ap_id
                            "%d,"    //guami_mme_group_id
                            "%d,"    //guami_mme_code
                            "%d,"    //enb_ue_x2ap_id
                            "%d"     //enb_ue_x2ap_id_extension
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null, ric_ind_frmt + 1
                            ,"GNB_DU_UE_ID_E2SM"
                            ,-1
                            ,-1
                            ,-1
                            ,(long int)-1
                            ,-1
                            ,-1
                            ,-1
                            ,(long int)1
                            ,gnb_du.gnb_cu_ue_f1ap
                            ,(long int)-1
                            ,-1
                            ,gnb_du.ran_ue_id ? *gnb_du.ran_ue_id : 0
                            ,0
                            ,-1
                            ,-1
                            ,-1
                            ,-1
                            ,-1
                          );
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
    return;
  } else if (ue_id_e2sm_type == GNB_CU_UP_UE_ID_E2SM) {
    gnb_cu_up_e2sm_t gnb_cu = ue_id_e2sm.gnb_cu_up;
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_UE_ID_E2SM VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%d,"    //format
                            "'%s',"  //ue_id_e2sm_type
                            "%d,"    //guami_plmn_id_mcc
                            "%d,"    //guami_plmn_id_mnc
                            "%d,"    //guami_plmn_id_mnc_digit_len
                            /// gnb.h ///
                            "%ld,"   //amf_ue_ngap_id
                            "%d,"    //guami_amf_region_id
                            "%d,"    //amf_set_id
                            "%d,"    //amf_ptr
                            "%ld,"   //gnb_cu_ue_f1ap_lst_len
                            "%d,"    //gnb_cu_ue_f1ap_lst
                            "%ld,"   //gnb_cu_cp_ue_e1ap_lst_len
                            "%d,"    //gnb_cu_cp_ue_e1ap_lst
                            "%lu,"   //ran_ue_id
                            "%u,"    //ng_ran_node_ue_xnap_id
                            /// enb.h ///
                            "%d,"    //mme_ue_s1ap_id
                            "%d,"    //guami_mme_group_id
                            "%d,"    //guami_mme_code
                            "%d,"    //enb_ue_x2ap_id
                            "%d"     //enb_ue_x2ap_id_extension
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ric_ind_frmt + 1
                            ,"GNB_CU_UP_UE_ID_E2SM"
                            ,-1
                            ,-1
                            ,-1
                            ,(long int)-1
                            ,-1
                            ,-1
                            ,-1
                            ,(long int)-1
                            ,-1
                            ,(long int)1
                            ,gnb_cu.gnb_cu_cp_ue_e1ap
                            ,gnb_cu.ran_ue_id ? *gnb_cu.ran_ue_id : 0
                            , 0
                            ,-1
                            ,-1
                            ,-1
                            ,-1
                            ,-1
                          );
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
    return;
  } else if (ue_id_e2sm_type == ENB_UE_ID_E2SM) {
    enb_e2sm_t enb = ue_id_e2sm.enb;
    int const rc = snprintf(out, max,
                            "INSERT INTO KPM_IND_UE_ID_E2SM VALUES("
                            "%lu,"   //tstamp
                            "%d,"    //ngran_node
                            "%d,"    //mcc
                            "%d,"    //mnc
                            "%d,"    //mnc_digit_len
                            "%d,"    //nb_id
                            "'%s',"  //cu_du_id
                            "%d,"    //format
                            "'%s',"  //ue_id_e2sm_type
                            "%d,"    //guami_plmn_id_mcc
                            "%d,"    //guami_plmn_id_mnc
                            "%d,"    //guami_plmn_id_mnc_digit_len
                            /// gnb.h ///
                            "%ld,"   //amf_ue_ngap_id
                            "%d,"    //guami_amf_region_id
                            "%d,"    //amf_set_id
                            "%d,"    //amf_ptr
                            "%ld,"   //gnb_cu_ue_f1ap_lst_len
                            "%d,"    //gnb_cu_ue_f1ap_lst
                            "%ld,"   //gnb_cu_cp_ue_e1ap_lst_len
                            "%d,"    //gnb_cu_cp_ue_e1ap_lst
                            "%ld,"    //ran_ue_id
                            "%d,"    //ng_ran_node_ue_xnap_id
                            /// enb.h ///
                            "%d,"    //mme_ue_s1ap_id
                            "%d,"    //guami_mme_group_id
                            "%d,"    //guami_mme_code
                            "%u,"    //enb_ue_x2ap_id
                            "%u"     //enb_ue_x2ap_id_extension
                            ");"
                            ,timestamp
                            ,id->type
                            ,id->plmn.mcc
                            ,id->plmn.mnc
                            ,id->plmn.mnc_digit_len
                            ,id->nb_id.nb_id
                            ,id->cu_du_id ? c_cu_du_id : c_null
                            ,ric_ind_frmt + 1
                            ,"ENB_UE_ID_E2SM"
                            ,enb.gummei.plmn_id.mcc
                            ,enb.gummei.plmn_id.mnc
                            ,enb.gummei.plmn_id.mnc_digit_len
                            ,(long int)-1
                            ,-1
                            ,-1
                            ,-1
                            ,(long int)-1
                            ,-1
                            ,(long int)-1
                            ,-1
                            ,(long int)-1
                            ,-1
                            ,enb.mme_ue_s1ap_id
                            ,enb.gummei.mme_group_id
                            ,enb.gummei.mme_code
                            ,enb.enb_ue_x2ap_id ? *enb.enb_ue_x2ap_id : 0
                            ,enb.enb_ue_x2ap_id_extension ? *enb.enb_ue_x2ap_id_extension : 0
                            );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else {
    assert(0!=0 && "not support ue_id_e2sm type");
  }
  assert(0!=0 && "Bad input data. Nothing for SQL to be created");
}

static
void write_mac_stats(sqlite3* db, global_e2_node_id_t const* id, mac_ind_data_t const* ind )
{
  assert(db != NULL);
  assert(ind != NULL);

  mac_ind_msg_t const* ind_msg_mac = &ind->msg; 

  char buffer[4096] = {0};
  int pos = 0;

  for(size_t i = 0; i < ind_msg_mac->len_ue_stats; ++i){
    pos += to_sql_string_mac_ue(id, &ind_msg_mac->ue_stats[i], ind_msg_mac->tstamp, buffer + pos, 4096 - pos);
  }

  insert_db(db, buffer);
}

static
void write_rlc_stats(sqlite3* db, global_e2_node_id_t const* id, rlc_ind_data_t const* ind)
{
  assert(db != NULL);
  assert(ind != NULL);

  rlc_ind_msg_t const* ind_msg_rlc = &ind->msg; 

  char buffer[2048] = {0};
  int pos = 0;

  for(size_t i = 0; i < ind_msg_rlc->len; ++i){
    pos += to_sql_string_rlc_rb(id, &ind_msg_rlc->rb[i], ind_msg_rlc->tstamp, buffer + pos, 2048 - pos);
  }
  insert_db(db, buffer);
}

static
void write_pdcp_stats(sqlite3* db, global_e2_node_id_t const* id, pdcp_ind_data_t const* ind)
{
  assert(db != NULL);
  assert(ind != NULL);

  pdcp_ind_msg_t const* ind_msg_pdcp = &ind->msg; 

  char buffer[2048] = {0};
  int pos = 0;

  for(size_t i = 0; i < ind_msg_pdcp->len; ++i){
    pos += to_sql_string_pdcp_rb(id, &ind_msg_pdcp->rb[i], ind_msg_pdcp->tstamp, buffer + pos, 2048 - pos);
  }

  insert_db(db, buffer);
}

static
void write_slice_conf_stats(sqlite3* db, global_e2_node_id_t const* id, int64_t tstamp, slice_conf_t const* slice_conf)
{
  char buffer[4096] = {0};
  int pos = 0;

  ul_dl_slice_conf_t const* dlslices = &slice_conf->dl;
  if (dlslices->len_slices > 0) {
    for(size_t i = 0; i < dlslices->len_slices; ++i) {
      fr_slice_t const* s = &dlslices->slices[i];
      pos += to_sql_string_slice_rb(id, dlslices, s, tstamp, buffer + pos, 4096 - pos);
    }
  } else {
    pos += to_sql_string_slice_rb(id, dlslices, NULL, tstamp, buffer + pos, 4096 - pos);
  }

  // TODO: Process uplink slice stats

  insert_db(db, buffer);
}

static
void write_ue_slice_conf_stats(sqlite3* db, global_e2_node_id_t const* id, int64_t tstamp, ue_slice_conf_t const* ue_slice_conf)
{
  char buffer[4096] = {0};
  int pos = 0;

  if (ue_slice_conf->len_ue_slice > 0) {
    for(uint32_t j = 0; j < ue_slice_conf->len_ue_slice; ++j) {
      ue_slice_assoc_t *u = &ue_slice_conf->ues[j];
      pos += to_sql_string_ue_slice_rb(id, ue_slice_conf, u, tstamp, buffer + pos, 2048 - pos);
    }
  } else {
    pos += to_sql_string_ue_slice_rb(id, ue_slice_conf, NULL, tstamp, buffer + pos, 2048 - pos);
  }

  insert_db(db, buffer);
}

static
void write_slice_stats(sqlite3* db, global_e2_node_id_t const* id, slice_ind_data_t const* ind)
{
  assert(db != NULL);
  assert(ind != NULL);

  slice_ind_msg_t const* ind_msg_slice = &ind->msg;

  write_slice_conf_stats(db, id, ind_msg_slice->tstamp, &ind_msg_slice->slice_conf);
  write_ue_slice_conf_stats(db, id, ind_msg_slice->tstamp, &ind_msg_slice->ue_slice_conf);

}

static
void write_gtp_stats(sqlite3* db, global_e2_node_id_t const* id, gtp_ind_data_t const* ind)
{
  assert(db != NULL);
  assert(ind != NULL);

  gtp_ind_msg_t const* ind_msg_gtp = &ind->msg; 

  char buffer[2048] = {0};
  int pos = 0;
  for(size_t i = 0; i < ind_msg_gtp->len; ++i){
    pos += to_sql_string_gtp_NGUT(id, &ind_msg_gtp->ngut[i], ind_msg_gtp->tstamp, buffer + pos, 2048 - pos);
  }

  insert_db(db, buffer);
}

void write_kpm_frm1_stats(sqlite3* db,
                          global_e2_node_id_t const* id,
                          format_ind_msg_e const ric_ind_frmt,
                          kpm_ind_msg_format_1_t const* msg,
                          uint64_t const timestamp,
                          size_t const ue_meas_report_idx)
{
  assert(db != NULL);
  assert(msg != NULL);

  char buffer[512] = {0};

  for (size_t i = 0; i < msg->meas_data_lst_len; i++) {
    sql_str_kpm_ind_frm1_t sql_str_kpm = {0};
    sql_str_kpm.meas_data_len = msg->meas_data_lst_len;
    sql_str_kpm.meas_data_idx = i;

    meas_data_lst_t meas_data = msg->meas_data_lst[i];
    if (msg->meas_info_lst_len != meas_data.meas_record_len) {
      printf("meas_info_lst_len != meas_record_len, cannot write kpm ind frm1 to sqlite3 db\n");
      break;
    }

    sql_str_kpm.meas_record_len = meas_data.meas_record_len;
    sql_str_kpm.meas_info_len = msg->meas_info_lst_len;
    for (size_t j = 0; j < meas_data.meas_record_len; j++) {
      meas_record_lst_t meas_record = meas_data.meas_record_lst[j];
      sql_str_kpm.meas_record_idx = j;
      sql_str_kpm.meas_record = meas_record;
      meas_info_format_1_lst_t meas_info = msg->meas_info_lst[j];
      sql_str_kpm.meas_info_idx = j;
      sql_str_kpm.meas_type = meas_info.meas_type;

      // meas data
      memset(buffer, 0, sizeof(buffer));
      to_sql_string_kpm_meas_data(id, sql_str_kpm, msg->gran_period_ms, meas_data.incomplete_flag, ric_ind_frmt, timestamp, ue_meas_report_idx, buffer, 512);
      insert_db(db, buffer);

      // meas info
      for (size_t k = 0; k < meas_info.label_info_lst_len; k++) {
        label_info_lst_t label_info = meas_info.label_info_lst[k];
        if (*label_info.noLabel == TRUE_ENUM_VALUE)
          continue;
        memset(buffer, 0, sizeof(buffer));
        to_sql_string_kpm_meas_info(id, sql_str_kpm, label_info, ric_ind_frmt, timestamp, ue_meas_report_idx, buffer, 512);
        insert_db(db, buffer);
      }

      // combine meas data with meas info
      memset(buffer, 0, sizeof(buffer));
      to_sql_string_kpm_meas_data_info(id, sql_str_kpm, msg->gran_period_ms, meas_data.incomplete_flag, ric_ind_frmt, timestamp, ue_meas_report_idx, buffer, 512);
      insert_db(db, buffer);

    }
  }

}

void write_kpm_frm3_stats(sqlite3* db,
                          global_e2_node_id_t const* id,
                          format_ind_msg_e const ric_ind_frmt,
                          kpm_ind_msg_format_3_t const* msg,
                          uint64_t const timestamp)
{
  assert(db != NULL);
  assert(msg != NULL);

  char buffer[512] = {0};

  for (size_t i = 0; i < msg->ue_meas_report_lst_len; i++) {
    // ue_id_e2sm_t
    ue_id_e2sm_t ue_id_e2sm = msg->meas_report_per_ue[i].ue_meas_report_lst;
    if (ue_id_e2sm.type == GNB_UE_ID_E2SM || ue_id_e2sm.type == GNB_DU_UE_ID_E2SM || ue_id_e2sm.type == GNB_CU_UP_UE_ID_E2SM || ue_id_e2sm.type == ENB_UE_ID_E2SM) {
      memset(buffer, 0, sizeof(buffer));
      to_sql_string_kpm_ue_id_e2sm(id, ue_id_e2sm, ric_ind_frmt, ue_id_e2sm.type, timestamp, buffer, 512);
      insert_db(db, buffer);
    } else {
      printf("not supported ue_id_e2sm type\n");
    }

    // kpm_ind_msg_format_1_t
    kpm_ind_msg_format_1_t const *meas_report = &msg->meas_report_per_ue[i].ind_msg_format_1;
    write_kpm_frm1_stats(db, id, ric_ind_frmt, meas_report, timestamp, i);
  }

}

static
void write_kpm_hdr_frm1_stats(sqlite3* db,
                              global_e2_node_id_t const* id,
                              kpm_ind_hdr_t const* hdr)
{
  assert(db != NULL);
  assert(hdr != NULL);


  if (hdr->type == FORMAT_1_INDICATION_HEADER) {
    char buffer[512] = {0};
    to_mysql_string_kpm_hdr(id, hdr->type, hdr->kpm_ric_ind_hdr_format_1, buffer, 512);
    insert_db(db, buffer);
  } else {
    assert(0!=0 && "unknown KPM hdr format");
  }

}

void write_kpm_stats(sqlite3* db, global_e2_node_id_t const* id, kpm_ind_data_t const* ind)
{

  assert(db != NULL);
  assert(ind != NULL);

  write_kpm_hdr_frm1_stats(db, id, &ind->hdr);

  kpm_ind_msg_t const* msg = &ind->msg;
  uint64_t const timestamp = ind->hdr.kpm_ric_ind_hdr_format_1.collectStartTime;

  if (msg->type == FORMAT_1_INDICATION_MESSAGE) {
    write_kpm_frm1_stats(db, id, msg->type, &msg->frm_1, timestamp, 999);
  } else if (msg->type == FORMAT_2_INDICATION_MESSAGE) {
    assert(0!=0);
  } else if (msg->type == FORMAT_3_INDICATION_MESSAGE) {
    write_kpm_frm3_stats(db, id, msg->type, &msg->frm_3, timestamp);
  }

}

void init_db_sqlite3(sqlite3** db, db_params_t const* db_params)
{
  assert(db != NULL);
  assert(db_params != NULL);

  char db_filename[256] = {0};
  if (strlen(db_params->dir) && strlen(db_params->filename)) {
    int n = snprintf(db_filename, 255, "%s%s", db_params->dir, db_params->filename);
    assert(n < 256 && "Overflow");
  }
  printf("[SQLite3]: Filename = %s \n", db_filename);

  int const rc = sqlite3_open(db_filename, db);
  assert(rc != SQLITE_CANTOPEN && "SQLITE3 cannot open the directory. Does it already exist?");
  assert(rc == SQLITE_OK && "Error while creating the DB at /tmp/db_xapp");


  // Optimizations. Write Ahead Logging
  char* err_msg = NULL;
  int const rc_2 = sqlite3_exec(*db, "pragma journal_mode=wal" , 0, 0, &err_msg);
  assert(rc_2 == SQLITE_OK && "Error while setting the wal mode in sqlite3");

  int const rc_3 = sqlite3_exec(*db, "pragma synchronous=normal" , 0, 0, &err_msg);
  assert(rc_3 == SQLITE_OK && "Error while setting the syncronous mode to normal");


  //////
  // MAC
  //////
  create_mac_ue_table(*db);

  //////
  // RLC
  //////
  create_rlc_bearer_table(*db);

  //////
  // PDCP
  //////
  create_pdcp_bearer_table(*db);

  //////
  // SLICE
  //////
  create_slice_table(*db);
  create_ue_slice_table(*db);

  ////
  // GTP
  ////
  create_gtp_table(*db);
  // KPM
  ////
  create_kpm_table(*db);
}

void close_db_sqlite3(sqlite3* db)
{
  assert(db != NULL);
  int const rc = sqlite3_close(db);
  assert(rc == SQLITE_OK && "Error while closing the DB");
}

static
int rc_acc = 0;

void write_db_sqlite3(sqlite3* db, global_e2_node_id_t const* id, sm_ag_if_rd_t const* ag_rd)
{
  assert(db != NULL);
  assert(ag_rd != NULL);
  assert(ag_rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);

  sm_ag_if_rd_ind_t const* rd = &ag_rd->ind;
  assert(rd->type == MAC_STATS_V0   || rd->type == RLC_STATS_V0
      || rd->type == PDCP_STATS_V0  || rd->type == SLICE_STATS_V0
      || rd->type == KPM_STATS_V3_0 || rd->type == GTP_STATS_V0
      || rd->type == RAN_CTRL_STATS_V1_03);

  if(rd->type == MAC_STATS_V0){
    write_mac_stats(db, id, &rd->mac);
  } else if(rd->type == RLC_STATS_V0 ){
    write_rlc_stats(db, id, &rd->rlc);
  } else if( rd->type == PDCP_STATS_V0) {
    write_pdcp_stats(db, id, &rd->pdcp);
  } else if (rd->type == SLICE_STATS_V0) {
    write_slice_stats(db, id, &rd->slice);
  } else if (rd->type == GTP_STATS_V0) {
    write_gtp_stats(db, id, &rd->gtp);
  } else if (rd->type == KPM_STATS_V3_0) {
    write_kpm_stats(db, id, &rd->kpm.ind);
  } else if(rd->type ==  RAN_CTRL_STATS_V1_03){
    rc_acc++;
    if(rc_acc > 2048){
      printf("RAN Control sqlite not implemented\n");
      rc_acc = 0;
    }
  } else {
    assert(0!=0 && "Unknown statistics type received ");
  }
}

