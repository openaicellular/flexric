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

#include "mysql/mysql.h"
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

  int rc = snprintf(out, max, "INSERT INTO MAC_UE ("
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
                              "VALUES("
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
void write_mac_stats(MYSQL* conn, global_e2_node_id_t const* id, mac_ind_data_t const* ind) {
  assert(conn != NULL);
  assert(ind != NULL);

  mac_ind_msg_t const *ind_msg_mac = &ind->msg;

  for (size_t i = 0; i < ind_msg_mac->len_ue_stats; ++i) {
    char buffer[2048] = {0};
    int pos = 0;
    pos += to_mysql_string_mac_ue(id, &ind_msg_mac->ue_stats[i], ind_msg_mac->tstamp, buffer + pos, 2048 - pos);
    if (mysql_query(conn, buffer))
      mysql_finish_with_error(conn);
    //printf("MySQL Insert data Successful\n");
  }

}

void init_db_mysql(MYSQL* conn, char const* db_filename)
{
  assert(conn != NULL);
  assert(db_filename != NULL);

  if(mysql_query(conn, "DROP DATABASE IF EXISTS testdb"))
    mysql_finish_with_error(conn);
  printf("MySQL Drop Exist DB Successful\n");
  // create db in server
  if(mysql_query(conn, "CREATE DATABASE IF NOT EXISTS testdb"))
    mysql_finish_with_error(conn);
  printf("MySQL Create New DB Successful\n");

  //////
  // MAC
  //////
  mysql_query(conn, "USE testdb");
  create_mac_ue_table(conn);

}

//void close_db_sqlite3(sqlite3* db)
//{
//  assert(db != NULL);
//  int const rc = sqlite3_close(db);
//  assert(rc == SQLITE_OK && "Error while closing the DB");
//}

void write_db_mysql(MYSQL* conn, global_e2_node_id_t const* id, sm_ag_if_rd_t const* rd)
{
  assert(conn != NULL);
  assert(rd != NULL);
  assert(rd->type == MAC_STATS_V0);

  if(rd->type == MAC_STATS_V0){
    write_mac_stats(conn, id, &rd->mac_stats);
//  } else if(rd->type == RLC_STATS_V0 ){
//    write_rlc_stats(db, id, &rd->rlc_stats);
//  } else if( rd->type == PDCP_STATS_V0) {
//    write_pdcp_stats(db, id, &rd->pdcp_stats);
//  } else if (rd->type == SLICE_STATS_V0) {
//    write_slice_stats(db, id, &rd->slice_stats);
//  } else if (rd->type == GTP_STATS_V0) {
//    write_gtp_stats(db, id, &rd->gtp_stats);
//  } else if (rd->type == KPM_STATS_V0) {
//    write_kpm_stats(db, id, &rd->kpm_stats);
  } else {
    assert(0!=0 && "Unknown statistics type received ");
  }
}

