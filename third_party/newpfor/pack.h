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

namespace indexlib {

template <typename T>
inline void pack(uint32_t* dest, const T* src, uint32_t n, uint32_t b)
{
    uint32_t offset = 0;
    uint32_t destLoc = 0;
    for (uint32_t i = 0; i < n; ++i) {
        dest[destLoc] |= src[i] << offset;
        if (offset + b - 1 > 31) {
            dest[destLoc + 1] |= src[i] >> (32 - offset);
        }

        destLoc += (offset + b) >> 5;
        offset = (offset + b) & 31;
    }
}

template <typename T>
inline void pack(uint32_t* dest, T* src, uint32_t n, uint32_t b)
{
    uint32_t offset = 0;
    uint32_t destLoc = 0;
    for (uint32_t i = 0; i < n; ++i) {
        dest[destLoc] |= src[i] << offset;
        if (offset + b - 1 > 31) {
            dest[destLoc + 1] |= src[i] >> (32 - offset);
        }

        destLoc += (offset + b) >> 5;
        offset = (offset + b) & 31;
    }
}

template <typename Type>
void pack_0(uint32_t* dest, const Type* encode, uint32_t n)
{
    return; // nothing to do
}

template <typename Type>
void pack_1(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, ++dest, encode += 32) {
        uint32_t res = encode[0] & 0x1;
        res |= ((encode[1] & 0x1) << 1);
        res |= ((encode[2] & 0x1) << 2);
        res |= ((encode[3] & 0x1) << 3);
        res |= ((encode[4] & 0x1) << 4);
        res |= ((encode[5] & 0x1) << 5);
        res |= ((encode[6] & 0x1) << 6);
        res |= ((encode[7] & 0x1) << 7);
        res |= ((encode[8] & 0x1) << 8);
        res |= ((encode[9] & 0x1) << 9);
        res |= ((encode[10] & 0x1) << 10);
        res |= ((encode[11] & 0x1) << 11);
        res |= ((encode[12] & 0x1) << 12);
        res |= ((encode[13] & 0x1) << 13);
        res |= ((encode[14] & 0x1) << 14);
        res |= ((encode[15] & 0x1) << 15);
        res |= ((encode[16] & 0x1) << 16);
        res |= ((encode[17] & 0x1) << 17);
        res |= ((encode[18] & 0x1) << 18);
        res |= ((encode[19] & 0x1) << 19);
        res |= ((encode[20] & 0x1) << 20);
        res |= ((encode[21] & 0x1) << 21);
        res |= ((encode[22] & 0x1) << 22);
        res |= ((encode[23] & 0x1) << 23);
        res |= ((encode[24] & 0x1) << 24);
        res |= ((encode[25] & 0x1) << 25);
        res |= ((encode[26] & 0x1) << 26);
        res |= ((encode[27] & 0x1) << 27);
        res |= ((encode[28] & 0x1) << 28);
        res |= ((encode[29] & 0x1) << 29);
        res |= ((encode[30] & 0x1) << 30);
        res |= ((encode[31] & 0x1) << 31);
        dest[0] = res;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 1);
    }
}

template <typename Type>
void pack_2(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 2) {
        uint32_t res0;
        uint32_t res1;
        res0 = encode[0] & 0x3;
        res0 |= (encode[1] & 0x3) << 2;
        res0 |= (encode[2] & 0x3) << 4;
        res0 |= (encode[3] & 0x3) << 6;
        res0 |= (encode[4] & 0x3) << 8;
        res0 |= (encode[5] & 0x3) << 10;
        res0 |= (encode[6] & 0x3) << 12;
        res0 |= (encode[7] & 0x3) << 14;
        res0 |= (encode[8] & 0x3) << 16;
        res0 |= (encode[9] & 0x3) << 18;
        res0 |= (encode[10] & 0x3) << 20;
        res0 |= (encode[11] & 0x3) << 22;
        res0 |= (encode[12] & 0x3) << 24;
        res0 |= (encode[13] & 0x3) << 26;
        res0 |= (encode[14] & 0x3) << 28;
        res0 |= (encode[15] & 0x3) << 30;

        res1 = encode[16] & 0x3;
        res1 |= (encode[17] & 0x3) << 2;
        res1 |= (encode[18] & 0x3) << 4;
        res1 |= (encode[19] & 0x3) << 6;
        res1 |= (encode[20] & 0x3) << 8;
        res1 |= (encode[21] & 0x3) << 10;
        res1 |= (encode[22] & 0x3) << 12;
        res1 |= (encode[23] & 0x3) << 14;
        res1 |= (encode[24] & 0x3) << 16;
        res1 |= (encode[25] & 0x3) << 18;
        res1 |= (encode[26] & 0x3) << 20;
        res1 |= (encode[27] & 0x3) << 22;
        res1 |= (encode[28] & 0x3) << 24;
        res1 |= (encode[29] & 0x3) << 26;
        res1 |= (encode[30] & 0x3) << 28;
        res1 |= (encode[31] & 0x3) << 30;

        dest[0] = res0;
        dest[1] = res1;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 2);
    }
}

template <typename Type>
void pack_3(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; encode += 32, i += 32, dest += 3) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        res0 = (encode[0] & 0x7);
        res0 |= ((encode[1] & 0x7) << 3);
        res0 |= ((encode[2] & 0x7) << 6);
        res0 |= ((encode[3] & 0x7) << 9);
        res0 |= ((encode[4] & 0x7) << 12);
        res0 |= ((encode[5] & 0x7) << 15);
        res0 |= ((encode[6] & 0x7) << 18);
        res0 |= ((encode[7] & 0x7) << 21);
        res0 |= ((encode[8] & 0x7) << 24);
        res0 |= ((encode[9] & 0x7) << 27);
        res0 |= ((encode[10] & 0x7) << 30);

        res1 = ((encode[10] & 0x7) >> 2);
        res1 |= ((encode[11] & 0x7) << 1);
        res1 |= ((encode[12] & 0x7) << 4);
        res1 |= ((encode[13] & 0x7) << 7);
        res1 |= ((encode[14] & 0x7) << 10);
        res1 |= ((encode[15] & 0x7) << 13);
        res1 |= ((encode[16] & 0x7) << 16);
        res1 |= ((encode[17] & 0x7) << 19);
        res1 |= ((encode[18] & 0x7) << 22);
        res1 |= ((encode[19] & 0x7) << 25);
        res1 |= ((encode[20] & 0x7) << 28);
        res1 |= ((encode[21] & 0x7) << 31);

        res2 = ((encode[21] & 0x7) >> 1);
        res2 |= ((encode[22] & 0x7) << 2);
        res2 |= ((encode[23] & 0x7) << 5);
        res2 |= ((encode[24] & 0x7) << 8);
        res2 |= ((encode[25] & 0x7) << 11);
        res2 |= ((encode[26] & 0x7) << 14);
        res2 |= ((encode[27] & 0x7) << 17);
        res2 |= ((encode[28] & 0x7) << 20);
        res2 |= ((encode[29] & 0x7) << 23);
        res2 |= ((encode[30] & 0x7) << 26);
        res2 |= ((encode[31] & 0x7) << 29);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 3);
    }
}

template <typename Type>
void pack_4(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 4, encode += 32) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;

        res0 = (encode[0] & 0xF);
        res0 |= ((encode[1] & 0xF) << 4);
        res0 |= ((encode[2] & 0xF) << 8);
        res0 |= ((encode[3] & 0xF) << 12);
        res0 |= ((encode[4] & 0xF) << 16);
        res0 |= ((encode[5] & 0xF) << 20);
        res0 |= ((encode[6] & 0xF) << 24);
        res0 |= ((encode[7] & 0xF) << 28);

        res1 = (encode[8] & 0xF);
        res1 |= ((encode[9] & 0xF) << 4);
        res1 |= ((encode[10] & 0xF) << 8);
        res1 |= ((encode[11] & 0xF) << 12);
        res1 |= ((encode[12] & 0xF) << 16);
        res1 |= ((encode[13] & 0xF) << 20);
        res1 |= ((encode[14] & 0xF) << 24);
        res1 |= ((encode[15] & 0xF) << 28);

        res2 = (encode[16] & 0xF);
        res2 |= ((encode[17] & 0xF) << 4);
        res2 |= ((encode[18] & 0xF) << 8);
        res2 |= ((encode[19] & 0xF) << 12);
        res2 |= ((encode[20] & 0xF) << 16);
        res2 |= ((encode[21] & 0xF) << 20);
        res2 |= ((encode[22] & 0xF) << 24);
        res2 |= ((encode[23] & 0xF) << 28);

        res3 = (encode[24] & 0xF);
        res3 |= ((encode[25] & 0xF) << 4);
        res3 |= ((encode[26] & 0xF) << 8);
        res3 |= ((encode[27] & 0xF) << 12);
        res3 |= ((encode[28] & 0xF) << 16);
        res3 |= ((encode[29] & 0xF) << 20);
        res3 |= ((encode[30] & 0xF) << 24);
        res3 |= ((encode[31] & 0xF) << 28);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 4);
    }
}

template <typename Type>
void pack_5(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 5) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;

        res0 = (encode[0] & 0x1F);
        res0 |= ((encode[1] & 0x1F) << 5);
        res0 |= ((encode[2] & 0x1F) << 10);
        res0 |= ((encode[3] & 0x1F) << 15);
        res0 |= ((encode[4] & 0x1F) << 20);
        res0 |= ((encode[5] & 0x1F) << 25);
        res0 |= ((encode[6] & 0x1F) << 30);

        res1 = ((encode[6] & 0x1F) >> 2);
        res1 |= ((encode[7] & 0x1F) << 3);
        res1 |= ((encode[8] & 0x1F) << 8);
        res1 |= ((encode[9] & 0x1F) << 13);
        res1 |= ((encode[10] & 0x1F) << 18);
        res1 |= ((encode[11] & 0x1F) << 23);
        res1 |= ((encode[12] & 0x1F) << 28);

        res2 = ((encode[12] & 0x1F) >> 4);
        res2 |= ((encode[13] & 0x1F) << 1);
        res2 |= ((encode[14] & 0x1F) << 6);
        res2 |= ((encode[15] & 0x1F) << 11);
        res2 |= ((encode[16] & 0x1F) << 16);
        res2 |= ((encode[17] & 0x1F) << 21);
        res2 |= ((encode[18] & 0x1F) << 26);
        res2 |= ((encode[19] & 0x1F) << 31);

        res3 = ((encode[19] & 0x1F) >> 1);
        res3 |= ((encode[20] & 0x1F) << 4);
        res3 |= ((encode[21] & 0x1F) << 9);
        res3 |= ((encode[22] & 0x1F) << 14);
        res3 |= ((encode[23] & 0x1F) << 19);
        res3 |= ((encode[24] & 0x1F) << 24);
        res3 |= ((encode[25] & 0x1F) << 29);

        res4 = ((encode[25] & 0x1F) >> 3);
        res4 |= ((encode[26] & 0x1F) << 2);
        res4 |= ((encode[27] & 0x1F) << 7);
        res4 |= ((encode[28] & 0x1F) << 12);
        res4 |= ((encode[29] & 0x1F) << 17);
        res4 |= ((encode[30] & 0x1F) << 22);
        res4 |= ((encode[31] & 0x1F) << 27);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 5);
    }
}

template <typename Type>
void pack_6(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, dest += 6, encode += 32) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;

        res0 = (encode[0] & 0x3F);
        res0 |= ((encode[1] & 0x3F) << 6);
        res0 |= ((encode[2] & 0x3F) << 12);
        res0 |= ((encode[3] & 0x3F) << 18);
        res0 |= ((encode[4] & 0x3F) << 24);
        res0 |= ((encode[5] & 0x3F) << 30);

        res1 = ((encode[5] & 0x3F) >> 2);
        res1 |= ((encode[6] & 0x3F) << 4);
        res1 |= ((encode[7] & 0x3F) << 10);
        res1 |= ((encode[8] & 0x3F) << 16);
        res1 |= ((encode[9] & 0x3F) << 22);
        res1 |= ((encode[10] & 0x3F) << 28);

        res2 = ((encode[10] & 0x3F) >> 4);
        res2 |= ((encode[11] & 0x3F) << 2);
        res2 |= ((encode[12] & 0x3F) << 8);
        res2 |= ((encode[13] & 0x3F) << 14);
        res2 |= ((encode[14] & 0x3F) << 20);
        res2 |= ((encode[15] & 0x3F) << 26);

        res3 = (encode[16] & 0x3F);
        res3 |= ((encode[17] & 0x3F) << 6);
        res3 |= ((encode[18] & 0x3F) << 12);
        res3 |= ((encode[19] & 0x3F) << 18);
        res3 |= ((encode[20] & 0x3F) << 24);
        res3 |= ((encode[21] & 0x3F) << 30);

        res4 = ((encode[21] & 0x3F) >> 2);
        res4 |= ((encode[22] & 0x3F) << 4);
        res4 |= ((encode[23] & 0x3F) << 10);
        res4 |= ((encode[24] & 0x3F) << 16);
        res4 |= ((encode[25] & 0x3F) << 22);
        res4 |= ((encode[26] & 0x3F) << 28);

        res5 = ((encode[26] & 0x3F) >> 4);
        res5 |= ((encode[27] & 0x3F) << 2);
        res5 |= ((encode[28] & 0x3F) << 8);
        res5 |= ((encode[29] & 0x3F) << 14);
        res5 |= ((encode[30] & 0x3F) << 20);
        res5 |= ((encode[31] & 0x3F) << 26);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 6);
    }
}

