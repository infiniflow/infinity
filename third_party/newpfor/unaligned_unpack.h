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

template <typename Type>
inline void unaligned_unpack_0(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    for (uint32_t i = 0; i < dataNum; ++i) {
        decode[i] = 0;
    }
}

template <typename Type>
inline void unaligned_unpack_1(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;

    uint32_t val = encode[offset++];

    decode[0] = (val << 31) >> 31;
    if (--rest == 0)
        return;
    decode[1] = (val << 30) >> 31;
    if (--rest == 0)
        return;
    decode[2] = (val << 29) >> 31;
    if (--rest == 0)
        return;
    decode[3] = (val << 28) >> 31;
    if (--rest == 0)
        return;
    decode[4] = (val << 27) >> 31;
    if (--rest == 0)
        return;
    decode[5] = (val << 26) >> 31;
    if (--rest == 0)
        return;
    decode[6] = (val << 25) >> 31;
    if (--rest == 0)
        return;
    decode[7] = (val << 24) >> 31;
    if (--rest == 0)
        return;
    decode[8] = (val << 23) >> 31;
    if (--rest == 0)
        return;
    decode[9] = (val << 22) >> 31; // 10
    if (--rest == 0)
        return;
    decode[10] = (val << 21) >> 31;
    if (--rest == 0)
        return;
    decode[11] = (val << 20) >> 31;
    if (--rest == 0)
        return;
    decode[12] = (val << 19) >> 31;
    if (--rest == 0)
        return;
    decode[13] = (val << 18) >> 31;
    if (--rest == 0)
        return;
    decode[14] = (val << 17) >> 31;
    if (--rest == 0)
        return;
    decode[15] = (val << 16) >> 31;
    if (--rest == 0)
        return;
    decode[16] = (val << 15) >> 31;
    if (--rest == 0)
        return;
    decode[17] = (val << 14) >> 31;
    if (--rest == 0)
        return;
    decode[18] = (val << 13) >> 31;
    if (--rest == 0)
        return;
    decode[19] = (val << 12) >> 31; // 20
    if (--rest == 0)
        return;
    decode[20] = (val << 11) >> 31;
    if (--rest == 0)
        return;
    decode[21] = (val << 10) >> 31;
    if (--rest == 0)
        return;
    decode[22] = (val << 9) >> 31;
    if (--rest == 0)
        return;
    decode[23] = (val << 8) >> 31;
    if (--rest == 0)
        return;
    decode[24] = (val << 7) >> 31;
    if (--rest == 0)
        return;
    decode[25] = (val << 6) >> 31;
    if (--rest == 0)
        return;
    decode[26] = (val << 5) >> 31;
    if (--rest == 0)
        return;
    decode[27] = (val << 4) >> 31;
    if (--rest == 0)
        return;
    decode[28] = (val << 3) >> 31;
    if (--rest == 0)
        return;
    decode[29] = (val << 2) >> 31;
    if (--rest == 0)
        return;
    decode[30] = (val << 1) >> 31;
    return;
}

template <typename Type>
inline void unaligned_unpack_2(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;
    uint32_t decodedOffset = 0;

    uint32_t val = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16) {
        val = encode[offset++];

        decode[0] = (val << 30) >> 30;
        decode[1] = (val << 28) >> 30;
        decode[2] = (val << 26) >> 30;
        decode[3] = (val << 24) >> 30;
        decode[4] = (val << 22) >> 30;
        decode[5] = (val << 20) >> 30;
        decode[6] = (val << 18) >> 30;
        decode[7] = (val << 16) >> 30;
        decode[8] = (val << 14) >> 30;
        decode[9] = (val << 12) >> 30;
        decode[10] = (val << 10) >> 30;
        decode[11] = (val << 8) >> 30;
        decode[12] = (val << 6) >> 30;
        decode[13] = (val << 4) >> 30;
        decode[14] = (val << 2) >> 30;
        decode[15] = val >> 30;
    }

    if (rest == 0)
        return;

    val = encode[offset++];
    decode[decodedOffset] = (val << 30) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 28) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 26) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 24) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 22) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 20) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 18) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 16) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 14) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 12) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 10) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 8) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 6) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 4) >> 30;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val << 2) >> 30;
    return;
}

template <typename Type>
inline void unaligned_unpack_3(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 29) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 26) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 23) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 20) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 17) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 14) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 11) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 8) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 5) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 2) >> 29;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 31) >> 29) | (val1 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 28) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 25) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 22) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 19) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 16) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 13) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 10) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 7) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 4) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 1) >> 29;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 30) >> 29) | (val2 >> 31);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 27) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 24) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 21) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 18) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 15) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 12) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 9) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 6) >> 29;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 3) >> 29;
    return;
}

template <typename Type>
inline void unaligned_unpack_4(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 3;
    uint32_t rest = dataNum % 8;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 8) {
        uint32_t val1 = encode[offset++];

        decode[0] = (val1 << 28) >> 28;
        decode[1] = (val1 << 24) >> 28;
        decode[2] = (val1 << 20) >> 28;
        decode[3] = (val1 << 16) >> 28;
        decode[4] = (val1 << 12) >> 28;
        decode[5] = (val1 << 8) >> 28;
        decode[6] = (val1 << 4) >> 28;
        decode[7] = val1 >> 28;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];

    decode[decodedOffset] = (val1 << 28) >> 28;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 24) >> 28;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 20) >> 28;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 16) >> 28;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 12) >> 28;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 8) >> 28;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 4) >> 28;
    return;
}

template <typename Type>
inline void unaligned_unpack_5(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 27) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 22) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 17) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 12) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 7) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 2) >> 27;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 29) >> 27) | (val1 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 24) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 19) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 14) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 9) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 4) >> 27;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 31) >> 27) | (val2 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 26) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 21) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 16) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 11) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 6) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 1) >> 27;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 28) >> 27) | (val3 >> 31);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 23) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 18) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 13) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 8) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 3) >> 27;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 30) >> 27) | (val4 >> 29);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 25) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 20) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 15) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 10) >> 27;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 5) >> 27;
    return;
}

template <typename Type>
inline void unaligned_unpack_6(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];

        decode[0] = (val1 << 26) >> 26;
        decode[1] = (val1 << 20) >> 26;
        decode[2] = (val1 << 14) >> 26;
        decode[3] = (val1 << 8) >> 26;
        decode[4] = (val1 << 2) >> 26;

        decode[5] = ((val2 << 28) >> 26) | (val1 >> 30);
        decode[6] = (val2 << 22) >> 26;
        decode[7] = (val2 << 16) >> 26;
        decode[8] = (val2 << 10) >> 26;
        decode[9] = (val2 << 4) >> 26;

        decode[10] = ((val3 << 30) >> 26) | (val2 >> 28);
        decode[11] = (val3 << 24) >> 26;
        decode[12] = (val3 << 18) >> 26;
        decode[13] = (val3 << 12) >> 26;
        decode[14] = (val3 << 6) >> 26;
        decode[15] = val3 >> 26;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 26) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 20) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 14) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 8) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 2) >> 26;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 28) >> 26) | (val1 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 22) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 16) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 10) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 4) >> 26;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 30) >> 26) | (val2 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 24) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 18) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 12) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 6) >> 26;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = val3 >> 26;
    return;
}

