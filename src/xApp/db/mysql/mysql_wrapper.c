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

//static
//void create_table_mysql(MYSQL* db, char* sql)
//{
//  char* err_msg = NULL;
//  int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
//  assert(rc == SQLITE_OK && "Error while creating the DB. Check the err_msg string for further info" );
//}

static
void create_mac_ue_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS MAC_UE"))
    mysql_finish_with_error(conn);
  if(mysql_query(conn, "CREATE TABLE MAC_UE("
                       "tstamp BIGINT CHECK(tstamp > 0),"
                       "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
                       "mcc INT,"
                       "mnc INT,"
                       "mnc_digit_len INT,"
                       "nb_id INT,"
                       "cu_du_id TEXT,"
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
                  "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
                  "mcc INT,"
                  "mnc INT,"
                  "mnc_digit_len INT,"
                  "nb_id INT,"
                  "cu_du_id TEXT,"
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
                  "txsdu_bytes BIGINT CHECK(txsdu_bytes >= 0 AND txsdu_bytes < 4294967296),"
                  "rxsdu_pkts BIGINT CHECK(rxsdu_pkts >= 0 AND rxsdu_pkts < 4294967296),"
                  "rxsdu_bytes BIGINT CHECK(rxsdu_bytes >= 0 AND rxsdu_bytes < 4294967296),"
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
                   "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
                   "mcc INT,"
                   "mnc INT,"
                   "mnc_digit_len INT,"
                   "nb_id INT,"
                   "cu_du_id TEXT,"
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
                    "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
                    "mcc INT,"
                    "mnc INT,"
                    "mnc_digit_len INT,"
                    "nb_id INT,"
                    "cu_du_id TEXT,"
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
                       "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
                       "mcc INT,"
                       "mnc INT,"
                       "mnc_digit_len INT,"
                       "nb_id INT,"
                       "cu_du_id TEXT,"
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
                  "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
                  "mcc INT,"
                  "mnc INT,"
                  "mnc_digit_len INT,"
                  "nb_id INT,"
                  "cu_du_id TEXT,"
                  "teidgnb INT,"
                  "rnti INT CHECK(rnti >= -1 AND rnti < 65535),"
                  "qfi INT,"
                  "teidupf INT"
                  ");";

  if(mysql_query(conn, sql_gtp))
    mysql_finish_with_error(conn);
}

static
void create_kpm_table(MYSQL* conn)
{
  assert(conn != NULL);

  // ToDo: PRIMARY KEY UNIQUE
  if(mysql_query(conn, "DROP TABLE IF EXISTS KPM_MeasRecord"))
    mysql_finish_with_error(conn);
  char* sql_kpm_measRecord = "CREATE TABLE KPM_MeasRecord("
                             "tstamp BIGINT CHECK(tstamp > 0),"
                             "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
                             "mcc INT,"
                             "mnc INT,"
                             "mnc_digit_len INT,"
                             "nb_id INT,"
                             "cu_du_id TEXT,"
                             "rnti INT CHECK(rnti >= -1 AND rnti < 65535),"
                             "incompleteFlag INT,"
                             "name TEXT," //measRecord name
                             "val REAL"
                             ");";

  if(mysql_query(conn, sql_kpm_measRecord))
    mysql_finish_with_error(conn);

  // ToDo: PRIMARY KEY UNIQUE
//  if(mysql_query(conn, "DROP TABLE IF EXISTS KPM_LabelInfo"))
//    mysql_finish_with_error(conn);
//  char* sql_kpm_labelInfo = "CREATE TABLE KPM_LabelInfo("
//                            "tstamp BIGINT CHECK(tstamp > 0),"
//                            "ngran_node INT CHECK(ngran_node >= 0 AND ngran_node < 9),"
//                            "mcc INT,"
//                            "mnc INT,"
//                            "mnc_digit_len INT,"
//                            "nb_id INT,"
//                            "cu_du_id TEXT,"
//                            "MeasType TEXT,"
//                            "noLabel BIGINT CHECK(noLabel >=0 AND noLabel < 4294967296),"
//                            "plmnID TEXT,"
//                            "sST TEXT,"
//                            "sD TEXT,"
//                            "fiveQI BIGINT CHECK(fiveQI >= 0 AND fiveQI < 4294967296),"
//                            "qFI BIGINT CHECK(qFI >= 0 AND qFI < 4294967296),"
//                            "qCI BIGINT CHECK(qCI >= 0 AND qCI < 4294967296),"
//                            "qCImax BIGINT CHECK(qCImax >= 0 AND qCImax < 4294967296),"
//                            "qCImin BIGINT CHECK(qCImin >= 0 AND qCImin < 4294967296),"
//                            "aRPmax BIGINT CHECK(aRPmax >= 0 AND aRPmax < 4294967296),"
//                            "aRPmin BIGINT CHECK(aRPmin >= 0 AND aRPmin < 4294967296),"
//                            "bitrateRange BIGINT CHECK(bitrateRange >= 0 AND bitrateRange < 4294967296),"
//                            "layerMU_MIMO BIGINT CHECK(layerMU_MIMO >= 0 AND layerMU_MIMO < 4294967296),"
//                            "sUM BIGINT CHECK(sUM >= 0 AND sUM < 4294967296),"
//                            "distBinX BIGINT CHECK(distBinX >= 0 AND distBinX < 4294967296),"
//                            "distBinY BIGINT CHECK(distBinY >= 0 AND distBinY < 4294967296),"
//                            "distBinZ BIGINT CHECK(distBinZ >= 0 AND distBinZ < 4294967296),"
//                            "preLabelOverride BIGINT CHECK(preLabelOverride >= 0 AND preLabelOverride < 4294967296),"
//                            "startEndInd BIGINT CHECK(startEndInd >= 0 AND startEndInd < 4294967296),"
//                            "min BIGINT CHECK(min >= 0 AND min < 4294967296),"
//                            "max BIGINT CHECK(max >= 0 AND max < 4294967296),"
//                            "avg BIGINT CHECK(avg >= 0 AND avg < 4294967296)"
//                            ");";

  //TODO: need to implement the function to write data of labelinfo
  //if(mysql_query(conn, sql_kpm_labelInfo))
  //  mysql_finish_with_error(conn);
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
                              id->nb_id,
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
                                    "%u," //rlc->txsdu_bytes
                                    "%u," //rlc->rxsdu_pkts
                                    "%u," //rlc->rxsdu_bytes
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
                                    id->nb_id,
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
                                    rlc->rxsdu_pkts,
                                    rlc->rxsdu_bytes,
                                    rlc->rxsdu_dd_pkts,
                                    rlc->rxsdu_dd_bytes,
                                    rlc->rnti,
                                    rlc->mode,
                                    rlc->rbid);

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
                    id->nb_id,
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
                  tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id,
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
                  ,tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id,
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
                    tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id,
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
                    tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id,
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
                  tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id,
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
                  tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id,
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
                tstamp, id->type, id->plmn.mcc, id->plmn.mnc, id->plmn.mnc_digit_len, id->nb_id,
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
                          id->nb_id,
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
void to_mysql_string_kpm_measRecord(global_e2_node_id_t const* id,
                                    adapter_MeasDataItem_t* kpm_measData,
                                    adapter_MeasRecord_t* kpm_measRecord,
                                    MeasInfo_t* kpm_measInfo,
                                    int64_t tstamp,
                                    uint32_t rnti,
                                    char* out,
                                    size_t out_len)
{
  assert(kpm_measData != NULL);
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);

  char* c_null = NULL;
  char c_cu_du_id[26];
  if (id->cu_du_id) {
    int rc = snprintf(c_cu_du_id, 26, "%lu", *id->cu_du_id);
    assert(rc < (int) max && "Not enough space in the char array to write all the data");
  }

  if (kpm_measRecord == NULL){
    int const rc = snprintf(out, max,
                            "("
                            "%ld,"// tstamp
                            "%d," //ngran_node
                            "%d," //mcc
                            "%d," //mnc
                            "%d," //mnc_digit_len
                            "%d," //nb_id
                            "'%s'," //cu_du_id
                            "%d," //rnti
                            "%ld,"  //kpm_measData->incompleteFlag
                            "'%s'," //kpm_measName
                            "'%s'"  //kpm_measRecord->int_val
                            ")",
                            tstamp,
                            id->type,
                            id->plmn.mcc,
                            id->plmn.mnc,
                            id->plmn.mnc_digit_len,
                            id->nb_id,
                            id->cu_du_id ? c_cu_du_id : c_null,
                            rnti,
                            kpm_measData->incompleteFlag,
                            kpm_measInfo ? kpm_measInfo->measName.buf : NULL,
                            c_null
                            );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    return ;
  } else {
    if(kpm_measRecord->type == MeasRecord_int){
      int const rc = snprintf(out, max,
                              "("
                              "%ld,"// tstamp
                              "%d," //ngran_node
                              "%d," //mcc
                              "%d," //mnc
                              "%d," //mnc_digit_len
                              "%d," //nb_id
                              "'%s'," //cu_du_id
                              "%d," //rnti
                              "%ld,"  //kpm_measData->incompleteFlag
                              "'%s'," //kpm_measName
                              "%ld"  //kpm_measRecord->int_val
                              ")",
                              tstamp,
                              id->type,
                              id->plmn.mcc,
                              id->plmn.mnc,
                              id->plmn.mnc_digit_len,
                              id->nb_id,
                              id->cu_du_id ? c_cu_du_id : c_null,
                              rnti,
                              kpm_measData->incompleteFlag,
                              kpm_measInfo ? kpm_measInfo->measName.buf : NULL,
                              kpm_measRecord->int_val
                              );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      return;
    }else if (kpm_measRecord->type == MeasRecord_real){
      int const rc = snprintf(out, max,
                              "("
                              "%ld,"// tstamp
                              "%d," //ngran_node
                              "%d," //mcc
                              "%d," //mnc
                              "%d," //mnc_digit_len
                              "%d," //nb_id
                              "'%s'," //cu_du_id
                              "%d," //rnti
                              "%ld,"  //kpm_measData->incompleteFlag
                              "'%s'," //kpm_measName
                              "%f"  //kpm_measRecord->real_val
                              ")",
                              tstamp,
                              id->type,
                              id->plmn.mcc,
                              id->plmn.mnc,
                              id->plmn.mnc_digit_len,
                              id->nb_id,
                              id->cu_du_id ? c_cu_du_id : c_null,
                              rnti,
                              kpm_measData->incompleteFlag,
                              kpm_measInfo ? kpm_measInfo->measName.buf : NULL,
                              kpm_measRecord->real_val
                              );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      return;
    }else if (kpm_measRecord->type == MeasRecord_noval){
      int const rc = snprintf(out, max,
                              "("
                              "%ld,"// tstamp
                              "%d," //ngran_node
                              "%d," //mcc
                              "%d," //mnc
                              "%d," //mnc_digit_len
                              "%d," //nb_id
                              "'%s'," //cu_du_id
                              "%d," //rnti
                              "%ld,"  //kpm_measData->incompleteFlag
                              "'%s'," //kpm_measName
                              "-1"  //kpm_measRecord->noVal
                              ")",
                              tstamp,
                              id->type,
                              id->plmn.mcc,
                              id->plmn.mnc,
                              id->plmn.mnc_digit_len,
                              id->nb_id,
                              id->cu_du_id ? c_cu_du_id : c_null,
                              rnti,
                              kpm_measData->incompleteFlag,
                              kpm_measInfo ? kpm_measInfo->measName.buf : NULL
                              );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      return;
    }
  }
  assert(0!=0 && "Bad input data. Nothing for SQL to be created");
}

