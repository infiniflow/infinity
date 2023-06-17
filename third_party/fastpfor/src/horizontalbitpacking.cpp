/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire
 */

/**
 * Implementation of
 *
 * Willhalm T, Popovici N, Boshmaf Y, Plattner H, Zeier A, Schaffner J.
 * SIMD-scan: ultra fast in-memory table scan using on-chip vector processing
 * units.
 * Proceedings of the VLDB Endowment Aug 2009; 2(1):385Ð394.
 *
 * Optimized for a recent Intel core i7 processor by D. Lemire on Oct. 2012.
 */

#include "horizontalbitpacking.h"

namespace FastPForLib {

using namespace std;

static void SIMD_nullunpacker32(const uint8_t *, uint32_t *out) {
  memset(out, 0, 32 * 4 * 4);
}

// Did not get good results with this:
/*


static void simdhunpack1(const uint8_t *  in,uint32_t *  out) {
    __m128i* pCurr = reinterpret_cast<__m128i*>(out);
    const static __m128i andmask = _mm_set_epi8
(-128,1<<6,1<<5,1<<4,1<<3,1<<2,1<<1,1<<0,-128,1<<6,1<<5,1<<4,1<<3,1<<2,1<<1,1<<0);
    const static __m128i shufmask = _mm_set_epi8
(1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0);
    const static __m128i allones = _mm_set1_epi8 (1);
    const static __m128i finalshufmask1 = _mm_set_epi8
(-128,-128,-128,3,-128,-128,-128,2,-128,-128,-128,1,-128,-128,-128,0);
    const static __m128i finalshufmask2 = _mm_set_epi8
(-128,-128,-128,7,-128,-128,-128,6,-128,-128,-128,5,-128,-128,-128,4);
    const static __m128i finalshufmask3 = _mm_set_epi8
(-128,-128,-128,11,-128,-128,-128,10,-128,-128,-128,9,-128,-128,-128,8);
    const static __m128i finalshufmask4 = _mm_set_epi8
(-128,-128,-128,15,-128,-128,-128,14,-128,-128,-128,13,-128,-128,-128,12);
    for (uint32_t j = 0; j<8;++j) {
        __m128i ba = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in + 2 *
j));
        __m128i ca = _mm_shuffle_epi8(ba, shufmask);
        ca=_mm_and_si128(ca,andmask);
        ca = _mm_cmpeq_epi8(ca,andmask);
        ca = _mm_and_si128(ca,allones);
        _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,finalshufmask1));
        //ca=_mm_srli_si128 (ca, 4);
        //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,finalshufmask1));
        //ca=_mm_srli_si128 (ca, 4);
        //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,finalshufmask1));
        //ca=_mm_srli_si128 (ca, 4);
        //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,finalshufmask1));
         _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,finalshufmask2));
         _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,finalshufmask3));
         _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,finalshufmask4));
    }
}


 */
const static __m128i shufkey1_1 = {-9187202500191551488, -9187202500191551488};
const static __m128i shufkey2_1 = {-9187202500191551488, -9187202500191551488};
const static __m128i multi1_1 = {17179869192, 4294967298};
const static __m128i multi2_1 = {17179869192, 4294967298};
const static __m128i mask_1 = {4294967297, 4294967297};

static void simdhunpack1(const uint8_t *in, uint32_t *out) {
  enum { b = 1 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 3 };
  enum { shift2 = 7 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_1);
    ca = _mm_mullo_epi32(ca, multi1_1);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_1);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_1);
    ca = _mm_mullo_epi32(ca, multi2_1);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_1);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_2 = {-9187202500191551488, -9187202500191551488};
const static __m128i shufkey2_2 = {-9187202500191551488, -9187202500191551488};
const static __m128i multi1_2 = {68719476800, 4294967300};
const static __m128i multi2_2 = {68719476800, 4294967300};
const static __m128i mask_2 = {12884901891, 12884901891};

static void simdhunpack2(const uint8_t *in, uint32_t *out) {
  enum { b = 2 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_2);
    ca = _mm_mullo_epi32(ca, multi1_2);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_2);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_2);
    ca = _mm_mullo_epi32(ca, multi2_2);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_2);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_3 = {-9187202500191551488, -9187202495896616704};
