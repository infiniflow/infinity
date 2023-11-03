// Copyright (c) 2016, Sergey Abbakumov
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

#ifndef RST_STRINGS_FORMAT_H_
#define RST_STRINGS_FORMAT_H_

#include <cstddef>
#include <initializer_list>
#include <string>

#include "rst/not_null/not_null.h"
#include "rst/strings/arg.h"

namespace rst {
namespace internal {

std::string FormatAndReturnString(NotNull<const char*> format,
                                  size_t format_size,
                                  Nullable<const Arg*> values, size_t size);
}  // namespace internal

// This function is for efficiently performing string formatting.
//
// Unlike `printf`-style format specifiers, `rst::Format()` functions do not
// need to specify the type of the arguments. Supported arguments following the
// format string, such as `string`s, `string_view`s, `int`s, `float`s, and
// `bool`s, are automatically converted to `string`s during the formatting
// process. See below for a full list of supported types.
//
// `rst::Format()` does not allow you to specify how to format a value, beyond
// the default conversion to string. For example, you cannot format an integer
// in hex.
//
// The format string uses identifiers indicated by a {} like in Python.
//
// A '{{' or '}}' sequence in the format string causes a literal '{' or '}' to
// be output.
//
// Example:
//
//   #include "rst/strings/format.h"
//
//   std::string s = rst::Format("{} purchased {} {}", {"Bob", 5, "Apples"});
//   RST_DCHECK(s == "Bob purchased 5 Apples");
//
// Supported types:
//   * `std::string_view`, `std::string`, `const char*`
//   * `short`, `unsigned short`, `int`, `unsigned int`, `long`,
//     `unsigned long`, `long long`, `unsigned long long`
//   * `float`, `double`, `long double` (printed as if %g is specified for
//     `printf()`)
//   * `bool` (printed as "true" or "false")
//   * `char`
//   * `enum`s (printed as underlying integer type)
//
// If an invalid format string is provided, `rst::Format()` asserts in a debug
// build.
template <size_t N>
std::string Format(const char (&format)[N]) {
  return internal::FormatAndReturnString(format, N - 1, nullptr, 0);
}

template <size_t N>
std::string Format(const char (&format)[N],
                   const std::initializer_list<internal::Arg> values) {
  return internal::FormatAndReturnString(format, N - 1, values.begin(),
                                         values.size());
}

}  // namespace rst

#endif  // RST_STRINGS_FORMAT_H_
