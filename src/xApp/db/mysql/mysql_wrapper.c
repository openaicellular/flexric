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

#include "../db.h"
#include "mysql_wrapper.h"
#include "../../../util/time_now_us.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"


static void mysql_finish_with_error(MYSQL *conn)
{
  fprintf(stderr, "%s\n", mysql_error(conn));
  mysql_close(conn);
  exit(1);
}

static
void create_mac_ue_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS MAC_UE"))
    mysql_finish_with_error(conn);
  if(mysql_query(conn, "CREATE TABLE MAC_UE("
                       "tstamp BIGINT CHECK(tstamp > 0),"
                       "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                       "e2node_mcc INT,"
                       "e2node_mnc INT,"
                       "e2node_mnc_digit_len INT,"
                       "e2node_nb_id INT,"
                       "e2node_cu_du_id TEXT,"
                       "frame INT,"
                       "slot INT,"
                       "dl_aggr_tbs BIGINT CHECK(dl_aggr_tbs >= 0 AND dl_aggr_tbs < 18446744073709551615),"
                       "ul_aggr_tbs BIGINT CHECK(ul_aggr_tbs >= 0 AND ul_aggr_tbs < 18446744073709551615),"
                       "dl_aggr_bytes_sdus BIGINT CHECK(dl_aggr_bytes_sdus >= 0 AND dl_aggr_bytes_sdus < 18446744073709551615),"
                       "ul_aggr_bytes_sdus BIGINT CHECK(ul_aggr_bytes_sdus >= 0 AND ul_aggr_bytes_sdus < 18446744073709551615),"
                       "dl_curr_tbs BIGINT CHECK(dl_curr_tbs >= 0 AND dl_curr_tbs < 18446744073709551615),"
                       "ul_curr_tbs BIGINT CHECK(ul_curr_tbs >= 0 AND ul_curr_tbs < 18446744073709551615),"
                       "dl_sched_rb BIGINT CHECK(dl_sched_rb >= 0 AND dl_sched_rb < 18446744073709551615),"
                       "ul_sched_rb BIGINT CHECK(ul_sched_rb >= 0 AND ul_sched_rb < 18446744073709551615),"
                       "pusch_snr REAL CHECK(pusch_snr is NULL OR (pusch_snr >= 0 AND pusch_snr < 4294967296)),"
                       "pucch_snr REAL CHECK(pucch_snr is NULL OR (pucch_snr >= 0 AND pucch_snr < 4294967296)),"
                       "rnti BIGINT CHECK(rnti  >= 0 AND rnti < 4294967296),"
                       "dl_aggr_prb BIGINT CHECK(dl_aggr_prb >= 0 AND dl_aggr_prb < 4294967296),"
                       "ul_aggr_prb BIGINT CHECK(ul_aggr_prb >= 0 AND ul_aggr_prb < 4294967296),"
                       "dl_aggr_sdus BIGINT CHECK(dl_aggr_sdus >= 0 AND dl_aggr_sdus < 4294967296),"
                       "ul_aggr_sdus BIGINT CHECK(ul_aggr_sdus >= 0 AND ul_aggr_sdus < 4294967296),"
                       "dl_aggr_retx_prb BIGINT CHECK(dl_aggr_retx_prb >= 0 AND dl_aggr_retx_prb < 4294967296),"
                       "ul_aggr_retx_prb BIGINT CHECK(ul_aggr_retx_prb >= 0 AND ul_aggr_retx_prb < 4294967296),"
                       "wb_cqi INT CHECK(wb_cqi >= 0 AND wb_cqi < 256),"
                       "dl_mcs1 INT CHECK(dl_mcs1 >= 0 AND dl_mcs1 < 256),"
                       "ul_mcs1 INT CHECK(ul_mcs1 >= 0 AND ul_mcs1 < 256),"
                       "dl_mcs2 INT CHECK(dl_mcs2 >= 0 AND dl_mcs2 < 256),"
                       "ul_mcs2 INT CHECK(ul_mcs2 >= 0 AND ul_mcs2 < 256),"
                       "phr INT CHECK(phr > -24 AND phr < 41)," // −23 dB to +40 dB
                       "bsr BIGINT CHECK(bsr >= 0 AND bsr < 4294967296),"
                       "dl_bler REAL CHECK(dl_bler >= 0 AND dl_bler < 4294967296),"
                       "ul_bler REAL CHECK(ul_bler >= 0 AND ul_bler < 4294967296),"
                       "dl_num_harq INT CHECK(dl_num_harq >= 0 AND dl_num_harq < 5),"
                       "dl_harq_round0 BIGINT CHECK(dl_harq_round0 >= 0 AND dl_harq_round0 < 4294967296),"
                       "dl_harq_round1 BIGINT CHECK(dl_harq_round1 >= 0 AND dl_harq_round1 < 4294967296),"
                       "dl_harq_round2 BIGINT CHECK(dl_harq_round2 >= 0 AND dl_harq_round2 < 4294967296),"
                       "dl_harq_round3 BIGINT CHECK(dl_harq_round3 >= 0 AND dl_harq_round3 < 4294967296),"
                       "dlsch_errors BIGINT CHECK(dlsch_errors >= 0 AND dlsch_errors < 4294967296),"
                       "ul_num_harq INT CHECK(ul_num_harq >= 0 AND ul_num_harq < 5),"
                       "ul_harq_round0 BIGINT CHECK(ul_harq_round0 >= 0 AND ul_harq_round0 < 4294967296),"
                       "ul_harq_round1 BIGINT CHECK(ul_harq_round1 >= 0 AND ul_harq_round1 < 4294967296),"
                       "ul_harq_round2 BIGINT CHECK(ul_harq_round2 >= 0 AND ul_harq_round2 < 4294967296),"
                       "ul_harq_round3 BIGINT CHECK(ul_harq_round3 >= 0 AND ul_harq_round3 < 4294967296),"
                       "ulsch_errors BIGINT CHECK(ulsch_errors >= 0 AND ulsch_errors < 4294967296)"
                       ")"))
    mysql_finish_with_error(conn);

}

static
void create_rlc_bearer_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS RLC_bearer"))
    mysql_finish_with_error(conn);
  char* sql_rlc = "CREATE TABLE RLC_bearer("
                  "tstamp BIGINT CHECK(tstamp > 0),"
                  "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                  "e2node_mcc INT,"
                  "e2node_mnc INT,"
                  "e2node_mnc_digit_len INT,"
                  "e2node_nb_id INT,"
                  "e2node_cu_du_id TEXT,"
                  "txpdu_pkts BIGINT CHECK(txpdu_pkts >= 0 AND txpdu_pkts < 4294967296)," // 1 << 32 = 4294967296
                  "txpdu_bytes BIGINT CHECK(txpdu_bytes >= 0 AND txpdu_bytes < 4294967296),"
                  "txpdu_wt_ms BIGINT CHECK(txpdu_wt_ms >= 0 AND txpdu_wt_ms < 4294967296),"
                  "txpdu_dd_pkts BIGINT CHECK(txpdu_dd_pkts >= 0 AND txpdu_dd_pkts < 4294967296),"
                  "txpdu_dd_bytes BIGINT CHECK(txpdu_dd_bytes >= 0 AND txpdu_dd_bytes < 4294967296),"
                  "txpdu_retx_pkts BIGINT CHECK(txpdu_retx_pkts >= 0 AND txpdu_retx_pkts < 4294967296),"
                  "txpdu_retx_bytes BIGINT CHECK(txpdu_retx_bytes >= 0 AND txpdu_retx_bytes < 4294967296),"
                  "txpdu_segmented BIGINT CHECK(txpdu_segmented >= 0 AND txpdu_segmented < 4294967296),"
                  "txpdu_status_pkts BIGINT CHECK(txpdu_status_pkts >= 0 AND txpdu_status_pkts < 4294967296),"
                  "txpdu_status_bytes BIGINT CHECK(txpdu_status_bytes >= 0 AND txpdu_status_bytes < 4294967296),"
                  "txbuf_occ_bytes BIGINT CHECK(txbuf_occ_bytes >= 0 AND txbuf_occ_bytes < 4294967296),"
                  "txbuf_occ_pkts BIGINT CHECK(txbuf_occ_pkts >= 0 AND txbuf_occ_pkts < 4294967296),"
                  "rxpdu_pkts BIGINT CHECK(rxpdu_pkts >= 0 AND rxpdu_pkts < 4294967296),"
                  "rxpdu_bytes BIGINT CHECK(rxpdu_bytes >= 0 AND rxpdu_bytes < 4294967296),"
                  "rxpdu_dup_pkts BIGINT CHECK(rxpdu_dup_pkts >= 0 AND rxpdu_dup_pkts < 4294967296),"
                  "rxpdu_dup_bytes BIGINT CHECK(rxpdu_dup_bytes >= 0 AND rxpdu_dup_bytes < 4294967296),"
                  "rxpdu_dd_pkts BIGINT CHECK(rxpdu_dd_pkts >= 0 AND rxpdu_dd_pkts < 4294967296),"
                  "rxpdu_dd_bytes BIGINT CHECK(rxpdu_dd_bytes >= 0 AND rxpdu_dd_bytes < 4294967296),"
                  "rxpdu_ow_pkts BIGINT CHECK(rxpdu_ow_pkts >= 0 AND rxpdu_ow_pkts < 4294967296),"
                  "rxpdu_ow_bytes BIGINT CHECK(rxpdu_ow_bytes >= 0 AND rxpdu_ow_bytes < 4294967296),"
                  "rxpdu_status_pkts BIGINT CHECK(rxpdu_status_pkts >= 0 AND rxpdu_status_pkts < 4294967296),"
                  "rxpdu_status_bytes BIGINT CHECK(rxpdu_status_bytes >= 0 AND rxpdu_status_bytes < 4294967296),"
                  "rxbuf_occ_bytes BIGINT CHECK(rxbuf_occ_bytes >= 0 AND rxbuf_occ_bytes < 4294967296),"
                  "rxbuf_occ_pkts BIGINT CHECK(rxbuf_occ_pkts >= 0 AND rxbuf_occ_pkts < 4294967296),"
                  "txsdu_pkts BIGINT CHECK(txsdu_pkts >= 0 AND txsdu_pkts < 4294967296),"
                  "txsdu_bytes BIGINT CHECK(txsdu_bytes >= 0 AND txsdu_bytes < 18446744073709551615),"
                  "txsdu_avg_time_to_tx REAL CHECK(txsdu_avg_time_to_tx >= 0 AND txsdu_avg_time_to_tx < 4294967296),"
                  "txsdu_wt_us BIGINT CHECK(txsdu_wt_us >= 0 AND txsdu_wt_us < 4294967296),"
                  "rxsdu_pkts BIGINT CHECK(rxsdu_pkts >= 0 AND rxsdu_pkts < 4294967296),"
                  "rxsdu_bytes BIGINT CHECK(rxsdu_bytes >= 0 AND rxsdu_bytes < 18446744073709551615),"
                  "rxsdu_dd_pkts BIGINT CHECK(rxsdu_dd_pkts >= 0 AND rxsdu_dd_pkts < 4294967296),"
                  "rxsdu_dd_bytes BIGINT CHECK(rxsdu_dd_bytes >= 0 AND rxsdu_dd_bytes < 4294967296),"
                  "rnti BIGINT CHECK(rnti >= 0 AND rnti < 4294967296),"
                  "mode INT CHECK(mode >= 0 AND mode < 3),"
                  "rbid INT CHECK(rbid >= 0 AND rbid < 16)"
                  ")";

  if(mysql_query(conn, sql_rlc))
    mysql_finish_with_error(conn);

}