template <typename Type>
inline void unaligned_unpack_7(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 25) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 18) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 11) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 4) >> 25;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 29) >> 25) | (val1 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 22) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 15) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 8) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 1) >> 25;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 26) >> 25) | (val2 >> 31);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 19) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 12) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 5) >> 25;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 30) >> 25) | (val3 >> 27);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 23) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 16) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 9) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 2) >> 25;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 27) >> 25) | (val4 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 20) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 13) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 6) >> 25;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 31) >> 25) | (val5 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 24) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 17) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 10) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 3) >> 25;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 28) >> 25) | (val6 >> 29);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 21) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 14) >> 25;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 7) >> 25;
    return;
}

template <typename Type>
inline void unaligned_unpack_8(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 2;
    uint32_t rest = dataNum % 4;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 4) {
        uint32_t val1 = encode[offset++];

        decode[0] = (val1 << 24) >> 24;
        decode[1] = (val1 << 16) >> 24;
        decode[2] = (val1 << 8) >> 24;
        decode[3] = val1 >> 24;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];

    decode[decodedOffset] = (val1 << 24) >> 24;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 16) >> 24;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 8) >> 24;
    return;
}

template <typename Type>
inline void unaligned_unpack_9(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 23) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 14) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 5) >> 23;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 28) >> 23) | (val1 >> 27);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 19) >> 23; // 5
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 10) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 1) >> 23;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 24) >> 23) | (val2 >> 31);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 15) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 6) >> 23; // 10
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 29) >> 23) | (val3 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 20) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 11) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 2) >> 23;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 25) >> 23) | (val4 >> 30); // 15
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 16) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 7) >> 23;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 30) >> 23) | (val5 >> 25);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 21) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 12) >> 23; // 20
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 3) >> 23;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 26) >> 23) | (val6 >> 29);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 17) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 8) >> 23;
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 31) >> 23) | (val7 >> 24); // 25
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 22) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 13) >> 23;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 4) >> 23;
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 27) >> 23) | (val8 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 18) >> 23; // 30
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 9) >> 23;
    return;
}

template <typename Type>
inline void unaligned_unpack_10(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];
        uint32_t val4 = encode[offset++];
        uint32_t val5 = encode[offset++];

        decode[0] = (val1 << 22) >> 22;
        decode[1] = (val1 << 12) >> 22;
        decode[2] = (val1 << 2) >> 22;
        decode[3] = ((val2 << 24) >> 22) | (val1 >> 30);

        decode[4] = (val2 << 14) >> 22;
        decode[5] = (val2 << 4) >> 22;
        decode[6] = ((val3 << 26) >> 22) | (val2 >> 28);

        decode[7] = (val3 << 16) >> 22;
        decode[8] = (val3 << 6) >> 22;
        decode[9] = ((val4 << 28) >> 22) | (val3 >> 26);

        decode[10] = (val4 << 18) >> 22;
        decode[11] = (val4 << 8) >> 22;
        decode[12] = ((val5 << 30) >> 22) | (val4 >> 24);

        decode[13] = (val5 << 20) >> 22;
        decode[14] = (val5 << 10) >> 22;
        decode[15] = val5 >> 22;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 22) >> 22;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 12) >> 22;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 2) >> 22;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 24) >> 22) | (val1 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 14) >> 22;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 4) >> 22;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 26) >> 22) | (val2 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 16) >> 22;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 6) >> 22;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 28) >> 22) | (val3 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 18) >> 22;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 8) >> 22;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 30) >> 22) | (val4 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 20) >> 22;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 10) >> 22;
    return;
}

template <typename Type>
inline void unaligned_unpack_11(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 21) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 10) >> 21;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 31) >> 21) | (val1 >> 22);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 20) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 9) >> 21;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 30) >> 21) | (val2 >> 23);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 19) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 8) >> 21;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 29) >> 21) | (val3 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 18) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 7) >> 21;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 28) >> 21) | (val4 >> 25);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 17) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 6) >> 21;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 27) >> 21) | (val5 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 16) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 5) >> 21;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 26) >> 21) | (val6 >> 27);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 15) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 4) >> 21;
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 25) >> 21) | (val7 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 14) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 3) >> 21;
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 24) >> 21) | (val8 >> 29);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 13) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 2) >> 21;
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[++decodedOffset] = ((val10 << 23) >> 21) | (val9 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val10 << 12) >> 21;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val10 << 1) >> 21;
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[++decodedOffset] = ((val11 << 22) >> 21) | (val10 >> 31);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val11 << 11) >> 21;
    return;
}

template <typename Type>
inline void unaligned_unpack_12(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 3;
    uint32_t rest = dataNum % 8;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 8) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];

        decode[0] = (val1 << 20) >> 20;
        decode[1] = (val1 << 8) >> 20;
        decode[2] = ((val2 << 28) >> 20) | (val1 >> 24);

        decode[3] = (val2 << 16) >> 20;
        decode[4] = (val2 << 4) >> 20;
        decode[5] = ((val3 << 24) >> 20) | (val2 >> 28);

        decode[6] = (val3 << 12) >> 20;
        decode[7] = val3 >> 20;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 20) >> 20;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 8) >> 20;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 28) >> 20) | (val1 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 16) >> 20;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 4) >> 20;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 24) >> 20) | (val2 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 12) >> 20;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = val3 >> 20;
    return;
}

template <typename Type>
inline void unaligned_unpack_13(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 19) >> 19;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 6) >> 19;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 25) >> 19) | (val1 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 12) >> 19;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 31) >> 19) | (val2 >> 20);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 18) >> 19;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 5) >> 19;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 24) >> 19) | (val3 >> 27);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 11) >> 19;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 30) >> 19) | (val4 >> 21);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 17) >> 19;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 4) >> 19;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 23) >> 19) | (val5 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 10) >> 19;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 29) >> 19) | (val6 >> 22);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 16) >> 19;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 3) >> 19;
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 22) >> 19) | (val7 >> 29);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 9) >> 19;
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 28) >> 19) | (val8 >> 23);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 15) >> 19;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 2) >> 19;
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[++decodedOffset] = ((val10 << 21) >> 19) | (val9 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val10 << 8) >> 19;
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[++decodedOffset] = ((val11 << 27) >> 19) | (val10 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val11 << 14) >> 19;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val11 << 1) >> 19;
    if (--rest == 0)
        return;

    uint32_t val12 = encode[offset++];
    decode[++decodedOffset] = ((val12 << 20) >> 19) | (val11 >> 31);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val12 << 7) >> 19;
    if (--rest == 0)
        return;

    uint32_t val13 = encode[offset++];
    decode[++decodedOffset] = ((val13 << 26) >> 19) | (val12 >> 25);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val13 << 13) >> 19;
    return;
}