int mac_count = 0;
int mac_stat_max = 50;
char mac_buffer[2048] = "INSERT INTO MAC_UE "
                        "("
                        "tstamp,"
                        "ngran_node,"
                        "mcc,"
                        "mnc,"
                        "mnc_digit_len,"
                        "nb_id,"
                        "cu_du_id,"
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
                    "mcc,"
                    "mnc,"
                    "mnc_digit_len,"
                    "nb_id,"
                    "cu_du_id,"
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
                        "mcc,"
                        "mnc,"
                        "mnc_digit_len,"
                        "nb_id,"
                        "cu_du_id,"
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
                          "mcc,"
                          "mnc,"
                          "mnc_digit_len,"
                          "nb_id,"
                          "cu_du_id,"
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
                             "mcc,"
                             "mnc,"
                             "mnc_digit_len,"
                             "nb_id,"
                             "cu_du_id,"
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
                         "mcc,"
                         "mnc,"
                         "mnc_digit_len,"
                         "nb_id,"
                         "cu_du_id,"
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

static uint32_t extract_kpm_rnti_val(const kpm_ind_msg_t* ind_msg, const adapter_MeasDataItem_t* measData)
{
  uint32_t rnti = -1; // -1 means not found.
  for (size_t j = 0; j < ind_msg->MeasInfo_len; j++) {
    if ((ind_msg->MeasInfo[j].meas_type == KPM_V2_MEASUREMENT_TYPE_NAME) &&
        (strcmp((char*)ind_msg->MeasInfo[j].measName.buf, "rnti") == 0)) {
          rnti = measData->measRecord[j].int_val;
          // printf("find rnti column, measRecord[%lu].real_val = %d\n", j, rnti);
          break;
    }
  }
  return rnti;
}

