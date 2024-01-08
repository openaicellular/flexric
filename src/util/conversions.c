#include <assert.h>

#include "conversions.h"

BIT_STRING_t cp_amf_region_id_to_bit_string(uint8_t src)
{
  assert(src < 64);    
  
  BIT_STRING_t dst = {.bits_unused = 0, .buf = malloc(1), .size = 1}; 
  assert(dst.buf != NULL);
  
  memcpy(dst.buf, &src, 1);    
  
  return dst;
}

uint8_t cp_amf_region_id_to_u8(BIT_STRING_t src)
{
  assert(src.bits_unused == 0);
  assert(src.size == 1);

  uint8_t dst = 0; 
  memcpy(&dst, src.buf, 1);

  return dst;
}

BIT_STRING_t cp_amf_set_id_to_bit_string(uint16_t val)
{
  assert(val < 1024);

  BIT_STRING_t dst = {0}; 
  dst.bits_unused = 6; // unused_bit;
  dst.size = 2;
  dst.buf = calloc(2, sizeof(uint8_t) ); 
  assert(dst.buf != NULL);

  dst.buf[0] = (val << 6) >> 8;
  dst.buf[1] = val << 6;

  return dst;
}

 
uint16_t cp_amf_set_id_to_u16(BIT_STRING_t src)
{
  assert(src.bits_unused == 6);
  assert(src.size == 2);
  assert(src.buf != NULL);

  uint16_t dst = (src.buf[0] << 8) >> 6  | src.buf[1] >> 6;

  return dst;
}

BIT_STRING_t cp_amf_ptr_to_bit_string(uint8_t src)
{
  assert(src < 64);

  uint8_t tmp = src << 2;

  BIT_STRING_t dst = {.bits_unused =2, .buf = malloc(1), .size = 1}; 
  assert(dst.buf != NULL);
  memcpy(dst.buf, &tmp, 1); 

  return dst;
}


uint8_t cp_amf_ptr_to_u8(BIT_STRING_t src)
{
  assert(src.bits_unused == 2);
  assert(src.size == 1);

  uint8_t dst = 0; 
  memcpy(&dst, src.buf, 1);

  dst = (dst >> 2);
  return dst;
}

BIT_STRING_t cp_nr_cell_id_to_bit_string(uint64_t val)
{
    assert(val < 68719476736); // val << 2^36

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 4; // unused_bit;
    dst.size = 5;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = val >> 16;
    dst.buf[3] = val >> 24;
    dst.buf[4] = (val >> 32) << 4; 

    return dst;
}

uint64_t cp_nr_cell_id_to_u64(BIT_STRING_t src)
{
    assert(src.bits_unused == 4);
    assert(src.size == 5);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[4] >> 4;
    dst = dst << 32 | src.buf[3] << 24 | src.buf[2] << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}

BIT_STRING_t cp_gnb_id_to_bit_string( e2ap_gnb_id_t src)
{
//  - copied from 3GPP 36.423 (X2AP) IEs
// -- note: ie-Extensions removed
// -- Note: to avoid duplicate names with XnAP, GNB-ID renamed ENGNB-ID, GlobalGNB-ID renamed
// GlobalenGNB-ID
// -- **************************************************************
// ENGNB-ID ::= CHOICE {
// gNB-ID BIT STRING (SIZE (22..32)),
// }
  assert(src.unused < 11); // [22 .. 32]  bits
  uint64_t const tmp = (uint64_t)src.nb_id >> (32 - src.unused);                        //
  assert( tmp == 0 && "Some bits are not set to zero and value would be lost");

  BIT_STRING_t dst = {0};
  dst.bits_unused = src.unused%8;
  uint32_t const bytes = src.unused > 7 ? 3 : 4;

  dst.buf = calloc(1, sizeof(uint8_t)*bytes);
  assert(dst.buf != NULL && "Memory exhausted");
  dst.size = bytes;

  if(bytes == 3){
    src.unused -= 8; 
    dst.buf[0] = 0xFF & (src.nb_id >> (16 - src.unused )); 
    dst.buf[1] = 0xFF & (src.nb_id >> (8 - src.unused)); 
    const uint8_t m = 0xFF >> src.unused;
    dst.buf[2] = (src.nb_id & m) << src.unused; 
  } else { // bytes == 4
    dst.buf[0] = 0xFF & (src.nb_id >> (24 - src.unused)); 
    dst.buf[1] = 0xFF & (src.nb_id >> (16 - src.unused)); 
    dst.buf[2] = 0xFF & (src.nb_id >> (8 - src.unused)); 
    const uint8_t m = 0xFF >> src.unused;
    dst.buf[3] = (src.nb_id & m) << src.unused; 
  }

  return dst;
}

