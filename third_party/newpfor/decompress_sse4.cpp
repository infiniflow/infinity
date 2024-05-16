/*
 * Copyright 2014-present Alibaba Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "decompress_sse4.h"

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/avx512.h>
#define __SSE__
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "unpack.h"

namespace indexlib {

const __m128i SSE4_zero_m128i __attribute__((unused)) = _mm_set_epi32(0x0, 0x0, 0x0, 0x0);
const __m128i SSE4_set_m128i __attribute__((unused)) = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

// c0
void decompress_sse4_c0(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 1, dest += 32) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        dest[3] = 0;
        dest[4] = 0;
        dest[5] = 0;
        dest[6] = 0;
        dest[7] = 0;
        dest[8] = 0;
        dest[9] = 0;
        dest[10] = 0;
        dest[11] = 0;
        dest[12] = 0;
        dest[13] = 0;
        dest[14] = 0;
        dest[15] = 0;
        dest[16] = 0;
        dest[17] = 0;
        dest[18] = 0;
        dest[19] = 0;
        dest[20] = 0;
        dest[21] = 0;
        dest[22] = 0;
        dest[23] = 0;
        dest[24] = 0;
        dest[25] = 0;
        dest[26] = 0;
        dest[27] = 0;
        dest[28] = 0;
        dest[29] = 0;
        dest[30] = 0;
        dest[31] = 0;
    }
    if (n & 0x1F) {
        unaligned_unpack_0(dest, encode, (n & 0x1F));
    }
}

// c1
const __m128i SSE4_c1_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01);

const __m128i SSE4_c1_mul_msk1_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

const __m128i SSE4_c1_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x10);

const __m128i SSE4_c1_mul_msk2_m128i = _mm_set_epi32(0x01, 0x02, 0x4, 0x08);

template <int segment>
inline void decompress_sse4_c1_m2(__m128i c1_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c1_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment, 0xFF, 0xFF, 0xFF, segment, 0xFF, 0xFF,
                                                  0xFF, segment, 0xFF, 0xFF, 0xFF, segment);

    __m128i c1_shfl_rslt_m128i = _mm_shuffle_epi8(c1_load_vec_m128i, SSE4_c1_shfl_msk_m128i);
    __m128i c1_and_rslt_m128i = _mm_and_si128(c1_shfl_rslt_m128i, SSE4_c1_and_msk1_m128i);
    __m128i c1_mul_rslt_m128i = _mm_mullo_epi32(c1_and_rslt_m128i, SSE4_c1_mul_msk1_m128i);
    __m128i c1_rslt_m128i = _mm_srli_epi32(c1_mul_rslt_m128i, 3);
    _mm_storeu_si128((__m128i*)presult, c1_rslt_m128i);

    presult = presult + 4;

    SSE4_c1_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment, 0xFF, 0xFF, 0xFF, segment, 0xFF, 0xFF, 0xFF,
                                          segment, 0xFF, 0xFF, 0xFF, segment);

    c1_shfl_rslt_m128i = _mm_shuffle_epi8(c1_load_vec_m128i, SSE4_c1_shfl_msk_m128i);
    c1_and_rslt_m128i = _mm_and_si128(c1_shfl_rslt_m128i, SSE4_c1_and_msk2_m128i);
    c1_mul_rslt_m128i = _mm_mullo_epi32(c1_and_rslt_m128i, SSE4_c1_mul_msk2_m128i);
    c1_rslt_m128i = _mm_srli_epi32(c1_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c1_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c1_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c1_load_rslt_m128i = _mm_loadu_si128(its_vector_m128i);

    decompress_sse4_c1_m2<0>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<1>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<2>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<3>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<4>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<5>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<6>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<7>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<8>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<9>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<10>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<11>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<12>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<13>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<14>(c1_load_rslt_m128i, presult);

    decompress_sse4_c1_m2<15>(c1_load_rslt_m128i, presult);
}

void decompress_sse4_c1(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c1_m1(vector_m128i, presult + index, offset);
        offset += 1;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_1(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c2
const __m128i SSE4_c2_and_msk_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x03);

const __m128i SSE4_c2_mul_msk_m128i = _mm_set_epi32(0x01, 0x04, 0x10, 0x40);

template <int segment>
inline void decompress_sse4_c2_m2(__m128i c2_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c2_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment, 0xFF, 0xFF, 0xFF, segment, 0xFF, 0xFF,
                                                  0xFF, segment, 0xFF, 0xFF, 0xFF, segment);

    __m128i c2_shfl_rslt_m128i = _mm_shuffle_epi8(c2_load_vec_m128i, SSE4_c2_shfl_msk_m128i);
    __m128i c2_and_rslt_m128i = _mm_and_si128(c2_shfl_rslt_m128i, SSE4_c2_and_msk_m128i);
    __m128i c2_mul_rslt_m128i = _mm_mullo_epi32(c2_and_rslt_m128i, SSE4_c2_mul_msk_m128i);
    __m128i c2_rslt_m128i = _mm_srli_epi32(c2_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c2_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c2_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c2_load_rslt_m128i = _mm_loadu_si128(its_vector_m128i);

    decompress_sse4_c2_m2<0>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<1>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<2>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<3>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<4>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<5>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<6>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<7>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<8>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<9>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<10>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<11>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<12>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<13>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<14>(c2_load_rslt_m128i, presult);

    decompress_sse4_c2_m2<15>(c2_load_rslt_m128i, presult);
}

void decompress_sse4_c2(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c2_m1(vector_m128i, presult + index, offset);
        offset += 1;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_2(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c3
const __m128i SSE4_c3_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x07);

const __m128i SSE4_c3_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x70);

const __m128i SSE4_c3_mul_msk1_m128i = _mm_set_epi32(0x20, 0x01, 0x08, 0x40);

const __m128i SSE4_c3_mul_msk2_m128i = _mm_set_epi32(0x04, 0x20, 0x01, 0x08);

template <int segment>
inline void decompress_sse4_c3_m2(__m128i c3_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c3_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment + 1, 0xFF, 0xFF, segment + 1, segment, 0xFF,
                                                  0xFF, 0xFF, segment, 0xFF, 0xFF, 0xFF, segment);

    __m128i c3_shfl_rslt_m128i = _mm_shuffle_epi8(c3_load_vec_m128i, SSE4_c3_shfl_msk_m128i);
    __m128i c3_and_rslt_m128i = _mm_and_si128(c3_shfl_rslt_m128i, SSE4_c3_and_msk1_m128i);
    __m128i c3_mul_rslt_m128i = _mm_mullo_epi32(c3_and_rslt_m128i, SSE4_c3_mul_msk1_m128i);
    __m128i c3_rslt_m128i = _mm_srli_epi32(c3_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c3_rslt_m128i);

    presult = presult + 4;

    SSE4_c3_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment + 2, 0xFF, 0xFF, 0xFF, segment + 2, 0xFF, 0xFF,
                                          segment + 2, segment + 1, 0xFF, 0xFF, 0xFF, segment + 1);

    c3_shfl_rslt_m128i = _mm_shuffle_epi8(c3_load_vec_m128i, SSE4_c3_shfl_msk_m128i);
    c3_and_rslt_m128i = _mm_and_si128(c3_shfl_rslt_m128i, SSE4_c3_and_msk2_m128i);
    c3_mul_rslt_m128i = _mm_mullo_epi32(c3_and_rslt_m128i, SSE4_c3_mul_msk2_m128i);
    c3_rslt_m128i = _mm_srli_epi32(c3_mul_rslt_m128i, 7);

    _mm_storeu_si128((__m128i*)presult, c3_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c3_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c3_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i); // contains more than 40 values

    decompress_sse4_c3_m2<0>(c3_load_rslt1_m128i, presult); // gets 1st 8 values

    decompress_sse4_c3_m2<3>(c3_load_rslt1_m128i, presult); // 2nd 8 values

    decompress_sse4_c3_m2<6>(c3_load_rslt1_m128i, presult); // 3th 8 values

    decompress_sse4_c3_m2<9>(c3_load_rslt1_m128i, presult); // 4th 8 values

    decompress_sse4_c3_m2<12>(c3_load_rslt1_m128i, presult); // 5th 8 values

    __m128i c3_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c3_align_rslt1_m128i = _mm_alignr_epi8(c3_load_rslt2_m128i, c3_load_rslt1_m128i, 15);
    decompress_sse4_c3_m2<0>(c3_align_rslt1_m128i, presult); // gets 1st 8 values

    decompress_sse4_c3_m2<3>(c3_align_rslt1_m128i, presult); // 2nd 8 values

    decompress_sse4_c3_m2<6>(c3_align_rslt1_m128i, presult); // 3th 8 values

    decompress_sse4_c3_m2<9>(c3_align_rslt1_m128i, presult); // 4th 8 values

    decompress_sse4_c3_m2<12>(c3_align_rslt1_m128i, presult); // 5th 8 values

    __m128i c3_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c3_align_rslt2_m128i = _mm_alignr_epi8(c3_load_rslt3_m128i, c3_load_rslt2_m128i, 14);
    decompress_sse4_c3_m2<0>(c3_align_rslt2_m128i, presult); // gets 1st 8 values

    decompress_sse4_c3_m2<3>(c3_align_rslt2_m128i, presult); // 2nd 8 values

    decompress_sse4_c3_m2<6>(c3_align_rslt2_m128i, presult); // 3th 8 values

    decompress_sse4_c3_m2<9>(c3_align_rslt2_m128i, presult); // 4th 8 values

    decompress_sse4_c3_m2<12>(c3_align_rslt2_m128i, presult); // 5th 8 values

    decompress_sse4_c3_m2<13>(c3_load_rslt3_m128i, presult);
}

void decompress_sse4_c3(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c3_m1(vector_m128i, presult + index, offset);
        offset += 3;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_3(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c4
const __m128i SSE4_c4_and_msk_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F);

const __m128i SSE4_c4_mul_msk_m128i = _mm_set_epi32(0x01, 0x10, 0x01, 0x10);

template <int segment>
inline void decompress_sse4_c4_m2(__m128i c4_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c4_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment + 1, 0xFF, 0xFF, 0xFF, segment + 1, 0xFF,
                                                  0xFF, 0xFF, segment, 0xFF, 0xFF, 0xFF, segment);

    __m128i c4_shfl_rslt_m128i = _mm_shuffle_epi8(c4_load_vec_m128i, SSE4_c4_shfl_msk_m128i);
    __m128i c4_and_rslt_m128i = _mm_and_si128(c4_shfl_rslt_m128i, SSE4_c4_and_msk_m128i);
    __m128i c4_mul_rslt_m128i = _mm_mullo_epi32(c4_and_rslt_m128i, SSE4_c4_mul_msk_m128i);
    __m128i c4_rslt_m128i = _mm_srli_epi32(c4_mul_rslt_m128i, 4);
    _mm_storeu_si128((__m128i*)presult, c4_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c4_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c4_load_rslt_m128i = _mm_loadu_si128(its_vector_m128i);

    decompress_sse4_c4_m2<0>(c4_load_rslt_m128i, presult);

    decompress_sse4_c4_m2<2>(c4_load_rslt_m128i, presult);

    decompress_sse4_c4_m2<4>(c4_load_rslt_m128i, presult);

    decompress_sse4_c4_m2<6>(c4_load_rslt_m128i, presult);

    decompress_sse4_c4_m2<8>(c4_load_rslt_m128i, presult);

    decompress_sse4_c4_m2<10>(c4_load_rslt_m128i, presult);

    decompress_sse4_c4_m2<12>(c4_load_rslt_m128i, presult);

    decompress_sse4_c4_m2<14>(c4_load_rslt_m128i, presult);
}

void decompress_sse4_c4(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 32 <= n) {
        decompress_sse4_c4_m1(vector_m128i, presult + index, offset);
        offset += 1;
        index += 32;
    }
    if (n & 0x1F) {
        unpack_4(dest + index, encode + offset * 4, (n & 0x1F));
    }
}

// c5
const __m128i SSE4_c5_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x1F);

const __m128i SSE4_c5_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x01, 0xF0);

const __m128i SSE4_c5_mul_msk1_m128i = _mm_set_epi32(0x01, 0x20, 0x04, 0x80);

const __m128i SSE4_c5_mul_msk2_m128i = _mm_set_epi32(0x08, 0x01, 0x20, 0x04);

template <int segment>
inline void decompress_sse4_c5_m2(__m128i c5_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c5_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, segment + 2, segment + 1, 0xFF, 0xFF, 0xFF, segment + 1,
                                                  0xFF, 0xFF, segment + 1, segment, 0xFF, 0xFF, 0xFF, segment);

    __m128i c5_shfl_rslt_m128i = _mm_shuffle_epi8(c5_load_vec_m128i, SSE4_c5_shfl_msk_m128i);
    __m128i c5_and_rslt_m128i = _mm_and_si128(c5_shfl_rslt_m128i, SSE4_c5_and_msk1_m128i);
    __m128i c5_mul_rslt_m128i = _mm_mullo_epi32(c5_and_rslt_m128i, SSE4_c5_mul_msk1_m128i);
    __m128i c5_rslt_m128i = _mm_srli_epi32(c5_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c5_rslt_m128i);

    presult = presult + 4;

    // decompress second 4 values
    SSE4_c5_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment + 4, 0xFF, 0xFF, segment + 4, segment + 3, 0xFF,
                                          0xFF, 0xFF, segment + 3, 0xFF, 0xFF, segment + 3, segment + 2);

    c5_shfl_rslt_m128i = _mm_shuffle_epi8(c5_load_vec_m128i, SSE4_c5_shfl_msk_m128i);
    c5_and_rslt_m128i = _mm_and_si128(c5_shfl_rslt_m128i, SSE4_c5_and_msk2_m128i);
    c5_mul_rslt_m128i = _mm_mullo_epi32(c5_and_rslt_m128i, SSE4_c5_mul_msk2_m128i);
    c5_rslt_m128i = _mm_srli_epi32(c5_mul_rslt_m128i, 6);

    _mm_storeu_si128((__m128i*)presult, c5_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c5_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;
    __m128i c5_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c5_m2<0>(c5_load_rslt1_m128i, presult);
    decompress_sse4_c5_m2<5>(c5_load_rslt1_m128i, presult);
    decompress_sse4_c5_m2<10>(c5_load_rslt1_m128i, presult);
    __m128i c5_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c5_align_rslt1_m128i = _mm_alignr_epi8(c5_load_rslt2_m128i, c5_load_rslt1_m128i, 15);
    decompress_sse4_c5_m2<0>(c5_align_rslt1_m128i, presult);
    decompress_sse4_c5_m2<5>(c5_align_rslt1_m128i, presult);
    decompress_sse4_c5_m2<10>(c5_align_rslt1_m128i, presult);
    __m128i c5_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c5_align_rslt2_m128i = _mm_alignr_epi8(c5_load_rslt3_m128i, c5_load_rslt2_m128i, 14);
    decompress_sse4_c5_m2<0>(c5_align_rslt2_m128i, presult);
    decompress_sse4_c5_m2<5>(c5_align_rslt2_m128i, presult);
    decompress_sse4_c5_m2<10>(c5_align_rslt2_m128i, presult);
    __m128i c5_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c5_align_rslt3_m128i = _mm_alignr_epi8(c5_load_rslt4_m128i, c5_load_rslt3_m128i, 13);
    decompress_sse4_c5_m2<0>(c5_align_rslt3_m128i, presult);
    decompress_sse4_c5_m2<5>(c5_align_rslt3_m128i, presult);
    decompress_sse4_c5_m2<10>(c5_align_rslt3_m128i, presult);
    __m128i c5_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c5_align_rslt4_m128i = _mm_alignr_epi8(c5_load_rslt5_m128i, c5_load_rslt4_m128i, 12);
    decompress_sse4_c5_m2<0>(c5_align_rslt4_m128i, presult);
    decompress_sse4_c5_m2<5>(c5_align_rslt4_m128i, presult);
    decompress_sse4_c5_m2<10>(c5_align_rslt4_m128i, presult);
    decompress_sse4_c5_m2<11>(c5_load_rslt5_m128i, presult);
}

void decompress_sse4_c5(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c5_m1(vector_m128i, presult + index, offset);
        offset += 5;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_5(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c6
const __m128i SSE4_c6_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x3F);

const __m128i SSE4_c6_mul_msk1_m128i = _mm_set_epi32(0x10, 0x04, 0x01, 0x40);

template <int segment>
inline void decompress_sse4_c6_m2(__m128i c6_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c6_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment + 2, 0xFF, 0xFF, segment + 2, segment + 1,
                                                  0xFF, 0xFF, segment + 1, segment, 0xFF, 0xFF, 0xFF, segment);

    __m128i c6_shfl_rslt_m128i = _mm_shuffle_epi8(c6_load_vec_m128i, SSE4_c6_shfl_msk_m128i);
    __m128i c6_and_rslt_m128i = _mm_and_si128(c6_shfl_rslt_m128i, SSE4_c6_and_msk1_m128i);
    __m128i c6_mul_rslt_m128i = _mm_mullo_epi32(c6_and_rslt_m128i, SSE4_c6_mul_msk1_m128i);
    __m128i c6_rslt_m128i = _mm_srli_epi32(c6_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c6_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c6_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c6_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c6_m2<0>(c6_load_rslt1_m128i, presult);

    decompress_sse4_c6_m2<3>(c6_load_rslt1_m128i, presult);

    decompress_sse4_c6_m2<6>(c6_load_rslt1_m128i, presult);

    decompress_sse4_c6_m2<9>(c6_load_rslt1_m128i, presult);

    decompress_sse4_c6_m2<12>(c6_load_rslt1_m128i, presult);

    __m128i c6_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c6_align_rslt1_m128i = _mm_alignr_epi8(c6_load_rslt2_m128i, c6_load_rslt1_m128i, 15);
    decompress_sse4_c6_m2<0>(c6_align_rslt1_m128i, presult);

    decompress_sse4_c6_m2<3>(c6_align_rslt1_m128i, presult);

    decompress_sse4_c6_m2<6>(c6_align_rslt1_m128i, presult);

    decompress_sse4_c6_m2<9>(c6_align_rslt1_m128i, presult);

    decompress_sse4_c6_m2<12>(c6_align_rslt1_m128i, presult);

    __m128i c6_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c6_align_rslt2_m128i = _mm_alignr_epi8(c6_load_rslt3_m128i, c6_load_rslt2_m128i, 14);
    decompress_sse4_c6_m2<0>(c6_align_rslt2_m128i, presult);

    decompress_sse4_c6_m2<3>(c6_align_rslt2_m128i, presult);

    decompress_sse4_c6_m2<6>(c6_align_rslt2_m128i, presult);

    decompress_sse4_c6_m2<9>(c6_align_rslt2_m128i, presult);

    decompress_sse4_c6_m2<12>(c6_align_rslt2_m128i, presult);

    decompress_sse4_c6_m2<13>(c6_load_rslt3_m128i, presult);
}

void decompress_sse4_c6(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c6_m1(vector_m128i, presult + index, offset);
        offset += 3;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_6(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c7
const __m128i SSE4_c7_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x7F);

const __m128i SSE4_c7_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x01, 0xFC, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x07, 0xF0);

const __m128i SSE4_c7_mul_msk1_m128i = _mm_set_epi32(0x04, 0x02, 0x01, 0x80);

const __m128i SSE4_c7_mul_msk2_m128i = _mm_set_epi32(0x08, 0x04, 0x02, 0x01);

template <int segment>
inline void decompress_sse4_c7_m2(__m128i c7_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c7_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 3, segment + 2, 0xFF, 0xFF, segment + 2, segment + 1, 0xFF, 0xFF,
                     segment + 1, segment, 0xFF, 0xFF, 0xFF, segment);

    __m128i c7_shfl_rslt_m128i = _mm_shuffle_epi8(c7_load_vec_m128i, SSE4_c7_shfl_msk_m128i);
    __m128i c7_and_rslt_m128i = _mm_and_si128(c7_shfl_rslt_m128i, SSE4_c7_and_msk1_m128i);
    __m128i c7_mul_rslt_m128i = _mm_mullo_epi32(c7_and_rslt_m128i, SSE4_c7_mul_msk1_m128i);
    __m128i c7_rslt_m128i = _mm_srli_epi32(c7_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c7_rslt_m128i);

    presult = presult + 4;

    // decompress second 4 values
    SSE4_c7_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment + 6, 0xFF, 0xFF, segment + 6, segment + 5, 0xFF,
                                          0xFF, segment + 5, segment + 4, 0xFF, 0xFF, segment + 4, segment + 3);

    c7_shfl_rslt_m128i = _mm_shuffle_epi8(c7_load_vec_m128i, SSE4_c7_shfl_msk_m128i);
    c7_and_rslt_m128i = _mm_and_si128(c7_shfl_rslt_m128i, SSE4_c7_and_msk2_m128i);
    c7_mul_rslt_m128i = _mm_mullo_epi32(c7_and_rslt_m128i, SSE4_c7_mul_msk2_m128i);
    c7_rslt_m128i = _mm_srli_epi32(c7_mul_rslt_m128i, 4);

    _mm_storeu_si128((__m128i*)presult, c7_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c7_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c7_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c7_m2<0>(c7_load_rslt1_m128i, presult);
    decompress_sse4_c7_m2<7>(c7_load_rslt1_m128i, presult);

    __m128i c7_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c7_align_rslt1_m128i = _mm_alignr_epi8(c7_load_rslt2_m128i, c7_load_rslt1_m128i, 14);
    decompress_sse4_c7_m2<0>(c7_align_rslt1_m128i, presult);
    decompress_sse4_c7_m2<7>(c7_align_rslt1_m128i, presult);

    __m128i c7_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c7_align_rslt2_m128i = _mm_alignr_epi8(c7_load_rslt3_m128i, c7_load_rslt2_m128i, 12);
    decompress_sse4_c7_m2<0>(c7_align_rslt2_m128i, presult);
    decompress_sse4_c7_m2<7>(c7_align_rslt2_m128i, presult);

    __m128i c7_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c7_align_rslt3_m128i = _mm_alignr_epi8(c7_load_rslt4_m128i, c7_load_rslt3_m128i, 10);
    decompress_sse4_c7_m2<0>(c7_align_rslt3_m128i, presult);
    decompress_sse4_c7_m2<7>(c7_align_rslt3_m128i, presult);

    __m128i c7_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c7_align_rslt4_m128i = _mm_alignr_epi8(c7_load_rslt5_m128i, c7_load_rslt4_m128i, 8);
    decompress_sse4_c7_m2<0>(c7_align_rslt4_m128i, presult);
    decompress_sse4_c7_m2<7>(c7_align_rslt4_m128i, presult);

    __m128i c7_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c7_align_rslt5_m128i = _mm_alignr_epi8(c7_load_rslt6_m128i, c7_load_rslt5_m128i, 6);
    decompress_sse4_c7_m2<0>(c7_align_rslt5_m128i, presult);
    decompress_sse4_c7_m2<7>(c7_align_rslt5_m128i, presult);

    __m128i c7_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c7_align_rslt6_m128i = _mm_alignr_epi8(c7_load_rslt7_m128i, c7_load_rslt6_m128i, 4);
    decompress_sse4_c7_m2<0>(c7_align_rslt6_m128i, presult);
    decompress_sse4_c7_m2<7>(c7_align_rslt6_m128i, presult);

    decompress_sse4_c7_m2<2>(c7_load_rslt7_m128i, presult);
    decompress_sse4_c7_m2<9>(c7_load_rslt7_m128i, presult);
}

void decompress_sse4_c7(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c7_m1(vector_m128i, presult + index, offset);
        offset += 7;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_7(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c8
template <int segment>
inline void decompress_sse4_c8_m2(__m128i c8_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c8_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, 0xFF, segment + 3, 0xFF, 0xFF, 0xFF, segment + 2, 0xFF,
                                                  0xFF, 0xFF, segment + 1, 0xFF, 0xFF, 0xFF, segment);

    __m128i c8_rslt_m128i = _mm_shuffle_epi8(c8_load_vec_m128i, SSE4_c8_shfl_msk_m128i);
    _mm_storeu_si128((__m128i*)presult, c8_rslt_m128i);

    //__m128i c8_equal_rslt_m128i = _mm_cmpeq_epi32(c8_rslt_m128i, SSE4_zero_m128i);
    // int c8_tstz_rslt_int = _mm_testz_si128(c8_equal_rslt_m128i, SSE4_set_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c8_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c8_load_rslt_m128i = _mm_loadu_si128(its_vector_m128i);

    decompress_sse4_c8_m2<0>(c8_load_rslt_m128i, presult);

    decompress_sse4_c8_m2<4>(c8_load_rslt_m128i, presult);

    decompress_sse4_c8_m2<8>(c8_load_rslt_m128i, presult);

    decompress_sse4_c8_m2<12>(c8_load_rslt_m128i, presult);
}

void decompress_sse4_c8(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 16 <= n) {
        decompress_sse4_c8_m1(vector_m128i, presult + index, offset);
        offset += 1;
        index += 16;
    }
    if (n & 0xF) {
        unpack_8(dest + index, encode + offset * 4, (n & 0xF));
    }
}

// c9
const __m128i SSE4_c9_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x03, 0xFE, 0x00, 0x00, 0x01, 0xFF);

const __m128i SSE4_c9_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0xFF, 0x80, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x3F, 0xE0, 0x00, 0x00, 0x1F, 0xF0);

const __m128i SSE4_c9_mul_msk1_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

const __m128i SSE4_c9_mul_msk2_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

template <int segment>
inline void decompress_sse4_c9_m2(__m128i c9_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c9_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 4, segment + 3, 0xFF, 0xFF, segment + 3, segment + 2, 0xFF, 0xFF,
                     segment + 2, segment + 1, 0xFF, 0xFF, segment + 1, segment);

    __m128i c9_shfl_rslt_m128i = _mm_shuffle_epi8(c9_load_vec_m128i, SSE4_c9_shfl_msk_m128i);
    __m128i c9_and_rslt_m128i = _mm_and_si128(c9_shfl_rslt_m128i, SSE4_c9_and_msk1_m128i);
    __m128i c9_mul_rslt_m128i = _mm_mullo_epi32(c9_and_rslt_m128i, SSE4_c9_mul_msk1_m128i);
    __m128i c9_rslt_m128i = _mm_srli_epi32(c9_mul_rslt_m128i, 3);
    _mm_storeu_si128((__m128i*)presult, c9_rslt_m128i);

    presult = presult + 4;

    // decompress second 4 values
    SSE4_c9_shfl_msk_m128i = _mm_set_epi8(0xFF, 0xFF, segment + 8, segment + 7, 0xFF, 0xFF, segment + 7, segment + 6,
                                          0xFF, 0xFF, segment + 6, segment + 5, 0xFF, 0xFF, segment + 5, segment + 4);

    c9_shfl_rslt_m128i = _mm_shuffle_epi8(c9_load_vec_m128i, SSE4_c9_shfl_msk_m128i);
    c9_and_rslt_m128i = _mm_and_si128(c9_shfl_rslt_m128i, SSE4_c9_and_msk2_m128i);
    c9_mul_rslt_m128i = _mm_mullo_epi32(c9_and_rslt_m128i, SSE4_c9_mul_msk2_m128i);
    c9_rslt_m128i = _mm_srli_epi32(c9_mul_rslt_m128i, 7);

    _mm_storeu_si128((__m128i*)presult, c9_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c9_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c9_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c9_m2<0>(c9_load_rslt1_m128i, presult);

    __m128i c9_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c9_align_rslt1_m128i = _mm_alignr_epi8(c9_load_rslt2_m128i, c9_load_rslt1_m128i, 9);
    decompress_sse4_c9_m2<0>(c9_align_rslt1_m128i, presult);

    decompress_sse4_c9_m2<2>(c9_load_rslt2_m128i, presult);

    __m128i c9_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c9_align_rslt2_m128i = _mm_alignr_epi8(c9_load_rslt3_m128i, c9_load_rslt2_m128i, 11);
    decompress_sse4_c9_m2<0>(c9_align_rslt2_m128i, presult);

    decompress_sse4_c9_m2<4>(c9_load_rslt3_m128i, presult);

    __m128i c9_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c9_align_rslt3_m128i = _mm_alignr_epi8(c9_load_rslt4_m128i, c9_load_rslt3_m128i, 13);
    decompress_sse4_c9_m2<0>(c9_align_rslt3_m128i, presult);

    decompress_sse4_c9_m2<6>(c9_load_rslt4_m128i, presult);

    __m128i c9_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c9_align_rslt4_m128i = _mm_alignr_epi8(c9_load_rslt5_m128i, c9_load_rslt4_m128i, 15);
    decompress_sse4_c9_m2<0>(c9_align_rslt4_m128i, presult);

    __m128i c9_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c9_align_rslt5_m128i = _mm_alignr_epi8(c9_load_rslt6_m128i, c9_load_rslt5_m128i, 8);
    decompress_sse4_c9_m2<0>(c9_align_rslt5_m128i, presult);

    decompress_sse4_c9_m2<1>(c9_load_rslt6_m128i, presult);

    __m128i c9_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c9_align_rslt6_m128i = _mm_alignr_epi8(c9_load_rslt7_m128i, c9_load_rslt6_m128i, 10);
    decompress_sse4_c9_m2<0>(c9_align_rslt6_m128i, presult);

    decompress_sse4_c9_m2<3>(c9_load_rslt7_m128i, presult);

    __m128i c9_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c9_align_rslt7_m128i = _mm_alignr_epi8(c9_load_rslt8_m128i, c9_load_rslt7_m128i, 12);
    decompress_sse4_c9_m2<0>(c9_align_rslt7_m128i, presult);

    decompress_sse4_c9_m2<5>(c9_load_rslt8_m128i, presult);

    __m128i c9_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c9_align_rslt8_m128i = _mm_alignr_epi8(c9_load_rslt9_m128i, c9_load_rslt8_m128i, 14);
    decompress_sse4_c9_m2<0>(c9_align_rslt8_m128i, presult);

    decompress_sse4_c9_m2<7>(c9_load_rslt9_m128i, presult);
}

void decompress_sse4_c9(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c9_m1(vector_m128i, presult + index, offset);
        offset += 9;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_9(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c10
const __m128i SSE4_c10_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x0F, 0xFC, 0x00, 0x00, 0x03, 0xFF);

const __m128i SSE4_c10_mul_msk1_m128i = _mm_set_epi32(0x01, 0x04, 0x10, 0x40);

template <int segment>
inline void decompress_sse4_c10_m2(__m128i c10_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c10_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 4, segment + 3, 0xFF, 0xFF, segment + 3, segment + 2, 0xFF, 0xFF,
                     segment + 2, segment + 1, 0xFF, 0xFF, segment + 1, segment);

    __m128i c10_shfl_rslt_m128i = _mm_shuffle_epi8(c10_load_vec_m128i, SSE4_c10_shfl_msk_m128i);
    __m128i c10_and_rslt_m128i = _mm_and_si128(c10_shfl_rslt_m128i, SSE4_c10_and_msk1_m128i);
    __m128i c10_mul_rslt_m128i = _mm_mullo_epi32(c10_and_rslt_m128i, SSE4_c10_mul_msk1_m128i);
    __m128i c10_rslt_m128i = _mm_srli_epi32(c10_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c10_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c10_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c10_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);

    decompress_sse4_c10_m2<0>(c10_load_rslt1_m128i, presult);

    decompress_sse4_c10_m2<5>(c10_load_rslt1_m128i, presult);

    decompress_sse4_c10_m2<10>(c10_load_rslt1_m128i, presult);

    __m128i c10_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c10_align_rslt1_m128i = _mm_alignr_epi8(c10_load_rslt2_m128i, c10_load_rslt1_m128i, 15);
    decompress_sse4_c10_m2<0>(c10_align_rslt1_m128i, presult);

    decompress_sse4_c10_m2<5>(c10_align_rslt1_m128i, presult);

    decompress_sse4_c10_m2<10>(c10_align_rslt1_m128i, presult);

    __m128i c10_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c10_align_rslt2_m128i = _mm_alignr_epi8(c10_load_rslt3_m128i, c10_load_rslt2_m128i, 14);
    decompress_sse4_c10_m2<0>(c10_align_rslt2_m128i, presult);

    decompress_sse4_c10_m2<5>(c10_align_rslt2_m128i, presult);

    decompress_sse4_c10_m2<10>(c10_align_rslt2_m128i, presult);

    __m128i c10_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c10_align_rslt3_m128i = _mm_alignr_epi8(c10_load_rslt4_m128i, c10_load_rslt3_m128i, 13);
    decompress_sse4_c10_m2<0>(c10_align_rslt3_m128i, presult);

    decompress_sse4_c10_m2<5>(c10_align_rslt3_m128i, presult);

    decompress_sse4_c10_m2<10>(c10_align_rslt3_m128i, presult);

    __m128i c10_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c10_align_rslt4_m128i = _mm_alignr_epi8(c10_load_rslt5_m128i, c10_load_rslt4_m128i, 12);
    decompress_sse4_c10_m2<0>(c10_align_rslt4_m128i, presult);

    decompress_sse4_c10_m2<5>(c10_align_rslt4_m128i, presult);

    decompress_sse4_c10_m2<10>(c10_align_rslt4_m128i, presult);

    decompress_sse4_c10_m2<11>(c10_load_rslt5_m128i, presult);
}

void decompress_sse4_c10(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c10_m1(vector_m128i, presult + index, offset);
        offset += 5;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_10(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c11
const __m128i SSE4_c11_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0x0F, 0xFE, 0x00, 0x01, 0xFF, 0xC0, 0x00, 0x00, 0x3F, 0xF8, 0x00, 0x00, 0x07, 0xFF);

const __m128i SSE4_c11_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0xFF, 0xE0, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x03, 0xFF, 0x80, 0x00, 0x00, 0x7F, 0xF0);

const __m128i SSE4_c11_mul_msk1_m128i = _mm_set_epi32(0x20, 0x01, 0x08, 0x40);

const __m128i SSE4_c11_mul_msk2_m128i = _mm_set_epi32(0x04, 0x20, 0x01, 0x08);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c11_m2(__m128i c11_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c11_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 5, segment + 4, 0xFF, segment + 4, segment + 3, segment + 2, 0xFF, 0xFF,
                     segment + 2, segment + 1, 0xFF, 0xFF, segment + 1, segment);

    __m128i c11_shfl_rslt_m128i = _mm_shuffle_epi8(c11_load_vec_m128i, SSE4_c11_shfl_msk_m128i);
    __m128i c11_and_rslt_m128i = _mm_and_si128(c11_shfl_rslt_m128i, SSE4_c11_and_msk1_m128i);
    __m128i c11_mul_rslt_m128i = _mm_mullo_epi32(c11_and_rslt_m128i, SSE4_c11_mul_msk1_m128i);
    __m128i c11_rslt_m128i = _mm_srli_epi32(c11_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c11_rslt_m128i);

    presult = presult + 4;
    // decompress second 4 values
    SSE4_c11_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 10, segment + 9, 0xFF, 0xFF, segment + 9, segment + 8, 0xFF, segment + 8,
                     segment + 7, segment + 6, 0xFF, 0xFF, segment + 6, segment + 5);

    c11_shfl_rslt_m128i = _mm_shuffle_epi8(c11_load_vec_m128i, SSE4_c11_shfl_msk_m128i);
    c11_and_rslt_m128i = _mm_and_si128(c11_shfl_rslt_m128i, SSE4_c11_and_msk2_m128i);
    c11_mul_rslt_m128i = _mm_mullo_epi32(c11_and_rslt_m128i, SSE4_c11_mul_msk2_m128i);
    c11_rslt_m128i = _mm_srli_epi32(c11_mul_rslt_m128i, 7);

    _mm_storeu_si128((__m128i*)presult, c11_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c11_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c11_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c11_m2<0>(c11_load_rslt1_m128i, presult);

    __m128i c11_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c11_align_rslt1_m128i = _mm_alignr_epi8(c11_load_rslt2_m128i, c11_load_rslt1_m128i, 11);
    decompress_sse4_c11_m2<0>(c11_align_rslt1_m128i, presult);

    __m128i c11_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c11_align_rslt2_m128i = _mm_alignr_epi8(c11_load_rslt3_m128i, c11_load_rslt2_m128i, 6);
    decompress_sse4_c11_m2<0>(c11_align_rslt2_m128i, presult);

    decompress_sse4_c11_m2<1>(c11_load_rslt3_m128i, presult);

    __m128i c11_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c11_align_rslt3_m128i = _mm_alignr_epi8(c11_load_rslt4_m128i, c11_load_rslt3_m128i, 12);
    decompress_sse4_c11_m2<0>(c11_align_rslt3_m128i, presult);

    __m128i c11_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c11_align_rslt4_m128i = _mm_alignr_epi8(c11_load_rslt5_m128i, c11_load_rslt4_m128i, 7);
    decompress_sse4_c11_m2<0>(c11_align_rslt4_m128i, presult);

    decompress_sse4_c11_m2<2>(c11_load_rslt5_m128i, presult);

    __m128i c11_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c11_align_rslt5_m128i = _mm_alignr_epi8(c11_load_rslt6_m128i, c11_load_rslt5_m128i, 13);
    decompress_sse4_c11_m2<0>(c11_align_rslt5_m128i, presult);

    __m128i c11_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c11_align_rslt6_m128i = _mm_alignr_epi8(c11_load_rslt7_m128i, c11_load_rslt6_m128i, 8);
    decompress_sse4_c11_m2<0>(c11_align_rslt6_m128i, presult);

    decompress_sse4_c11_m2<3>(c11_load_rslt7_m128i, presult);

    __m128i c11_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c11_align_rslt7_m128i = _mm_alignr_epi8(c11_load_rslt8_m128i, c11_load_rslt7_m128i, 14);
    decompress_sse4_c11_m2<0>(c11_align_rslt7_m128i, presult);

    __m128i c11_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c11_align_rslt8_m128i = _mm_alignr_epi8(c11_load_rslt9_m128i, c11_load_rslt8_m128i, 9);
    decompress_sse4_c11_m2<0>(c11_align_rslt8_m128i, presult);

    decompress_sse4_c11_m2<4>(c11_load_rslt9_m128i, presult);

    __m128i c11_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c11_align_rslt9_m128i = _mm_alignr_epi8(c11_load_rslt10_m128i, c11_load_rslt9_m128i, 15);
    decompress_sse4_c11_m2<0>(c11_align_rslt9_m128i, presult);

    __m128i c11_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c11_align_rslt10_m128i = _mm_alignr_epi8(c11_load_rslt11_m128i, c11_load_rslt10_m128i, 10);
    decompress_sse4_c11_m2<0>(c11_align_rslt10_m128i, presult);

    decompress_sse4_c11_m2<5>(c11_load_rslt11_m128i, presult);
}

void decompress_sse4_c11(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c11_m1(vector_m128i, presult + index, offset);
        offset += 11;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_11(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c12
const __m128i SSE4_c12_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xFF);

const __m128i SSE4_c12_mul_msk1_m128i = _mm_set_epi32(0x01, 0x10, 0x01, 0x10);

template <int segment>
inline void decompress_sse4_c12_m2(__m128i c12_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c12_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 5, segment + 4, 0xFF, 0xFF, segment + 4, segment + 3, 0xFF, 0xFF,
                     segment + 2, segment + 1, 0xFF, 0xFF, segment + 1, segment);

    __m128i c12_shfl_rslt_m128i = _mm_shuffle_epi8(c12_load_vec_m128i, SSE4_c12_shfl_msk_m128i);
    __m128i c12_and_rslt_m128i = _mm_and_si128(c12_shfl_rslt_m128i, SSE4_c12_and_msk1_m128i);
    __m128i c12_mul_rslt_m128i = _mm_mullo_epi32(c12_and_rslt_m128i, SSE4_c12_mul_msk1_m128i);
    __m128i c12_rslt_m128i = _mm_srli_epi32(c12_mul_rslt_m128i, 4);
    _mm_storeu_si128((__m128i*)presult, c12_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c12_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c12_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c12_m2<0>(c12_load_rslt1_m128i, presult);

    decompress_sse4_c12_m2<6>(c12_load_rslt1_m128i, presult);

    __m128i c12_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c12_align_rslt1_m128i = _mm_alignr_epi8(c12_load_rslt2_m128i, c12_load_rslt1_m128i, 12);
    decompress_sse4_c12_m2<0>(c12_align_rslt1_m128i, presult);

    decompress_sse4_c12_m2<6>(c12_align_rslt1_m128i, presult);

    decompress_sse4_c12_m2<8>(c12_load_rslt2_m128i, presult);

    __m128i c12_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c12_align_rslt2_m128i = _mm_alignr_epi8(c12_load_rslt3_m128i, c12_load_rslt2_m128i, 14);
    decompress_sse4_c12_m2<0>(c12_align_rslt2_m128i, presult);

    decompress_sse4_c12_m2<6>(c12_align_rslt2_m128i, presult);

    decompress_sse4_c12_m2<10>(c12_load_rslt3_m128i, presult);
}

void decompress_sse4_c12(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 32 <= n) {
        decompress_sse4_c12_m1(vector_m128i, presult + index, offset);
        offset += 3;
        index += 32;
    }
    if (n & 0x1F) {
        unpack_12(dest + index, encode + offset * 4, (n & 0x1F));
    }
}

// c13
const __m128i SSE4_c13_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x0F, 0xFF, 0x80, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x03, 0xFF, 0xE0, 0x00, 0x00, 0x1F, 0xFF);

const __m128i SSE4_c13_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xC0, 0x00, 0x00, 0x3F, 0xFE, 0x00, 0x01, 0xFF, 0xF0);

const __m128i SSE4_c13_mul_msk1_m128i = _mm_set_epi32(0x01, 0x20, 0x04, 0x80);

const __m128i SSE4_c13_mul_msk2_m128i = _mm_set_epi32(0x08, 0x01, 0x20, 0x04);

template <int segment>
inline void decompress_sse4_c13_m2(__m128i c13_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c13_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 6, segment + 5, segment + 4, 0xFF, 0xFF, segment + 4, segment + 3, 0xFF,
                     segment + 3, segment + 2, segment + 1, 0xFF, 0xFF, segment + 1, segment);

    __m128i c13_shfl_rslt_m128i = _mm_shuffle_epi8(c13_load_vec_m128i, SSE4_c13_shfl_msk_m128i);
    __m128i c13_and_rslt_m128i = _mm_and_si128(c13_shfl_rslt_m128i, SSE4_c13_and_msk1_m128i);
    __m128i c13_mul_rslt_m128i = _mm_mullo_epi32(c13_and_rslt_m128i, SSE4_c13_mul_msk1_m128i);
    __m128i c13_rslt_m128i = _mm_srli_epi32(c13_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c13_rslt_m128i);

    presult = presult + 4;

    // decompress second 4 values
    SSE4_c13_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 12, segment + 11, 0xFF, segment + 11, segment + 10, segment + 9, 0xFF, 0xFF,
                     segment + 9, segment + 8, 0xFF, segment + 8, segment + 7, segment + 6);

    c13_shfl_rslt_m128i = _mm_shuffle_epi8(c13_load_vec_m128i, SSE4_c13_shfl_msk_m128i);
    c13_and_rslt_m128i = _mm_and_si128(c13_shfl_rslt_m128i, SSE4_c13_and_msk2_m128i);
    c13_mul_rslt_m128i = _mm_mullo_epi32(c13_and_rslt_m128i, SSE4_c13_mul_msk2_m128i);
    c13_rslt_m128i = _mm_srli_epi32(c13_mul_rslt_m128i, 6);

    _mm_storeu_si128((__m128i*)presult, c13_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c13_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c13_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c13_m2<0>(c13_load_rslt1_m128i, presult);

    __m128i c13_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c13_align_rslt1_m128i = _mm_alignr_epi8(c13_load_rslt2_m128i, c13_load_rslt1_m128i, 13);
    decompress_sse4_c13_m2<0>(c13_align_rslt1_m128i, presult);

    __m128i c13_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c13_align_rslt2_m128i = _mm_alignr_epi8(c13_load_rslt3_m128i, c13_load_rslt2_m128i, 10);
    decompress_sse4_c13_m2<0>(c13_align_rslt2_m128i, presult);

    __m128i c13_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c13_align_rslt3_m128i = _mm_alignr_epi8(c13_load_rslt4_m128i, c13_load_rslt3_m128i, 7);
    decompress_sse4_c13_m2<0>(c13_align_rslt3_m128i, presult);

    __m128i c13_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c13_align_rslt4_m128i = _mm_alignr_epi8(c13_load_rslt5_m128i, c13_load_rslt4_m128i, 4);
    decompress_sse4_c13_m2<0>(c13_align_rslt4_m128i, presult);

    decompress_sse4_c13_m2<1>(c13_load_rslt5_m128i, presult);

    __m128i c13_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c13_align_rslt5_m128i = _mm_alignr_epi8(c13_load_rslt6_m128i, c13_load_rslt5_m128i, 14);
    decompress_sse4_c13_m2<0>(c13_align_rslt5_m128i, presult);

    __m128i c13_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c13_align_rslt6_m128i = _mm_alignr_epi8(c13_load_rslt7_m128i, c13_load_rslt6_m128i, 11);
    decompress_sse4_c13_m2<0>(c13_align_rslt6_m128i, presult);

    __m128i c13_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c13_align_rslt7_m128i = _mm_alignr_epi8(c13_load_rslt8_m128i, c13_load_rslt7_m128i, 8);
    decompress_sse4_c13_m2<0>(c13_align_rslt7_m128i, presult);

    __m128i c13_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c13_align_rslt8_m128i = _mm_alignr_epi8(c13_load_rslt9_m128i, c13_load_rslt8_m128i, 5);
    decompress_sse4_c13_m2<0>(c13_align_rslt8_m128i, presult);

    decompress_sse4_c13_m2<2>(c13_load_rslt9_m128i, presult);

    __m128i c13_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c13_align_rslt9_m128i = _mm_alignr_epi8(c13_load_rslt10_m128i, c13_load_rslt9_m128i, 15);
    decompress_sse4_c13_m2<0>(c13_align_rslt9_m128i, presult);

    __m128i c13_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c13_align_rslt10_m128i = _mm_alignr_epi8(c13_load_rslt11_m128i, c13_load_rslt10_m128i, 12);
    decompress_sse4_c13_m2<0>(c13_align_rslt10_m128i, presult);

    __m128i c13_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c13_align_rslt11_m128i = _mm_alignr_epi8(c13_load_rslt12_m128i, c13_load_rslt11_m128i, 9);
    decompress_sse4_c13_m2<0>(c13_align_rslt11_m128i, presult);

    __m128i c13_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c13_align_rslt12_m128i = _mm_alignr_epi8(c13_load_rslt13_m128i, c13_load_rslt12_m128i, 6);
    decompress_sse4_c13_m2<0>(c13_align_rslt12_m128i, presult);

    decompress_sse4_c13_m2<3>(c13_load_rslt13_m128i, presult);
}

void decompress_sse4_c13(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c13_m1(vector_m128i, presult + index, offset);
        offset += 13;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_13(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c14
const __m128i SSE4_c14_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x00, 0xFF, 0xFC, 0x00, 0x03, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0xC0, 0x00, 0x00, 0x3F, 0xFF);

const __m128i SSE4_c14_mul_msk1_m128i = _mm_set_epi32(0x10, 0x04, 0x01, 0x40);

template <int segment>
inline void decompress_sse4_c14_m2(__m128i c14_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c14_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 6, segment + 5, 0xFF, segment + 5, segment + 4, segment + 3, 0xFF,
                     segment + 3, segment + 2, segment + 1, 0xFF, 0xFF, segment + 1, segment);

    __m128i c14_shfl_rslt_m128i = _mm_shuffle_epi8(c14_load_vec_m128i, SSE4_c14_shfl_msk_m128i);
    __m128i c14_and_rslt_m128i = _mm_and_si128(c14_shfl_rslt_m128i, SSE4_c14_and_msk1_m128i);
    __m128i c14_mul_rslt_m128i = _mm_mullo_epi32(c14_and_rslt_m128i, SSE4_c14_mul_msk1_m128i);
    __m128i c14_rslt_m128i = _mm_srli_epi32(c14_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c14_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c14_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c14_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c14_m2<0>(c14_load_rslt1_m128i, presult);

    decompress_sse4_c14_m2<7>(c14_load_rslt1_m128i, presult);

    __m128i c14_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c14_align_rslt1_m128i = _mm_alignr_epi8(c14_load_rslt2_m128i, c14_load_rslt1_m128i, 14);
    decompress_sse4_c14_m2<0>(c14_align_rslt1_m128i, presult);

    decompress_sse4_c14_m2<7>(c14_align_rslt1_m128i, presult);

    __m128i c14_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c14_align_rslt2_m128i = _mm_alignr_epi8(c14_load_rslt3_m128i, c14_load_rslt2_m128i, 12);
    decompress_sse4_c14_m2<0>(c14_align_rslt2_m128i, presult);

    decompress_sse4_c14_m2<7>(c14_align_rslt2_m128i, presult);

    __m128i c14_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c14_align_rslt3_m128i = _mm_alignr_epi8(c14_load_rslt4_m128i, c14_load_rslt3_m128i, 10);
    decompress_sse4_c14_m2<0>(c14_align_rslt3_m128i, presult);

    decompress_sse4_c14_m2<7>(c14_align_rslt3_m128i, presult);

    __m128i c14_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c14_align_rslt4_m128i = _mm_alignr_epi8(c14_load_rslt5_m128i, c14_load_rslt4_m128i, 8);
    decompress_sse4_c14_m2<0>(c14_align_rslt4_m128i, presult);

    decompress_sse4_c14_m2<7>(c14_align_rslt4_m128i, presult);

    __m128i c14_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c14_align_rslt5_m128i = _mm_alignr_epi8(c14_load_rslt6_m128i, c14_load_rslt5_m128i, 6);
    decompress_sse4_c14_m2<0>(c14_align_rslt5_m128i, presult);

    decompress_sse4_c14_m2<7>(c14_align_rslt5_m128i, presult);

    __m128i c14_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c14_align_rslt6_m128i = _mm_alignr_epi8(c14_load_rslt7_m128i, c14_load_rslt6_m128i, 4);
    decompress_sse4_c14_m2<0>(c14_align_rslt6_m128i, presult);

    decompress_sse4_c14_m2<7>(c14_align_rslt6_m128i, presult);

    decompress_sse4_c14_m2<2>(c14_load_rslt7_m128i, presult);

    decompress_sse4_c14_m2<9>(c14_load_rslt7_m128i, presult);
}

void decompress_sse4_c14(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c14_m1(vector_m128i, presult + index, offset);
        offset += 7;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_14(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c15
const __m128i SSE4_c15_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x0F, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xC0, 0x00, 0x3F, 0xFF, 0x80, 0x00, 0x00, 0x7F, 0xFF);

const __m128i SSE4_c15_and_msk2_m128i =
    _mm_set_epi8(0x00, 0x00, 0xFF, 0xFE, 0x00, 0x01, 0xFF, 0xFC, 0x00, 0x03, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xF0);

const __m128i SSE4_c15_mul_msk1_m128i = _mm_set_epi32(0x04, 0x02, 0x01, 0x80);

const __m128i SSE4_c15_mul_msk2_m128i = _mm_set_epi32(0x08, 0x04, 0x02, 0x01);

template <int segment>
inline void decompress_sse4_c15_m2(__m128i c15_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c15_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 7, segment + 6, segment + 5, 0xFF, segment + 5, segment + 4, segment + 3, 0xFF,
                     segment + 3, segment + 2, segment + 1, 0xFF, 0xFF, segment + 1, segment);

    __m128i c15_shfl_rslt_m128i = _mm_shuffle_epi8(c15_load_vec_m128i, SSE4_c15_shfl_msk_m128i);
    __m128i c15_and_rslt_m128i = _mm_and_si128(c15_shfl_rslt_m128i, SSE4_c15_and_msk1_m128i);
    __m128i c15_mul_rslt_m128i = _mm_mullo_epi32(c15_and_rslt_m128i, SSE4_c15_mul_msk1_m128i);
    __m128i c15_rslt_m128i = _mm_srli_epi32(c15_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c15_rslt_m128i);

    presult = presult + 4;

    // decompress second 4 values
    SSE4_c15_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 14, segment + 13, 0xFF, segment + 13, segment + 12, segment + 11, 0xFF,
                     segment + 11, segment + 10, segment + 9, 0xFF, segment + 9, segment + 8, segment + 7);

    c15_shfl_rslt_m128i = _mm_shuffle_epi8(c15_load_vec_m128i, SSE4_c15_shfl_msk_m128i);
    c15_and_rslt_m128i = _mm_and_si128(c15_shfl_rslt_m128i, SSE4_c15_and_msk2_m128i);
    c15_mul_rslt_m128i = _mm_mullo_epi32(c15_and_rslt_m128i, SSE4_c15_mul_msk2_m128i);
    c15_rslt_m128i = _mm_srli_epi32(c15_mul_rslt_m128i, 4);

    _mm_storeu_si128((__m128i*)presult, c15_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c15_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c15_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c15_m2<0>(c15_load_rslt1_m128i, presult);

    __m128i c15_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c15_align_rslt1_m128i = _mm_alignr_epi8(c15_load_rslt2_m128i, c15_load_rslt1_m128i, 15);
    decompress_sse4_c15_m2<0>(c15_align_rslt1_m128i, presult);

    __m128i c15_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c15_align_rslt2_m128i = _mm_alignr_epi8(c15_load_rslt3_m128i, c15_load_rslt2_m128i, 14);
    decompress_sse4_c15_m2<0>(c15_align_rslt2_m128i, presult);

    __m128i c15_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c15_align_rslt3_m128i = _mm_alignr_epi8(c15_load_rslt4_m128i, c15_load_rslt3_m128i, 13);
    decompress_sse4_c15_m2<0>(c15_align_rslt3_m128i, presult);

    __m128i c15_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c15_align_rslt4_m128i = _mm_alignr_epi8(c15_load_rslt5_m128i, c15_load_rslt4_m128i, 12);
    decompress_sse4_c15_m2<0>(c15_align_rslt4_m128i, presult);

    __m128i c15_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c15_align_rslt5_m128i = _mm_alignr_epi8(c15_load_rslt6_m128i, c15_load_rslt5_m128i, 11);
    decompress_sse4_c15_m2<0>(c15_align_rslt5_m128i, presult);

    __m128i c15_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c15_align_rslt6_m128i = _mm_alignr_epi8(c15_load_rslt7_m128i, c15_load_rslt6_m128i, 10);
    decompress_sse4_c15_m2<0>(c15_align_rslt6_m128i, presult);

    __m128i c15_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c15_align_rslt7_m128i = _mm_alignr_epi8(c15_load_rslt8_m128i, c15_load_rslt7_m128i, 9);
    decompress_sse4_c15_m2<0>(c15_align_rslt7_m128i, presult);

    __m128i c15_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c15_align_rslt8_m128i = _mm_alignr_epi8(c15_load_rslt9_m128i, c15_load_rslt8_m128i, 8);
    decompress_sse4_c15_m2<0>(c15_align_rslt8_m128i, presult);

    __m128i c15_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c15_align_rslt9_m128i = _mm_alignr_epi8(c15_load_rslt10_m128i, c15_load_rslt9_m128i, 7);
    decompress_sse4_c15_m2<0>(c15_align_rslt9_m128i, presult);

    __m128i c15_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c15_align_rslt10_m128i = _mm_alignr_epi8(c15_load_rslt11_m128i, c15_load_rslt10_m128i, 6);
    decompress_sse4_c15_m2<0>(c15_align_rslt10_m128i, presult);

    __m128i c15_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c15_align_rslt11_m128i = _mm_alignr_epi8(c15_load_rslt12_m128i, c15_load_rslt11_m128i, 5);
    decompress_sse4_c15_m2<0>(c15_align_rslt11_m128i, presult);

    __m128i c15_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c15_align_rslt12_m128i = _mm_alignr_epi8(c15_load_rslt13_m128i, c15_load_rslt12_m128i, 4);
    decompress_sse4_c15_m2<0>(c15_align_rslt12_m128i, presult);

    __m128i c15_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c15_align_rslt13_m128i = _mm_alignr_epi8(c15_load_rslt14_m128i, c15_load_rslt13_m128i, 3);
    decompress_sse4_c15_m2<0>(c15_align_rslt13_m128i, presult);

    __m128i c15_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c15_align_rslt14_m128i = _mm_alignr_epi8(c15_load_rslt15_m128i, c15_load_rslt14_m128i, 2);
    decompress_sse4_c15_m2<0>(c15_align_rslt14_m128i, presult);

    decompress_sse4_c15_m2<1>(c15_load_rslt15_m128i, presult);
}

/*
const __m128i SSE4_c15_and_msk1_m128i = _mm_set_epi8 (0x00, 0x0F, 0xFF, 0xE0,
                              0x00, 0x1F, 0xFF, 0xC0,
                              0x00, 0x3F, 0xFF, 0x80,
                              0x00, 0x00, 0x7F, 0xFF);

const __m128i SSE4_c15_and_msk2_m128i = _mm_set_epi8 (0x00, 0x00, 0xFF, 0xFE,
                              0x00, 0x01, 0xFF, 0xFC,
                              0x00, 0x03, 0xFF, 0xF8,
                              0x00, 0x07, 0xFF, 0xF0);



#ifdef MAVX2
  const __m128i SSE4_c15_srlv_off1 = _mm_set_epi32(5, 6, 7, 0);
  const __m128i SSE4_c15_srlv_off2 = _mm_set_epi32(1, 2, 3, 4);
#else
  const __m128i SSE4_c15_mul_msk1_m128i = _mm_set_epi32(0x04, 0x02, 0x01, 0x80);
  const __m128i SSE4_c15_mul_msk2_m128i = _mm_set_epi32(0x08, 0x04, 0x02, 0x01);
#endif



////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

const      __m128i SSE4_c15_shfl_msk_m2_1 = _mm_set_epi8(0xFF, 7, 6, 5,
                            0xFF, 5, 4, 3,
                            0xFF, 3, 2, 1,
                            0xFF, 0xFF, 1, 0);

const      __m128i SSE4_c15_shfl_msk_m2_2 = _mm_set_epi8(0xFF, 0xFF, 14, 13,
                        0xFF, 13, 12, 11,
                        0xFF, 11, 10, 9,
                        0xFF, 9, 8, 7);

template <int segment>
inline void decompress_sse4_c15_m2(__m128i c15_load_vec_m128i, unsigned int*& presult)
{
     // decompress first 4 values

     __m128i c15_shfl_rslt_m128i  = _mm_shuffle_epi8(c15_load_vec_m128i, SSE4_c15_shfl_msk_m2_1);
     __m128i c15_and_rslt_m128i   = _mm_and_si128(c15_shfl_rslt_m128i, SSE4_c15_and_msk1_m128i);
#ifdef MAVX2
     __m128i c15_rslt_m128i = _mm_srlv_epi32(c15_and_rslt_m128i, SSE4_c15_srlv_off1);
#else
     __m128i c15_mul_rslt_m128i   = _mm_mullo_epi32(c15_and_rslt_m128i, SSE4_c15_mul_msk1_m128i);
     __m128i c15_rslt_m128i = _mm_srli_epi32(c15_mul_rslt_m128i, 7);
#endif

     _mm_storeu_si128((__m128i*)presult, c15_rslt_m128i);

     presult = presult + 4;

     // decompress second 4 values

     c15_shfl_rslt_m128i  = _mm_shuffle_epi8(c15_load_vec_m128i, SSE4_c15_shfl_msk_m2_2);
     c15_and_rslt_m128i   = _mm_and_si128(c15_shfl_rslt_m128i, SSE4_c15_and_msk2_m128i);

#ifdef MAVX2
     c15_rslt_m128i = _mm_srlv_epi32(c15_and_rslt_m128i, SSE4_c15_srlv_off2);
#else
     c15_mul_rslt_m128i   = _mm_mullo_epi32(c15_and_rslt_m128i, SSE4_c15_mul_msk2_m128i);
     c15_rslt_m128i = _mm_srli_epi32(c15_mul_rslt_m128i, 4);
 #endif
     _mm_storeu_si128((__m128i*)presult, c15_rslt_m128i);

     presult = presult + 4;

}

const     __m128i SSE4_c15_shfl_msk_m2_last_1 = _mm_set_epi8(0xFF, 8, 7, 6,
                            0xFF, 6, 5, 4,
                            0xFF, 4, 3, 2,
                            0xFF, 0xFF, 2, 1);

const     __m128i SSE4_c15_shfl_msk_m2_last_2 = _mm_set_epi8(0xFF, 0xFF, 15, 14,
                        0xFF, 14, 13, 12,
                        0xFF, 12, 11, 10,
                        0xFF, 10, 9, 8);

template <int segment>
inline void decompress_sse4_c15_m2_last(__m128i c15_load_vec_m128i, unsigned int*& presult)
{
     // decompress first 4 values


     __m128i c15_shfl_rslt_m128i  = _mm_shuffle_epi8(c15_load_vec_m128i, SSE4_c15_shfl_msk_m2_last_1);
     __m128i c15_and_rslt_m128i   = _mm_and_si128(c15_shfl_rslt_m128i, SSE4_c15_and_msk1_m128i);

#ifdef MAVX2
     __m128i c15_rslt_m128i = _mm_srlv_epi32(c15_and_rslt_m128i, SSE4_c15_srlv_off1);
#else
     __m128i c15_mul_rslt_m128i   = _mm_mullo_epi32(c15_and_rslt_m128i, SSE4_c15_mul_msk1_m128i);
     __m128i c15_rslt_m128i = _mm_srli_epi32(c15_mul_rslt_m128i, 7);
#endif
     _mm_storeu_si128((__m128i*)presult, c15_rslt_m128i);

     presult = presult + 4;

     c15_shfl_rslt_m128i  = _mm_shuffle_epi8(c15_load_vec_m128i, SSE4_c15_shfl_msk_m2_last_2);
     c15_and_rslt_m128i   = _mm_and_si128(c15_shfl_rslt_m128i, SSE4_c15_and_msk2_m128i);



#ifdef MAVX2
     c15_rslt_m128i = _mm_srlv_epi32(c15_and_rslt_m128i, SSE4_c15_srlv_off2);
#else
     c15_mul_rslt_m128i   = _mm_mullo_epi32(c15_and_rslt_m128i, SSE4_c15_mul_msk2_m128i);
     c15_rslt_m128i = _mm_srli_epi32(c15_mul_rslt_m128i, 4);
#endif

     _mm_storeu_si128((__m128i*)presult, c15_rslt_m128i);

     presult = presult + 4;

}


inline void decompress_sse4_c15_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
     __m128i* its_vector_m128i = vector_m128i + offset;

     __m128i c15_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
     decompress_sse4_c15_m2<0>(c15_load_rslt1_m128i, presult);

     __m128i c15_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
     __m128i c15_align_rslt1_m128i = _mm_alignr_epi8(c15_load_rslt2_m128i, c15_load_rslt1_m128i, 15);
     decompress_sse4_c15_m2<0>(c15_align_rslt1_m128i, presult);

     __m128i c15_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
     __m128i c15_align_rslt2_m128i = _mm_alignr_epi8(c15_load_rslt3_m128i, c15_load_rslt2_m128i, 14);
     decompress_sse4_c15_m2<0>(c15_align_rslt2_m128i, presult);

     __m128i c15_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
     __m128i c15_align_rslt3_m128i = _mm_alignr_epi8(c15_load_rslt4_m128i, c15_load_rslt3_m128i, 13);
     decompress_sse4_c15_m2<0>(c15_align_rslt3_m128i, presult);

     __m128i c15_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
     __m128i c15_align_rslt4_m128i = _mm_alignr_epi8(c15_load_rslt5_m128i, c15_load_rslt4_m128i, 12);
     decompress_sse4_c15_m2<0>(c15_align_rslt4_m128i, presult);

     __m128i c15_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
     __m128i c15_align_rslt5_m128i = _mm_alignr_epi8(c15_load_rslt6_m128i, c15_load_rslt5_m128i, 11);
     decompress_sse4_c15_m2<0>(c15_align_rslt5_m128i, presult);

     __m128i c15_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
     __m128i c15_align_rslt6_m128i = _mm_alignr_epi8(c15_load_rslt7_m128i, c15_load_rslt6_m128i, 10);
     decompress_sse4_c15_m2<0>(c15_align_rslt6_m128i, presult);

     __m128i c15_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
     __m128i c15_align_rslt7_m128i = _mm_alignr_epi8(c15_load_rslt8_m128i, c15_load_rslt7_m128i, 9);
     decompress_sse4_c15_m2<0>(c15_align_rslt7_m128i, presult);

     __m128i c15_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
     __m128i c15_align_rslt8_m128i = _mm_alignr_epi8(c15_load_rslt9_m128i, c15_load_rslt8_m128i, 8);
     decompress_sse4_c15_m2<0>(c15_align_rslt8_m128i, presult);

     __m128i c15_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
     __m128i c15_align_rslt9_m128i = _mm_alignr_epi8(c15_load_rslt10_m128i, c15_load_rslt9_m128i, 7);
     decompress_sse4_c15_m2<0>(c15_align_rslt9_m128i, presult);

     __m128i c15_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
     __m128i c15_align_rslt10_m128i = _mm_alignr_epi8(c15_load_rslt11_m128i, c15_load_rslt10_m128i, 6);
     decompress_sse4_c15_m2<0>(c15_align_rslt10_m128i, presult);

     __m128i c15_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
     __m128i c15_align_rslt11_m128i = _mm_alignr_epi8(c15_load_rslt12_m128i, c15_load_rslt11_m128i, 5);
     decompress_sse4_c15_m2<0>(c15_align_rslt11_m128i, presult);

     __m128i c15_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
     __m128i c15_align_rslt12_m128i = _mm_alignr_epi8(c15_load_rslt13_m128i, c15_load_rslt12_m128i, 4);
     decompress_sse4_c15_m2<0>(c15_align_rslt12_m128i, presult);

     __m128i c15_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
     __m128i c15_align_rslt13_m128i = _mm_alignr_epi8(c15_load_rslt14_m128i, c15_load_rslt13_m128i, 3);
     decompress_sse4_c15_m2<0>(c15_align_rslt13_m128i, presult);

     __m128i c15_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
     __m128i c15_align_rslt14_m128i = _mm_alignr_epi8(c15_load_rslt15_m128i, c15_load_rslt14_m128i, 2);
     decompress_sse4_c15_m2<0>(c15_align_rslt14_m128i, presult);

     decompress_sse4_c15_m2_last<1>(c15_load_rslt15_m128i, presult);

}
*/
void decompress_sse4_c15(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c15_m1(vector_m128i, presult + index, offset);
        offset += 15;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_15(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c16
template <int segment>
inline void decompress_sse4_c16_m2(__m128i c16_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c16_shfl_msk_m128i =
        _mm_set_epi8(0xFF, 0xFF, segment + 7, segment + 6, 0xFF, 0xFF, segment + 5, segment + 4, 0xFF, 0xFF,
                     segment + 3, segment + 2, 0xFF, 0xFF, segment + 1, segment);

    __m128i c16_rslt_m128i = _mm_shuffle_epi8(c16_load_vec_m128i, SSE4_c16_shfl_msk_m128i);
    _mm_storeu_si128((__m128i*)presult, c16_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c16_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c16_load_rslt_m128i = _mm_loadu_si128(its_vector_m128i);

    decompress_sse4_c16_m2<0>(c16_load_rslt_m128i, presult);

    decompress_sse4_c16_m2<8>(c16_load_rslt_m128i, presult);
}

void decompress_sse4_c16(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 8 <= n) {
        decompress_sse4_c16_m1(vector_m128i, presult + index, offset);
        offset += 1;
        index += 8;
    }
    if (n & 0x7) {
        unpack_16(dest + index, encode + offset * 4, (n & 0x7));
    }
}

// c17
const __m128i SSE4_c17_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x0F, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xFC, 0x00, 0x03, 0xFF, 0xFE, 0x00, 0x01, 0xFF, 0xFF);

const __m128i SSE4_c17_and_msk2_m128i =
    _mm_set_epi8(0x00, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0xFF, 0xC0, 0x00, 0x3F, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xF0);

const __m128i SSE4_c17_mul_msk1_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

const __m128i SSE4_c17_mul_msk2_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

template <int segment>
inline void decompress_sse4_c17_m2_s1(__m128i c17_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c17_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 8, segment + 7, segment + 6, 0xFF, segment + 6, segment + 5, segment + 4, 0xFF,
                     segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c17_shfl_rslt_m128i = _mm_shuffle_epi8(c17_load_vec_m128i, SSE4_c17_shfl_msk_m128i);
    __m128i c17_and_rslt_m128i = _mm_and_si128(c17_shfl_rslt_m128i, SSE4_c17_and_msk1_m128i);
    __m128i c17_mul_rslt_m128i = _mm_mullo_epi32(c17_and_rslt_m128i, SSE4_c17_mul_msk1_m128i);
    __m128i c17_rslt_m128i = _mm_srli_epi32(c17_mul_rslt_m128i, 3);
    _mm_storeu_si128((__m128i*)presult, c17_rslt_m128i);

    //__m128i c17_equal_rslt_m128i = _mm_cmpeq_epi32(c17_rslt_m128i, SSE4_zero_m128i);
    // int c17_tstz_rslt_int = _mm_testz_si128(c17_equal_rslt_m128i, SSE4_set_m128i);

    presult = presult + 4;
}

template <int segment>
inline void decompress_sse4_c17_m2_s2(__m128i c17_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c17_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 8, segment + 7, segment + 6, 0xFF, segment + 6, segment + 5, segment + 4, 0xFF,
                     segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c17_shfl_rslt_m128i = _mm_shuffle_epi8(c17_load_vec_m128i, SSE4_c17_shfl_msk_m128i);
    __m128i c17_and_rslt_m128i = _mm_and_si128(c17_shfl_rslt_m128i, SSE4_c17_and_msk2_m128i);
    __m128i c17_mul_rslt_m128i = _mm_mullo_epi32(c17_and_rslt_m128i, SSE4_c17_mul_msk2_m128i);
    __m128i c17_rslt_m128i = _mm_srli_epi32(c17_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c17_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c17_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c17_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c17_m2_s1<0>(c17_load_rslt1_m128i, presult);

    __m128i c17_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c17_align_rslt1_m128i = _mm_alignr_epi8(c17_load_rslt2_m128i, c17_load_rslt1_m128i, 8);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt1_m128i, presult);

    decompress_sse4_c17_m2_s1<1>(c17_load_rslt2_m128i, presult);

    __m128i c17_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c17_align_rslt2_m128i = _mm_alignr_epi8(c17_load_rslt3_m128i, c17_load_rslt2_m128i, 9);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt2_m128i, presult);

    decompress_sse4_c17_m2_s1<2>(c17_load_rslt3_m128i, presult);

    __m128i c17_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c17_align_rslt3_m128i = _mm_alignr_epi8(c17_load_rslt4_m128i, c17_load_rslt3_m128i, 10);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt3_m128i, presult);

    decompress_sse4_c17_m2_s1<3>(c17_load_rslt4_m128i, presult);

    __m128i c17_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c17_align_rslt4_m128i = _mm_alignr_epi8(c17_load_rslt5_m128i, c17_load_rslt4_m128i, 11);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt4_m128i, presult);

    decompress_sse4_c17_m2_s1<4>(c17_load_rslt5_m128i, presult);

    __m128i c17_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c17_align_rslt5_m128i = _mm_alignr_epi8(c17_load_rslt6_m128i, c17_load_rslt5_m128i, 12);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt5_m128i, presult);

    decompress_sse4_c17_m2_s1<5>(c17_load_rslt6_m128i, presult);

    __m128i c17_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c17_align_rslt6_m128i = _mm_alignr_epi8(c17_load_rslt7_m128i, c17_load_rslt6_m128i, 13);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt6_m128i, presult);

    decompress_sse4_c17_m2_s1<6>(c17_load_rslt7_m128i, presult);

    __m128i c17_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c17_align_rslt7_m128i = _mm_alignr_epi8(c17_load_rslt8_m128i, c17_load_rslt7_m128i, 14);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt7_m128i, presult);

    decompress_sse4_c17_m2_s1<7>(c17_load_rslt8_m128i, presult);

    __m128i c17_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c17_align_rslt8_m128i = _mm_alignr_epi8(c17_load_rslt9_m128i, c17_load_rslt8_m128i, 15);
    decompress_sse4_c17_m2_s2<0>(c17_align_rslt8_m128i, presult);

    __m128i c17_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c17_align_rslt9_m128i = _mm_alignr_epi8(c17_load_rslt10_m128i, c17_load_rslt9_m128i, 8);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt9_m128i, presult);

    decompress_sse4_c17_m2_s2<0>(c17_load_rslt10_m128i, presult);

    __m128i c17_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c17_align_rslt10_m128i = _mm_alignr_epi8(c17_load_rslt11_m128i, c17_load_rslt10_m128i, 9);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt10_m128i, presult);

    decompress_sse4_c17_m2_s2<1>(c17_load_rslt11_m128i, presult);

    __m128i c17_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c17_align_rslt11_m128i = _mm_alignr_epi8(c17_load_rslt12_m128i, c17_load_rslt11_m128i, 10);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt11_m128i, presult);

    decompress_sse4_c17_m2_s2<2>(c17_load_rslt12_m128i, presult);

    __m128i c17_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c17_align_rslt12_m128i = _mm_alignr_epi8(c17_load_rslt13_m128i, c17_load_rslt12_m128i, 11);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt12_m128i, presult);

    decompress_sse4_c17_m2_s2<3>(c17_load_rslt13_m128i, presult);

    __m128i c17_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c17_align_rslt13_m128i = _mm_alignr_epi8(c17_load_rslt14_m128i, c17_load_rslt13_m128i, 12);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt13_m128i, presult);

    decompress_sse4_c17_m2_s2<4>(c17_load_rslt14_m128i, presult);

    __m128i c17_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c17_align_rslt14_m128i = _mm_alignr_epi8(c17_load_rslt15_m128i, c17_load_rslt14_m128i, 13);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt14_m128i, presult);

    decompress_sse4_c17_m2_s2<5>(c17_load_rslt15_m128i, presult);

    __m128i c17_load_rslt16_m128i = _mm_loadu_si128(its_vector_m128i + 15);
    __m128i c17_align_rslt15_m128i = _mm_alignr_epi8(c17_load_rslt16_m128i, c17_load_rslt15_m128i, 14);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt15_m128i, presult);

    decompress_sse4_c17_m2_s2<6>(c17_load_rslt16_m128i, presult);

    __m128i c17_load_rslt17_m128i = _mm_loadu_si128(its_vector_m128i + 16);
    __m128i c17_align_rslt16_m128i = _mm_alignr_epi8(c17_load_rslt17_m128i, c17_load_rslt16_m128i, 15);
    decompress_sse4_c17_m2_s1<0>(c17_align_rslt16_m128i, presult);

    decompress_sse4_c17_m2_s2<7>(c17_load_rslt17_m128i, presult);
}