template <typename Type>
void pack_7(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 7) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;

        res0 = (encode[0] & 0x7F);
        res0 |= ((encode[1] & 0x7F) << 7);
        res0 |= ((encode[2] & 0x7F) << 14);
        res0 |= ((encode[3] & 0x7F) << 21);
        res0 |= ((encode[4] & 0x7F) << 28);

        res1 = ((encode[4] & 0x7F) >> 4);
        res1 |= ((encode[5] & 0x7F) << 3);
        res1 |= ((encode[6] & 0x7F) << 10);
        res1 |= ((encode[7] & 0x7F) << 17);
        res1 |= ((encode[8] & 0x7F) << 24);
        res1 |= ((encode[9] & 0x7F) << 31);

        res2 = ((encode[9] & 0x7F) >> 1);
        res2 |= ((encode[10] & 0x7F) << 6);
        res2 |= ((encode[11] & 0x7F) << 13);
        res2 |= ((encode[12] & 0x7F) << 20);
        res2 |= ((encode[13] & 0x7F) << 27);

        res3 = ((encode[13] & 0x7F) >> 5);
        res3 |= ((encode[14] & 0x7F) << 2);
        res3 |= ((encode[15] & 0x7F) << 9);
        res3 |= ((encode[16] & 0x7F) << 16);
        res3 |= ((encode[17] & 0x7F) << 23);
        res3 |= ((encode[18] & 0x7F) << 30);

        res4 = ((encode[18] & 0x7F) >> 2);
        res4 |= ((encode[19] & 0x7F) << 5);
        res4 |= ((encode[20] & 0x7F) << 12);
        res4 |= ((encode[21] & 0x7F) << 19);
        res4 |= ((encode[22] & 0x7F) << 26);

        res5 = ((encode[22] & 0x7F) >> 6);
        res5 |= ((encode[23] & 0x7F) << 1);
        res5 |= ((encode[24] & 0x7F) << 8);
        res5 |= ((encode[25] & 0x7F) << 15);
        res5 |= ((encode[26] & 0x7F) << 22);
        res5 |= ((encode[27] & 0x7F) << 29);

        res6 = ((encode[27] & 0x7F) >> 3);
        res6 |= ((encode[28] & 0x7F) << 4);
        res6 |= ((encode[29] & 0x7F) << 11);
        res6 |= ((encode[30] & 0x7F) << 18);
        res6 |= ((encode[31] & 0x7F) << 25);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 7);
    }
}

template <typename Type>
void pack_8(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 8) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;

        res0 = (encode[0] & 0xFF);
        res0 |= ((encode[1] & 0xFF) << 8);
        res0 |= ((encode[2] & 0xFF) << 16);
        res0 |= ((encode[3] & 0xFF) << 24);

        res1 = (encode[4] & 0xFF);
        res1 |= ((encode[5] & 0xFF) << 8);
        res1 |= ((encode[6] & 0xFF) << 16);
        res1 |= ((encode[7] & 0xFF) << 24);

        res2 = (encode[8] & 0xFF);
        res2 |= ((encode[9] & 0xFF) << 8);
        res2 |= ((encode[10] & 0xFF) << 16);
        res2 |= ((encode[11] & 0xFF) << 24);

        res3 = (encode[12] & 0xFF);
        res3 |= ((encode[13] & 0xFF) << 8);
        res3 |= ((encode[14] & 0xFF) << 16);
        res3 |= ((encode[15] & 0xFF) << 24);

        res4 = (encode[16] & 0xFF);
        res4 |= ((encode[17] & 0xFF) << 8);
        res4 |= ((encode[18] & 0xFF) << 16);
        res4 |= ((encode[19] & 0xFF) << 24);

        res5 = (encode[20] & 0xFF);
        res5 |= ((encode[21] & 0xFF) << 8);
        res5 |= ((encode[22] & 0xFF) << 16);
        res5 |= ((encode[23] & 0xFF) << 24);

        res6 = (encode[24] & 0xFF);
        res6 |= ((encode[25] & 0xFF) << 8);
        res6 |= ((encode[26] & 0xFF) << 16);
        res6 |= ((encode[27] & 0xFF) << 24);

        res7 = (encode[28] & 0xFF);
        res7 |= ((encode[29] & 0xFF) << 8);
        res7 |= ((encode[30] & 0xFF) << 16);
        res7 |= ((encode[31] & 0xFF) << 24);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 8);
    }
}

template <typename Type>
void pack_9(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 9) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;

        res0 = (encode[0] & 0x1FF);
        res0 |= ((encode[1] & 0x1FF) << 9);
        res0 |= ((encode[2] & 0x1FF) << 18);
        res0 |= ((encode[3] & 0x1FF) << 27);

        res1 = ((encode[3] & 0x1FF) >> 5);
        res1 |= ((encode[4] & 0x1FF) << 4);
        res1 |= ((encode[5] & 0x1FF) << 13);
        res1 |= ((encode[6] & 0x1FF) << 22);
        res1 |= ((encode[7] & 0x1FF) << 31);

        res2 = ((encode[7] & 0x1FF) >> 1);
        res2 |= ((encode[8] & 0x1FF) << 8);
        res2 |= ((encode[9] & 0x1FF) << 17);
        res2 |= ((encode[10] & 0x1FF) << 26);

        res3 = ((encode[10] & 0x1FF) >> 6);
        res3 |= ((encode[11] & 0x1FF) << 3);
        res3 |= ((encode[12] & 0x1FF) << 12);
        res3 |= ((encode[13] & 0x1FF) << 21);
        res3 |= ((encode[14] & 0x1FF) << 30);

        res4 = ((encode[14] & 0x1FF) >> 2);
        res4 |= ((encode[15] & 0x1FF) << 7);
        res4 |= ((encode[16] & 0x1FF) << 16);
        res4 |= ((encode[17] & 0x1FF) << 25);

        res5 = ((encode[17] & 0x1FF) >> 7);
        res5 |= ((encode[18] & 0x1FF) << 2);
        res5 |= ((encode[19] & 0x1FF) << 11);
        res5 |= ((encode[20] & 0x1FF) << 20);
        res5 |= ((encode[21] & 0x1FF) << 29);

        res6 = ((encode[21] & 0x1FF) >> 3);
        res6 |= ((encode[22] & 0x1FF) << 6);
        res6 |= ((encode[23] & 0x1FF) << 15);
        res6 |= ((encode[24] & 0x1FF) << 24);

        res7 = ((encode[24] & 0x1FF) >> 8);
        res7 |= ((encode[25] & 0x1FF) << 1);
        res7 |= ((encode[26] & 0x1FF) << 10);
        res7 |= ((encode[27] & 0x1FF) << 19);
        res7 |= ((encode[28] & 0x1FF) << 28);

        res8 = ((encode[28] & 0x1FF) >> 4);
        res8 |= ((encode[29] & 0x1FF) << 5);
        res8 |= ((encode[30] & 0x1FF) << 14);
        res8 |= ((encode[31] & 0x1FF) << 23);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 9);
    }
}

template <typename Type>
void pack_10(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 10) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;

        res0 = (encode[0] & 0x3FF);
        res0 |= ((encode[1] & 0x3FF) << 10);
        res0 |= ((encode[2] & 0x3FF) << 20);
        res0 |= ((encode[3] & 0x3FF) << 30);

        res1 = ((encode[3] & 0x3FF) >> 2);
        res1 |= ((encode[4] & 0x3FF) << 8);
        res1 |= ((encode[5] & 0x3FF) << 18);
        res1 |= ((encode[6] & 0x3FF) << 28);

        res2 = ((encode[6] & 0x3FF) >> 4);
        res2 |= ((encode[7] & 0x3FF) << 6);
        res2 |= ((encode[8] & 0x3FF) << 16);
        res2 |= ((encode[9] & 0x3FF) << 26);

        res3 = ((encode[9] & 0x3FF) >> 6);
        res3 |= ((encode[10] & 0x3FF) << 4);
        res3 |= ((encode[11] & 0x3FF) << 14);
        res3 |= ((encode[12] & 0x3FF) << 24);

        res4 = ((encode[12] & 0x3FF) >> 8);
        res4 |= ((encode[13] & 0x3FF) << 2);
        res4 |= ((encode[14] & 0x3FF) << 12);
        res4 |= ((encode[15] & 0x3FF) << 22);

        res5 = (encode[16] & 0x3FF);
        res5 |= ((encode[17] & 0x3FF) << 10);
        res5 |= ((encode[18] & 0x3FF) << 20);
        res5 |= ((encode[19] & 0x3FF) << 30);

        res6 = ((encode[19] & 0x3FF) >> 2);
        res6 |= ((encode[20] & 0x3FF) << 8);
        res6 |= ((encode[21] & 0x3FF) << 18);
        res6 |= ((encode[22] & 0x3FF) << 28);

        res7 = ((encode[22] & 0x3FF) >> 4);
        res7 |= ((encode[23] & 0x3FF) << 6);
        res7 |= ((encode[24] & 0x3FF) << 16);
        res7 |= ((encode[25] & 0x3FF) << 26);

        res8 = ((encode[25] & 0x3FF) >> 6);
        res8 |= ((encode[26] & 0x3FF) << 4);
        res8 |= ((encode[27] & 0x3FF) << 14);
        res8 |= ((encode[28] & 0x3FF) << 24);

        res9 = ((encode[28] & 0x3FF) >> 8);
        res9 |= ((encode[29] & 0x3FF) << 2);
        res9 |= ((encode[30] & 0x3FF) << 12);
        res9 |= ((encode[31] & 0x3FF) << 22);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 10);
    }
}

template <typename Type>
void pack_11(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 11) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;

        res0 = (encode[0] & 0x7FF);
        res0 |= ((encode[1] & 0x7FF) << 11);
        res0 |= ((encode[2] & 0x7FF) << 22);

        res1 = ((encode[2] & 0x7FF) >> 10);
        res1 |= ((encode[3] & 0x7FF) << 1);
        res1 |= ((encode[4] & 0x7FF) << 12);
        res1 |= ((encode[5] & 0x7FF) << 23);

        res2 = ((encode[5] & 0x7FF) >> 9);
        res2 |= ((encode[6] & 0x7FF) << 2);
        res2 |= ((encode[7] & 0x7FF) << 13);
        res2 |= ((encode[8] & 0x7FF) << 24);

        res3 = ((encode[8] & 0x7FF) >> 8);
        res3 |= ((encode[9] & 0x7FF) << 3);
        res3 |= ((encode[10] & 0x7FF) << 14);
        res3 |= ((encode[11] & 0x7FF) << 25);

        res4 = ((encode[11] & 0x7FF) >> 7);
        res4 |= ((encode[12] & 0x7FF) << 4);
        res4 |= ((encode[13] & 0x7FF) << 15);
        res4 |= ((encode[14] & 0x7FF) << 26);

        res5 = ((encode[14] & 0x7FF) >> 6);
        res5 |= ((encode[15] & 0x7FF) << 5);
        res5 |= ((encode[16] & 0x7FF) << 16);
        res5 |= ((encode[17] & 0x7FF) << 27);

        res6 = ((encode[17] & 0x7FF) >> 5);
        res6 |= ((encode[18] & 0x7FF) << 6);
        res6 |= ((encode[19] & 0x7FF) << 17);
        res6 |= ((encode[20] & 0x7FF) << 28);

        res7 = ((encode[20] & 0x7FF) >> 4);
        res7 |= ((encode[21] & 0x7FF) << 7);
        res7 |= ((encode[22] & 0x7FF) << 18);
        res7 |= ((encode[23] & 0x7FF) << 29);

        res8 = ((encode[23] & 0x7FF) >> 3);
        res8 |= ((encode[24] & 0x7FF) << 8);
        res8 |= ((encode[25] & 0x7FF) << 19);
        res8 |= ((encode[26] & 0x7FF) << 30);

        res9 = ((encode[26] & 0x7FF) >> 2);
        res9 |= ((encode[27] & 0x7FF) << 9);
        res9 |= ((encode[28] & 0x7FF) << 20);
        res9 |= ((encode[29] & 0x7FF) << 31);

        res10 = ((encode[29] & 0x7FF) >> 1);
        res10 |= ((encode[30] & 0x7FF) << 10);
        res10 |= ((encode[31] & 0x7FF) << 21);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 11);
    }
}