const static __m128i shufkey2_3 = {-9187342138168279040, -9187202495896584191};
const static __m128i multi1_3 = {34359738432, 137438953473};
const static __m128i multi2_3 = {4294967304, 17179869216};
const static __m128i mask_3 = {30064771079, 30064771079};

static void simdhunpack3(const uint8_t *in, uint32_t *out) {
  enum { b = 3 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 7 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_3);
    ca = _mm_mullo_epi32(ca, multi1_3);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_3);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_3);
    ca = _mm_mullo_epi32(ca, multi2_3);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_3);
    _mm_store_si128(pCurr++, ca);
  }
}
// Did not get good results with this:
/*


static  void simdhunpack4(const uint8_t *  in,uint32_t *  out) {

    enum{ b = 4};
    __m128i* pCurr = reinterpret_cast<__m128i*>(out);
    const static __m128i shuf8 =  _mm_set_epi8 (13,12, 15,14, 9,8, 11,10, 5,4,
7,6, 1,0, 3,2);
    const static __m128i shufoneoutoftwo =  _mm_set1_epi32 (252645135);
    const static __m128i shufmid8_1 =  _mm_set_epi8 (-128, -128, -128, 3, -128,
-128, -128, 1, -128, -128, -128, 2, -128, -128, -128, 0);
    const static __m128i shufmid8_2 =  _mm_set_epi8 (-128, -128, -128, 7, -128,
-128, -128, 5, -128, -128, -128, 6, -128, -128, -128, 4);
    const static __m128i shufmid8_3 =  _mm_set_epi8 (-128, -128, -128, 11, -128,
-128, -128, 9, -128, -128, -128, 10, -128, -128, -128, 8);
    const static __m128i shufmid8_4 =  _mm_set_epi8 (-128, -128, -128, 15, -128,
-128, -128, 13, -128, -128, -128, 14, -128, -128, -128, 12);
    for (uint32_t j = 0; j<4;++j) {
        const __m128i ba = _mm_load_si128(reinterpret_cast<const __m128i*>(in +
16 * j));
        __m128i ca = _mm_srli_epi16(ba,4);
        ca = _mm_shuffle_epi8(ca,shuf8);
        ca = _mm_blend_epi16 (ba, ca,170);
        ca =  _mm_and_si128(ca,shufoneoutoftwo);
       const __m128i altba = _mm_shuffle_epi8(ba,shuf8);
       __m128i altca = _mm_srli_epi16(altba,4);
       altca = _mm_shuffle_epi8(altca,shuf8);
       altca = _mm_blend_epi16 (altba, altca,170);
       altca =  _mm_and_si128(altca,shufoneoutoftwo);
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,shufmid8_1) );
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(altca,shufmid8_1) );
       //ca=_mm_srli_si128 (ca, 4);
       //altca=_mm_srli_si128 (altca, 4);
       //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,shufmid8_1) );
       //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(altca,shufmid8_1) );
       //ca=_mm_srli_si128 (ca, 4);
       //altca=_mm_srli_si128 (altca, 4);
       //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,shufmid8_1) );
       //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(altca,shufmid8_1) );
       //ca=_mm_srli_si128 (ca, 4);
       //altca=_mm_srli_si128 (altca, 4);
       //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,shufmid8_1) );
       //_mm_store_si128(pCurr++ , _mm_shuffle_epi8(altca,shufmid8_1) );
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,shufmid8_2) );
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(altca,shufmid8_2) );
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,shufmid8_3) );
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(altca,shufmid8_3) );
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(ca,shufmid8_4) );
       _mm_store_si128(pCurr++ , _mm_shuffle_epi8(altca,shufmid8_4) );
    }
}


 */
const static __m128i shufkey1_4 = {-9187202500191551488, -9187202495896584191};
const static __m128i shufkey2_4 = {-9187202500191551488, -9187202495896584191};
const static __m128i multi1_4 = {4294967312, 4294967312};
const static __m128i multi2_4 = {4294967312, 4294967312};
const static __m128i mask_4 = {64424509455, 64424509455};

static void simdhunpack4(const uint8_t *in, uint32_t *out) {
  enum { b = 4 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 4 };
  enum { shift2 = 4 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_4);
    ca = _mm_mullo_epi32(ca, multi1_4);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_4);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_4);
    ca = _mm_mullo_epi32(ca, multi2_4);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_4);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_5 = {-9187342138168279040, -9187341034361683967};
