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
#include <memory>
#include <string>
#include <stdexcept>

namespace indexlib {

class S9Compressor
{
public:
    S9Compressor(){}
    ~S9Compressor(){}

public:
    template <typename T>
    static size_t Encode(uint32_t* dest, size_t destLen, const T* src, size_t srcLen);

    template <typename T>
    static size_t Decode(T* dest, const uint32_t* src, size_t srcLen);

    static size_t CalculateBytes(uint32_t* bits, uint32_t len);

};

/////////////////////////////////////////////////////////////////
// inline functions

inline size_t S9Compressor::CalculateBytes(uint32_t* bits, uint32_t len)
{
    const static uint32_t BIT_LENGTH[] = {1, 2, 3, 4, 5, 7, 9, 14, 28};
    const static uint32_t CODE_NUM[] = {28, 14, 9, 7, 5, 4, 3, 2, 1};
    uint32_t resNum = 0;
    uint32_t curPos = 0;
    while (curPos < len) {
        for (uint32_t k = 0; k < 9; k++) {
            uint32_t curSize = CODE_NUM[k];
            if (len <= curPos + curSize - 1)
                continue;
            uint32_t b = BIT_LENGTH[k];
            uint32_t i = 0;
            for (; i < curSize && bits[curPos + i] <= b; i++)
                ;
            if (i == curSize) {
                resNum++;
                curPos = curPos + i;
                break;
            }
            assert(k < 8);
        }
    }

    return resNum * 4;
}

template <typename T>
inline size_t S9Compressor::Encode(uint32_t* dest, size_t destLen, const T* src, size_t srcLen)
{
    const static uint32_t BIT_LENGTH[] = {1, 2, 3, 4, 5, 7, 9, 14, 28};
    const static uint32_t CODE_NUM[] = {28, 14, 9, 7, 5, 4, 3, 2, 1};

    uint32_t resNum = 0;
    uint32_t curPos = 0;
    while (curPos < srcLen) {
        for (uint32_t k = 0; k < 9; k++) {
            uint32_t res = 0;
            uint32_t curSize = CODE_NUM[k];
            if (srcLen <= curPos + curSize - 1)
                continue;
            uint32_t b = BIT_LENGTH[k];
            uint32_t i = 0;
            for (; (i < curSize) && (src[curPos + i] < (1u << b)); i++) {
                res = (res << b) + src[curPos + i];
            }

            if (i == curSize) {
                res |= k << 28;
                dest[resNum++] = res;
                curPos += curSize;
                break;
            }
            if (k >= 8) {
                std::stringstream errMsg;
                errMsg << "Encoding failed "<<(uint32_t)src[curPos]<<" exceeds 28 bit";
                throw std::runtime_error(errMsg.str().c_str());
            }
        }
    }

    assert(destLen >= resNum);
    return resNum;
}

template <typename T>
inline size_t S9Compressor::Decode(T* dest, const uint32_t* src, size_t srcLen)
{
    T* destCursor = dest;
    for (size_t i = 0; i < srcLen; ++i) {
        uint32_t val = src[i];
        uint32_t header = (val >> 28);

        switch (header) {
        case 0:
            *destCursor++ = ((val << 4) >> 31);
            *destCursor++ = ((val << 5) >> 31);
            *destCursor++ = ((val << 6) >> 31);
            *destCursor++ = ((val << 7) >> 31);
            *destCursor++ = ((val << 8) >> 31);
            *destCursor++ = ((val << 9) >> 31);
            *destCursor++ = ((val << 10) >> 31);
            *destCursor++ = ((val << 11) >> 31);
            *destCursor++ = ((val << 12) >> 31);
            *destCursor++ = ((val << 13) >> 31);
            *destCursor++ = ((val << 14) >> 31);
            *destCursor++ = ((val << 15) >> 31);
            *destCursor++ = ((val << 16) >> 31);
            *destCursor++ = ((val << 17) >> 31);
            *destCursor++ = ((val << 18) >> 31);
            *destCursor++ = ((val << 19) >> 31);
            *destCursor++ = ((val << 20) >> 31);
            *destCursor++ = ((val << 21) >> 31);
            *destCursor++ = ((val << 22) >> 31);
            *destCursor++ = ((val << 23) >> 31);
            *destCursor++ = ((val << 24) >> 31);
            *destCursor++ = ((val << 25) >> 31);
            *destCursor++ = ((val << 26) >> 31);
            *destCursor++ = ((val << 27) >> 31);
            *destCursor++ = ((val << 28) >> 31);
            *destCursor++ = ((val << 29) >> 31);
            *destCursor++ = ((val << 30) >> 31);
            *destCursor++ = ((val << 31) >> 31);
            break;
        case 1: // code num : 14, bitwidth : 2
        {
            *destCursor++ = ((val << 4) >> 30);
            *destCursor++ = ((val << 6) >> 30);
            *destCursor++ = ((val << 8) >> 30);
            *destCursor++ = ((val << 10) >> 30);
            *destCursor++ = ((val << 12) >> 30);
            *destCursor++ = ((val << 14) >> 30);
            *destCursor++ = ((val << 16) >> 30);
            *destCursor++ = ((val << 18) >> 30);
            *destCursor++ = ((val << 20) >> 30);
            *destCursor++ = ((val << 22) >> 30);
            *destCursor++ = ((val << 24) >> 30);
            *destCursor++ = ((val << 26) >> 30);
            *destCursor++ = ((val << 28) >> 30);
            *destCursor++ = ((val << 30) >> 30);
            break;
        }
        case 2: // code num : 9, bitwidth : 3
        {
            *destCursor++ = ((val << 5) >> 29);
            *destCursor++ = ((val << 8) >> 29);
            *destCursor++ = ((val << 11) >> 29);
            *destCursor++ = ((val << 14) >> 29);
            *destCursor++ = ((val << 17) >> 29);
            *destCursor++ = ((val << 20) >> 29);
            *destCursor++ = ((val << 23) >> 29);
            *destCursor++ = ((val << 26) >> 29);
            *destCursor++ = ((val << 29) >> 29);
            break;
        }
        case 3: // code num : 7, bitwidth : 4
        {
            *destCursor++ = ((val << 4) >> 28);
            *destCursor++ = ((val << 8) >> 28);
            *destCursor++ = ((val << 12) >> 28);
            *destCursor++ = ((val << 16) >> 28);
            *destCursor++ = ((val << 20) >> 28);
            *destCursor++ = ((val << 24) >> 28);
            *destCursor++ = ((val << 28) >> 28);
            break;
        }
        case 4: // code num : 5, bitwidth : 5
        {
            *destCursor++ = ((val << 7) >> 27);
            *destCursor++ = ((val << 12) >> 27);
            *destCursor++ = ((val << 17) >> 27);
            *destCursor++ = ((val << 22) >> 27);
            *destCursor++ = ((val << 27) >> 27);
            break;
        }
        case 5: // code num : 4, bitwidth : 7
        {
            *destCursor++ = ((val << 4) >> 25);
            *destCursor++ = ((val << 11) >> 25);
            *destCursor++ = ((val << 18) >> 25);
            *destCursor++ = ((val << 25) >> 25);
            break;
        }
        case 6: // code num : 3, bitwidth : 9
        {
            *destCursor++ = ((val << 5) >> 23);
            *destCursor++ = ((val << 14) >> 23);
            *destCursor++ = ((val << 23) >> 23);
            break;
        }
        case 7: // code num : 2, bitwidth : 14
        {
            *destCursor++ = ((val << 4) >> 18);
            *destCursor++ = ((val << 18) >> 18);
            break;
        }
        case 8: // code num : 1, bitwidth : 28
        {
            *destCursor++ = ((val << 4) >> 4);
            break;
        }
        default:
            assert(0);
        }
    }
    return destCursor - dest;
}
}
