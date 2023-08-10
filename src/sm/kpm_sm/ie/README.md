# Overview

We provide O-RAN KPM Service Model, v2.03 and v3.00.

The ASN.1 files are generated according to the chosen version.

# 1. ASN.1 types - v2.03

## 1.1 Before
The ASN.1 file "e2sm_kpm_v2_03_standard.asn1" is taken from the O-RAN specification "O-RAN.WG3.E2SM-KPM-v02.03".

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


## 1.2 After
To avoid this behavior, we defined new ASN.1 file called "e2sm_kpm_v2_03_modified.asn1" by moving the 3 dots at the end:

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

# 2. ASN.1 types - v3.00
In the same manner, ASN.1 types for v3.00 are generated in the same way.