const static __m128i shufkey2_5 = {-9187202495896616704, -9187202491601649151};
const static __m128i multi1_5 = {17179869312, 4294967328};
const static __m128i multi2_5 = {137438953476, 34359738369};
const static __m128i mask_5 = {133143986207, 133143986207};

static void simdhunpack5(const uint8_t *in, uint32_t *out) {
  enum { b = 5 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 7 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_5);
    ca = _mm_mullo_epi32(ca, multi1_5);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_5);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_5);
    ca = _mm_mullo_epi32(ca, multi2_5);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_5);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_6 = {-9187342138168279040, -9187202491601649151};
const static __m128i shufkey2_6 = {-9187342138168279040, -9187202491601649151};
const static __m128i multi1_6 = {4294967360, 68719476740};
const static __m128i multi2_6 = {4294967360, 68719476740};
const static __m128i mask_6 = {270582939711, 270582939711};

static void simdhunpack6(const uint8_t *in, uint32_t *out) {
  enum { b = 6 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_6);
    ca = _mm_mullo_epi32(ca, multi1_6);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_6);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_6);
    ca = _mm_mullo_epi32(ca, multi2_6);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_6);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_7 = {-9187342138168279040, -9187339930555121151};
const static __m128i shufkey2_7 = {-9187341034361716480, -9187202487306681598};
const static __m128i multi1_7 = {4294967424, 17179869186};
const static __m128i multi2_7 = {8589934593, 34359738372};
const static __m128i mask_7 = {545460846719, 545460846719};

static void simdhunpack7(const uint8_t *in, uint32_t *out) {
  enum { b = 7 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 7 };
  enum { shift2 = 4 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_7);
    ca = _mm_mullo_epi32(ca, multi1_7);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_7);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_7);
    ca = _mm_mullo_epi32(ca, multi2_7);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_7);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_8 = {-9187202495896584192, -9187202487306649598};
const static __m128i shufkey2_8 = {-9187202495896584192, -9187202487306649598};

static void simdhunpack8(const uint8_t *in, uint32_t *out) {
  enum { b = 8 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_8);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_8);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_9 = {-9187341034361716480, -9187338826748525822};
const static __m128i shufkey2_9 = {-9187341034361716480, -9187338826748525822};
const static __m128i multi1_9 = {17179869192, 4294967298};
const static __m128i multi2_9 = {17179869192, 4294967298};
const static __m128i mask_9 = {2194728288767, 2194728288767};

static void simdhunpack9(const uint8_t *in, uint32_t *out) {
  enum { b = 9 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 3 };
  enum { shift2 = 7 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_9);
    ca = _mm_mullo_epi32(ca, multi1_9);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_9);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_9);
    ca = _mm_mullo_epi32(ca, multi2_9);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_9);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_10 = {-9187341034361716480, -9187338826748525822};
const static __m128i shufkey2_10 = {-9187341034361716480, -9187338826748525822};
const static __m128i multi1_10 = {68719476800, 4294967300};
const static __m128i multi2_10 = {68719476800, 4294967300};
const static __m128i mask_10 = {4393751544831, 4393751544831};

static void simdhunpack10(const uint8_t *in, uint32_t *out) {
  enum { b = 10 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_10);
    ca = _mm_mullo_epi32(ca, multi1_10);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_10);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_10);
    ca = _mm_mullo_epi32(ca, multi2_10);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_10);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_11 = {-9187341034361716480, -9187337722950057214};
const static __m128i shufkey2_11 = {-9222525406450548480, -9187337722941930493};
const static __m128i multi1_11 = {34359738432, 137438953473};
const static __m128i multi2_11 = {4294967304, 17179869216};
const static __m128i mask_11 = {8791798056959, 8791798056959};

static void simdhunpack11(const uint8_t *in, uint32_t *out) {
  enum { b = 11 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 7 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_11);
    ca = _mm_mullo_epi32(ca, multi1_11);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_11);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_11);
    ca = _mm_mullo_epi32(ca, multi2_11);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_11);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_12 = {-9187341034361716480, -9187337722941930493};
const static __m128i shufkey2_12 = {-9187341034361716480, -9187337722941930493};
const static __m128i multi1_12 = {4294967312, 4294967312};
const static __m128i multi2_12 = {4294967312, 4294967312};
const static __m128i mask_12 = {17587891081215, 17587891081215};