void decompress_sse4_c17(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c17_m1(vector_m128i, presult + index, offset);
        offset += 17;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_17(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c18
const __m128i SSE4_c18_and_msk1_m128i =
    _mm_set_epi8(0x00, 0xFF, 0xFF, 0xC0, 0x00, 0x3F, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0xFC, 0x00, 0x03, 0xFF, 0xFF);

const __m128i SSE4_c18_mul_msk1_m128i = _mm_set_epi32(0x01, 0x04, 0x10, 0x40);

template <int segment>
inline void decompress_sse4_c18_m2(__m128i c18_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c18_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 8, segment + 7, segment + 6, 0xFF, segment + 6, segment + 5, segment + 4, 0xFF,
                     segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c18_shfl_rslt_m128i = _mm_shuffle_epi8(c18_load_vec_m128i, SSE4_c18_shfl_msk_m128i);
    __m128i c18_and_rslt_m128i = _mm_and_si128(c18_shfl_rslt_m128i, SSE4_c18_and_msk1_m128i);
    __m128i c18_mul_rslt_m128i = _mm_mullo_epi32(c18_and_rslt_m128i, SSE4_c18_mul_msk1_m128i);
    __m128i c18_rslt_m128i = _mm_srli_epi32(c18_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c18_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c18_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c18_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c18_m2<0>(c18_load_rslt1_m128i, presult);

    __m128i c18_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c18_align_rslt1_m128i = _mm_alignr_epi8(c18_load_rslt2_m128i, c18_load_rslt1_m128i, 9);
    decompress_sse4_c18_m2<0>(c18_align_rslt1_m128i, presult);

    decompress_sse4_c18_m2<2>(c18_load_rslt2_m128i, presult);

    __m128i c18_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c18_align_rslt2_m128i = _mm_alignr_epi8(c18_load_rslt3_m128i, c18_load_rslt2_m128i, 11);
    decompress_sse4_c18_m2<0>(c18_align_rslt2_m128i, presult);

    decompress_sse4_c18_m2<4>(c18_load_rslt3_m128i, presult);

    __m128i c18_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c18_align_rslt3_m128i = _mm_alignr_epi8(c18_load_rslt4_m128i, c18_load_rslt3_m128i, 13);
    decompress_sse4_c18_m2<0>(c18_align_rslt3_m128i, presult);

    decompress_sse4_c18_m2<6>(c18_load_rslt4_m128i, presult);

    __m128i c18_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c18_align_rslt4_m128i = _mm_alignr_epi8(c18_load_rslt5_m128i, c18_load_rslt4_m128i, 15);
    decompress_sse4_c18_m2<0>(c18_align_rslt4_m128i, presult);

    __m128i c18_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c18_align_rslt5_m128i = _mm_alignr_epi8(c18_load_rslt6_m128i, c18_load_rslt5_m128i, 8);
    decompress_sse4_c18_m2<0>(c18_align_rslt5_m128i, presult);

    decompress_sse4_c18_m2<1>(c18_load_rslt6_m128i, presult);

    __m128i c18_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c18_align_rslt6_m128i = _mm_alignr_epi8(c18_load_rslt7_m128i, c18_load_rslt6_m128i, 10);
    decompress_sse4_c18_m2<0>(c18_align_rslt6_m128i, presult);

    decompress_sse4_c18_m2<3>(c18_load_rslt7_m128i, presult);

    __m128i c18_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c18_align_rslt7_m128i = _mm_alignr_epi8(c18_load_rslt8_m128i, c18_load_rslt7_m128i, 12);
    decompress_sse4_c18_m2<0>(c18_align_rslt7_m128i, presult);

    decompress_sse4_c18_m2<5>(c18_load_rslt8_m128i, presult);

    __m128i c18_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c18_align_rslt8_m128i = _mm_alignr_epi8(c18_load_rslt9_m128i, c18_load_rslt8_m128i, 14);
    decompress_sse4_c18_m2<0>(c18_align_rslt8_m128i, presult);

    decompress_sse4_c18_m2<7>(c18_load_rslt9_m128i, presult);
}

void decompress_sse4_c18(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c18_m1(vector_m128i, presult + index, offset);
        offset += 9;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_18(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c19
const __m128i SSE4_c19_and_msk1_m128i =
    _mm_set_epi8(0x00, 0x0F, 0xFF, 0xFE, 0x01, 0xFF, 0xFF, 0xC0, 0x00, 0x3F, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xFF);

const __m128i SSE4_c19_and_msk2_m128i =
    _mm_set_epi8(0x00, 0xFF, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFC, 0x03, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0xFF, 0xF0);

const __m128i SSE4_c19_mul_msk1_m128i = _mm_set_epi32(0x20, 0x01, 0x08, 0x40);

const __m128i SSE4_c19_mul_msk2_m128i = _mm_set_epi32(0x04, 0x20, 0x01, 0x08);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c19_m2_s1(__m128i c19_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c19_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 9, segment + 8, segment + 7, segment + 7, segment + 6, segment + 5, segment + 4,
                     0xFF, segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c19_shfl_rslt_m128i = _mm_shuffle_epi8(c19_load_vec_m128i, SSE4_c19_shfl_msk_m128i);
    __m128i c19_and_rslt_m128i = _mm_and_si128(c19_shfl_rslt_m128i, SSE4_c19_and_msk1_m128i);
    __m128i c19_mul_rslt_m128i = _mm_mullo_epi32(c19_and_rslt_m128i, SSE4_c19_mul_msk1_m128i);
    __m128i c19_rslt_m128i = _mm_srli_epi32(c19_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c19_rslt_m128i);

    presult = presult + 4;
}

template <int segment>
inline void decompress_sse4_c19_m2_s2(__m128i c19_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c19_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 9, segment + 8, segment + 7, 0xFF, segment + 7, segment + 6, segment + 5,
                     segment + 5, segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c19_shfl_rslt_m128i = _mm_shuffle_epi8(c19_load_vec_m128i, SSE4_c19_shfl_msk_m128i);
    __m128i c19_and_rslt_m128i = _mm_and_si128(c19_shfl_rslt_m128i, SSE4_c19_and_msk2_m128i);
    __m128i c19_mul_rslt_m128i = _mm_mullo_epi32(c19_and_rslt_m128i, SSE4_c19_mul_msk2_m128i);
    __m128i c19_rslt_m128i = _mm_srli_epi32(c19_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c19_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c19_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c19_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c19_m2_s1<0>(c19_load_rslt1_m128i, presult);

    __m128i c19_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c19_align_rslt1_m128i = _mm_alignr_epi8(c19_load_rslt2_m128i, c19_load_rslt1_m128i, 9);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt1_m128i, presult);

    decompress_sse4_c19_m2_s1<3>(c19_load_rslt2_m128i, presult);

    __m128i c19_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c19_align_rslt2_m128i = _mm_alignr_epi8(c19_load_rslt3_m128i, c19_load_rslt2_m128i, 12);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt2_m128i, presult);

    decompress_sse4_c19_m2_s1<6>(c19_load_rslt3_m128i, presult);

    __m128i c19_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c19_align_rslt3_m128i = _mm_alignr_epi8(c19_load_rslt4_m128i, c19_load_rslt3_m128i, 15);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt3_m128i, presult);

    __m128i c19_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c19_align_rslt4_m128i = _mm_alignr_epi8(c19_load_rslt5_m128i, c19_load_rslt4_m128i, 9);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt4_m128i, presult);

    decompress_sse4_c19_m2_s2<2>(c19_load_rslt5_m128i, presult);

    __m128i c19_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c19_align_rslt5_m128i = _mm_alignr_epi8(c19_load_rslt6_m128i, c19_load_rslt5_m128i, 12);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt5_m128i, presult);

    decompress_sse4_c19_m2_s2<5>(c19_load_rslt6_m128i, presult);

    __m128i c19_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c19_align_rslt6_m128i = _mm_alignr_epi8(c19_load_rslt7_m128i, c19_load_rslt6_m128i, 15);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt6_m128i, presult);

    __m128i c19_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c19_align_rslt7_m128i = _mm_alignr_epi8(c19_load_rslt8_m128i, c19_load_rslt7_m128i, 8);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt7_m128i, presult);

    decompress_sse4_c19_m2_s1<2>(c19_load_rslt8_m128i, presult);

    __m128i c19_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c19_align_rslt8_m128i = _mm_alignr_epi8(c19_load_rslt9_m128i, c19_load_rslt8_m128i, 11);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt8_m128i, presult);

    decompress_sse4_c19_m2_s1<5>(c19_load_rslt9_m128i, presult);

    __m128i c19_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c19_align_rslt9_m128i = _mm_alignr_epi8(c19_load_rslt10_m128i, c19_load_rslt9_m128i, 14);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt9_m128i, presult);

    __m128i c19_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c19_align_rslt10_m128i = _mm_alignr_epi8(c19_load_rslt11_m128i, c19_load_rslt10_m128i, 8);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt10_m128i, presult);

    decompress_sse4_c19_m2_s2<1>(c19_load_rslt11_m128i, presult);

    __m128i c19_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c19_align_rslt11_m128i = _mm_alignr_epi8(c19_load_rslt12_m128i, c19_load_rslt11_m128i, 11);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt11_m128i, presult);

    decompress_sse4_c19_m2_s2<4>(c19_load_rslt12_m128i, presult);

    __m128i c19_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c19_align_rslt12_m128i = _mm_alignr_epi8(c19_load_rslt13_m128i, c19_load_rslt12_m128i, 14);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt12_m128i, presult);

    __m128i c19_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c19_align_rslt13_m128i = _mm_alignr_epi8(c19_load_rslt14_m128i, c19_load_rslt13_m128i, 7);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt13_m128i, presult);

    decompress_sse4_c19_m2_s1<1>(c19_load_rslt14_m128i, presult);

    __m128i c19_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c19_align_rslt14_m128i = _mm_alignr_epi8(c19_load_rslt15_m128i, c19_load_rslt14_m128i, 10);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt14_m128i, presult);

    decompress_sse4_c19_m2_s1<4>(c19_load_rslt15_m128i, presult);

    __m128i c19_load_rslt16_m128i = _mm_loadu_si128(its_vector_m128i + 15);
    __m128i c19_align_rslt15_m128i = _mm_alignr_epi8(c19_load_rslt16_m128i, c19_load_rslt15_m128i, 13);
    decompress_sse4_c19_m2_s2<0>(c19_align_rslt15_m128i, presult);

    __m128i c19_load_rslt17_m128i = _mm_loadu_si128(its_vector_m128i + 16);
    __m128i c19_align_rslt16_m128i = _mm_alignr_epi8(c19_load_rslt17_m128i, c19_load_rslt16_m128i, 7);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt16_m128i, presult);

    decompress_sse4_c19_m2_s2<0>(c19_load_rslt17_m128i, presult);

    __m128i c19_load_rslt18_m128i = _mm_loadu_si128(its_vector_m128i + 17);
    __m128i c19_align_rslt17_m128i = _mm_alignr_epi8(c19_load_rslt18_m128i, c19_load_rslt17_m128i, 10);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt17_m128i, presult);

    decompress_sse4_c19_m2_s2<3>(c19_load_rslt18_m128i, presult);

    __m128i c19_load_rslt19_m128i = _mm_loadu_si128(its_vector_m128i + 18);
    __m128i c19_align_rslt18_m128i = _mm_alignr_epi8(c19_load_rslt19_m128i, c19_load_rslt18_m128i, 13);
    decompress_sse4_c19_m2_s1<0>(c19_align_rslt18_m128i, presult);

    decompress_sse4_c19_m2_s2<6>(c19_load_rslt19_m128i, presult);
}