template <typename Type>
void pack_12(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 12) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;

        res0 = (encode[0] & 0xFFF);
        res0 |= ((encode[1] & 0xFFF) << 12);
        res0 |= ((encode[2] & 0xFFF) << 24);

        res1 = ((encode[2] & 0xFFF) >> 8);
        res1 |= ((encode[3] & 0xFFF) << 4);
        res1 |= ((encode[4] & 0xFFF) << 16);
        res1 |= ((encode[5] & 0xFFF) << 28);

        res2 = ((encode[5] & 0xFFF) >> 4);
        res2 |= ((encode[6] & 0xFFF) << 8);
        res2 |= ((encode[7] & 0xFFF) << 20);

        res3 = (encode[8] & 0xFFF);
        res3 |= ((encode[9] & 0xFFF) << 12);
        res3 |= ((encode[10] & 0xFFF) << 24);

        res4 = ((encode[10] & 0xFFF) >> 8);
        res4 |= ((encode[11] & 0xFFF) << 4);
        res4 |= ((encode[12] & 0xFFF) << 16);
        res4 |= ((encode[13] & 0xFFF) << 28);

        res5 = ((encode[13] & 0xFFF) >> 4);
        res5 |= ((encode[14] & 0xFFF) << 8);
        res5 |= ((encode[15] & 0xFFF) << 20);

        res6 = (encode[16] & 0xFFF);
        res6 |= ((encode[17] & 0xFFF) << 12);
        res6 |= ((encode[18] & 0xFFF) << 24);

        res7 = ((encode[18] & 0xFFF) >> 8);
        res7 |= ((encode[19] & 0xFFF) << 4);
        res7 |= ((encode[20] & 0xFFF) << 16);
        res7 |= ((encode[21] & 0xFFF) << 28);

        res8 = ((encode[21] & 0xFFF) >> 4);
        res8 |= ((encode[22] & 0xFFF) << 8);
        res8 |= ((encode[23] & 0xFFF) << 20);

        res9 = (encode[24] & 0xFFF);
        res9 |= ((encode[25] & 0xFFF) << 12);
        res9 |= ((encode[26] & 0xFFF) << 24);

        res10 = ((encode[26] & 0xFFF) >> 8);
        res10 |= ((encode[27] & 0xFFF) << 4);
        res10 |= ((encode[28] & 0xFFF) << 16);
        res10 |= ((encode[29] & 0xFFF) << 28);

        res11 = ((encode[29] & 0xFFF) >> 4);
        res11 |= ((encode[30] & 0xFFF) << 8);
        res11 |= ((encode[31] & 0xFFF) << 20);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 12);
    }
}

template <typename Type>
void pack_13(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 13) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        res0 = (encode[0] & 0x1FFF);
        res0 |= ((encode[1] & 0x1FFF) << 13);
        res0 |= ((encode[2] & 0x1FFF) << 26);

        res1 = ((encode[2] & 0x1FFF) >> 6);
        res1 |= ((encode[3] & 0x1FFF) << 7);
        res1 |= ((encode[4] & 0x1FFF) << 20);

        res2 = ((encode[4] & 0x1FFF) >> 12);
        res2 |= ((encode[5] & 0x1FFF) << 1);
        res2 |= ((encode[6] & 0x1FFF) << 14);
        res2 |= ((encode[7] & 0x1FFF) << 27);

        res3 = ((encode[7] & 0x1FFF) >> 5);
        res3 |= ((encode[8] & 0x1FFF) << 8);
        res3 |= ((encode[9] & 0x1FFF) << 21);

        res4 = ((encode[9] & 0x1FFF) >> 11);
        res4 |= ((encode[10] & 0x1FFF) << 2);
        res4 |= ((encode[11] & 0x1FFF) << 15);
        res4 |= ((encode[12] & 0x1FFF) << 28);

        res5 = ((encode[12] & 0x1FFF) >> 4);
        res5 |= ((encode[13] & 0x1FFF) << 9);
        res5 |= ((encode[14] & 0x1FFF) << 22);

        res6 = ((encode[14] & 0x1FFF) >> 10);
        res6 |= ((encode[15] & 0x1FFF) << 3);
        res6 |= ((encode[16] & 0x1FFF) << 16);
        res6 |= ((encode[17] & 0x1FFF) << 29);

        res7 = ((encode[17] & 0x1FFF) >> 3);
        res7 |= ((encode[18] & 0x1FFF) << 10);
        res7 |= ((encode[19] & 0x1FFF) << 23);

        res8 = ((encode[19] & 0x1FFF) >> 9);
        res8 |= ((encode[20] & 0x1FFF) << 4);
        res8 |= ((encode[21] & 0x1FFF) << 17);
        res8 |= ((encode[22] & 0x1FFF) << 30);

        res9 = ((encode[22] & 0x1FFF) >> 2);
        res9 |= ((encode[23] & 0x1FFF) << 11);
        res9 |= ((encode[24] & 0x1FFF) << 24);

        res10 = ((encode[24] & 0x1FFF) >> 8);
        res10 |= ((encode[25] & 0x1FFF) << 5);
        res10 |= ((encode[26] & 0x1FFF) << 18);
        res10 |= ((encode[27] & 0x1FFF) << 31);

        res11 = ((encode[27] & 0x1FFF) >> 1);
        res11 |= ((encode[28] & 0x1FFF) << 12);
        res11 |= ((encode[29] & 0x1FFF) << 25);

        res12 = ((encode[29] & 0x1FFF) >> 7);
        res12 |= ((encode[30] & 0x1FFF) << 6);
        res12 |= ((encode[31] & 0x1FFF) << 19);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 13);
    }
}

template <typename Type>
void pack_14(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 14) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;

        res0 = (encode[0] & 0x3FFF);
        res0 |= ((encode[1] & 0x3FFF) << 14);
        res0 |= ((encode[2] & 0x3FFF) << 28);

        res1 = ((encode[2] & 0x3FFF) >> 4);
        res1 |= ((encode[3] & 0x3FFF) << 10);
        res1 |= ((encode[4] & 0x3FFF) << 24);

        res2 = ((encode[4] & 0x3FFF) >> 8);
        res2 |= ((encode[5] & 0x3FFF) << 6);
        res2 |= ((encode[6] & 0x3FFF) << 20);

        res3 = ((encode[6] & 0x3FFF) >> 12);
        res3 |= ((encode[7] & 0x3FFF) << 2);
        res3 |= ((encode[8] & 0x3FFF) << 16);
        res3 |= ((encode[9] & 0x3FFF) << 30);

        res4 = ((encode[9] & 0x3FFF) >> 2);
        res4 |= ((encode[10] & 0x3FFF) << 12);
        res4 |= ((encode[11] & 0x3FFF) << 26);

        res5 = ((encode[11] & 0x3FFF) >> 6);
        res5 |= ((encode[12] & 0x3FFF) << 8);
        res5 |= ((encode[13] & 0x3FFF) << 22);

        res6 = ((encode[13] & 0x3FFF) >> 10);
        res6 |= ((encode[14] & 0x3FFF) << 4);
        res6 |= ((encode[15] & 0x3FFF) << 18);

        res7 = (encode[16] & 0x3FFF);
        res7 |= ((encode[17] & 0x3FFF) << 14);
        res7 |= ((encode[18] & 0x3FFF) << 28);

        res8 = ((encode[18] & 0x3FFF) >> 4);
        res8 |= ((encode[19] & 0x3FFF) << 10);
        res8 |= ((encode[20] & 0x3FFF) << 24);

        res9 = ((encode[20] & 0x3FFF) >> 8);
        res9 |= ((encode[21] & 0x3FFF) << 6);
        res9 |= ((encode[22] & 0x3FFF) << 20);

        res10 = ((encode[22] & 0x3FFF) >> 12);
        res10 |= ((encode[23] & 0x3FFF) << 2);
        res10 |= ((encode[24] & 0x3FFF) << 16);
        res10 |= ((encode[25] & 0x3FFF) << 30);

        res11 = ((encode[25] & 0x3FFF) >> 2);
        res11 |= ((encode[26] & 0x3FFF) << 12);
        res11 |= ((encode[27] & 0x3FFF) << 26);

        res12 = ((encode[27] & 0x3FFF) >> 6);
        res12 |= ((encode[28] & 0x3FFF) << 8);
        res12 |= ((encode[29] & 0x3FFF) << 22);

        res13 = ((encode[29] & 0x3FFF) >> 10);
        res13 |= ((encode[30] & 0x3FFF) << 4);
        res13 |= ((encode[31] & 0x3FFF) << 18);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 14);
    }
}

template <typename Type>
void pack_15(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 15) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;

        res0 = (encode[0] & 0x7FFF);
        res0 |= ((encode[1] & 0x7FFF) << 15);
        res0 |= ((encode[2] & 0x7FFF) << 30);

        res1 = ((encode[2] & 0x7FFF) >> 2);
        res1 |= ((encode[3] & 0x7FFF) << 13);
        res1 |= ((encode[4] & 0x7FFF) << 28);

        res2 = ((encode[4] & 0x7FFF) >> 4);
        res2 |= ((encode[5] & 0x7FFF) << 11);
        res2 |= ((encode[6] & 0x7FFF) << 26);

        res3 = ((encode[6] & 0x7FFF) >> 6);
        res3 |= ((encode[7] & 0x7FFF) << 9);
        res3 |= ((encode[8] & 0x7FFF) << 24);

        res4 = ((encode[8] & 0x7FFF) >> 8);
        res4 |= ((encode[9] & 0x7FFF) << 7);
        res4 |= ((encode[10] & 0x7FFF) << 22);

        res5 = ((encode[10] & 0x7FFF) >> 10);
        res5 |= ((encode[11] & 0x7FFF) << 5);
        res5 |= ((encode[12] & 0x7FFF) << 20);

        res6 = ((encode[12] & 0x7FFF) >> 12);
        res6 |= ((encode[13] & 0x7FFF) << 3);
        res6 |= ((encode[14] & 0x7FFF) << 18);

        res7 = ((encode[14] & 0x7FFF) >> 14);
        res7 |= ((encode[15] & 0x7FFF) << 1);
        res7 |= ((encode[16] & 0x7FFF) << 16);
        res7 |= ((encode[17] & 0x7FFF) << 31);

        res8 = ((encode[17] & 0x7FFF) >> 1);
        res8 |= ((encode[18] & 0x7FFF) << 14);
        res8 |= ((encode[19] & 0x7FFF) << 29);

        res9 = ((encode[19] & 0x7FFF) >> 3);
        res9 |= ((encode[20] & 0x7FFF) << 12);
        res9 |= ((encode[21] & 0x7FFF) << 27);

        res10 = ((encode[21] & 0x7FFF) >> 5);
        res10 |= ((encode[22] & 0x7FFF) << 10);
        res10 |= ((encode[23] & 0x7FFF) << 25);

        res11 = ((encode[23] & 0x7FFF) >> 7);
        res11 |= ((encode[24] & 0x7FFF) << 8);
        res11 |= ((encode[25] & 0x7FFF) << 23);

        res12 = ((encode[25] & 0x7FFF) >> 9);
        res12 |= ((encode[26] & 0x7FFF) << 6);
        res12 |= ((encode[27] & 0x7FFF) << 21);

        res13 = ((encode[27] & 0x7FFF) >> 11);
        res13 |= ((encode[28] & 0x7FFF) << 4);
        res13 |= ((encode[29] & 0x7FFF) << 19);

        res14 = ((encode[29] & 0x7FFF) >> 13);
        res14 |= ((encode[30] & 0x7FFF) << 2);
        res14 |= ((encode[31] & 0x7FFF) << 17);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 15);
    }
}

template <typename Type>
void pack_16(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 16) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;

        res0 = (encode[0] & 0xFFFF);
        res0 |= ((encode[1] & 0xFFFF) << 16);

        res1 = (encode[2] & 0xFFFF);
        res1 |= ((encode[3] & 0xFFFF) << 16);

        res2 = (encode[4] & 0xFFFF);
        res2 |= ((encode[5] & 0xFFFF) << 16);

        res3 = (encode[6] & 0xFFFF);
        res3 |= ((encode[7] & 0xFFFF) << 16);

        res4 = (encode[8] & 0xFFFF);
        res4 |= ((encode[9] & 0xFFFF) << 16);

        res5 = (encode[10] & 0xFFFF);
        res5 |= ((encode[11] & 0xFFFF) << 16);

        res6 = (encode[12] & 0xFFFF);
        res6 |= ((encode[13] & 0xFFFF) << 16);

        res7 = (encode[14] & 0xFFFF);
        res7 |= ((encode[15] & 0xFFFF) << 16);

        res8 = (encode[16] & 0xFFFF);
        res8 |= ((encode[17] & 0xFFFF) << 16);

        res9 = (encode[18] & 0xFFFF);
        res9 |= ((encode[19] & 0xFFFF) << 16);

        res10 = (encode[20] & 0xFFFF);
        res10 |= ((encode[21] & 0xFFFF) << 16);

        res11 = (encode[22] & 0xFFFF);
        res11 |= ((encode[23] & 0xFFFF) << 16);

        res12 = (encode[24] & 0xFFFF);
        res12 |= ((encode[25] & 0xFFFF) << 16);

        res13 = (encode[26] & 0xFFFF);
        res13 |= ((encode[27] & 0xFFFF) << 16);

        res14 = (encode[28] & 0xFFFF);
        res14 |= ((encode[29] & 0xFFFF) << 16);

        res15 = (encode[30] & 0xFFFF);
        res15 |= ((encode[31] & 0xFFFF) << 16);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 16);
    }
}

