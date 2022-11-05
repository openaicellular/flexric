#include "../ie/asn/E2SM-KPM-EventTriggerDefinition-Format1.h"
#include "../ie/asn/E2SM-KPM-EventTriggerDefinition.h"
#include "../ie/asn/E2SM-KPM-IndicationHeader.h"
#include "../ie/asn/E2SM-KPM-IndicationHeader-Format1.h"
#include "../ie/asn/E2SM-KPM-IndicationMessage.h"
#include "../ie/asn/E2SM-KPM-IndicationMessage-Format1.h"
#include "../ie/asn/MeasurementDataItem.h"
#include "../ie/asn/MeasurementData.h"
#include "../ie/asn/MeasurementRecord.h"
#include "../ie/asn/MeasurementRecordItem.h"
#include "../ie/asn/E2SM-KPM-RANfunction-Description.h"
#include "../ie/asn/RANfunction-Name.h"
#include "../ie/asn/E2SM-KPM-ActionDefinition.h"
#include "../ie/asn/E2SM-KPM-ActionDefinition-Format1.h"
#include "../ie/asn/MeasurementInfoItem.h"
#include "../ie/asn/MeasurementType.h"
#include "../ie/asn/LabelInfoItem.h"
#include "../ie/asn/asn_constant.h"

#include "kpm_dec_asn.h"
#include "../../../util/conversions.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void cp_label_info_item_from_asn(adapter_LabelInfoItem_t *dst, LabelInfoItem_t const *src);

kpm_event_trigger_t kpm_dec_event_trigger_asn(size_t len, uint8_t const ev_tr[len])
{
  assert(len>0);
  assert(ev_tr != NULL);

  E2SM_KPM_EventTriggerDefinition_t *pdu = calloc(1, sizeof(E2SM_KPM_EventTriggerDefinition_t));
  assert( pdu !=NULL && "Memory exhausted" );

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_KPM_EventTriggerDefinition, (void**)&pdu, ev_tr, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGNED_BASIC_PER syntax?");

  xer_fprint(stderr, &asn_DEF_E2SM_KPM_EventTriggerDefinition, pdu);

  kpm_event_trigger_t ret = {0};

  assert ( pdu->eventDefinition_formats.present == E2SM_KPM_EventTriggerDefinition__eventDefinition_formats_PR_eventDefinition_Format1 && "Invalid Choice"  );

  ret.ms = pdu->eventDefinition_formats.choice.eventDefinition_Format1->reportingPeriod;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_KPM_EventTriggerDefinition,pdu);
  free(pdu);

  return ret;
}


