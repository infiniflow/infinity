/*
 * portability.h
 *
 */

 /**
  * All macros should be prefixed with either CROARING or ROARING.
  * The library uses both ROARING_...
  * as well as CROAIRING_ as prefixes. The ROARING_ prefix is for
  * macros that are provided by the build system or that are closely
  * related to the format. The header macros may also use ROARING_.
  * The CROARING_ prefix is for internal macros that a user is unlikely
  * to ever interact with.
  */

#ifndef INCLUDE_PORTABILITY_H_
#define INCLUDE_PORTABILITY_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif // _GNU_SOURCE
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS 1
#endif // __STDC_FORMAT_MACROS

#ifdef _MSC_VER
#define CROARING_VISUAL_STUDIO 1
/**
 * We want to differentiate carefully between
 * clang under visual studio and regular visual
 * studio.
 */
#ifdef __clang__
// clang under visual studio
#define CROARING_CLANG_VISUAL_STUDIO 1
#else
// just regular visual studio (best guess)
#define CROARING_REGULAR_VISUAL_STUDIO 1
#endif // __clang__
#endif // _MSC_VER

#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE < 200809L)
#undef _POSIX_C_SOURCE
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif // !(defined(_POSIX_C_SOURCE)) || (_POSIX_C_SOURCE < 200809L)
#if !(defined(_XOPEN_SOURCE)) || (_XOPEN_SOURCE < 700)
#define _XOPEN_SOURCE 700
#endif // !(defined(_XOPEN_SOURCE)) || (_XOPEN_SOURCE < 700)

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>  // will provide posix_memalign with _POSIX_C_SOURCE as defined above
#ifdef __GLIBC__
#include <malloc.h>  // this should never be needed but there are some reports that it is needed.
#endif

#ifdef __cplusplus
extern "C" {  // portability definitions are in global scope, not a namespace
#endif

#if CROARING_REGULAR_VISUAL_STUDIO && !defined(_WIN64) && !defined(CROARING_ACK_32BIT)
#pragma message( \
    "You appear to be attempting a 32-bit build under Visual Studio. We recommend a 64-bit build instead.")
#endif

#if defined(__SIZEOF_LONG_LONG__) && __SIZEOF_LONG_LONG__ != 8
#error This code assumes  64-bit long longs (by use of the GCC intrinsics). Your system is not currently supported.
#endif

#if CROARING_REGULAR_VISUAL_STUDIO
#define __restrict__ __restrict
#endif // CROARING_REGULAR_VISUAL_STUDIO



#if defined(__x86_64__) || defined(_M_X64)
// we have an x64 processor
#define CROARING_IS_X64 1

#if defined(_MSC_VER) && (_MSC_VER < 1910)
// Old visual studio systems won't support AVX2 well.
#undef CROARING_IS_X64
#endif

#if defined(__clang_major__) && (__clang_major__<= 8) && !defined(__AVX2__)
// Older versions of clang have a bug affecting us
// https://stackoverflow.com/questions/57228537/how-does-one-use-pragma-clang-attribute-push-with-c-namespaces
#undef CROARING_IS_X64
#endif

#ifdef CROARING_DISABLE_X64
#undef CROARING_IS_X64
#endif
// we include the intrinsic header
#if !CROARING_REGULAR_VISUAL_STUDIO
/* Non-Microsoft C/C++-compatible compiler */
#include <x86intrin.h>  // on some recent GCC, this will declare posix_memalign



#ifdef CROARING_CLANG_VISUAL_STUDIO

/**
 * You are not supposed, normally, to include these
 * headers directly. Instead you should either include intrin.h
 * or x86intrin.h. However, when compiling with clang
 * under Windows (i.e., when _MSC_VER is set), these headers
 * only get included *if* the corresponding features are detected
 * from macros:
 * e.g., if __AVX2__ is set... in turn,  we normally set these
 * macros by compiling against the corresponding architecture
 * (e.g., arch:AVX2, -mavx2, etc.) which compiles the whole
 * software with these advanced instructions. These headers would
 * normally guard against such usage, but we carefully included
 * <x86intrin.h>  (or <intrin.h>) before, so the headers
 * are fooled.
 */