template <typename Type>
inline void unaligned_unpack_14(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];
        uint32_t val4 = encode[offset++];
        uint32_t val5 = encode[offset++];
        uint32_t val6 = encode[offset++];
        uint32_t val7 = encode[offset++];

        decode[0] = (val1 << 18) >> 18;
        decode[1] = (val1 << 4) >> 18;
        decode[2] = ((val2 << 22) >> 18) | (val1 >> 28);

        decode[3] = (val2 << 8) >> 18;
        decode[4] = ((val3 << 26) >> 18) | (val2 >> 24);

        decode[5] = (val3 << 12) >> 18;
        decode[6] = ((val4 << 30) >> 18) | (val3 >> 20);

        decode[7] = (val4 << 16) >> 18;
        decode[8] = (val4 << 2) >> 18;
        decode[9] = ((val5 << 20) >> 18) | (val4 >> 30);

        decode[10] = (val5 << 6) >> 18;
        decode[11] = ((val6 << 24) >> 18) | (val5 >> 26);

        decode[12] = (val6 << 10) >> 18;
        decode[13] = ((val7 << 28) >> 18) | (val6 >> 22);

        decode[14] = (val7 << 14) >> 18;
        decode[15] = val7 >> 18;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 18) >> 18;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 4) >> 18;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 22) >> 18) | (val1 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 8) >> 18;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 26) >> 18) | (val2 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 12) >> 18;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 30) >> 18) | (val3 >> 20);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 16) >> 18;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 2) >> 18;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 20) >> 18) | (val4 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 6) >> 18;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 24) >> 18) | (val5 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 10) >> 18;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 28) >> 18) | (val6 >> 22);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 14) >> 18;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = val7 >> 18;
    return;
}

template <typename Type>
inline void unaligned_unpack_15(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 17) >> 17;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val1 << 2) >> 17;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 19) >> 17) | (val1 >> 30);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 4) >> 17;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 21) >> 17) | (val2 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 6) >> 17;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 23) >> 17) | (val3 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 8) >> 17;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 25) >> 17) | (val4 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 10) >> 17;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 27) >> 17) | (val5 >> 22);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 12) >> 17;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 29) >> 17) | (val6 >> 20);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 14) >> 17;
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 31) >> 17) | (val7 >> 18);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 16) >> 17;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 1) >> 17;
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 18) >> 17) | (val8 >> 31);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 3) >> 17;
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[++decodedOffset] = ((val10 << 20) >> 17) | (val9 >> 29);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val10 << 5) >> 17;
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[++decodedOffset] = ((val11 << 22) >> 17) | (val10 >> 27);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val11 << 7) >> 17;
    if (--rest == 0)
        return;

    uint32_t val12 = encode[offset++];
    decode[++decodedOffset] = ((val12 << 24) >> 17) | (val11 >> 25);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val12 << 9) >> 17;
    if (--rest == 0)
        return;

    uint32_t val13 = encode[offset++];
    decode[++decodedOffset] = ((val13 << 26) >> 17) | (val12 >> 23);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val13 << 11) >> 17;
    if (--rest == 0)
        return;

    uint32_t val14 = encode[offset++];
    decode[++decodedOffset] = ((val14 << 28) >> 17) | (val13 >> 21);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val14 << 13) >> 17;
    if (--rest == 0)
        return;

    uint32_t val15 = encode[offset++];
    decode[++decodedOffset] = ((val15 << 30) >> 17) | (val14 >> 19);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val15 << 15) >> 17;
    if (--rest == 0)
        return;
    decode[++decodedOffset] = val15 >> 17;
    return;
}

template <typename Type>
inline void unaligned_unpack_16(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t decodedOffset = 0;
    uint32_t maxBlocks = dataNum >> 1;
    uint32_t rest = dataNum % 2;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 2) {
        uint32_t val1 = encode[offset++];

        decode[0] = (val1 << 16) >> 16;
        decode[1] = (val1 >> 16);
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 16) >> 16;
    return;
}

template <typename Type>
inline void unaligned_unpack_17(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 15) >> 15;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 30) >> 15) | (val1 >> 17);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 13) >> 15;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 28) >> 15) | (val2 >> 19);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 11) >> 15;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 26) >> 15) | (val3 >> 21);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 9) >> 15;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 24) >> 15) | (val4 >> 23);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 7) >> 15;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 22) >> 15) | (val5 >> 25);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 5) >> 15;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 20) >> 15) | (val6 >> 27);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 3) >> 15;
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 18) >> 15) | (val7 >> 29);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 1) >> 15;
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 16) >> 15) | (val8 >> 31);
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[++decodedOffset] = ((val10 << 31) >> 15) | (val9 >> 16);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val10 << 14) >> 15;
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[++decodedOffset] = ((val11 << 29) >> 15) | (val10 >> 18);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val11 << 12) >> 15;
    if (--rest == 0)
        return;

    uint32_t val12 = encode[offset++];
    decode[++decodedOffset] = ((val12 << 27) >> 15) | (val11 >> 20);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val12 << 10) >> 15;
    if (--rest == 0)
        return;

    uint32_t val13 = encode[offset++];
    decode[++decodedOffset] = ((val13 << 25) >> 15) | (val12 >> 22);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val13 << 8) >> 15;
    if (--rest == 0)
        return;

    uint32_t val14 = encode[offset++];
    decode[++decodedOffset] = ((val14 << 23) >> 15) | (val13 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val14 << 6) >> 15;
    if (--rest == 0)
        return;

    uint32_t val15 = encode[offset++];
    decode[++decodedOffset] = ((val15 << 21) >> 15) | (val14 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val15 << 4) >> 15;
    if (--rest == 0)
        return;

    uint32_t val16 = encode[offset++];
    decode[++decodedOffset] = ((val16 << 19) >> 15) | (val15 >> 28);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val16 << 2) >> 15;
    if (--rest == 0)
        return;

    uint32_t val17 = encode[offset++];
    decode[++decodedOffset] = ((val17 << 17) >> 15) | (val16 >> 30);
    return;
}

template <typename Type>
inline void unaligned_unpack_18(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];
        uint32_t val4 = encode[offset++];
        uint32_t val5 = encode[offset++];
        uint32_t val6 = encode[offset++];
        uint32_t val7 = encode[offset++];
        uint32_t val8 = encode[offset++];
        uint32_t val9 = encode[offset++];

        decode[0] = (val1 << 14) >> 14;
        decode[1] = ((val2 << 28) >> 14) | (val1 >> 18);

        decode[2] = (val2 << 10) >> 14;
        decode[3] = ((val3 << 24) >> 14) | (val2 >> 22);

        decode[4] = (val3 << 6) >> 14;
        decode[5] = ((val4 << 20) >> 14) | (val3 >> 26);

        decode[6] = (val4 << 2) >> 14;
        decode[7] = ((val5 << 16) >> 14) | (val4 >> 30);

        decode[8] = ((val6 << 30) >> 14) | (val5 >> 16);

        decode[9] = (val6 << 12) >> 14;
        decode[10] = ((val7 << 26) >> 14) | (val6 >> 20);

        decode[11] = (val7 << 8) >> 14;
        decode[12] = ((val8 << 22) >> 14) | (val7 >> 24);

        decode[13] = (val8 << 4) >> 14;
        decode[14] = ((val9 << 18) >> 14) | (val8 >> 28);

        decode[15] = val9 >> 14;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 14) >> 14;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 28) >> 14) | (val1 >> 18);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 10) >> 14;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 24) >> 14) | (val2 >> 22);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val3 << 6) >> 14;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 20) >> 14) | (val3 >> 26);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 2) >> 14;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 16) >> 14) | (val4 >> 30);
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 30) >> 14) | (val5 >> 16);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 12) >> 14;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 26) >> 14) | (val6 >> 20);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val7 << 8) >> 14;
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 22) >> 14) | (val7 >> 24);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 4) >> 14;
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 18) >> 14) | (val8 >> 28);
    return;
}