template <typename Type>
void pack_17(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 17) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;

        res0 = (encode[0] & 0x1FFFF);
        res0 |= ((encode[1] & 0x1FFFF) << 17);

        res1 = ((encode[1] & 0x1FFFF) >> 15);
        res1 |= ((encode[2] & 0x1FFFF) << 2);
        res1 |= ((encode[3] & 0x1FFFF) << 19);

        res2 = ((encode[3] & 0x1FFFF) >> 13);
        res2 |= ((encode[4] & 0x1FFFF) << 4);
        res2 |= ((encode[5] & 0x1FFFF) << 21);

        res3 = ((encode[5] & 0x1FFFF) >> 11);
        res3 |= ((encode[6] & 0x1FFFF) << 6);
        res3 |= ((encode[7] & 0x1FFFF) << 23);

        res4 = ((encode[7] & 0x1FFFF) >> 9);
        res4 |= ((encode[8] & 0x1FFFF) << 8);
        res4 |= ((encode[9] & 0x1FFFF) << 25);

        res5 = ((encode[9] & 0x1FFFF) >> 7);
        res5 |= ((encode[10] & 0x1FFFF) << 10);
        res5 |= ((encode[11] & 0x1FFFF) << 27);

        res6 = ((encode[11] & 0x1FFFF) >> 5);
        res6 |= ((encode[12] & 0x1FFFF) << 12);
        res6 |= ((encode[13] & 0x1FFFF) << 29);

        res7 = ((encode[13] & 0x1FFFF) >> 3);
        res7 |= ((encode[14] & 0x1FFFF) << 14);
        res7 |= ((encode[15] & 0x1FFFF) << 31);

        res8 = ((encode[15] & 0x1FFFF) >> 1);
        res8 |= ((encode[16] & 0x1FFFF) << 16);

        res9 = ((encode[16] & 0x1FFFF) >> 16);
        res9 |= ((encode[17] & 0x1FFFF) << 1);
        res9 |= ((encode[18] & 0x1FFFF) << 18);

        res10 = ((encode[18] & 0x1FFFF) >> 14);
        res10 |= ((encode[19] & 0x1FFFF) << 3);
        res10 |= ((encode[20] & 0x1FFFF) << 20);

        res11 = ((encode[20] & 0x1FFFF) >> 12);
        res11 |= ((encode[21] & 0x1FFFF) << 5);
        res11 |= ((encode[22] & 0x1FFFF) << 22);

        res12 = ((encode[22] & 0x1FFFF) >> 10);
        res12 |= ((encode[23] & 0x1FFFF) << 7);
        res12 |= ((encode[24] & 0x1FFFF) << 24);

        res13 = ((encode[24] & 0x1FFFF) >> 8);
        res13 |= ((encode[25] & 0x1FFFF) << 9);
        res13 |= ((encode[26] & 0x1FFFF) << 26);

        res14 = ((encode[26] & 0x1FFFF) >> 6);
        res14 |= ((encode[27] & 0x1FFFF) << 11);
        res14 |= ((encode[28] & 0x1FFFF) << 28);

        res15 = ((encode[28] & 0x1FFFF) >> 4);
        res15 |= ((encode[29] & 0x1FFFF) << 13);
        res15 |= ((encode[30] & 0x1FFFF) << 30);

        res16 = ((encode[30] & 0x1FFFF) >> 2);
        res16 |= ((encode[31] & 0x1FFFF) << 15);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 17);
    }
}

template <typename Type>
void pack_18(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 18) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;

        res0 = (encode[0] & 0x3FFFF);
        res0 |= ((encode[1] & 0x3FFFF) << 18);

        res1 = ((encode[1] & 0x3FFFF) >> 14);
        res1 |= ((encode[2] & 0x3FFFF) << 4);
        res1 |= ((encode[3] & 0x3FFFF) << 22);

        res2 = ((encode[3] & 0x3FFFF) >> 10);
        res2 |= ((encode[4] & 0x3FFFF) << 8);
        res2 |= ((encode[5] & 0x3FFFF) << 26);

        res3 = ((encode[5] & 0x3FFFF) >> 6);
        res3 |= ((encode[6] & 0x3FFFF) << 12);
        res3 |= ((encode[7] & 0x3FFFF) << 30);

        res4 = ((encode[7] & 0x3FFFF) >> 2);
        res4 |= ((encode[8] & 0x3FFFF) << 16);

        res5 = ((encode[8] & 0x3FFFF) >> 16);
        res5 |= ((encode[9] & 0x3FFFF) << 2);
        res5 |= ((encode[10] & 0x3FFFF) << 20);

        res6 = ((encode[10] & 0x3FFFF) >> 12);
        res6 |= ((encode[11] & 0x3FFFF) << 6);
        res6 |= ((encode[12] & 0x3FFFF) << 24);

        res7 = ((encode[12] & 0x3FFFF) >> 8);
        res7 |= ((encode[13] & 0x3FFFF) << 10);
        res7 |= ((encode[14] & 0x3FFFF) << 28);

        res8 = ((encode[14] & 0x3FFFF) >> 4);
        res8 |= ((encode[15] & 0x3FFFF) << 14);

        res9 = (encode[16] & 0x3FFFF);
        res9 |= ((encode[17] & 0x3FFFF) << 18);

        res10 = ((encode[17] & 0x3FFFF) >> 14);
        res10 |= ((encode[18] & 0x3FFFF) << 4);
        res10 |= ((encode[19] & 0x3FFFF) << 22);

        res11 = ((encode[19] & 0x3FFFF) >> 10);
        res11 |= ((encode[20] & 0x3FFFF) << 8);
        res11 |= ((encode[21] & 0x3FFFF) << 26);

        res12 = ((encode[21] & 0x3FFFF) >> 6);
        res12 |= ((encode[22] & 0x3FFFF) << 12);
        res12 |= ((encode[23] & 0x3FFFF) << 30);

        res13 = ((encode[23] & 0x3FFFF) >> 2);
        res13 |= ((encode[24] & 0x3FFFF) << 16);

        res14 = ((encode[24] & 0x3FFFF) >> 16);
        res14 |= ((encode[25] & 0x3FFFF) << 2);
        res14 |= ((encode[26] & 0x3FFFF) << 20);

        res15 = ((encode[26] & 0x3FFFF) >> 12);
        res15 |= ((encode[27] & 0x3FFFF) << 6);
        res15 |= ((encode[28] & 0x3FFFF) << 24);

        res16 = ((encode[28] & 0x3FFFF) >> 8);
        res16 |= ((encode[29] & 0x3FFFF) << 10);
        res16 |= ((encode[30] & 0x3FFFF) << 28);

        res17 = ((encode[30] & 0x3FFFF) >> 4);
        res17 |= ((encode[31] & 0x3FFFF) << 14);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 18);
    }
}

template <typename Type>
void pack_19(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 19) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;

        res0 = (encode[0] & 0x7FFFF);
        res0 |= ((encode[1] & 0x7FFFF) << 19);

        res1 = ((encode[1] & 0x7FFFF) >> 13);
        res1 |= ((encode[2] & 0x7FFFF) << 6);
        res1 |= ((encode[3] & 0x7FFFF) << 25);

        res2 = ((encode[3] & 0x7FFFF) >> 7);
        res2 |= ((encode[4] & 0x7FFFF) << 12);
        res2 |= ((encode[5] & 0x7FFFF) << 31);

        res3 = ((encode[5] & 0x7FFFF) >> 1);
        res3 |= ((encode[6] & 0x7FFFF) << 18);

        res4 = ((encode[6] & 0x7FFFF) >> 14);
        res4 |= ((encode[7] & 0x7FFFF) << 5);
        res4 |= ((encode[8] & 0x7FFFF) << 24);

        res5 = ((encode[8] & 0x7FFFF) >> 8);
        res5 |= ((encode[9] & 0x7FFFF) << 11);
        res5 |= ((encode[10] & 0x7FFFF) << 30);

        res6 = ((encode[10] & 0x7FFFF) >> 2);
        res6 |= ((encode[11] & 0x7FFFF) << 17);

        res7 = ((encode[11] & 0x7FFFF) >> 15);
        res7 |= ((encode[12] & 0x7FFFF) << 4);
        res7 |= ((encode[13] & 0x7FFFF) << 23);

        res8 = ((encode[13] & 0x7FFFF) >> 9);
        res8 |= ((encode[14] & 0x7FFFF) << 10);
        res8 |= ((encode[15] & 0x7FFFF) << 29);

        res9 = ((encode[15] & 0x7FFFF) >> 3);
        res9 |= ((encode[16] & 0x7FFFF) << 16);

        res10 = ((encode[16] & 0x7FFFF) >> 16);
        res10 |= ((encode[17] & 0x7FFFF) << 3);
        res10 |= ((encode[18] & 0x7FFFF) << 22);

        res11 = ((encode[18] & 0x7FFFF) >> 10);
        res11 |= ((encode[19] & 0x7FFFF) << 9);
        res11 |= ((encode[20] & 0x7FFFF) << 28);

        res12 = ((encode[20] & 0x7FFFF) >> 4);
        res12 |= ((encode[21] & 0x7FFFF) << 15);

        res13 = ((encode[21] & 0x7FFFF) >> 17);
        res13 |= ((encode[22] & 0x7FFFF) << 2);
        res13 |= ((encode[23] & 0x7FFFF) << 21);

        res14 = ((encode[23] & 0x7FFFF) >> 11);
        res14 |= ((encode[24] & 0x7FFFF) << 8);
        res14 |= ((encode[25] & 0x7FFFF) << 27);

        res15 = ((encode[25] & 0x7FFFF) >> 5);
        res15 |= ((encode[26] & 0x7FFFF) << 14);

        res16 = ((encode[26] & 0x7FFFF) >> 18);
        res16 |= ((encode[27] & 0x7FFFF) << 1);
        res16 |= ((encode[28] & 0x7FFFF) << 20);

        res17 = ((encode[28] & 0x7FFFF) >> 12);
        res17 |= ((encode[29] & 0x7FFFF) << 7);
        res17 |= ((encode[30] & 0x7FFFF) << 26);

        res18 = ((encode[30] & 0x7FFFF) >> 6);
        res18 |= ((encode[31] & 0x7FFFF) << 13);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 19);
    }
}

template <typename Type>
void pack_20(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 20) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;

        res0 = (encode[0] & 0xFFFFF);
        res0 |= ((encode[1] & 0xFFFFF) << 20);

        res1 = ((encode[1] & 0xFFFFF) >> 12);
        res1 |= ((encode[2] & 0xFFFFF) << 8);
        res1 |= ((encode[3] & 0xFFFFF) << 28);

        res2 = ((encode[3] & 0xFFFFF) >> 4);
        res2 |= ((encode[4] & 0xFFFFF) << 16);

        res3 = ((encode[4] & 0xFFFFF) >> 16);
        res3 |= ((encode[5] & 0xFFFFF) << 4);
        res3 |= ((encode[6] & 0xFFFFF) << 24);

        res4 = ((encode[6] & 0xFFFFF) >> 8);
        res4 |= ((encode[7] & 0xFFFFF) << 12);

        res5 = (encode[8] & 0xFFFFF);
        res5 |= ((encode[9] & 0xFFFFF) << 20);

        res6 = ((encode[9] & 0xFFFFF) >> 12);
        res6 |= ((encode[10] & 0xFFFFF) << 8);
        res6 |= ((encode[11] & 0xFFFFF) << 28);

        res7 = ((encode[11] & 0xFFFFF) >> 4);
        res7 |= ((encode[12] & 0xFFFFF) << 16);

        res8 = ((encode[12] & 0xFFFFF) >> 16);
        res8 |= ((encode[13] & 0xFFFFF) << 4);
        res8 |= ((encode[14] & 0xFFFFF) << 24);

        res9 = ((encode[14] & 0xFFFFF) >> 8);
        res9 |= ((encode[15] & 0xFFFFF) << 12);

        res10 = (encode[16] & 0xFFFFF);
        res10 |= ((encode[17] & 0xFFFFF) << 20);

        res11 = ((encode[17] & 0xFFFFF) >> 12);
        res11 |= ((encode[18] & 0xFFFFF) << 8);
        res11 |= ((encode[19] & 0xFFFFF) << 28);

        res12 = ((encode[19] & 0xFFFFF) >> 4);
        res12 |= ((encode[20] & 0xFFFFF) << 16);

        res13 = ((encode[20] & 0xFFFFF) >> 16);
        res13 |= ((encode[21] & 0xFFFFF) << 4);
        res13 |= ((encode[22] & 0xFFFFF) << 24);

        res14 = ((encode[22] & 0xFFFFF) >> 8);
        res14 |= ((encode[23] & 0xFFFFF) << 12);

        res15 = (encode[24] & 0xFFFFF);
        res15 |= ((encode[25] & 0xFFFFF) << 20);

        res16 = ((encode[25] & 0xFFFFF) >> 12);
        res16 |= ((encode[26] & 0xFFFFF) << 8);
        res16 |= ((encode[27] & 0xFFFFF) << 28);

        res17 = ((encode[27] & 0xFFFFF) >> 4);
        res17 |= ((encode[28] & 0xFFFFF) << 16);

        res18 = ((encode[28] & 0xFFFFF) >> 16);
        res18 |= ((encode[29] & 0xFFFFF) << 4);
        res18 |= ((encode[30] & 0xFFFFF) << 24);

        res19 = ((encode[30] & 0xFFFFF) >> 8);
        res19 |= ((encode[31] & 0xFFFFF) << 12);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 20);
    }
}