kpm_action_def_t kpm_dec_action_def_asn(size_t len, uint8_t const action_def[len]) 
{
  assert(len>0);
  assert(action_def != NULL);

  kpm_action_def_t ret = {0};

  E2SM_KPM_ActionDefinition_t *pdu = calloc(1, sizeof(E2SM_KPM_ActionDefinition_t));
  assert( pdu !=NULL && "Memory exhausted" );
  
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_KPM_ActionDefinition, (void**)&pdu, action_def, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGNED_BASIC_PER syntax?");
  // note that constraints checking on data extracted is already performed by asn_decode()

  xer_fprint(stderr, &asn_DEF_E2SM_KPM_ActionDefinition, pdu);

  ret.ric_style_type = pdu->ric_Style_Type;
  
  switch (pdu->actionDefinition_formats.present)
  {
    case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_NOTHING:
      // manages backward compatibility
      break;
    case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format1:
      ; 
      E2SM_KPM_ActionDefinition_Format1_t *adf_p = pdu->actionDefinition_formats.choice.actionDefinition_Format1;
      
      // 1. Extract 'measInfoList'
      ret.MeasInfo_len = adf_p->measInfoList.list.count;
      
      ret.MeasInfo = calloc(ret.MeasInfo_len, sizeof(MeasInfo_t));
      assert (ret.MeasInfo != NULL && "Memory exhausted");

      for (size_t i = 0; i<ret.MeasInfo_len; i++)
      { 
        switch (adf_p->measInfoList.list.array[i]->measType.present)
        {
          case MeasurementType_PR_NOTHING:// manages backward compatibility
            continue;
          case MeasurementType_PR_measName:
            BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING(ret.MeasInfo[i].measName, adf_p->measInfoList.list.array[i]->measType.choice.measName);
            ret.MeasInfo[i].meas_type = KPM_V2_MEASUREMENT_TYPE_NAME;
            break;
          case MeasurementType_PR_measID:
            ret.MeasInfo[i].measID = adf_p->measInfoList.list.array[i]->measType.choice.measID;
            ret.MeasInfo[i].meas_type =  KPM_V2_MEASUREMENT_TYPE_ID;
            break;
          default:
            break;
        }
        ret.MeasInfo[i].labelInfo_len = adf_p->measInfoList.list.array[i]->labelInfoList.list.count;
        ret.MeasInfo[i].labelInfo = calloc(ret.MeasInfo[i].labelInfo_len, sizeof(adapter_LabelInfoItem_t));
        assert (ret.MeasInfo[i].labelInfo != NULL && "Memory exhausted");
        for (size_t j = 0; j<ret.MeasInfo[i].labelInfo_len; j++)
          cp_label_info_item_from_asn(&ret.MeasInfo[i].labelInfo[j], adf_p->measInfoList.list.array[i]->labelInfoList.list.array[j]);       
      }
      // 2. granular period
      ret.granularity_period = adf_p->granulPeriod;

      // 3. cell Global ID (optional)
      // TO-COMPLETE
      break;
    default:
      assert( 0!=0 && "unexpected action defintion type");
  }
  
  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_KPM_ActionDefinition, pdu);
  free(pdu);
  return ret;
  
}

kpm_ind_hdr_t kpm_dec_ind_hdr_asn(size_t len, uint8_t const ind_hdr[len])
{
  assert(len>0);
  assert(ind_hdr != NULL);

  kpm_ind_hdr_t ret = {0};

  E2SM_KPM_IndicationHeader_t *pdu = calloc(1, sizeof(E2SM_KPM_IndicationHeader_t));
  assert( pdu !=NULL && "Memory exhausted" );

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_KPM_IndicationHeader, (void**)&pdu, ind_hdr, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGNED_BASIC_PER syntax?");

  xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationHeader, pdu);

  if (pdu->indicationHeader_formats.present == E2SM_KPM_IndicationHeader__indicationHeader_formats_PR_indicationHeader_Format1)
  {
    E2SM_KPM_IndicationHeader_Format1_t *hdr = pdu->indicationHeader_formats.choice.indicationHeader_Format1;

    uint32_t reversed_ts;
    OCTET_STRING_TO_INT32(&hdr->colletStartTime, reversed_ts);
    ret.collectStartTime = ntohl(reversed_ts);
    
    if (hdr->fileFormatversion){
      ret.fileFormatversion = calloc(1, sizeof(*ret.fileFormatversion));
      assert(ret.fileFormatversion !=NULL && "Memory exhausted" );
      BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING_POINTERS(ret.fileFormatversion, hdr->fileFormatversion);
    }

    if (hdr->senderName){

      ret.senderName = calloc(1, sizeof(*ret.senderName));
      assert(ret.senderName !=NULL && "Memory exhausted" );
      BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING_POINTERS(ret.senderName, hdr->senderName);
    }

    if (hdr->senderType){
      ret.senderType = calloc(1, sizeof(*ret.senderType));
      assert(ret.senderType !=NULL && "Memory exhausted" );
      BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING_POINTERS(ret.senderType, hdr->senderType);
    }

    if (hdr->vendorName){
      ret.vendorName = calloc(1, sizeof(*ret.vendorName));
      assert(ret.vendorName !=NULL && "Memory exhausted" );
      BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING_POINTERS(ret.vendorName, hdr->vendorName);
    }
  }

	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_KPM_IndicationHeader, pdu);
  free(pdu);

  return ret;
}