void decompress_sse4_c19(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c19_m1(vector_m128i, presult + index, offset);
        offset += 19;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_19(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c20
const __m128i SSE4_c20_and_msk1_m128i =
    _mm_set_epi8(0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0xFF);

const __m128i SSE4_c20_mul_msk1_m128i = _mm_set_epi32(0x01, 0x10, 0x01, 0x10);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c20_m2(__m128i c20_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c20_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 9, segment + 8, segment + 7, 0xFF, segment + 7, segment + 6, segment + 5, 0xFF,
                     segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c20_shfl_rslt_m128i = _mm_shuffle_epi8(c20_load_vec_m128i, SSE4_c20_shfl_msk_m128i);
    __m128i c20_and_rslt_m128i = _mm_and_si128(c20_shfl_rslt_m128i, SSE4_c20_and_msk1_m128i);
    __m128i c20_mul_rslt_m128i = _mm_mullo_epi32(c20_and_rslt_m128i, SSE4_c20_mul_msk1_m128i);
    __m128i c20_rslt_m128i = _mm_srli_epi32(c20_mul_rslt_m128i, 4);
    _mm_storeu_si128((__m128i*)presult, c20_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c20_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c20_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c20_m2<0>(c20_load_rslt1_m128i, presult);

    __m128i c20_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c20_align_rslt1_m128i = _mm_alignr_epi8(c20_load_rslt2_m128i, c20_load_rslt1_m128i, 10);
    decompress_sse4_c20_m2<0>(c20_align_rslt1_m128i, presult);

    decompress_sse4_c20_m2<4>(c20_load_rslt2_m128i, presult);

    __m128i c20_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c20_align_rslt2_m128i = _mm_alignr_epi8(c20_load_rslt3_m128i, c20_load_rslt2_m128i, 14);
    decompress_sse4_c20_m2<0>(c20_align_rslt2_m128i, presult);

    __m128i c20_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c20_align_rslt3_m128i = _mm_alignr_epi8(c20_load_rslt4_m128i, c20_load_rslt3_m128i, 8);
    decompress_sse4_c20_m2<0>(c20_align_rslt3_m128i, presult);

    decompress_sse4_c20_m2<2>(c20_load_rslt4_m128i, presult);

    __m128i c20_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c20_align_rslt4_m128i = _mm_alignr_epi8(c20_load_rslt5_m128i, c20_load_rslt4_m128i, 12);
    decompress_sse4_c20_m2<0>(c20_align_rslt4_m128i, presult);

    decompress_sse4_c20_m2<6>(c20_load_rslt5_m128i, presult);
}

void decompress_sse4_c20(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 32 <= n) {
        decompress_sse4_c20_m1(vector_m128i, presult + index, offset);
        offset += 5;
        index += 32;
    }
    if (n & 0x1F) {
        unpack_20(dest + index, encode + offset * 4, (n & 0x1F));
    }
}

// c21
const __m128i SSE4_c21_and_msk1_m128i =
    _mm_set_epi8(0x0F, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0xFF, 0xFC, 0x03, 0xFF, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF);

const __m128i SSE4_c21_and_msk2_m128i =
    _mm_set_epi8(0x00, 0xFF, 0xFF, 0xF8, 0x07, 0xFF, 0xFF, 0xC0, 0x00, 0x3F, 0xFF, 0xFE, 0x01, 0xFF, 0xFF, 0xF0);

const __m128i SSE4_c21_mul_msk1_m128i = _mm_set_epi32(0x01, 0x20, 0x04, 0x80);

const __m128i SSE4_c21_mul_msk2_m128i = _mm_set_epi32(0x08, 0x01, 0x20, 0x04);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c21_m2_s1(__m128i c21_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c21_shfl_msk_m128i =
        _mm_set_epi8(segment + 10, segment + 9, segment + 8, segment + 7, 0xFF, segment + 7, segment + 6, segment + 5,
                     segment + 5, segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c21_shfl_rslt_m128i = _mm_shuffle_epi8(c21_load_vec_m128i, SSE4_c21_shfl_msk_m128i);
    __m128i c21_and_rslt_m128i = _mm_and_si128(c21_shfl_rslt_m128i, SSE4_c21_and_msk1_m128i);
    __m128i c21_mul_rslt_m128i = _mm_mullo_epi32(c21_and_rslt_m128i, SSE4_c21_mul_msk1_m128i);
    __m128i c21_rslt_m128i = _mm_srli_epi32(c21_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c21_rslt_m128i);

    presult = presult + 4;
}

template <int segment>
inline void decompress_sse4_c21_m2_s2(__m128i c21_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c21_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 10, segment + 9, segment + 8, segment + 8, segment + 7, segment + 6, segment + 5,
                     0xFF, segment + 5, segment + 4, segment + 3, segment + 3, segment + 2, segment + 1, segment);

    __m128i c21_shfl_rslt_m128i = _mm_shuffle_epi8(c21_load_vec_m128i, SSE4_c21_shfl_msk_m128i);
    __m128i c21_and_rslt_m128i = _mm_and_si128(c21_shfl_rslt_m128i, SSE4_c21_and_msk2_m128i);
    __m128i c21_mul_rslt_m128i = _mm_mullo_epi32(c21_and_rslt_m128i, SSE4_c21_mul_msk2_m128i);
    __m128i c21_rslt_m128i = _mm_srli_epi32(c21_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c21_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c21_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c21_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c21_m2_s1<0>(c21_load_rslt1_m128i, presult);

    __m128i c21_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c21_align_rslt1_m128i = _mm_alignr_epi8(c21_load_rslt2_m128i, c21_load_rslt1_m128i, 10);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt1_m128i, presult);

    decompress_sse4_c21_m2_s1<5>(c21_load_rslt2_m128i, presult);

    __m128i c21_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c21_align_rslt2_m128i = _mm_alignr_epi8(c21_load_rslt3_m128i, c21_load_rslt2_m128i, 15);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt2_m128i, presult);

    __m128i c21_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c21_align_rslt3_m128i = _mm_alignr_epi8(c21_load_rslt4_m128i, c21_load_rslt3_m128i, 10);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt3_m128i, presult);

    decompress_sse4_c21_m2_s2<4>(c21_load_rslt4_m128i, presult);

    __m128i c21_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c21_align_rslt4_m128i = _mm_alignr_epi8(c21_load_rslt5_m128i, c21_load_rslt4_m128i, 15);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt4_m128i, presult);

    __m128i c21_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c21_align_rslt5_m128i = _mm_alignr_epi8(c21_load_rslt6_m128i, c21_load_rslt5_m128i, 9);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt5_m128i, presult);

    decompress_sse4_c21_m2_s1<4>(c21_load_rslt6_m128i, presult);

    __m128i c21_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c21_align_rslt6_m128i = _mm_alignr_epi8(c21_load_rslt7_m128i, c21_load_rslt6_m128i, 14);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt6_m128i, presult);

    __m128i c21_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c21_align_rslt7_m128i = _mm_alignr_epi8(c21_load_rslt8_m128i, c21_load_rslt7_m128i, 9);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt7_m128i, presult);

    decompress_sse4_c21_m2_s2<3>(c21_load_rslt8_m128i, presult);

    __m128i c21_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c21_align_rslt8_m128i = _mm_alignr_epi8(c21_load_rslt9_m128i, c21_load_rslt8_m128i, 14);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt8_m128i, presult);

    __m128i c21_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c21_align_rslt9_m128i = _mm_alignr_epi8(c21_load_rslt10_m128i, c21_load_rslt9_m128i, 8);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt9_m128i, presult);

    decompress_sse4_c21_m2_s1<3>(c21_load_rslt10_m128i, presult);

    __m128i c21_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c21_align_rslt10_m128i = _mm_alignr_epi8(c21_load_rslt11_m128i, c21_load_rslt10_m128i, 13);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt10_m128i, presult);

    __m128i c21_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c21_align_rslt11_m128i = _mm_alignr_epi8(c21_load_rslt12_m128i, c21_load_rslt11_m128i, 8);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt11_m128i, presult);

    decompress_sse4_c21_m2_s2<2>(c21_load_rslt12_m128i, presult);

    __m128i c21_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c21_align_rslt12_m128i = _mm_alignr_epi8(c21_load_rslt13_m128i, c21_load_rslt12_m128i, 13);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt12_m128i, presult);

    __m128i c21_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c21_align_rslt13_m128i = _mm_alignr_epi8(c21_load_rslt14_m128i, c21_load_rslt13_m128i, 7);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt13_m128i, presult);

    decompress_sse4_c21_m2_s1<2>(c21_load_rslt14_m128i, presult);

    __m128i c21_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c21_align_rslt14_m128i = _mm_alignr_epi8(c21_load_rslt15_m128i, c21_load_rslt14_m128i, 12);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt14_m128i, presult);

    __m128i c21_load_rslt16_m128i = _mm_loadu_si128(its_vector_m128i + 15);
    __m128i c21_align_rslt15_m128i = _mm_alignr_epi8(c21_load_rslt16_m128i, c21_load_rslt15_m128i, 7);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt15_m128i, presult);

    decompress_sse4_c21_m2_s2<1>(c21_load_rslt16_m128i, presult);

    __m128i c21_load_rslt17_m128i = _mm_loadu_si128(its_vector_m128i + 16);
    __m128i c21_align_rslt16_m128i = _mm_alignr_epi8(c21_load_rslt17_m128i, c21_load_rslt16_m128i, 12);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt16_m128i, presult);

    __m128i c21_load_rslt18_m128i = _mm_loadu_si128(its_vector_m128i + 17);
    __m128i c21_align_rslt17_m128i = _mm_alignr_epi8(c21_load_rslt18_m128i, c21_load_rslt17_m128i, 6);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt17_m128i, presult);

    decompress_sse4_c21_m2_s1<1>(c21_load_rslt18_m128i, presult);

    __m128i c21_load_rslt19_m128i = _mm_loadu_si128(its_vector_m128i + 18);
    __m128i c21_align_rslt18_m128i = _mm_alignr_epi8(c21_load_rslt19_m128i, c21_load_rslt18_m128i, 11);
    decompress_sse4_c21_m2_s2<0>(c21_align_rslt18_m128i, presult);

    __m128i c21_load_rslt20_m128i = _mm_loadu_si128(its_vector_m128i + 19);
    __m128i c21_align_rslt19_m128i = _mm_alignr_epi8(c21_load_rslt20_m128i, c21_load_rslt19_m128i, 6);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt19_m128i, presult);

    decompress_sse4_c21_m2_s2<0>(c21_load_rslt20_m128i, presult);

    __m128i c21_load_rslt21_m128i = _mm_loadu_si128(its_vector_m128i + 20);
    __m128i c21_align_rslt20_m128i = _mm_alignr_epi8(c21_load_rslt21_m128i, c21_load_rslt20_m128i, 11);
    decompress_sse4_c21_m2_s1<0>(c21_align_rslt20_m128i, presult);

    decompress_sse4_c21_m2_s2<5>(c21_load_rslt21_m128i, presult);
}