template <typename Type>
void pack_21(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 21) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;

        res0 = (encode[0] & 0x1FFFFF);
        res0 |= ((encode[1] & 0x1FFFFF) << 21);

        res1 = ((encode[1] & 0x1FFFFF) >> 11);
        res1 |= ((encode[2] & 0x1FFFFF) << 10);
        res1 |= ((encode[3] & 0x1FFFFF) << 31);

        res2 = ((encode[3] & 0x1FFFFF) >> 1);
        res2 |= ((encode[4] & 0x1FFFFF) << 20);

        res3 = ((encode[4] & 0x1FFFFF) >> 12);
        res3 |= ((encode[5] & 0x1FFFFF) << 9);
        res3 |= ((encode[6] & 0x1FFFFF) << 30);

        res4 = ((encode[6] & 0x1FFFFF) >> 2);
        res4 |= ((encode[7] & 0x1FFFFF) << 19);

        res5 = ((encode[7] & 0x1FFFFF) >> 13);
        res5 |= ((encode[8] & 0x1FFFFF) << 8);
        res5 |= ((encode[9] & 0x1FFFFF) << 29);

        res6 = ((encode[9] & 0x1FFFFF) >> 3);
        res6 |= ((encode[10] & 0x1FFFFF) << 18);

        res7 = ((encode[10] & 0x1FFFFF) >> 14);
        res7 |= ((encode[11] & 0x1FFFFF) << 7);
        res7 |= ((encode[12] & 0x1FFFFF) << 28);

        res8 = ((encode[12] & 0x1FFFFF) >> 4);
        res8 |= ((encode[13] & 0x1FFFFF) << 17);

        res9 = ((encode[13] & 0x1FFFFF) >> 15);
        res9 |= ((encode[14] & 0x1FFFFF) << 6);
        res9 |= ((encode[15] & 0x1FFFFF) << 27);

        res10 = ((encode[15] & 0x1FFFFF) >> 5);
        res10 |= ((encode[16] & 0x1FFFFF) << 16);

        res11 = ((encode[16] & 0x1FFFFF) >> 16);
        res11 |= ((encode[17] & 0x1FFFFF) << 5);
        res11 |= ((encode[18] & 0x1FFFFF) << 26);

        res12 = ((encode[18] & 0x1FFFFF) >> 6);
        res12 |= ((encode[19] & 0x1FFFFF) << 15);

        res13 = ((encode[19] & 0x1FFFFF) >> 17);
        res13 |= ((encode[20] & 0x1FFFFF) << 4);
        res13 |= ((encode[21] & 0x1FFFFF) << 25);

        res14 = ((encode[21] & 0x1FFFFF) >> 7);
        res14 |= ((encode[22] & 0x1FFFFF) << 14);

        res15 = ((encode[22] & 0x1FFFFF) >> 18);
        res15 |= ((encode[23] & 0x1FFFFF) << 3);
        res15 |= ((encode[24] & 0x1FFFFF) << 24);

        res16 = ((encode[24] & 0x1FFFFF) >> 8);
        res16 |= ((encode[25] & 0x1FFFFF) << 13);

        res17 = ((encode[25] & 0x1FFFFF) >> 19);
        res17 |= ((encode[26] & 0x1FFFFF) << 2);
        res17 |= ((encode[27] & 0x1FFFFF) << 23);

        res18 = ((encode[27] & 0x1FFFFF) >> 9);
        res18 |= ((encode[28] & 0x1FFFFF) << 12);

        res19 = ((encode[28] & 0x1FFFFF) >> 20);
        res19 |= ((encode[29] & 0x1FFFFF) << 1);
        res19 |= ((encode[30] & 0x1FFFFF) << 22);

        res20 = ((encode[30] & 0x1FFFFF) >> 10);
        res20 |= ((encode[31] & 0x1FFFFF) << 11);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 21);
    }
}

template <typename Type>
void pack_22(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 22) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;

        res0 = (encode[0] & 0x3FFFFF);
        res0 |= ((encode[1] & 0x3FFFFF) << 22);

        res1 = ((encode[1] & 0x3FFFFF) >> 10);
        res1 |= ((encode[2] & 0x3FFFFF) << 12);

        res2 = ((encode[2] & 0x3FFFFF) >> 20);
        res2 |= ((encode[3] & 0x3FFFFF) << 2);
        res2 |= ((encode[4] & 0x3FFFFF) << 24);

        res3 = ((encode[4] & 0x3FFFFF) >> 8);
        res3 |= ((encode[5] & 0x3FFFFF) << 14);

        res4 = ((encode[5] & 0x3FFFFF) >> 18);
        res4 |= ((encode[6] & 0x3FFFFF) << 4);
        res4 |= ((encode[7] & 0x3FFFFF) << 26);

        res5 = ((encode[7] & 0x3FFFFF) >> 6);
        res5 |= ((encode[8] & 0x3FFFFF) << 16);

        res6 = ((encode[8] & 0x3FFFFF) >> 16);
        res6 |= ((encode[9] & 0x3FFFFF) << 6);
        res6 |= ((encode[10] & 0x3FFFFF) << 28);

        res7 = ((encode[10] & 0x3FFFFF) >> 4);
        res7 |= ((encode[11] & 0x3FFFFF) << 18);

        res8 = ((encode[11] & 0x3FFFFF) >> 14);
        res8 |= ((encode[12] & 0x3FFFFF) << 8);
        res8 |= ((encode[13] & 0x3FFFFF) << 30);

        res9 = ((encode[13] & 0x3FFFFF) >> 2);
        res9 |= ((encode[14] & 0x3FFFFF) << 20);

        res10 = ((encode[14] & 0x3FFFFF) >> 12);
        res10 |= ((encode[15] & 0x3FFFFF) << 10);

        res11 = (encode[16] & 0x3FFFFF);
        res11 |= ((encode[17] & 0x3FFFFF) << 22);

        res12 = ((encode[17] & 0x3FFFFF) >> 10);
        res12 |= ((encode[18] & 0x3FFFFF) << 12);

        res13 = ((encode[18] & 0x3FFFFF) >> 20);
        res13 |= ((encode[19] & 0x3FFFFF) << 2);
        res13 |= ((encode[20] & 0x3FFFFF) << 24);

        res14 = ((encode[20] & 0x3FFFFF) >> 8);
        res14 |= ((encode[21] & 0x3FFFFF) << 14);

        res15 = ((encode[21] & 0x3FFFFF) >> 18);
        res15 |= ((encode[22] & 0x3FFFFF) << 4);
        res15 |= ((encode[23] & 0x3FFFFF) << 26);

        res16 = ((encode[23] & 0x3FFFFF) >> 6);
        res16 |= ((encode[24] & 0x3FFFFF) << 16);

        res17 = ((encode[24] & 0x3FFFFF) >> 16);
        res17 |= ((encode[25] & 0x3FFFFF) << 6);
        res17 |= ((encode[26] & 0x3FFFFF) << 28);

        res18 = ((encode[26] & 0x3FFFFF) >> 4);
        res18 |= ((encode[27] & 0x3FFFFF) << 18);

        res19 = ((encode[27] & 0x3FFFFF) >> 14);
        res19 |= ((encode[28] & 0x3FFFFF) << 8);
        res19 |= ((encode[29] & 0x3FFFFF) << 30);

        res20 = ((encode[29] & 0x3FFFFF) >> 2);
        res20 |= ((encode[30] & 0x3FFFFF) << 20);

        res21 = ((encode[30] & 0x3FFFFF) >> 12);
        res21 |= ((encode[31] & 0x3FFFFF) << 10);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 22);
    }
}

template <typename Type>
void pack_23(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 23) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;

        res0 = (encode[0] & 0x7FFFFF);
        res0 |= ((encode[1] & 0x7FFFFF) << 23);

        res1 = ((encode[1] & 0x7FFFFF) >> 9);
        res1 |= ((encode[2] & 0x7FFFFF) << 14);

        res2 = ((encode[2] & 0x7FFFFF) >> 18);
        res2 |= ((encode[3] & 0x7FFFFF) << 5);
        res2 |= ((encode[4] & 0x7FFFFF) << 28);

        res3 = ((encode[4] & 0x7FFFFF) >> 4);
        res3 |= ((encode[5] & 0x7FFFFF) << 19);

        res4 = ((encode[5] & 0x7FFFFF) >> 13);
        res4 |= ((encode[6] & 0x7FFFFF) << 10);

        res5 = ((encode[6] & 0x7FFFFF) >> 22);
        res5 |= ((encode[7] & 0x7FFFFF) << 1);
        res5 |= ((encode[8] & 0x7FFFFF) << 24);

        res6 = ((encode[8] & 0x7FFFFF) >> 8);
        res6 |= ((encode[9] & 0x7FFFFF) << 15);

        res7 = ((encode[9] & 0x7FFFFF) >> 17);
        res7 |= ((encode[10] & 0x7FFFFF) << 6);
        res7 |= ((encode[11] & 0x7FFFFF) << 29);

        res8 = ((encode[11] & 0x7FFFFF) >> 3);
        res8 |= ((encode[12] & 0x7FFFFF) << 20);

        res9 = ((encode[12] & 0x7FFFFF) >> 12);
        res9 |= ((encode[13] & 0x7FFFFF) << 11);

        res10 = ((encode[13] & 0x7FFFFF) >> 21);
        res10 |= ((encode[14] & 0x7FFFFF) << 2);
        res10 |= ((encode[15] & 0x7FFFFF) << 25);

        res11 = ((encode[15] & 0x7FFFFF) >> 7);
        res11 |= ((encode[16] & 0x7FFFFF) << 16);

        res12 = ((encode[16] & 0x7FFFFF) >> 16);
        res12 |= ((encode[17] & 0x7FFFFF) << 7);
        res12 |= ((encode[18] & 0x7FFFFF) << 30);

        res13 = ((encode[18] & 0x7FFFFF) >> 2);
        res13 |= ((encode[19] & 0x7FFFFF) << 21);

        res14 = ((encode[19] & 0x7FFFFF) >> 11);
        res14 |= ((encode[20] & 0x7FFFFF) << 12);

        res15 = ((encode[20] & 0x7FFFFF) >> 20);
        res15 |= ((encode[21] & 0x7FFFFF) << 3);
        res15 |= ((encode[22] & 0x7FFFFF) << 26);

        res16 = ((encode[22] & 0x7FFFFF) >> 6);
        res16 |= ((encode[23] & 0x7FFFFF) << 17);

        res17 = ((encode[23] & 0x7FFFFF) >> 15);
        res17 |= ((encode[24] & 0x7FFFFF) << 8);
        res17 |= ((encode[25] & 0x7FFFFF) << 31);

        res18 = ((encode[25] & 0x7FFFFF) >> 1);
        res18 |= ((encode[26] & 0x7FFFFF) << 22);

        res19 = ((encode[26] & 0x7FFFFF) >> 10);
        res19 |= ((encode[27] & 0x7FFFFF) << 13);

        res20 = ((encode[27] & 0x7FFFFF) >> 19);
        res20 |= ((encode[28] & 0x7FFFFF) << 4);
        res20 |= ((encode[29] & 0x7FFFFF) << 27);

        res21 = ((encode[29] & 0x7FFFFF) >> 5);
        res21 |= ((encode[30] & 0x7FFFFF) << 18);

        res22 = ((encode[30] & 0x7FFFFF) >> 14);
        res22 |= ((encode[31] & 0x7FFFFF) << 9);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 23);
    }
}