static
void create_pdcp_bearer_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS PDCP_bearer"))
    mysql_finish_with_error(conn);
  char* sql_pdcp = "CREATE TABLE PDCP_bearer("
                   "tstamp BIGINT CHECK(tstamp > 0),"
                   "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                   "e2node_mcc INT,"
                   "e2node_mnc INT,"
                   "e2node_mnc_digit_len INT,"
                   "e2node_nb_id INT,"
                   "e2node_cu_du_id TEXT,"
                   "txpdu_pkts BIGINT CHECK(txpdu_pkts >= 0 AND txpdu_pkts < 4294967296),"
                   "txpdu_bytes BIGINT CHECK(txpdu_bytes >=0 AND txpdu_bytes < 4294967296),"
                   "txpdu_sn BIGINT CHECK(txpdu_sn >=0 AND txpdu_sn < 4294967296),"
                   "rxpdu_pkts BIGINT CHECK(rxpdu_pkts >=0 AND rxpdu_pkts < 4294967296),"
                   "rxpdu_bytes BIGINT CHECK(rxpdu_bytes >=0 AND rxpdu_bytes < 4294967296),"
                   "rxpdu_sn BIGINT CHECK(rxpdu_sn >= 0 AND rxpdu_sn < 4294967296) ,"
                   "rxpdu_oo_pkts BIGINT CHECK(rxpdu_oo_pkts >= 0 AND rxpdu_oo_pkts < 4294967296),"
                   "rxpdu_oo_bytes BIGINT CHECK(rxpdu_oo_bytes >= 0 AND rxpdu_oo_bytes < 4294967296),"
                   "rxpdu_dd_pkts BIGINT CHECK(rxpdu_dd_pkts >= 0 AND rxpdu_dd_pkts < 4294967296),"
                   "rxpdu_dd_bytes BIGINT CHECK(rxpdu_dd_bytes >= 0 AND rxpdu_dd_bytes < 4294967296),"
                   "rxpdu_ro_count BIGINT CHECK(rxpdu_ro_count >= 0 AND rxpdu_ro_count < 4294967296),"
                   "txsdu_pkts BIGINT CHECK(txsdu_pkts >= 0 AND txsdu_pkts < 4294967296),"
                   "txsdu_bytes BIGINT CHECK(txsdu_bytes >= 0 AND txsdu_bytes < 4294967296),"
                   "rxsdu_pkts BIGINT CHECK(rxsdu_pkts >= 0 AND rxsdu_pkts < 4294967296),"
                   "rxsdu_bytes BIGINT CHECK(rxsdu_bytes >= 0 AND rxsdu_bytes < 4294967296),"
                   "rnti BIGINT CHECK(rnti >= 0 AND rnti <4294967296),"
                   "mode BIGINT CHECK(mode >= 0 AND mode < 4294967296),"
                   "rbid BIGINT CHECK(rbid >= 0 AND rbid < 4294967296)"
                   ");";

  if(mysql_query(conn, sql_pdcp))
    mysql_finish_with_error(conn);

}

static
void create_slice_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS SLICE"))
    mysql_finish_with_error(conn);
  char* sql_slice = "CREATE TABLE SLICE("
                    "tstamp BIGINT CHECK(tstamp > 0),"
                    "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                    "e2node_mcc INT,"
                    "e2node_mnc INT,"
                    "e2node_mnc_digit_len INT,"
                    "e2node_nb_id INT,"
                    "e2node_cu_du_id TEXT,"
                    "len_slices INT CHECK(len_slices  >= 0 AND len_slices < 4),"
                    "sched_name TEXT,"
                    "id INT CHECK(id >=0 AND id < 4294967296),"
                    "label TEXT,"
                    "slice_sched_algo TEXT,"
                    "slice_sched_algo_type TEXT,"
                    "ue_sched_algo TEXT,"
                    "slice_algo_param0 REAL CHECK(slice_algo_param0 is NULL OR (slice_algo_param0 >= 0 AND slice_algo_param0 < 4294967296)),"
                    "slice_algo_param1 REAL CHECK(slice_algo_param1 is NULL OR (slice_algo_param1 >= 0 AND slice_algo_param1 < 4294967296)),"
                    "slice_algo_param2 REAL CHECK(slice_algo_param2 is NULL OR (slice_algo_param2 >= 0 AND slice_algo_param2 < 4294967296))"
                    ");";

  if(mysql_query(conn, sql_slice))
    mysql_finish_with_error(conn);

}

static
void create_ue_slice_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS UE_SLICE"))
    mysql_finish_with_error(conn);
  char* sql_ue_slice = "CREATE TABLE UE_SLICE("
                       "tstamp BIGINT CHECK(tstamp > 0),"
                       "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                       "e2node_mcc INT,"
                       "e2node_mnc INT,"
                       "e2node_mnc_digit_len INT,"
                       "e2node_nb_id INT,"
                       "e2node_cu_du_id TEXT,"
                       "len_ue_slice BIGINT CHECK(len_ue_slice >= 0 AND len_ue_slice < 4294967296),"
                       "rnti INT CHECK(rnti >= -1 AND rnti < 65535),"
                       "dl_id BIGINT CHECK(dl_id >= -1 AND dl_id < 4294967296)"
                       ");";

  if(mysql_query(conn, sql_ue_slice))
    mysql_finish_with_error(conn);
}

static
void create_gtp_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS GTP_NGUT"))
    mysql_finish_with_error(conn);
  char* sql_gtp = "CREATE TABLE GTP_NGUT("
                  "tstamp BIGINT CHECK(tstamp > 0),"
                  "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                  "e2node_mcc INT,"
                  "e2node_mnc INT,"
                  "e2node_mnc_digit_len INT,"
                  "e2node_nb_id INT,"
                  "e2node_cu_du_id TEXT,"
                  "teidgnb BIGINT,"
                  "rnti INT CHECK(rnti >= -1 AND rnti < 65535),"
                  "qfi INT,"
                  "teidupf BIGINT"
                  ");";

  if(mysql_query(conn, sql_gtp))
    mysql_finish_with_error(conn);
}