static void simdhunpack12(const uint8_t *in, uint32_t *out) {
  enum { b = 12 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 4 };
  enum { shift2 = 4 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_12);
    ca = _mm_mullo_epi32(ca, multi1_12);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_12);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_12);
    ca = _mm_mullo_epi32(ca, multi2_12);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_12);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_13 = {-9222525406450548480, -9221677670100630525};
const static __m128i shufkey2_13 = {-9187339930563378944, -9187336619143396349};
const static __m128i multi1_13 = {17179869312, 4294967328};
const static __m128i multi2_13 = {137438953476, 34359738369};
const static __m128i mask_13 = {35180077129727, 35180077129727};

static void simdhunpack13(const uint8_t *in, uint32_t *out) {
  enum { b = 13 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 7 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_13);
    ca = _mm_mullo_epi32(ca, multi1_13);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_13);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_13);
    ca = _mm_mullo_epi32(ca, multi2_13);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_13);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_14 = {-9222525406450548480, -9187336619143396349};
const static __m128i shufkey2_14 = {-9222525406450548480, -9187336619143396349};
const static __m128i multi1_14 = {4294967360, 68719476740};
const static __m128i multi2_14 = {4294967360, 68719476740};
const static __m128i mask_14 = {70364449226751, 70364449226751};

static void simdhunpack14(const uint8_t *in, uint32_t *out) {
  enum { b = 14 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_14);
    ca = _mm_mullo_epi32(ca, multi1_14);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_14);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_14);
    ca = _mm_mullo_epi32(ca, multi2_14);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_14);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_15 = {-9222525406450548480, -9221395091325385725};
const static __m128i shufkey2_15 = {-9222242827675500288, -9187335515336735484};
const static __m128i multi1_15 = {4294967424, 17179869186};
const static __m128i multi2_15 = {8589934593, 34359738372};
const static __m128i mask_15 = {140733193420799, 140733193420799};

static void simdhunpack15(const uint8_t *in, uint32_t *out) {
  enum { b = 15 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 7 };
  enum { shift2 = 4 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_15);
    ca = _mm_mullo_epi32(ca, multi1_15);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_15);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_15);
    ca = _mm_mullo_epi32(ca, multi2_15);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_15);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_16 = {-9187339930555121408, -9187335515328740092};
const static __m128i shufkey2_16 = {-9187339930555121408, -9187335515328740092};

static void simdhunpack16(const uint8_t *in, uint32_t *out) {
  enum { b = 16 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_16);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_16);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_17 = {-9222242827675500288, -9221112512542014204};
const static __m128i shufkey2_17 = {-9222242827675500288, -9221112512542014204};
const static __m128i multi1_17 = {17179869192, 4294967298};
const static __m128i multi2_17 = {17179869192, 4294967298};
const static __m128i mask_17 = {562945658585087, 562945658585087};

static void simdhunpack17(const uint8_t *in, uint32_t *out) {
  enum { b = 17 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 3 };
  enum { shift2 = 7 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_17);
    ca = _mm_mullo_epi32(ca, multi1_17);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_17);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_17);
    ca = _mm_mullo_epi32(ca, multi2_17);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_17);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_18 = {-9222242827675500288, -9221112512542014204};
const static __m128i shufkey2_18 = {-9222242827675500288, -9221112512542014204};
const static __m128i multi1_18 = {68719476800, 4294967300};
const static __m128i multi2_18 = {68719476800, 4294967300};
const static __m128i mask_18 = {1125895612137471, 1125895612137471};

static void simdhunpack18(const uint8_t *in, uint32_t *out) {
  enum { b = 18 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_18);
    ca = _mm_mullo_epi32(ca, multi1_18);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_18);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_18);
    ca = _mm_mullo_epi32(ca, multi2_18);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_18);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_19 = {-9222242827675500288, -9220829935788751612};
const static __m128i shufkey2_19 = {361417179368915200, -9220829933758642683};
const static __m128i multi1_19 = {34359738432, 137438953473};
const static __m128i multi2_19 = {4294967304, 17179869216};
const static __m128i mask_19 = {2251795519242239, 2251795519242239};