template <typename Type>
void pack_24(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 24) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;

        res0 = (encode[0] & 0xFFFFFF);
        res0 |= ((encode[1] & 0xFFFFFF) << 24);

        res1 = ((encode[1] & 0xFFFFFF) >> 8);
        res1 |= ((encode[2] & 0xFFFFFF) << 16);

        res2 = ((encode[2] & 0xFFFFFF) >> 16);
        res2 |= ((encode[3] & 0xFFFFFF) << 8);

        res3 = (encode[4] & 0xFFFFFF);
        res3 |= ((encode[5] & 0xFFFFFF) << 24);

        res4 = ((encode[5] & 0xFFFFFF) >> 8);
        res4 |= ((encode[6] & 0xFFFFFF) << 16);

        res5 = ((encode[6] & 0xFFFFFF) >> 16);
        res5 |= ((encode[7] & 0xFFFFFF) << 8);

        res6 = (encode[8] & 0xFFFFFF);
        res6 |= ((encode[9] & 0xFFFFFF) << 24);

        res7 = ((encode[9] & 0xFFFFFF) >> 8);
        res7 |= ((encode[10] & 0xFFFFFF) << 16);

        res8 = ((encode[10] & 0xFFFFFF) >> 16);
        res8 |= ((encode[11] & 0xFFFFFF) << 8);

        res9 = (encode[12] & 0xFFFFFF);
        res9 |= ((encode[13] & 0xFFFFFF) << 24);

        res10 = ((encode[13] & 0xFFFFFF) >> 8);
        res10 |= ((encode[14] & 0xFFFFFF) << 16);

        res11 = ((encode[14] & 0xFFFFFF) >> 16);
        res11 |= ((encode[15] & 0xFFFFFF) << 8);

        res12 = (encode[16] & 0xFFFFFF);
        res12 |= ((encode[17] & 0xFFFFFF) << 24);

        res13 = ((encode[17] & 0xFFFFFF) >> 8);
        res13 |= ((encode[18] & 0xFFFFFF) << 16);

        res14 = ((encode[18] & 0xFFFFFF) >> 16);
        res14 |= ((encode[19] & 0xFFFFFF) << 8);

        res15 = (encode[20] & 0xFFFFFF);
        res15 |= ((encode[21] & 0xFFFFFF) << 24);

        res16 = ((encode[21] & 0xFFFFFF) >> 8);
        res16 |= ((encode[22] & 0xFFFFFF) << 16);

        res17 = ((encode[22] & 0xFFFFFF) >> 16);
        res17 |= ((encode[23] & 0xFFFFFF) << 8);

        res18 = (encode[24] & 0xFFFFFF);
        res18 |= ((encode[25] & 0xFFFFFF) << 24);

        res19 = ((encode[25] & 0xFFFFFF) >> 8);
        res19 |= ((encode[26] & 0xFFFFFF) << 16);

        res20 = ((encode[26] & 0xFFFFFF) >> 16);
        res20 |= ((encode[27] & 0xFFFFFF) << 8);

        res21 = (encode[28] & 0xFFFFFF);
        res21 |= ((encode[29] & 0xFFFFFF) << 24);

        res22 = ((encode[29] & 0xFFFFFF) >> 8);
        res22 |= ((encode[30] & 0xFFFFFF) << 16);

        res23 = ((encode[30] & 0xFFFFFF) >> 16);
        res23 |= ((encode[31] & 0xFFFFFF) << 8);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 24);
    }
}

template <typename Type>
void pack_25(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 25) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;
        uint32_t res24;

        res0 = (encode[0] & 0x1FFFFFF);
        res0 |= ((encode[1] & 0x1FFFFFF) << 25);

        res1 = ((encode[1] & 0x1FFFFFF) >> 7);
        res1 |= ((encode[2] & 0x1FFFFFF) << 18);

        res2 = ((encode[2] & 0x1FFFFFF) >> 14);
        res2 |= ((encode[3] & 0x1FFFFFF) << 11);

        res3 = ((encode[3] & 0x1FFFFFF) >> 21);
        res3 |= ((encode[4] & 0x1FFFFFF) << 4);
        res3 |= ((encode[5] & 0x1FFFFFF) << 29);

        res4 = ((encode[5] & 0x1FFFFFF) >> 3);
        res4 |= ((encode[6] & 0x1FFFFFF) << 22);

        res5 = ((encode[6] & 0x1FFFFFF) >> 10);
        res5 |= ((encode[7] & 0x1FFFFFF) << 15);

        res6 = ((encode[7] & 0x1FFFFFF) >> 17);
        res6 |= ((encode[8] & 0x1FFFFFF) << 8);

        res7 = ((encode[8] & 0x1FFFFFF) >> 24);
        res7 |= ((encode[9] & 0x1FFFFFF) << 1);
        res7 |= ((encode[10] & 0x1FFFFFF) << 26);

        res8 = ((encode[10] & 0x1FFFFFF) >> 6);
        res8 |= ((encode[11] & 0x1FFFFFF) << 19);

        res9 = ((encode[11] & 0x1FFFFFF) >> 13);
        res9 |= ((encode[12] & 0x1FFFFFF) << 12);

        res10 = ((encode[12] & 0x1FFFFFF) >> 20);
        res10 |= ((encode[13] & 0x1FFFFFF) << 5);
        res10 |= ((encode[14] & 0x1FFFFFF) << 30);

        res11 = ((encode[14] & 0x1FFFFFF) >> 2);
        res11 |= ((encode[15] & 0x1FFFFFF) << 23);

        res12 = ((encode[15] & 0x1FFFFFF) >> 9);
        res12 |= ((encode[16] & 0x1FFFFFF) << 16);

        res13 = ((encode[16] & 0x1FFFFFF) >> 16);
        res13 |= ((encode[17] & 0x1FFFFFF) << 9);

        res14 = ((encode[17] & 0x1FFFFFF) >> 23);
        res14 |= ((encode[18] & 0x1FFFFFF) << 2);
        res14 |= ((encode[19] & 0x1FFFFFF) << 27);

        res15 = ((encode[19] & 0x1FFFFFF) >> 5);
        res15 |= ((encode[20] & 0x1FFFFFF) << 20);

        res16 = ((encode[20] & 0x1FFFFFF) >> 12);
        res16 |= ((encode[21] & 0x1FFFFFF) << 13);

        res17 = ((encode[21] & 0x1FFFFFF) >> 19);
        res17 |= ((encode[22] & 0x1FFFFFF) << 6);
        res17 |= ((encode[23] & 0x1FFFFFF) << 31);

        res18 = ((encode[23] & 0x1FFFFFF) >> 1);
        res18 |= ((encode[24] & 0x1FFFFFF) << 24);

        res19 = ((encode[24] & 0x1FFFFFF) >> 8);
        res19 |= ((encode[25] & 0x1FFFFFF) << 17);

        res20 = ((encode[25] & 0x1FFFFFF) >> 15);
        res20 |= ((encode[26] & 0x1FFFFFF) << 10);

        res21 = ((encode[26] & 0x1FFFFFF) >> 22);
        res21 |= ((encode[27] & 0x1FFFFFF) << 3);
        res21 |= ((encode[28] & 0x1FFFFFF) << 28);

        res22 = ((encode[28] & 0x1FFFFFF) >> 4);
        res22 |= ((encode[29] & 0x1FFFFFF) << 21);

        res23 = ((encode[29] & 0x1FFFFFF) >> 11);
        res23 |= ((encode[30] & 0x1FFFFFF) << 14);

        res24 = ((encode[30] & 0x1FFFFFF) >> 18);
        res24 |= ((encode[31] & 0x1FFFFFF) << 7);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
        dest[24] = res24;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 25);
    }
}

template <typename Type>
void pack_26(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 26) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;
        uint32_t res24;
        uint32_t res25;

        res0 = (encode[0] & 0x3FFFFFF);
        res0 |= ((encode[1] & 0x3FFFFFF) << 26);

        res1 = ((encode[1] & 0x3FFFFFF) >> 6);
        res1 |= ((encode[2] & 0x3FFFFFF) << 20);

        res2 = ((encode[2] & 0x3FFFFFF) >> 12);
        res2 |= ((encode[3] & 0x3FFFFFF) << 14);

        res3 = ((encode[3] & 0x3FFFFFF) >> 18);
        res3 |= ((encode[4] & 0x3FFFFFF) << 8);

        res4 = ((encode[4] & 0x3FFFFFF) >> 24);
        res4 |= ((encode[5] & 0x3FFFFFF) << 2);
        res4 |= ((encode[6] & 0x3FFFFFF) << 28);

        res5 = ((encode[6] & 0x3FFFFFF) >> 4);
        res5 |= ((encode[7] & 0x3FFFFFF) << 22);

        res6 = ((encode[7] & 0x3FFFFFF) >> 10);
        res6 |= ((encode[8] & 0x3FFFFFF) << 16);
        res7 = ((encode[8] & 0x3FFFFFF) >> 16);
        res7 |= ((encode[9] & 0x3FFFFFF) << 10);

        res8 = ((encode[9] & 0x3FFFFFF) >> 22);
        res8 |= ((encode[10] & 0x3FFFFFF) << 4);
        res8 |= ((encode[11] & 0x3FFFFFF) << 30);

        res9 = ((encode[11] & 0x3FFFFFF) >> 2);
        res9 |= ((encode[12] & 0x3FFFFFF) << 24);

        res10 = ((encode[12] & 0x3FFFFFF) >> 8);
        res10 |= ((encode[13] & 0x3FFFFFF) << 18);

        res11 = ((encode[13] & 0x3FFFFFF) >> 14);
        res11 |= ((encode[14] & 0x3FFFFFF) << 12);

        res12 = ((encode[14] & 0x3FFFFFF) >> 20);
        res12 |= ((encode[15] & 0x3FFFFFF) << 6);

        res13 = (encode[16] & 0x3FFFFFF);
        res13 |= ((encode[17] & 0x3FFFFFF) << 26);

        res14 = ((encode[17] & 0x3FFFFFF) >> 6);
        res14 |= ((encode[18] & 0x3FFFFFF) << 20);

        res15 = ((encode[18] & 0x3FFFFFF) >> 12);
        res15 |= ((encode[19] & 0x3FFFFFF) << 14);

        res16 = ((encode[19] & 0x3FFFFFF) >> 18);
        res16 |= ((encode[20] & 0x3FFFFFF) << 8);

        res17 = ((encode[20] & 0x3FFFFFF) >> 24);
        res17 |= ((encode[21] & 0x3FFFFFF) << 2);
        res17 |= ((encode[22] & 0x3FFFFFF) << 28);

        res18 = ((encode[22] & 0x3FFFFFF) >> 4);
        res18 |= ((encode[23] & 0x3FFFFFF) << 22);

        res19 = ((encode[23] & 0x3FFFFFF) >> 10);
        res19 |= ((encode[24] & 0x3FFFFFF) << 16);

        res20 = ((encode[24] & 0x3FFFFFF) >> 16);
        res20 |= ((encode[25] & 0x3FFFFFF) << 10);

        res21 = ((encode[25] & 0x3FFFFFF) >> 22);
        res21 |= ((encode[26] & 0x3FFFFFF) << 4);
        res21 |= ((encode[27] & 0x3FFFFFF) << 30);

        res22 = ((encode[27] & 0x3FFFFFF) >> 2);
        res22 |= ((encode[28] & 0x3FFFFFF) << 24);

        res23 = ((encode[28] & 0x3FFFFFF) >> 8);
        res23 |= ((encode[29] & 0x3FFFFFF) << 18);

        res24 = ((encode[29] & 0x3FFFFFF) >> 14);
        res24 |= ((encode[30] & 0x3FFFFFF) << 12);

        res25 = ((encode[30] & 0x3FFFFFF) >> 20);
        res25 |= ((encode[31] & 0x3FFFFFF) << 6);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
        dest[24] = res24;
        dest[25] = res25;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 26);
    }
}

