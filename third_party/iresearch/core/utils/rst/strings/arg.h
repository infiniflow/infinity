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

#ifndef RST_STRINGS_ARG_H_
#define RST_STRINGS_ARG_H_

#include <charconv>
#include <cstddef>
#include <cstdio>
#include <string>
#include <string_view>
#include <type_traits>

#include "check.h"
#include "macros.h"
#include "not_null.h"

namespace rst {
namespace internal {

template <class Float, size_t N>
std::string_view FloatToString(char (&str)[N],
                               const NotNull<const char*> format,
                               const Float val) {
  static_assert(std::is_floating_point_v<Float>);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  // NOLINTNEXTLINE(runtime/printf)
  const auto bytes_written = std::sprintf(str, format.get(), val);
#pragma clang diagnostic pop
  RST_DCHECK(bytes_written > 0);
  RST_DCHECK(static_cast<size_t>(bytes_written) < N);
  RST_DCHECK(str[bytes_written] == '\0');
  return std::string_view(str, static_cast<size_t>(bytes_written));
}

template <class Int, size_t N>
std::string_view IntToString(char (&str)[N], const Int val) {
  const auto result = std::to_chars(str, str + N, val);
  RST_DCHECK(result.ec == std::errc());
  return std::string_view(str, static_cast<size_t>(result.ptr - str));
}

class Arg {
 public:
  static constexpr size_t kBufferSize = 20;

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const bool value) : view_(value ? "true" : "false") {}

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const char value) : view_(buffer_, 1) { buffer_[0] = value; }

  Arg(const short value) : view_(IntToString(buffer_, value)) {}  // NOLINT(*)

  // NOLINTNEXTLINE(*)
  Arg(const unsigned short value) : view_(IntToString(buffer_, value)) {}

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const int value) : view_(IntToString(buffer_, value)) {}

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const unsigned int value) : view_(IntToString(buffer_, value)) {}

  Arg(const long value) : view_(IntToString(buffer_, value)) {}  // NOLINT(*)

  // NOLINTNEXTLINE(*)
  Arg(const unsigned long value) : view_(IntToString(buffer_, value)) {}

  // NOLINTNEXTLINE(*)
  Arg(const long long value) : view_(IntToString(buffer_, value)) {}

  // NOLINTNEXTLINE(*)
  Arg(const unsigned long long value) : view_(IntToString(buffer_, value)) {}

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const float value) : Arg(static_cast<double>(value)) {}

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const double value) : view_(FloatToString(buffer_, "%g", value)) {}

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const long double value) : view_(FloatToString(buffer_, "%Lg", value)) {}

  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const std::string_view value) : view_(value) {}
  Arg(const std::string& value) : view_(value) {}  // NOLINT(runtime/explicit)

  // Provides const char* overload since otherwise it will be implicitly
  // converted to bool.
  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const char* value) : view_(value) { RST_DCHECK(value != nullptr); }
  // NOLINTNEXTLINE(runtime/explicit)
  Arg(const NotNull<const char*> value) : view_(value.get()) {}

  template <class T, class = typename std::enable_if_t<std::is_enum_v<T>>>
  Arg(const T e)  // NOLINT(runtime/explicit)
      : Arg(static_cast<typename std::underlying_type_t<T>>(e)) {}

  // Prevents Arg(pointer) from accidentally producing a bool.
  Arg(void*) = delete;  // NOLINT(runtime/explicit)

  ~Arg() = default;

  std::string_view view() const { return view_; }
  size_t size() const { return view_.size(); }

 private:
  const std::string_view view_;
  char buffer_[kBufferSize];  // Can store 2^64 - 1 that is
                              // 18,446,744,073,709,551,615 (without '\0').

  RST_DISALLOW_COPY_AND_ASSIGN(Arg);
};

}  // namespace internal
}  // namespace rst

#endif  // RST_STRINGS_ARG_H_