static void simdhunpack19(const uint8_t *in, uint32_t *out) {
  enum { b = 19 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 7 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_19);
    ca = _mm_mullo_epi32(ca, multi1_19);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_19);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_19);
    ca = _mm_mullo_epi32(ca, multi2_19);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_19);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_20 = {-9222242827675500288, -9220829933758642683};
const static __m128i shufkey2_20 = {-9222242827675500288, -9220829933758642683};
const static __m128i multi1_20 = {4294967312, 4294967312};
const static __m128i multi2_20 = {4294967312, 4294967312};
const static __m128i mask_20 = {4503595333451775, 4503595333451775};

static void simdhunpack20(const uint8_t *in, uint32_t *out) {
  enum { b = 20 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 4 };
  enum { shift2 = 4 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_20);
    ca = _mm_mullo_epi32(ca, multi1_20);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_20);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_20);
    ca = _mm_mullo_epi32(ca, multi2_20);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_20);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_21 = {361417179368915200, 723118043475412485};
const static __m128i shufkey2_21 = {-9221960250989346560, -9220547356988602875};
const static __m128i multi1_21 = {17179869312, 4294967328};
const static __m128i multi2_21 = {137438953476, 34359738369};
const static __m128i mask_21 = {9007194961870847, 9007194961870847};

static void simdhunpack21(const uint8_t *in, uint32_t *out) {
  enum { b = 21 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 7 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_21);
    ca = _mm_mullo_epi32(ca, multi1_21);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_21);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_21);
    ca = _mm_mullo_epi32(ca, multi2_21);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_21);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_22 = {361417179368915200, -9220547356988602875};
const static __m128i shufkey2_22 = {361417179368915200, -9220547356988602875};
const static __m128i multi1_22 = {4294967360, 68719476740};
const static __m128i multi2_22 = {4294967360, 68719476740};
const static __m128i mask_22 = {18014394218708991, 18014394218708991};

static void simdhunpack22(const uint8_t *in, uint32_t *out) {
  enum { b = 22 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_22);
    ca = _mm_mullo_epi32(ca, multi1_22);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_22);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_22);
    ca = _mm_mullo_epi32(ca, multi2_22);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_22);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_23 = {361417179368915200, 795458214283380229};
const static __m128i shufkey2_23 = {433757350092996864, -9220264778188454138};
const static __m128i multi1_23 = {4294967424, 17179869186};
const static __m128i multi2_23 = {8589934593, 34359738372};
const static __m128i mask_23 = {36028792732385279, 36028792732385279};

static void simdhunpack23(const uint8_t *in, uint32_t *out) {
  enum { b = 23 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 7 };
  enum { shift2 = 4 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_23);
    ca = _mm_mullo_epi32(ca, multi1_23);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_23);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_23);
    ca = _mm_mullo_epi32(ca, multi2_23);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_23);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_24 = {-9221960248892194560, -9220264776191965434};
const static __m128i shufkey2_24 = {-9221960248892194560, -9220264776191965434};

static void simdhunpack24(const uint8_t *in, uint32_t *out) {
  enum { b = 24 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_24);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_24);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_25 = {433757350092996864, 867798387121456902};
const static __m128i shufkey2_25 = {433757350092996864, 867798387121456902};
const static __m128i multi1_25 = {17179869192, 4294967298};
const static __m128i multi2_25 = {17179869192, 4294967298};
const static __m128i mask_25 = {144115183814443007, 144115183814443007};

static void simdhunpack25(const uint8_t *in, uint32_t *out) {
  enum { b = 25 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 3 };
  enum { shift2 = 7 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_25);
    ca = _mm_mullo_epi32(ca, multi1_25);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_25);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_25);
    ca = _mm_mullo_epi32(ca, multi2_25);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_25);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_26 = {433757350092996864, 867798387121456902};
const static __m128i shufkey2_26 = {433757350092996864, 867798387121456902};
const static __m128i multi1_26 = {68719476800, 4294967300};
const static __m128i multi2_26 = {68719476800, 4294967300};
const static __m128i mask_26 = {288230371923853311, 288230371923853311};

static void simdhunpack26(const uint8_t *in, uint32_t *out) {
  enum { b = 26 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 6 };
  enum { shift2 = 6 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_26);
    ca = _mm_mullo_epi32(ca, multi1_26);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_26);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_26);
    ca = _mm_mullo_epi32(ca, multi2_26);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_26);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_27 = {433757350092996864, 940138559942690566};
