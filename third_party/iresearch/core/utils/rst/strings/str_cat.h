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

#ifndef RST_STRINGS_STR_CAT_H_
#define RST_STRINGS_STR_CAT_H_

#include <initializer_list>
#include <string>

#include "arg.h"

namespace rst {

// This function is for efficiently performing merging an arbitrary number of
// strings or numbers into one string, and is designed to be the fastest
// possible way to construct a string out of a mix of raw C strings,
// `string_view` elements, `std::string` value, boolean and numeric values.
// `rst::StrCat()` is generally more efficient on string concatenations
// involving more than one unary operator, such as a + b + c or a += b + c,
// since it avoids the creation of temporary string objects during string
// construction.
//
// Supported arguments such as `string`s, `string_view`s, `int`s, `float`s, and
// `bool`s, are automatically converted to `string`s during the concatenation
// process. See below for a full list of supported types.
//
// Example:
//
//   #include "rst/strings/str_cat.h"
//
//   std::string s = rst::StrCat({"Bob", " purchased ", 5, " ", "Apples"});
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
std::string StrCat(std::initializer_list<internal::Arg> values);

}  // namespace rst

#endif  // RST_STRINGS_STR_CAT_H_
