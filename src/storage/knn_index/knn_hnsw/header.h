#pragma once
#ifndef NO_MANUAL_VECTORIZATION
#if (defined(__SSE2__) || _M_IX86_FP > 0 || defined(_M_AMD64) || defined(_M_X64))
#define USE_SSE
#ifdef __AVX2__
#define USE_AVX
#ifdef __AVX512F__
#define USE_AVX512
#endif
#endif
#elif defined(__aarch64__)
#define USE_SSE
#define USE_AVX
#endif
#endif

#if defined(USE_AVX) || defined(USE_SSE)

#if defined(_MSC_VER)
     /* Microsoft C/C++-compatible compiler */
     #include <intrin.h>
     #include <stdexcept>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
     /* GCC-compatible compiler, targeting x86/x86-64 */
     #include <x86intrin.h>
     #include <cpuid.h>
     #include <stdint.h>
#elif defined(__GNUC__) && defined(__aarch64__)
     /* GCC-compatible compiler, targeting ARM with NEON */
     #include <simde/x86/avx2.h>
     #include <simde/x86/avx512.h>
     #include <stdint.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
     /* GCC-compatible compiler, targeting ARM with WMMX */
     #include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
     /* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
     #include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
     /* GCC-compatible compiler, targeting PowerPC with SPE */
     #include <spe.h>
#endif

#if defined(USE_AVX512)
#include <immintrin.h>
#endif

#if defined(__GNUC__)
#define PORTABLE_ALIGN32 __attribute__((aligned(32)))
#define PORTABLE_ALIGN64 __attribute__((aligned(64)))
#else
#define PORTABLE_ALIGN32 __declspec(align(32))
#define PORTABLE_ALIGN64 __declspec(align(64))
#endif

// Adapted from https://github.com/Mysticial/FeatureDetector
#define _XCR_XFEATURE_ENABLED_MASK  0

// static bool AVXCapable() {
//     int cpuInfo[4];

//     // CPU support
//     cpuid(cpuInfo, 0, 0);
//     int nIds = cpuInfo[0];

//     bool HW_AVX = false;
//     if (nIds >= 0x00000001) {
//         cpuid(cpuInfo, 0x00000001, 0);
//         HW_AVX = (cpuInfo[2] & ((int)1 << 28)) != 0;
//     }

//     // OS support
//     cpuid(cpuInfo, 1, 0);

//     bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
//     bool cpuAVXSuport = (cpuInfo[2] & (1 << 28)) != 0;

//     bool avxSupported = false;
//     if (osUsesXSAVE_XRSTORE && cpuAVXSuport) {
//         uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
//         avxSupported = (xcrFeatureMask & 0x6) == 0x6;
//     }
//     return HW_AVX && avxSupported;
// }

// static bool AVX512Capable() {
//     if (!AVXCapable()) return false;

//     int cpuInfo[4];

//     // CPU support
//     cpuid(cpuInfo, 0, 0);
//     int nIds = cpuInfo[0];

//     bool HW_AVX512F = false;
//     if (nIds >= 0x00000007) {  //  AVX512 Foundation
//         cpuid(cpuInfo, 0x00000007, 0);
//         HW_AVX512F = (cpuInfo[1] & ((int)1 << 16)) != 0;
//     }

//     // OS support
//     cpuid(cpuInfo, 1, 0);

//     bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
//     bool cpuAVXSuport = (cpuInfo[2] & (1 << 28)) != 0;

//     bool avx512Supported = false;
//     if (osUsesXSAVE_XRSTORE && cpuAVXSuport) {
//         uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
//         avx512Supported = (xcrFeatureMask & 0xe6) == 0xe6;
//     }
//     return HW_AVX512F && avx512Supported;
// }
#endif