////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2020 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Max Neunhoeffer
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <cstring>
#include <iostream>

#include "asm-functions.h"
#include "asm-utf8check.h"
#include "velocypack/Utf8Helper.h"
#include "velocypack/velocypack-common.h"

#if VELOCYPACK_ASM_OPTIMIZATIONS == 1
#if defined(__SSE4_2__)
#include <cpuid.h>
#include <x86intrin.h>
#elif defined(__aarch64__)
#include <sse2neon.h>
#endif
#endif

namespace arangodb::velocypack {
namespace {

inline std::size_t JSONStringCopyC(uint8_t* dst, uint8_t const* src,
                                   std::size_t limit) {
  // Copy up to limit uint8_t from src to dst.
  // Stop at the first control character or backslash or double quote.
  // Report the number of bytes copied. May copy less bytes, for example
  // for alignment reasons.
  uint8_t const* end = src + limit;
  while (src < end && *src >= 32 && *src != '\\' && *src != '"') {
    *dst++ = *src++;
  }
  return limit - (end - src);
}

inline std::size_t JSONStringCopyCheckUtf8C(uint8_t* dst, uint8_t const* src,
                                            std::size_t limit) {
  // Copy up to limit uint8_t from src to dst.
  // Stop at the first control character or backslash or double quote.
  // Also stop at byte with high bit set.
  // Report the number of bytes copied. May copy less bytes, for example
  // for alignment reasons.
  uint8_t const* end = src + limit;
  while (src < end && *src >= 32 && *src != '\\' && *src != '"' &&
         *src < 0x80) {
    *dst++ = *src++;
  }
  return limit - (end - src);
}

inline std::size_t JSONSkipWhiteSpaceC(uint8_t const* src, std::size_t limit) {
  // Skip up to limit uint8_t from src as long as they are whitespace.
  // Advance ptr and return the number of skipped bytes.
  uint8_t const* end = src + limit;
  while (src < end &&
         (*src == ' ' || *src == '\t' || *src == '\n' || *src == '\r')) {
    src++;
  }
  return limit - (end - src);
}

inline bool ValidateUtf8StringC(uint8_t const* src, std::size_t limit) {
  return Utf8Helper::isValidUtf8(src, static_cast<ValueLength>(limit));
}

#if defined(__SSE4_2__) && VELOCYPACK_ASM_OPTIMIZATIONS == 1
bool hasSSE42() noexcept {
  unsigned int eax, ebx, ecx, edx;
  if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
    if ((ecx & 0x100000) != 0) {
      return true;
    }
  }
  return false;
}

std::size_t JSONStringCopySSE42(uint8_t* dst, uint8_t const* src,
                                std::size_t limit) {
  alignas(16) static char const ranges[17] =
      "\x20\x21\x23\x5b\x5d\xff          ";
  //= "\x01\x1f\"\"\\\\\"\"\"\"\"\"\"\"\"\"";

  __m128i const r = _mm_load_si128(reinterpret_cast<__m128i const*>(ranges));
  std::size_t count = 0;
  int x = 0;
  while (limit >= 16) {
    __m128i const s = _mm_loadu_si128(reinterpret_cast<__m128i const*>(src));
    x = _mm_cmpistri(r, /* 6, */ s, /* 16, */
                     _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES |
                         _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
    if (x < 16) {
      memcpy(dst, src, x);
      count += x;
      return count;
    }
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), s);
    src += 16;
    dst += 16;
    limit -= 16;
    count += 16;
  }
  if (limit == 0) {
    return count;
  }
  __m128i const s = _mm_loadu_si128(reinterpret_cast<__m128i const*>(src));
  x = _mm_cmpistri(r, /* 6, */ s, /* limit, */
                   _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES |
                       _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
  if (x > static_cast<int>(limit)) {
    x = static_cast<int>(limit);
  }
  memcpy(dst, src, x);
  count += x;
  return count;
}

std::size_t JSONStringCopyCheckUtf8SSE42(uint8_t* dst, uint8_t const* src,
                                         std::size_t limit) {
  alignas(16) static unsigned char const ranges[17] =
      "\x20\x21\x23\x5b\x5d\x7f          ";
  //= "\x01\x1f\x80\xff\"\"\\\\\"\"\"\"\"\"\"\"";
  __m128i const r = _mm_load_si128(reinterpret_cast<__m128i const*>(ranges));
  std::size_t count = 0;
  int x = 0;
  while (limit >= 16) {
    __m128i const s = _mm_loadu_si128(reinterpret_cast<__m128i const*>(src));
    x = _mm_cmpistri(r, /* 8, */ s, /* 16, */
                     _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES |
                         _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
    if (x < 16) {
      memcpy(dst, src, x);
      count += x;
      return count;
    }
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), s);
    src += 16;
    dst += 16;
    limit -= 16;
    count += 16;
  }
  if (limit == 0) {
    return count;
  }
  __m128i const s = _mm_loadu_si128(reinterpret_cast<__m128i const*>(src));
  x = _mm_cmpistri(r, /* 8, */ s, /* limit, */
                   _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES |
                       _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
  if (x > static_cast<int>(limit)) {
    x = static_cast<int>(limit);
  }
  memcpy(dst, src, x);
  count += x;
  return count;
}

std::size_t JSONSkipWhiteSpaceSSE42(uint8_t const* ptr, std::size_t limit) {
  alignas(16) static char const white[17] = " \t\n\r            ";
  __m128i const w = _mm_load_si128(reinterpret_cast<__m128i const*>(white));
  std::size_t count = 0;
  int x = 0;
  while (limit >= 16) {
    __m128i const s = _mm_loadu_si128(reinterpret_cast<__m128i const*>(ptr));
    x = _mm_cmpistri(w, /* 4, */ s, /*  16, */
                     _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
                         _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
    if (x < 16) {
      count += x;
      return count;
    }
    ptr += 16;
    limit -= 16;
    count += 16;
  }
  if (limit == 0) {
    return count;
  }
  __m128i const s = _mm_loadu_si128(reinterpret_cast<__m128i const*>(ptr));
  x = _mm_cmpistri(w, /* 4, */ s, /* limit, */
                   _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
                       _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
  if (static_cast<std::size_t>(x) > limit) {
    x = static_cast<int>(limit);
  }
  count += x;
  return count;
}

#endif
#if VELOCYPACK_ASM_OPTIMIZATIONS == 1

bool ValidateUtf8StringSSE42(uint8_t const* src, std::size_t len) {
  if (len >= 16) {
    return validate_utf8_fast_sse42(src, len);
  }
  return Utf8Helper::isValidUtf8(src, len);
}

#endif
#if defined(__AVX2__) && VELOCYPACK_ASM_OPTIMIZATIONS == 1

bool hasAVX2() noexcept {
  unsigned int eax, ebx, ecx, edx;
  if (__get_cpuid(7, &eax, &ebx, &ecx, &edx)) {
    if ((ebx & bit_AVX2) != 0) {
      return true;
    }
  }
  return false;
}

bool ValidateUtf8StringAVX(uint8_t const* src, std::size_t len) {
  if (len >= 32) {
    return validate_utf8_fast_avx(src, len);
  }
  return Utf8Helper::isValidUtf8(src, len);
}

#endif

struct EnableNative {
  EnableNative() noexcept { enableNativeStringFunctions(); }
};

[[maybe_unused]] EnableNative const init;

}  // namespace

std::size_t (*JSONStringCopy)(uint8_t*, uint8_t const*,
                              std::size_t) = JSONStringCopyC;

std::size_t (*JSONStringCopyCheckUtf8)(uint8_t*, uint8_t const*,
                                       std::size_t) = JSONStringCopyCheckUtf8C;

std::size_t (*JSONSkipWhiteSpace)(uint8_t const*,
                                  std::size_t) = JSONSkipWhiteSpaceC;

bool (*ValidateUtf8String)(uint8_t const*, std::size_t) = ValidateUtf8StringC;

void enableNativeStringFunctions() noexcept {
  enableBuiltinStringFunctions();
#if defined(__SSE4_2__) && VELOCYPACK_ASM_OPTIMIZATIONS == 1
  if (hasSSE42()) {
    JSONStringCopy = JSONStringCopySSE42;
    JSONStringCopyCheckUtf8 = JSONStringCopyCheckUtf8SSE42;
    JSONSkipWhiteSpace = JSONSkipWhiteSpaceSSE42;
    ValidateUtf8String = ValidateUtf8StringSSE42;
  }
#elif defined(__aarch64__) && VELOCYPACK_ASM_OPTIMIZATIONS == 1
  ValidateUtf8String = ValidateUtf8StringSSE42;
#endif
#if defined(__AVX2__) && VELOCYPACK_ASM_OPTIMIZATIONS == 1
  if (hasAVX2()) {
    ValidateUtf8String = ValidateUtf8StringAVX;
  }
#endif
}

void enableBuiltinStringFunctions() noexcept {
  JSONStringCopy = JSONStringCopyC;
  JSONStringCopyCheckUtf8 = JSONStringCopyCheckUtf8C;
  JSONSkipWhiteSpace = JSONSkipWhiteSpaceC;
  ValidateUtf8String = ValidateUtf8StringC;
}

}  // namespace arangodb::velocypack

#if defined(COMPILE_VELOCYPACK_ASM_UNITTESTS)

int testPositions[] = {
    0,   1,   2,   3,   4,   5,   6,    7,    8,    9,    10,   11,   12,  13,
    14,  15,  16,  23,  31,  32,  67,   103,  178,  210,  234,  247,  254, 255,
    -1,  -2,  -3,  -4,  -5,  -6,  -7,   -8,   -9,   -10,  -11,  -12,  -13, -14,
    -15, -16, -23, -31, -32, -67, -103, -178, -210, -234, -247, -254, -255};

using namespace arangodb::velocypack;

void TestStringCopyCorrectness(uint8_t* src, uint8_t* dst, std::size_t size) {
  std::size_t copied;

  std::cout << "Performing correctness tests..." << std::endl;

  auto start = std::chrono::high_resolution_clock::now();

  for (std::size_t salign = 0; salign < 16; salign++) {
    src += salign;
    for (std::size_t dalign = 0; dalign < 16; dalign++) {
      dst += dalign;
      for (std::size_t i = 0;
           i < static_cast<int>(sizeof(testPositions) / sizeof(int)); i++) {
        uint8_t merk;
        int off = testPositions[i];
        std::size_t pos;
        if (off >= 0) {
          pos = off;
        } else {
          pos = size - static_cast<std::size_t>(-off);
        }
        if (pos >= size) {
          continue;
        }

        // Test a quote character:
        merk = src[pos];
        src[pos] = '"';
        copied = JSONStringCopy(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;

        // Test a backslash character:
        src[pos] = '\\';
        copied = JSONStringCopy(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;

        // Test a 0 character:
        src[pos] = 1;
        copied = JSONStringCopy(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;

        // Test a control character:
        src[pos] = 31;
        copied = JSONStringCopy(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;
      }
      dst -= dalign;
    }
    src -= salign;
  }

  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);
  std::cout << "Tests took altogether " << totalTime.count() << " seconds."
            << std::endl;
}

void TestStringCopyCorrectnessCheckUtf8(uint8_t* src, uint8_t* dst,
                                        std::size_t size) {
  std::size_t copied;

  std::cout << "Performing correctness tests (check UTF8)..." << std::endl;

  auto start = std::chrono::high_resolution_clock::now();

  for (int salign = 0; salign < 16; salign++) {
    src += salign;
    for (int dalign = 0; dalign < 16; dalign++) {
      dst += dalign;
      for (int i = 0; i < static_cast<int>(sizeof(testPositions) / sizeof(int));
           i++) {
        uint8_t merk;
        int off = testPositions[i];
        std::size_t pos;
        if (off >= 0) {
          pos = off;
        } else {
          pos = size - static_cast<std::size_t>(-off);
        }
        if (pos >= size) {
          continue;
        }

        // Test a quote character:
        merk = src[pos];
        src[pos] = '"';
        copied = JSONStringCopyCheckUtf8(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;

        // Test a backslash character:
        src[pos] = '\\';
        copied = JSONStringCopyCheckUtf8(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;

        // Test a 0 character:
        src[pos] = 1;
        copied = JSONStringCopyCheckUtf8(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;

        // Test a control character:
        src[pos] = 31;
        copied = JSONStringCopyCheckUtf8(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;

        // Test a high bitcharacter:
        src[pos] = 0x80;
        copied = JSONStringCopyCheckUtf8(dst, src, size);
        if (copied != pos || memcmp(dst, src, copied) != 0) {
          std::cout << "Error: " << salign << " " << dalign << " " << i << " "
                    << pos << " " << copied << std::endl;
        }
        src[pos] = merk;
      }
      dst -= dalign;
    }
    src -= salign;
  }

  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);
  std::cout << "Tests took altogether " << totalTime.count() << " seconds."
            << std::endl;
}

void TestSkipWhiteSpaceCorrectness(uint8_t* src, std::size_t size) {
  std::size_t copied;

  std::cout << "Performing correctness tests for whitespace skipping..."
            << std::endl;

  auto start = std::chrono::high_resolution_clock::now();

  for (int salign = 0; salign < 16; salign++) {
    src += salign;
    for (int i = 0; i < static_cast<int>(sizeof(testPositions) / sizeof(int));
         i++) {
      uint8_t merk;
      int off = testPositions[i];
      std::size_t pos;
      if (off >= 0) {
        pos = off;
      } else {
        pos = size - static_cast<std::size_t>(-off);
      }
      if (pos >= size) {
        continue;
      }

      // Test a non-whitespace character:
      merk = src[pos];
      src[pos] = 'x';
      copied = JSONSkipWhiteSpace(src, size);
      if (copied != pos) {
        std::cout << "Error: " << salign << " " << i << " " << pos << " "
                  << copied << std::endl;
      }
      src[pos] = merk;
    }
    src -= salign;
  }

  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);
  std::cout << "Whitespace tests took altogether " << totalTime.count()
            << " seconds." << std::endl;
}

void RaceStringCopy(uint8_t* dst, uint8_t* src, std::size_t size, int repeat,
                    uint64_t& akku) {
  std::size_t copied;

  std::cout << "\nNow racing for the repeated full string, "
            << "first target aligned...\n"
            << std::endl;

  src[size] = 0;
  auto start = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < repeat; j++) {
    copied = JSONStringCopy(dst, src, size);
    akku = akku * 13 + copied;
  }
  auto now = std::chrono::high_resolution_clock::now();
  src[size] = 'a' + (size % 26);

  auto totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);

  std::cout << "Race took altogether " << totalTime.count() << " seconds."
            << std::endl;
  std::cout << "Time to copy string of length " << size
            << " on average is: " << totalTime.count() / repeat << "."
            << std::endl;
  std::cout << "Bytes copied per second: "
            << (double)size * (double)repeat / totalTime.count() << std::endl;

  std::cout << "\nNow racing for the repeated full string, "
            << "now unaligned target...\n"
            << std::endl;

  dst++;
  start = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < repeat; j++) {
    copied = JSONStringCopy(dst, src, size);
    akku = akku * 13 + copied;
  }
  now = std::chrono::high_resolution_clock::now();

  totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);

  std::cout << "Race took altogether " << totalTime.count() << " seconds."
            << std::endl;
  std::cout << "Time to copy string of length " << size
            << " on average is: " << totalTime.count() / repeat << "."
            << std::endl;
  std::cout << "Bytes copied per second: "
            << (double)size * (double)repeat / totalTime.count() << std::endl;
}

void RaceStringCopyCheckUtf8(uint8_t* dst, uint8_t* src, std::size_t size,
                             int repeat, uint64_t& akku) {
  std::size_t copied;

  std::cout << "\nNow racing for the repeated (check UTF8) full string, "
            << "first target aligned...\n"
            << std::endl;

  src[size] = 0;
  auto start = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < repeat; j++) {
    copied = JSONStringCopy(dst, src, size);
    akku = akku * 13 + copied;
  }
  auto now = std::chrono::high_resolution_clock::now();
  src[size] = 'a' + (size % 26);

  auto totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);