e2ap_gnb_id_t cp_bit_string_to_gnb_id(BIT_STRING_t src)
{
  assert(src.size < 5 && "Max. 32 bits permited [22..32]");
  assert(src.size == 3 || src.size == 4);

  assert(src.bits_unused < 8);

 e2ap_gnb_id_t dst = {0};

  if(src.size == 3){
    assert(src.bits_unused < 3 && "Min. 22 bits");
    uint32_t const b0 = ((uint32_t)(src.buf[0]) << (16 - src.bits_unused) ); 
    uint32_t const b1 = ((uint32_t)(src.buf[1]) << (8 - src.bits_unused) ); 
    uint32_t const b2 = ((uint32_t)(src.buf[2]) >> src.bits_unused); 
    dst.nb_id = b0 | b1 | b2;
  } else { //src.size == 4
    uint32_t const b0 = ((uint32_t)(src.buf[0]) << (24 - src.bits_unused) ); 
    uint32_t const b1 = ((uint32_t)(src.buf[1]) << (16 - src.bits_unused) ); 
    uint32_t const b2 = ((uint32_t)(src.buf[2]) << (8 - src.bits_unused) ); 
    uint32_t const b3 = ((uint32_t)(src.buf[3]) >> src.bits_unused); 
    dst.nb_id = b0 | b1 | b2 | b3;
  }

  dst.unused = src.bits_unused + (src.size == 3 ? 8 : 0);
  return dst;
}


BIT_STRING_t cp_eutra_cell_id_to_bit_string(uint32_t val)
{
    assert(val < 268435456); // val << 28

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 4; // unused_bit;
    dst.size = 4;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = val >> 16;
    dst.buf[3] = (val >> 24) << 4; 

    return dst;
}

uint32_t cp_eutra_cell_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 4);
    assert(src.size == 4);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[3] >> 4;
    dst = dst << 24 | src.buf[2] << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}


BIT_STRING_t cp_macro_enb_id_to_bit_string(uint32_t val)
{
    assert(val < 1048576); // val << 2^20

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 4; // unused_bit;
    dst.size = 3;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = (val >> 16) << 4; 

    return dst;
}

uint32_t cp_macro_enb_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 4);
    assert(src.size == 3);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[2] >> 4;
    dst = dst << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}


BIT_STRING_t cp_home_enb_id_to_bit_string(uint32_t val)
{
    assert(val < 268435456); // val << 2^28

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 4; // unused_bit;
    dst.size = 4;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = val >> 16;
    dst.buf[3] = (val >> 24) << 4; 

    return dst;
}

uint32_t cp_home_enb_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 4);
    assert(src.size == 4);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[3] >> 4;
    dst = dst << 24 | src.buf[2] << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}

BIT_STRING_t cp_short_macro_enb_id_to_bit_string(uint32_t val)
{
    assert(val < 262144); // val << 2^18

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 6; // unused_bit;
    dst.size = 3;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = (val >> 16) << 6; 

    return dst;
}

uint32_t cp_short_macro_enb_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 6);
    assert(src.size == 3);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[2] >> 6;
    dst = dst << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}

BIT_STRING_t cp_long_macro_enb_id_to_bit_string(uint32_t val)
{
    assert(val < 2097152); // val << 2^21

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 3; // unused_bit;
    dst.size = 3;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = (val >> 16) << 3; 

    return dst;
}

uint32_t cp_long_macro_enb_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 3);
    assert(src.size == 3);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[2] >> 3;
    dst = dst << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}


BIT_STRING_t cp_macro_ng_enb_id_to_bit_string(uint32_t val)
{
    assert(val < 1048576); // val << 2^20

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 4; // unused_bit;
    dst.size = 3;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = (val >> 16) << 4; 

    return dst;
}

uint32_t cp_macro_ng_enb_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 4);
    assert(src.size == 3);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[2] >> 4;
    dst = dst << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}

BIT_STRING_t cp_short_macro_ng_enb_id_to_bit_string(uint32_t val)
{
    assert(val < 262144); // val << 2^18

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 6; // unused_bit;
    dst.size = 3;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = (val >> 16) << 6; 

    return dst;
}

uint32_t cp_short_macro_ng_enb_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 6);
    assert(src.size == 3);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[2] >> 6;
    dst = dst << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}

BIT_STRING_t cp_long_macro_ng_enb_id_to_bit_string(uint32_t val)
{
    assert(val < 2097152); // val << 2^21

    BIT_STRING_t dst = {0}; 
    dst.bits_unused = 3; // unused_bit;
    dst.size = 3;
    dst.buf = calloc(dst.size, sizeof(uint8_t)); 
    assert(dst.buf != NULL);

    dst.buf[0] = val;
    dst.buf[1] = val >> 8;
    dst.buf[2] = (val >> 16) << 3; 

    return dst;
}

uint32_t cp_long_macro_ng_enb_id_to_u32(BIT_STRING_t src)
{
    assert(src.bits_unused == 3);
    assert(src.size == 3);
    assert(src.buf != NULL);

    uint64_t dst = {0}; 
    
    dst = src.buf[2] >> 3;
    dst = dst << 16 | src.buf[1] << 8 | src.buf[0];

    return dst;
}

OCTET_STRING_t int_64_to_octet_string(uint64_t x)
{
    OCTET_STRING_t asn = {0};

    asn.buf = calloc(sizeof(x) + 1, sizeof(uint8_t));
    memcpy(asn.buf,&x,sizeof(x));
    asn.size = sizeof(x);

    return asn;
}

uint64_t octet_string_to_int_64(OCTET_STRING_t asn)
{
    uint64_t x = {0};

    memcpy(&x, asn.buf, asn.size);

    return x;
}
