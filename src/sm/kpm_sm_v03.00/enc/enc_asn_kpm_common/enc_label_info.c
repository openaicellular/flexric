#include <stdint.h>
#include <stdlib.h>

#include "enc_label_info.h"

#include "../../../../util/conversions.h"


LabelInfoItem_t * kpm_enc_label_info_asn(const label_info_lst_t * label_info)
{ 
    LabelInfoItem_t * label_info_asn = calloc(1, sizeof(LabelInfoItem_t));
    assert (label_info_asn != NULL && "Memory exhausted");

    if (label_info->noLabel != NULL) {
      printf("Encoding no label\n");
      assert(*label_info->noLabel == TRUE_ENUM_VALUE && "has only one value (true)");
      label_info_asn->measLabel.noLabel = malloc (sizeof(*(label_info_asn->measLabel.noLabel)));
      assert (label_info_asn->measLabel.noLabel != NULL && "Memory exhausted");
      *label_info_asn->measLabel.noLabel = 0;
      /* 
       * specification mentions that if 'noLabel' is included, other elements in the same datastructure 
       * 'LabelInfoItem_t' shall not be included.
       */
      return label_info_asn;
    }      

    if (label_info->plmn_id != NULL){
      label_info_asn->measLabel.plmnID = calloc(1, sizeof(*label_info_asn->measLabel.plmnID));
      MCC_MNC_TO_PLMNID(label_info->plmn_id->mcc, label_info->plmn_id->mnc, label_info->plmn_id->mnc_digit_len, label_info_asn->measLabel.plmnID);
    }

    if (label_info->sliceID != NULL) {
      label_info_asn->measLabel.sliceID = (struct S_NSSAI*)calloc(1, sizeof(struct S_NSSAI));
      label_info_asn->measLabel.sliceID->sST = label_info->sliceID->sST;
      // for(int i=0;i<3;i++){
      //   label_info_asn->measLabel.sliceID->sD[i] = malloc(sizeof(uint8_t));
      //   label_info_asn->measLabel.sliceID->sD[i][0] = label_info->sliceID->sD[i];
      // }
      // assert(false && "not implemented");
    }
    if (label_info->fiveQI != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->qFI != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->qCI != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->qCImax != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->qCImin != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->aRPmax != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->aRPmin != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->bitrateRange != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->layerMU_MIMO != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->sUM != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->distBinX != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->distBinY != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->distBinZ != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->preLabelOverride != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->startEndInd != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->min != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->max != NULL) {
      assert(false && "not implemented");
    }
    if (label_info->avg != NULL) {
      assert(*label_info->avg == TRUE_ENUM_VALUE && "has only one value (true)");
      label_info_asn->measLabel.avg = malloc (sizeof(*(label_info_asn->measLabel.avg)));
      assert (label_info_asn->measLabel.avg!= NULL && "Memory exhausted");
      *label_info_asn->measLabel.avg = 0; // or TRUE type
    }

    return label_info_asn;


}
