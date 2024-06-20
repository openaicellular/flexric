#include "nr_freq_info.h"
#include <assert.h>
#include <stdlib.h>

void free_nr_freq_info(nr_freq_info_t* src)
{
  assert(src != NULL);

  //NR ARFCN
  //Mandatory
  //6.2.3.30
  // uint32_t arfcn; // [0,3279165]

  // [1,32]
  assert(src->sz_frq_bnd_it > 0 && src->sz_frq_bnd_it < 33); 
  for(size_t i = 0; i < src->sz_frq_bnd_it; ++i){
    free_nr_frq_bnd_it(&src->frq_bnd_it[i]);
  }

  free(src->frq_bnd_it);

  // NRFrequency Shift
  // 7p5khz
  // Optional
  free(src->freq_shift_7p5khz);
}

bool eq_nr_freq_info(nr_freq_info_t const* m0, nr_freq_info_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //NR ARFCN
  //Mandatory
  //6.2.3.30
  // [0,3279165]
  assert(m0->arfcn < 3279166); 
  assert(m1->arfcn < 3279167); 
  if(m0->arfcn != m1->arfcn)
    return false;

  // [1,32]
  assert(m0->sz_frq_bnd_it > 0 && m0->sz_frq_bnd_it < 33);
  assert(m1->sz_frq_bnd_it > 0 && m0->sz_frq_bnd_it < 33);
  if(m0->sz_frq_bnd_it != m1->sz_frq_bnd_it)
    return false;

  for(size_t i = 0; i < m0->sz_frq_bnd_it; ++i){
    if(eq_nr_frq_bnd_it(&m0->frq_bnd_it[i], &m1->frq_bnd_it[i] ) == false)
      return false;
  }

  // NRFrequency Shift
  // 7p5khz
  // Optional
  assert(m0->freq_shift_7p5khz == NULL && "Not implemented");
  assert(m1->freq_shift_7p5khz == NULL && "Not implemented");

  return true;
}

nr_freq_info_t cp_nr_freq_info(nr_freq_info_t const* src)
{
  assert(src != NULL);

  nr_freq_info_t dst = {0}; 

  //NR ARFCN
  //Mandatory
  //6.2.3.30
  // [0,3279165]
  assert(src->arfcn < 3279166);
  dst.arfcn = src->arfcn; 

  // [1,32]
  assert(src->sz_frq_bnd_it > 0 && src->sz_frq_bnd_it < 33);
  dst.sz_frq_bnd_it = src->sz_frq_bnd_it;
  dst.frq_bnd_it = calloc(dst.sz_frq_bnd_it, sizeof(nr_frq_bnd_it_t));
  assert(dst.frq_bnd_it != NULL && "Memory exhausted");
  for(size_t i = 0; i < dst.sz_frq_bnd_it; ++i){
    dst.frq_bnd_it[i] = cp_nr_frq_bnd_it(&src->frq_bnd_it[i]);
  }
  
  // NRFrequency Shift
  // 7p5khz
  // Optional
  assert(src->freq_shift_7p5khz == NULL && "Not Implemented"); 

  return dst;
}