kpm_ind_msg_t kpm_dec_ind_msg_asn(size_t len, uint8_t const ind_msg[len])
{
  assert(ind_msg != NULL);
  assert(len>0);

  kpm_ind_msg_t ret = {0};

  E2SM_KPM_IndicationMessage_t *pdu = calloc(1, sizeof(E2SM_KPM_IndicationMessage_t));
  assert( pdu !=NULL && "Memory exhausted" );

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_KPM_IndicationMessage, (void**)&pdu, ind_msg, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGNED_BASIC_PER syntax?");

  xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationMessage, pdu);

  if (pdu->indicationMessage_formats.present == E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1)
  {
    E2SM_KPM_IndicationMessage_Format1_t *msg = pdu->indicationMessage_formats.choice.indicationMessage_Format1;

    // 1. MeasData
    adapter_MeasDataItem_t *mData = calloc(msg->measData.list.count, sizeof(adapter_MeasDataItem_t));
    ret.MeasData = mData;
    ret.MeasData_len = msg->measData.list.count;
    for (size_t i = 0; i< (size_t)msg->measData.list.count; i++)
    {
      MeasurementDataItem_t *item = msg->measData.list.array[i]; 
      mData[i].incompleteFlag = (item->incompleteFlag) ? 0 : -1;
      adapter_MeasRecord_t * rec = calloc(item->measRecord.list.count, sizeof(adapter_MeasRecord_t));
      for (size_t j = 0; j < (size_t) item->measRecord.list.count; j++){
        switch (item->measRecord.list.array[j]->present){

        case MeasurementRecordItem_PR_noValue:
          rec[j].type = MeasRecord_noval;
          break;

        case MeasurementRecordItem_PR_integer:
          rec[j].type = MeasRecord_int;
          rec[j].int_val = item->measRecord.list.array[j]->choice.integer;
          break;

        case  MeasurementRecordItem_PR_real: 
          rec[j].type = MeasRecord_real;
          rec[j].real_val = item->measRecord.list.array[j]->choice.real;
          break;

        default:
          assert( 0!=0 && "Unknown type");
          break;
        } 
      }
      mData[i].measRecord_len = item->measRecord.list.count;
      mData[i].measRecord = rec;
    }
    // 2. measInfoList (OPTIONAL)
    if (msg->measInfoList != NULL)
    {
      ret.MeasInfo_len = msg->measInfoList->list.count; 
      ret.MeasInfo = calloc(ret.MeasInfo_len, sizeof(MeasInfo_t));
      for (int i=0; i<msg->measInfoList->list.count; i++)
      {
        MeasurementInfoItem_t * mInfo = msg->measInfoList->list.array[i];
        switch (mInfo->measType.present)
        {
        case MeasurementType_PR_measName:
          ret.MeasInfo[i].meas_type =  KPM_V2_MEASUREMENT_TYPE_NAME; 
          BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING(ret.MeasInfo[i].measName, mInfo->measType.choice.measName);
          break;
        case MeasurementType_PR_measID:
          ret.MeasInfo[i].meas_type =  KPM_V2_MEASUREMENT_TYPE_ID; 
          ret.MeasInfo[i].measID = mInfo->measType.choice.measID;
          break;
        default:
          break;
        }
        ret.MeasInfo[i].labelInfo_len = mInfo->labelInfoList.list.count;
        ret.MeasInfo[i].labelInfo = calloc(ret.MeasInfo->labelInfo_len, sizeof(adapter_LabelInfoItem_t));
        assert (ret.MeasInfo[i].labelInfo != NULL && "Memory exhausted");
        for (int j=0; j<mInfo->labelInfoList.list.count; j++)
          cp_label_info_item_from_asn(&ret.MeasInfo[i].labelInfo[j], mInfo->labelInfoList.list.array[j]);
      }
    }

    // 3. granulPeriod (OPTIONAL)
    assert(msg->granulPeriod == NULL && "Decoding of granulPeriod not implemented yet");
  }
  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_KPM_IndicationMessage, pdu);
  free(pdu); 

  return ret;
}

