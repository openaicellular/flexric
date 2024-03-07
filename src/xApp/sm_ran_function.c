#include "sm_ran_function.h"

#include <assert.h>

void free_sm_ran_function(sm_ran_function_t* src)
{
  assert(src != NULL);

  free_sm_ran_function_def(&src->defn);

#ifdef E2AP_V1
  // optional
  if(src->oid != NULL){
    free_byte_array(*src->oid);
    free(src->oid);
  }
#elif defined(E2AP_V2) || defined(E2AP_V3) 
  free_byte_array(src->oid);
#endif

}

sm_ran_function_t cp_sm_ran_function(sm_ran_function_t const* src)
{
  assert(src != NULL);
  sm_ran_function_t dst = {0};
  
  dst.defn = cp_sm_ran_function_def(&src->defn);

  dst.id = src->id;
  dst.rev = src->rev;
#ifdef E2AP_V1
   // optional
  if(src->oid != NULL){
    dst.oid = calloc(1, sizeof(byte_array_t));
    assert(dst.oid != NULL && "Memory exhausted");
    *dst.oid = copy_byte_array(*src->oid);
  } 
#elif defined(E2AP_V2) || defined(E2AP_V3) 
  dst.oid = copy_byte_array(src->oid);
#endif

  return dst;
}

bool eq_sm_ran_function(sm_ran_function_t const* m0, sm_ran_function_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_sm_ran_function_def(&m0->defn, &m1->defn) == false)
    return false;

  if(m0->id != m1->id)
    return false;

  if(m0->rev != m1->rev)
    return false;

#ifdef E2AP_V1
  if(eq_byte_array(m0->oid, m1->oid) == false)
    return false;
#elif defined(E2AP_V2) || defined(E2AP_V3) 
  if(eq_byte_array(&m0->oid, &m1->oid) == false)
    return false;
#endif

  return true;
}

