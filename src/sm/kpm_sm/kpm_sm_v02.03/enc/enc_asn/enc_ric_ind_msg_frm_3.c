#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "enc_ric_ind_msg_frm_3.h"
#include "../../ie/asn/UEMeasurementReportList.h"
#include "../../ie/asn/UEMeasurementReportItem.h"
#include "enc_ric_ind_msg_frm_1.h"
#include "../../../../../lib/sm/enc/enc_ue_id.h"


E2SM_KPM_IndicationMessage_Format3_t * kpm_enc_ind_msg_frm_3_asn(const kpm_ind_msg_format_3_t * ind_msg)
{
    E2SM_KPM_IndicationMessage_Format3_t * ind_msg_asn = calloc(1, sizeof(E2SM_KPM_IndicationMessage_Format3_t));
    assert(ind_msg_asn != NULL && "Memory exhausted");

    // List of UE Measurement Reports
    
    assert(ind_msg->ue_meas_report_lst_len >= 1 && ind_msg->ue_meas_report_lst_len <= maxnoofUEMeasReport);


    for (size_t i = 0; i<ind_msg->ue_meas_report_lst_len; i++)
    {
        UEMeasurementReportItem_t * UE_data = calloc(1, sizeof(UEMeasurementReportItem_t));
        assert(UE_data != NULL && "Memory exhausted");
        
        UE_data->ueID = enc_ue_id_asn(&ind_msg->meas_report_per_ue[i].ue_meas_report_lst);

        UE_data->measReport = kpm_enc_ind_msg_frm_1_asn(&ind_msg->meas_report_per_ue[i].ind_msg_format_1);


        int rc1 = ASN_SEQUENCE_ADD(&ind_msg_asn->ueMeasReportList.list, UE_data);
        assert(rc1 == 0);
    }


    return ind_msg_asn;
}