static
void create_kpm_table(MYSQL* conn)
{
  assert(conn != NULL);

  if(mysql_query(conn, "DROP TABLE IF EXISTS KPM_HDR"))
    mysql_finish_with_error(conn);
  char* sql_kpm_hdr = "CREATE TABLE KPM_HDR("
                      "tstamp BIGINT CHECK(tstamp > 0),"
                      "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                      "e2node_mcc INT,"
                      "e2node_mnc INT,"
                      "e2node_mnc_digit_len INT,"
                      "e2node_nb_id INT,"
                      "e2node_cu_du_id TEXT,"
                      "format INT,"
                      "collectStartTime BIGINT,"
                      "fileformat_version TEXT,"
                      "sender_name TEXT,"
                      "sender_type TEXT,"
                      "vendor_name TEXT"
                      ");";

  if(mysql_query(conn, sql_kpm_hdr))
    mysql_finish_with_error(conn);


  if(mysql_query(conn, "DROP TABLE IF EXISTS KPM_IND_MEAS_DATA"))
    mysql_finish_with_error(conn);
  char* sql_kpm_meas_data = "CREATE TABLE KPM_IND_MEAS_DATA("
                            "tstamp BIGINT CHECK(tstamp > 0),"
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

  if(mysql_query(conn, sql_kpm_meas_data))
    mysql_finish_with_error(conn);

  if(mysql_query(conn, "DROP TABLE IF EXISTS KPM_IND_MEAS_INFO"))
    mysql_finish_with_error(conn);
  char* sql_kpm_meas_info_frm1 = "CREATE TABLE KPM_IND_MEAS_INFO("
                                 "tstamp BIGINT CHECK(tstamp > 0),"
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

  if(mysql_query(conn, sql_kpm_meas_info_frm1))
    mysql_finish_with_error(conn);

  if(mysql_query(conn, "DROP TABLE IF EXISTS KPM_IND_MEAS_DATA_INFO"))
    mysql_finish_with_error(conn);
  char* sql_kpm_meas_data_info = "CREATE TABLE KPM_IND_MEAS_DATA_INFO("
                                 "tstamp BIGINT CHECK(tstamp > 0),"
                                 "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 11),"
                                 "e2node_mcc INT,"
                                 "e2node_mnc INT,"
                                 "e2node_mnc_digit_len INT,"
                                 "e2node_nb_id INT,"
                                 "e2node_cu_du_id TEXT,"
                                 "ue_meas_report_idx INT,"
                                 "ric_ind_format INT,"
                                 "meas_data_len BIGINT,"
                                 "meas_data_idx BIGINT,"
                                 "meas_record_len BIGINT,"
                                 "meas_record_idx BIGINT,"
                                 "incompleteFlag TEXT,"
                                 "meas_info_len BIGINT,"
                                 "meas_info_idx BIGINT,"
                                 "gran_period_ms INT,"
                                 "meas_type TEXT," // To map the value to the measurement name or id
                                 "meas_name TEXT,"
                                 "meas_id INT,"
                                 "meas_value_type TEXT,"
                                 "meas_value_int INT,"
                                 "meas_value_real REAL,"
                                 "meas_value_no TEXT"
                                 ");";

  if(mysql_query(conn, sql_kpm_meas_data_info))
    mysql_finish_with_error(conn);

  if(mysql_query(conn, "DROP TABLE IF EXISTS KPM_IND_UE_ID_E2SM"))
    mysql_finish_with_error(conn);
  char* sql_kpm_ue_id_e2sm = "CREATE TABLE KPM_IND_UE_ID_E2SM("
                             "tstamp BIGINT CHECK(tstamp > 0),"
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
                             "amf_ue_ngap_id BIGINT,"
                             "guami_amf_region_id INT,"
                             "guami_amf_set_id INT,"
                             "guami_amf_ptr INT,"
                             "gnb_cu_ue_f1ap_lst_len BIGINT,"
                             "gnb_cu_ue_f1ap_lst INT,"
                             "gnb_cu_cp_ue_e1ap_lst_len BIGINT,"
                             "gnb_cu_cp_ue_e1ap_lst INT,"
                             "ran_ue_id BIGINT,"
                             "ng_ran_node_ue_xnap_id INT,"
                             /// enb.h ///
                             "mme_ue_s1ap_id INT,"
                             "guami_mme_group_id INT,"
                             "guami_mme_code INT,"
                             "enb_ue_x2ap_id BIGINT,"
                             "enb_ue_x2ap_id_extension BIGINT"
                             ");";

  if(mysql_query(conn, sql_kpm_ue_id_e2sm))
    mysql_finish_with_error(conn);
}

//static
//void insert_db(sqlite3* db, char const* sql)
//{
//  assert(db != NULL);
//  assert(sql != NULL);
//
//  char* err_msg = NULL;
//  int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
//  assert(rc == SQLITE_OK && "Error while inserting into the DB. Check the err_msg string for further info");
//}

static
int to_mysql_string_mac_ue(global_e2_node_id_t const* id, mac_ue_stats_impl_t* stats, int64_t tstamp, char* out, size_t out_len)
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

  int rc = snprintf(out, out_len,
                              "("
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
                              ")",
                              tstamp,
                              id->type,
                              id->plmn.mcc,
                              id->plmn.mnc,
                              id->plmn.mnc_digit_len,
                              id->nb_id.nb_id,
                              id->cu_du_id ? c_cu_du_id : c_null,
                              stats->frame,
                              stats->slot,
                              stats->dl_aggr_tbs,
                              stats->ul_aggr_tbs,
                              stats->dl_aggr_bytes_sdus,
                              stats->ul_aggr_bytes_sdus,
                              stats->dl_curr_tbs,
                              stats->ul_curr_tbs,
                              stats->dl_sched_rb,
                              stats->ul_sched_rb,
                              stats->pusch_snr,
                              stats->pucch_snr,
                              stats->rnti,
                              stats->dl_aggr_prb,
                              stats->ul_aggr_prb,
                              stats->dl_aggr_sdus,
                              stats->ul_aggr_sdus,
                              stats->dl_aggr_retx_prb,
                              stats->ul_aggr_retx_prb,
                              stats->wb_cqi,
                              stats->dl_mcs1,
                              stats->ul_mcs1,
                              stats->dl_mcs2,
                              stats->ul_mcs2,
                              stats->phr,
                              stats->bsr,
                              stats->dl_bler,
                              stats->ul_bler,
                              stats->dl_num_harq,
                              stats->dl_harq[0],
                              stats->dl_harq[1],
                              stats->dl_harq[2],
                              stats->dl_harq[3],
                              stats->dl_harq[4],
                              stats->ul_num_harq,
                              stats->ul_harq[0],
                              stats->ul_harq[1],
                              stats->ul_harq[2],
                              stats->ul_harq[3],
                              stats->ul_harq[4]);

  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_mysql_string_rlc_rb(global_e2_node_id_t const* id,rlc_radio_bearer_stats_t* rlc, int64_t tstamp, char* out, size_t out_len)
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

  int const rc = snprintf(out, out_len,
                                    "("
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
                                    ")",
                                    tstamp,
                                    id->type,
                                    id->plmn.mcc,
                                    id->plmn.mnc,
                                    id->plmn.mnc_digit_len,
                                    id->nb_id.nb_id,
                                    id->cu_du_id ? c_cu_du_id : c_null,
                                    rlc->txpdu_pkts,
                                    rlc->txpdu_bytes,
                                    rlc->txpdu_wt_ms,
                                    rlc->txpdu_dd_pkts,
                                    rlc->txpdu_dd_bytes,
                                    rlc->txpdu_retx_pkts,
                                    rlc->txpdu_retx_bytes,
                                    rlc->txpdu_segmented,
                                    rlc->txpdu_status_pkts,
                                    rlc->txpdu_status_bytes,
                                    rlc->txbuf_occ_bytes,
                                    rlc->txbuf_occ_pkts,
                                    rlc->rxpdu_pkts,
                                    rlc->rxpdu_bytes,
                                    rlc->rxpdu_dup_pkts,
                                    rlc->rxpdu_dup_bytes,
                                    rlc->rxpdu_dd_pkts,
                                    rlc->rxpdu_dd_bytes,
                                    rlc->rxpdu_ow_pkts,
                                    rlc->rxpdu_ow_bytes,
                                    rlc->rxpdu_status_pkts,
                                    rlc->rxpdu_status_bytes,
                                    rlc->rxbuf_occ_bytes,
                                    rlc->rxbuf_occ_pkts,
                                    rlc->txsdu_pkts,
                                    rlc->txsdu_bytes,
                                    rlc->txsdu_avg_time_to_tx,
                                    rlc->txsdu_wt_us,
                                    rlc->rxsdu_pkts,
                                    rlc->rxsdu_bytes,
                                    rlc->rxsdu_dd_pkts,
                                    rlc->rxsdu_dd_bytes,
                                    rlc->rnti,
                                    rlc->mode,
                                    rlc->rbid);
