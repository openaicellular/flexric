#include <assert.h>
#include <string.h>

#include "bin_range_def.h"

void free_kpm_bin_range_def(bin_range_def_t* src)
{
  assert(src != NULL);

  // Bin X
  free(src->bin_x_lst);

  // Bin Y
  if (src->bin_y_lst != NULL)
    free(src->bin_y_lst);

  // Bin Z
  if (src->bin_z_lst != NULL)
    free(src->bin_z_lst);

}

bool eq_kpm_bin_range_def(bin_range_def_t const * m0, bin_range_def_t const * m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Bin X
  if (m0->bin_x_lst_len != m1->bin_x_lst_len)
    return false;

  for (size_t i = 0; i<m0->bin_x_lst_len; i++)
  {
    if (m0->bin_x_lst[i].bin_index != m1->bin_x_lst[i].bin_index)
      return false;

    if (m0->bin_x_lst[i].start_value.value != m1->bin_x_lst[i].start_value.value)
      return false;

    // start value
    switch (m0->bin_x_lst[i].start_value.value)
    {
    case INTEGER_BIN_RANGE:
        if (m0->bin_x_lst[i].start_value.int_value != m1->bin_x_lst[i].start_value.int_value)
          return false;
        break;
    
    case REAL_BIN_RANGE:
        if (m0->bin_x_lst[i].start_value.real_value != m1->bin_x_lst[i].start_value.real_value)
          return false;
        break;

    default:
        break;
    }

    if (m0->bin_x_lst[i].end_value.value != m1->bin_x_lst[i].end_value.value)
      return false;

    // end value
    switch (m0->bin_x_lst[i].end_value.value)
    {
    case INTEGER_BIN_RANGE:
        if (m0->bin_x_lst[i].end_value.int_value != m1->bin_x_lst[i].end_value.int_value)
          return false;
        break;
    
    case REAL_BIN_RANGE:
        if (m0->bin_x_lst[i].end_value.real_value != m1->bin_x_lst[i].end_value.real_value)
          return false;
        break;

    default:
        break;
    }
  }

  // Bin Y
  if ((m0->bin_y_lst != NULL || m1->bin_y_lst != NULL) && m0->bin_y_lst_len != m1->bin_y_lst_len)
    assert(false && "Not yet implemented");

  // Bin Z
  if ((m0->bin_z_lst != NULL || m1->bin_z_lst != NULL) && m0->bin_z_lst_len != m1->bin_z_lst_len)
    assert(false && "Not yet implemented");


  return true;
}


bool eq_kpm_bin_range_def_wrapper(void const * m0, void const * m1)
{
  return  eq_kpm_bin_range_def(m0, m1);
}


bin_range_def_t cp_kpm_bin_range_def(const bin_range_def_t * src)
{
  assert(src != NULL);

  bin_range_def_t dst = {0};

  // [1, 65535]
  assert(src->bin_x_lst_len > 0); 

  // Bin X
  dst.bin_x_lst_len = src->bin_x_lst_len;
  dst.bin_x_lst = calloc(dst.bin_x_lst_len, sizeof(bin_distr_t));
  assert(dst.bin_x_lst != NULL && "Memory exhausted");

  for (size_t i = 0; i<dst.bin_x_lst_len; i++)
  {
    dst.bin_x_lst[i].bin_index = src->bin_x_lst[i].bin_index;
    dst.bin_x_lst[i].start_value.value = src->bin_x_lst[i].start_value.value;

    // start value
    switch (dst.bin_x_lst[i].start_value.value)
    {
    case INTEGER_BIN_RANGE:
      memcpy(&dst.bin_x_lst[i].start_value.int_value, &src->bin_x_lst[i].start_value.int_value, 4);
      break;
    
    case REAL_BIN_RANGE:
      memcpy(&dst.bin_x_lst[i].start_value.real_value, &src->bin_x_lst[i].start_value.real_value, 8);
      break;

    default:
      assert(0 != 0 && "Unknown type");
      break;
    }


    dst.bin_x_lst[i].end_value.value = src->bin_x_lst[i].end_value.value;

    // end value
    switch (dst.bin_x_lst[i].end_value.value)
    {
    case INTEGER_BIN_RANGE:
      memcpy(&dst.bin_x_lst[i].end_value.int_value, &src->bin_x_lst[i].end_value.int_value, 4);
      break;
    
    case REAL_BIN_RANGE:
      memcpy(&dst.bin_x_lst[i].end_value.real_value, &src->bin_x_lst[i].end_value.real_value, 8);
      break;

    default:
      break;
    }

  }

  // Bin Y
  assert(src->bin_y_lst == NULL && "Not yet implemented");

  // Bin Z
  assert(src->bin_z_lst == NULL && "Not yet implemented");

  return dst;
}
