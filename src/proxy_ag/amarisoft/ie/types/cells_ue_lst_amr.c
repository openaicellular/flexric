#include "cells_ue_lst_amr.h"

#include <assert.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

cells_ue_lst_amr_t parse_cells_ue_lst_amr(void *it) 
{
  assert(it != NULL);
  cells_ue_lst_amr_t dst = {0};

  dst.cell_id = parse_int(it, "cell_id");
  dst.cqi = parse_int(it, "cqi");
  dst.ri = parse_int(it, "ri");
  dst.dl_bitrate = parse_int(it, "dl_bitrate");
  dst.ul_bitrate = parse_int(it, "ul_bitrate");
  dst.ul_rank = parse_int(it, "ul_rank");
  dst.dl_tx = parse_int(it, "dl_tx");
  dst.ul_tx = parse_int(it, "ul_tx");
  dst.dl_err = parse_int(it, "dl_err");
  dst.ul_err = parse_int(it, "ul_err");
  dst.dl_retx = parse_int(it, "dl_retx");
  dst.ul_retx = parse_int(it, "ul_retx");
  dst.dl_mcs = parse_int(it, "dl_mcs");
  dst.ul_mcs = parse_int(it, "ul_mcs");
  dst.ul_n_layer = parse_int(it, "ul_n_layer");
  dst.turbo_decoder_min = parse_int(it, "turbo_decoder_min");
  dst.turbo_decoder_avg = parse_int(it, "turbo_decoder_avg");
  dst.turbo_decoder_max = parse_int(it, "turbo_decoder_max");
  dst.pusch_snr = parse_int(it, "pusch_snr");
  dst.epre = parse_int(it, "epre");
  dst.ul_phr = parse_int(it, "ul_phr");
  dst.ul_path_loss = parse_int(it, "ul_path_loss");
  dst.p_ue = parse_int(it, "p_ue");
  dst.initial_ta = parse_int(it, "initial_ta");

  return dst;
}