template <typename Type>
inline void unaligned_unpack_19(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t rest = dataNum;

    decode[0] = encode[0] & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[1] = ((encode[0] >> 19) | (encode[1] << 13)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[2] = (encode[1] >> 6) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[3] = ((encode[1] >> 25) | (encode[2] << 7)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[4] = (encode[2] >> 12) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[5] = ((encode[2] >> 31) | (encode[3] << 1)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[6] = ((encode[3] >> 18) | (encode[4] << 14)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[7] = (encode[4] >> 5) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[8] = ((encode[4] >> 24) | (encode[5] << 8)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[9] = (encode[5] >> 11) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[10] = ((encode[5] >> 30) | (encode[6] << 2)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[11] = ((encode[6] >> 17) | (encode[7] << 15)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[12] = (encode[7] >> 4) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[13] = ((encode[7] >> 23) | (encode[8] << 9)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[14] = (encode[8] >> 10) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[15] = ((encode[8] >> 29) | (encode[9] << 3)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[16] = ((encode[9] >> 16) | (encode[10] << 16)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[17] = (encode[10] >> 3) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[18] = ((encode[10] >> 22) | (encode[11] << 10)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[19] = (encode[11] >> 9) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[20] = ((encode[11] >> 28) | (encode[12] << 4)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[21] = ((encode[12] >> 15) | (encode[13] << 17)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[22] = (encode[13] >> 2) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[23] = ((encode[13] >> 21) | (encode[14] << 11)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[24] = (encode[14] >> 8) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[25] = ((encode[14] >> 27) | (encode[15] << 5)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[26] = ((encode[15] >> 14) | (encode[16] << 18)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[27] = (encode[16] >> 1) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[28] = ((encode[16] >> 20) | (encode[17] << 12)) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[29] = (encode[17] >> 7) & 0x7FFFF;
    if (--rest == 0)
        return;
    decode[30] = ((encode[17] >> 26) | (encode[18] << 6)) & 0x7FFFF;

    // uint32_t offset = 0;
    // uint32_t rest = dataNum;
    // uint32_t val1  = encode[offset++];
    // decode[0] = (val1 << 13) >> 13;
    // if(--rest == 0) return;

    // uint32_t val2  = encode[offset++];
    // decode[1] = ((val2 << 26) >> 13) | (val1 >> 19);
    // if(--rest == 0) return;
    // decode[2] = (val2 << 7) >> 13;
    // if(--rest == 0) return;

    // uint32_t val3  = encode[offset++];
    // decode[3] = ((val3 << 20) >> 13) | (val2 >> 25);
    // if(--rest == 0) return;
    // decode[4] = (val3 << 1 ) >> 13;
    // if(--rest == 0) return;

    // uint32_t val4  = encode[offset++];
    // decode[5] = ((val4 << 14) >> 13) | (val3 >> 31);
    // if(--rest == 0) return;

    // uint32_t val5  = encode[offset++];
    // decode[6] = ((val5 << 27) >> 13) | (val4 >> 18);
    // if(--rest == 0) return;
    // decode[7] = (val5 << 8 ) >> 13;
    // if(--rest == 0) return;

    // uint32_t val6  = encode[offset++];
    // decode[8] = ((val6 << 8 ) | (val5 >> 24)) >> 13;//((val6 << 8 ) >> 13) | (val5 >> 24);
    // if(--rest == 0) return;
    // decode[9] = (val6 << 2) >> 13;
    // if(--rest == 0) return;

    // uint32_t val7  = encode[offset++];
    // decode[10] = ((val7 << 15) >> 13) | (val6 >> 30);
    // if(--rest == 0) return;

    // uint32_t val8  = encode[offset++];
    // decode[11] = ((val8 << 28) >> 13) | (val7 >> 17);
    // if(--rest == 0) return;
    // decode[12] = (val8 << 9) >> 13;
    // if(--rest == 0) return;

    // uint32_t val9  = encode[offset++];
    // decode[13] = ((val9 << 22) >> 13) | (val8 >> 23);
    // if(--rest == 0) return;
    // decode[14] = (val9 << 3) >> 13;
    // if(--rest == 0) return;

    // uint32_t val10 = encode[offset++];
    // decode[15] = ((val10 << 16) >> 13) | (val9 >> 29);
    // if(--rest == 0) return;

    // uint32_t val11 = encode[offset++];
    // decode[16] = ((val11 << 29) >> 13) | (val10 >> 16);
    // if(--rest == 0) return;
    // decode[17] = (val11 << 10) >> 13;
    // if(--rest == 0) return;

    // uint32_t val12 = encode[offset++];
    // decode[18] = ((val12 << 23) >> 13) | (val11 >> 22);
    // if(--rest == 0) return;
    // decode[19] = (val12 << 4) >> 13;
    // if(--rest == 0) return;

    // uint32_t val13 = encode[offset++];
    // decode[20] = ((val13 << 17) >> 13) | (val12 >> 28);
    // if(--rest == 0) return;

    // uint32_t val14 = encode[offset++];
    // decode[21] = ((val14 << 30) >> 13) | (val13 >> 15);
    // if(--rest == 0) return;
    // decode[22] = (val14 << 11) >> 13;
    // if(--rest == 0) return;

    // uint32_t val15 = encode[offset++];
    // decode[23] = ((val15 << 24) >> 13) | (val14 >> 21);
    // if(--rest == 0) return;
    // decode[24] = (val15 << 5) >> 13;
    // if(--rest == 0) return;

    // uint32_t val16 = encode[offset++];
    // decode[25] = ((val16 << 18) >> 13) | (val15 >> 27);
    // if(--rest == 0) return;

    // uint32_t val17 = encode[offset++];
    // decode[26] = ((val17 << 31) >> 13) | (val16 >> 14);
    // if(--rest == 0) return;
    // decode[27] = (val17 << 12) >> 13;
    // if(--rest == 0) return;

    // uint32_t val18 = encode[offset++];
    // decode[28] = ((val18 << 25) >> 13) | (val17 >> 20);
    // if(--rest == 0) return;
    // decode[29] = (val18 << 6) >> 13;
    // if(--rest == 0) return;

    // uint32_t val19 = encode[offset++];
    // decode[30] = ((val19 << 19 ) >> 13) | (val18 >> 26);
    // return;
}

template <typename Type>
inline void unaligned_unpack_20(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 3;
    uint32_t rest = dataNum % 8;

    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 8) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];
        uint32_t val4 = encode[offset++];
        uint32_t val5 = encode[offset++];

        decode[0] = (val1 << 12) >> 12;
        decode[1] = ((val2 << 24) >> 12) | (val1 >> 20);

        decode[2] = (val2 << 4) >> 12;
        decode[3] = ((val3 << 16) >> 12) | (val2 >> 28);

        decode[4] = ((val4 << 28) >> 12) | (val3 >> 16);

        decode[5] = (val4 << 8) >> 12;
        decode[6] = ((val5 << 20) >> 12) | (val4 >> 24);

        decode[7] = val5 >> 12;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 12) >> 12;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 24) >> 12) | (val1 >> 20);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val2 << 4) >> 12;
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 16) >> 12) | (val2 >> 28);
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 28) >> 12) | (val3 >> 16);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 8) >> 12;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 20) >> 12) | (val4 >> 24);
    return;
}

template <typename Type>
inline void unaligned_unpack_21(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 11) >> 11;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 22) >> 11) | (val1 >> 21);
    if (--rest == 0)
        return;

    decode[++decodedOffset] = (val2 << 1) >> 11;
    if (--rest == 0)
        return;
    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 12) >> 11) | (val2 >> 31);
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 23) >> 11) | (val3 >> 20);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val4 << 2) >> 11;
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 13) >> 11) | (val4 >> 30);
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 24) >> 11) | (val5 >> 19);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val6 << 3) >> 11;
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 14) >> 11) | (val6 >> 29);
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 25) >> 11) | (val7 >> 18);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val8 << 4) >> 11;
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 15) >> 11) | (val8 >> 28);
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[++decodedOffset] = ((val10 << 26) >> 11) | (val9 >> 17);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val10 << 5) >> 11;
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[++decodedOffset] = ((val11 << 16) >> 11) | (val10 >> 27);
    if (--rest == 0)
        return;

    uint32_t val12 = encode[offset++];
    decode[++decodedOffset] = ((val12 << 27) >> 11) | (val11 >> 16);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val12 << 6) >> 11;
    if (--rest == 0)
        return;

    uint32_t val13 = encode[offset++];
    decode[++decodedOffset] = ((val13 << 17) >> 11) | (val12 >> 26);
    if (--rest == 0)
        return;

    uint32_t val14 = encode[offset++];
    decode[++decodedOffset] = ((val14 << 28) >> 11) | (val13 >> 15);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val14 << 7) >> 11;
    if (--rest == 0)
        return;

    uint32_t val15 = encode[offset++];
    decode[++decodedOffset] = ((val15 << 18) >> 11) | (val14 >> 25);
    if (--rest == 0)
        return;

    uint32_t val16 = encode[offset++];
    decode[++decodedOffset] = ((val16 << 29) >> 11) | (val15 >> 14);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val16 << 8) >> 11;
    if (--rest == 0)
        return;

    uint32_t val17 = encode[offset++];
    decode[++decodedOffset] = ((val17 << 19) >> 11) | (val16 >> 24);
    if (--rest == 0)
        return;

    uint32_t val18 = encode[offset++];
    decode[++decodedOffset] = ((val18 << 30) >> 11) | (val17 >> 13);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val18 << 9) >> 11;
    if (--rest == 0)
        return;

    uint32_t val19 = encode[offset++];
    decode[++decodedOffset] = ((val19 << 20) >> 11) | (val18 >> 23);
    if (--rest == 0)
        return;

    uint32_t val20 = encode[offset++];
    decode[++decodedOffset] = ((val20 << 31) >> 11) | (val19 >> 12);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val20 << 10) >> 11;
    if (--rest == 0)
        return;

    uint32_t val21 = encode[offset++];
    decode[++decodedOffset] = ((val21 << 21) >> 11) | (val20 >> 22);
    return;
}

