#include "connected_mobility_amr.h"

#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"


void free_connected_mobility_amr(connected_mobility_amr_t* src)
{
  assert(src != NULL);

  //Mandatory. 
  //bool scell_config;

  //Mandatory. 
  //bool nr_handover;

  //Mandatory. 
  //bool nr_cell_redirect;

  //Mandatory. 
  //bool eutra_handover;

  //Mandatory. 
  //bool eutra_cell_redirect;

  //Mandatory. 
  //bool nr_dc_setup;

  //Mandatory. 
  //bool mr_dc_release;
}

connected_mobility_amr_t parse_connected_mobility_amr(void* it)
{
  assert(it != NULL);
  connected_mobility_amr_t dst = {0}; 

  //Mandatory. 
  dst.scell_config = parse_int(it, "scell_config");

  //Mandatory. 
  dst.nr_handover = parse_int(it, "nr_handover");

  //Mandatory.
  dst.nr_cell_redirect = parse_int(it, "nr_cell_redirect");

  //Mandatory.
  dst.eutra_handover = parse_int(it, "eutra_handover");

  //Mandatory.
  dst.eutra_cell_redirect = parse_int(it, "eutra_cell_redirect");

  //Mandatory.
  dst.nr_dc_setup = parse_int(it, "nr_dc_setup");

  //Mandatory.
  dst.mr_dc_release = parse_int(it, "mr_dc_release");

  return dst;
}