void decompress_sse4_c21(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c21_m1(vector_m128i, presult + index, offset);
        offset += 21;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_21(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c22
const __m128i SSE4_c22_and_msk1_m128i =
    _mm_set_epi8(0x00, 0xFF, 0xFF, 0xFC, 0x03, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xC0, 0x00, 0x3F, 0xFF, 0xFF);

const __m128i SSE4_c22_mul_msk1_m128i = _mm_set_epi32(0x10, 0x04, 0x01, 0x40);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c22_m2(__m128i c22_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c22_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 10, segment + 9, segment + 8, segment + 8, segment + 7, segment + 6, segment + 5,
                     segment + 5, segment + 4, segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c22_shfl_rslt_m128i = _mm_shuffle_epi8(c22_load_vec_m128i, SSE4_c22_shfl_msk_m128i);
    __m128i c22_and_rslt_m128i = _mm_and_si128(c22_shfl_rslt_m128i, SSE4_c22_and_msk1_m128i);
    __m128i c22_mul_rslt_m128i = _mm_mullo_epi32(c22_and_rslt_m128i, SSE4_c22_mul_msk1_m128i);
    __m128i c22_rslt_m128i = _mm_srli_epi32(c22_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c22_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c22_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c22_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c22_m2<0>(c22_load_rslt1_m128i, presult);

    __m128i c22_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c22_align_rslt1_m128i = _mm_alignr_epi8(c22_load_rslt2_m128i, c22_load_rslt1_m128i, 11);
    decompress_sse4_c22_m2<0>(c22_align_rslt1_m128i, presult);

    __m128i c22_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c22_align_rslt2_m128i = _mm_alignr_epi8(c22_load_rslt3_m128i, c22_load_rslt2_m128i, 6);
    decompress_sse4_c22_m2<0>(c22_align_rslt2_m128i, presult);

    decompress_sse4_c22_m2<1>(c22_load_rslt3_m128i, presult);

    __m128i c22_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c22_align_rslt3_m128i = _mm_alignr_epi8(c22_load_rslt4_m128i, c22_load_rslt3_m128i, 12);
    decompress_sse4_c22_m2<0>(c22_align_rslt3_m128i, presult);

    __m128i c22_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c22_align_rslt4_m128i = _mm_alignr_epi8(c22_load_rslt5_m128i, c22_load_rslt4_m128i, 7);
    decompress_sse4_c22_m2<0>(c22_align_rslt4_m128i, presult);

    decompress_sse4_c22_m2<2>(c22_load_rslt5_m128i, presult);

    __m128i c22_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c22_align_rslt5_m128i = _mm_alignr_epi8(c22_load_rslt6_m128i, c22_load_rslt5_m128i, 13);
    decompress_sse4_c22_m2<0>(c22_align_rslt5_m128i, presult);

    __m128i c22_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c22_align_rslt6_m128i = _mm_alignr_epi8(c22_load_rslt7_m128i, c22_load_rslt6_m128i, 8);
    decompress_sse4_c22_m2<0>(c22_align_rslt6_m128i, presult);

    decompress_sse4_c22_m2<3>(c22_load_rslt7_m128i, presult);

    __m128i c22_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c22_align_rslt7_m128i = _mm_alignr_epi8(c22_load_rslt8_m128i, c22_load_rslt7_m128i, 14);
    decompress_sse4_c22_m2<0>(c22_align_rslt7_m128i, presult);

    __m128i c22_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c22_align_rslt8_m128i = _mm_alignr_epi8(c22_load_rslt9_m128i, c22_load_rslt8_m128i, 9);
    decompress_sse4_c22_m2<0>(c22_align_rslt8_m128i, presult);

    decompress_sse4_c22_m2<4>(c22_load_rslt9_m128i, presult);

    __m128i c22_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c22_align_rslt9_m128i = _mm_alignr_epi8(c22_load_rslt10_m128i, c22_load_rslt9_m128i, 15);
    decompress_sse4_c22_m2<0>(c22_align_rslt9_m128i, presult);

    __m128i c22_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c22_align_rslt10_m128i = _mm_alignr_epi8(c22_load_rslt11_m128i, c22_load_rslt10_m128i, 10);
    decompress_sse4_c22_m2<0>(c22_align_rslt10_m128i, presult);

    decompress_sse4_c22_m2<5>(c22_load_rslt11_m128i, presult);
}

void decompress_sse4_c22(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c22_m1(vector_m128i, presult + index, offset);
        offset += 11;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_22(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c23
const __m128i SSE4_c23_and_msk1_m128i =
    _mm_set_epi8(0x0F, 0xFF, 0xFF, 0xE0, 0x1F, 0xFF, 0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0xFF, 0xFF);

const __m128i SSE4_c23_and_msk2_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFE, 0x01, 0xFF, 0xFF, 0xFC, 0x03, 0xFF, 0xFF, 0xF8, 0x07, 0xFF, 0xFF, 0xF0);

const __m128i SSE4_c23_mul_msk1_m128i = _mm_set_epi32(0x04, 0x02, 0x01, 0x80);

const __m128i SSE4_c23_mul_msk2_m128i = _mm_set_epi32(0x08, 0x04, 0x02, 0x01);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c23_m2_s1(__m128i c23_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c23_shfl_msk_m128i = _mm_set_epi8(segment + 11, segment + 10, segment + 9, segment + 8, segment + 8,
                                                   segment + 7, segment + 6, segment + 5, segment + 5, segment + 4,
                                                   segment + 3, segment + 2, 0xFF, segment + 2, segment + 1, segment);

    __m128i c23_shfl_rslt_m128i = _mm_shuffle_epi8(c23_load_vec_m128i, SSE4_c23_shfl_msk_m128i);
    __m128i c23_and_rslt_m128i = _mm_and_si128(c23_shfl_rslt_m128i, SSE4_c23_and_msk1_m128i);
    __m128i c23_mul_rslt_m128i = _mm_mullo_epi32(c23_and_rslt_m128i, SSE4_c23_mul_msk1_m128i);
    __m128i c23_rslt_m128i = _mm_srli_epi32(c23_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c23_rslt_m128i);

    presult = presult + 4;
}

template <int segment>
inline void decompress_sse4_c23_m2_s2(__m128i c23_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c23_shfl_msk_m128i = _mm_set_epi8(
        0xFF, segment + 11, segment + 10, segment + 9, segment + 9, segment + 8, segment + 7, segment + 6, segment + 6,
        segment + 5, segment + 4, segment + 3, segment + 3, segment + 2, segment + 1, segment);

    __m128i c23_shfl_rslt_m128i = _mm_shuffle_epi8(c23_load_vec_m128i, SSE4_c23_shfl_msk_m128i);
    __m128i c23_and_rslt_m128i = _mm_and_si128(c23_shfl_rslt_m128i, SSE4_c23_and_msk2_m128i);
    __m128i c23_mul_rslt_m128i = _mm_mullo_epi32(c23_and_rslt_m128i, SSE4_c23_mul_msk2_m128i);
    __m128i c23_rslt_m128i = _mm_srli_epi32(c23_mul_rslt_m128i, 4);
    _mm_storeu_si128((__m128i*)presult, c23_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c23_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c23_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c23_m2_s1<0>(c23_load_rslt1_m128i, presult);

    __m128i c23_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c23_align_rslt1_m128i = _mm_alignr_epi8(c23_load_rslt2_m128i, c23_load_rslt1_m128i, 11);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt1_m128i, presult);

    __m128i c23_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c23_align_rslt2_m128i = _mm_alignr_epi8(c23_load_rslt3_m128i, c23_load_rslt2_m128i, 7);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt2_m128i, presult);

    decompress_sse4_c23_m2_s2<2>(c23_load_rslt3_m128i, presult);

    __m128i c23_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c23_align_rslt3_m128i = _mm_alignr_epi8(c23_load_rslt4_m128i, c23_load_rslt3_m128i, 14);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt3_m128i, presult);

    __m128i c23_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c23_align_rslt4_m128i = _mm_alignr_epi8(c23_load_rslt5_m128i, c23_load_rslt4_m128i, 9);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt4_m128i, presult);

    __m128i c23_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c23_align_rslt5_m128i = _mm_alignr_epi8(c23_load_rslt6_m128i, c23_load_rslt5_m128i, 5);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt5_m128i, presult);

    decompress_sse4_c23_m2_s2<0>(c23_load_rslt6_m128i, presult);

    __m128i c23_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c23_align_rslt6_m128i = _mm_alignr_epi8(c23_load_rslt7_m128i, c23_load_rslt6_m128i, 12);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt6_m128i, presult);

    __m128i c23_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c23_align_rslt7_m128i = _mm_alignr_epi8(c23_load_rslt8_m128i, c23_load_rslt7_m128i, 7);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt7_m128i, presult);

    decompress_sse4_c23_m2_s1<3>(c23_load_rslt8_m128i, presult);

    __m128i c23_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c23_align_rslt8_m128i = _mm_alignr_epi8(c23_load_rslt9_m128i, c23_load_rslt8_m128i, 14);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt8_m128i, presult);

    __m128i c23_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c23_align_rslt9_m128i = _mm_alignr_epi8(c23_load_rslt10_m128i, c23_load_rslt9_m128i, 10);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt9_m128i, presult);

    __m128i c23_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c23_align_rslt10_m128i = _mm_alignr_epi8(c23_load_rslt11_m128i, c23_load_rslt10_m128i, 5);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt10_m128i, presult);

    decompress_sse4_c23_m2_s1<1>(c23_load_rslt11_m128i, presult);

    __m128i c23_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c23_align_rslt11_m128i = _mm_alignr_epi8(c23_load_rslt12_m128i, c23_load_rslt11_m128i, 12);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt11_m128i, presult);

    __m128i c23_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c23_align_rslt12_m128i = _mm_alignr_epi8(c23_load_rslt13_m128i, c23_load_rslt12_m128i, 8);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt12_m128i, presult);

    decompress_sse4_c23_m2_s2<3>(c23_load_rslt13_m128i, presult);

    __m128i c23_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c23_align_rslt13_m128i = _mm_alignr_epi8(c23_load_rslt14_m128i, c23_load_rslt13_m128i, 15);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt13_m128i, presult);

    __m128i c23_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c23_align_rslt14_m128i = _mm_alignr_epi8(c23_load_rslt15_m128i, c23_load_rslt14_m128i, 10);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt14_m128i, presult);

    __m128i c23_load_rslt16_m128i = _mm_loadu_si128(its_vector_m128i + 15);
    __m128i c23_align_rslt15_m128i = _mm_alignr_epi8(c23_load_rslt16_m128i, c23_load_rslt15_m128i, 6);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt15_m128i, presult);

    decompress_sse4_c23_m2_s2<1>(c23_load_rslt16_m128i, presult);

    __m128i c23_load_rslt17_m128i = _mm_loadu_si128(its_vector_m128i + 16);
    __m128i c23_align_rslt16_m128i = _mm_alignr_epi8(c23_load_rslt17_m128i, c23_load_rslt16_m128i, 13);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt16_m128i, presult);

    __m128i c23_load_rslt18_m128i = _mm_loadu_si128(its_vector_m128i + 17);
    __m128i c23_align_rslt17_m128i = _mm_alignr_epi8(c23_load_rslt18_m128i, c23_load_rslt17_m128i, 8);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt17_m128i, presult);

    decompress_sse4_c23_m2_s1<4>(c23_load_rslt18_m128i, presult);

    __m128i c23_load_rslt19_m128i = _mm_loadu_si128(its_vector_m128i + 18);
    __m128i c23_align_rslt18_m128i = _mm_alignr_epi8(c23_load_rslt19_m128i, c23_load_rslt18_m128i, 15);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt18_m128i, presult);

    __m128i c23_load_rslt20_m128i = _mm_loadu_si128(its_vector_m128i + 19);
    __m128i c23_align_rslt19_m128i = _mm_alignr_epi8(c23_load_rslt20_m128i, c23_load_rslt19_m128i, 11);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt19_m128i, presult);

    __m128i c23_load_rslt21_m128i = _mm_loadu_si128(its_vector_m128i + 20);
    __m128i c23_align_rslt20_m128i = _mm_alignr_epi8(c23_load_rslt21_m128i, c23_load_rslt20_m128i, 6);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt20_m128i, presult);

    decompress_sse4_c23_m2_s1<2>(c23_load_rslt21_m128i, presult);

    __m128i c23_load_rslt22_m128i = _mm_loadu_si128(its_vector_m128i + 21);
    __m128i c23_align_rslt21_m128i = _mm_alignr_epi8(c23_load_rslt22_m128i, c23_load_rslt21_m128i, 13);
    decompress_sse4_c23_m2_s2<0>(c23_align_rslt21_m128i, presult);

    __m128i c23_load_rslt23_m128i = _mm_loadu_si128(its_vector_m128i + 22);
    __m128i c23_align_rslt22_m128i = _mm_alignr_epi8(c23_load_rslt23_m128i, c23_load_rslt22_m128i, 9);
    decompress_sse4_c23_m2_s1<0>(c23_align_rslt22_m128i, presult);

    decompress_sse4_c23_m2_s2<4>(c23_load_rslt23_m128i, presult);
}

