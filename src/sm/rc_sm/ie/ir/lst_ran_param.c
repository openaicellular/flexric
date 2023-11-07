#include "lst_ran_param.h"

#include <assert.h>
#include <stdlib.h>

void free_lst_ran_param(lst_ran_param_t* src)
{
  assert(src != NULL);

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  //1.. 4294967295
  // uint32_t ran_param_id;

  // RAN Parameter Structure
  // Mandatory
  // 9.3.12
  free_ran_param_struct(&src->ran_param_struct);

}

bool eq_lst_ran_param(lst_ran_param_t const* m0, lst_ran_param_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Bug in the standard. RAN Parameter List 9.3.13 
  // has a mandatory ie RAN Parameter ID 9.3.8 
  // and a mandatory ie RAN Parameter Structure 9.3.12
  // However, the ASN  
  // RANParameter-LIST ::= SEQUENCE {
  // list-of-ranParameter  SEQUENCE (SIZE(1..maxnoofItemsinList)) OF RANParameter-STRUCTURE, 
  // ..
  // }
  //
  // Misses RAN Parameter ID and only has RAN Parameter Structure

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  //1.. 4294967295
  // Let's ignore the english written standard and believe the ASN.1 is the correct
  //  uint32_t ran_param_id;

  // RAN Parameter Structure
  // Mandatory
  // 9.3.12
  //ran_param_struct_t ran_param_struct;

  // if(m0->ran_param_id != m1->ran_param_id)
  //   return false;

  if(eq_ran_param_struct(&m0->ran_param_struct, &m1->ran_param_struct) == false)
    return false;

  return true;
}

lst_ran_param_t cp_lst_ran_param(lst_ran_param_t const* src)
{
  assert(src != NULL);

  lst_ran_param_t dst = {0};

  // Bug in the standard. RAN Parameter List 9.3.13 
  // has a mandatory ie RAN Parameter ID 9.3.8 
  // and a mandatory ie RAN Parameter Structure 9.3.12
  // However, the ASN  
  // RANParameter-LIST ::= SEQUENCE {
  // list-of-ranParameter  SEQUENCE (SIZE(1..maxnoofItemsinList)) OF RANParameter-STRUCTURE, 
  // ..
  // }
  //
  // Misses RAN Parameter ID and only has RAN Parameter Structure

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  //1.. 4294967295
  // Let's ignore the english written standard and believe the ASN.1 is the correct
  //  uint32_t ran_param_id;

  // RAN Parameter Structure
  // Mandatory
  // 9.3.12
  //ran_param_struct_t ran_param_struct;

  // assert(src->ran_param_id >= 1 && src->ran_param_id <= 4294967295);
  // dst.ran_param_id = src->ran_param_id;

  // RAN Parameter Structure
  // Mandatory
  // 9.3.12
  dst.ran_param_struct = cp_ran_param_struct(&src->ran_param_struct);
  
  return dst;
}

