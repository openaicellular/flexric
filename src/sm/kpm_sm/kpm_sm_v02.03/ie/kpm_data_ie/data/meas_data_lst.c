#include <assert.h>
#include <math.h>

#include "meas_data_lst.h"

bool eq_meas_data_lst(meas_data_lst_t const* m0, meas_data_lst_t const* m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (m0->meas_record_len != m1->meas_record_len)
      return false;
    for (size_t j = 0; j < m0->meas_record_len; ++j)
    {
      
      if (m0->meas_record_lst[j].value != m1->meas_record_lst[j].value)
        return false;

      switch (m0->meas_record_lst[j].value)
      {
        case INTEGER_MEAS_VALUE:
          if (m0->meas_record_lst[j].int_val != m1->meas_record_lst[j].int_val)
            return false;
          break;

        case REAL_MEAS_VALUE:
          if (fabs(m0->meas_record_lst[j].real_val - m1->meas_record_lst[j].real_val) > 0.0001f)
            return false;
          break;

        case NO_VALUE_MEAS_VALUE:
          if (m0->meas_record_lst[j].no_value != m1->meas_record_lst[j].no_value)
            return false;
          break;

        default:
          assert(false && "Unknown Measurement Value");
      }

      
    }


    if ((m0->incomplete_flag != NULL || m1->incomplete_flag != NULL) && *m0->incomplete_flag != *m1->incomplete_flag)
      return false;


    return true;
}