template <typename Type>
void pack_27(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 27) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;
        uint32_t res24;
        uint32_t res25;
        uint32_t res26;

        res0 = (encode[0] & 0x7FFFFFF);
        res0 |= ((encode[1] & 0x7FFFFFF) << 27);

        res1 = ((encode[1] & 0x7FFFFFF) >> 5);
        res1 |= ((encode[2] & 0x7FFFFFF) << 22);

        res2 = ((encode[2] & 0x7FFFFFF) >> 10);
        res2 |= ((encode[3] & 0x7FFFFFF) << 17);

        res3 = ((encode[3] & 0x7FFFFFF) >> 15);
        res3 |= ((encode[4] & 0x7FFFFFF) << 12);

        res4 = ((encode[4] & 0x7FFFFFF) >> 20);
        res4 |= ((encode[5] & 0x7FFFFFF) << 7);

        res5 = ((encode[5] & 0x7FFFFFF) >> 25);
        res5 |= ((encode[6] & 0x7FFFFFF) << 2);
        res5 |= ((encode[7] & 0x7FFFFFF) << 29);

        res6 = ((encode[7] & 0x7FFFFFF) >> 3);
        res6 |= ((encode[8] & 0x7FFFFFF) << 24);

        res7 = ((encode[8] & 0x7FFFFFF) >> 8);
        res7 |= ((encode[9] & 0x7FFFFFF) << 19);

        res8 = ((encode[9] & 0x7FFFFFF) >> 13);
        res8 |= ((encode[10] & 0x7FFFFFF) << 14);

        res9 = ((encode[10] & 0x7FFFFFF) >> 18);
        res9 |= ((encode[11] & 0x7FFFFFF) << 9);

        res10 = ((encode[11] & 0x7FFFFFF) >> 23);
        res10 |= ((encode[12] & 0x7FFFFFF) << 4);
        res10 |= ((encode[13] & 0x7FFFFFF) << 31);

        res11 = ((encode[13] & 0x7FFFFFF) >> 1);
        res11 |= ((encode[14] & 0x7FFFFFF) << 26);

        res12 = ((encode[14] & 0x7FFFFFF) >> 6);
        res12 |= ((encode[15] & 0x7FFFFFF) << 21);

        res13 = ((encode[15] & 0x7FFFFFF) >> 11);
        res13 |= ((encode[16] & 0x7FFFFFF) << 16);

        res14 = ((encode[16] & 0x7FFFFFF) >> 16);
        res14 |= ((encode[17] & 0x7FFFFFF) << 11);

        res15 = ((encode[17] & 0x7FFFFFF) >> 21);
        res15 |= ((encode[18] & 0x7FFFFFF) << 6);

        res16 = ((encode[18] & 0x7FFFFFF) >> 26);
        res16 |= ((encode[19] & 0x7FFFFFF) << 1);
        res16 |= ((encode[20] & 0x7FFFFFF) << 28);

        res17 = ((encode[20] & 0x7FFFFFF) >> 4);
        res17 |= ((encode[21] & 0x7FFFFFF) << 23);

        res18 = ((encode[21] & 0x7FFFFFF) >> 9);
        res18 |= ((encode[22] & 0x7FFFFFF) << 18);

        res19 = ((encode[22] & 0x7FFFFFF) >> 14);
        res19 |= ((encode[23] & 0x7FFFFFF) << 13);

        res20 = ((encode[23] & 0x7FFFFFF) >> 19);
        res20 |= ((encode[24] & 0x7FFFFFF) << 8);

        res21 = ((encode[24] & 0x7FFFFFF) >> 24);
        res21 |= ((encode[25] & 0x7FFFFFF) << 3);
        res21 |= ((encode[26] & 0x7FFFFFF) << 30);

        res22 = ((encode[26] & 0x7FFFFFF) >> 2);
        res22 |= ((encode[27] & 0x7FFFFFF) << 25);

        res23 = ((encode[27] & 0x7FFFFFF) >> 7);
        res23 |= ((encode[28] & 0x7FFFFFF) << 20);

        res24 = ((encode[28] & 0x7FFFFFF) >> 12);
        res24 |= ((encode[29] & 0x7FFFFFF) << 15);

        res25 = ((encode[29] & 0x7FFFFFF) >> 17);
        res25 |= ((encode[30] & 0x7FFFFFF) << 10);

        res26 = ((encode[30] & 0x7FFFFFF) >> 22);
        res26 |= ((encode[31] & 0x7FFFFFF) << 5);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
        dest[24] = res24;
        dest[25] = res25;
        dest[26] = res26;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 27);
    }
}

template <typename Type>
void pack_28(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 28) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;
        uint32_t res24;
        uint32_t res25;
        uint32_t res26;
        uint32_t res27;

        res0 = (encode[0] & 0xFFFFFFF);
        res0 |= ((encode[1] & 0xFFFFFFF) << 28);

        res1 = ((encode[1] & 0xFFFFFFF) >> 4);
        res1 |= ((encode[2] & 0xFFFFFFF) << 24);

        res2 = ((encode[2] & 0xFFFFFFF) >> 8);
        res2 |= ((encode[3] & 0xFFFFFFF) << 20);

        res3 = ((encode[3] & 0xFFFFFFF) >> 12);
        res3 |= ((encode[4] & 0xFFFFFFF) << 16);

        res4 = ((encode[4] & 0xFFFFFFF) >> 16);
        res4 |= ((encode[5] & 0xFFFFFFF) << 12);

        res5 = ((encode[5] & 0xFFFFFFF) >> 20);
        res5 |= ((encode[6] & 0xFFFFFFF) << 8);

        res6 = ((encode[6] & 0xFFFFFFF) >> 24);
        res6 |= ((encode[7] & 0xFFFFFFF) << 4);

        res7 = (encode[8] & 0xFFFFFFF);
        res7 |= ((encode[9] & 0xFFFFFFF) << 28);

        res8 = ((encode[9] & 0xFFFFFFF) >> 4);
        res8 |= ((encode[10] & 0xFFFFFFF) << 24);

        res9 = ((encode[10] & 0xFFFFFFF) >> 8);
        res9 |= ((encode[11] & 0xFFFFFFF) << 20);

        res10 = ((encode[11] & 0xFFFFFFF) >> 12);
        res10 |= ((encode[12] & 0xFFFFFFF) << 16);

        res11 = ((encode[12] & 0xFFFFFFF) >> 16);
        res11 |= ((encode[13] & 0xFFFFFFF) << 12);

        res12 = ((encode[13] & 0xFFFFFFF) >> 20);
        res12 |= ((encode[14] & 0xFFFFFFF) << 8);

        res13 = ((encode[14] & 0xFFFFFFF) >> 24);
        res13 |= ((encode[15] & 0xFFFFFFF) << 4);

        res14 = (encode[16] & 0xFFFFFFF);
        res14 |= ((encode[17] & 0xFFFFFFF) << 28);

        res15 = ((encode[17] & 0xFFFFFFF) >> 4);
        res15 |= ((encode[18] & 0xFFFFFFF) << 24);

        res16 = ((encode[18] & 0xFFFFFFF) >> 8);
        res16 |= ((encode[19] & 0xFFFFFFF) << 20);

        res17 = ((encode[19] & 0xFFFFFFF) >> 12);
        res17 |= ((encode[20] & 0xFFFFFFF) << 16);

        res18 = ((encode[20] & 0xFFFFFFF) >> 16);
        res18 |= ((encode[21] & 0xFFFFFFF) << 12);

        res19 = ((encode[21] & 0xFFFFFFF) >> 20);
        res19 |= ((encode[22] & 0xFFFFFFF) << 8);

        res20 = ((encode[22] & 0xFFFFFFF) >> 24);
        res20 |= ((encode[23] & 0xFFFFFFF) << 4);

        res21 = (encode[24] & 0xFFFFFFF);
        res21 |= ((encode[25] & 0xFFFFFFF) << 28);

        res22 = ((encode[25] & 0xFFFFFFF) >> 4);
        res22 |= ((encode[26] & 0xFFFFFFF) << 24);

        res23 = ((encode[26] & 0xFFFFFFF) >> 8);
        res23 |= ((encode[27] & 0xFFFFFFF) << 20);

        res24 = ((encode[27] & 0xFFFFFFF) >> 12);
        res24 |= ((encode[28] & 0xFFFFFFF) << 16);

        res25 = ((encode[28] & 0xFFFFFFF) >> 16);
        res25 |= ((encode[29] & 0xFFFFFFF) << 12);

        res26 = ((encode[29] & 0xFFFFFFF) >> 20);
        res26 |= ((encode[30] & 0xFFFFFFF) << 8);

        res27 = ((encode[30] & 0xFFFFFFF) >> 24);
        res27 |= ((encode[31] & 0xFFFFFFF) << 4);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
        dest[24] = res24;
        dest[25] = res25;
        dest[26] = res26;
        dest[27] = res27;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 28);
    }
}

template <typename Type>
void pack_29(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 29) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;
        uint32_t res24;
        uint32_t res25;
        uint32_t res26;
        uint32_t res27;
        uint32_t res28;

        res0 = (encode[0] & 0x1FFFFFFF);
        res0 |= ((encode[1] & 0x1FFFFFFF) << 29);

        res1 = ((encode[1] & 0x1FFFFFFF) >> 3);
        res1 |= ((encode[2] & 0x1FFFFFFF) << 26);

        res2 = ((encode[2] & 0x1FFFFFFF) >> 6);
        res2 |= ((encode[3] & 0x1FFFFFFF) << 23);

        res3 = ((encode[3] & 0x1FFFFFFF) >> 9);
        res3 |= ((encode[4] & 0x1FFFFFFF) << 20);

        res4 = ((encode[4] & 0x1FFFFFFF) >> 12);
        res4 |= ((encode[5] & 0x1FFFFFFF) << 17);

        res5 = ((encode[5] & 0x1FFFFFFF) >> 15);
        res5 |= ((encode[6] & 0x1FFFFFFF) << 14);

        res6 = ((encode[6] & 0x1FFFFFFF) >> 18);
        res6 |= ((encode[7] & 0x1FFFFFFF) << 11);

        res7 = ((encode[7] & 0x1FFFFFFF) >> 21);
        res7 |= ((encode[8] & 0x1FFFFFFF) << 8);

        res8 = ((encode[8] & 0x1FFFFFFF) >> 24);
        res8 |= ((encode[9] & 0x1FFFFFFF) << 5);

        res9 = ((encode[9] & 0x1FFFFFFF) >> 27);
        res9 |= ((encode[10] & 0x1FFFFFFF) << 2);
        res9 |= ((encode[11] & 0x1FFFFFFF) << 31);

        res10 = ((encode[11] & 0x1FFFFFFF) >> 1);
        res10 |= ((encode[12] & 0x1FFFFFFF) << 28);

        res11 = ((encode[12] & 0x1FFFFFFF) >> 4);
        res11 |= ((encode[13] & 0x1FFFFFFF) << 25);

        res12 = ((encode[13] & 0x1FFFFFFF) >> 7);
        res12 |= ((encode[14] & 0x1FFFFFFF) << 22);

        res13 = ((encode[14] & 0x1FFFFFFF) >> 10);
        res13 |= ((encode[15] & 0x1FFFFFFF) << 19);

        res14 = ((encode[15] & 0x1FFFFFFF) >> 13);
        res14 |= ((encode[16] & 0x1FFFFFFF) << 16);

        res15 = ((encode[16] & 0x1FFFFFFF) >> 16);
        res15 |= ((encode[17] & 0x1FFFFFFF) << 13);

        res16 = ((encode[17] & 0x1FFFFFFF) >> 19);
        res16 |= ((encode[18] & 0x1FFFFFFF) << 10);

        res17 = ((encode[18] & 0x1FFFFFFF) >> 22);
        res17 |= ((encode[19] & 0x1FFFFFFF) << 7);

        res18 = ((encode[19] & 0x1FFFFFFF) >> 25);
        res18 |= ((encode[20] & 0x1FFFFFFF) << 4);

        res19 = ((encode[20] & 0x1FFFFFFF) >> 28);
        res19 |= ((encode[21] & 0x1FFFFFFF) << 1);
        res19 |= ((encode[22] & 0x1FFFFFFF) << 30);

        res20 = ((encode[22] & 0x1FFFFFFF) >> 2);
        res20 |= ((encode[23] & 0x1FFFFFFF) << 27);

        res21 = ((encode[23] & 0x1FFFFFFF) >> 5);
        res21 |= ((encode[24] & 0x1FFFFFFF) << 24);

        res22 = ((encode[24] & 0x1FFFFFFF) >> 8);
        res22 |= ((encode[25] & 0x1FFFFFFF) << 21);

        res23 = ((encode[25] & 0x1FFFFFFF) >> 11);
        res23 |= ((encode[26] & 0x1FFFFFFF) << 18);

        res24 = ((encode[26] & 0x1FFFFFFF) >> 14);
        res24 |= ((encode[27] & 0x1FFFFFFF) << 15);

        res25 = ((encode[27] & 0x1FFFFFFF) >> 17);
        res25 |= ((encode[28] & 0x1FFFFFFF) << 12);

        res26 = ((encode[28] & 0x1FFFFFFF) >> 20);
        res26 |= ((encode[29] & 0x1FFFFFFF) << 9);

        res27 = ((encode[29] & 0x1FFFFFFF) >> 23);
        res27 |= ((encode[30] & 0x1FFFFFFF) << 6);

        res28 = ((encode[30] & 0x1FFFFFFF) >> 26);
        res28 |= ((encode[31] & 0x1FFFFFFF) << 3);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
        dest[24] = res24;
        dest[25] = res25;
        dest[26] = res26;
        dest[27] = res27;
        dest[28] = res28;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 29);
    }
}

