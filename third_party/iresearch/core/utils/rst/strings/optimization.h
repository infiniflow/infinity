// Copyright (c) 2020, Sergey Abbakumov
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef RST_MACROS_OPTIMIZATION_H_
#define RST_MACROS_OPTIMIZATION_H_

// Enables the compiler to prioritize compilation using static analysis for
// likely paths within a boolean or switch branches.
//
// Example:
//
//   #include "rst/macros/optimization.h"
//
//   if (RST_LIKELY(expression))
//     return result;  // Faster if more likely.
//   else
//     return 0;
//
//   if (RST_UNLIKELY(expression))
//     return result;
//   else
//     return 0;  // Faster if more likely.
//
//   switch (RST_LIKELY_EQ(x, 5)) {
//     default:
//     case 0:
//     case 3:
//     case 5:  // Faster if more likely.
//   }
//
// Compilers can use the information that a certain branch is not likely to be
// taken (for instance, a `RST_CHECK()` failure) to optimize for the common case
// in the absence of better information.
//
// Recommendation: modern CPUs dynamically predict branch execution paths,
// typically with accuracy greater than 97%. As a result, annotating every
// branch in a codebase is likely counterproductive; however, annotating
// specific branches that are both hot and consistently mispredicted is likely
// to yield performance improvements.
#if defined(__GNUC__) || defined(__clang__)
#define RST_LIKELY_EQ(x, val) __builtin_expect(x, val)
#else  // !(defined(__GNUC__) || defined(__clang__))
#define RST_LIKELY_EQ(x, val) (static_cast<void>(val), x)
#endif  // defined(__GNUC__) || defined(__clang__)

#define RST_LIKELY(x) RST_LIKELY_EQ(x, 1)
#define RST_UNLIKELY(x) RST_LIKELY_EQ(x, 0)

#endif  // RST_MACROS_OPTIMIZATION_H_