//  printf("("
//         " 0/ %ld,"// tstamp
//         " 1/ %d," //ngran_node
//         " 2/ %d," //mcc
//         " 3/ %d," //mnc
//         " 4/ %d," //mnc_digit_len
//         " 5/ %d," //nb_id
//         " 6/ '%s'," //cu_du_id
//         " 7/ %u," //rlc->txpdu_pkts
//         " 8/ %u," //rlc->txpdu_bytes
//         " 9/ %u," //rlc->txpdu_wt_ms
//         " 10/ %u," //rlc->txpdu_dd_pkts
//         " 11/ %u," //rlc->txpdu_dd_bytes
//         " 12/ %u," //rlc->txpdu_retx_pkts
//         " 13/ %u," //rlc->txpdu_retx_bytes
//         " 14/ %u," //rlc->txpdu_segmented
//         " 15/ %u," //rlc->txpdu_status_pkts
//         " 16/ %u," //rlc->txpdu_status_bytes
//         " 17/ %u," //rlc->txbuf_occ_bytes
//         " 18/ %u," //rlc->txbuf_occ_pkts
//         " 19/ %u," //rlc->rxpdu_pkts
//         " 20/ %u," //rlc->rxpdu_bytes
//         " 21/ %u," //rlc->rxpdu_dup_pkts
//         " 22/ %u," //rlc->rxpdu_dup_bytes
//         " 23/ %u," //rlc->rxpdu_dd_pkts
//         " 24/ %u," //rlc->rxpdu_dd_bytes
//         " 25/ %u," //rlc->rxpdu_ow_pkts
//         " 26/ %u," //rlc->rxpdu_ow_bytes
//         " 27/ %u," //rlc->rxpdu_status_pkts
//         " 28/ %u," //rlc->rxpdu_status_bytes
//         " 29/ %u," //rlc->rxbuf_occ_bytes
//         " 30/ %u," //rlc->rxbuf_occ_pkts
//         " 31/ %u," //rlc->txsdu_pkts
//         " 32/ %lu," //rlc->txsdu_bytes
//         " 33/ %.2f," //rlc->txsdu_avg_time_to_tx
//         " 34/ %u," //rlc->txsdu_wt_us
//         " 35/ %u," //rlc->rxsdu_pkts
//         " 36/ %lu," //rlc->rxsdu_bytes
//         " 37/ %u," //rlc->rxsdu_dd_pkts
//         " 38/ %u," //rlc->rxsdu_dd_bytes
//         " 39/ %u," //rlc->rnti
//         " 40/ %u,"  //rlc->mode
//         " 41/ %u"  //rlc->rbid
//         ")",
//         tstamp,
//         id->type,
//         id->plmn.mcc,
//         id->plmn.mnc,
//         id->plmn.mnc_digit_len,
//         id->nb_id.nb_id,
//         id->cu_du_id ? c_cu_du_id : c_null,
//         rlc->txpdu_pkts < 4294967296? 1:0,
//         rlc->txpdu_bytes < 4294967296? 1:0,
//         rlc->txpdu_wt_ms < 4294967296? 1:0,
//         rlc->txpdu_dd_pkts < 4294967296? 1:0,
//         rlc->txpdu_dd_bytes < 4294967296? 1:0,
//         rlc->txpdu_retx_pkts < 4294967296? 1:0,
//         rlc->txpdu_retx_bytes < 4294967296? 1:0,
//         rlc->txpdu_segmented < 4294967296? 1:0,
//         rlc->txpdu_status_pkts < 4294967296? 1:0,
//         rlc->txpdu_status_bytes < 4294967296? 1:0,
//         rlc->txbuf_occ_bytes < 4294967296? 1:0,
//         rlc->txbuf_occ_pkts < 4294967296? 1:0,
//         rlc->rxpdu_pkts < 4294967296? 1:0,
//         rlc->rxpdu_bytes < 4294967296? 1:0,
//         rlc->rxpdu_dup_pkts < 4294967296? 1:0,
//         rlc->rxpdu_dup_bytes < 4294967296? 1:0,
//         rlc->rxpdu_dd_pkts < 4294967296? 1:0,
//         rlc->rxpdu_dd_bytes < 4294967296? 1:0,
//         rlc->rxpdu_ow_pkts < 4294967296? 1:0,
//         rlc->rxpdu_ow_bytes < 4294967296? 1:0,
//         rlc->rxpdu_status_pkts < 4294967296? 1:0,
//         rlc->rxpdu_status_bytes < 4294967296? 1:0,
//         rlc->rxbuf_occ_bytes < 4294967296? 1:0,
//         rlc->rxbuf_occ_pkts < 4294967296? 1:0,
//         rlc->txsdu_pkts < 4294967296? 1:0,
//         rlc->txsdu_bytes < 18446744073709551615? 1:0,
//         rlc->txsdu_avg_time_to_tx < 4294967296? 1:0,
//         rlc->txsdu_wt_us < 4294967296? 1:0,
//         rlc->rxsdu_pkts < 4294967296? 1:0,
//         rlc->rxsdu_bytes < 18446744073709551615? 1:0,
//         rlc->rxsdu_dd_pkts < 4294967296? 1:0,
//         rlc->rxsdu_dd_bytes < 4294967296? 1:0,
//         rlc->rnti < 4294967296? 1:0,
//         rlc->mode,
//         rlc->rbid);

  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_mysql_string_pdcp_rb(global_e2_node_id_t const* id, pdcp_radio_bearer_stats_t* pdcp, int64_t tstamp, char* out, size_t out_len)
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
                    "("
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
                    ")",
                    tstamp,
                    id->type,
                    id->plmn.mcc,
                    id->plmn.mnc,
                    id->plmn.mnc_digit_len,
                    id->nb_id.nb_id,
                    id->cu_du_id ? c_cu_du_id : c_null,
                    pdcp->txpdu_pkts,
                    pdcp->txpdu_bytes,
                    pdcp->txpdu_sn,
                    pdcp->rxpdu_pkts,
                    pdcp->rxpdu_bytes,
                    pdcp->rxpdu_sn,
                    pdcp->rxpdu_oo_pkts,
                    pdcp->rxpdu_oo_bytes,
                    pdcp->rxpdu_dd_pkts,
                    pdcp->rxpdu_dd_bytes,
                    pdcp->rxpdu_ro_count,
                    pdcp->txsdu_pkts,
                    pdcp->txsdu_bytes,
                    pdcp->rxsdu_pkts,
                    pdcp->rxsdu_bytes,
                    pdcp->rnti,
                    pdcp->mode,
                    pdcp->rbid);
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_mysql_string_slice_rb(global_e2_node_id_t const* id, ul_dl_slice_conf_t const* slices, fr_slice_t const* s, int64_t tstamp, char* out, size_t out_len)
{
  assert(slices != NULL);
  assert(out != NULL);
  const size_t max = 2048;
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
                  "("
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
                  ")",
                  tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id,
                  id->cu_du_id ? c_cu_du_id : c_null,
                  0, sched_name, 0, c_null, c_null, c_null, c_null, 0.00, 0.00, 0.00);
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
                  "("
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
                  ")"
                  ,tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id,
                  id->cu_du_id ? c_cu_du_id : c_null,
                  slices->len_slices, c_null,
                  s->id, label, params_type, c_null, sched,
                  s->params.u.sta.pos_low, s->params.u.sta.pos_high, 0.00);
  } else if (s->params.type == SLICE_ALG_SM_V0_NVS) {
    strcat(params_type, "NVS");
    if (s->params.u.nvs.conf == SLICE_SM_NVS_V0_RATE) {
      strcat(params_type_conf, "RATE");
      rc = snprintf(out, out_len,
                    "("
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
                    ")",
                    tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id,
                    id->cu_du_id ? c_cu_du_id : c_null,
                    slices->len_slices, c_null,
                    s->id, label, params_type, params_type_conf, sched,
                    s->params.u.nvs.u.rate.u1.mbps_required, s->params.u.nvs.u.rate.u2.mbps_reference, 0.00);
    } else if (s->params.u.nvs.conf == SLICE_SM_NVS_V0_CAPACITY) {
      strcat(params_type_conf, "CAPACITY");
      rc = snprintf(out, out_len,
                    "("
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
                    ")",
                    tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id,
                    id->cu_du_id ? c_cu_du_id : c_null,
                    slices->len_slices, c_null,
                    s->id, label, params_type, params_type_conf, sched,
                    s->params.u.nvs.u.capacity.u.pct_reserved, 0.00, 0.00);
    }
  } else if (s->params.type == SLICE_ALG_SM_V0_EDF) {
    strcat(params_type, "EDF");
    rc = snprintf(out, out_len,
                  "("
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
                  ")",
                  tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id,
                  id->cu_du_id ? c_cu_du_id : c_null,
                  slices->len_slices, c_null,
                  s->id, label, params_type, c_null, sched,
                  s->params.u.edf.deadline,
                  s->params.u.edf.guaranteed_prbs,
                  s->params.u.edf.max_replenish);
  }
  assert(rc < (int) max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_mysql_string_ue_slice_rb(global_e2_node_id_t const* id, ue_slice_conf_t const* ues, ue_slice_assoc_t const* u, int64_t tstamp, char* out, size_t out_len)
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
                  "("
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
                  ")",
                  tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id,
                  id->cu_du_id ? c_cu_du_id : c_null,
                  ues->len_ue_slice, -1, -1);
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return rc;
  }

  rc = snprintf(out, out_len,
                "("
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
                ")",
                tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id.nb_id,
                id->cu_du_id ? c_cu_du_id : c_null,
                ues->len_ue_slice, u->rnti, u->dl_id);
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  return rc;
}

static
int to_mysql_string_gtp_NGUT(global_e2_node_id_t const* id,gtp_ngu_t_stats_t* gtp, int64_t tstamp, char* out, size_t out_len)
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
                          "("
                          "%ld," //tstamp
                          "%d," //ngran_node
                          "%d," //mcc
                          "%d," //mnc
                          "%d," //mnc_digit_len
                          "%d," //nb_id
                          "'%s'," //cu_du_id
                          "%u," //teidgnb
                          "%u," //rnti
                          "%u," //qfi
                          "%u" //teidupf
                          ")",
                          tstamp,
                          id->type,
                          id->plmn.mcc,
                          id->plmn.mnc,
                          id->plmn.mnc_digit_len,
                          id->nb_id.nb_id,
                          id->cu_du_id ? c_cu_du_id : c_null,
                          gtp->teidgnb,
                          gtp->rnti,
                          gtp->qfi,
                          gtp->teidupf
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
                           "("
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
                           ")"
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
} mysql_str_kpm_ind_frm1_t;