kpm_func_def_t kpm_dec_func_def_asn(size_t len, uint8_t const func_def[len])
{
  assert(func_def != NULL);
  assert(len>0);

  kpm_func_def_t ret = {0};

  E2SM_KPM_RANfunction_Description_t *pdu = calloc(1, sizeof(E2SM_KPM_RANfunction_Description_t));
  assert( pdu !=NULL && "Memory exhausted" );

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_KPM_RANfunction_Description, (void**)&pdu, func_def, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGNED_BASIC_PER syntax?");

  xer_fprint(stderr, &asn_DEF_E2SM_KPM_RANfunction_Description, pdu);
  
  BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING (ret.ranFunction_Name.Description, pdu->ranFunction_Name.ranFunction_Description);
  BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING (ret.ranFunction_Name.E2SM_OID, pdu->ranFunction_Name.ranFunction_E2SM_OID);
  BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING (ret.ranFunction_Name.ShortName,pdu->ranFunction_Name.ranFunction_ShortName);
  
  assert(pdu->ranFunction_Name.ranFunction_Instance == NULL && "Decoding of ranFunction_Instance not implemented yet");
  
  assert(pdu->ric_EventTriggerStyle_List == NULL && "Decoding of ric_EventTriggerStyle_List not implemented yet");
  assert(pdu->ric_ReportStyle_List== NULL && "Decoding of ric_ReportStyle_List not implemented yet");

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_KPM_RANfunction_Description, pdu);
  free(pdu); 
  
  return ret;
}

static void cp_label_info_item_from_asn(adapter_LabelInfoItem_t *dst, LabelInfoItem_t const *src)
{
  assert(dst != NULL);

  if (src->measLabel.noLabel)
  {
    dst->noLabel = malloc(sizeof(*(dst->noLabel)));
    assert (dst->noLabel != NULL && "Memory exhausted");        
    *(dst->noLabel) = *(src->measLabel.noLabel);
      /* 
     * specification mentions that if 'noLabel' is included, other elements in the same datastructure 
     * 'LabelInfoItem_t' shall not be included.
     */
    return;
  }
 
  if (src->measLabel.plmnID != NULL)
    BYTE_ARRAY_HEAP_CP_FROM_OCTET_STRING_POINTERS (dst->plmnID, src->measLabel.plmnID);

  // To complete with below fields.
  // internal_S_NSSAI_t	          *sliceID;	/* OPTIONAL */
  // internal_FiveQI_t	          *fiveQI;	/* OPTIONAL */
  // internal_QosFlowIdentifier_t	*qFI;	    /* OPTIONAL */
  // internal_QCI_t	              *qCI;	    /* OPTIONAL */
  // internal_QCI_t	              *qCImax;	/* OPTIONAL */
  // internal_QCI_t	              *qCImin;	/* OPTIONAL */
  // long	              *aRPmax;	/* OPTIONAL */
  // long	              *aRPmin;	/* OPTIONAL */
  // long	              *bitrateRange;/* OPTIONAL */
  // long	              *layerMU_MIMO;/* OPTIONAL */
  // long	              *sUM;	    /* OPTIONAL */
  // long	              *distBinX;/* OPTIONAL */
  // long	              *distBinY;/* OPTIONAL */
  // long	              *distBinZ;/* OPTIONAL */
  // long	              *preLabelOverride;/* OPTIONAL */
  // long	              *startEndInd;	/* OPTIONAL */
  // long	              *min;	    /* OPTIONAL */
  // long	              *max;	    /* OPTIONAL */
  // long	              *avg;	    /* OPTIONAL */

} 