template <typename Type>
inline void unaligned_unpack_22(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];
        uint32_t val4 = encode[offset++];
        uint32_t val5 = encode[offset++];
        uint32_t val6 = encode[offset++];
        uint32_t val7 = encode[offset++];
        uint32_t val8 = encode[offset++];
        uint32_t val9 = encode[offset++];
        uint32_t val10 = encode[offset++];
        uint32_t val11 = encode[offset++];

        decode[0] = (val1 << 10) >> 10;
        decode[1] = ((val2 << 20) >> 10) | (val1 >> 22);

        decode[2] = ((val3 << 30) >> 10) | (val2 >> 12);

        decode[3] = (val3 << 8) >> 10;
        decode[4] = ((val4 << 18) >> 10) | (val3 >> 24);

        decode[5] = ((val5 << 28) >> 10) | (val4 >> 14);

        decode[6] = (val5 << 6) >> 10;
        decode[7] = ((val6 << 16) >> 10) | (val5 >> 26);

        decode[8] = ((val7 << 26) >> 10) | (val6 >> 16);

        decode[9] = (val7 << 4) >> 10;
        decode[10] = ((val8 << 14) >> 10) | (val7 >> 28);

        decode[11] = ((val9 << 24) >> 10) | (val8 >> 18);

        decode[12] = (val9 << 2) >> 10;
        decode[13] = ((val10 << 12) >> 10) | (val9 >> 30);

        decode[14] = ((val11 << 22) >> 10) | (val10 >> 20);

        decode[15] = val11 >> 10;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset++] = (val1 << 10) >> 10;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[decodedOffset++] = ((val2 << 20) >> 10) | (val1 >> 22);
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[decodedOffset++] = ((val3 << 30) >> 10) | (val2 >> 12);
    if (--rest == 0)
        return;
    decode[decodedOffset++] = (val3 << 8) >> 10;
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[decodedOffset++] = ((val4 << 18) >> 10) | (val3 >> 24);
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[decodedOffset++] = ((val5 << 28) >> 10) | (val4 >> 14);
    if (--rest == 0)
        return;
    decode[decodedOffset++] = (val5 << 6) >> 10;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[decodedOffset++] = ((val6 << 16) >> 10) | (val5 >> 26);
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[decodedOffset++] = ((val7 << 26) >> 10) | (val6 >> 16);
    if (--rest == 0)
        return;
    decode[decodedOffset++] = (val7 << 4) >> 10;
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[decodedOffset++] = ((val8 << 14) >> 10) | (val7 >> 28);
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[decodedOffset++] = ((val9 << 24) >> 10) | (val8 >> 18);
    if (--rest == 0)
        return;
    decode[decodedOffset++] = (val9 << 2) >> 10;
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[decodedOffset++] = ((val10 << 12) >> 10) | (val9 >> 30);
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[decodedOffset++] = ((val11 << 22) >> 10) | (val10 >> 20);
    return;
}