const static __m128i shufkey2_27 = {506097522914230528, 940138559959533575};
const static __m128i multi1_27 = {4294967304, 17179869192};
const static __m128i multi2_27 = {137438953474, 4294967304};
const static __m128i mask_27 = {576460748142673919, 576460748142673919};

static void simdhunpack27(const uint8_t *in, uint32_t *out) {
  enum { b = 27 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 3 };
  enum { shift2 = 5 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_27);
    ca = _mm_blend_epi16(ca, _mm_srli_epi64(ca, 6), 48);
    ca = _mm_mullo_epi32(ca, multi1_27);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_27);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_27);
    ca = _mm_blend_epi16(ca, _mm_slli_epi64(ca, 1), 12);
    ca = _mm_mullo_epi32(ca, multi2_27);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_27);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_28 = {433757350092996864, 940138559959533575};
const static __m128i shufkey2_28 = {433757350092996864, 940138559959533575};
const static __m128i multi1_28 = {4294967312, 4294967312};
const static __m128i multi2_28 = {4294967312, 4294967312};
const static __m128i mask_28 = {1152921500580315135, 1152921500580315135};

static void simdhunpack28(const uint8_t *in, uint32_t *out) {
  enum { b = 28 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 4 };
  enum { shift2 = 4 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_28);
    ca = _mm_mullo_epi32(ca, multi1_28);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_28);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_28);
    ca = _mm_mullo_epi32(ca, multi2_28);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_28);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey2_29 = {506097522914230528, 1012478732780767239};
const static __m128i multi1_29 = {17179869188, 4294967297};
const static __m128i multi2_29 = {17179869192, 4294967298};
const static __m128i mask_29 = {2305843005455597567, 2305843005455597567};

static void simdhunpack29(const uint8_t *in, uint32_t *out) {
  enum { b = 29 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 2 };
  enum { shift2 = 3 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i tmp =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    __m128i ca = _mm_blend_epi16(tmp, _mm_slli_epi64(tmp, 3), 236);
    const static __m128i x2 =
        _mm_set_epi8(14, 13, 12, 11, 10, 9, 8, 7, 7, 6, 5, 4, 3, 2, 1, 0);
    ca = _mm_shuffle_epi8(ca, x2);
    const static __m128i x1 =
        _mm_set_epi8(14, 13, 12, 11, 10, 9, 8, 7, 7, 6, 5, 4, 3, 2, 1, 0);
    ca = _mm_blend_epi16(ca, _mm_shuffle_epi8(tmp, x1), 51);
    ca = _mm_mullo_epi32(ca, multi1_29);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_29);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_29);
    ca = _mm_blend_epi16(ca, _mm_srli_epi64(ca, 4), 51);
    ca = _mm_mullo_epi32(ca, multi2_29);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_29);
    _mm_store_si128(pCurr++, ca);
  }
}
const static __m128i shufkey1_30 = {433757350092996864, 1012478732780767239};
const static __m128i shufkey2_30 = {433757350092996864, 1012478732780767239};
const static __m128i multi1_30 = {4294967300, 4294967300};
const static __m128i multi2_30 = {4294967300, 4294967300};
const static __m128i mask_30 = {4611686015206162431, 4611686015206162431};

static void simdhunpack30(const uint8_t *in, uint32_t *out) {
  enum { b = 30 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  enum { shift1 = 2 };
  enum { shift2 = 2 };
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i tmp =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    __m128i ca = _mm_shuffle_epi8(tmp, shufkey1_30);
    ca = _mm_blend_epi16(ca, _mm_srli_epi64(ca, 4), 60);
    ca = _mm_blend_epi16(ca, _mm_slli_epi64(tmp, 4), 12);
    ca = _mm_mullo_epi32(ca, multi1_30);
    ca = _mm_srli_epi32(ca, shift1);
    ca = _mm_and_si128(ca, mask_30);
    _mm_store_si128(pCurr++, ca);
    tmp =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(tmp, shufkey2_30);
    ca = _mm_blend_epi16(ca, _mm_srli_epi64(ca, 4), 60);
    ca = _mm_blend_epi16(ca, _mm_slli_epi64(tmp, 4), 12);
    ca = _mm_mullo_epi32(ca, multi2_30);
    ca = _mm_srli_epi32(ca, shift2);
    ca = _mm_and_si128(ca, mask_30);
    _mm_store_si128(pCurr++, ca);
  }
}

