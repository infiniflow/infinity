// Copyright (c) 2017, Sergey Abbakumov
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

#ifndef RST_CHECK_CHECK_H_
#define RST_CHECK_CHECK_H_

#include <cassert>
#include <cstdlib>

#include "optimization.h"

// Chromium-like checking macros for better programming error handling.
//
// The `RST_CHECK()` macro will cause an immediate crash if its condition is not
// met. `RST_DCHECK()` is like `RST_CHECK()` but is only compiled in debug
// build. `RST_NOTREACHED()` is equivalent to `RST_DCHECK(false)`. Here are some
// rules for using these:
//
// Use `RST_DCHECK()` or `RST_NOTREACHED()` as assertions, e.g. to document pre-
// and post-conditions. A `RST_DCHECK()` means "this condition must always be
// true", not "this condition is normally true, but perhaps not in exceptional
// cases." Things like disk corruption or strange network errors are examples
// of exceptional circumstances that nevertheless should not result in
// `RST_DCHECK()` failure.
// A consequence of this is that you should not handle `RST_DCHECK()` failures,
// even if failure would result in a crash. Attempting to handle a
// `RST_DCHECK()` failure is a statement that the `RST_DCHECK()` can fail, which
// contradicts the point of writing the `RST_DCHECK()`. In particular, do not
// write code like the following:
//
//   #include "rst/check/check.h"
//
//   RST_DCHECK(foo);
//   if (!foo)  // Eliminate this code.
//     ...
//
//   if (!bar) {  // Replace this whole conditional with "RST_DCHECK(bar);".
//     RST_NOTREACHED();
//     return;
//   }
//
// Use `RST_CHECK()` if the consequence of a failed assertion would be a
// security vulnerability or a contract violation, where crashing is preferable.
//
// If you want to do more complex logic in a debug build write the following:
//
//   #include "rst/check/check.h"
//   #include "rst/macros/macros.h"
//
//   #if RST_BUILDFLAG(DCHECK_IS_ON)
//   // Some complex logic in a debug build.
//   #endif  // RST_BUILDFLAG(DCHECK_IS_ON)
//
#if defined(NDEBUG)

#define RST_BUILDFLAG_DCHECK_IS_ON() (false)

// Ensures |condition| is referenced to not make warnings on unused variables in
// the code like:
//
//   bool is_used = IsUsed();
//   RST_DCHECK(is_used);
//
#define RST_DCHECK(condition)                                    \
  do {                                                           \
    false ? static_cast<void>(condition) : static_cast<void>(0); \
  } while (false)

#else  // !defined(NDEBUG)

#define RST_BUILDFLAG_DCHECK_IS_ON() (true)
#define RST_DCHECK(condition) assert(condition)

#endif  // defined(NDEBUG)

#define RST_NOTREACHED() RST_DCHECK(false)

#define RST_CHECK(condition)        \
  do {                              \
    if (RST_UNLIKELY(!(condition))) \
      std::abort();                 \
  } while (false)

#endif  // RST_CHECK_CHECK_H_
