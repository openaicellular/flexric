#include "nr_frq_bnd_it.h"
#include <assert.h>
#include <stdlib.h>

void free_nr_frq_bnd_it(nr_frq_bnd_it_t* src)
{
  assert(src != NULL);

  //uint16_t bnd; // [1,1024]

  // [0,32]
  //size_t sz_sul;
  // [1,1024]
  free(src->sul_bnd_it);
}

bool eq_nr_frq_bnd_it( nr_frq_bnd_it_t const* m0,  nr_frq_bnd_it_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // [1,1024]
  assert(m0->bnd > 0 && m0->bnd < 1025);
  assert(m1->bnd > 0 && m1->bnd < 1025);
  if(m0->bnd != m1->bnd)
    return false;

  // [0,32]
  assert(m0->sz_sul < 33);
  assert(m1->sz_sul < 33);
  if(m0->sz_sul != m1->sz_sul)
    return false;

  for(size_t i = 0; i < m0->sz_sul; ++i){
    if(m0->sul_bnd_it[i] != m1->sul_bnd_it[i])
      return false;
  }

  return true;
}

nr_frq_bnd_it_t cp_nr_frq_bnd_it(nr_frq_bnd_it_t const* src)
{
  assert(src != NULL);

  nr_frq_bnd_it_t dst = {0};

  // [1,1024]
  assert(src->bnd > 0 && src->bnd < 1025); 
  dst.bnd = src->bnd;

  // [0,32]
  assert(src->sz_sul < 33);
  dst.sz_sul = src->sz_sul; 
  if(dst.sz_sul > 0){
    dst.sul_bnd_it = calloc(dst.sz_sul, sizeof(uint16_t));
    assert(dst.sul_bnd_it != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < src->sz_sul; ++i){
    dst.sul_bnd_it[i] = src->sul_bnd_it[i];
  }

  return dst;
}

