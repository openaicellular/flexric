/*
 * Copyright (c) 2004-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <ANY.h>

asn_OCTET_STRING_specifics_t asn_SPC_ANY_specs_e2ap_v1_01 = {
    sizeof(ANY_t),
    offsetof(ANY_t, _asn_ctx),
    ASN_OSUBV_ANY
};
asn_TYPE_operation_t asn_OP_ANY_e2ap_v1_01 = {
    OCTET_STRING_free_e2ap_v1_01,
#if !defined(ASN_DISABLE_PRINT_SUPPORT)
    OCTET_STRING_print_e2ap_v1_01,
#else
    0,
#endif  /* !defined(ASN_DISABLE_PRINT_SUPPORT) */
    OCTET_STRING_compare_e2ap_v1_01,
#if !defined(ASN_DISABLE_BER_SUPPORT)
    OCTET_STRING_decode_ber_e2ap_v1_01,
    OCTET_STRING_encode_der_e2ap_v1_01,
#else
    0,
    0,
#endif  /* !defined(ASN_DISABLE_BER_SUPPORT) */
#if !defined(ASN_DISABLE_XER_SUPPORT)
    OCTET_STRING_decode_xer_hex_e2ap_v1_01,
    ANY_encode_xer_e2ap_v1_01,
#else
    0,
    0,
#endif  /* !defined(ASN_DISABLE_XER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
    ANY_encode_jer,
#else
    0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
#if !defined(ASN_DISABLE_OER_SUPPORT)
    0,
    0,
#else
    0,
    0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT)
    ANY_decode_uper_e2ap_v1_01,
    ANY_encode_uper_e2ap_v1_01,
#else
    0,
    0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) */
#if !defined(ASN_DISABLE_APER_SUPPORT)
    ANY_decode_aper_e2ap_v1_01,
    ANY_encode_aper_e2ap_v1_01,
#else
    0,
    0,
#endif  /* !defined(ASN_DISABLE_APER_SUPPORT) */
    0,  /* Random fill is not defined for ANY type */
    0  /* Use generic outmost tag fetcher */
};
asn_TYPE_descriptor_t asn_DEF_ANY_e2ap_v1_01 = {
    "ANY",
    "ANY",
    &asn_OP_ANY_e2ap_v1_01,
    0, 0, 0, 0,
    {
#if !defined(ASN_DISABLE_OER_SUPPORT)
        0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
        0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
        asn_generic_no_constraint_e2ap_v1_01
    },  /* No constraints */
    0, 0,  /* No members */
    &asn_SPC_ANY_specs_e2ap_v1_01,
};
