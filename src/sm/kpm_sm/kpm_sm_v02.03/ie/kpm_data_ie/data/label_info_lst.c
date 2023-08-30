#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "label_info_lst.h"


void cp_label_info(label_info_lst_t *dst, label_info_lst_t const *src) 
{
  assert(src != NULL);
  assert(dst != NULL);

  if (src->noLabel != NULL)
  {
    dst->noLabel = calloc(1, sizeof(enum_value_e));
    dst->noLabel = src->noLabel;
  }
  
  if (src->plmn_id != NULL) {
    dst->plmn_id = malloc(sizeof(*dst->plmn_id));
    *dst->plmn_id = *src->plmn_id;
  }

  // TO BE COMPLETED with the other fields
}

void free_label_info(label_info_lst_t *l) 
{
  assert(l != NULL);

  if (l->noLabel)
    free(l->noLabel);
  if (l->plmn_id)
    free(l->plmn_id);
 
  if (l->sliceID != NULL) {
    assert(false && "not implemented");
  }
	if (l->fiveQI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qFI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCImax != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCImin != NULL) {
    assert(false && "not implemented");
  }
	if (l->aRPmax != NULL) {
    assert(false && "not implemented");
  }
	if (l->aRPmin != NULL) {
    assert(false && "not implemented");
  }
	if (l->bitrateRange != NULL) {
    assert(false && "not implemented");
  }
	if (l->layerMU_MIMO != NULL) {
    assert(false && "not implemented");
  }
	if (l->sUM != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinX != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinY != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinZ != NULL) {
    assert(false && "not implemented");
  }
	if (l->preLabelOverride != NULL) {
    assert(false && "not implemented");
  }
	if (l->startEndInd != NULL) {
    assert(false && "not implemented");
  }
	if (l->min != NULL) {
    assert(false && "not implemented");
  }
	if (l->max != NULL) {
    assert(false && "not implemented");
  }
	if (l->avg != NULL) {
    assert(false && "not implemented");
  }
}


bool eq_label_info(const label_info_lst_t *l1, const label_info_lst_t *l2)
{
  assert(l1 != NULL);
  assert(l2 != NULL);

  if (l1->noLabel != NULL && l2->noLabel != NULL && l1->noLabel != l2->noLabel)
    return false;    
  
  if (eq_e2sm_plmn(l1->plmn_id, l2->plmn_id) != true)
    return false;

  // the rest of optional parameters not yet implemented

  return true;
}