template <typename Type>
inline void unaligned_unpack_23(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t rest = dataNum;
    decode[0] = encode[0] & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[1] = ((encode[0] >> 23) | (encode[1] << 9)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[2] = ((encode[1] >> 14) | (encode[2] << 18)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[3] = (encode[2] >> 5) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[4] = ((encode[2] >> 28) | (encode[3] << 4)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[5] = ((encode[3] >> 19) | (encode[4] << 13)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[6] = ((encode[4] >> 10) | (encode[5] << 22)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[7] = (encode[5] >> 1) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[8] = ((encode[5] >> 24) | (encode[6] << 8)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[9] = ((encode[6] >> 15) | (encode[7] << 17)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[10] = (encode[7] >> 6) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[11] = ((encode[7] >> 29) | (encode[8] << 3)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[12] = ((encode[8] >> 20) | (encode[9] << 12)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[13] = ((encode[9] >> 11) | (encode[10] << 21)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[14] = (encode[10] >> 2) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[15] = ((encode[10] >> 25) | (encode[11] << 7)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[16] = ((encode[11] >> 16) | (encode[12] << 16)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[17] = (encode[12] >> 7) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[18] = ((encode[12] >> 30) | (encode[13] << 2)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[19] = ((encode[13] >> 21) | (encode[14] << 11)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[20] = ((encode[14] >> 12) | (encode[15] << 20)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[21] = (encode[15] >> 3) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[22] = ((encode[15] >> 26) | (encode[16] << 6)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[23] = ((encode[16] >> 17) | (encode[17] << 15)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[24] = (encode[17] >> 8) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[25] = ((encode[17] >> 31) | (encode[18] << 1)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[26] = ((encode[18] >> 22) | (encode[19] << 10)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[27] = ((encode[19] >> 13) | (encode[20] << 19)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[28] = (encode[20] >> 4) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[29] = ((encode[20] >> 27) | (encode[21] << 5)) & 0x7FFFFF;
    if (--rest == 0)
        return;
    decode[30] = ((encode[21] >> 18) | (encode[22] << 14)) & 0x7FFFFF;
}

template <typename Type>
inline void unaligned_unpack_24(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t maxBlocks = dataNum >> 2;
    uint32_t rest = dataNum % 4;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 4, encode += 3) {
        uint32_t val1 = encode[0];
        uint32_t val2 = encode[1];
        uint32_t val3 = encode[2];

        decode[0] = val1 & 0xFFFFFF;
        decode[1] = ((val1 >> 24) | (val2 << 8)) & 0xFFFFFF;
        decode[2] = ((val2 >> 16) | (val3 << 16)) & 0xFFFFFF;
        decode[3] = (val3 >> 8) & 0xFFFFFF;
    }

    if (rest == 0)
        return;

    decode[0] = encode[0] & 0xFFFFFF;
    if (--rest == 0)
        return;
    decode[1] = ((encode[0] >> 24) | (encode[1] << 8)) & 0xFFFFFF;
    if (--rest == 0)
        return;
    decode[2] = ((encode[1] >> 16) | (encode[2] << 16)) & 0xFFFFFF;
}

template <typename Type>
inline void unaligned_unpack_25(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t rest = dataNum;

    decode[0] = encode[0] & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[1] = ((encode[0] >> 25) | (encode[1] << 7)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[2] = ((encode[1] >> 18) | (encode[2] << 14)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[3] = ((encode[2] >> 11) | (encode[3] << 21)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[4] = (encode[3] >> 4) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[5] = ((encode[3] >> 29) | (encode[4] << 3)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[6] = ((encode[4] >> 22) | (encode[5] << 10)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[7] = ((encode[5] >> 15) | (encode[6] << 17)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[8] = ((encode[6] >> 8) | (encode[7] << 24)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[9] = (encode[7] >> 1) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[10] = ((encode[7] >> 26) | (encode[8] << 6)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[11] = ((encode[8] >> 19) | (encode[9] << 13)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[12] = ((encode[9] >> 12) | (encode[10] << 20)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[13] = (encode[10] >> 5) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[14] = ((encode[10] >> 30) | (encode[11] << 2)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[15] = ((encode[11] >> 23) | (encode[12] << 9)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[16] = ((encode[12] >> 16) | (encode[13] << 16)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[17] = ((encode[13] >> 9) | (encode[14] << 23)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[18] = (encode[14] >> 2) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[19] = ((encode[14] >> 27) | (encode[15] << 5)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[20] = ((encode[15] >> 20) | (encode[16] << 12)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[21] = ((encode[16] >> 13) | (encode[17] << 19)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[22] = (encode[17] >> 6) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[23] = ((encode[17] >> 31) | (encode[18] << 1)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[24] = ((encode[18] >> 24) | (encode[19] << 8)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[25] = ((encode[19] >> 17) | (encode[20] << 15)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[26] = ((encode[20] >> 10) | (encode[21] << 22)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[27] = (encode[21] >> 3) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[28] = ((encode[21] >> 28) | (encode[22] << 4)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[29] = ((encode[22] >> 21) | (encode[23] << 11)) & 0x1FFFFFF;
    if (--rest == 0)
        return;
    decode[30] = ((encode[23] >> 14) | (encode[24] << 18)) & 0x1FFFFFF;
}

template <typename Type>
inline void unaligned_unpack_26(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];
        uint32_t val4 = encode[offset++];
        uint32_t val5 = encode[offset++];
        uint32_t val6 = encode[offset++];
        uint32_t val7 = encode[offset++];
        uint32_t val8 = encode[offset++];
        uint32_t val9 = encode[offset++];
        uint32_t val10 = encode[offset++];
        uint32_t val11 = encode[offset++];
        uint32_t val12 = encode[offset++];
        uint32_t val13 = encode[offset++];

        decode[0] = (val1 << 6) >> 6;
        decode[1] = ((val2 << 12) >> 6) | (val1 >> 26);

        decode[2] = ((val3 << 18) >> 6) | (val2 >> 20);

        decode[3] = ((val4 << 24) >> 6) | (val3 >> 14);

        decode[4] = ((val5 << 30) >> 6) | (val4 >> 8);

        decode[5] = (val5 << 4) >> 6;
        decode[6] = ((val6 << 10) >> 6) | (val5 >> 28);

        decode[7] = ((val7 << 16) >> 6) | (val6 >> 22);

        decode[8] = ((val8 << 22) >> 6) | (val7 >> 16);

        decode[9] = ((val9 << 28) >> 6) | (val8 >> 10);

        decode[10] = (val9 << 2) >> 6;
        decode[11] = ((val10 << 8) >> 6) | (val9 >> 30);

        decode[12] = ((val11 << 14) >> 6) | (val10 >> 24);

        decode[13] = ((val12 << 20) >> 6) | (val11 >> 18);

        decode[14] = ((val13 << 26) >> 6) | (val12 >> 12);
        decode[15] = val13 >> 6;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 6) >> 6;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 12) >> 6) | (val1 >> 26);
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 18) >> 6) | (val2 >> 20);
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 24) >> 6) | (val3 >> 14);
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 30) >> 6) | (val4 >> 8);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val5 << 4) >> 6;
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 10) >> 6) | (val5 >> 28);
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 16) >> 6) | (val6 >> 22);
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 22) >> 6) | (val7 >> 16);
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 28) >> 6) | (val8 >> 10);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val9 << 2) >> 6;
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[++decodedOffset] = ((val10 << 8) >> 6) | (val9 >> 30);
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[++decodedOffset] = ((val11 << 14) >> 6) | (val10 >> 24);
    if (--rest == 0)
        return;

    uint32_t val12 = encode[offset++];
    decode[++decodedOffset] = ((val12 << 20) >> 6) | (val11 >> 18);
    if (--rest == 0)
        return;

    uint32_t val13 = encode[offset++];
    decode[++decodedOffset] = ((val13 << 26) >> 6) | (val12 >> 12);
    return;
}

template <typename Type>
inline void unaligned_unpack_27(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t rest = dataNum;
    decode[0] = encode[0] & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[1] = ((encode[0] >> 27) | (encode[1] << 5)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[2] = ((encode[1] >> 22) | (encode[2] << 10)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[3] = ((encode[2] >> 17) | (encode[3] << 15)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[4] = ((encode[3] >> 12) | (encode[4] << 20)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[5] = ((encode[4] >> 7) | (encode[5] << 25)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[6] = (encode[5] >> 2) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[7] = ((encode[5] >> 29) | (encode[6] << 3)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[8] = ((encode[6] >> 24) | (encode[7] << 8)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[9] = ((encode[7] >> 19) | (encode[8] << 13)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[10] = ((encode[8] >> 14) | (encode[9] << 18)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[11] = ((encode[9] >> 9) | (encode[10] << 23)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[12] = (encode[10] >> 4) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[13] = ((encode[10] >> 31) | (encode[11] << 1)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[14] = ((encode[11] >> 26) | (encode[12] << 6)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[15] = ((encode[12] >> 21) | (encode[13] << 11)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[16] = ((encode[13] >> 16) | (encode[14] << 16)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[17] = ((encode[14] >> 11) | (encode[15] << 21)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[18] = ((encode[15] >> 6) | (encode[16] << 26)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[19] = (encode[16] >> 1) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[20] = ((encode[16] >> 28) | (encode[17] << 4)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[21] = ((encode[17] >> 23) | (encode[18] << 9)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[22] = ((encode[18] >> 18) | (encode[19] << 14)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[23] = ((encode[19] >> 13) | (encode[20] << 19)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[24] = ((encode[20] >> 8) | (encode[21] << 24)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[25] = (encode[21] >> 3) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[26] = ((encode[21] >> 30) | (encode[22] << 2)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[27] = ((encode[22] >> 25) | (encode[23] << 7)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[28] = ((encode[23] >> 20) | (encode[24] << 12)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[29] = ((encode[24] >> 15) | (encode[25] << 17)) & 0x7FFFFFF;
    if (--rest == 0)
        return;
    decode[30] = ((encode[25] >> 10) | (encode[26] << 22)) & 0x7FFFFFF;
}

template <typename Type>
inline void unaligned_unpack_28(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t maxBlocks = dataNum >> 3;
    uint32_t rest = dataNum % 8;
    uint32_t decodedOffset = 0;

    // block process
    for (uint32_t block = 0; block < maxBlocks; block++, decode += 8) {
        uint32_t val1 = encode[offset++];
        uint32_t val2 = encode[offset++];
        uint32_t val3 = encode[offset++];
        uint32_t val4 = encode[offset++];
        uint32_t val5 = encode[offset++];
        uint32_t val6 = encode[offset++];
        uint32_t val7 = encode[offset++];

        decode[0] = (val1 << 4) >> 4;
        decode[1] = ((val2 << 8) >> 4) | (val1 >> 28);

        decode[2] = ((val3 << 12) >> 4) | (val2 >> 24);

        decode[3] = ((val4 << 16) >> 4) | (val3 >> 20);

        decode[4] = ((val5 << 20) >> 4) | (val4 >> 16);

        decode[5] = ((val6 << 24) >> 4) | (val5 >> 12);

        decode[6] = ((val7 << 28) >> 4) | (val6 >> 8);

        decode[7] = val7 >> 4;
    }

    if (rest == 0)
        return;

    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 4) >> 4;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 8) >> 4) | (val1 >> 28);
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 12) >> 4) | (val2 >> 24);
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 16) >> 4) | (val3 >> 20);
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 20) >> 4) | (val4 >> 16);
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 24) >> 4) | (val5 >> 12);
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 28) >> 4) | (val6 >> 8);
    return;
}

template <typename Type>
inline void unaligned_unpack_29(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t offset = 0;
    uint32_t rest = dataNum;
    uint32_t decodedOffset = 0;
    uint32_t val1 = encode[offset++];
    decode[decodedOffset] = (val1 << 3) >> 3;
    if (--rest == 0)
        return;

    uint32_t val2 = encode[offset++];
    decode[++decodedOffset] = ((val2 << 6) >> 3) | (val1 >> 29);
    if (--rest == 0)
        return;

    uint32_t val3 = encode[offset++];
    decode[++decodedOffset] = ((val3 << 9) >> 3) | (val2 >> 26);
    if (--rest == 0)
        return;

    uint32_t val4 = encode[offset++];
    decode[++decodedOffset] = ((val4 << 12) >> 3) | (val3 >> 23);
    if (--rest == 0)
        return;

    uint32_t val5 = encode[offset++];
    decode[++decodedOffset] = ((val5 << 15) >> 3) | (val4 >> 20);
    if (--rest == 0)
        return;

    uint32_t val6 = encode[offset++];
    decode[++decodedOffset] = ((val6 << 18) >> 3) | (val5 >> 17);
    if (--rest == 0)
        return;

    uint32_t val7 = encode[offset++];
    decode[++decodedOffset] = ((val7 << 21) >> 3) | (val6 >> 14);
    if (--rest == 0)
        return;

    uint32_t val8 = encode[offset++];
    decode[++decodedOffset] = ((val8 << 24) >> 3) | (val7 >> 11);
    if (--rest == 0)
        return;

    uint32_t val9 = encode[offset++];
    decode[++decodedOffset] = ((val9 << 27) >> 3) | (val8 >> 8);
    if (--rest == 0)
        return;

    uint32_t val10 = encode[offset++];
    decode[++decodedOffset] = ((val10 << 30) >> 3) | (val9 >> 5);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val10 << 1) >> 3;
    if (--rest == 0)
        return;

    uint32_t val11 = encode[offset++];
    decode[++decodedOffset] = ((val11 << 4) >> 3) | (val10 >> 31);
    if (--rest == 0)
        return;

    uint32_t val12 = encode[offset++];
    decode[++decodedOffset] = ((val12 << 7) >> 3) | (val11 >> 28);
    if (--rest == 0)
        return;

    uint32_t val13 = encode[offset++];
    decode[++decodedOffset] = ((val13 << 10) >> 3) | (val12 >> 25);
    if (--rest == 0)
        return;

    uint32_t val14 = encode[offset++];
    decode[++decodedOffset] = ((val14 << 13) >> 3) | (val13 >> 22);
    if (--rest == 0)
        return;

    uint32_t val15 = encode[offset++];
    decode[++decodedOffset] = ((val15 << 16) >> 3) | (val14 >> 19);
    if (--rest == 0)
        return;

    uint32_t val16 = encode[offset++];
    decode[++decodedOffset] = ((val16 << 19) >> 3) | (val15 >> 16);
    if (--rest == 0)
        return;

    uint32_t val17 = encode[offset++];
    decode[++decodedOffset] = ((val17 << 22) >> 3) | (val16 >> 13);
    if (--rest == 0)
        return;

    uint32_t val18 = encode[offset++];
    decode[++decodedOffset] = ((val18 << 25) >> 3) | (val17 >> 10);
    if (--rest == 0)
        return;

    uint32_t val19 = encode[offset++];
    decode[++decodedOffset] = ((val19 << 28) >> 3) | (val18 >> 7);
    if (--rest == 0)
        return;

    uint32_t val20 = encode[offset++];
    decode[++decodedOffset] = ((val20 << 31) >> 3) | (val19 >> 4);
    if (--rest == 0)
        return;
    decode[++decodedOffset] = (val20 << 2) >> 3;
    if (--rest == 0)
        return;

    uint32_t val21 = encode[offset++];
    decode[++decodedOffset] = ((val21 << 5) >> 3) | (val20 >> 30);
    if (--rest == 0)
        return;

    uint32_t val22 = encode[offset++];
    decode[++decodedOffset] = ((val22 << 8) >> 3) | (val21 >> 27);
    if (--rest == 0)
        return;

    uint32_t val23 = encode[offset++];
    decode[++decodedOffset] = ((val23 << 11) >> 3) | (val22 >> 24);
    if (--rest == 0)
        return;

    uint32_t val24 = encode[offset++];
    decode[++decodedOffset] = ((val24 << 14) >> 3) | (val23 >> 21);
    if (--rest == 0)
        return;

    uint32_t val25 = encode[offset++];
    decode[++decodedOffset] = ((val25 << 17) >> 3) | (val24 >> 18);
    if (--rest == 0)
        return;

    uint32_t val26 = encode[offset++];
    decode[++decodedOffset] = ((val26 << 20) >> 3) | (val25 >> 15);
    if (--rest == 0)
        return;

    uint32_t val27 = encode[offset++];
    decode[++decodedOffset] = ((val27 << 23) >> 3) | (val26 >> 12);
    if (--rest == 0)
        return;

    uint32_t val28 = encode[offset++];
    decode[++decodedOffset] = ((val28 << 26) >> 3) | (val27 >> 9);
    if (--rest == 0)
        return;

    uint32_t val29 = encode[offset++];
    decode[++decodedOffset] = ((val29 << 29) >> 3) | (val28 >> 6);
    return;
}

template <typename Type>
inline void unaligned_unpack_30(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t maxBlocks = dataNum >> 4;
    uint32_t rest = dataNum % 16;

    for (uint32_t block = 0; block < maxBlocks; block++, decode += 16, encode += 15) {
        decode[0] = encode[0] & 0x3FFFFFFF;
        decode[1] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x3FFFFFFF;
        decode[2] = ((encode[1] >> 28) | (encode[2] << 4)) & 0x3FFFFFFF;
        decode[3] = ((encode[2] >> 26) | (encode[3] << 6)) & 0x3FFFFFFF;
        decode[4] = ((encode[3] >> 24) | (encode[4] << 8)) & 0x3FFFFFFF;
        decode[5] = ((encode[4] >> 22) | (encode[5] << 10)) & 0x3FFFFFFF;
        decode[6] = ((encode[5] >> 20) | (encode[6] << 12)) & 0x3FFFFFFF;
        decode[7] = ((encode[6] >> 18) | (encode[7] << 14)) & 0x3FFFFFFF;
        decode[8] = ((encode[7] >> 16) | (encode[8] << 16)) & 0x3FFFFFFF;
        decode[9] = ((encode[8] >> 14) | (encode[9] << 18)) & 0x3FFFFFFF;
        decode[10] = ((encode[9] >> 12) | (encode[10] << 20)) & 0x3FFFFFFF;
        decode[11] = ((encode[10] >> 10) | (encode[11] << 22)) & 0x3FFFFFFF;
        decode[12] = ((encode[11] >> 8) | (encode[12] << 24)) & 0x3FFFFFFF;
        decode[13] = ((encode[12] >> 6) | (encode[13] << 26)) & 0x3FFFFFFF;
        decode[14] = ((encode[13] >> 4) | (encode[14] << 28)) & 0x3FFFFFFF;
        decode[15] = (encode[14] >> 2) & 0x3FFFFFFF;
    }
    if (rest == 0)
        return;

    decode[0] = encode[0] & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[1] = ((encode[0] >> 30) | (encode[1] << 2)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[2] = ((encode[1] >> 28) | (encode[2] << 4)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[3] = ((encode[2] >> 26) | (encode[3] << 6)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[4] = ((encode[3] >> 24) | (encode[4] << 8)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[5] = ((encode[4] >> 22) | (encode[5] << 10)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[6] = ((encode[5] >> 20) | (encode[6] << 12)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[7] = ((encode[6] >> 18) | (encode[7] << 14)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[8] = ((encode[7] >> 16) | (encode[8] << 16)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[9] = ((encode[8] >> 14) | (encode[9] << 18)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[10] = ((encode[9] >> 12) | (encode[10] << 20)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[11] = ((encode[10] >> 10) | (encode[11] << 22)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[12] = ((encode[11] >> 8) | (encode[12] << 24)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[13] = ((encode[12] >> 6) | (encode[13] << 26)) & 0x3FFFFFFF;
    if (--rest == 0)
        return;
    decode[14] = ((encode[13] >> 4) | (encode[14] << 28)) & 0x3FFFFFFF;
}

template <typename Type>
inline void unaligned_unpack_31(Type* decode, const uint32_t* encode, uint32_t dataNum)
{
    uint32_t rest = dataNum;

    decode[0] = encode[0] & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[1] = ((encode[0] >> 31) | (encode[1] << 1)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[2] = ((encode[1] >> 30) | (encode[2] << 2)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[3] = ((encode[2] >> 29) | (encode[3] << 3)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[4] = ((encode[3] >> 28) | (encode[4] << 4)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[5] = ((encode[4] >> 27) | (encode[5] << 5)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[6] = ((encode[5] >> 26) | (encode[6] << 6)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[7] = ((encode[6] >> 25) | (encode[7] << 7)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[8] = ((encode[7] >> 24) | (encode[8] << 8)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[9] = ((encode[8] >> 23) | (encode[9] << 9)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[10] = ((encode[9] >> 22) | (encode[10] << 10)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[11] = ((encode[10] >> 21) | (encode[11] << 11)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[12] = ((encode[11] >> 20) | (encode[12] << 12)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[13] = ((encode[12] >> 19) | (encode[13] << 13)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[14] = ((encode[13] >> 18) | (encode[14] << 14)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[15] = ((encode[14] >> 17) | (encode[15] << 15)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[16] = ((encode[15] >> 16) | (encode[16] << 16)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[17] = ((encode[16] >> 15) | (encode[17] << 17)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[18] = ((encode[17] >> 14) | (encode[18] << 18)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[19] = ((encode[18] >> 13) | (encode[19] << 19)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[20] = ((encode[19] >> 12) | (encode[20] << 20)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[21] = ((encode[20] >> 11) | (encode[21] << 21)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[22] = ((encode[21] >> 10) | (encode[22] << 22)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[23] = ((encode[22] >> 9) | (encode[23] << 23)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[24] = ((encode[23] >> 8) | (encode[24] << 24)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[25] = ((encode[24] >> 7) | (encode[25] << 25)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[26] = ((encode[25] >> 6) | (encode[26] << 26)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[27] = ((encode[26] >> 5) | (encode[27] << 27)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[28] = ((encode[27] >> 4) | (encode[28] << 28)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[29] = ((encode[28] >> 3) | (encode[29] << 29)) & 0x7FFFFFFF;
    if (--rest == 0)
        return;
    decode[30] = ((encode[29] >> 2) | (encode[30] << 30)) & 0x7FFFFFFF;
}
}