  std::cout << "Race took altogether " << totalTime.count() << " seconds."
            << std::endl;
  std::cout << "Time to copy string of length " << size
            << " on average is: " << totalTime.count() / repeat << "."
            << std::endl;
  std::cout << "Bytes copied per second: "
            << (double)size * (double)repeat / totalTime.count() << std::endl;

  std::cout
      << "\nNow racing for the repeated full string, now unaligned target...\n"
      << std::endl;

  dst++;
  start = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < repeat; j++) {
    copied = JSONStringCopy(dst, src, size);
    akku = akku * 13 + copied;
  }
  now = std::chrono::high_resolution_clock::now();
  dst--;

  totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);

  std::cout << "Race took altogether " << totalTime.count() << " seconds."
            << std::endl;
  std::cout << "Time to copy string of length " << size
            << " on average is: " << totalTime.count() / repeat << "."
            << std::endl;
  std::cout << "Bytes copied per second: "
            << (double)size * (double)repeat / totalTime.count() << std::endl;

  std::cout << "\nNow comparing with strcpy...\n" << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < repeat; j++) {
    // strcpy((char*) dst, (char*) src);
    memcpy((char*)dst, (char*)src, size);
  }
  now = std::chrono::high_resolution_clock::now();

  totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);

  std::cout << "Race took altogether " << totalTime.count() << " seconds."
            << std::endl;
  std::cout << "Time to copy string of length " << size
            << " on average is: " << totalTime.count() / repeat << "."
            << std::endl;
  std::cout << "Bytes copied per second: "
            << (double)size * (double)repeat / totalTime.count() << std::endl;
}

