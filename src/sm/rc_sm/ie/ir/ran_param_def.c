#include "ran_param_def.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_def(ran_param_def_t* src)
{
  assert(src != NULL);

  switch (src->type)
  {
  case LIST_RAN_PARAMETER_DEF_TYPE:
    for(size_t i = 0; i < src->lst->sz_ran_param; i++)
    {
      // RAN Parameter ID
      // Mandatory
      // 9.3.8
      // uint32_t ran_param_id;

      // RAN Parameter Name
      // Mandatory
      // 9.3.9
      free_byte_array(src->lst->ran_param[i].ran_param_name);

      // RAN Parameter Definition
      // Optional
      // 9.3.51
      assert(src->lst->ran_param[i].ran_param_def == NULL && "Not implemented");
    }
    free(src->lst->ran_param);
    free(src->lst);
    break;

  case STRUCTURE_RAN_PARAMETER_DEF_TYPE:
   for(size_t i = 0; i < src->strct->sz_ran_param; i++)
    {
      // RAN Parameter ID
      // Mandatory
      // 9.3.8
      // uint32_t ran_param_id;

      // RAN Parameter Name
      // Mandatory
      // 9.3.9
      free_byte_array(src->strct->ran_param[i].ran_param_name);

      // RAN Parameter Definition
      // Optional
      // 9.3.51
      assert(src->strct->ran_param[i].ran_param_def == NULL && "Not implemented");
    }
    free(src->strct->ran_param);
    free(src->strct);
   break;
    
  default:
    assert(false && "Unknown RAN Parameter Type");
  }

}

bool eq_ran_param_def(ran_param_def_t const* m0, ran_param_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->type != m1->type) return false;

  switch (m0->type)
  {
  case LIST_RAN_PARAMETER_DEF_TYPE:
    if (m0->lst->sz_ran_param != m1->lst->sz_ran_param) return false;

    for (size_t i = 0; i < m0->lst->sz_ran_param; i++)
    {
      // RAN Parameter ID
      // Mandatory
      // 9.3.8
      if (m0->lst->ran_param[i].ran_param_id != m1->lst->ran_param[i].ran_param_id)
        return false;

      // RAN Parameter Name
      // Mandatory
      // 9.3.9
      if(eq_byte_array(&m0->lst->ran_param[i].ran_param_name, &m1->lst->ran_param[i].ran_param_name) == false)
        return false;

      // RAN Parameter Definition
      // Optional
      // 9.3.51
      assert(m0->lst->ran_param[i].ran_param_def == NULL && "Not implemented");
      assert(m1->lst->ran_param[i].ran_param_def == NULL && "Not implemented");
    }
    break;

  case STRUCTURE_RAN_PARAMETER_DEF_TYPE:
    if (m0->strct->sz_ran_param != m1->strct->sz_ran_param) return false;

    for (size_t i = 0; i < m0->strct->sz_ran_param; i++)
    {
      // RAN Parameter ID
      // Mandatory
      // 9.3.8
      if (m0->strct->ran_param[i].ran_param_id != m1->strct->ran_param[i].ran_param_id)
        return false;

      // RAN Parameter Name
      // Mandatory
      // 9.3.9
      if(eq_byte_array(&m0->strct->ran_param[i].ran_param_name, &m1->strct->ran_param[i].ran_param_name) == false)
        return false;

      // RAN Parameter Definition
      // Optional
      // 9.3.51
      assert(m0->strct->ran_param[i].ran_param_def == NULL && "Not implemented");
      assert(m1->strct->ran_param[i].ran_param_def == NULL && "Not implemented");
    }
    break;
  
  default:
    assert(false && "Unknown RAN Parameter Type");
  }

  return true;
}