template <typename Type>
void pack_30(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 30) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;
        uint32_t res24;
        uint32_t res25;
        uint32_t res26;
        uint32_t res27;
        uint32_t res28;
        uint32_t res29;
        res0 = (encode[0] & 0x3FFFFFFF);
        res0 |= ((encode[1] & 0x3FFFFFFF) << 30);

        res1 = ((encode[1] & 0x3FFFFFFF) >> 2);
        res1 |= ((encode[2] & 0x3FFFFFFF) << 28);

        res2 = ((encode[2] & 0x3FFFFFFF) >> 4);
        res2 |= ((encode[3] & 0x3FFFFFFF) << 26);

        res3 = ((encode[3] & 0x3FFFFFFF) >> 6);
        res3 |= ((encode[4] & 0x3FFFFFFF) << 24);

        res4 = ((encode[4] & 0x3FFFFFFF) >> 8);
        res4 |= ((encode[5] & 0x3FFFFFFF) << 22);

        res5 = ((encode[5] & 0x3FFFFFFF) >> 10);
        res5 |= ((encode[6] & 0x3FFFFFFF) << 20);

        res6 = ((encode[6] & 0x3FFFFFFF) >> 12);
        res6 |= ((encode[7] & 0x3FFFFFFF) << 18);

        res7 = ((encode[7] & 0x3FFFFFFF) >> 14);
        res7 |= ((encode[8] & 0x3FFFFFFF) << 16);

        res8 = ((encode[8] & 0x3FFFFFFF) >> 16);
        res8 |= ((encode[9] & 0x3FFFFFFF) << 14);

        res9 = ((encode[9] & 0x3FFFFFFF) >> 18);
        res9 |= ((encode[10] & 0x3FFFFFFF) << 12);

        res10 = ((encode[10] & 0x3FFFFFFF) >> 20);
        res10 |= ((encode[11] & 0x3FFFFFFF) << 10);

        res11 = ((encode[11] & 0x3FFFFFFF) >> 22);
        res11 |= ((encode[12] & 0x3FFFFFFF) << 8);

        res12 = ((encode[12] & 0x3FFFFFFF) >> 24);
        res12 |= ((encode[13] & 0x3FFFFFFF) << 6);

        res13 = ((encode[13] & 0x3FFFFFFF) >> 26);
        res13 |= ((encode[14] & 0x3FFFFFFF) << 4);

        res14 = ((encode[14] & 0x3FFFFFFF) >> 28);
        res14 |= ((encode[15] & 0x3FFFFFFF) << 2);

        res15 = (encode[16] & 0x3FFFFFFF);
        res15 |= ((encode[17] & 0x3FFFFFFF) << 30);

        res16 = ((encode[17] & 0x3FFFFFFF) >> 2);
        res16 |= ((encode[18] & 0x3FFFFFFF) << 28);

        res17 = ((encode[18] & 0x3FFFFFFF) >> 4);
        res17 |= ((encode[19] & 0x3FFFFFFF) << 26);

        res18 = ((encode[19] & 0x3FFFFFFF) >> 6);
        res18 |= ((encode[20] & 0x3FFFFFFF) << 24);

        res19 = ((encode[20] & 0x3FFFFFFF) >> 8);
        res19 |= ((encode[21] & 0x3FFFFFFF) << 22);

        res20 = ((encode[21] & 0x3FFFFFFF) >> 10);
        res20 |= ((encode[22] & 0x3FFFFFFF) << 20);

        res21 = ((encode[22] & 0x3FFFFFFF) >> 12);
        res21 |= ((encode[23] & 0x3FFFFFFF) << 18);

        res22 = ((encode[23] & 0x3FFFFFFF) >> 14);
        res22 |= ((encode[24] & 0x3FFFFFFF) << 16);

        res23 = ((encode[24] & 0x3FFFFFFF) >> 16);
        res23 |= ((encode[25] & 0x3FFFFFFF) << 14);

        res24 = ((encode[25] & 0x3FFFFFFF) >> 18);
        res24 |= ((encode[26] & 0x3FFFFFFF) << 12);

        res25 = ((encode[26] & 0x3FFFFFFF) >> 20);
        res25 |= ((encode[27] & 0x3FFFFFFF) << 10);

        res26 = ((encode[27] & 0x3FFFFFFF) >> 22);
        res26 |= ((encode[28] & 0x3FFFFFFF) << 8);

        res27 = ((encode[28] & 0x3FFFFFFF) >> 24);
        res27 |= ((encode[29] & 0x3FFFFFFF) << 6);

        res28 = ((encode[29] & 0x3FFFFFFF) >> 26);
        res28 |= ((encode[30] & 0x3FFFFFFF) << 4);

        res29 = ((encode[30] & 0x3FFFFFFF) >> 28);
        res29 |= ((encode[31] & 0x3FFFFFFF) << 2);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
        dest[24] = res24;
        dest[25] = res25;
        dest[26] = res26;
        dest[27] = res27;
        dest[28] = res28;
        dest[29] = res29;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 30);
    }
}

template <typename Type>
void pack_31(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 31) {
        uint32_t res0;
        uint32_t res1;
        uint32_t res2;
        uint32_t res3;
        uint32_t res4;
        uint32_t res5;
        uint32_t res6;
        uint32_t res7;
        uint32_t res8;
        uint32_t res9;
        uint32_t res10;
        uint32_t res11;
        uint32_t res12;
        uint32_t res13;
        uint32_t res14;
        uint32_t res15;
        uint32_t res16;
        uint32_t res17;
        uint32_t res18;
        uint32_t res19;
        uint32_t res20;
        uint32_t res21;
        uint32_t res22;
        uint32_t res23;
        uint32_t res24;
        uint32_t res25;
        uint32_t res26;
        uint32_t res27;
        uint32_t res28;
        uint32_t res29;
        uint32_t res30;

        res0 = (encode[0] & 0x7FFFFFFF);
        res0 |= ((encode[1] & 0x7FFFFFFF) << 31);

        res1 = ((encode[1] & 0x7FFFFFFF) >> 1);
        res1 |= ((encode[2] & 0x7FFFFFFF) << 30);

        res2 = ((encode[2] & 0x7FFFFFFF) >> 2);
        res2 |= ((encode[3] & 0x7FFFFFFF) << 29);

        res3 = ((encode[3] & 0x7FFFFFFF) >> 3);
        res3 |= ((encode[4] & 0x7FFFFFFF) << 28);

        res4 = ((encode[4] & 0x7FFFFFFF) >> 4);
        res4 |= ((encode[5] & 0x7FFFFFFF) << 27);

        res5 = ((encode[5] & 0x7FFFFFFF) >> 5);
        res5 |= ((encode[6] & 0x7FFFFFFF) << 26);

        res6 = ((encode[6] & 0x7FFFFFFF) >> 6);
        res6 |= ((encode[7] & 0x7FFFFFFF) << 25);

        res7 = ((encode[7] & 0x7FFFFFFF) >> 7);
        res7 |= ((encode[8] & 0x7FFFFFFF) << 24);

        res8 = ((encode[8] & 0x7FFFFFFF) >> 8);
        res8 |= ((encode[9] & 0x7FFFFFFF) << 23);

        res9 = ((encode[9] & 0x7FFFFFFF) >> 9);
        res9 |= ((encode[10] & 0x7FFFFFFF) << 22);

        res10 = ((encode[10] & 0x7FFFFFFF) >> 10);
        res10 |= ((encode[11] & 0x7FFFFFFF) << 21);

        res11 = ((encode[11] & 0x7FFFFFFF) >> 11);
        res11 |= ((encode[12] & 0x7FFFFFFF) << 20);

        res12 = ((encode[12] & 0x7FFFFFFF) >> 12);
        res12 |= ((encode[13] & 0x7FFFFFFF) << 19);

        res13 = ((encode[13] & 0x7FFFFFFF) >> 13);
        res13 |= ((encode[14] & 0x7FFFFFFF) << 18);

        res14 = ((encode[14] & 0x7FFFFFFF) >> 14);
        res14 |= ((encode[15] & 0x7FFFFFFF) << 17);

        res15 = ((encode[15] & 0x7FFFFFFF) >> 15);
        res15 |= ((encode[16] & 0x7FFFFFFF) << 16);

        res16 = ((encode[16] & 0x7FFFFFFF) >> 16);
        res16 |= ((encode[17] & 0x7FFFFFFF) << 15);

        res17 = ((encode[17] & 0x7FFFFFFF) >> 17);
        res17 |= ((encode[18] & 0x7FFFFFFF) << 14);

        res18 = ((encode[18] & 0x7FFFFFFF) >> 18);
        res18 |= ((encode[19] & 0x7FFFFFFF) << 13);

        res19 = ((encode[19] & 0x7FFFFFFF) >> 19);
        res19 |= ((encode[20] & 0x7FFFFFFF) << 12);

        res20 = ((encode[20] & 0x7FFFFFFF) >> 20);
        res20 |= ((encode[21] & 0x7FFFFFFF) << 11);

        res21 = ((encode[21] & 0x7FFFFFFF) >> 21);
        res21 |= ((encode[22] & 0x7FFFFFFF) << 10);

        res22 = ((encode[22] & 0x7FFFFFFF) >> 22);
        res22 |= ((encode[23] & 0x7FFFFFFF) << 9);

        res23 = ((encode[23] & 0x7FFFFFFF) >> 23);
        res23 |= ((encode[24] & 0x7FFFFFFF) << 8);

        res24 = ((encode[24] & 0x7FFFFFFF) >> 24);
        res24 |= ((encode[25] & 0x7FFFFFFF) << 7);

        res25 = ((encode[25] & 0x7FFFFFFF) >> 25);
        res25 |= ((encode[26] & 0x7FFFFFFF) << 6);

        res26 = ((encode[26] & 0x7FFFFFFF) >> 26);
        res26 |= ((encode[27] & 0x7FFFFFFF) << 5);

        res27 = ((encode[27] & 0x7FFFFFFF) >> 27);
        res27 |= ((encode[28] & 0x7FFFFFFF) << 4);

        res28 = ((encode[28] & 0x7FFFFFFF) >> 28);
        res28 |= ((encode[29] & 0x7FFFFFFF) << 3);

        res29 = ((encode[29] & 0x7FFFFFFF) >> 29);
        res29 |= ((encode[30] & 0x7FFFFFFF) << 2);

        res30 = ((encode[30] & 0x7FFFFFFF) >> 30);
        res30 |= ((encode[31] & 0x7FFFFFFF) << 1);

        dest[0] = res0;
        dest[1] = res1;
        dest[2] = res2;
        dest[3] = res3;
        dest[4] = res4;
        dest[5] = res5;
        dest[6] = res6;
        dest[7] = res7;
        dest[8] = res8;
        dest[9] = res9;
        dest[10] = res10;
        dest[11] = res11;
        dest[12] = res12;
        dest[13] = res13;
        dest[14] = res14;
        dest[15] = res15;
        dest[16] = res16;
        dest[17] = res17;
        dest[18] = res18;
        dest[19] = res19;
        dest[20] = res20;
        dest[21] = res21;
        dest[22] = res22;
        dest[23] = res23;
        dest[24] = res24;
        dest[25] = res25;
        dest[26] = res26;
        dest[27] = res27;
        dest[28] = res28;
        dest[29] = res29;
        dest[30] = res30;
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 31);
    }
}

template <typename Type>
void pack_32(uint32_t* dest, const Type* encode, uint32_t n)
{
    for (uint32_t i = 32; i <= n; i += 32, encode += 32, dest += 32) {
        dest[0] = encode[0];
        dest[1] = encode[1];
        dest[2] = encode[2];
        dest[3] = encode[3];
        dest[4] = encode[4];
        dest[5] = encode[5];
        dest[6] = encode[6];
        dest[7] = encode[7];
        dest[8] = encode[8];
        dest[9] = encode[9];
        dest[10] = encode[10];
        dest[11] = encode[11];
        dest[12] = encode[12];
        dest[13] = encode[13];
        dest[14] = encode[14];
        dest[15] = encode[15];
        dest[16] = encode[16];
        dest[17] = encode[17];
        dest[18] = encode[18];
        dest[19] = encode[19];
        dest[20] = encode[20];
        dest[21] = encode[21];
        dest[22] = encode[22];
        dest[23] = encode[23];
        dest[24] = encode[24];
        dest[25] = encode[25];
        dest[26] = encode[26];
        dest[27] = encode[27];
        dest[28] = encode[28];
        dest[29] = encode[29];
        dest[30] = encode[30];
        dest[31] = encode[31];
    }
    uint32_t rest = n % 32;
    if (rest > 0) {
        pack(dest, encode, rest, 32);
    }
}

}