void RaceSkipWhiteSpace(uint8_t* src, std::size_t size, int repeat,
                        uint64_t& akku) {
  std::size_t copied;

  std::cout << "\nNow racing for the repeated full string...\n" << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  akku = 0;
  src[size] = 0;
  for (int j = 0; j < repeat; j++) {
    copied = JSONSkipWhiteSpace(src, size);
    akku = akku * 13 + copied;
  }
  auto now = std::chrono::high_resolution_clock::now();

  auto totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);

  std::cout << "Race took altogether " << totalTime.count() << " seconds."
            << std::endl;
  std::cout << "Time to skip white string of length " << size
            << " on average is: " << totalTime.count() / repeat << "."
            << std::endl;
  std::cout << "Bytes skipped per second: "
            << (double)size * (double)repeat / totalTime.count() << std::endl;

  std::cout << "\nNow comparing with strlen...\n" << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < repeat; j++) {
    copied = strlen((char*)src);
    // Fake activity for the compiler:
    src[0] = (j & 0xf) + 1;
    akku = akku * 13 + copied;
  }
  now = std::chrono::high_resolution_clock::now();

  totalTime =
      std::chrono::duration_cast<std::chrono::duration<double>>(now - start);

  std::cout << "Race took altogether " << totalTime.count() << " seconds."
            << std::endl;
  std::cout << "Time to strlen string of length " << size
            << " on average is: " << totalTime.count() / repeat << "."
            << std::endl;
  std::cout << "Bytes scanned per second: "
            << (double)size * (double)repeat / totalTime.count() << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " SIZE REPEAT CORRECTNESS"
              << std::endl;
    return 0;
  }

  std::size_t size = atol(argv[1]);
  int repeat = atoi(argv[2]);
  int docorrectness = atoi(argv[3]);
  uint64_t akku = 0;
  std::cout << "Size: " << size << std::endl;
  std::cout << "Repeat:" << repeat << std::endl;

  uint8_t* src = new uint8_t[size + 17 + 16];
  uint8_t* dst = new uint8_t[size + 17];
  std::cout << "Src pointer: " << (void*)src << std::endl;
  std::cout << "Dst pointer: " << (void*)dst << std::endl;
  for (std::size_t i = 0; i < size + 16; i++) {
    src[i] = 'a' + (i % 26);
  }
  src[size + 16] = 0;

  if (docorrectness > 0) {
    TestStringCopyCorrectness(src, dst, size);
  }

  RaceStringCopy(dst, src, size, repeat, akku);

  if (docorrectness > 0) {
    TestStringCopyCorrectnessCheckUtf8(src, dst, size);
  }

  RaceStringCopyCheckUtf8(dst, src, size, repeat, akku);

  std::cout << "\n\n\nNOW WHITESPACE SKIPPING\n" << std::endl;

  // Now do the whitespace skipping tests/measurements:
  static char const whitetab[17] = "       \t   \n   \r";
  for (std::size_t i = 0; i < size + 16; i++) {
    src[i] = whitetab[i % 16];
  }
  src[size + 16] = 0;

  if (docorrectness > 0) {
    TestSkipWhiteSpaceCorrectness(src, size);
  }

  RaceSkipWhiteSpace(src, size, repeat, akku);

  std::cout << "\n\n\nAkku (please ignore):" << akku << std::endl;
  std::cout << "\n\n\nGuck (please ignore): " << dst[100] << std::endl;

  delete[] src;
  delete[] dst;
  return 0;
}

#endif