int kpm_count = 0;
int kpm_stat_max = 50;
char kpm_buffer[2048] = "INSERT INTO KPM_MeasRecord "
                        "("
                        "tstamp,"
                        "ngran_node,"
                        "mcc,"
                        "mnc,"
                        "mnc_digit_len,"
                        "nb_id,"
                        "cu_du_id,"
                        "rnti,"
                        "incompleteFlag,"
                        "name,"
                        "val"
                        ") "
                        "VALUES";
char kpm_temp[16384] = "";
static
void write_kpm_stats(MYSQL* conn, global_e2_node_id_t const* id, kpm_ind_data_t const* ind)
{
  // TODO: Add granulPeriod into database
  // TODO: Add MeasInfo and LabelInfo into database

  assert(conn != NULL);
  assert(ind != NULL);

  kpm_ind_msg_t const* ind_msg_kpm = &ind->msg;

  int64_t timestamp_us = 0;
  if (ind->msg.MeasData_len > 0)
    if (ind->msg.MeasData[0].measRecord_len > 0)
      timestamp_us = ind->msg.MeasData[0].measRecord[0].real_val;
  for(size_t i = 0; i < ind_msg_kpm->MeasData_len; i++){
    adapter_MeasDataItem_t* curMeasData = &ind_msg_kpm->MeasData[i];
    if (curMeasData->measRecord_len > 0){
      uint32_t rnti = extract_kpm_rnti_val(ind_msg_kpm, curMeasData);
      for (size_t j = 1; j < curMeasData->measRecord_len; j++){
        adapter_MeasRecord_t* curMeasRecord = &curMeasData->measRecord[j];
        MeasInfo_t* curMeasInfo = &ind_msg_kpm->MeasInfo[j];

        if ((curMeasInfo->meas_type == KPM_V2_MEASUREMENT_TYPE_NAME) &&
                (strcmp((char*)curMeasInfo->measName.buf, "rnti") == 0))
          continue;

        char buffer[2048] = "";
        if (kpm_count == 0)
          strcat(kpm_temp, kpm_buffer);
        kpm_count += 1;
        to_mysql_string_kpm_measRecord(id, curMeasData, curMeasRecord, curMeasInfo, timestamp_us, rnti,
                                       buffer, 512);
        // TODO: by our convention, the first record contains "timestamp_us". Add that value to collectStartTime that is microseconds to be able to have a
        // consistent high resolution timestamp in microseconds.
        // to_mysql_string_kpm_measRecord(id, curMeasData, curMeasRecord, curMeasInfo, (long)ind->hdr.collectStartTime * 1000000 + ts,
        //                                buffer, 512);
        if (kpm_count < kpm_stat_max) {
          //printf("%d add ,\n", kpm_count);
          strcat(buffer, ",");
          strcat(kpm_temp, buffer);
        } else {
          //printf("%d add ;\n", kpm_temp);
          kpm_count = 0;
          strcat(kpm_temp, buffer);
          strcat(kpm_temp, ";");
          //for(size_t i = 0; i < strlen(kpm_temp); i++)
          //  printf("%c", kpm_temp[i]);
          //printf("\n");
          //int64_t st = time_now_us();
          if (mysql_query(conn, kpm_temp))
            mysql_finish_with_error(conn);
          //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
          strcpy(kpm_temp,"");
        }
      }
    } else {
      char buffer[2048] = "";
      if (kpm_count == 0)
        strcat(kpm_temp, kpm_buffer);
      kpm_count += 1;
      to_mysql_string_kpm_measRecord(id, curMeasData, NULL, NULL, timestamp_us, -1,
      // to_mysql_string_kpm_measRecord(id, curMeasData, NULL, NULL, (long)ind->hdr.collectStartTime * 1000000 + ts,
                                      buffer, 512);
      if (kpm_count < kpm_stat_max) {
        //printf("%d add ,\n", kpm_count);
        strcat(buffer, ",");
        strcat(kpm_temp, buffer);
      } else {
        //printf("%d add ;\n", kpm_temp);
        kpm_count = 0;
        strcat(kpm_temp, buffer);
        strcat(kpm_temp, ";");
        //for(size_t i = 0; i < strlen(kpm_temp); i++)
        //  printf("%c", kpm_temp[i]);
        //printf("\n");
        //int64_t st = time_now_us();
        if (mysql_query(conn, kpm_temp))
          mysql_finish_with_error(conn);
        //printf("[MYSQL]: write db consuming time: %ld\n", time_now_us() - st);
        strcpy(kpm_temp,"");
      }
    }
  }
}

