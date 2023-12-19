#include "sm_gtp.h"
#include "../../../test/rnd/fill_rnd_data_gtp.h"
#include <assert.h>

void init_gtp_sm(void)
{
  // No data allocated
}

void free_gtp_sm(void)
{
  // No data allocated
}

bool read_gtp_sm(void * data)
{
  assert(data != NULL);

  gtp_ind_data_t* gtp = (gtp_ind_data_t*)(data);
  fill_gtp_ind_data(gtp);
  return true;
}

void read_gtp_setup_sm(void* data)
{
  assert(data != NULL);
  assert(0 !=0 && "Not supported");
}

sm_ag_if_ans_t write_ctrl_gtp_sm(void const* src)
{
  assert(src != NULL);
  assert(0 !=0 && "Not supported");
}