static void simdhunpack31(const uint8_t *in, uint32_t *out) {
  enum { b = 31 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  const static __m128i mask = _mm_set1_epi32((1U << 31) - 1);
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i tmp =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    __m128i ca = _mm_blend_epi16(tmp, _mm_slli_epi64(tmp, 1), 12);
    ca = _mm_blend_epi16(ca, _mm_srli_epi64(_mm_slli_si128(tmp, 1), 6), 48);
    ca = _mm_blend_epi16(ca, _mm_slli_epi64(tmp, 3), 192);
    ca = _mm_and_si128(ca, mask);
    _mm_store_si128(pCurr++, ca);
    tmp =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_blend_epi16(_mm_srli_epi64(tmp, 4),
                         _mm_slli_epi64(_mm_srli_si128(tmp, 1), 5), 12);
    ca = _mm_blend_epi16(ca, _mm_srli_epi64(tmp, 2), 48);
    ca = _mm_blend_epi16(ca, _mm_srli_epi64(tmp, 1), 192);
    ca = _mm_and_si128(ca, mask);
    _mm_store_si128(pCurr++, ca);
  }
}

const static __m128i shufkey1_32 = {506097522914230528, 1084818905618843912};
const static __m128i shufkey2_32 = {506097522914230528, 1084818905618843912};

static void simdhunpack32(const uint8_t *in, uint32_t *out) {
  enum { b = 32 };
  __m128i *pCurr = reinterpret_cast<__m128i *>(out);
  for (uint32_t j = 0; j < 16; ++j) {
    __m128i ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j));
    ca = _mm_shuffle_epi8(ca, shufkey1_32);
    _mm_store_si128(pCurr++, ca);
    ca = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + b * j + b / 2));
    ca = _mm_shuffle_epi8(ca, shufkey2_32);
    _mm_store_si128(pCurr++, ca);
  }
}

void simdhunpack(const uint8_t *in, uint32_t *out, uint32_t bit) {
  switch (bit) {
  case 0:
    SIMD_nullunpacker32(in, out);
    return;

  case 1:
    simdhunpack1(in, out);
    return;

  case 2:
    simdhunpack2(in, out);
    return;

  case 3:
    simdhunpack3(in, out);
    return;

  case 4:
    simdhunpack4(in, out);
    return;

  case 5:
    simdhunpack5(in, out);
    return;

  case 6:
    simdhunpack6(in, out);
    return;

  case 7:
    simdhunpack7(in, out);
    return;

  case 8:
    simdhunpack8(in, out);
    return;

  case 9:
    simdhunpack9(in, out);
    return;

  case 10:
    simdhunpack10(in, out);
    return;

  case 11:
    simdhunpack11(in, out);
    return;

  case 12:
    simdhunpack12(in, out);
    return;

  case 13:
    simdhunpack13(in, out);
    return;

  case 14:
    simdhunpack14(in, out);
    return;

  case 15:
    simdhunpack15(in, out);
    return;

  case 16:
    simdhunpack16(in, out);
    return;

  case 17:
    simdhunpack17(in, out);
    return;

  case 18:
    simdhunpack18(in, out);
    return;

  case 19:
    simdhunpack19(in, out);
    return;

  case 20:
    simdhunpack20(in, out);
    return;

  case 21:
    simdhunpack21(in, out);
    return;

  case 22:
    simdhunpack22(in, out);
    return;

  case 23:
    simdhunpack23(in, out);
    return;

  case 24:
    simdhunpack24(in, out);
    return;

  case 25:
    simdhunpack25(in, out);
    return;

  case 26:
    simdhunpack26(in, out);
    return;

  case 27:
    simdhunpack27(in, out);
    return;

  case 28:
    simdhunpack28(in, out);
    return;

  case 29:
    simdhunpack29(in, out);
    return;

  case 30:
    simdhunpack30(in, out);
    return;

  case 31:
    simdhunpack31(in, out);
    return;

  case 32:
    simdhunpack32(in, out);
    return;

  default:
    break;
  }
  throw logic_error("number of bits is unsupported");
}

} // namespace FastPFor
