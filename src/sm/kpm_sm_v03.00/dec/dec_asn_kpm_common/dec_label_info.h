#ifndef DECODE_LABEL_INFO_FORMAT_1_KPM_V3_H
#define DECODE_LABEL_INFO_FORMAT_1_KPM_V3_H

#include "../../ie/asn/LabelInfoItem.h"
#include "../../ie/kpm_data_ie/data/label_info_lst.h"

label_info_lst_t kpm_dec_label_info_asn(const MeasurementLabel_t * meas_label_asn);

/* S-NSSAI */
typedef struct SNSSAI {
	OCTET_STRING_t	 sST;
	OCTET_STRING_t	*sD;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SNSSAI_t;


#endif
