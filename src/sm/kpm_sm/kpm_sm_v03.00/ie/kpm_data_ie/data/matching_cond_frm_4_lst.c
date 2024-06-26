#include <assert.h>
#include "../../../../../../util/eq.h"
#include "matching_cond_frm_4_lst.h"

void free_matching_cond_frm_4(matching_condition_format_4_lst_t* src)
{
  assert(src != NULL);

  // Test Information List
  free_test_info(&src->test_info_lst);

  // Logical OR
  if (src->logical_OR != NULL)
    free(src->logical_OR);

}

bool eq_matching_cond_frm_4(matching_condition_format_4_lst_t const * m0, matching_condition_format_4_lst_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Test Information List
  if (eq_test_info(&m0->test_info_lst, &m1->test_info_lst) != true)
    return false;

  // Logical OR
  if (eq_ptr(m0->logical_OR, m1->logical_OR, NULL) == false)
    return false;

  return true;
}

matching_condition_format_4_lst_t cp_matching_cond_frm_4(const matching_condition_format_4_lst_t * src)
{
  assert(src != NULL);

  matching_condition_format_4_lst_t dst = {0};

  // Test Information List
  dst.test_info_lst = cp_kpm_test_info(&src->test_info_lst);

  // Logical OR
  if (src->logical_OR != NULL)
  {
    dst.logical_OR = calloc(1, sizeof(enum_value_e));
    *dst.logical_OR = *src->logical_OR;
  }

  return dst;
}
