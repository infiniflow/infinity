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

#include "rst/strings/str_cat.h"

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "rst/not_null/not_null.h"

namespace rst {

TEST(StrCat, NoArgs) { EXPECT_EQ(StrCat({"test"}), "test"); }

TEST(StrCat, ArgsInDifferentPositions) {
  EXPECT_EQ(StrCat({42}), "42");
  EXPECT_EQ(StrCat({"before ", 42}), "before 42");
  EXPECT_EQ(StrCat({42, " after"}), "42 after");
  EXPECT_EQ(StrCat({"before ", 42, " after"}), "before 42 after");
  EXPECT_EQ(StrCat({"answer", " = ", 42}), "answer = 42");
  EXPECT_EQ(StrCat({"abra", "cad", "abra"}), "abracadabra");
}

TEST(StrCat, Variadic) { EXPECT_EQ(StrCat({"ab", "c", 1}), "abc1"); }

TEST(StrCat, MaxArgs) {
  EXPECT_EQ(StrCat({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'a', 'b', 'c', 'd', 'e', 1.23,
                    1.23f}),
            "0123456789abcde1.231.23");
}

TEST(StrCat, Strings) {
  const std::string s = "string";
  EXPECT_EQ(StrCat({s}), "string");
  EXPECT_EQ(StrCat({s, s}), "stringstring");
  EXPECT_EQ(StrCat({std::string("temp")}), "temp");
  EXPECT_EQ(StrCat({std::string_view("temp")}), "temp");
}

TEST(StrCat, Common) {
  EXPECT_EQ(StrCat({}), std::string());
  EXPECT_EQ(StrCat({1234, " Hello, ", "wor", 'l', 'd'}), "1234 Hello, world");
}

TEST(StrCat, Numbers) {
  static constexpr short kShort = 0;  // NOLINT(runtime/int)
  static constexpr int kInt = 1;
  static constexpr long kLong = 2;                     // NOLINT(runtime/int)
  static constexpr long long kLongLong = 3;            // NOLINT(runtime/int)
  static constexpr unsigned short kUnsignedShort = 4;  // NOLINT(runtime/int)
  static constexpr unsigned int kUnsignedInt = 5;
  static constexpr unsigned long kUnsignedLong = 6;  // NOLINT(runtime/int)
  static constexpr auto kUnsignedLongLong =
      static_cast<unsigned long long>(7);  // NOLINT(runtime/int)
  static constexpr float kFloat = 8.0f;
  static constexpr double kDouble = 9.0;
  static constexpr long double kLongDouble = 10.0L;

  EXPECT_EQ(
      StrCat({kShort, kInt, kLong, kLongLong, kUnsignedShort, kUnsignedInt,
              kUnsignedLong, kUnsignedLongLong, kFloat, kDouble, kLongDouble}),
      "012345678910");
}

TEST(StrCat, MinMax) {
  const auto string = StrCat(
      {std::numeric_limits<short>::min(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<short>::max(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<int>::min(),
       " ",
       std::numeric_limits<int>::max(),
       " ",
       std::numeric_limits<long>::min(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<long>::max(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<long long>::min(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<long long>::max(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<unsigned short>::min(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<unsigned short>::max(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<unsigned int>::min(),
       " ",
       std::numeric_limits<unsigned int>::max(),
       " ",
       std::numeric_limits<unsigned long>::min(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<unsigned long>::max(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<unsigned long long>::min(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<unsigned long long>::max(),  // NOLINT(runtime/int)
       " ",
       std::numeric_limits<float>::min(),
       " ",
       std::numeric_limits<float>::max(),
       " ",
       std::numeric_limits<double>::min(),
       " ",
       std::numeric_limits<double>::max(),
       " ",
       std::numeric_limits<long double>::min(),
       " ",
       std::numeric_limits<long double>::max()});

  std::string result;
  result +=
      std::to_string(std::numeric_limits<short>::min());  // NOLINT(runtime/int)
  result += ' ';
  result +=
      std::to_string(std::numeric_limits<short>::max());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(std::numeric_limits<int>::min());
  result += ' ';
  result += std::to_string(std::numeric_limits<int>::max());
  result += ' ';
  result +=
      std::to_string(std::numeric_limits<long>::min());  // NOLINT(runtime/int)
  result += ' ';
  result +=
      std::to_string(std::numeric_limits<long>::max());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(
      std::numeric_limits<long long>::min());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(
      std::numeric_limits<long long>::max());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(
      std::numeric_limits<unsigned short>::min());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(
      std::numeric_limits<unsigned short>::max());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(std::numeric_limits<unsigned int>::min());
  result += ' ';
  result += std::to_string(std::numeric_limits<unsigned int>::max());
  result += ' ';
  result += std::to_string(
      std::numeric_limits<unsigned long>::min());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(
      std::numeric_limits<unsigned long>::max());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(
      std::numeric_limits<unsigned long long>::min());  // NOLINT(runtime/int)
  result += ' ';
  result += std::to_string(
      std::numeric_limits<unsigned long long>::max());  // NOLINT(runtime/int)
  result += ' ';

  std::ostringstream stream;
  stream << std::numeric_limits<float>::min();
  stream << ' ';
  stream << std::numeric_limits<float>::max();
  stream << ' ';
  stream << std::numeric_limits<double>::min();
  stream << ' ';
  stream << std::numeric_limits<double>::max();
  stream << ' ';
  stream << std::numeric_limits<long double>::min();
  stream << ' ';
  stream << std::numeric_limits<long double>::max();

  result += stream.str();

  EXPECT_EQ(string, result);
}

TEST(StrCat, EmptyStdString) {
  const std::string str;
  EXPECT_EQ(StrCat({str}), std::string());
}

TEST(StrCat, EmptyStringView) {
  const std::string_view str;
  EXPECT_EQ(StrCat({str}), std::string());
}

TEST(StrCat, EmptyCharPtr) {
  static constexpr auto kStr = "";
  EXPECT_EQ(StrCat({kStr}), std::string());
}

TEST(StrCat, NormalStdString) {
  const std::string str = "Normal";
  EXPECT_EQ(StrCat({str}), "Normal");
}

TEST(StrCat, NormalCharPtr) {
  static constexpr auto kStr = "Normal";
  EXPECT_EQ(StrCat({kStr}), "Normal");
}

TEST(StrCat, StringView) {
  static constexpr std::string_view kStr = "Normal";
  EXPECT_EQ(StrCat({kStr}), "Normal");
}

TEST(StrCat, NormalChar) {
  static constexpr auto kC = 'C';
  EXPECT_EQ(StrCat({kC}), "C");
}

TEST(StrCat, BigStdString) {
  const std::string str(1024, 'A');
  EXPECT_EQ(StrCat({str}), str);
}

TEST(StrCat, BigCharPtr) {
  const std::string str(1024, 'A');
  EXPECT_EQ(StrCat({str.c_str()}), str);
}

TEST(StrCat, AppendBigStdString) {
  const std::string initial_str = "Initial";
  const std::string str(1024, 'A');
  EXPECT_EQ(StrCat({initial_str, str, initial_str}),
            initial_str + str + initial_str);
}

TEST(StrCat, AppendBigCharPtr) {
  static constexpr auto kInitialStr = "Initial";
  const std::string str(1024, 'A');
  EXPECT_EQ(StrCat({kInitialStr, str.c_str(), kInitialStr}),
            kInitialStr + str + kInitialStr);
}

TEST(StrCat, AppendBigChar) {
  const std::string str(1024, 'A');
  static constexpr auto kC = 'C';
  EXPECT_EQ(StrCat({str, kC}), str + kC);
}

TEST(StrCat, Bool) {
  EXPECT_EQ(StrCat({false}), "false");
  EXPECT_EQ(StrCat({true}), "true");
}

TEST(StrCat, Doubles) {
  std::ostringstream stream;

  for (int64_t i = -11000; i < 11000; i++) {
    stream.str(std::string());
    stream << static_cast<double>(i);
    EXPECT_EQ(StrCat({static_cast<double>(i)}), stream.str());
  }
}

TEST(StrCat, Enum) {
  enum Old {
    kZero,
    kOne,
    kTwo,
    kThree,
  };

  EXPECT_EQ(StrCat({kZero, kOne, kTwo, kThree}), "0123");
}

TEST(StrCat, ScopedEnum) {
  enum class New {
    kZero,
    kOne,
    kTwo,
    kThree,
  };

  EXPECT_EQ(StrCat({New::kZero, New::kOne, New::kTwo, New::kThree}), "0123");
}

TEST(StrCat, StringCapacity) {
  static constexpr char kStr[] = "abcdefghijklmno";
  EXPECT_EQ(StrCat({kStr}), kStr);
}

TEST(StrCat, NotNullStr) {
  static constexpr char kStr[] = "abcdefghijklmno";
  EXPECT_EQ(StrCat({NotNull(kStr)}), kStr);
}

}  // namespace rst