void decompress_sse4_c23(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c23_m1(vector_m128i, presult + index, offset);
        offset += 23;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_23(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c24
template <int segment>
inline void decompress_sse4_c24_m2(__m128i c24_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c24_shfl_msk_m128i =
        _mm_set_epi8(0xFF, segment + 11, segment + 10, segment + 9, 0xFF, segment + 8, segment + 7, segment + 6, 0xFF,
                     segment + 5, segment + 4, segment + 3, 0xFF, segment + 2, segment + 1, segment);

    __m128i c24_rslt_m128i = _mm_shuffle_epi8(c24_load_vec_m128i, SSE4_c24_shfl_msk_m128i);
    _mm_storeu_si128((__m128i*)presult, c24_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c24_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c24_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c24_m2<0>(c24_load_rslt1_m128i, presult);

    __m128i c24_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c24_align_rslt1_m128i = _mm_alignr_epi8(c24_load_rslt2_m128i, c24_load_rslt1_m128i, 12);
    decompress_sse4_c24_m2<0>(c24_align_rslt1_m128i, presult);

    __m128i c24_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c24_align_rslt2_m128i = _mm_alignr_epi8(c24_load_rslt3_m128i, c24_load_rslt2_m128i, 8);
    decompress_sse4_c24_m2<0>(c24_align_rslt2_m128i, presult);

    decompress_sse4_c24_m2<4>(c24_load_rslt3_m128i, presult);
}

void decompress_sse4_c24(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 16 <= n) {
        decompress_sse4_c24_m1(vector_m128i, presult + index, offset);
        offset += 3;
        index += 16;
    }
    if (n & 0xF) {
        unpack_24(dest + index, encode + offset * 4, (n & 0xF));
    }
}

// c25
const __m128i SSE4_c25_and_msk1_m128i =
    _mm_set_epi8(0x0F, 0xFF, 0xFF, 0xF8, 0x07, 0xFF, 0xFF, 0xFC, 0x03, 0xFF, 0xFF, 0xFE, 0x01, 0xFF, 0xFF, 0xFF);

const __m128i SSE4_c25_and_msk2_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0x80, 0x7F, 0xFF, 0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0xE0, 0x1F, 0xFF, 0xFF, 0xF0);

const __m128i SSE4_c25_mul_msk1_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

const __m128i SSE4_c25_mul_msk2_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c25_m2_s1(__m128i c25_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c25_shfl_msk_m128i = _mm_set_epi8(
        segment + 12, segment + 11, segment + 10, segment + 9, segment + 9, segment + 8, segment + 7, segment + 6,
        segment + 6, segment + 5, segment + 4, segment + 3, segment + 3, segment + 2, segment + 1, segment);

    __m128i c25_shfl_rslt_m128i = _mm_shuffle_epi8(c25_load_vec_m128i, SSE4_c25_shfl_msk_m128i);
    __m128i c25_and_rslt_m128i = _mm_and_si128(c25_shfl_rslt_m128i, SSE4_c25_and_msk1_m128i);
    __m128i c25_mul_rslt_m128i = _mm_mullo_epi32(c25_and_rslt_m128i, SSE4_c25_mul_msk1_m128i);
    __m128i c25_rslt_m128i = _mm_srli_epi32(c25_mul_rslt_m128i, 3);
    _mm_storeu_si128((__m128i*)presult, c25_rslt_m128i);

    presult = presult + 4;
}

template <int segment>
inline void decompress_sse4_c25_m2_s2(__m128i c25_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c25_shfl_msk_m128i = _mm_set_epi8(
        segment + 12, segment + 11, segment + 10, segment + 9, segment + 9, segment + 8, segment + 7, segment + 6,
        segment + 6, segment + 5, segment + 4, segment + 3, segment + 3, segment + 2, segment + 1, segment);

    __m128i c25_shfl_rslt_m128i = _mm_shuffle_epi8(c25_load_vec_m128i, SSE4_c25_shfl_msk_m128i);
    __m128i c25_and_rslt_m128i = _mm_and_si128(c25_shfl_rslt_m128i, SSE4_c25_and_msk2_m128i);
    __m128i c25_mul_rslt_m128i = _mm_mullo_epi32(c25_and_rslt_m128i, SSE4_c25_mul_msk2_m128i);
    __m128i c25_rslt_m128i = _mm_srli_epi32(c25_mul_rslt_m128i, 7);
    _mm_storeu_si128((__m128i*)presult, c25_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c25_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c25_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c25_m2_s1<0>(c25_load_rslt1_m128i, presult);

    __m128i c25_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c25_align_rslt1_m128i = _mm_alignr_epi8(c25_load_rslt2_m128i, c25_load_rslt1_m128i, 12);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt1_m128i, presult);

    __m128i c25_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c25_align_rslt2_m128i = _mm_alignr_epi8(c25_load_rslt3_m128i, c25_load_rslt2_m128i, 9);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt2_m128i, presult);

    __m128i c25_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c25_align_rslt3_m128i = _mm_alignr_epi8(c25_load_rslt4_m128i, c25_load_rslt3_m128i, 5);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt3_m128i, presult);

    decompress_sse4_c25_m2_s1<2>(c25_load_rslt4_m128i, presult);

    __m128i c25_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c25_align_rslt4_m128i = _mm_alignr_epi8(c25_load_rslt5_m128i, c25_load_rslt4_m128i, 14);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt4_m128i, presult);

    __m128i c25_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c25_align_rslt5_m128i = _mm_alignr_epi8(c25_load_rslt6_m128i, c25_load_rslt5_m128i, 11);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt5_m128i, presult);

    __m128i c25_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c25_align_rslt6_m128i = _mm_alignr_epi8(c25_load_rslt7_m128i, c25_load_rslt6_m128i, 7);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt6_m128i, presult);

    __m128i c25_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c25_align_rslt7_m128i = _mm_alignr_epi8(c25_load_rslt8_m128i, c25_load_rslt7_m128i, 4);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt7_m128i, presult);

    decompress_sse4_c25_m2_s2<0>(c25_load_rslt8_m128i, presult);

    __m128i c25_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c25_align_rslt8_m128i = _mm_alignr_epi8(c25_load_rslt9_m128i, c25_load_rslt8_m128i, 13);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt8_m128i, presult);

    __m128i c25_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c25_align_rslt9_m128i = _mm_alignr_epi8(c25_load_rslt10_m128i, c25_load_rslt9_m128i, 9);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt9_m128i, presult);

    __m128i c25_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c25_align_rslt10_m128i = _mm_alignr_epi8(c25_load_rslt11_m128i, c25_load_rslt10_m128i, 6);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt10_m128i, presult);

    decompress_sse4_c25_m2_s2<2>(c25_load_rslt11_m128i, presult);

    __m128i c25_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c25_align_rslt11_m128i = _mm_alignr_epi8(c25_load_rslt12_m128i, c25_load_rslt11_m128i, 15);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt11_m128i, presult);

    __m128i c25_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c25_align_rslt12_m128i = _mm_alignr_epi8(c25_load_rslt13_m128i, c25_load_rslt12_m128i, 11);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt12_m128i, presult);

    __m128i c25_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c25_align_rslt13_m128i = _mm_alignr_epi8(c25_load_rslt14_m128i, c25_load_rslt13_m128i, 8);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt13_m128i, presult);

    __m128i c25_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c25_align_rslt14_m128i = _mm_alignr_epi8(c25_load_rslt15_m128i, c25_load_rslt14_m128i, 4);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt14_m128i, presult);

    decompress_sse4_c25_m2_s1<1>(c25_load_rslt15_m128i, presult);

    __m128i c25_load_rslt16_m128i = _mm_loadu_si128(its_vector_m128i + 15);
    __m128i c25_align_rslt15_m128i = _mm_alignr_epi8(c25_load_rslt16_m128i, c25_load_rslt15_m128i, 13);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt15_m128i, presult);

    __m128i c25_load_rslt17_m128i = _mm_loadu_si128(its_vector_m128i + 16);
    __m128i c25_align_rslt16_m128i = _mm_alignr_epi8(c25_load_rslt17_m128i, c25_load_rslt16_m128i, 10);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt16_m128i, presult);

    __m128i c25_load_rslt18_m128i = _mm_loadu_si128(its_vector_m128i + 17);
    __m128i c25_align_rslt17_m128i = _mm_alignr_epi8(c25_load_rslt18_m128i, c25_load_rslt17_m128i, 6);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt17_m128i, presult);

    decompress_sse4_c25_m2_s1<3>(c25_load_rslt18_m128i, presult);

    __m128i c25_load_rslt19_m128i = _mm_loadu_si128(its_vector_m128i + 18);
    __m128i c25_align_rslt18_m128i = _mm_alignr_epi8(c25_load_rslt19_m128i, c25_load_rslt18_m128i, 15);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt18_m128i, presult);

    __m128i c25_load_rslt20_m128i = _mm_loadu_si128(its_vector_m128i + 19);
    __m128i c25_align_rslt19_m128i = _mm_alignr_epi8(c25_load_rslt20_m128i, c25_load_rslt19_m128i, 12);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt19_m128i, presult);

    __m128i c25_load_rslt21_m128i = _mm_loadu_si128(its_vector_m128i + 20);
    __m128i c25_align_rslt20_m128i = _mm_alignr_epi8(c25_load_rslt21_m128i, c25_load_rslt20_m128i, 8);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt20_m128i, presult);

    __m128i c25_load_rslt22_m128i = _mm_loadu_si128(its_vector_m128i + 21);
    __m128i c25_align_rslt21_m128i = _mm_alignr_epi8(c25_load_rslt22_m128i, c25_load_rslt21_m128i, 5);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt21_m128i, presult);

    decompress_sse4_c25_m2_s2<1>(c25_load_rslt22_m128i, presult);

    __m128i c25_load_rslt23_m128i = _mm_loadu_si128(its_vector_m128i + 22);
    __m128i c25_align_rslt22_m128i = _mm_alignr_epi8(c25_load_rslt23_m128i, c25_load_rslt22_m128i, 14);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt22_m128i, presult);

    __m128i c25_load_rslt24_m128i = _mm_loadu_si128(its_vector_m128i + 23);
    __m128i c25_align_rslt23_m128i = _mm_alignr_epi8(c25_load_rslt24_m128i, c25_load_rslt23_m128i, 10);
    decompress_sse4_c25_m2_s2<0>(c25_align_rslt23_m128i, presult);

    __m128i c25_load_rslt25_m128i = _mm_loadu_si128(its_vector_m128i + 24);
    __m128i c25_align_rslt24_m128i = _mm_alignr_epi8(c25_load_rslt25_m128i, c25_load_rslt24_m128i, 7);
    decompress_sse4_c25_m2_s1<0>(c25_align_rslt24_m128i, presult);

    decompress_sse4_c25_m2_s2<3>(c25_load_rslt25_m128i, presult);
}

