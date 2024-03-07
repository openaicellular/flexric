#ifndef ENCODE_3GPP_ASN_MIR_H
#define ENCODE_3GPP_ASN_MIR_H 

#if defined(KPM_V2_01)

  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/asn_constant.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-GNB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-GNB-CU-CP-E1AP-ID-Item.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-GNB-CU-F1AP-ID-List.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-GNB-CU-CP-F1AP-ID-Item.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-GNB-CU-UP.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-GNB-DU.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-NG-ENB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-NG-ENB-DU.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-ENB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/UEID-EN-GNB.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/GlobalENB-ID.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/GlobalGNB-ID.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/GlobalNgENB-ID.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/GlobalNGRANNodeID.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn/asn_SEQUENCE_OF.h"

#elif defined(KPM_V2_03)

  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/asn_constant.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-GNB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-GNB-CU-CP-E1AP-ID-Item.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-GNB-CU-F1AP-ID-List.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-GNB-CU-CP-F1AP-ID-Item.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-GNB-CU-UP.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-GNB-DU.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-NG-ENB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-NG-ENB-DU.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-ENB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/UEID-EN-GNB.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/GlobalENB-ID.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/GlobalGNB-ID.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/GlobalNgENB-ID.h"
  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/GlobalNGRANNodeID.h"

  #include "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn/asn_SEQUENCE_OF.h"

#elif defined(KPM_V3_00)

  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/asn_constant.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-GNB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-GNB-CU-CP-E1AP-ID-Item.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-GNB-CU-F1AP-ID-List.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-GNB-CU-CP-F1AP-ID-Item.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-GNB-CU-UP.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-GNB-DU.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-NG-ENB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-NG-ENB-DU.h"

  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-ENB.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/UEID-EN-GNB.h"

  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/GlobalENB-ID.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/GlobalGNB-ID.h"

  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/GlobalNgENB-ID.h"
  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/GlobalNGRANNodeID.h"

  #include "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn/asn_SEQUENCE_OF.h"

#elif defined RC_SM

  #include "../../../sm/rc_sm/ie/asn/asn_constant.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-Item.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-F1AP-ID-List.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-F1AP-ID-Item.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-UP.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-GNB-DU.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-NG-ENB.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-NG-ENB-DU.h"

  #include "../../../sm/rc_sm/ie/asn/UEID-ENB.h"
  #include "../../../sm/rc_sm/ie/asn/UEID-EN-GNB.h"

  #include "../../../sm/rc_sm/ie/asn/GlobalENB-ID.h"
  #include "../../../sm/rc_sm/ie/asn/GlobalGNB-ID.h"

  #include "../../../sm/rc_sm/ie/asn/GlobalNgENB-ID.h"
  #include "../../../sm/rc_sm/ie/asn/GlobalNGRANNodeID.h"

  #include "../../../sm/rc_sm/ie/asn/asn_SEQUENCE_OF.h"

#else
  _Static_assert(0 !=0 && "Unknown macro type");

#endif

#endif