static
void to_mysql_string_kpm_meas_data(global_e2_node_id_t const* id,
                                   mysql_str_kpm_ind_frm1_t const sql_str_kpm,
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
                             "("
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
                             ")"
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
                            ,gran_period_ms ? *gran_period_ms: 0
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
                             "("
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
                             ")"
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
                            ,gran_period_ms ? *gran_period_ms: 0
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
                             "("
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
                             "%u,"  //gran_period_ms
                             "'%s',"  //meas_value_type
                             "%d,"    //meas_value_int
                             "%f,"    //meas_value_real
                             "'%s'"   //meas_value_no
                             ")"
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
                            ,gran_period_ms ? *gran_period_ms: 0
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
void to_mysql_string_kpm_meas_info(global_e2_node_id_t const* id,
                                   mysql_str_kpm_ind_frm1_t const sql_str_kpm,
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
                            "("
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
                            ")"
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
                            "("
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
                            ")"
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
void to_mysql_string_kpm_meas_data_info(global_e2_node_id_t const* id,
                                        mysql_str_kpm_ind_frm1_t const sql_str_kpm,
                                        uint32_t* const gran_period_ms,
                                        enum_value_e* const incomplete_flag,
                                        format_ind_msg_e const ric_ind_frmt,
                                        uint64_t const timestamp,
                                        size_t const ue_meas_report_idx,
                                        char* out,
                                        size_t out_len) {
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char *c_null = NULL;
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
    uint8_t* meas_name = calloc(sql_str_kpm.meas_type.name.len, sizeof(uint8_t));
    memcpy(meas_name, sql_str_kpm.meas_type.name.buf, sql_str_kpm.meas_type.name.len);
    int const rc = snprintf(out, max,
                            "("
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
                            ")"
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
                            ,sql_str_kpm.meas_type.name.len > 0 ? (char*)meas_name : "null"
                            ,c_value_type
                            ,int_value
                            ,real_value
                            ,no_value ? "TRUE":"FALSE"
                            );
    free(meas_name);
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return;
  } else if (sql_str_kpm.meas_type.type == ID_MEAS_TYPE) {
    int const rc = snprintf(out, max,
                            "("
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
                            ")"
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
void to_mysql_string_kpm_ue_id_e2sm(global_e2_node_id_t const* id,
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
                            "("
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
                            ")"
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
                            "("
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
                            ")"
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
                            "("
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
                            ")"
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
                            "("
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
                            ")"
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


int mac_count = 0;
int mac_stat_max = 50;
char mac_buffer[2048] = "INSERT INTO MAC_UE "
                        "("
                        "tstamp,"
                        "ngran_node,"
                        "e2node_mcc,"
                        "e2node_mnc,"
                        "e2node_mnc_digit_len,"
                        "e2node_nb_id,"
                        "e2node_cu_du_id,"
                        "frame,"
                        "slot,"
                        "dl_aggr_tbs,"
                        "ul_aggr_tbs,"
                        "dl_aggr_bytes_sdus,"
                        "ul_aggr_bytes_sdus,"
                        "dl_curr_tbs,"
                        "ul_curr_tbs,"
                        "dl_sched_rb,"
                        "ul_sched_rb,"
                        "pusch_snr,"
                        "pucch_snr,"
                        "rnti,"
                        "dl_aggr_prb,"
                        "ul_aggr_prb,"
                        "dl_aggr_sdus,"
                        "ul_aggr_sdus,"
                        "dl_aggr_retx_prb,"
                        "ul_aggr_retx_prb,"
                        "wb_cqi,"
                        "dl_mcs1,"
                        "ul_mcs1,"
                        "dl_mcs2,"
                        "ul_mcs2,"
                        "phr,"
                        "bsr,"
                        "dl_bler,"
                        "ul_bler,"
                        "dl_num_harq,"
                        "dl_harq_round0,"
                        "dl_harq_round1,"
                        "dl_harq_round2,"
                        "dl_harq_round3,"
                        "dlsch_errors,"
                        "ul_num_harq,"
                        "ul_harq_round0,"
                        "ul_harq_round1,"
                        "ul_harq_round2,"
                        "ul_harq_round3,"
                        "ulsch_errors"
                        ") "
                        "VALUES";
char mac_temp[16384] = "";
static
void write_mac_stats(MYSQL* conn, global_e2_node_id_t const* id, mac_ind_data_t const* ind) {
  assert(conn != NULL);
  assert(ind != NULL);

  mac_ind_msg_t const *ind_msg_mac = &ind->msg;

  for (size_t i = 0; i < ind_msg_mac->len_ue_stats; ++i) {
    char buffer[2048] = "";
    int pos = strlen(buffer);
    if (mac_count == 0)
      strcat(mac_temp, mac_buffer);
    mac_count += 1;
    pos += to_mysql_string_mac_ue(id, &ind_msg_mac->ue_stats[i], ind_msg_mac->tstamp, buffer + pos, 2048 - pos);
    if (mac_count < mac_stat_max) {
      //printf("%d add ,\n", mac_count);
      strcat(buffer, ",");
      strcat(mac_temp, buffer);
    } else {
      //printf("%d add ;\n", mac_count);
      mac_count = 0;
      strcat(mac_temp, buffer);
      strcat(mac_temp, ";");
      //for(size_t i = 0; i < strlen(mac_temp); i++)
      //  printf("%c", mac_temp[i]);
      //printf("\n");
      //int64_t st = time_now_us();
      if (mysql_query(conn, mac_temp))
        mysql_finish_with_error(conn);
      //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
      strcpy(mac_temp,"");
    }
  }

}

int rlc_count = 0;
int rlc_stat_max = 50;
char rlc_buffer[2048] = "INSERT INTO RLC_bearer "
                    "("
                    "tstamp,"
                    "ngran_node,"
                    "e2node_mcc,"
                    "e2node_mnc,"
                    "e2node_mnc_digit_len,"
                    "e2node_nb_id,"
                    "e2node_cu_du_id,"
                    "txpdu_pkts,"
                    "txpdu_bytes,"
                    "txpdu_wt_ms,"
                    "txpdu_dd_pkts,"
                    "txpdu_dd_bytes,"
                    "txpdu_retx_pkts,"
                    "txpdu_retx_bytes,"
                    "txpdu_segmented,"
                    "txpdu_status_pkts,"
                    "txpdu_status_bytes,"
                    "txbuf_occ_bytes,"
                    "txbuf_occ_pkts,"
                    "rxpdu_pkts,"
                    "rxpdu_bytes,"
                    "rxpdu_dup_pkts,"
                    "rxpdu_dup_bytes,"
                    "rxpdu_dd_pkts,"
                    "rxpdu_dd_bytes,"
                    "rxpdu_ow_pkts,"
                    "rxpdu_ow_bytes,"
                    "rxpdu_status_pkts,"
                    "rxpdu_status_bytes,"
                    "rxbuf_occ_bytes,"
                    "rxbuf_occ_pkts,"
                    "txsdu_pkts,"
                    "txsdu_bytes,"
                    "txsdu_avg_time_to_tx,"
                    "txsdu_wt_us,"
                    "rxsdu_pkts,"
                    "rxsdu_bytes,"
                    "rxsdu_dd_pkts,"
                    "rxsdu_dd_bytes,"
                    "rnti,"
                    "mode,"
                    "rbid"
                    ") "
                    "VALUES";
char rlc_temp[16384] = "";
static
void write_rlc_stats(MYSQL* conn, global_e2_node_id_t const* id, rlc_ind_data_t const* ind)
{
  assert(conn != NULL);
  assert(ind != NULL);

  rlc_ind_msg_t const* ind_msg_rlc = &ind->msg;

  for(size_t i = 0; i < ind_msg_rlc->len; ++i){
    char buffer[2048] = "";
    int pos = strlen(buffer);
    if (rlc_count == 0)
      strcat(rlc_temp, rlc_buffer);
    rlc_count += 1;
    pos += to_mysql_string_rlc_rb(id, &ind_msg_rlc->rb[i], ind_msg_rlc->tstamp, buffer + pos, 2048 - pos);
    if (rlc_count < rlc_stat_max) {
      //printf("%d add ,\n", rlc_count);
      strcat(buffer, ",");
      strcat(rlc_temp, buffer);
    } else {
      //printf("%d add ;\n", rlc_count);
      rlc_count = 0;
      strcat(rlc_temp, buffer);
      strcat(rlc_temp, ";");
      //for(size_t i = 0; i < strlen(rlc_temp); i++)
      //  printf("%c", rlc_temp[i]);
      //printf("\n");
      //int64_t st = time_now_us();
      if (mysql_query(conn, rlc_temp))
        mysql_finish_with_error(conn);
      //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
      strcpy(rlc_temp,"");
    }
  }

}

int pdcp_count = 0;
int pdcp_stat_max = 50;
char pdcp_buffer[2048] = "INSERT INTO PDCP_bearer "
                        "("
                        "tstamp,"
                        "ngran_node,"
                        "e2node_mcc,"
                        "e2node_mnc,"
                        "e2node_mnc_digit_len,"
                        "e2node_nb_id,"
                        "e2node_cu_du_id,"
                        "txpdu_pkts,"
                        "txpdu_bytes,"
                        "txpdu_sn,"
                        "rxpdu_pkts,"
                        "rxpdu_bytes,"
                        "rxpdu_sn,"
                        "rxpdu_oo_pkts,"
                        "rxpdu_oo_bytes,"
                        "rxpdu_dd_pkts,"
                        "rxpdu_dd_bytes,"
                        "rxpdu_ro_count,"
                        "txsdu_pkts,"
                        "txsdu_bytes,"
                        "rxsdu_pkts,"
                        "rxsdu_bytes,"
                        "rnti,"
                        "mode,"
                        "rbid"
                        ") "
                        "VALUES";
char pdcp_temp[16384] = "";
static
void write_pdcp_stats(MYSQL* conn, global_e2_node_id_t const* id, pdcp_ind_data_t const* ind)
{
  assert(conn != NULL);
  assert(ind != NULL);

  pdcp_ind_msg_t const* ind_msg_pdcp = &ind->msg;

  for(size_t i = 0; i < ind_msg_pdcp->len; ++i){
    char buffer[2048] = "";
    int pos = strlen(buffer);
    if (pdcp_count == 0)
      strcat(pdcp_temp, pdcp_buffer);
    pdcp_count += 1;
    pos += to_mysql_string_pdcp_rb(id, &ind_msg_pdcp->rb[i], ind_msg_pdcp->tstamp, buffer + pos, 2048 - pos);
    if (pdcp_count < pdcp_stat_max) {
      //printf("%d add ,\n", pdcp_count);
      strcat(buffer, ",");
      strcat(pdcp_temp, buffer);
    } else {
      //printf("%d add ;\n", pdcp_count);
      pdcp_count = 0;
      strcat(pdcp_temp, buffer);
      strcat(pdcp_temp, ";");
      //for(size_t i = 0; i < strlen(pdcp_temp); i++)
      //  printf("%c", pdcp_temp[i]);
      //printf("\n");
      //int64_t st = time_now_us();
      if (mysql_query(conn, pdcp_temp))
        mysql_finish_with_error(conn);
      //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
      strcpy(pdcp_temp,"");
    }
  }

}

int slice_count = 0;
int slice_stat_max = 20;
char slice_buffer[2048] = "INSERT INTO SLICE "
                          "("
                          "tstamp,"
                          "ngran_node,"
                          "e2node_mcc,"
                          "e2node_mnc,"
                          "e2node_mnc_digit_len,"
                          "e2node_nb_id,"
                          "e2node_cu_du_id,"
                          "len_slices,"
                          "sched_name,"
                          "id,"
                          "label,"
                          "slice_sched_algo,"
                          "slice_sched_algo_type,"
                          "ue_sched_algo,"
                          "slice_algo_param0,"
                          "slice_algo_param1,"
                          "slice_algo_param2"
                          ") "
                          "VALUES";
char slice_temp[16384] = "";
static
void write_slice_conf_stats(MYSQL* conn, global_e2_node_id_t const* id, int64_t tstamp, slice_conf_t const* slice_conf)
{

  ul_dl_slice_conf_t const* dlslices = &slice_conf->dl;
  if (dlslices->len_slices > 0) {
    for(size_t i = 0; i < dlslices->len_slices; ++i) {
      fr_slice_t const* s = &dlslices->slices[i];
      char buffer[4096] = {0};
      int pos = strlen(buffer);
      if (slice_count == 0)
        strcat(slice_temp, slice_buffer);
      slice_count += 1;
      pos += to_mysql_string_slice_rb(id, dlslices, s, tstamp, buffer + pos, 4096 - pos);
      if (slice_count < slice_stat_max) {
        //printf("len_slice > 0: %d add ,\n", slice_count);
        strcat(buffer, ",");
        strcat(slice_temp, buffer);
      } else {
        //printf("len_slice > 0: %d add ;\n", slice_count);
        slice_count = 0;
        strcat(slice_temp, buffer);
        strcat(slice_temp, ";");
        //printf("len_slice > 0: ");
        //for(size_t i = 0; i < strlen(slice_temp); i++)
        //  printf("%c", slice_temp[i]);
        //printf("\n");
        //int64_t st = time_now_us();
        if (mysql_query(conn, slice_temp))
          mysql_finish_with_error(conn);
        //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
        strcpy(slice_temp,"");
      }
    }
  } else {
    char buffer[4096] = {0};
    int pos = strlen(buffer);
    if (slice_count == 0)
      strcat(slice_temp, slice_buffer);
    slice_count += 1;
    pos += to_mysql_string_slice_rb(id, dlslices, NULL, tstamp, buffer + pos, 4096 - pos);
    if (slice_count < slice_stat_max) {
      //printf("len_slice = 0: %d add ,\n", slice_count);
      strcat(buffer, ",");
      strcat(slice_temp, buffer);
    } else {
      //printf("len_slice = 0: %d add ;\n", slice_count);
      slice_count = 0;
      strcat(slice_temp, buffer);
      strcat(slice_temp, ";");
      //printf("len_slice = 0: ");
      //for(size_t i = 0; i < strlen(slice_temp); i++)
      //  printf("%c", slice_temp[i]);
      //printf("\n");
      //int64_t st = time_now_us();
      if (mysql_query(conn, slice_temp))
        mysql_finish_with_error(conn);
      //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
      strcpy(slice_temp,"");
    }
  }

  // TODO: Process uplink slice stats

}

int ue_slice_count = 0;
int ue_slice_stat_max = 20;
char ue_slice_buffer[2048] = "INSERT INTO UE_SLICE "
                             "("
                             "tstamp,"
                             "ngran_node,"
                             "e2node_mcc,"
                             "e2node_mnc,"
                             "e2node_mnc_digit_len,"
                             "e2node_nb_id,"
                             "e2node_cu_du_id,"
                             "len_ue_slice,"
                             "rnti,"
                             "dl_id"
                             ") "
                             "VALUES";
char ue_slice_temp[16384] = "";
static
void write_ue_slice_conf_stats(MYSQL* conn, global_e2_node_id_t const* id, int64_t tstamp, ue_slice_conf_t const* ue_slice_conf)
{

  if (ue_slice_conf->len_ue_slice > 0) {
    for(uint32_t j = 0; j < ue_slice_conf->len_ue_slice; ++j) {
      ue_slice_assoc_t *u = &ue_slice_conf->ues[j];
      char buffer[4096] = {0};
      int pos = strlen(buffer);
      if (ue_slice_count == 0)
        strcat(ue_slice_temp, ue_slice_buffer);
      ue_slice_count += 1;
      pos += to_mysql_string_ue_slice_rb(id, ue_slice_conf, u, tstamp, buffer + pos, 2048 - pos);
      if (ue_slice_count < ue_slice_stat_max) {
        //printf("len_ue_slice > 0: %d add ,\n", ue_slice_count);
        strcat(buffer, ",");
        strcat(ue_slice_temp, buffer);
      } else {
        //printf("len_ue_slice > 0: %d add ;\n", ue_slice_count);
        ue_slice_count = 0;
        strcat(ue_slice_temp, buffer);
        strcat(ue_slice_temp, ";");
        //printf("len_ue_slice > 0: ");
        //for(size_t i = 0; i < strlen(ue_slice_temp); i++)
        //  printf("%c", ue_slice_temp[i]);
        //printf("\n");
        //int64_t st = time_now_us();
        if (mysql_query(conn, ue_slice_temp))
          mysql_finish_with_error(conn);
        //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
        strcpy(ue_slice_temp,"");
      }
    }
  } else {
    char buffer[4096] = {0};
    int pos = strlen(buffer);
    if (ue_slice_count == 0)
      strcat(ue_slice_temp, ue_slice_buffer);
    ue_slice_count += 1;
    pos += to_mysql_string_ue_slice_rb(id, ue_slice_conf, NULL, tstamp, buffer + pos, 2048 - pos);
    if (ue_slice_count < ue_slice_stat_max) {
      //printf("len_ue_slice = 0: %d add ,\n", ue_slice_count);
      strcat(buffer, ",");
      strcat(ue_slice_temp, buffer);
    } else {
      //printf("len_ue_slice = 0: %d add ;\n", ue_slice_count);
      ue_slice_count = 0;
      strcat(ue_slice_temp, buffer);
      strcat(ue_slice_temp, ";");
      //printf("len_ue_slice = 0: ");
      //for(size_t i = 0; i < strlen(ue_slice_temp); i++)
      //  printf("%c", ue_slice_temp[i]);
      //printf("\n");
      //int64_t st = time_now_us();
      if (mysql_query(conn, ue_slice_temp))
        mysql_finish_with_error(conn);
      //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
      strcpy(ue_slice_temp,"");
    }
  }

}

static
void write_slice_stats(MYSQL* conn, global_e2_node_id_t const* id, slice_ind_data_t const* ind)
{
  assert(conn != NULL);
  assert(ind != NULL);

  slice_ind_msg_t const* ind_msg_slice = &ind->msg;

  write_slice_conf_stats(conn, id, ind_msg_slice->tstamp, &ind_msg_slice->slice_conf);
  write_ue_slice_conf_stats(conn, id, ind_msg_slice->tstamp, &ind_msg_slice->ue_slice_conf);

}

int gtp_count = 0;
int gtp_stat_max = 50;
char gtp_buffer[2048] = "INSERT INTO GTP_NGUT "
                         "("
                         "tstamp,"
                         "ngran_node,"
                         "e2node_mcc,"
                         "e2node_mnc,"
                         "e2node_mnc_digit_len,"
                         "e2node_nb_id,"
                         "e2node_cu_du_id,"
                         "teidgnb,"
                         "rnti,"
                         "qfi,"
                         "teidupf"
                         ") "
                         "VALUES";
char gtp_temp[16384] = "";
static
void write_gtp_stats(MYSQL* conn, global_e2_node_id_t const* id, gtp_ind_data_t const* ind)
{
  assert(conn != NULL);
  assert(ind != NULL);

  gtp_ind_msg_t const* ind_msg_gtp = &ind->msg;

  for(size_t i = 0; i < ind_msg_gtp->len; ++i){
    char buffer[2048] = "";
    int pos = strlen(buffer);
    if (gtp_count == 0)
      strcat(gtp_temp, gtp_buffer);
    gtp_count += 1;
    pos += to_mysql_string_gtp_NGUT(id, &ind_msg_gtp->ngut[i], ind_msg_gtp->tstamp, buffer + pos, 2048 - pos);
    if (gtp_count < gtp_stat_max) {
      //printf("%d add ,\n", gtp_count);
      strcat(buffer, ",");
      strcat(gtp_temp, buffer);
    } else {
      //printf("%d add ;\n", gtp_count);
      gtp_count = 0;
      strcat(gtp_temp, buffer);
      strcat(gtp_temp, ";");
      //for(size_t i = 0; i < strlen(gtp_temp); i++)
      //  printf("%c", gtp_temp[i]);
      //printf("\n");
      //int64_t st = time_now_us();
      if (mysql_query(conn, gtp_temp))
        mysql_finish_with_error(conn);
      //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
      strcpy(gtp_temp,"");
    }
  }

}

// kpm meas data
int kpm_meas_data_count = 0;
int kpm_meas_data_max = 50;
char kpm_buffer_meas_data[2048] = "INSERT INTO KPM_IND_MEAS_DATA "
                                  "("
                                  "tstamp,"
                                  "ngran_node,"
                                  "e2node_mcc,"
                                  "e2node_mnc,"
                                  "e2node_mnc_digit_len,"
                                  "e2node_nb_id,"
                                  "e2node_cu_du_id,"
                                  "ue_meas_report_idx,"
                                  "ric_ind_format,"
                                  "meas_data_len,"
                                  "meas_data_idx,"
                                  "meas_record_len,"
                                  "meas_record_idx,"
                                  "incompleteFlag,"
                                  "gran_period_ms,"
                                  "meas_value_type,"
                                  "meas_value_int,"
                                  "meas_value_real,"
                                  "meas_value_no"
                                  ") "
                                  "VALUES";
char kpm_meas_data_temp[16384] = "";
// kpm meas info
int kpm_meas_info_count = 0;
int kpm_meas_info_max = 50;
char kpm_buffer_meas_info[2048] = "INSERT INTO KPM_IND_MEAS_INFO "
                                  "("
                                  "tstamp,"
                                  "ngran_node,"
                                  "e2node_mcc,"
                                  "e2node_mnc,"
                                  "e2node_mnc_digit_len,"
                                  "e2node_nb_id,"
                                  "e2node_cu_du_id,"
                                  "ue_meas_report_idx,"
                                  "ric_ind_format,"
                                  "meas_info_len,"
                                  "meas_info_idx,"
                                  "meas_type,"
                                  "meas_name,"
                                  "meas_id,"
                                  "label_info_len,"
                                  "label_info_idx,"
                                  "noLabel,"
                                  "plmn_id_mcc,"
                                  "plmn_id_mnc,"
                                  "plmn_id_mnc_digit_len,"
                                  "sliceID_sST,"
                                  "sliceID_sD,"
                                  "fiveQI,"
                                  "qFI,"
                                  "qCI,"
                                  "qCImax,"
                                  "qCImin,"
                                  "aRPmax,"
                                  "aRPmin,"
                                  "bitrateRange,"
                                  "layerMU_MIMO,"
                                  "sUM,"
                                  "distBinX,"
                                  "distBinY,"
                                  "distBinZ,"
                                  "preLabelOverride,"
                                  "startEndInd,"
                                  "min,"
                                  "max,"
                                  "avg,"
                                  "ssbIndex,"
                                  "nonGoB_beamformModeIndex,"
                                  "mimoModeIndex"
                                  ") "
                                  "VALUES";
char kpm_meas_info_temp[16384] = "";
// combine meas data with meas info
int kpm_meas_data_info_count = 0;
int kpm_meas_data_info_max = 50;
char kpm_buffer_meas_data_info[2048] = "INSERT INTO KPM_IND_MEAS_DATA_INFO "
                                  "("
                                  "tstamp,"
                                  "ngran_node,"
                                  "e2node_mcc,"
                                  "e2node_mnc,"
                                  "e2node_mnc_digit_len,"
                                  "e2node_nb_id,"
                                  "e2node_cu_du_id,"
                                  "ue_meas_report_idx,"
                                  "ric_ind_format,"
                                  "meas_data_len,"
                                  "meas_data_idx,"
                                  "meas_record_len,"
                                  "meas_record_idx,"
                                  "incompleteFlag,"
                                  "meas_info_len,"
                                  "meas_info_idx,"
                                  "gran_period_ms,"
                                  "meas_type," // To map the value to the measurement name or id
                                  "meas_name,"
                                  "meas_id,"
                                  "meas_value_type,"
                                  "meas_value_int,"
                                  "meas_value_real,"
                                  "meas_value_no"
                                  ") "
                                  "VALUES";
char kpm_meas_data_info_temp[16384] = "";
static void write_kpm_frm1_stats(MYSQL* conn,
                                 global_e2_node_id_t const* id,
                                 format_ind_msg_e const ric_ind_frmt,
                                 kpm_ind_msg_format_1_t const* msg,
                                 uint64_t const timestamp,
                                 size_t const ue_meas_report_idx)
{
  assert(conn != NULL);
  assert(msg != NULL);

  for (size_t i = 0; i < msg->meas_data_lst_len; i++) {
    mysql_str_kpm_ind_frm1_t sql_str_kpm = {0};
    sql_str_kpm.meas_data_len = msg->meas_data_lst_len;
    sql_str_kpm.meas_data_idx = i;

    meas_data_lst_t meas_data = msg->meas_data_lst[i];
    if (msg->meas_info_lst_len != meas_data.meas_record_len) {
      printf("meas_info_lst_len != meas_record_len, cannot write kpm ind frm1 to MySQL db\n");
      break;
    }

    sql_str_kpm.meas_record_len = meas_data.meas_record_len;
    sql_str_kpm.meas_info_len = msg->meas_info_lst_len;
    for (size_t j = 0; j < meas_data.meas_record_len; j++) {
      meas_record_lst_t meas_record = meas_data.meas_record_lst[j];
      sql_str_kpm.meas_record_idx = j;
      if (meas_record.value == INTEGER_MEAS_VALUE) {
        sql_str_kpm.meas_record.value = INTEGER_MEAS_VALUE;
        sql_str_kpm.meas_record.int_val = meas_record.int_val;
      } else if (meas_record.value == REAL_MEAS_VALUE) {
        sql_str_kpm.meas_record.value = REAL_MEAS_VALUE;
        sql_str_kpm.meas_record.real_val = meas_record.real_val;
      } else if (meas_record.value == NO_VALUE_MEAS_VALUE) {
        sql_str_kpm.meas_record.value = NO_VALUE_MEAS_VALUE;
        sql_str_kpm.meas_record.no_value = meas_record.no_value;
      } else {
        assert(0!=0 && "unknown meas record type");
      }

      meas_info_format_1_lst_t meas_info = msg->meas_info_lst[j];
      sql_str_kpm.meas_info_idx = j;
      sql_str_kpm.meas_type = meas_info.meas_type;

      // meas data
      char buffer[2048] = "";
      if (kpm_meas_data_count == 0)
        strcat(kpm_meas_data_temp, kpm_buffer_meas_data);
      kpm_meas_data_count += 1;
      to_mysql_string_kpm_meas_data(id, sql_str_kpm, msg->gran_period_ms, meas_data.incomplete_flag, ric_ind_frmt, timestamp, ue_meas_report_idx, buffer, 512);
      if (kpm_meas_data_count < kpm_meas_data_max) {
        strcat(buffer, ",");
        strcat(kpm_meas_data_temp, buffer);
      } else {
        strcat(kpm_meas_data_temp, buffer);
        strcat(kpm_meas_data_temp, ";");
        if (mysql_query(conn, kpm_meas_data_temp))
          mysql_finish_with_error(conn);
        strcpy(kpm_meas_data_temp,"");
        kpm_meas_data_count = 0;
      }

      // meas info
      for (size_t k = 0; k < meas_info.label_info_lst_len; k++) {
        label_info_lst_t label_info = meas_info.label_info_lst[k];
        if (*label_info.noLabel == TRUE_ENUM_VALUE)
          continue;
        char buffer2[2048] = "";
        if (kpm_meas_info_count == 0)
          strcat(kpm_meas_info_temp, kpm_buffer_meas_info);
        kpm_meas_info_count += 1;
        to_mysql_string_kpm_meas_info(id, sql_str_kpm, label_info, ric_ind_frmt, timestamp, ue_meas_report_idx, buffer2, 512);
        if (kpm_meas_info_count < kpm_meas_info_max) {
          strcat(buffer2, ",");
          strcat(kpm_meas_info_temp, buffer2);
        } else {
          strcat(kpm_meas_info_temp, buffer2);
          strcat(kpm_meas_info_temp, ";");
          if (mysql_query(conn, kpm_meas_info_temp))
            mysql_finish_with_error(conn);
          strcpy(kpm_meas_info_temp,"");
          kpm_meas_info_count = 0;
        }
      }

      // combine meas data with meas info
      char buffer3[2048] = "";
      if (kpm_meas_data_info_count == 0)
        strcat(kpm_meas_data_info_temp, kpm_buffer_meas_data_info);
      kpm_meas_data_info_count += 1;
      to_mysql_string_kpm_meas_data_info(id, sql_str_kpm, msg->gran_period_ms, meas_data.incomplete_flag, ric_ind_frmt, timestamp, ue_meas_report_idx, buffer3, 512);
      if (kpm_meas_data_info_count < kpm_meas_data_info_max) {
        strcat(buffer3, ",");
        strcat(kpm_meas_data_info_temp, buffer3);
      } else {
        strcat(kpm_meas_data_info_temp, buffer3);
        strcat(kpm_meas_data_info_temp, ";");
        if (mysql_query(conn, kpm_meas_data_info_temp))
          mysql_finish_with_error(conn);
        strcpy(kpm_meas_data_info_temp,"");
        kpm_meas_data_info_count = 0;
      }
    }
  }
}

// ue_id_e2sm
int kpm_ue_id_e2sm_count = 0;
int kpm_ue_id_e2sm_max = 50;
char kpm_buffer_ue_id_e2sm[2048] = "INSERT INTO KPM_IND_UE_ID_E2SM "
                                   "("
                                   "tstamp,"
                                   "ngran_node,"
                                   "e2node_mcc,"
                                   "e2node_mnc,"
                                   "e2node_mnc_digit_len,"
                                   "e2node_nb_id,"
                                   "e2node_cu_du_id,"
                                   "ric_ind_format,"
                                   "ue_id_e2sm_type,"
                                   "guami_plmn_id_mcc,"
                                   "guami_plmn_id_mnc,"
                                   "guami_plmn_id_mnc_digit_len,"
                                   /// gnb.h ///
                                   "amf_ue_ngap_id,"
                                   "guami_amf_region_id,"
                                   "guami_amf_set_id,"
                                   "guami_amf_ptr,"
                                   "gnb_cu_ue_f1ap_lst_len,"
                                   "gnb_cu_ue_f1ap_lst,"
                                   "gnb_cu_cp_ue_e1ap_lst_len,"
                                   "gnb_cu_cp_ue_e1ap_lst,"
                                   "ran_ue_id,"
                                   "ng_ran_node_ue_xnap_id,"
                                   /// enb.h ///
                                   "mme_ue_s1ap_id,"
                                   "guami_mme_group_id,"
                                   "guami_mme_code,"
                                   "enb_ue_x2ap_id,"
                                   "enb_ue_x2ap_id_extension"
                                   ") "
                                   "VALUES";
char kpm_ue_id_e2sm_temp[16384] = "";
static
void write_kpm_frm3_stats(MYSQL* conn,
                          global_e2_node_id_t const* id,
                          format_ind_msg_e const ric_ind_frmt,
                          kpm_ind_msg_format_3_t const* msg,
                          uint64_t const timestamp)
{
  assert(conn != NULL);
  assert(msg != NULL);

  for (size_t i = 0; i < msg->ue_meas_report_lst_len; i++) {
    // ue_id_e2sm_t
    ue_id_e2sm_t ue_id_e2sm = msg->meas_report_per_ue[i].ue_meas_report_lst;
    if (ue_id_e2sm.type == GNB_UE_ID_E2SM || ue_id_e2sm.type == GNB_DU_UE_ID_E2SM || ue_id_e2sm.type == GNB_CU_UP_UE_ID_E2SM || ue_id_e2sm.type == ENB_UE_ID_E2SM) {
      char buffer[2048] = "";
      if (kpm_ue_id_e2sm_count == 0)
        strcat(kpm_ue_id_e2sm_temp, kpm_buffer_ue_id_e2sm);
      kpm_ue_id_e2sm_count += 1;
      to_mysql_string_kpm_ue_id_e2sm(id, ue_id_e2sm, ric_ind_frmt, ue_id_e2sm.type, timestamp, buffer, 512);
      if (kpm_ue_id_e2sm_count < kpm_ue_id_e2sm_max) {
        strcat(buffer, ",");
        strcat(kpm_ue_id_e2sm_temp, buffer);
      } else {
        strcat(kpm_ue_id_e2sm_temp, buffer);
        strcat(kpm_ue_id_e2sm_temp, ";");
        if (mysql_query(conn, kpm_ue_id_e2sm_temp))
          mysql_finish_with_error(conn);
        strcpy(kpm_ue_id_e2sm_temp,"");
        kpm_ue_id_e2sm_count = 0;
      }
    } else {
      printf("not supported ue_id_e2sm type\n");
    }

    // kpm_ind_msg_format_1_t
    kpm_ind_msg_format_1_t const *meas_report = &msg->meas_report_per_ue[i].ind_msg_format_1;
    write_kpm_frm1_stats(conn, id, ric_ind_frmt, meas_report, timestamp, i);
  }

}

// kpm_ric_ind_hdr_format_1_t
int kpm_hdr_count = 0;
int kpm_hdr_max = 50;
char kpm_buffer_hdr[2048] = "INSERT INTO KPM_HDR "
                                  "("
                                  "tstamp,"
                                  "ngran_node,"
                                  "e2node_mcc,"
                                  "e2node_mnc,"
                                  "e2node_mnc_digit_len,"
                                  "e2node_nb_id,"
                                  "e2node_cu_du_id,"
                                  "format,"
                                  "collectStartTime,"
                                  "fileformat_version,"
                                  "sender_name,"
                                  "sender_type,"
                                  "vendor_name"
                                  ") "
                                  "VALUES";
char kpm_hdr_temp[16384] = "";
static
void write_kpm_hdr_frm1_stats(MYSQL* conn,
                              global_e2_node_id_t const* id,
                              kpm_ind_hdr_t const* hdr)
{
  assert(conn != NULL);
  assert(hdr != NULL);


  if (hdr->type == FORMAT_1_INDICATION_HEADER) {
    char buffer[2048] = "";
    if (kpm_hdr_count == 0)
      strcat(kpm_hdr_temp, kpm_buffer_hdr);
    kpm_hdr_count += 1;
    to_mysql_string_kpm_hdr(id, hdr->type, hdr->kpm_ric_ind_hdr_format_1, buffer, 512);
    if (kpm_hdr_count < kpm_hdr_max) {
      strcat(buffer, ",");
      strcat(kpm_hdr_temp, buffer);
    } else {
      strcat(kpm_hdr_temp, buffer);
      strcat(kpm_hdr_temp, ";");
      if (mysql_query(conn, kpm_hdr_temp))
        mysql_finish_with_error(conn);
      strcpy(kpm_hdr_temp,"");
      kpm_hdr_count = 0;
    }
  } else {
    assert(0!=0 && "unknown KPM hdr format");
  }

}

static
void write_kpm_stats(MYSQL* conn, global_e2_node_id_t const* id, kpm_ind_data_t const* ind)
{
  assert(conn != NULL);
  assert(ind != NULL);

  write_kpm_hdr_frm1_stats(conn, id, &ind->hdr);

  kpm_ind_msg_t const* msg = &ind->msg;
  uint64_t const timestamp = ind->hdr.kpm_ric_ind_hdr_format_1.collectStartTime;

  if (msg->type == FORMAT_1_INDICATION_MESSAGE) {
    write_kpm_frm1_stats(conn, id, msg->type, &msg->frm_1, timestamp, 999);
  } else if (msg->type == FORMAT_2_INDICATION_MESSAGE) {
    assert(0!=0);
  } else if (msg->type == FORMAT_3_INDICATION_MESSAGE) {
    write_kpm_frm3_stats(conn, id, msg->type, &msg->frm_3, timestamp);
  }
}

void init_db_mysql(MYSQL** conn, db_params_t const* db_params)
{
  assert(conn != NULL);
  assert(db_params != NULL);
  assert(db_params->ip != NULL);
  assert(db_params->user != NULL);
  assert(db_params->pass != NULL);

  *conn = mysql_init(NULL);
  assert(*conn != NULL && "Error initialializing mySQL\n");

  printf("[MySQL]: try to connect server ip %s\n", db_params->ip);
  if(mysql_real_connect(*conn, db_params->ip, db_params->user, db_params->pass, NULL, 0, NULL, 0) == NULL)
  {
    fprintf(stderr, "Fatal: connecting to mySQL: %s\n", mysql_error(*conn));
    mysql_close(*conn);
    *conn = NULL; // Do not use the handler after it has been closed. 
    return;
  }
  printf("[MySQL]: Connection Successful\n");


  // drop exists db in server
  char cmd_drop_db[512] = "DROP DATABASE IF EXISTS ";
  strcat(cmd_drop_db, db_params->filename);
  printf("[MySQL]: %s\n", cmd_drop_db);
  if(mysql_query(*conn, cmd_drop_db))
    mysql_finish_with_error(*conn);
  printf("[MySQL]: Drop Exist DB Successful\n");

  // create db in server
  char cmd_create_db[512] = "CREATE DATABASE IF NOT EXISTS ";
  strcat(cmd_create_db, db_params->filename);
  printf("[MySQL]: %s\n", cmd_create_db);
  if(mysql_query(*conn, cmd_create_db))
    mysql_finish_with_error(*conn);
  printf("[MySQL]: Create New DB Successful\n");

  // use db created db
  char cmd_use_db[512] = "USE ";
  strcat(cmd_use_db, db_params->filename);
  printf("[MySQL]: %s\n", cmd_use_db);

  //////
  // MAC
  //////
  mysql_query(*conn, cmd_use_db);
  create_mac_ue_table(*conn);
  printf("[MySQL]: Create New MAC_UE Table Successful\n");

  //////
  // RLC
  //////
  mysql_query(*conn, cmd_use_db);
  create_rlc_bearer_table(*conn);
  printf("[MySQL]: Create New RLC_bearer Table Successful\n");

  //////
  // PDCP
  //////
  mysql_query(*conn, cmd_use_db);
  create_pdcp_bearer_table(*conn);
  printf("[MySQL]: Create New PDCP_bearer Table Successful\n");

  //////
  // SLICE
  //////
  mysql_query(*conn, cmd_use_db);
  create_slice_table(*conn);
  printf("[MySQL]: Create New Slice Table Successful\n");
  mysql_query(*conn, cmd_use_db);
  create_ue_slice_table(*conn);
  printf("[MySQL]: Create New UE_SLICE Table Successful\n");

  //////
  // GTP
  //////
  mysql_query(*conn, cmd_use_db);
  create_gtp_table(*conn);
  printf("[MySQL]: Create New GTP Table Successful\n");

  //////
  // KPM
  //////
  mysql_query(*conn, cmd_use_db);
  create_kpm_table(*conn);
  printf("[MySQL]: Create New KPM Table Successful\n");
}

void close_db_mysql(MYSQL* conn)
{
  assert(conn != NULL);
  mysql_close(conn);

}

void write_db_mysql(MYSQL* conn, global_e2_node_id_t const* id, sm_ag_if_rd_t const* ag_rd)
{
  assert(conn != NULL);
  assert(ag_rd != NULL);
  assert(ag_rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);

  sm_ag_if_rd_ind_t const* rd = &ag_rd->ind;
  assert(rd->type == MAC_STATS_V0   || rd->type == RLC_STATS_V0
         || rd->type == PDCP_STATS_V0  || rd->type == GTP_STATS_V0 || rd->type == SLICE_STATS_V0
         || rd->type == KPM_STATS_V3_0 || rd->type == RAN_CTRL_STATS_V1_03);

  if(rd->type == MAC_STATS_V0){
    write_mac_stats(conn, id, &rd->mac);
  } else if(rd->type == RLC_STATS_V0 ){
    write_rlc_stats(conn, id, &rd->rlc);
  } else if(rd->type == PDCP_STATS_V0) {
    write_pdcp_stats(conn, id, &rd->pdcp);
  } else if (rd->type == SLICE_STATS_V0) {
    write_slice_stats(conn, id, &rd->slice);
  } else if (rd->type == GTP_STATS_V0) {
    write_gtp_stats(conn, id, &rd->gtp);
  } else if (rd->type == KPM_STATS_V3_0) {
    write_kpm_stats(conn, id, &rd->kpm.ind);
  } else if (rd->type == RAN_CTRL_STATS_V1_03) {
    printf("mysql not support rc sm\n");
  } else {
    assert(0!=0 && "Unknown statistics type received ");
  }
}

