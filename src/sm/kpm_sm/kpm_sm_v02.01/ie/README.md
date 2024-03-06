asn1c -no-gen-BER  -no-gen-UPER -no-gen-OER -no-gen-JER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps e2sm_kpm_v02.01_standard.asn1

/home/mir/workspace/asn1c_mouse/asn1c/asn1c -S /home/mir/workspace/asn1c_mouse/skeletons/ -no-gen-BER  -no-gen-UPER -no-gen-OER -no-gen-JER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps e2sm_kpm_v02.01_standard.asn1


## Bug in the standard in the type
Test Condition Info

TestCondInfo ::= SEQUENCE{
testType TestCond-Type,
testExpr TestCond-Expression (OPTIONAL),
testValue TestCond-Value (OPTIONAL),

In ASN definition the fields as mandatory, in the standard optional, following the asn definition 