void init_db_mysql(MYSQL* conn, char const* db_name)
{
  assert(conn != NULL);
  assert(db_name != NULL);

  // drop exists db in server
  char cmd_drop_db[512] = "DROP DATABASE IF EXISTS ";
  strcat(cmd_drop_db, db_name);
  printf("[MySQL]: %s\n", cmd_drop_db);
  if(mysql_query(conn, cmd_drop_db))
    mysql_finish_with_error(conn);
  printf("[MySQL]: Drop Exist DB Successful\n");

  // create db in server
  char cmd_create_db[512] = "CREATE DATABASE IF NOT EXISTS ";
  strcat(cmd_create_db, db_name);
  printf("[MySQL]: %s\n", cmd_create_db);
  if(mysql_query(conn, cmd_create_db))
    mysql_finish_with_error(conn);
  printf("[MySQL]: Create New DB Successful\n");

  // use db created db
  char cmd_use_db[512] = "USE ";
  strcat(cmd_use_db, db_name);
  printf("[MySQL]: %s\n", cmd_use_db);

  //////
  // MAC
  //////
  mysql_query(conn, cmd_use_db);
  create_mac_ue_table(conn);
  printf("[MySQL]: Create New MAC_UE Table Successful\n");

  //////
  // RLC
  //////
  mysql_query(conn, cmd_use_db);
  create_rlc_bearer_table(conn);
  printf("[MySQL]: Create New RLC_bearer Table Successful\n");

  //////
  // PDCP
  //////
  mysql_query(conn, cmd_use_db);
  create_pdcp_bearer_table(conn);
  printf("[MySQL]: Create New PDCP_bearer Table Successful\n");

  //////
  // SLICE
  //////
  mysql_query(conn, cmd_use_db);
  create_slice_table(conn);
  printf("[MySQL]: Create New Slice Table Successful\n");
  mysql_query(conn, cmd_use_db);
  create_ue_slice_table(conn);
  printf("[MySQL]: Create New UE_SLICE Table Successful\n");

  //////
  // GTP
  //////
  mysql_query(conn, cmd_use_db);
  create_gtp_table(conn);
  printf("[MySQL]: Create New GTP Table Successful\n");

  //////
  // KPM
  //////
  mysql_query(conn, cmd_use_db);
  create_kpm_table(conn);
  printf("[MySQL]: Create New KPM Table Successful\n");
}