void decompress_sse4_c25(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c25_m1(vector_m128i, presult + index, offset);
        offset += 25;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_25(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c26
const __m128i SSE4_c26_and_msk1_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xFC, 0x03, 0xFF, 0xFF, 0xFF);

const __m128i SSE4_c26_mul_msk1_m128i = _mm_set_epi32(0x01, 0x04, 0x10, 0x40);

template <int segment>
inline void decompress_sse4_c26_m2(__m128i c26_load_vec_m128i, unsigned int*& presult)
{
    // decompress first 4 values
    __m128i SSE4_c26_shfl_msk_m128i = _mm_set_epi8(
        segment + 12, segment + 11, segment + 10, segment + 9, segment + 9, segment + 8, segment + 7, segment + 6,
        segment + 6, segment + 5, segment + 4, segment + 3, segment + 3, segment + 2, segment + 1, segment);

    __m128i c26_shfl_rslt_m128i = _mm_shuffle_epi8(c26_load_vec_m128i, SSE4_c26_shfl_msk_m128i);
    __m128i c26_and_rslt_m128i = _mm_and_si128(c26_shfl_rslt_m128i, SSE4_c26_and_msk1_m128i);
    __m128i c26_mul_rslt_m128i = _mm_mullo_epi32(c26_and_rslt_m128i, SSE4_c26_mul_msk1_m128i);
    __m128i c26_rslt_m128i = _mm_srli_epi32(c26_mul_rslt_m128i, 6);
    _mm_storeu_si128((__m128i*)presult, c26_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c26_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c26_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c26_m2<0>(c26_load_rslt1_m128i, presult);

    __m128i c26_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c26_align_rslt1_m128i = _mm_alignr_epi8(c26_load_rslt2_m128i, c26_load_rslt1_m128i, 13);
    decompress_sse4_c26_m2<0>(c26_align_rslt1_m128i, presult);

    __m128i c26_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c26_align_rslt2_m128i = _mm_alignr_epi8(c26_load_rslt3_m128i, c26_load_rslt2_m128i, 10);
    decompress_sse4_c26_m2<0>(c26_align_rslt2_m128i, presult);

    __m128i c26_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c26_align_rslt3_m128i = _mm_alignr_epi8(c26_load_rslt4_m128i, c26_load_rslt3_m128i, 7);
    decompress_sse4_c26_m2<0>(c26_align_rslt3_m128i, presult);

    __m128i c26_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c26_align_rslt4_m128i = _mm_alignr_epi8(c26_load_rslt5_m128i, c26_load_rslt4_m128i, 4);
    decompress_sse4_c26_m2<0>(c26_align_rslt4_m128i, presult);

    decompress_sse4_c26_m2<1>(c26_load_rslt5_m128i, presult);

    __m128i c26_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c26_align_rslt5_m128i = _mm_alignr_epi8(c26_load_rslt6_m128i, c26_load_rslt5_m128i, 14);
    decompress_sse4_c26_m2<0>(c26_align_rslt5_m128i, presult);

    __m128i c26_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c26_align_rslt6_m128i = _mm_alignr_epi8(c26_load_rslt7_m128i, c26_load_rslt6_m128i, 11);
    decompress_sse4_c26_m2<0>(c26_align_rslt6_m128i, presult);

    __m128i c26_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c26_align_rslt7_m128i = _mm_alignr_epi8(c26_load_rslt8_m128i, c26_load_rslt7_m128i, 8);
    decompress_sse4_c26_m2<0>(c26_align_rslt7_m128i, presult);

    __m128i c26_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c26_align_rslt8_m128i = _mm_alignr_epi8(c26_load_rslt9_m128i, c26_load_rslt8_m128i, 5);
    decompress_sse4_c26_m2<0>(c26_align_rslt8_m128i, presult);

    decompress_sse4_c26_m2<2>(c26_load_rslt9_m128i, presult);

    __m128i c26_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c26_align_rslt9_m128i = _mm_alignr_epi8(c26_load_rslt10_m128i, c26_load_rslt9_m128i, 15);
    decompress_sse4_c26_m2<0>(c26_align_rslt9_m128i, presult);

    __m128i c26_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c26_align_rslt10_m128i = _mm_alignr_epi8(c26_load_rslt11_m128i, c26_load_rslt10_m128i, 12);
    decompress_sse4_c26_m2<0>(c26_align_rslt10_m128i, presult);

    __m128i c26_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c26_align_rslt11_m128i = _mm_alignr_epi8(c26_load_rslt12_m128i, c26_load_rslt11_m128i, 9);
    decompress_sse4_c26_m2<0>(c26_align_rslt11_m128i, presult);

    __m128i c26_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c26_align_rslt12_m128i = _mm_alignr_epi8(c26_load_rslt13_m128i, c26_load_rslt12_m128i, 6);
    decompress_sse4_c26_m2<0>(c26_align_rslt12_m128i, presult);

    decompress_sse4_c26_m2<3>(c26_load_rslt13_m128i, presult);
}

void decompress_sse4_c26(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c26_m1(vector_m128i, presult + index, offset);
        offset += 13;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_26(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c27
const __m128i SSE4_c27_and_msk1_m128i =
    _mm_set_epi8(0x0F, 0xFF, 0xFF, 0xFE, 0x07, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xF8, 0x07, 0xFF, 0xFF, 0xFF);

const __m128i SSE4_c27_and_msk2_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xE0, 0x1F, 0xFF, 0xFF, 0xFC, 0x07, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xF0);

const __m128i SSE4_c27_mul_msk1_m128i = _mm_set_epi32(0x04, 0x08, 0x01, 0x08);

const __m128i SSE4_c27_mul_msk2_m128i = _mm_set_epi32(0x01, 0x08, 0x20, 0x02);

template <int segment>
inline void decompress_sse4_c27_m2_s1(__m128i c27_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c27_shfl_msk_m128i = _mm_set_epi8(
        segment + 13, segment + 12, segment + 11, segment + 10, segment + 9, segment + 8, segment + 7, segment + 6,
        segment + 6, segment + 5, segment + 4, segment + 3, segment + 3, segment + 2, segment + 1, segment);

    __m128i c27_shfl_rslt_m128i = _mm_shuffle_epi8(c27_load_vec_m128i, SSE4_c27_shfl_msk_m128i);
    __m128i c27_shift_rslt_m128i = _mm_srli_epi64(c27_shfl_rslt_m128i, 6);

    __m128i c27_blend_rslt_m128i = _mm_blend_epi16(c27_shfl_rslt_m128i, c27_shift_rslt_m128i, 0x30);

    __m128i c27_and_rslt_m128i = _mm_and_si128(c27_blend_rslt_m128i, SSE4_c27_and_msk1_m128i);
    __m128i c27_mul_rslt_m128i = _mm_mullo_epi32(c27_and_rslt_m128i, SSE4_c27_mul_msk1_m128i);
    __m128i c27_rslt_m128i = _mm_srli_epi32(c27_mul_rslt_m128i, 3);
    _mm_storeu_si128((__m128i*)presult, c27_rslt_m128i);

    presult = presult + 4;
}

template <int segment>
inline void decompress_sse4_c27_m2_s2(__m128i c27_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c27_shfl_msk_m128i = _mm_set_epi8(
        segment + 13, segment + 12, segment + 11, segment + 10, segment + 10, segment + 9, segment + 8, segment + 7,
        segment + 7, segment + 6, segment + 5, segment + 4, segment + 3, segment + 2, segment + 1, segment);

    __m128i c27_shfl_rslt_m128i = _mm_shuffle_epi8(c27_load_vec_m128i, SSE4_c27_shfl_msk_m128i);
    __m128i c27_shift_rslt_m128i = _mm_slli_epi64(c27_shfl_rslt_m128i, 1);

    __m128i c27_blend_rslt_m128i = _mm_blend_epi16(c27_shfl_rslt_m128i, c27_shift_rslt_m128i, 0x0C);

    __m128i c27_and_rslt_m128i = _mm_and_si128(c27_blend_rslt_m128i, SSE4_c27_and_msk2_m128i);
    __m128i c27_mul_rslt_m128i = _mm_mullo_epi32(c27_and_rslt_m128i, SSE4_c27_mul_msk2_m128i);
    __m128i c27_rslt_m128i = _mm_srli_epi32(c27_mul_rslt_m128i, 5);
    _mm_storeu_si128((__m128i*)presult, c27_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c27_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c27_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c27_m2_s1<0>(c27_load_rslt1_m128i, presult);

    __m128i c27_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c27_align_rslt1_m128i = _mm_alignr_epi8(c27_load_rslt2_m128i, c27_load_rslt1_m128i, 13);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt1_m128i, presult);

    __m128i c27_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c27_align_rslt2_m128i = _mm_alignr_epi8(c27_load_rslt3_m128i, c27_load_rslt2_m128i, 11);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt2_m128i, presult);

    __m128i c27_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c27_align_rslt3_m128i = _mm_alignr_epi8(c27_load_rslt4_m128i, c27_load_rslt3_m128i, 8);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt3_m128i, presult);

    __m128i c27_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c27_align_rslt4_m128i = _mm_alignr_epi8(c27_load_rslt5_m128i, c27_load_rslt4_m128i, 6);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt4_m128i, presult);

    __m128i c27_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c27_align_rslt5_m128i = _mm_alignr_epi8(c27_load_rslt6_m128i, c27_load_rslt5_m128i, 3);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt5_m128i, presult);

    decompress_sse4_c27_m2_s1<1>(c27_load_rslt6_m128i, presult);

    __m128i c27_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c27_align_rslt6_m128i = _mm_alignr_epi8(c27_load_rslt7_m128i, c27_load_rslt6_m128i, 14);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt6_m128i, presult);

    __m128i c27_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c27_align_rslt7_m128i = _mm_alignr_epi8(c27_load_rslt8_m128i, c27_load_rslt7_m128i, 12);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt7_m128i, presult);

    __m128i c27_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c27_align_rslt8_m128i = _mm_alignr_epi8(c27_load_rslt9_m128i, c27_load_rslt8_m128i, 9);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt8_m128i, presult);

    __m128i c27_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c27_align_rslt9_m128i = _mm_alignr_epi8(c27_load_rslt10_m128i, c27_load_rslt9_m128i, 7);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt9_m128i, presult);

    __m128i c27_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c27_align_rslt10_m128i = _mm_alignr_epi8(c27_load_rslt11_m128i, c27_load_rslt10_m128i, 4);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt10_m128i, presult);

    decompress_sse4_c27_m2_s1<2>(c27_load_rslt11_m128i, presult);

    __m128i c27_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c27_align_rslt11_m128i = _mm_alignr_epi8(c27_load_rslt12_m128i, c27_load_rslt11_m128i, 15);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt11_m128i, presult);

    __m128i c27_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c27_align_rslt12_m128i = _mm_alignr_epi8(c27_load_rslt13_m128i, c27_load_rslt12_m128i, 13);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt12_m128i, presult);

    __m128i c27_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c27_align_rslt13_m128i = _mm_alignr_epi8(c27_load_rslt14_m128i, c27_load_rslt13_m128i, 10);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt13_m128i, presult);

    __m128i c27_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c27_align_rslt14_m128i = _mm_alignr_epi8(c27_load_rslt15_m128i, c27_load_rslt14_m128i, 8);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt14_m128i, presult);

    __m128i c27_load_rslt16_m128i = _mm_loadu_si128(its_vector_m128i + 15);
    __m128i c27_align_rslt15_m128i = _mm_alignr_epi8(c27_load_rslt16_m128i, c27_load_rslt15_m128i, 5);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt15_m128i, presult);

    __m128i c27_load_rslt17_m128i = _mm_loadu_si128(its_vector_m128i + 16);
    __m128i c27_align_rslt16_m128i = _mm_alignr_epi8(c27_load_rslt17_m128i, c27_load_rslt16_m128i, 3);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt16_m128i, presult);

    decompress_sse4_c27_m2_s2<0>(c27_load_rslt17_m128i, presult);

    __m128i c27_load_rslt18_m128i = _mm_loadu_si128(its_vector_m128i + 17);
    __m128i c27_align_rslt17_m128i = _mm_alignr_epi8(c27_load_rslt18_m128i, c27_load_rslt17_m128i, 14);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt17_m128i, presult);

    __m128i c27_load_rslt19_m128i = _mm_loadu_si128(its_vector_m128i + 18);
    __m128i c27_align_rslt18_m128i = _mm_alignr_epi8(c27_load_rslt19_m128i, c27_load_rslt18_m128i, 11);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt18_m128i, presult);

    __m128i c27_load_rslt20_m128i = _mm_loadu_si128(its_vector_m128i + 19);
    __m128i c27_align_rslt19_m128i = _mm_alignr_epi8(c27_load_rslt20_m128i, c27_load_rslt19_m128i, 9);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt19_m128i, presult);

    __m128i c27_load_rslt21_m128i = _mm_loadu_si128(its_vector_m128i + 20);
    __m128i c27_align_rslt20_m128i = _mm_alignr_epi8(c27_load_rslt21_m128i, c27_load_rslt20_m128i, 6);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt20_m128i, presult);

    __m128i c27_load_rslt22_m128i = _mm_loadu_si128(its_vector_m128i + 21);
    __m128i c27_align_rslt21_m128i = _mm_alignr_epi8(c27_load_rslt22_m128i, c27_load_rslt21_m128i, 4);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt21_m128i, presult);

    decompress_sse4_c27_m2_s2<1>(c27_load_rslt22_m128i, presult);

    __m128i c27_load_rslt23_m128i = _mm_loadu_si128(its_vector_m128i + 22);
    __m128i c27_align_rslt22_m128i = _mm_alignr_epi8(c27_load_rslt23_m128i, c27_load_rslt22_m128i, 15);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt22_m128i, presult);

    __m128i c27_load_rslt24_m128i = _mm_loadu_si128(its_vector_m128i + 23);
    __m128i c27_align_rslt23_m128i = _mm_alignr_epi8(c27_load_rslt24_m128i, c27_load_rslt23_m128i, 12);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt23_m128i, presult);

    __m128i c27_load_rslt25_m128i = _mm_loadu_si128(its_vector_m128i + 24);
    __m128i c27_align_rslt24_m128i = _mm_alignr_epi8(c27_load_rslt25_m128i, c27_load_rslt24_m128i, 10);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt24_m128i, presult);

    __m128i c27_load_rslt26_m128i = _mm_loadu_si128(its_vector_m128i + 25);
    __m128i c27_align_rslt25_m128i = _mm_alignr_epi8(c27_load_rslt26_m128i, c27_load_rslt25_m128i, 7);
    decompress_sse4_c27_m2_s2<0>(c27_align_rslt25_m128i, presult);

    __m128i c27_load_rslt27_m128i = _mm_loadu_si128(its_vector_m128i + 26);
    __m128i c27_align_rslt26_m128i = _mm_alignr_epi8(c27_load_rslt27_m128i, c27_load_rslt26_m128i, 5);
    decompress_sse4_c27_m2_s1<0>(c27_align_rslt26_m128i, presult);

    decompress_sse4_c27_m2_s2<2>(c27_load_rslt27_m128i, presult);
}

