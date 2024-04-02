#include "channel_dl_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"


void free_channel_dl_amr(channel_dl_amr_t* src)
{
  assert(src != NULL);
  // Mandatory. Array of float. Noise level table, in dB
  free(src->noise_level);
  //size_t sz_noise_level;

  // Mandatory. Global frequency shift in Hz
  //float freq_shift;

  // Mandatory. Array Each object contains the
  // parameters of a path
  for(size_t i = 0; i < src->sz_path; ++i){
    free_path_amr(&src->path[i]);
  } 

  free(src->path);
}

channel_dl_amr_t parse_channel_dl_amr(void* it)
{
  assert(it != NULL);

  channel_dl_amr_t dst = {0}; 

  // Mandatory. Array of float. Noise level table, in dB
  ans_cjson_t ans = find_object(it, "noise_level");

  dst.sz_noise_level = cJSON_GetArraySize(ans.it);
  assert(dst.sz_noise_level != 0 && "Not clear from specs" );
  dst.noise_level = calloc(dst.sz_noise_level, sizeof(float));
  assert(dst.noise_level != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_noise_level; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans.it, i);
    dst.noise_level[i] = tmp->valuedouble;
  }

  // Mandatory. Global frequency shift in Hz
  dst.freq_shift = parse_float(it, "freq_shift"); 

  // Mandatory. Array Each object contains the
  // parameters of a path
  
  ans_cjson_t ans_path = find_object(it, "path");

  dst.sz_path = cJSON_GetArraySize(ans_path.it);
  assert(dst.sz_path != 0 && "Not clear from specs" );
  dst.path = calloc(dst.sz_path, sizeof(path_amr_t));
  assert(dst.path != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_path; ++i){
    cJSON* tmp = cJSON_GetArrayItem(ans_path.it, i);
    dst.path[i] = parse_path_amr(tmp);
  }

  return dst;
}