#include <bmiintrin.h>   // for _blsr_u64
#include <lzcntintrin.h> // for  __lzcnt64
#include <immintrin.h>   // for most things (AVX2, AVX512, _popcnt64)
#include <smmintrin.h>
#include <tmmintrin.h>
#include <avxintrin.h>
#include <avx2intrin.h>
#include <wmmintrin.h>
// unfortunately, we may not get _blsr_u64, but, thankfully, clang
// has it as a macro.
#ifndef _blsr_u64
// we roll our own
#define _blsr_u64(n) ((n - 1) & n)
#endif //  _blsr_u64
#endif // SIMDJSON_CLANG_VISUAL_STUDIO


#endif // CROARING_REGULAR_VISUAL_STUDIO
#endif // defined(__x86_64__) || defined(_M_X64)

#if !defined(USENEON) && !defined(DISABLENEON) && defined(__ARM_NEON)
#  define USENEON
#endif
#if defined(USENEON)
#  include <arm_neon.h>
#endif

#if !CROARING_REGULAR_VISUAL_STUDIO
/* Non-Microsoft C/C++-compatible compiler, assumes that it supports inline
 * assembly */
#define CROARING_INLINE_ASM 1
#endif  // _MSC_VER

#if CROARING_REGULAR_VISUAL_STUDIO
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>

#ifndef __clang__  // if one compiles with MSVC *with* clang, then these
                   // intrinsics are defined!!!
// sadly there is no way to check whether we are missing these intrinsics
// specifically.

/* wrappers for Visual Studio built-ins that look like gcc built-ins */
/* result might be undefined when input_num is zero */
inline int __builtin_ctzll(unsigned long long input_num) {
    unsigned long index;
#ifdef _WIN64  // highly recommended!!!
    _BitScanForward64(&index, input_num);
#else  // if we must support 32-bit Windows
    if ((uint32_t)input_num != 0) {
        _BitScanForward(&index, (uint32_t)input_num);
    } else {
        _BitScanForward(&index, (uint32_t)(input_num >> 32));
        index += 32;
    }
#endif
    return index;
}

/* result might be undefined when input_num is zero */
inline int __builtin_clzll(unsigned long long input_num) {
    unsigned long index;
#ifdef _WIN64  // highly recommended!!!
    _BitScanReverse64(&index, input_num);
#else  // if we must support 32-bit Windows
    if (input_num > 0xFFFFFFFF) {
        _BitScanReverse(&index, (uint32_t)(input_num >> 32));
        index += 32;
    } else {
        _BitScanReverse(&index, (uint32_t)(input_num));
    }
#endif
    return 63 - index;
}


/* software implementation avoids POPCNT */
/*static inline int __builtin_popcountll(unsigned long long input_num) {
  const uint64_t m1 = 0x5555555555555555; //binary: 0101...
  const uint64_t m2 = 0x3333333333333333; //binary: 00110011..
  const uint64_t m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
  const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

  input_num -= (input_num >> 1) & m1;
  input_num = (input_num & m2) + ((input_num >> 2) & m2);
  input_num = (input_num + (input_num >> 4)) & m4;
  return (input_num * h01) >> 56;
}*/

/* Use #define so this is effective even under /Ob0 (no inline) */
#define __builtin_unreachable() __assume(0)
#endif

#endif

#if CROARING_REGULAR_VISUAL_STUDIO
#define ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__) || defined(__clang__)
#define ALIGNED(x) __attribute__((aligned(x)))
#else
#warning "Warning. Unrecognized compiler."
#define ALIGNED(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define WARN_UNUSED __attribute__((warn_unused_result))
#else
#define WARN_UNUSED
#endif

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

#ifdef USENEON
// we can always compute the popcount fast.
#elif (defined(_M_ARM) || defined(_M_ARM64)) && ((defined(_WIN64) || defined(_WIN32)) && defined(CROARING_REGULAR_VISUAL_STUDIO) && CROARING_REGULAR_VISUAL_STUDIO)
// we will need this function:
static inline int hammingbackup(uint64_t x) {
  uint64_t c1 = UINT64_C(0x5555555555555555);
  uint64_t c2 = UINT64_C(0x3333333333333333);
  uint64_t c4 = UINT64_C(0x0F0F0F0F0F0F0F0F);
  x -= (x >> 1) & c1;
  x = (( x >> 2) & c2) + (x & c2); x=(x +(x>>4))&c4;
  x *= UINT64_C(0x0101010101010101);
  return x >> 56;
}
#endif