void decompress_sse4_c27(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c27_m1(vector_m128i, presult + index, offset);
        offset += 27;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_27(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c28
const __m128i SSE4_c28_and_msk1_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xFF);

const __m128i SSE4_c28_mul_msk1_m128i = _mm_set_epi32(0x01, 0x10, 0x01, 0x10);

////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////

template <int segment>
inline void decompress_sse4_c28_m2(__m128i c28_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c28_shfl_msk_m128i = _mm_set_epi8(
        segment + 13, segment + 12, segment + 11, segment + 10, segment + 10, segment + 9, segment + 8, segment + 7,
        segment + 6, segment + 5, segment + 4, segment + 3, segment + 3, segment + 2, segment + 1, segment);

    __m128i c28_shfl_rslt_m128i = _mm_shuffle_epi8(c28_load_vec_m128i, SSE4_c28_shfl_msk_m128i);
    __m128i c28_and_rslt_m128i = _mm_and_si128(c28_shfl_rslt_m128i, SSE4_c28_and_msk1_m128i);
    __m128i c28_mul_rslt_m128i = _mm_mullo_epi32(c28_and_rslt_m128i, SSE4_c28_mul_msk1_m128i);
    __m128i c28_rslt_m128i = _mm_srli_epi32(c28_mul_rslt_m128i, 4);
    _mm_storeu_si128((__m128i*)presult, c28_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c28_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c28_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c28_m2<0>(c28_load_rslt1_m128i, presult);

    __m128i c28_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c28_align_rslt1_m128i = _mm_alignr_epi8(c28_load_rslt2_m128i, c28_load_rslt1_m128i, 14);
    decompress_sse4_c28_m2<0>(c28_align_rslt1_m128i, presult);

    __m128i c28_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c28_align_rslt2_m128i = _mm_alignr_epi8(c28_load_rslt3_m128i, c28_load_rslt2_m128i, 12);
    decompress_sse4_c28_m2<0>(c28_align_rslt2_m128i, presult);

    __m128i c28_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c28_align_rslt3_m128i = _mm_alignr_epi8(c28_load_rslt4_m128i, c28_load_rslt3_m128i, 10);
    decompress_sse4_c28_m2<0>(c28_align_rslt3_m128i, presult);

    __m128i c28_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c28_align_rslt4_m128i = _mm_alignr_epi8(c28_load_rslt5_m128i, c28_load_rslt4_m128i, 8);
    decompress_sse4_c28_m2<0>(c28_align_rslt4_m128i, presult);

    __m128i c28_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c28_align_rslt5_m128i = _mm_alignr_epi8(c28_load_rslt6_m128i, c28_load_rslt5_m128i, 6);
    decompress_sse4_c28_m2<0>(c28_align_rslt5_m128i, presult);

    __m128i c28_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c28_align_rslt6_m128i = _mm_alignr_epi8(c28_load_rslt7_m128i, c28_load_rslt6_m128i, 4);
    decompress_sse4_c28_m2<0>(c28_align_rslt6_m128i, presult);

    decompress_sse4_c28_m2<2>(c28_load_rslt7_m128i, presult);
}

void decompress_sse4_c28(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 32 <= n) {
        decompress_sse4_c28_m1(vector_m128i, presult + index, offset);
        offset += 7;
        index += 32;
    }
    if (n & 0x1F) {
        unpack_28(dest + index, encode + offset * 4, (n & 0x1F));
    }
}

// c29
const __m128i SSE4_c29_and_msk1_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF);

const __m128i SSE4_c29_and_msk2_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xFF);

const __m128i SSE4_c29_mul_msk1_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

const __m128i SSE4_c29_mul_msk2_m128i = _mm_set_epi32(0x01, 0x02, 0x04, 0x08);

template <int segment>
inline void decompress_sse4_c29_m2_s1(__m128i c29_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c29_shfl_msk_m128i = _mm_set_epi8(
        segment + 14, segment + 13, segment + 12, segment + 11, segment + 10, segment + 9, segment + 8, segment + 7,
        segment + 7, segment + 6, segment + 5, segment + 4, segment + 3, segment + 2, segment + 1, segment);

    __m128i c29_shfl_rslt_m128i = _mm_shuffle_epi8(c29_load_vec_m128i, SSE4_c29_shfl_msk_m128i);
    __m128i c29_shift_rslt_m128i = _mm_slli_epi64(c29_shfl_rslt_m128i, 4);

    __m128i c29_blend_rslt_m128i = _mm_blend_epi16(c29_shfl_rslt_m128i, c29_shift_rslt_m128i, 0xCC);

    __m128i c29_and_rslt_m128i = _mm_and_si128(c29_blend_rslt_m128i, SSE4_c29_and_msk1_m128i);
    __m128i c29_mul_rslt_m128i = _mm_mullo_epi32(c29_and_rslt_m128i, SSE4_c29_mul_msk1_m128i);
    __m128i c29_rslt_m128i = _mm_srli_epi32(c29_mul_rslt_m128i, 3);
    _mm_storeu_si128((__m128i*)presult, c29_rslt_m128i);

    presult = presult + 4;
}

template <int segment>
inline void decompress_sse4_c29_m2_s2(__m128i c29_load_vec_m128i, unsigned int*& presult)
{
    __m128i SSE4_c29_shfl_msk_m128i = _mm_set_epi8(
        segment + 14, segment + 13, segment + 12, segment + 11, segment + 10, segment + 9, segment + 8, segment + 7,
        segment + 7, segment + 6, segment + 5, segment + 4, segment + 3, segment + 2, segment + 1, segment);

    __m128i c29_shfl_rslt_m128i = _mm_shuffle_epi8(c29_load_vec_m128i, SSE4_c29_shfl_msk_m128i);
    __m128i c29_shift_rslt_m128i = _mm_srli_epi64(c29_shfl_rslt_m128i, 4);

    __m128i c29_blend_rslt_m128i = _mm_blend_epi16(c29_shfl_rslt_m128i, c29_shift_rslt_m128i, 0x33);

    __m128i c29_and_rslt_m128i = _mm_and_si128(c29_blend_rslt_m128i, SSE4_c29_and_msk2_m128i);
    __m128i c29_mul_rslt_m128i = _mm_mullo_epi32(c29_and_rslt_m128i, SSE4_c29_mul_msk2_m128i);
    __m128i c29_rslt_m128i = _mm_srli_epi32(c29_mul_rslt_m128i, 3);
    _mm_storeu_si128((__m128i*)presult, c29_rslt_m128i);

    presult = presult + 4;
}

