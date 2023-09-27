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
#pragma once

#include <stdint.h>

#include "unaligned_unpack.h"

namespace indexlib {

template <typename Type>
void unpack_0(Type* dest, const uint32_t* encode, uint32_t n)
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

template <typename Type>
void unpack_1(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 1, dest += 32) {
        dest[0] = encode[0] & 0x1;
        dest[1] = (encode[0] >> 1) & 0x1;
        dest[2] = (encode[0] >> 2) & 0x1;
        dest[3] = (encode[0] >> 3) & 0x1;
        dest[4] = (encode[0] >> 4) & 0x1;
        dest[5] = (encode[0] >> 5) & 0x1;
        dest[6] = (encode[0] >> 6) & 0x1;
        dest[7] = (encode[0] >> 7) & 0x1;
        dest[8] = (encode[0] >> 8) & 0x1;
        dest[9] = (encode[0] >> 9) & 0x1;
        dest[10] = (encode[0] >> 10) & 0x1;
        dest[11] = (encode[0] >> 11) & 0x1;
        dest[12] = (encode[0] >> 12) & 0x1;
        dest[13] = (encode[0] >> 13) & 0x1;
        dest[14] = (encode[0] >> 14) & 0x1;
        dest[15] = (encode[0] >> 15) & 0x1;
        dest[16] = (encode[0] >> 16) & 0x1;
        dest[17] = (encode[0] >> 17) & 0x1;
        dest[18] = (encode[0] >> 18) & 0x1;
        dest[19] = (encode[0] >> 19) & 0x1;
        dest[20] = (encode[0] >> 20) & 0x1;
        dest[21] = (encode[0] >> 21) & 0x1;
        dest[22] = (encode[0] >> 22) & 0x1;
        dest[23] = (encode[0] >> 23) & 0x1;
        dest[24] = (encode[0] >> 24) & 0x1;
        dest[25] = (encode[0] >> 25) & 0x1;
        dest[26] = (encode[0] >> 26) & 0x1;
        dest[27] = (encode[0] >> 27) & 0x1;
        dest[28] = (encode[0] >> 28) & 0x1;
        dest[29] = (encode[0] >> 29) & 0x1;
        dest[30] = (encode[0] >> 30) & 0x1;
        dest[31] = (encode[0] >> 31) & 0x1;
    }
    if (n & 0x1F) {
        unaligned_unpack_1(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_2(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 2) {
        dest[0] = encode[0] & 0x3;
        dest[1] = (encode[0] >> 2) & 0x3;
        dest[2] = (encode[0] >> 4) & 0x3;
        dest[3] = (encode[0] >> 6) & 0x3;
        dest[4] = (encode[0] >> 8) & 0x3;
        dest[5] = (encode[0] >> 10) & 0x3;
        dest[6] = (encode[0] >> 12) & 0x3;
        dest[7] = (encode[0] >> 14) & 0x3;
        dest[8] = (encode[0] >> 16) & 0x3;
        dest[9] = (encode[0] >> 18) & 0x3;
        dest[10] = (encode[0] >> 20) & 0x3;
        dest[11] = (encode[0] >> 22) & 0x3;
        dest[12] = (encode[0] >> 24) & 0x3;
        dest[13] = (encode[0] >> 26) & 0x3;
        dest[14] = (encode[0] >> 28) & 0x3;
        dest[15] = (encode[0] >> 30) & 0x3;
        dest[16] = encode[1] & 0x3;
        dest[17] = (encode[1] >> 2) & 0x3;
        dest[18] = (encode[1] >> 4) & 0x3;
        dest[19] = (encode[1] >> 6) & 0x3;
        dest[20] = (encode[1] >> 8) & 0x3;
        dest[21] = (encode[1] >> 10) & 0x3;
        dest[22] = (encode[1] >> 12) & 0x3;
        dest[23] = (encode[1] >> 14) & 0x3;
        dest[24] = (encode[1] >> 16) & 0x3;
        dest[25] = (encode[1] >> 18) & 0x3;
        dest[26] = (encode[1] >> 20) & 0x3;
        dest[27] = (encode[1] >> 22) & 0x3;
        dest[28] = (encode[1] >> 24) & 0x3;
        dest[29] = (encode[1] >> 26) & 0x3;
        dest[30] = (encode[1] >> 28) & 0x3;
        dest[31] = (encode[1] >> 30) & 0x3;
    }

    if (n & 0x1F) {
        unaligned_unpack_2(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_3(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; dest += 32, i += 32, encode += 3) {
        dest[0] = encode[0] & 0x7;
        dest[1] = (encode[0] >> 3) & 0x7;
        dest[2] = (encode[0] >> 6) & 0x7;
        dest[3] = (encode[0] >> 9) & 0x7;
        dest[4] = (encode[0] >> 12) & 0x7;
        dest[5] = (encode[0] >> 15) & 0x7;
        dest[6] = (encode[0] >> 18) & 0x7;
        dest[7] = (encode[0] >> 21) & 0x7;
        dest[8] = (encode[0] >> 24) & 0x7;
        dest[9] = (encode[0] >> 27) & 0x7;
        dest[10] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x7;
        dest[11] = (encode[1] >> 1) & 0x7;
        dest[12] = (encode[1] >> 4) & 0x7;
        dest[13] = (encode[1] >> 7) & 0x7;
        dest[14] = (encode[1] >> 10) & 0x7;
        dest[15] = (encode[1] >> 13) & 0x7;
        dest[16] = (encode[1] >> 16) & 0x7;
        dest[17] = (encode[1] >> 19) & 0x7;
        dest[18] = (encode[1] >> 22) & 0x7;
        dest[19] = (encode[1] >> 25) & 0x7;
        dest[20] = (encode[1] >> 28) & 0x7;
        dest[21] = ((encode[1] >> 31) | (encode[2] << 1)) & 0x7;
        dest[22] = (encode[2] >> 2) & 0x7;
        dest[23] = (encode[2] >> 5) & 0x7;
        dest[24] = (encode[2] >> 8) & 0x7;
        dest[25] = (encode[2] >> 11) & 0x7;
        dest[26] = (encode[2] >> 14) & 0x7;
        dest[27] = (encode[2] >> 17) & 0x7;
        dest[28] = (encode[2] >> 20) & 0x7;
        dest[29] = (encode[2] >> 23) & 0x7;
        dest[30] = (encode[2] >> 26) & 0x7;
        dest[31] = (encode[2] >> 29) & 0x7;
    }

    if (n & 0x1F) {
        unaligned_unpack_3(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_4(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 4) {
        dest[0] = encode[0] & 0xF;
        dest[1] = (encode[0] >> 4) & 0xF;
        dest[2] = (encode[0] >> 8) & 0xF;
        dest[3] = (encode[0] >> 12) & 0xF;
        dest[4] = (encode[0] >> 16) & 0xF;
        dest[5] = (encode[0] >> 20) & 0xF;
        dest[6] = (encode[0] >> 24) & 0xF;
        dest[7] = (encode[0] >> 28) & 0xF;
        dest[8] = encode[1] & 0xF;
        dest[9] = (encode[1] >> 4) & 0xF;
        dest[10] = (encode[1] >> 8) & 0xF;
        dest[11] = (encode[1] >> 12) & 0xF;
        dest[12] = (encode[1] >> 16) & 0xF;
        dest[13] = (encode[1] >> 20) & 0xF;
        dest[14] = (encode[1] >> 24) & 0xF;
        dest[15] = (encode[1] >> 28) & 0xF;
        dest[16] = encode[2] & 0xF;
        dest[17] = (encode[2] >> 4) & 0xF;
        dest[18] = (encode[2] >> 8) & 0xF;
        dest[19] = (encode[2] >> 12) & 0xF;
        dest[20] = (encode[2] >> 16) & 0xF;
        dest[21] = (encode[2] >> 20) & 0xF;
        dest[22] = (encode[2] >> 24) & 0xF;
        dest[23] = (encode[2] >> 28) & 0xF;
        dest[24] = encode[3] & 0xF;
        dest[25] = (encode[3] >> 4) & 0xF;
        dest[26] = (encode[3] >> 8) & 0xF;
        dest[27] = (encode[3] >> 12) & 0xF;
        dest[28] = (encode[3] >> 16) & 0xF;
        dest[29] = (encode[3] >> 20) & 0xF;
        dest[30] = (encode[3] >> 24) & 0xF;
        dest[31] = (encode[3] >> 28) & 0xF;
    }

    if (n & 0x1F) {
        unaligned_unpack_4(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_5(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 5) {
        dest[0] = encode[0] & 0x1F;
        dest[1] = (encode[0] >> 5) & 0x1F;
        dest[2] = (encode[0] >> 10) & 0x1F;
        dest[3] = (encode[0] >> 15) & 0x1F;
        dest[4] = (encode[0] >> 20) & 0x1F;
        dest[5] = (encode[0] >> 25) & 0x1F;
        dest[6] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x1F;
        dest[7] = (encode[1] >> 3) & 0x1F;
        dest[8] = (encode[1] >> 8) & 0x1F;
        dest[9] = (encode[1] >> 13) & 0x1F;
        dest[10] = (encode[1] >> 18) & 0x1F;
        dest[11] = (encode[1] >> 23) & 0x1F;
        dest[12] = ((encode[1] >> 28) | (encode[2] << 4)) & 0x1F;
        dest[13] = (encode[2] >> 1) & 0x1F;
        dest[14] = (encode[2] >> 6) & 0x1F;
        dest[15] = (encode[2] >> 11) & 0x1F;
        dest[16] = (encode[2] >> 16) & 0x1F;
        dest[17] = (encode[2] >> 21) & 0x1F;
        dest[18] = (encode[2] >> 26) & 0x1F;
        dest[19] = ((encode[2] >> 31) | (encode[3] << 1)) & 0x1F;
        dest[20] = (encode[3] >> 4) & 0x1F;
        dest[21] = (encode[3] >> 9) & 0x1F;
        dest[22] = (encode[3] >> 14) & 0x1F;
        dest[23] = (encode[3] >> 19) & 0x1F;
        dest[24] = (encode[3] >> 24) & 0x1F;
        dest[25] = ((encode[3] >> 29) | (encode[4] << 3)) & 0x1F;
        dest[26] = (encode[4] >> 2) & 0x1F;
        dest[27] = (encode[4] >> 7) & 0x1F;
        dest[28] = (encode[4] >> 12) & 0x1F;
        dest[29] = (encode[4] >> 17) & 0x1F;
        dest[30] = (encode[4] >> 22) & 0x1F;
        dest[31] = (encode[4] >> 27) & 0x1F;
    }

    if (n & 0x1F) {
        unaligned_unpack_5(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_6(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 6, dest += 32) {
        dest[0] = encode[0] & 0x3F;
        dest[1] = (encode[0] >> 6) & 0x3F;
        dest[2] = (encode[0] >> 12) & 0x3F;
        dest[3] = (encode[0] >> 18) & 0x3F;
        dest[4] = (encode[0] >> 24) & 0x3F;
        dest[5] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x3F;
        dest[6] = (encode[1] >> 4) & 0x3F;
        dest[7] = (encode[1] >> 10) & 0x3F;
        dest[8] = (encode[1] >> 16) & 0x3F;
        dest[9] = (encode[1] >> 22) & 0x3F;
        dest[10] = ((encode[1] >> 28) | (encode[2] << 4)) & 0x3F;
        dest[11] = (encode[2] >> 2) & 0x3F;
        dest[12] = (encode[2] >> 8) & 0x3F;
        dest[13] = (encode[2] >> 14) & 0x3F;
        dest[14] = (encode[2] >> 20) & 0x3F;
        dest[15] = (encode[2] >> 26) & 0x3F;
        dest[16] = encode[3] & 0x3F;
        dest[17] = (encode[3] >> 6) & 0x3F;
        dest[18] = (encode[3] >> 12) & 0x3F;
        dest[19] = (encode[3] >> 18) & 0x3F;
        dest[20] = (encode[3] >> 24) & 0x3F;
        dest[21] = ((encode[3] >> 30) | (encode[4] << 2)) & 0x3F;
        dest[22] = (encode[4] >> 4) & 0x3F;
        dest[23] = (encode[4] >> 10) & 0x3F;
        dest[24] = (encode[4] >> 16) & 0x3F;
        dest[25] = (encode[4] >> 22) & 0x3F;
        dest[26] = ((encode[4] >> 28) | (encode[5] << 4)) & 0x3F;
        dest[27] = (encode[5] >> 2) & 0x3F;
        dest[28] = (encode[5] >> 8) & 0x3F;
        dest[29] = (encode[5] >> 14) & 0x3F;
        dest[30] = (encode[5] >> 20) & 0x3F;
        dest[31] = (encode[5] >> 26) & 0x3F;
    }

    if (n & 0x1F) {
        unaligned_unpack_6(dest, encode, (n & 0x1F));
    }
}

#ifdef INDEXLIB_OPTIMIZE_UNPACK

static const struct mask_6_0 {
    uint8_t a[16];
} mask_6_0 __attribute__((aligned(16))) = {
    {0, 0xff, 0xff, 0xff, 3, 0xff, 0xff, 0xff, 6, 0xff, 0xff, 0xff, 9, 0xff, 0xff, 0xff},
};
static const struct mask_6_1 {
    uint8_t a[16];
} mask_6_1 __attribute__((aligned(16))) = {
    {0, 1, 0xff, 0xff, 3, 4, 0xff, 0xff, 6, 7, 0xff, 0xff, 9, 10, 0xff, 0xff},
};
static const struct mask_6_2 {
    uint8_t a[16];
} mask_6_2 __attribute__((aligned(16))) = {
    {1, 2, 0xff, 0xff, 4, 5, 0xff, 0xff, 7, 8, 0xff, 0xff, 10, 11, 0xff, 0xff},
};
static const struct mask_6_3 {
    uint8_t a[16];
} mask_6_3 __attribute__((aligned(16))) = {
    {2, 0xff, 0xff, 0xff, 5, 0xff, 0xff, 0xff, 8, 0xff, 0xff, 0xff, 11, 0xff, 0xff, 0xff},
};
static const struct and_6_0 {
    uint16_t a[8];
} and_6_0 __attribute__((aligned(16))) = {
    {0x3f, 0, 0x3f, 0, 0x3f, 0, 0x3f, 0},
};

template <>
inline void unpack_6<uint32_t>(uint32_t* dest, const uint32_t* encode, uint32_t n)
{
    __asm__("movdqa %0,%%xmm4\n" // and_0
            "movdqa %1,%%xmm0\n" // mask_0
            "movdqa %2,%%xmm1\n" // mask_1
            "movdqa %3,%%xmm2\n" // mask_2
            "movdqa %4,%%xmm3\n" // mask_3
            ::"m"(and_6_0.a[0]),
            "m"(mask_6_0.a[0]), "m"(mask_6_1.a[0]), "m"(mask_6_2.a[0]), "m"(mask_6_3.a[0]));

    for (uint32_t i = 32; i <= n; i += 32, encode += 6, dest += 32) {
        __asm__ volatile("prefetchnta %0" ::"m"(encode[0]));
        __asm__("movdqu %4,%%xmm5\n"     // encode[0]->xmm5
                "movdqa %%xmm5,%%xmm6\n" // save for futrue aligned move
                "movdqa %%xmm5,%%xmm7\n"
                "movdqa %%xmm5,%%xmm8\n"
                "pshufb %%xmm0,%%xmm5\n" // shuffle 0,3,6,9:a0,b0,c0,d0->xmm5
                "pand %%xmm4,%%xmm5\n"
                "pshufb %%xmm1,%%xmm6\n" // shuffle 1,4,7,10:a1,b1,c1,d1->xmm6
                "psrld $6,%%xmm6\n"
                "pand %%xmm4,%%xmm6\n"
                "pshufb %%xmm2,%%xmm7\n" // shuffle 2,5,8,11:a2,b2,c2,d2->xmm7
                "psrld $4,%%xmm7\n"
                "pand %%xmm4,%%xmm7\n"
                "pshufb %%xmm3,%%xmm8\n" // shuffle 3,6,9,12:a3,b3,c3,d3->xmm8
                "psrld $2,%%xmm8\n"
                "pand %%xmm4,%%xmm8\n"
                "movdqa %%xmm5,%%xmm9\n"
                "movdqa %%xmm6,%%xmm10\n"
                "punpckldq %%xmm7, %%xmm5\n"  // a0,a2,b0,b2->xmm5
                "punpckhdq %%xmm7, %%xmm9\n"  // c0,c2,d0,d2->xmm9
                "punpckldq %%xmm8, %%xmm6\n"  // a1,a3,b1,b3->xmm6
                "punpckhdq %%xmm8, %%xmm10\n" // c1,c3,d1,d3->xmm10
                "movdqa %%xmm5, %%xmm7\n"
                "punpckldq %%xmm6, %%xmm5\n" // a0,a1,a2,a3->xmm5
                "movdqu %%xmm5,%0\n"         // save output
                "punpckhdq %%xmm6, %%xmm7\n" // b0,b1,b2,b3->xmm7
                "movdqu %%xmm7,%1\n"         // save output
                "movdqa %%xmm9, %%xmm8\n"
                "punpckldq %%xmm10, %%xmm9\n" // c0,c1,c2,c3->xmm9
                "movdqu %%xmm9,%2\n"          // save output
                "punpckhdq %%xmm10, %%xmm8\n" // d0,d1,d2,d3->xmm8
                "movdqu %%xmm8,%3\n"          // save output
                : "=m"(dest[0]), "=m"(dest[4]), "=m"(dest[8]), "=m"(dest[12])
                : "m"(encode[0])
                : "memory");
        __asm__("movdqu %4,%%xmm5\n"     //(encode[2],encode[3],encode[4],encode[5])->xmm5
                "psrldq $4,%%xmm5\n"     // shift encode[2] out of xmm5
                "movdqa %%xmm5,%%xmm6\n" // save for futrue aligned move
                "movdqa %%xmm5,%%xmm7\n"
                "movdqa %%xmm5,%%xmm8\n"
                "pshufb %%xmm0,%%xmm5\n" // shuffle 0,3,6,9:a0,b0,c0,d0->xmm5
                "pand %%xmm4,%%xmm5\n"
                "pshufb %%xmm1,%%xmm6\n" // shuffle 1,4,7,10:a1,b1,c1,d1->xmm6
                "psrld $6,%%xmm6\n"
                "pand %%xmm4,%%xmm6\n"
                "pshufb %%xmm2,%%xmm7\n" // shuffle 2,5,8,11:a2,b2,c2,d2->xmm7
                "psrld $4,%%xmm7\n"
                "pand %%xmm4,%%xmm7\n"
                "pshufb %%xmm3,%%xmm8\n" // shuffle 3,6,9,12:a3,b3,c3,d3->xmm8
                "psrld $2,%%xmm8\n"
                "pand %%xmm4,%%xmm8\n"
                "movdqa %%xmm5,%%xmm9\n"
                "movdqa %%xmm6,%%xmm10\n"
                "punpckldq %%xmm7, %%xmm5\n"  // a0,a2,b0,b2->xmm5
                "punpckhdq %%xmm7, %%xmm9\n"  // c0,c2,d0,d2->xmm9
                "punpckldq %%xmm8, %%xmm6\n"  // a1,a3,b1,b3->xmm6
                "punpckhdq %%xmm8, %%xmm10\n" // c1,c3,d1,d3->xmm10
                "movdqa %%xmm5, %%xmm7\n"
                "punpckldq %%xmm6, %%xmm5\n" // a0,a1,a2,a3->xmm5
                "movdqu %%xmm5,%0\n"         // save output
                "punpckhdq %%xmm6, %%xmm7\n" // b0,b1,b2,b3->xmm7
                "movdqu %%xmm7,%1\n"         // save output
                "movdqa %%xmm9, %%xmm8\n"
                "punpckldq %%xmm10, %%xmm9\n" // c0,c1,c2,c3->xmm9
                "movdqu %%xmm9,%2\n"          // save output
                "punpckhdq %%xmm10, %%xmm8\n" // d0,d1,d2,d3->xmm8
                "movdqu %%xmm8,%3\n"          // save output
                : "=m"(dest[16]), "=m"(dest[20]), "=m"(dest[24]), "=m"(dest[28])
                : "m"(encode[2])
                : "memory");
        // asm volatile("sfence" : : : "memory");
    }

    if (n & 0x1F) {
        unaligned_unpack_6(dest, encode, (n & 0x1F));
    }
}

#endif // INDEXLIB_OPTIMIZE_UNPACK

template <typename Type>
void unpack_7(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 7) {
        dest[0] = encode[0] & 0x7F;
        dest[1] = (encode[0] >> 7) & 0x7F;
        dest[2] = (encode[0] >> 14) & 0x7F;
        dest[3] = (encode[0] >> 21) & 0x7F;
        dest[4] = ((encode[0] >> 28) | (encode[1] << 4)) & 0x7F;
        dest[5] = (encode[1] >> 3) & 0x7F;
        dest[6] = (encode[1] >> 10) & 0x7F;
        dest[7] = (encode[1] >> 17) & 0x7F;
        dest[8] = (encode[1] >> 24) & 0x7F;
        dest[9] = ((encode[1] >> 31) | (encode[2] << 1)) & 0x7F;
        dest[10] = (encode[2] >> 6) & 0x7F;
        dest[11] = (encode[2] >> 13) & 0x7F;
        dest[12] = (encode[2] >> 20) & 0x7F;
        dest[13] = ((encode[2] >> 27) | (encode[3] << 5)) & 0x7F;
        dest[14] = (encode[3] >> 2) & 0x7F;
        dest[15] = (encode[3] >> 9) & 0x7F;
        dest[16] = (encode[3] >> 16) & 0x7F;
        dest[17] = (encode[3] >> 23) & 0x7F;
        dest[18] = ((encode[3] >> 30) | (encode[4] << 2)) & 0x7F;
        dest[19] = (encode[4] >> 5) & 0x7F;
        dest[20] = (encode[4] >> 12) & 0x7F;
        dest[21] = (encode[4] >> 19) & 0x7F;
        dest[22] = ((encode[4] >> 26) | (encode[5] << 6)) & 0x7F;
        dest[23] = (encode[5] >> 1) & 0x7F;
        dest[24] = (encode[5] >> 8) & 0x7F;
        dest[25] = (encode[5] >> 15) & 0x7F;
        dest[26] = (encode[5] >> 22) & 0x7F;
        dest[27] = ((encode[5] >> 29) | (encode[6] << 3)) & 0x7F;
        dest[28] = (encode[6] >> 4) & 0x7F;
        dest[29] = (encode[6] >> 11) & 0x7F;
        dest[30] = (encode[6] >> 18) & 0x7F;
        dest[31] = (encode[6] >> 25) & 0x7F;
    }

    if (n & 0x1F) {
        unaligned_unpack_7(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_8(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 8) {
        dest[0] = encode[0] & 0xFF;
        dest[1] = (encode[0] >> 8) & 0xFF;
        dest[2] = (encode[0] >> 16) & 0xFF;
        dest[3] = (encode[0] >> 24) & 0xFF;
        dest[4] = encode[1] & 0xFF;
        dest[5] = (encode[1] >> 8) & 0xFF;
        dest[6] = (encode[1] >> 16) & 0xFF;
        dest[7] = (encode[1] >> 24) & 0xFF;
        dest[8] = encode[2] & 0xFF;
        dest[9] = (encode[2] >> 8) & 0xFF;
        dest[10] = (encode[2] >> 16) & 0xFF;
        dest[11] = (encode[2] >> 24) & 0xFF;
        dest[12] = encode[3] & 0xFF;
        dest[13] = (encode[3] >> 8) & 0xFF;
        dest[14] = (encode[3] >> 16) & 0xFF;
        dest[15] = (encode[3] >> 24) & 0xFF;
        dest[16] = encode[4] & 0xFF;
        dest[17] = (encode[4] >> 8) & 0xFF;
        dest[18] = (encode[4] >> 16) & 0xFF;
        dest[19] = (encode[4] >> 24) & 0xFF;
        dest[20] = encode[5] & 0xFF;
        dest[21] = (encode[5] >> 8) & 0xFF;
        dest[22] = (encode[5] >> 16) & 0xFF;
        dest[23] = (encode[5] >> 24) & 0xFF;
        dest[24] = encode[6] & 0xFF;
        dest[25] = (encode[6] >> 8) & 0xFF;
        dest[26] = (encode[6] >> 16) & 0xFF;
        dest[27] = (encode[6] >> 24) & 0xFF;
        dest[28] = encode[7] & 0xFF;
        dest[29] = (encode[7] >> 8) & 0xFF;
        dest[30] = (encode[7] >> 16) & 0xFF;
        dest[31] = (encode[7] >> 24) & 0xFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_8(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_9(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 9) {
        dest[0] = encode[0] & 0x1FF;
        dest[1] = (encode[0] >> 9) & 0x1FF;
        dest[2] = (encode[0] >> 18) & 0x1FF;
        dest[3] = ((encode[0] >> 27) | (encode[1] << 5)) & 0x1FF;
        dest[4] = (encode[1] >> 4) & 0x1FF;
        dest[5] = (encode[1] >> 13) & 0x1FF;
        dest[6] = (encode[1] >> 22) & 0x1FF;
        dest[7] = ((encode[1] >> 31) | (encode[2] << 1)) & 0x1FF;
        dest[8] = (encode[2] >> 8) & 0x1FF;
        dest[9] = (encode[2] >> 17) & 0x1FF;
        dest[10] = ((encode[2] >> 26) | (encode[3] << 6)) & 0x1FF;
        dest[11] = (encode[3] >> 3) & 0x1FF;
        dest[12] = (encode[3] >> 12) & 0x1FF;
        dest[13] = (encode[3] >> 21) & 0x1FF;
        dest[14] = ((encode[3] >> 30) | (encode[4] << 2)) & 0x1FF;
        dest[15] = (encode[4] >> 7) & 0x1FF;
        dest[16] = (encode[4] >> 16) & 0x1FF;
        dest[17] = ((encode[4] >> 25) | (encode[5] << 7)) & 0x1FF;
        dest[18] = (encode[5] >> 2) & 0x1FF;
        dest[19] = (encode[5] >> 11) & 0x1FF;
        dest[20] = (encode[5] >> 20) & 0x1FF;
        dest[21] = ((encode[5] >> 29) | (encode[6] << 3)) & 0x1FF;
        dest[22] = (encode[6] >> 6) & 0x1FF;
        dest[23] = (encode[6] >> 15) & 0x1FF;
        dest[24] = ((encode[6] >> 24) | (encode[7] << 8)) & 0x1FF;
        dest[25] = (encode[7] >> 1) & 0x1FF;
        dest[26] = (encode[7] >> 10) & 0x1FF;
        dest[27] = (encode[7] >> 19) & 0x1FF;
        dest[28] = ((encode[7] >> 28) | (encode[8] << 4)) & 0x1FF;
        dest[29] = (encode[8] >> 5) & 0x1FF;
        dest[30] = (encode[8] >> 14) & 0x1FF;
        dest[31] = (encode[8] >> 23) & 0x1FF;
    }

    if (n & 0x1F) {
        unaligned_unpack_9(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_10(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 10) {
        dest[0] = encode[0] & 0x3FF;
        dest[1] = (encode[0] >> 10) & 0x3FF;
        dest[2] = (encode[0] >> 20) & 0x3FF;
        dest[3] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x3FF;
        dest[4] = (encode[1] >> 8) & 0x3FF;
        dest[5] = (encode[1] >> 18) & 0x3FF;
        dest[6] = ((encode[1] >> 28) | (encode[2] << 4)) & 0x3FF;
        dest[7] = (encode[2] >> 6) & 0x3FF;
        dest[8] = (encode[2] >> 16) & 0x3FF;
        dest[9] = ((encode[2] >> 26) | (encode[3] << 6)) & 0x3FF;
        dest[10] = (encode[3] >> 4) & 0x3FF;
        dest[11] = (encode[3] >> 14) & 0x3FF;
        dest[12] = ((encode[3] >> 24) | (encode[4] << 8)) & 0x3FF;
        dest[13] = (encode[4] >> 2) & 0x3FF;
        dest[14] = (encode[4] >> 12) & 0x3FF;
        dest[15] = (encode[4] >> 22) & 0x3FF;
        dest[16] = encode[5] & 0x3FF;
        dest[17] = (encode[5] >> 10) & 0x3FF;
        dest[18] = (encode[5] >> 20) & 0x3FF;
        dest[19] = ((encode[5] >> 30) | (encode[6] << 2)) & 0x3FF;
        dest[20] = (encode[6] >> 8) & 0x3FF;
        dest[21] = (encode[6] >> 18) & 0x3FF;
        dest[22] = ((encode[6] >> 28) | (encode[7] << 4)) & 0x3FF;
        dest[23] = (encode[7] >> 6) & 0x3FF;
        dest[24] = (encode[7] >> 16) & 0x3FF;
        dest[25] = ((encode[7] >> 26) | (encode[8] << 6)) & 0x3FF;
        dest[26] = (encode[8] >> 4) & 0x3FF;
        dest[27] = (encode[8] >> 14) & 0x3FF;
        dest[28] = ((encode[8] >> 24) | (encode[9] << 8)) & 0x3FF;
        dest[29] = (encode[9] >> 2) & 0x3FF;
        dest[30] = (encode[9] >> 12) & 0x3FF;
        dest[31] = (encode[9] >> 22) & 0x3FF;
    }

    if (n & 0x1F) {
        unaligned_unpack_10(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_11(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 11) {
        dest[0] = encode[0] & 0x7FF;
        dest[1] = (encode[0] >> 11) & 0x7FF;
        dest[2] = ((encode[0] >> 22) | (encode[1] << 10)) & 0x7FF;
        dest[3] = (encode[1] >> 1) & 0x7FF;
        dest[4] = (encode[1] >> 12) & 0x7FF;
        dest[5] = ((encode[1] >> 23) | (encode[2] << 9)) & 0x7FF;
        dest[6] = (encode[2] >> 2) & 0x7FF;
        dest[7] = (encode[2] >> 13) & 0x7FF;
        dest[8] = ((encode[2] >> 24) | (encode[3] << 8)) & 0x7FF;
        dest[9] = (encode[3] >> 3) & 0x7FF;
        dest[10] = (encode[3] >> 14) & 0x7FF;
        dest[11] = ((encode[3] >> 25) | (encode[4] << 7)) & 0x7FF;
        dest[12] = (encode[4] >> 4) & 0x7FF;
        dest[13] = (encode[4] >> 15) & 0x7FF;
        dest[14] = ((encode[4] >> 26) | (encode[5] << 6)) & 0x7FF;
        dest[15] = (encode[5] >> 5) & 0x7FF;
        dest[16] = (encode[5] >> 16) & 0x7FF;
        dest[17] = ((encode[5] >> 27) | (encode[6] << 5)) & 0x7FF;
        dest[18] = (encode[6] >> 6) & 0x7FF;
        dest[19] = (encode[6] >> 17) & 0x7FF;
        dest[20] = ((encode[6] >> 28) | (encode[7] << 4)) & 0x7FF;
        dest[21] = (encode[7] >> 7) & 0x7FF;
        dest[22] = (encode[7] >> 18) & 0x7FF;
        dest[23] = ((encode[7] >> 29) | (encode[8] << 3)) & 0x7FF;
        dest[24] = (encode[8] >> 8) & 0x7FF;
        dest[25] = (encode[8] >> 19) & 0x7FF;
        dest[26] = ((encode[8] >> 30) | (encode[9] << 2)) & 0x7FF;
        dest[27] = (encode[9] >> 9) & 0x7FF;
        dest[28] = (encode[9] >> 20) & 0x7FF;
        dest[29] = ((encode[9] >> 31) | (encode[10] << 1)) & 0x7FF;
        dest[30] = (encode[10] >> 10) & 0x7FF;
        dest[31] = (encode[10] >> 21) & 0x7FF;
    }

    if (n & 0x1F) {
        unaligned_unpack_11(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_12(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 12) {
        dest[0] = encode[0] & 0xFFF;
        dest[1] = (encode[0] >> 12) & 0xFFF;
        dest[2] = ((encode[0] >> 24) | (encode[1] << 8)) & 0xFFF;
        dest[3] = (encode[1] >> 4) & 0xFFF;
        dest[4] = (encode[1] >> 16) & 0xFFF;
        dest[5] = ((encode[1] >> 28) | (encode[2] << 4)) & 0xFFF;
        dest[6] = (encode[2] >> 8) & 0xFFF;
        dest[7] = (encode[2] >> 20) & 0xFFF;
        dest[8] = encode[3] & 0xFFF;
        dest[9] = (encode[3] >> 12) & 0xFFF;
        dest[10] = ((encode[3] >> 24) | (encode[4] << 8)) & 0xFFF;
        dest[11] = (encode[4] >> 4) & 0xFFF;
        dest[12] = (encode[4] >> 16) & 0xFFF;
        dest[13] = ((encode[4] >> 28) | (encode[5] << 4)) & 0xFFF;
        dest[14] = (encode[5] >> 8) & 0xFFF;
        dest[15] = (encode[5] >> 20) & 0xFFF;
        dest[16] = encode[6] & 0xFFF;
        dest[17] = (encode[6] >> 12) & 0xFFF;
        dest[18] = ((encode[6] >> 24) | (encode[7] << 8)) & 0xFFF;
        dest[19] = (encode[7] >> 4) & 0xFFF;
        dest[20] = (encode[7] >> 16) & 0xFFF;
        dest[21] = ((encode[7] >> 28) | (encode[8] << 4)) & 0xFFF;
        dest[22] = (encode[8] >> 8) & 0xFFF;
        dest[23] = (encode[8] >> 20) & 0xFFF;
        dest[24] = encode[9] & 0xFFF;
        dest[25] = (encode[9] >> 12) & 0xFFF;
        dest[26] = ((encode[9] >> 24) | (encode[10] << 8)) & 0xFFF;
        dest[27] = (encode[10] >> 4) & 0xFFF;
        dest[28] = (encode[10] >> 16) & 0xFFF;
        dest[29] = ((encode[10] >> 28) | (encode[11] << 4)) & 0xFFF;
        dest[30] = (encode[11] >> 8) & 0xFFF;
        dest[31] = (encode[11] >> 20) & 0xFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_12(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_13(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 13) {
        dest[0] = encode[0] & 0x1FFF;
        dest[1] = (encode[0] >> 13) & 0x1FFF;
        dest[2] = ((encode[0] >> 26) | (encode[1] << 6)) & 0x1FFF;
        dest[3] = (encode[1] >> 7) & 0x1FFF;
        dest[4] = ((encode[1] >> 20) | (encode[2] << 12)) & 0x1FFF;
        dest[5] = (encode[2] >> 1) & 0x1FFF;
        dest[6] = (encode[2] >> 14) & 0x1FFF;
        dest[7] = ((encode[2] >> 27) | (encode[3] << 5)) & 0x1FFF;
        dest[8] = (encode[3] >> 8) & 0x1FFF;
        dest[9] = ((encode[3] >> 21) | (encode[4] << 11)) & 0x1FFF;
        dest[10] = (encode[4] >> 2) & 0x1FFF;
        dest[11] = (encode[4] >> 15) & 0x1FFF;
        dest[12] = ((encode[4] >> 28) | (encode[5] << 4)) & 0x1FFF;
        dest[13] = (encode[5] >> 9) & 0x1FFF;
        dest[14] = ((encode[5] >> 22) | (encode[6] << 10)) & 0x1FFF;
        dest[15] = (encode[6] >> 3) & 0x1FFF;
        dest[16] = (encode[6] >> 16) & 0x1FFF;
        dest[17] = ((encode[6] >> 29) | (encode[7] << 3)) & 0x1FFF;
        dest[18] = (encode[7] >> 10) & 0x1FFF;
        dest[19] = ((encode[7] >> 23) | (encode[8] << 9)) & 0x1FFF;
        dest[20] = (encode[8] >> 4) & 0x1FFF;
        dest[21] = (encode[8] >> 17) & 0x1FFF;
        dest[22] = ((encode[8] >> 30) | (encode[9] << 2)) & 0x1FFF;
        dest[23] = (encode[9] >> 11) & 0x1FFF;
        dest[24] = ((encode[9] >> 24) | (encode[10] << 8)) & 0x1FFF;
        dest[25] = (encode[10] >> 5) & 0x1FFF;
        dest[26] = (encode[10] >> 18) & 0x1FFF;
        dest[27] = ((encode[10] >> 31) | (encode[11] << 1)) & 0x1FFF;
        dest[28] = (encode[11] >> 12) & 0x1FFF;
        dest[29] = ((encode[11] >> 25) | (encode[12] << 7)) & 0x1FFF;
        dest[30] = (encode[12] >> 6) & 0x1FFF;
        dest[31] = (encode[12] >> 19) & 0x1FFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_13(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_14(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 14) {
        dest[0] = encode[0] & 0x3FFF;
        dest[1] = (encode[0] >> 14) & 0x3FFF;
        dest[2] = ((encode[0] >> 28) | (encode[1] << 4)) & 0x3FFF;
        dest[3] = (encode[1] >> 10) & 0x3FFF;
        dest[4] = ((encode[1] >> 24) | (encode[2] << 8)) & 0x3FFF;
        dest[5] = (encode[2] >> 6) & 0x3FFF;
        dest[6] = ((encode[2] >> 20) | (encode[3] << 12)) & 0x3FFF;
        dest[7] = (encode[3] >> 2) & 0x3FFF;
        dest[8] = (encode[3] >> 16) & 0x3FFF;
        dest[9] = ((encode[3] >> 30) | (encode[4] << 2)) & 0x3FFF;
        dest[10] = (encode[4] >> 12) & 0x3FFF;
        dest[11] = ((encode[4] >> 26) | (encode[5] << 6)) & 0x3FFF;
        dest[12] = (encode[5] >> 8) & 0x3FFF;
        dest[13] = ((encode[5] >> 22) | (encode[6] << 10)) & 0x3FFF;
        dest[14] = (encode[6] >> 4) & 0x3FFF;
        dest[15] = (encode[6] >> 18) & 0x3FFF;
        dest[16] = encode[7] & 0x3FFF;
        dest[17] = (encode[7] >> 14) & 0x3FFF;
        dest[18] = ((encode[7] >> 28) | (encode[8] << 4)) & 0x3FFF;
        dest[19] = (encode[8] >> 10) & 0x3FFF;
        dest[20] = ((encode[8] >> 24) | (encode[9] << 8)) & 0x3FFF;
        dest[21] = (encode[9] >> 6) & 0x3FFF;
        dest[22] = ((encode[9] >> 20) | (encode[10] << 12)) & 0x3FFF;
        dest[23] = (encode[10] >> 2) & 0x3FFF;
        dest[24] = (encode[10] >> 16) & 0x3FFF;
        dest[25] = ((encode[10] >> 30) | (encode[11] << 2)) & 0x3FFF;
        dest[26] = (encode[11] >> 12) & 0x3FFF;
        dest[27] = ((encode[11] >> 26) | (encode[12] << 6)) & 0x3FFF;
        dest[28] = (encode[12] >> 8) & 0x3FFF;
        dest[29] = ((encode[12] >> 22) | (encode[13] << 10)) & 0x3FFF;
        dest[30] = (encode[13] >> 4) & 0x3FFF;
        dest[31] = (encode[13] >> 18) & 0x3FFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_14(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_15(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 15) {
        dest[0] = encode[0] & 0x7FFF;
        dest[1] = (encode[0] >> 15) & 0x7FFF;
        dest[2] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x7FFF;
        dest[3] = (encode[1] >> 13) & 0x7FFF;
        dest[4] = ((encode[1] >> 28) | (encode[2] << 4)) & 0x7FFF;
        dest[5] = (encode[2] >> 11) & 0x7FFF;
        dest[6] = ((encode[2] >> 26) | (encode[3] << 6)) & 0x7FFF;
        dest[7] = (encode[3] >> 9) & 0x7FFF;
        dest[8] = ((encode[3] >> 24) | (encode[4] << 8)) & 0x7FFF;
        dest[9] = (encode[4] >> 7) & 0x7FFF;
        dest[10] = ((encode[4] >> 22) | (encode[5] << 10)) & 0x7FFF;
        dest[11] = (encode[5] >> 5) & 0x7FFF;
        dest[12] = ((encode[5] >> 20) | (encode[6] << 12)) & 0x7FFF;
        dest[13] = (encode[6] >> 3) & 0x7FFF;
        dest[14] = ((encode[6] >> 18) | (encode[7] << 14)) & 0x7FFF;
        dest[15] = (encode[7] >> 1) & 0x7FFF;
        dest[16] = (encode[7] >> 16) & 0x7FFF;
        dest[17] = ((encode[7] >> 31) | (encode[8] << 1)) & 0x7FFF;
        dest[18] = (encode[8] >> 14) & 0x7FFF;
        dest[19] = ((encode[8] >> 29) | (encode[9] << 3)) & 0x7FFF;
        dest[20] = (encode[9] >> 12) & 0x7FFF;
        dest[21] = ((encode[9] >> 27) | (encode[10] << 5)) & 0x7FFF;
        dest[22] = (encode[10] >> 10) & 0x7FFF;
        dest[23] = ((encode[10] >> 25) | (encode[11] << 7)) & 0x7FFF;
        dest[24] = (encode[11] >> 8) & 0x7FFF;
        dest[25] = ((encode[11] >> 23) | (encode[12] << 9)) & 0x7FFF;
        dest[26] = (encode[12] >> 6) & 0x7FFF;
        dest[27] = ((encode[12] >> 21) | (encode[13] << 11)) & 0x7FFF;
        dest[28] = (encode[13] >> 4) & 0x7FFF;
        dest[29] = ((encode[13] >> 19) | (encode[14] << 13)) & 0x7FFF;
        dest[30] = (encode[14] >> 2) & 0x7FFF;
        dest[31] = (encode[14] >> 17) & 0x7FFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_15(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_16(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 16) {
        dest[0] = encode[0] & 0xFFFF;
        dest[1] = (encode[0] >> 16) & 0xFFFF;
        dest[2] = encode[1] & 0xFFFF;
        dest[3] = (encode[1] >> 16) & 0xFFFF;
        dest[4] = encode[2] & 0xFFFF;
        dest[5] = (encode[2] >> 16) & 0xFFFF;
        dest[6] = encode[3] & 0xFFFF;
        dest[7] = (encode[3] >> 16) & 0xFFFF;
        dest[8] = encode[4] & 0xFFFF;
        dest[9] = (encode[4] >> 16) & 0xFFFF;
        dest[10] = encode[5] & 0xFFFF;
        dest[11] = (encode[5] >> 16) & 0xFFFF;
        dest[12] = encode[6] & 0xFFFF;
        dest[13] = (encode[6] >> 16) & 0xFFFF;
        dest[14] = encode[7] & 0xFFFF;
        dest[15] = (encode[7] >> 16) & 0xFFFF;
        dest[16] = encode[8] & 0xFFFF;
        dest[17] = (encode[8] >> 16) & 0xFFFF;
        dest[18] = encode[9] & 0xFFFF;
        dest[19] = (encode[9] >> 16) & 0xFFFF;
        dest[20] = encode[10] & 0xFFFF;
        dest[21] = (encode[10] >> 16) & 0xFFFF;
        dest[22] = encode[11] & 0xFFFF;
        dest[23] = (encode[11] >> 16) & 0xFFFF;
        dest[24] = encode[12] & 0xFFFF;
        dest[25] = (encode[12] >> 16) & 0xFFFF;
        dest[26] = encode[13] & 0xFFFF;
        dest[27] = (encode[13] >> 16) & 0xFFFF;
        dest[28] = encode[14] & 0xFFFF;
        dest[29] = (encode[14] >> 16) & 0xFFFF;
        dest[30] = encode[15] & 0xFFFF;
        dest[31] = (encode[15] >> 16) & 0xFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_16(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_17(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 17) {
        dest[0] = encode[0] & 0x1FFFF;
        dest[1] = ((encode[0] >> 17) | (encode[1] << 15)) & 0x1FFFF;
        dest[2] = (encode[1] >> 2) & 0x1FFFF;
        dest[3] = ((encode[1] >> 19) | (encode[2] << 13)) & 0x1FFFF;
        dest[4] = (encode[2] >> 4) & 0x1FFFF;
        dest[5] = ((encode[2] >> 21) | (encode[3] << 11)) & 0x1FFFF;
        dest[6] = (encode[3] >> 6) & 0x1FFFF;
        dest[7] = ((encode[3] >> 23) | (encode[4] << 9)) & 0x1FFFF;
        dest[8] = (encode[4] >> 8) & 0x1FFFF;
        dest[9] = ((encode[4] >> 25) | (encode[5] << 7)) & 0x1FFFF;
        dest[10] = (encode[5] >> 10) & 0x1FFFF;
        dest[11] = ((encode[5] >> 27) | (encode[6] << 5)) & 0x1FFFF;
        dest[12] = (encode[6] >> 12) & 0x1FFFF;
        dest[13] = ((encode[6] >> 29) | (encode[7] << 3)) & 0x1FFFF;
        dest[14] = (encode[7] >> 14) & 0x1FFFF;
        dest[15] = ((encode[7] >> 31) | (encode[8] << 1)) & 0x1FFFF;
        dest[16] = ((encode[8] >> 16) | (encode[9] << 16)) & 0x1FFFF;
        dest[17] = (encode[9] >> 1) & 0x1FFFF;
        dest[18] = ((encode[9] >> 18) | (encode[10] << 14)) & 0x1FFFF;
        dest[19] = (encode[10] >> 3) & 0x1FFFF;
        dest[20] = ((encode[10] >> 20) | (encode[11] << 12)) & 0x1FFFF;
        dest[21] = (encode[11] >> 5) & 0x1FFFF;
        dest[22] = ((encode[11] >> 22) | (encode[12] << 10)) & 0x1FFFF;
        dest[23] = (encode[12] >> 7) & 0x1FFFF;
        dest[24] = ((encode[12] >> 24) | (encode[13] << 8)) & 0x1FFFF;
        dest[25] = (encode[13] >> 9) & 0x1FFFF;
        dest[26] = ((encode[13] >> 26) | (encode[14] << 6)) & 0x1FFFF;
        dest[27] = (encode[14] >> 11) & 0x1FFFF;
        dest[28] = ((encode[14] >> 28) | (encode[15] << 4)) & 0x1FFFF;
        dest[29] = (encode[15] >> 13) & 0x1FFFF;
        dest[30] = ((encode[15] >> 30) | (encode[16] << 2)) & 0x1FFFF;
        dest[31] = (encode[16] >> 15) & 0x1FFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_17(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_18(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 18) {
        dest[0] = encode[0] & 0x3FFFF;
        dest[1] = ((encode[0] >> 18) | (encode[1] << 14)) & 0x3FFFF;
        dest[2] = (encode[1] >> 4) & 0x3FFFF;
        dest[3] = ((encode[1] >> 22) | (encode[2] << 10)) & 0x3FFFF;
        dest[4] = (encode[2] >> 8) & 0x3FFFF;
        dest[5] = ((encode[2] >> 26) | (encode[3] << 6)) & 0x3FFFF;
        dest[6] = (encode[3] >> 12) & 0x3FFFF;
        dest[7] = ((encode[3] >> 30) | (encode[4] << 2)) & 0x3FFFF;
        dest[8] = ((encode[4] >> 16) | (encode[5] << 16)) & 0x3FFFF;
        dest[9] = (encode[5] >> 2) & 0x3FFFF;
        dest[10] = ((encode[5] >> 20) | (encode[6] << 12)) & 0x3FFFF;
        dest[11] = (encode[6] >> 6) & 0x3FFFF;
        dest[12] = ((encode[6] >> 24) | (encode[7] << 8)) & 0x3FFFF;
        dest[13] = (encode[7] >> 10) & 0x3FFFF;
        dest[14] = ((encode[7] >> 28) | (encode[8] << 4)) & 0x3FFFF;
        dest[15] = (encode[8] >> 14) & 0x3FFFF;
        dest[16] = encode[9] & 0x3FFFF;
        dest[17] = ((encode[9] >> 18) | (encode[10] << 14)) & 0x3FFFF;
        dest[18] = (encode[10] >> 4) & 0x3FFFF;
        dest[19] = ((encode[10] >> 22) | (encode[11] << 10)) & 0x3FFFF;
        dest[20] = (encode[11] >> 8) & 0x3FFFF;
        dest[21] = ((encode[11] >> 26) | (encode[12] << 6)) & 0x3FFFF;
        dest[22] = (encode[12] >> 12) & 0x3FFFF;
        dest[23] = ((encode[12] >> 30) | (encode[13] << 2)) & 0x3FFFF;
        dest[24] = ((encode[13] >> 16) | (encode[14] << 16)) & 0x3FFFF;
        dest[25] = (encode[14] >> 2) & 0x3FFFF;
        dest[26] = ((encode[14] >> 20) | (encode[15] << 12)) & 0x3FFFF;
        dest[27] = (encode[15] >> 6) & 0x3FFFF;
        dest[28] = ((encode[15] >> 24) | (encode[16] << 8)) & 0x3FFFF;
        dest[29] = (encode[16] >> 10) & 0x3FFFF;
        dest[30] = ((encode[16] >> 28) | (encode[17] << 4)) & 0x3FFFF;
        dest[31] = (encode[17] >> 14) & 0x3FFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_18(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_19(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 19) {
        dest[0] = encode[0] & 0x7FFFF;
        dest[1] = ((encode[0] >> 19) | (encode[1] << 13)) & 0x7FFFF;
        dest[2] = (encode[1] >> 6) & 0x7FFFF;
        dest[3] = ((encode[1] >> 25) | (encode[2] << 7)) & 0x7FFFF;
        dest[4] = (encode[2] >> 12) & 0x7FFFF;
        dest[5] = ((encode[2] >> 31) | (encode[3] << 1)) & 0x7FFFF;
        dest[6] = ((encode[3] >> 18) | (encode[4] << 14)) & 0x7FFFF;
        dest[7] = (encode[4] >> 5) & 0x7FFFF;
        dest[8] = ((encode[4] >> 24) | (encode[5] << 8)) & 0x7FFFF;
        dest[9] = (encode[5] >> 11) & 0x7FFFF;
        dest[10] = ((encode[5] >> 30) | (encode[6] << 2)) & 0x7FFFF;
        dest[11] = ((encode[6] >> 17) | (encode[7] << 15)) & 0x7FFFF;
        dest[12] = (encode[7] >> 4) & 0x7FFFF;
        dest[13] = ((encode[7] >> 23) | (encode[8] << 9)) & 0x7FFFF;
        dest[14] = (encode[8] >> 10) & 0x7FFFF;
        dest[15] = ((encode[8] >> 29) | (encode[9] << 3)) & 0x7FFFF;
        dest[16] = ((encode[9] >> 16) | (encode[10] << 16)) & 0x7FFFF;
        dest[17] = (encode[10] >> 3) & 0x7FFFF;
        dest[18] = ((encode[10] >> 22) | (encode[11] << 10)) & 0x7FFFF;
        dest[19] = (encode[11] >> 9) & 0x7FFFF;
        dest[20] = ((encode[11] >> 28) | (encode[12] << 4)) & 0x7FFFF;
        dest[21] = ((encode[12] >> 15) | (encode[13] << 17)) & 0x7FFFF;
        dest[22] = (encode[13] >> 2) & 0x7FFFF;
        dest[23] = ((encode[13] >> 21) | (encode[14] << 11)) & 0x7FFFF;
        dest[24] = (encode[14] >> 8) & 0x7FFFF;
        dest[25] = ((encode[14] >> 27) | (encode[15] << 5)) & 0x7FFFF;
        dest[26] = ((encode[15] >> 14) | (encode[16] << 18)) & 0x7FFFF;
        dest[27] = (encode[16] >> 1) & 0x7FFFF;
        dest[28] = ((encode[16] >> 20) | (encode[17] << 12)) & 0x7FFFF;
        dest[29] = (encode[17] >> 7) & 0x7FFFF;
        dest[30] = ((encode[17] >> 26) | (encode[18] << 6)) & 0x7FFFF;
        dest[31] = (encode[18] >> 13) & 0x7FFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_19(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_20(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 20) {
        dest[0] = encode[0] & 0xFFFFF;
        dest[1] = ((encode[0] >> 20) | (encode[1] << 12)) & 0xFFFFF;
        dest[2] = (encode[1] >> 8) & 0xFFFFF;
        dest[3] = ((encode[1] >> 28) | (encode[2] << 4)) & 0xFFFFF;
        dest[4] = ((encode[2] >> 16) | (encode[3] << 16)) & 0xFFFFF;
        dest[5] = (encode[3] >> 4) & 0xFFFFF;
        dest[6] = ((encode[3] >> 24) | (encode[4] << 8)) & 0xFFFFF;
        dest[7] = (encode[4] >> 12) & 0xFFFFF;
        dest[8] = encode[5] & 0xFFFFF;
        dest[9] = ((encode[5] >> 20) | (encode[6] << 12)) & 0xFFFFF;
        dest[10] = (encode[6] >> 8) & 0xFFFFF;
        dest[11] = ((encode[6] >> 28) | (encode[7] << 4)) & 0xFFFFF;
        dest[12] = ((encode[7] >> 16) | (encode[8] << 16)) & 0xFFFFF;
        dest[13] = (encode[8] >> 4) & 0xFFFFF;
        dest[14] = ((encode[8] >> 24) | (encode[9] << 8)) & 0xFFFFF;
        dest[15] = (encode[9] >> 12) & 0xFFFFF;
        dest[16] = encode[10] & 0xFFFFF;
        dest[17] = ((encode[10] >> 20) | (encode[11] << 12)) & 0xFFFFF;
        dest[18] = (encode[11] >> 8) & 0xFFFFF;
        dest[19] = ((encode[11] >> 28) | (encode[12] << 4)) & 0xFFFFF;
        dest[20] = ((encode[12] >> 16) | (encode[13] << 16)) & 0xFFFFF;
        dest[21] = (encode[13] >> 4) & 0xFFFFF;
        dest[22] = ((encode[13] >> 24) | (encode[14] << 8)) & 0xFFFFF;
        dest[23] = (encode[14] >> 12) & 0xFFFFF;
        dest[24] = encode[15] & 0xFFFFF;
        dest[25] = ((encode[15] >> 20) | (encode[16] << 12)) & 0xFFFFF;
        dest[26] = (encode[16] >> 8) & 0xFFFFF;
        dest[27] = ((encode[16] >> 28) | (encode[17] << 4)) & 0xFFFFF;
        dest[28] = ((encode[17] >> 16) | (encode[18] << 16)) & 0xFFFFF;
        dest[29] = (encode[18] >> 4) & 0xFFFFF;
        dest[30] = ((encode[18] >> 24) | (encode[19] << 8)) & 0xFFFFF;
        dest[31] = (encode[19] >> 12) & 0xFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_20(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_21(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 21) {
        dest[0] = encode[0] & 0x1FFFFF;
        dest[1] = ((encode[0] >> 21) | (encode[1] << 11)) & 0x1FFFFF;
        dest[2] = (encode[1] >> 10) & 0x1FFFFF;
        dest[3] = ((encode[1] >> 31) | (encode[2] << 1)) & 0x1FFFFF;
        dest[4] = ((encode[2] >> 20) | (encode[3] << 12)) & 0x1FFFFF;
        dest[5] = (encode[3] >> 9) & 0x1FFFFF;
        dest[6] = ((encode[3] >> 30) | (encode[4] << 2)) & 0x1FFFFF;
        dest[7] = ((encode[4] >> 19) | (encode[5] << 13)) & 0x1FFFFF;
        dest[8] = (encode[5] >> 8) & 0x1FFFFF;
        dest[9] = ((encode[5] >> 29) | (encode[6] << 3)) & 0x1FFFFF;
        dest[10] = ((encode[6] >> 18) | (encode[7] << 14)) & 0x1FFFFF;
        dest[11] = (encode[7] >> 7) & 0x1FFFFF;
        dest[12] = ((encode[7] >> 28) | (encode[8] << 4)) & 0x1FFFFF;
        dest[13] = ((encode[8] >> 17) | (encode[9] << 15)) & 0x1FFFFF;
        dest[14] = (encode[9] >> 6) & 0x1FFFFF;
        dest[15] = ((encode[9] >> 27) | (encode[10] << 5)) & 0x1FFFFF;
        dest[16] = ((encode[10] >> 16) | (encode[11] << 16)) & 0x1FFFFF;
        dest[17] = (encode[11] >> 5) & 0x1FFFFF;
        dest[18] = ((encode[11] >> 26) | (encode[12] << 6)) & 0x1FFFFF;
        dest[19] = ((encode[12] >> 15) | (encode[13] << 17)) & 0x1FFFFF;
        dest[20] = (encode[13] >> 4) & 0x1FFFFF;
        dest[21] = ((encode[13] >> 25) | (encode[14] << 7)) & 0x1FFFFF;
        dest[22] = ((encode[14] >> 14) | (encode[15] << 18)) & 0x1FFFFF;
        dest[23] = (encode[15] >> 3) & 0x1FFFFF;
        dest[24] = ((encode[15] >> 24) | (encode[16] << 8)) & 0x1FFFFF;
        dest[25] = ((encode[16] >> 13) | (encode[17] << 19)) & 0x1FFFFF;
        dest[26] = (encode[17] >> 2) & 0x1FFFFF;
        dest[27] = ((encode[17] >> 23) | (encode[18] << 9)) & 0x1FFFFF;
        dest[28] = ((encode[18] >> 12) | (encode[19] << 20)) & 0x1FFFFF;
        dest[29] = (encode[19] >> 1) & 0x1FFFFF;
        dest[30] = ((encode[19] >> 22) | (encode[20] << 10)) & 0x1FFFFF;
        dest[31] = (encode[20] >> 11) & 0x1FFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_21(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_22(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 22) {
        dest[0] = encode[0] & 0x3FFFFF;
        dest[1] = ((encode[0] >> 22) | (encode[1] << 10)) & 0x3FFFFF;
        dest[2] = ((encode[1] >> 12) | (encode[2] << 20)) & 0x3FFFFF;
        dest[3] = (encode[2] >> 2) & 0x3FFFFF;
        dest[4] = ((encode[2] >> 24) | (encode[3] << 8)) & 0x3FFFFF;
        dest[5] = ((encode[3] >> 14) | (encode[4] << 18)) & 0x3FFFFF;
        dest[6] = (encode[4] >> 4) & 0x3FFFFF;
        dest[7] = ((encode[4] >> 26) | (encode[5] << 6)) & 0x3FFFFF;
        dest[8] = ((encode[5] >> 16) | (encode[6] << 16)) & 0x3FFFFF;
        dest[9] = (encode[6] >> 6) & 0x3FFFFF;
        dest[10] = ((encode[6] >> 28) | (encode[7] << 4)) & 0x3FFFFF;
        dest[11] = ((encode[7] >> 18) | (encode[8] << 14)) & 0x3FFFFF;
        dest[12] = (encode[8] >> 8) & 0x3FFFFF;
        dest[13] = ((encode[8] >> 30) | (encode[9] << 2)) & 0x3FFFFF;
        dest[14] = ((encode[9] >> 20) | (encode[10] << 12)) & 0x3FFFFF;
        dest[15] = (encode[10] >> 10) & 0x3FFFFF;
        dest[16] = encode[11] & 0x3FFFFF;
        dest[17] = ((encode[11] >> 22) | (encode[12] << 10)) & 0x3FFFFF;
        dest[18] = ((encode[12] >> 12) | (encode[13] << 20)) & 0x3FFFFF;
        dest[19] = (encode[13] >> 2) & 0x3FFFFF;
        dest[20] = ((encode[13] >> 24) | (encode[14] << 8)) & 0x3FFFFF;
        dest[21] = ((encode[14] >> 14) | (encode[15] << 18)) & 0x3FFFFF;
        dest[22] = (encode[15] >> 4) & 0x3FFFFF;
        dest[23] = ((encode[15] >> 26) | (encode[16] << 6)) & 0x3FFFFF;
        dest[24] = ((encode[16] >> 16) | (encode[17] << 16)) & 0x3FFFFF;
        dest[25] = (encode[17] >> 6) & 0x3FFFFF;
        dest[26] = ((encode[17] >> 28) | (encode[18] << 4)) & 0x3FFFFF;
        dest[27] = ((encode[18] >> 18) | (encode[19] << 14)) & 0x3FFFFF;
        dest[28] = (encode[19] >> 8) & 0x3FFFFF;
        dest[29] = ((encode[19] >> 30) | (encode[20] << 2)) & 0x3FFFFF;
        dest[30] = ((encode[20] >> 20) | (encode[21] << 12)) & 0x3FFFFF;
        dest[31] = (encode[21] >> 10) & 0x3FFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_22(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_23(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 23) {
        dest[0] = encode[0] & 0x7FFFFF;
        dest[1] = ((encode[0] >> 23) | (encode[1] << 9)) & 0x7FFFFF;
        dest[2] = ((encode[1] >> 14) | (encode[2] << 18)) & 0x7FFFFF;
        dest[3] = (encode[2] >> 5) & 0x7FFFFF;
        dest[4] = ((encode[2] >> 28) | (encode[3] << 4)) & 0x7FFFFF;
        dest[5] = ((encode[3] >> 19) | (encode[4] << 13)) & 0x7FFFFF;
        dest[6] = ((encode[4] >> 10) | (encode[5] << 22)) & 0x7FFFFF;
        dest[7] = (encode[5] >> 1) & 0x7FFFFF;
        dest[8] = ((encode[5] >> 24) | (encode[6] << 8)) & 0x7FFFFF;
        dest[9] = ((encode[6] >> 15) | (encode[7] << 17)) & 0x7FFFFF;
        dest[10] = (encode[7] >> 6) & 0x7FFFFF;
        dest[11] = ((encode[7] >> 29) | (encode[8] << 3)) & 0x7FFFFF;
        dest[12] = ((encode[8] >> 20) | (encode[9] << 12)) & 0x7FFFFF;
        dest[13] = ((encode[9] >> 11) | (encode[10] << 21)) & 0x7FFFFF;
        dest[14] = (encode[10] >> 2) & 0x7FFFFF;
        dest[15] = ((encode[10] >> 25) | (encode[11] << 7)) & 0x7FFFFF;
        dest[16] = ((encode[11] >> 16) | (encode[12] << 16)) & 0x7FFFFF;
        dest[17] = (encode[12] >> 7) & 0x7FFFFF;
        dest[18] = ((encode[12] >> 30) | (encode[13] << 2)) & 0x7FFFFF;
        dest[19] = ((encode[13] >> 21) | (encode[14] << 11)) & 0x7FFFFF;
        dest[20] = ((encode[14] >> 12) | (encode[15] << 20)) & 0x7FFFFF;
        dest[21] = (encode[15] >> 3) & 0x7FFFFF;
        dest[22] = ((encode[15] >> 26) | (encode[16] << 6)) & 0x7FFFFF;
        dest[23] = ((encode[16] >> 17) | (encode[17] << 15)) & 0x7FFFFF;
        dest[24] = (encode[17] >> 8) & 0x7FFFFF;
        dest[25] = ((encode[17] >> 31) | (encode[18] << 1)) & 0x7FFFFF;
        dest[26] = ((encode[18] >> 22) | (encode[19] << 10)) & 0x7FFFFF;
        dest[27] = ((encode[19] >> 13) | (encode[20] << 19)) & 0x7FFFFF;
        dest[28] = (encode[20] >> 4) & 0x7FFFFF;
        dest[29] = ((encode[20] >> 27) | (encode[21] << 5)) & 0x7FFFFF;
        dest[30] = ((encode[21] >> 18) | (encode[22] << 14)) & 0x7FFFFF;
        dest[31] = (encode[22] >> 9) & 0x7FFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_23(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_24(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 24) {
        dest[0] = encode[0] & 0xFFFFFF;
        dest[1] = ((encode[0] >> 24) | (encode[1] << 8)) & 0xFFFFFF;
        dest[2] = ((encode[1] >> 16) | (encode[2] << 16)) & 0xFFFFFF;
        dest[3] = (encode[2] >> 8) & 0xFFFFFF;
        dest[4] = encode[3] & 0xFFFFFF;
        dest[5] = ((encode[3] >> 24) | (encode[4] << 8)) & 0xFFFFFF;
        dest[6] = ((encode[4] >> 16) | (encode[5] << 16)) & 0xFFFFFF;
        dest[7] = (encode[5] >> 8) & 0xFFFFFF;
        dest[8] = encode[6] & 0xFFFFFF;
        dest[9] = ((encode[6] >> 24) | (encode[7] << 8)) & 0xFFFFFF;
        dest[10] = ((encode[7] >> 16) | (encode[8] << 16)) & 0xFFFFFF;
        dest[11] = (encode[8] >> 8) & 0xFFFFFF;
        dest[12] = encode[9] & 0xFFFFFF;
        dest[13] = ((encode[9] >> 24) | (encode[10] << 8)) & 0xFFFFFF;
        dest[14] = ((encode[10] >> 16) | (encode[11] << 16)) & 0xFFFFFF;
        dest[15] = (encode[11] >> 8) & 0xFFFFFF;
        dest[16] = encode[12] & 0xFFFFFF;
        dest[17] = ((encode[12] >> 24) | (encode[13] << 8)) & 0xFFFFFF;
        dest[18] = ((encode[13] >> 16) | (encode[14] << 16)) & 0xFFFFFF;
        dest[19] = (encode[14] >> 8) & 0xFFFFFF;
        dest[20] = encode[15] & 0xFFFFFF;
        dest[21] = ((encode[15] >> 24) | (encode[16] << 8)) & 0xFFFFFF;
        dest[22] = ((encode[16] >> 16) | (encode[17] << 16)) & 0xFFFFFF;
        dest[23] = (encode[17] >> 8) & 0xFFFFFF;
        dest[24] = encode[18] & 0xFFFFFF;
        dest[25] = ((encode[18] >> 24) | (encode[19] << 8)) & 0xFFFFFF;
        dest[26] = ((encode[19] >> 16) | (encode[20] << 16)) & 0xFFFFFF;
        dest[27] = (encode[20] >> 8) & 0xFFFFFF;
        dest[28] = encode[21] & 0xFFFFFF;
        dest[29] = ((encode[21] >> 24) | (encode[22] << 8)) & 0xFFFFFF;
        dest[30] = ((encode[22] >> 16) | (encode[23] << 16)) & 0xFFFFFF;
        dest[31] = (encode[23] >> 8) & 0xFFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_24(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_25(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 25) {
        dest[0] = encode[0] & 0x1FFFFFF;
        dest[1] = ((encode[0] >> 25) | (encode[1] << 7)) & 0x1FFFFFF;
        dest[2] = ((encode[1] >> 18) | (encode[2] << 14)) & 0x1FFFFFF;
        dest[3] = ((encode[2] >> 11) | (encode[3] << 21)) & 0x1FFFFFF;
        dest[4] = (encode[3] >> 4) & 0x1FFFFFF;
        dest[5] = ((encode[3] >> 29) | (encode[4] << 3)) & 0x1FFFFFF;
        dest[6] = ((encode[4] >> 22) | (encode[5] << 10)) & 0x1FFFFFF;
        dest[7] = ((encode[5] >> 15) | (encode[6] << 17)) & 0x1FFFFFF;
        dest[8] = ((encode[6] >> 8) | (encode[7] << 24)) & 0x1FFFFFF;
        dest[9] = (encode[7] >> 1) & 0x1FFFFFF;
        dest[10] = ((encode[7] >> 26) | (encode[8] << 6)) & 0x1FFFFFF;
        dest[11] = ((encode[8] >> 19) | (encode[9] << 13)) & 0x1FFFFFF;
        dest[12] = ((encode[9] >> 12) | (encode[10] << 20)) & 0x1FFFFFF;
        dest[13] = (encode[10] >> 5) & 0x1FFFFFF;
        dest[14] = ((encode[10] >> 30) | (encode[11] << 2)) & 0x1FFFFFF;
        dest[15] = ((encode[11] >> 23) | (encode[12] << 9)) & 0x1FFFFFF;
        dest[16] = ((encode[12] >> 16) | (encode[13] << 16)) & 0x1FFFFFF;
        dest[17] = ((encode[13] >> 9) | (encode[14] << 23)) & 0x1FFFFFF;
        dest[18] = (encode[14] >> 2) & 0x1FFFFFF;
        dest[19] = ((encode[14] >> 27) | (encode[15] << 5)) & 0x1FFFFFF;
        dest[20] = ((encode[15] >> 20) | (encode[16] << 12)) & 0x1FFFFFF;
        dest[21] = ((encode[16] >> 13) | (encode[17] << 19)) & 0x1FFFFFF;
        dest[22] = (encode[17] >> 6) & 0x1FFFFFF;
        dest[23] = ((encode[17] >> 31) | (encode[18] << 1)) & 0x1FFFFFF;
        dest[24] = ((encode[18] >> 24) | (encode[19] << 8)) & 0x1FFFFFF;
        dest[25] = ((encode[19] >> 17) | (encode[20] << 15)) & 0x1FFFFFF;
        dest[26] = ((encode[20] >> 10) | (encode[21] << 22)) & 0x1FFFFFF;
        dest[27] = (encode[21] >> 3) & 0x1FFFFFF;
        dest[28] = ((encode[21] >> 28) | (encode[22] << 4)) & 0x1FFFFFF;
        dest[29] = ((encode[22] >> 21) | (encode[23] << 11)) & 0x1FFFFFF;
        dest[30] = ((encode[23] >> 14) | (encode[24] << 18)) & 0x1FFFFFF;
        dest[31] = (encode[24] >> 7) & 0x1FFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_25(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_26(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 26) {
        dest[0] = encode[0] & 0x3FFFFFF;
        dest[1] = ((encode[0] >> 26) | (encode[1] << 6)) & 0x3FFFFFF;
        dest[2] = ((encode[1] >> 20) | (encode[2] << 12)) & 0x3FFFFFF;
        dest[3] = ((encode[2] >> 14) | (encode[3] << 18)) & 0x3FFFFFF;
        dest[4] = ((encode[3] >> 8) | (encode[4] << 24)) & 0x3FFFFFF;
        dest[5] = (encode[4] >> 2) & 0x3FFFFFF;
        dest[6] = ((encode[4] >> 28) | (encode[5] << 4)) & 0x3FFFFFF;
        dest[7] = ((encode[5] >> 22) | (encode[6] << 10)) & 0x3FFFFFF;
        dest[8] = ((encode[6] >> 16) | (encode[7] << 16)) & 0x3FFFFFF;
        dest[9] = ((encode[7] >> 10) | (encode[8] << 22)) & 0x3FFFFFF;
        dest[10] = (encode[8] >> 4) & 0x3FFFFFF;
        dest[11] = ((encode[8] >> 30) | (encode[9] << 2)) & 0x3FFFFFF;
        dest[12] = ((encode[9] >> 24) | (encode[10] << 8)) & 0x3FFFFFF;
        dest[13] = ((encode[10] >> 18) | (encode[11] << 14)) & 0x3FFFFFF;
        dest[14] = ((encode[11] >> 12) | (encode[12] << 20)) & 0x3FFFFFF;
        dest[15] = (encode[12] >> 6) & 0x3FFFFFF;
        dest[16] = encode[13] & 0x3FFFFFF;
        dest[17] = ((encode[13] >> 26) | (encode[14] << 6)) & 0x3FFFFFF;
        dest[18] = ((encode[14] >> 20) | (encode[15] << 12)) & 0x3FFFFFF;
        dest[19] = ((encode[15] >> 14) | (encode[16] << 18)) & 0x3FFFFFF;
        dest[20] = ((encode[16] >> 8) | (encode[17] << 24)) & 0x3FFFFFF;
        dest[21] = (encode[17] >> 2) & 0x3FFFFFF;
        dest[22] = ((encode[17] >> 28) | (encode[18] << 4)) & 0x3FFFFFF;
        dest[23] = ((encode[18] >> 22) | (encode[19] << 10)) & 0x3FFFFFF;
        dest[24] = ((encode[19] >> 16) | (encode[20] << 16)) & 0x3FFFFFF;
        dest[25] = ((encode[20] >> 10) | (encode[21] << 22)) & 0x3FFFFFF;
        dest[26] = (encode[21] >> 4) & 0x3FFFFFF;
        dest[27] = ((encode[21] >> 30) | (encode[22] << 2)) & 0x3FFFFFF;
        dest[28] = ((encode[22] >> 24) | (encode[23] << 8)) & 0x3FFFFFF;
        dest[29] = ((encode[23] >> 18) | (encode[24] << 14)) & 0x3FFFFFF;
        dest[30] = ((encode[24] >> 12) | (encode[25] << 20)) & 0x3FFFFFF;
        dest[31] = (encode[25] >> 6) & 0x3FFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_26(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_27(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 27) {
        dest[0] = encode[0] & 0x7FFFFFF;
        dest[1] = ((encode[0] >> 27) | (encode[1] << 5)) & 0x7FFFFFF;
        dest[2] = ((encode[1] >> 22) | (encode[2] << 10)) & 0x7FFFFFF;
        dest[3] = ((encode[2] >> 17) | (encode[3] << 15)) & 0x7FFFFFF;
        dest[4] = ((encode[3] >> 12) | (encode[4] << 20)) & 0x7FFFFFF;
        dest[5] = ((encode[4] >> 7) | (encode[5] << 25)) & 0x7FFFFFF;
        dest[6] = (encode[5] >> 2) & 0x7FFFFFF;
        dest[7] = ((encode[5] >> 29) | (encode[6] << 3)) & 0x7FFFFFF;
        dest[8] = ((encode[6] >> 24) | (encode[7] << 8)) & 0x7FFFFFF;
        dest[9] = ((encode[7] >> 19) | (encode[8] << 13)) & 0x7FFFFFF;
        dest[10] = ((encode[8] >> 14) | (encode[9] << 18)) & 0x7FFFFFF;
        dest[11] = ((encode[9] >> 9) | (encode[10] << 23)) & 0x7FFFFFF;
        dest[12] = (encode[10] >> 4) & 0x7FFFFFF;
        dest[13] = ((encode[10] >> 31) | (encode[11] << 1)) & 0x7FFFFFF;
        dest[14] = ((encode[11] >> 26) | (encode[12] << 6)) & 0x7FFFFFF;
        dest[15] = ((encode[12] >> 21) | (encode[13] << 11)) & 0x7FFFFFF;
        dest[16] = ((encode[13] >> 16) | (encode[14] << 16)) & 0x7FFFFFF;
        dest[17] = ((encode[14] >> 11) | (encode[15] << 21)) & 0x7FFFFFF;
        dest[18] = ((encode[15] >> 6) | (encode[16] << 26)) & 0x7FFFFFF;
        dest[19] = (encode[16] >> 1) & 0x7FFFFFF;
        dest[20] = ((encode[16] >> 28) | (encode[17] << 4)) & 0x7FFFFFF;
        dest[21] = ((encode[17] >> 23) | (encode[18] << 9)) & 0x7FFFFFF;
        dest[22] = ((encode[18] >> 18) | (encode[19] << 14)) & 0x7FFFFFF;
        dest[23] = ((encode[19] >> 13) | (encode[20] << 19)) & 0x7FFFFFF;
        dest[24] = ((encode[20] >> 8) | (encode[21] << 24)) & 0x7FFFFFF;
        dest[25] = (encode[21] >> 3) & 0x7FFFFFF;
        dest[26] = ((encode[21] >> 30) | (encode[22] << 2)) & 0x7FFFFFF;
        dest[27] = ((encode[22] >> 25) | (encode[23] << 7)) & 0x7FFFFFF;
        dest[28] = ((encode[23] >> 20) | (encode[24] << 12)) & 0x7FFFFFF;
        dest[29] = ((encode[24] >> 15) | (encode[25] << 17)) & 0x7FFFFFF;
        dest[30] = ((encode[25] >> 10) | (encode[26] << 22)) & 0x7FFFFFF;
        dest[31] = (encode[26] >> 5) & 0x7FFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_27(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_28(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 28) {
        dest[0] = encode[0] & 0xFFFFFFF;
        dest[1] = ((encode[0] >> 28) | (encode[1] << 4)) & 0xFFFFFFF;
        dest[2] = ((encode[1] >> 24) | (encode[2] << 8)) & 0xFFFFFFF;
        dest[3] = ((encode[2] >> 20) | (encode[3] << 12)) & 0xFFFFFFF;
        dest[4] = ((encode[3] >> 16) | (encode[4] << 16)) & 0xFFFFFFF;
        dest[5] = ((encode[4] >> 12) | (encode[5] << 20)) & 0xFFFFFFF;
        dest[6] = ((encode[5] >> 8) | (encode[6] << 24)) & 0xFFFFFFF;
        dest[7] = (encode[6] >> 4) & 0xFFFFFFF;
        dest[8] = encode[7] & 0xFFFFFFF;
        dest[9] = ((encode[7] >> 28) | (encode[8] << 4)) & 0xFFFFFFF;
        dest[10] = ((encode[8] >> 24) | (encode[9] << 8)) & 0xFFFFFFF;
        dest[11] = ((encode[9] >> 20) | (encode[10] << 12)) & 0xFFFFFFF;
        dest[12] = ((encode[10] >> 16) | (encode[11] << 16)) & 0xFFFFFFF;
        dest[13] = ((encode[11] >> 12) | (encode[12] << 20)) & 0xFFFFFFF;
        dest[14] = ((encode[12] >> 8) | (encode[13] << 24)) & 0xFFFFFFF;
        dest[15] = (encode[13] >> 4) & 0xFFFFFFF;
        dest[16] = encode[14] & 0xFFFFFFF;
        dest[17] = ((encode[14] >> 28) | (encode[15] << 4)) & 0xFFFFFFF;
        dest[18] = ((encode[15] >> 24) | (encode[16] << 8)) & 0xFFFFFFF;
        dest[19] = ((encode[16] >> 20) | (encode[17] << 12)) & 0xFFFFFFF;
        dest[20] = ((encode[17] >> 16) | (encode[18] << 16)) & 0xFFFFFFF;
        dest[21] = ((encode[18] >> 12) | (encode[19] << 20)) & 0xFFFFFFF;
        dest[22] = ((encode[19] >> 8) | (encode[20] << 24)) & 0xFFFFFFF;
        dest[23] = (encode[20] >> 4) & 0xFFFFFFF;
        dest[24] = encode[21] & 0xFFFFFFF;
        dest[25] = ((encode[21] >> 28) | (encode[22] << 4)) & 0xFFFFFFF;
        dest[26] = ((encode[22] >> 24) | (encode[23] << 8)) & 0xFFFFFFF;
        dest[27] = ((encode[23] >> 20) | (encode[24] << 12)) & 0xFFFFFFF;
        dest[28] = ((encode[24] >> 16) | (encode[25] << 16)) & 0xFFFFFFF;
        dest[29] = ((encode[25] >> 12) | (encode[26] << 20)) & 0xFFFFFFF;
        dest[30] = ((encode[26] >> 8) | (encode[27] << 24)) & 0xFFFFFFF;
        dest[31] = (encode[27] >> 4) & 0xFFFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_28(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_29(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 29) {
        dest[0] = encode[0] & 0x1FFFFFFF;
        dest[1] = ((encode[0] >> 29) | (encode[1] << 3)) & 0x1FFFFFFF;
        dest[2] = ((encode[1] >> 26) | (encode[2] << 6)) & 0x1FFFFFFF;
        dest[3] = ((encode[2] >> 23) | (encode[3] << 9)) & 0x1FFFFFFF;
        dest[4] = ((encode[3] >> 20) | (encode[4] << 12)) & 0x1FFFFFFF;
        dest[5] = ((encode[4] >> 17) | (encode[5] << 15)) & 0x1FFFFFFF;
        dest[6] = ((encode[5] >> 14) | (encode[6] << 18)) & 0x1FFFFFFF;
        dest[7] = ((encode[6] >> 11) | (encode[7] << 21)) & 0x1FFFFFFF;
        dest[8] = ((encode[7] >> 8) | (encode[8] << 24)) & 0x1FFFFFFF;
        dest[9] = ((encode[8] >> 5) | (encode[9] << 27)) & 0x1FFFFFFF;
        dest[10] = (encode[9] >> 2) & 0x1FFFFFFF;
        dest[11] = ((encode[9] >> 31) | (encode[10] << 1)) & 0x1FFFFFFF;
        dest[12] = ((encode[10] >> 28) | (encode[11] << 4)) & 0x1FFFFFFF;
        dest[13] = ((encode[11] >> 25) | (encode[12] << 7)) & 0x1FFFFFFF;
        dest[14] = ((encode[12] >> 22) | (encode[13] << 10)) & 0x1FFFFFFF;
        dest[15] = ((encode[13] >> 19) | (encode[14] << 13)) & 0x1FFFFFFF;
        dest[16] = ((encode[14] >> 16) | (encode[15] << 16)) & 0x1FFFFFFF;
        dest[17] = ((encode[15] >> 13) | (encode[16] << 19)) & 0x1FFFFFFF;
        dest[18] = ((encode[16] >> 10) | (encode[17] << 22)) & 0x1FFFFFFF;
        dest[19] = ((encode[17] >> 7) | (encode[18] << 25)) & 0x1FFFFFFF;
        dest[20] = ((encode[18] >> 4) | (encode[19] << 28)) & 0x1FFFFFFF;
        dest[21] = (encode[19] >> 1) & 0x1FFFFFFF;
        dest[22] = ((encode[19] >> 30) | (encode[20] << 2)) & 0x1FFFFFFF;
        dest[23] = ((encode[20] >> 27) | (encode[21] << 5)) & 0x1FFFFFFF;
        dest[24] = ((encode[21] >> 24) | (encode[22] << 8)) & 0x1FFFFFFF;
        dest[25] = ((encode[22] >> 21) | (encode[23] << 11)) & 0x1FFFFFFF;
        dest[26] = ((encode[23] >> 18) | (encode[24] << 14)) & 0x1FFFFFFF;
        dest[27] = ((encode[24] >> 15) | (encode[25] << 17)) & 0x1FFFFFFF;
        dest[28] = ((encode[25] >> 12) | (encode[26] << 20)) & 0x1FFFFFFF;
        dest[29] = ((encode[26] >> 9) | (encode[27] << 23)) & 0x1FFFFFFF;
        dest[30] = ((encode[27] >> 6) | (encode[28] << 26)) & 0x1FFFFFFF;
        dest[31] = (encode[28] >> 3) & 0x1FFFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_29(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_30(Type* dest, const uint32_t* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 32, encode += 30) {
        dest[0] = encode[0] & 0x3FFFFFFF;
        dest[1] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x3FFFFFFF;
        dest[2] = ((encode[1] >> 28) | (encode[2] << 4)) & 0x3FFFFFFF;
        dest[3] = ((encode[2] >> 26) | (encode[3] << 6)) & 0x3FFFFFFF;
        dest[4] = ((encode[3] >> 24) | (encode[4] << 8)) & 0x3FFFFFFF;
        dest[5] = ((encode[4] >> 22) | (encode[5] << 10)) & 0x3FFFFFFF;
        dest[6] = ((encode[5] >> 20) | (encode[6] << 12)) & 0x3FFFFFFF;
        dest[7] = ((encode[6] >> 18) | (encode[7] << 14)) & 0x3FFFFFFF;
        dest[8] = ((encode[7] >> 16) | (encode[8] << 16)) & 0x3FFFFFFF;
        dest[9] = ((encode[8] >> 14) | (encode[9] << 18)) & 0x3FFFFFFF;
        dest[10] = ((encode[9] >> 12) | (encode[10] << 20)) & 0x3FFFFFFF;
        dest[11] = ((encode[10] >> 10) | (encode[11] << 22)) & 0x3FFFFFFF;
        dest[12] = ((encode[11] >> 8) | (encode[12] << 24)) & 0x3FFFFFFF;
        dest[13] = ((encode[12] >> 6) | (encode[13] << 26)) & 0x3FFFFFFF;
        dest[14] = ((encode[13] >> 4) | (encode[14] << 28)) & 0x3FFFFFFF;
        dest[15] = (encode[14] >> 2) & 0x3FFFFFFF;
        dest[16] = encode[15] & 0x3FFFFFFF;
        dest[17] = ((encode[15] >> 30) | (encode[16] << 2)) & 0x3FFFFFFF;
        dest[18] = ((encode[16] >> 28) | (encode[17] << 4)) & 0x3FFFFFFF;
        dest[19] = ((encode[17] >> 26) | (encode[18] << 6)) & 0x3FFFFFFF;
        dest[20] = ((encode[18] >> 24) | (encode[19] << 8)) & 0x3FFFFFFF;
        dest[21] = ((encode[19] >> 22) | (encode[20] << 10)) & 0x3FFFFFFF;
        dest[22] = ((encode[20] >> 20) | (encode[21] << 12)) & 0x3FFFFFFF;
        dest[23] = ((encode[21] >> 18) | (encode[22] << 14)) & 0x3FFFFFFF;
        dest[24] = ((encode[22] >> 16) | (encode[23] << 16)) & 0x3FFFFFFF;
        dest[25] = ((encode[23] >> 14) | (encode[24] << 18)) & 0x3FFFFFFF;
        dest[26] = ((encode[24] >> 12) | (encode[25] << 20)) & 0x3FFFFFFF;
        dest[27] = ((encode[25] >> 10) | (encode[26] << 22)) & 0x3FFFFFFF;
        dest[28] = ((encode[26] >> 8) | (encode[27] << 24)) & 0x3FFFFFFF;
        dest[29] = ((encode[27] >> 6) | (encode[28] << 26)) & 0x3FFFFFFF;
        dest[30] = ((encode[28] >> 4) | (encode[29] << 28)) & 0x3FFFFFFF;
        dest[31] = (encode[29] >> 2) & 0x3FFFFFFF;
    }

    if (n & 0x1F) {
        unaligned_unpack_30(dest, encode, (n & 0x1F));
    }
}

template <typename Type>
void unpack_31(Type* dest, const uint32_t* encode, uint32_t n)
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

template <typename Type>
void unpack_32(Type* dest, const uint32_t* encode, uint32_t n)
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
}
