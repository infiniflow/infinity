// Copyright (c) 2019, Sergey Abbakumov
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

#ifndef RST_STL_RESIZE_UNINITIALIZED_H_
#define RST_STL_RESIZE_UNINITIALIZED_H_

#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>

#include "not_null.h"

// Abseil-like utility for resizing string without default initializing.
namespace rst {
namespace internal {

template <class String, class = void>
struct ResizeUninitializedTraits {
  static void Resize(const NotNull<String*> s, const size_t new_size) {
    s->resize(new_size);
  }
};

// __resize_default_init() is provided by libc++ >= 8.0.
template <class String>
struct ResizeUninitializedTraits<
    String,
    std::void_t<decltype(std::declval<String&>().__resize_default_init(237))>> {
  static void Resize(const NotNull<String*> s, const size_t new_size) {
    s->__resize_default_init(new_size);
  }
};

}  // namespace internal

// Like `std::string::resize()`, except any new characters added to a string as
// a result of resizing may be left uninitialized, rather than being filled with
// '\0' bytes. Typically used when code is then going to overwrite the backing
// store of the `std::string` with known data.
//
// Example:
//
//   #include "rst/stl/resize_uninitialized.h"
//
//   std::string s;
//   rst::StringResizeUninitialized(&s, 10);
//   void* ptr = ...
//   std::memcpy(s.data(), ptr, 10);
//
template <class String>
void StringResizeUninitialized(const NotNull<String*> s,
                               const size_t new_size) {
  internal::ResizeUninitializedTraits<String>::Resize(s, new_size);
}

template <class String>
void StringResizeUninitialized(String* s, const size_t new_size) {
  StringResizeUninitialized(NotNull(s), new_size);
}

}  // namespace rst

#endif  // RST_STL_RESIZE_UNINITIALIZED_H_