inline void decompress_sse4_c29_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c29_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c29_m2_s1<0>(c29_load_rslt1_m128i, presult);

    __m128i c29_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c29_align_rslt1_m128i = _mm_alignr_epi8(c29_load_rslt2_m128i, c29_load_rslt1_m128i, 14);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt1_m128i, presult);

    __m128i c29_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c29_align_rslt2_m128i = _mm_alignr_epi8(c29_load_rslt3_m128i, c29_load_rslt2_m128i, 13);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt2_m128i, presult);

    __m128i c29_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c29_align_rslt3_m128i = _mm_alignr_epi8(c29_load_rslt4_m128i, c29_load_rslt3_m128i, 11);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt3_m128i, presult);

    __m128i c29_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c29_align_rslt4_m128i = _mm_alignr_epi8(c29_load_rslt5_m128i, c29_load_rslt4_m128i, 10);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt4_m128i, presult);

    __m128i c29_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c29_align_rslt5_m128i = _mm_alignr_epi8(c29_load_rslt6_m128i, c29_load_rslt5_m128i, 8);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt5_m128i, presult);

    __m128i c29_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c29_align_rslt6_m128i = _mm_alignr_epi8(c29_load_rslt7_m128i, c29_load_rslt6_m128i, 7);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt6_m128i, presult);

    __m128i c29_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c29_align_rslt7_m128i = _mm_alignr_epi8(c29_load_rslt8_m128i, c29_load_rslt7_m128i, 5);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt7_m128i, presult);

    __m128i c29_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c29_align_rslt8_m128i = _mm_alignr_epi8(c29_load_rslt9_m128i, c29_load_rslt8_m128i, 4);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt8_m128i, presult);

    __m128i c29_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c29_align_rslt9_m128i = _mm_alignr_epi8(c29_load_rslt10_m128i, c29_load_rslt9_m128i, 2);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt9_m128i, presult);

    decompress_sse4_c29_m2_s1<1>(c29_load_rslt10_m128i, presult);

    __m128i c29_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c29_align_rslt10_m128i = _mm_alignr_epi8(c29_load_rslt11_m128i, c29_load_rslt10_m128i, 15);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt10_m128i, presult);

    __m128i c29_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c29_align_rslt11_m128i = _mm_alignr_epi8(c29_load_rslt12_m128i, c29_load_rslt11_m128i, 14);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt11_m128i, presult);

    __m128i c29_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c29_align_rslt12_m128i = _mm_alignr_epi8(c29_load_rslt13_m128i, c29_load_rslt12_m128i, 12);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt12_m128i, presult);

    __m128i c29_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c29_align_rslt13_m128i = _mm_alignr_epi8(c29_load_rslt14_m128i, c29_load_rslt13_m128i, 11);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt13_m128i, presult);

    __m128i c29_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c29_align_rslt14_m128i = _mm_alignr_epi8(c29_load_rslt15_m128i, c29_load_rslt14_m128i, 9);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt14_m128i, presult);

    __m128i c29_load_rslt16_m128i = _mm_loadu_si128(its_vector_m128i + 15);
    __m128i c29_align_rslt15_m128i = _mm_alignr_epi8(c29_load_rslt16_m128i, c29_load_rslt15_m128i, 8);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt15_m128i, presult);

    __m128i c29_load_rslt17_m128i = _mm_loadu_si128(its_vector_m128i + 16);
    __m128i c29_align_rslt16_m128i = _mm_alignr_epi8(c29_load_rslt17_m128i, c29_load_rslt16_m128i, 6);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt16_m128i, presult);

    __m128i c29_load_rslt18_m128i = _mm_loadu_si128(its_vector_m128i + 17);
    __m128i c29_align_rslt17_m128i = _mm_alignr_epi8(c29_load_rslt18_m128i, c29_load_rslt17_m128i, 5);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt17_m128i, presult);

    __m128i c29_load_rslt19_m128i = _mm_loadu_si128(its_vector_m128i + 18);
    __m128i c29_align_rslt18_m128i = _mm_alignr_epi8(c29_load_rslt19_m128i, c29_load_rslt18_m128i, 3);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt18_m128i, presult);

    __m128i c29_load_rslt20_m128i = _mm_loadu_si128(its_vector_m128i + 19);
    __m128i c29_align_rslt19_m128i = _mm_alignr_epi8(c29_load_rslt20_m128i, c29_load_rslt19_m128i, 2);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt19_m128i, presult);

    decompress_sse4_c29_m2_s2<0>(c29_load_rslt20_m128i, presult);

    __m128i c29_load_rslt21_m128i = _mm_loadu_si128(its_vector_m128i + 20);
    __m128i c29_align_rslt20_m128i = _mm_alignr_epi8(c29_load_rslt21_m128i, c29_load_rslt20_m128i, 15);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt20_m128i, presult);

    __m128i c29_load_rslt22_m128i = _mm_loadu_si128(its_vector_m128i + 21);
    __m128i c29_align_rslt21_m128i = _mm_alignr_epi8(c29_load_rslt22_m128i, c29_load_rslt21_m128i, 13);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt21_m128i, presult);

    __m128i c29_load_rslt23_m128i = _mm_loadu_si128(its_vector_m128i + 22);
    __m128i c29_align_rslt22_m128i = _mm_alignr_epi8(c29_load_rslt23_m128i, c29_load_rslt22_m128i, 12);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt22_m128i, presult);

    __m128i c29_load_rslt24_m128i = _mm_loadu_si128(its_vector_m128i + 23);
    __m128i c29_align_rslt23_m128i = _mm_alignr_epi8(c29_load_rslt24_m128i, c29_load_rslt23_m128i, 10);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt23_m128i, presult);

    __m128i c29_load_rslt25_m128i = _mm_loadu_si128(its_vector_m128i + 24);
    __m128i c29_align_rslt24_m128i = _mm_alignr_epi8(c29_load_rslt25_m128i, c29_load_rslt24_m128i, 9);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt24_m128i, presult);

    __m128i c29_load_rslt26_m128i = _mm_loadu_si128(its_vector_m128i + 25);
    __m128i c29_align_rslt25_m128i = _mm_alignr_epi8(c29_load_rslt26_m128i, c29_load_rslt25_m128i, 7);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt25_m128i, presult);

    __m128i c29_load_rslt27_m128i = _mm_loadu_si128(its_vector_m128i + 26);
    __m128i c29_align_rslt26_m128i = _mm_alignr_epi8(c29_load_rslt27_m128i, c29_load_rslt26_m128i, 6);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt26_m128i, presult);

    __m128i c29_load_rslt28_m128i = _mm_loadu_si128(its_vector_m128i + 27);
    __m128i c29_align_rslt27_m128i = _mm_alignr_epi8(c29_load_rslt28_m128i, c29_load_rslt27_m128i, 4);
    decompress_sse4_c29_m2_s2<0>(c29_align_rslt27_m128i, presult);

    __m128i c29_load_rslt29_m128i = _mm_loadu_si128(its_vector_m128i + 28);
    __m128i c29_align_rslt28_m128i = _mm_alignr_epi8(c29_load_rslt29_m128i, c29_load_rslt28_m128i, 3);
    decompress_sse4_c29_m2_s1<0>(c29_align_rslt28_m128i, presult);

    decompress_sse4_c29_m2_s2<1>(c29_load_rslt29_m128i, presult);
}

void decompress_sse4_c29(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 128 <= n) {
        decompress_sse4_c29_m1(vector_m128i, presult + index, offset);
        offset += 29;
        index += 128;
    }
    if (n & 0x7F) {
        unpack_29(dest + index, encode + offset * 4, (n & 0x7F));
    }
}

// c30
const __m128i SSE4_c30_and_msk1_m128i =
    _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF);

const __m128i SSE4_c30_mul_msk1_m128i = _mm_set_epi32(0x01, 0x04, 0x04, 0x04);

template <int segment>
inline void decompress_sse4_c30_m2(__m128i c30_load_vec_m128i, unsigned int* presult)
{
    __m128i SSE4_c30_shfl_msk_m128i = _mm_set_epi8(
        segment + 14, segment + 13, segment + 12, segment + 11, segment + 10, segment + 9, segment + 8, segment + 7,
        segment + 7, segment + 6, segment + 5, segment + 4, segment + 3, segment + 2, segment + 1, segment);

    __m128i c30_shfl_rslt_m128i = _mm_shuffle_epi8(c30_load_vec_m128i, SSE4_c30_shfl_msk_m128i);
    __m128i c30_shift_rslt_t1_m128i = _mm_slli_epi64(c30_shfl_rslt_m128i, 2);
    __m128i c30_shift_rslt_t2_m128i = _mm_srli_epi64(c30_shfl_rslt_m128i, 4);

    __m128i c30_blend_rslt1_m128i = _mm_blend_epi16(c30_shfl_rslt_m128i, c30_shift_rslt_t1_m128i, 0x0C);
    __m128i c30_blend_rslt2_m128i = _mm_blend_epi16(c30_blend_rslt1_m128i, c30_shift_rslt_t2_m128i, 0x30);

    __m128i c30_and_rslt_m128i = _mm_and_si128(c30_blend_rslt2_m128i, SSE4_c30_and_msk1_m128i);
    __m128i c30_mul_rslt_m128i = _mm_mullo_epi32(c30_and_rslt_m128i, SSE4_c30_mul_msk1_m128i);
    __m128i c30_rslt_m128i = _mm_srli_epi32(c30_mul_rslt_m128i, 2);
    _mm_storeu_si128((__m128i*)presult, c30_rslt_m128i);

    //__m128i c30_equal_rslt_m128i = _mm_cmpeq_epi32(c30_rslt_m128i, SSE4_zero_m128i);
    // int c30_tstz_rslt_int = _mm_testz_si128(c30_equal_rslt_m128i, SSE4_set_m128i);
}

inline void decompress_sse4_c30_m1(__m128i* vector_m128i, unsigned int* presult, unsigned int offset)
{
    __m128i* its_vector_m128i = vector_m128i + offset;

    __m128i c30_load_rslt1_m128i = _mm_loadu_si128(its_vector_m128i);
    decompress_sse4_c30_m2<0>(c30_load_rslt1_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt2_m128i = _mm_loadu_si128(its_vector_m128i + 1);
    __m128i c30_align_rslt1_m128i = _mm_alignr_epi8(c30_load_rslt2_m128i, c30_load_rslt1_m128i, 15);
    decompress_sse4_c30_m2<0>(c30_align_rslt1_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt3_m128i = _mm_loadu_si128(its_vector_m128i + 2);
    __m128i c30_align_rslt2_m128i = _mm_alignr_epi8(c30_load_rslt3_m128i, c30_load_rslt2_m128i, 14);
    decompress_sse4_c30_m2<0>(c30_align_rslt2_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt4_m128i = _mm_loadu_si128(its_vector_m128i + 3);
    __m128i c30_align_rslt3_m128i = _mm_alignr_epi8(c30_load_rslt4_m128i, c30_load_rslt3_m128i, 13);
    decompress_sse4_c30_m2<0>(c30_align_rslt3_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt5_m128i = _mm_loadu_si128(its_vector_m128i + 4);
    __m128i c30_align_rslt4_m128i = _mm_alignr_epi8(c30_load_rslt5_m128i, c30_load_rslt4_m128i, 12);
    decompress_sse4_c30_m2<0>(c30_align_rslt4_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt6_m128i = _mm_loadu_si128(its_vector_m128i + 5);
    __m128i c30_align_rslt5_m128i = _mm_alignr_epi8(c30_load_rslt6_m128i, c30_load_rslt5_m128i, 11);
    decompress_sse4_c30_m2<0>(c30_align_rslt5_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt7_m128i = _mm_loadu_si128(its_vector_m128i + 6);
    __m128i c30_align_rslt6_m128i = _mm_alignr_epi8(c30_load_rslt7_m128i, c30_load_rslt6_m128i, 10);
    decompress_sse4_c30_m2<0>(c30_align_rslt6_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt8_m128i = _mm_loadu_si128(its_vector_m128i + 7);
    __m128i c30_align_rslt7_m128i = _mm_alignr_epi8(c30_load_rslt8_m128i, c30_load_rslt7_m128i, 9);
    decompress_sse4_c30_m2<0>(c30_align_rslt7_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt9_m128i = _mm_loadu_si128(its_vector_m128i + 8);
    __m128i c30_align_rslt8_m128i = _mm_alignr_epi8(c30_load_rslt9_m128i, c30_load_rslt8_m128i, 8);
    decompress_sse4_c30_m2<0>(c30_align_rslt8_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt10_m128i = _mm_loadu_si128(its_vector_m128i + 9);
    __m128i c30_align_rslt9_m128i = _mm_alignr_epi8(c30_load_rslt10_m128i, c30_load_rslt9_m128i, 7);
    decompress_sse4_c30_m2<0>(c30_align_rslt9_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt11_m128i = _mm_loadu_si128(its_vector_m128i + 10);
    __m128i c30_align_rslt10_m128i = _mm_alignr_epi8(c30_load_rslt11_m128i, c30_load_rslt10_m128i, 6);
    decompress_sse4_c30_m2<0>(c30_align_rslt10_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt12_m128i = _mm_loadu_si128(its_vector_m128i + 11);
    __m128i c30_align_rslt11_m128i = _mm_alignr_epi8(c30_load_rslt12_m128i, c30_load_rslt11_m128i, 5);
    decompress_sse4_c30_m2<0>(c30_align_rslt11_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt13_m128i = _mm_loadu_si128(its_vector_m128i + 12);
    __m128i c30_align_rslt12_m128i = _mm_alignr_epi8(c30_load_rslt13_m128i, c30_load_rslt12_m128i, 4);
    decompress_sse4_c30_m2<0>(c30_align_rslt12_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt14_m128i = _mm_loadu_si128(its_vector_m128i + 13);
    __m128i c30_align_rslt13_m128i = _mm_alignr_epi8(c30_load_rslt14_m128i, c30_load_rslt13_m128i, 3);
    decompress_sse4_c30_m2<0>(c30_align_rslt13_m128i, presult);
    presult = presult + 4;

    __m128i c30_load_rslt15_m128i = _mm_loadu_si128(its_vector_m128i + 14);
    __m128i c30_align_rslt14_m128i = _mm_alignr_epi8(c30_load_rslt15_m128i, c30_load_rslt14_m128i, 2);
    decompress_sse4_c30_m2<0>(c30_align_rslt14_m128i, presult);
    presult = presult + 4;

    decompress_sse4_c30_m2<1>(c30_load_rslt15_m128i, presult);
    presult = presult + 4;
}

void decompress_sse4_c30(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    unsigned int index = 0, offset = 0;
    __m128i* vector_m128i = (__m128i*)encode;
    unsigned int* presult = (unsigned int*)dest;
    while (index + 64 <= n) {
        decompress_sse4_c30_m1(vector_m128i, presult + index, offset);
        offset += 15;
        index += 64;
    }
    if (n & 0x3F) {
        unpack_30(dest + index, encode + offset * 4, (n & 0x3F));
    }
}

// c31
void decompress_sse4_c31(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 31) {
        dest[0] = encode[0] & 0x7FFFFFFF;
        dest[1] = ((encode[0] >> 31) | (encode[1] << 1)) & 0x7FFFFFFF;
        dest[2] = ((encode[1] >> 30) | (encode[2] << 2)) & 0x7FFFFFFF;
        dest[3] = ((encode[2] >> 29) | (encode[3] << 3)) & 0x7FFFFFFF;
        dest[4] = ((encode[3] >> 28) | (encode[4] << 4)) & 0x7FFFFFFF;
        dest[5] = ((encode[4] >> 27) | (encode[5] << 5)) & 0x7FFFFFFF;
        dest[6] = ((encode[5] >> 26) | (encode[6] << 6)) & 0x7FFFFFFF;
        dest[7] = ((encode[6] >> 25) | (encode[7] << 7)) & 0x7FFFFFFF;
        dest[8] = ((encode[7] >> 24) | (encode[8] << 8)) & 0x7FFFFFFF;
        dest[9] = ((encode[8] >> 23) | (encode[9] << 9)) & 0x7FFFFFFF;
        dest[10] = ((encode[9] >> 22) | (encode[10] << 10)) & 0x7FFFFFFF;
        dest[11] = ((encode[10] >> 21) | (encode[11] << 11)) & 0x7FFFFFFF;
        dest[12] = ((encode[11] >> 20) | (encode[12] << 12)) & 0x7FFFFFFF;
        dest[13] = ((encode[12] >> 19) | (encode[13] << 13)) & 0x7FFFFFFF;
        dest[14] = ((encode[13] >> 18) | (encode[14] << 14)) & 0x7FFFFFFF;
        dest[15] = ((encode[14] >> 17) | (encode[15] << 15)) & 0x7FFFFFFF;
        dest[16] = ((encode[15] >> 16) | (encode[16] << 16)) & 0x7FFFFFFF;
        dest[17] = ((encode[16] >> 15) | (encode[17] << 17)) & 0x7FFFFFFF;
        dest[18] = ((encode[17] >> 14) | (encode[18] << 18)) & 0x7FFFFFFF;
        dest[19] = ((encode[18] >> 13) | (encode[19] << 19)) & 0x7FFFFFFF;
        dest[20] = ((encode[19] >> 12) | (encode[20] << 20)) & 0x7FFFFFFF;
        dest[21] = ((encode[20] >> 11) | (encode[21] << 21)) & 0x7FFFFFFF;
        dest[22] = ((encode[21] >> 10) | (encode[22] << 22)) & 0x7FFFFFFF;
        dest[23] = ((encode[22] >> 9) | (encode[23] << 23)) & 0x7FFFFFFF;
        dest[24] = ((encode[23] >> 8) | (encode[24] << 24)) & 0x7FFFFFFF;
        dest[25] = ((encode[24] >> 7) | (encode[25] << 25)) & 0x7FFFFFFF;
        dest[26] = ((encode[25] >> 6) | (encode[26] << 26)) & 0x7FFFFFFF;
        dest[27] = ((encode[26] >> 5) | (encode[27] << 27)) & 0x7FFFFFFF;
        dest[28] = ((encode[27] >> 4) | (encode[28] << 28)) & 0x7FFFFFFF;
        dest[29] = ((encode[28] >> 3) | (encode[29] << 29)) & 0x7FFFFFFF;
        dest[30] = ((encode[29] >> 2) | (encode[30] << 30)) & 0x7FFFFFFF;
        dest[31] = (encode[30] >> 1) & 0x7FFFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_31(dest, encode, (n & 0x1F));
    }
}

// c32
void decompress_sse4_c32(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 32) {
        dest[0] = encode[0] & 0xFFFFFFFF;
        dest[1] = encode[1] & 0xFFFFFFFF;
        dest[2] = encode[2] & 0xFFFFFFFF;
        dest[3] = encode[3] & 0xFFFFFFFF;
        dest[4] = encode[4] & 0xFFFFFFFF;
        dest[5] = encode[5] & 0xFFFFFFFF;
        dest[6] = encode[6] & 0xFFFFFFFF;
        dest[7] = encode[7] & 0xFFFFFFFF;
        dest[8] = encode[8] & 0xFFFFFFFF;
        dest[9] = encode[9] & 0xFFFFFFFF;
        dest[10] = encode[10] & 0xFFFFFFFF;
        dest[11] = encode[11] & 0xFFFFFFFF;
        dest[12] = encode[12] & 0xFFFFFFFF;
        dest[13] = encode[13] & 0xFFFFFFFF;
        dest[14] = encode[14] & 0xFFFFFFFF;
        dest[15] = encode[15] & 0xFFFFFFFF;
        dest[16] = encode[16] & 0xFFFFFFFF;
        dest[17] = encode[17] & 0xFFFFFFFF;
        dest[18] = encode[18] & 0xFFFFFFFF;
        dest[19] = encode[19] & 0xFFFFFFFF;
        dest[20] = encode[20] & 0xFFFFFFFF;
        dest[21] = encode[21] & 0xFFFFFFFF;
        dest[22] = encode[22] & 0xFFFFFFFF;
        dest[23] = encode[23] & 0xFFFFFFFF;
        dest[24] = encode[24] & 0xFFFFFFFF;
        dest[25] = encode[25] & 0xFFFFFFFF;
        dest[26] = encode[26] & 0xFFFFFFFF;
        dest[27] = encode[27] & 0xFFFFFFFF;
        dest[28] = encode[28] & 0xFFFFFFFF;
        dest[29] = encode[29] & 0xFFFFFFFF;
        dest[30] = encode[30] & 0xFFFFFFFF;
        dest[31] = encode[31] & 0xFFFFFFFF;
    }
    if (n & 0x1F) {
        for (uint32_t i = 0; i < (n & 0x1F); ++i) {
            dest[i] = encode[i] & 0xFFFFFFFF;
        }
    }
}
} // namespace indexlib::index