static inline int hamming(uint64_t x) {
#if defined(_WIN64) && defined(CROARING_REGULAR_VISUAL_STUDIO) && CROARING_REGULAR_VISUAL_STUDIO
#ifdef USENEON
   return vaddv_u8(vcnt_u8(vcreate_u8(input_num)));
#elif defined(_M_ARM64)
  return hammingbackup(x);
  // (int) _CountOneBits64(x); is unavailable
#else  // _M_ARM64
  return (int) __popcnt64(x);
#endif // _M_ARM64
#elif defined(_WIN32) && defined(CROARING_REGULAR_VISUAL_STUDIO) && CROARING_REGULAR_VISUAL_STUDIO
#ifdef _M_ARM
  return hammingbackup(x);
  // _CountOneBits is unavailable
#else // _M_ARM
    return (int) __popcnt(( unsigned int)x) + (int)  __popcnt(( unsigned int)(x>>32));
#endif // _M_ARM
#else
    return __builtin_popcountll(x);
#endif
}

#ifndef UINT64_C
#define UINT64_C(c) (c##ULL)
#endif // UINT64_C

#ifndef UINT32_C
#define UINT32_C(c) (c##UL)
#endif // UINT32_C

#ifdef __cplusplus
}  // extern "C" {
#endif // __cplusplus


// this is almost standard?
#undef STRINGIFY_IMPLEMENTATION_
#undef STRINGIFY
#define STRINGIFY_IMPLEMENTATION_(a) #a
#define STRINGIFY(a) STRINGIFY_IMPLEMENTATION_(a)

// Our fast kernels require 64-bit systems.
//
// On 32-bit x86, we lack 64-bit popcnt, lzcnt, blsr instructions.
// Furthermore, the number of SIMD registers is reduced.
//
// On 32-bit ARM, we would have smaller registers.
//
// The library should still have the fallback kernel. It is
// slower, but it should run everywhere.

//
// Enable valid runtime implementations, and select CROARING_BUILTIN_IMPLEMENTATION
//

// We are going to use runtime dispatch.
#ifdef CROARING_IS_X64
#ifdef __clang__
// clang does not have GCC push pop
// warning: clang attribute push can't be used within a namespace in clang up
// til 8.0 so CROARING_TARGET_REGION and CROARING_UNTARGET_REGION must be *outside* of a
// namespace.
#define CROARING_TARGET_REGION(T)                                                       \
  _Pragma(STRINGIFY(                                                           \
      clang attribute push(__attribute__((target(T))), apply_to = function)))
#define CROARING_UNTARGET_REGION _Pragma("clang attribute pop")
#elif defined(__GNUC__)
// GCC is easier
#define CROARING_TARGET_REGION(T)                                                       \
  _Pragma("GCC push_options") _Pragma(STRINGIFY(GCC target(T)))
#define CROARING_UNTARGET_REGION _Pragma("GCC pop_options")
#endif // clang then gcc

#endif // CROARING_IS_X64

// Default target region macros don't do anything.
#ifndef CROARING_TARGET_REGION
#define CROARING_TARGET_REGION(T)
#define CROARING_UNTARGET_REGION
#endif

#define CROARING_TARGET_AVX2 CROARING_TARGET_REGION("avx2,bmi,pclmul,lzcnt")

#ifdef __AVX2__
// No need for runtime dispatching.
// It is unnecessary and harmful to old clang to tag regions.
#undef CROARING_TARGET_AVX2
#define CROARING_TARGET_AVX2
#undef CROARING_UNTARGET_REGION
#define CROARING_UNTARGET_REGION
#endif


// We need portability.h to be included first,
// but we also always want isadetection.h to be
// included (right after).
// See https://github.com/RoaringBitmap/CRoaring/issues/394
// There is no scenario where we want portability.h to
// be included, but not isadetection.h: the latter is a
// strict requirement.
#include <roaring/isadetection.h> // include it last!
#endif /* INCLUDE_PORTABILITY_H_ */