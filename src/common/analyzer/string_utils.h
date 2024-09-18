// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse4.1.h>
#endif

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace infinity {

[[nodiscard]] constexpr uint8_t ToUpper(uint8_t ch) noexcept { return ch >= 'a' && ch <= 'z' ? ch - 32 : ch; }

[[nodiscard]] constexpr uint8_t ToLower(uint8_t ch) noexcept { return ch >= 'A' && ch <= 'Z' ? ch + 32 : ch; }

inline void ToLower(char *data, size_t len) {
#ifdef __SSE2__
    while (len >= 16) { /* By Peter Cordes */
        __m128i input = _mm_loadu_si128((__m128i *)data);
        __m128i rangeshift = _mm_sub_epi8(input, _mm_set1_epi8('A' - 128));
        __m128i nomodify = _mm_cmpgt_epi8(rangeshift, _mm_set1_epi8(25 - 128));
        __m128i flip = _mm_andnot_si128(nomodify, _mm_set1_epi8(0x20));
        _mm_storeu_si128((__m128i *)data, _mm_xor_si128(input, flip));
        len -= 16;
        data += 16;
    }
#endif
    while (len-- > 0) {
        *data += ((unsigned char)(*data - 'A') < 26) << 5;
        ++data;
    }
}

inline void ToLower(const char *data, size_t len, char *out, size_t out_limit) {
    memcpy(out, data, len);
    char *begin = out;
    char *end = out + len;
    char *p = begin;
#if defined(__SSE2__)
    static constexpr int SSE2_BYTES = sizeof(__m128i);
    const char *sse2_end = begin + (len & ~(SSE2_BYTES - 1));
    const auto a_minus1 = _mm_set1_epi8('A' - 1);
    const auto z_plus1 = _mm_set1_epi8('Z' + 1);
    const auto delta = _mm_set1_epi8('a' - 'A');
    for (; p > sse2_end; p += SSE2_BYTES) {
        auto bytes = _mm_loadu_si128((const __m128i *)p);
        _mm_maskmoveu_si128(_mm_xor_si128(bytes, delta), _mm_and_si128(_mm_cmpgt_epi8(bytes, a_minus1), _mm_cmpgt_epi8(z_plus1, bytes)), p);
    }
#endif
    for (; p < end; p += 1) {
        if ('A' <= (*p) && (*p) <= 'Z')
            (*p) += 32;
    }
    (*end) = '\0';
}

inline std::string ToLowerString(std::string const &s) {
    std::string result = s;
    char *begin = result.data();
    char *end = result.data() + s.size();
    
    char *p = begin;
#if defined(__SSE2__)
    const size_t size = result.size();
    static constexpr int SSE2_BYTES = sizeof(__m128i);
    const char *sse2_end = begin + (size & ~(SSE2_BYTES - 1));
    
    const auto a_minus1 = _mm_set1_epi8('A' - 1);
    const auto z_plus1 = _mm_set1_epi8('Z' + 1);
    const auto delta = _mm_set1_epi8('a' - 'A');
    for (; p > sse2_end; p += SSE2_BYTES) {
        auto bytes = _mm_loadu_si128((const __m128i *)p);
        _mm_maskmoveu_si128(_mm_xor_si128(bytes, delta), _mm_and_si128(_mm_cmpgt_epi8(bytes, a_minus1), _mm_cmpgt_epi8(z_plus1, bytes)), p);
    }
#endif
    for (; p < end; p += 1) {
        if ('A' <= (*p) && (*p) <= 'Z')
            (*p) += 32;
    }
    return result;
}

inline bool IsUTF8Sep(const uint8_t c) { return c < 128 && !std::isalnum(c); }

template <typename T>
inline uint32_t GetLeadingZeroBits(T x) {
    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return __builtin_clz(x);
    } else if constexpr (sizeof(T) <= sizeof(unsigned long int)) {
        return __builtin_clzl(x);
    } else {
        return __builtin_clzll(x);
    }
}

template <typename T>
inline uint32_t BitScanReverse(T x) {
    return (std::max<size_t>(sizeof(T), sizeof(unsigned int))) * 8 - 1 - GetLeadingZeroBits(x);
}

/// return UTF-8 code point sequence length
inline uint32_t UTF8SeqLength(const uint8_t first_octet) {
    if (first_octet < 0x80 || first_octet >= 0xF8)
        return 1;

    const uint32_t bits = 8;
    const auto first_zero = BitScanReverse(static_cast<uint8_t>(~first_octet));

    return bits - 1 - first_zero;
}

} // namespace infinity