void close_db_mysql(MYSQL* conn)
{
  assert(conn != NULL);
  mysql_close(conn);

}

void write_db_mysql(MYSQL* conn, global_e2_node_id_t const* id, sm_ag_if_rd_t const* rd)
{
  assert(conn != NULL);
  assert(rd != NULL);
  assert(rd->type == MAC_STATS_V0 ||
        rd->type == RLC_STATS_V0 ||
        rd->type == PDCP_STATS_V0 ||
        rd->type == SLICE_STATS_V0 ||
        rd->type == GTP_STATS_V0 ||
        rd->type == KPM_STATS_V0);

  if(rd->type == MAC_STATS_V0){
    write_mac_stats(conn, id, &rd->mac_stats);
  } else if(rd->type == RLC_STATS_V0 ){
    write_rlc_stats(conn, id, &rd->rlc_stats);
  } else if(rd->type == PDCP_STATS_V0) {
    write_pdcp_stats(conn, id, &rd->pdcp_stats);
  } else if (rd->type == SLICE_STATS_V0) {
    write_slice_stats(conn, id, &rd->slice_stats);
  } else if (rd->type == GTP_STATS_V0) {
    write_gtp_stats(conn, id, &rd->gtp_stats);
  } else if (rd->type == KPM_STATS_V0) {
    write_kpm_stats(conn, id, &rd->kpm_stats);
  } else {
    assert(0!=0 && "Unknown statistics type received ");
  }
}

