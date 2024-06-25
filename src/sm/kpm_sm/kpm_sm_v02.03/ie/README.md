
## Types that change from non optional (KPM v2.01) to optional(KPM v2.02 KPM v2.03)

TestCondInfo ::= SEQUENCE{
testType TestCond-Type,
testExpr TestCond-Expression (OPTIONAL),
testValue TestCond-Value (OPTIONAL),
...

## 1. ASN.1 types - v02.03



1.1 Before
The ASN.1 file "e2sm_kpm_v02.03_standard.asn1" is taken from the O-RAN specification "O-RAN.WG3.E2SM-KPM-v02.00.033".

As per this standard, actionDefinition-Format4 and actionDefinition-Format5 are defined but not included in the E2SM-KPM-ActionDefinition:

E2SM-KPM-ActionDefinition ::= SEQUENCE{
        ric-Style-Type                                  RIC-Style-Type,
        actionDefinition-formats        CHOICE{
                actionDefinition-Format1                E2SM-KPM-ActionDefinition-Format1,
                actionDefinition-Format2                E2SM-KPM-ActionDefinition-Format2,
                actionDefinition-Format3                E2SM-KPM-ActionDefinition-Format3,
                ...,
                actionDefinition-Format4                E2SM-KPM-ActionDefinition-Format4,
                actionDefinition-Format5                E2SM-KPM-ActionDefinition-Format5
        },
        ...
}


The ASN.1 encoder doesn't recognize the structures defined after 3 dots (or the message
    "/*
     * This type is extensible,
     * possible extensions are below.
     */"
after generating the ASN.1 types).


1.2 After
To avoid this behavior, we defined new ASN.1 file called "e2sm_kpm_v02.03_modified.asn1" by moving the 3 dots at the end:

E2SM-KPM-ActionDefinition ::= SEQUENCE{
        ric-Style-Type                                  RIC-Style-Type,
        actionDefinition-formats        CHOICE{
                actionDefinition-Format1                E2SM-KPM-ActionDefinition-Format1,
                actionDefinition-Format2                E2SM-KPM-ActionDefinition-Format2,
                actionDefinition-Format3                E2SM-KPM-ActionDefinition-Format3,
                actionDefinition-Format4                E2SM-KPM-ActionDefinition-Format4,
                actionDefinition-Format5                E2SM-KPM-ActionDefinition-Format5,
                ...
        },
        ...
}

The following ASN.1 types are changed:
- E2SM-KPM-ActionDefinition
- E2SM-KPM-IndicationMessage
- E2SM-KPM-ActionDefinition-Format1
- MeasurementCondUEidItem
- MeasurementCondItem
- TestCond-Type
- TestCond-Value 


