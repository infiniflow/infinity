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

#include "rst/strings/format.h"

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "rst/not_null/not_null.h"

namespace rst {

TEST(Format, Escape) {
  EXPECT_EQ(Format("{{"), "{");
  EXPECT_EQ(Format("before {{"), "before {");
  EXPECT_EQ(Format("{{ after"), "{ after");
  EXPECT_EQ(Format("before {{ after"), "before { after");

  EXPECT_EQ(Format("}}"), "}");
  EXPECT_EQ(Format("before }}"), "before }");
  EXPECT_EQ(Format("}} after"), "} after");
  EXPECT_EQ(Format("before }} after"), "before } after");

  EXPECT_EQ(Format("{{}}"), "{}");
  EXPECT_EQ(Format("{{{}}}", {42}), "{42}");
}

TEST(Format, UnmatchedBraces) {
  EXPECT_DEATH(Format("{"), "");
  EXPECT_DEATH(Format("}"), "");
  EXPECT_DEATH(Format("{0{}"), "");
}

TEST(Format, UnmatchedBracesWithArgs) {
  EXPECT_DEATH(Format("{", {1}), "");
  EXPECT_DEATH(Format("}", {1}), "");
  EXPECT_DEATH(Format("{0{}", {1}), "");
}

TEST(Format, NoArgs) { EXPECT_EQ(Format("test"), "test"); }

TEST(Format, ArgsInDifferentPositions) {
  EXPECT_EQ(Format("{}", {42}), "42");
  EXPECT_EQ(Format("before {}", {42}), "before 42");
  EXPECT_EQ(Format("{} after", {42}), "42 after");
  EXPECT_EQ(Format("before {} after", {42}), "before 42 after");
  EXPECT_EQ(Format("{} = {}", {"answer", 42}), "answer = 42");
  EXPECT_EQ(Format("{} is the {}", {42, "answer"}), "42 is the answer");
  EXPECT_EQ(Format("{}{}{}", {"abra", "cad", "abra"}), "abracadabra");
}

TEST(Format, ArgErrors) {
  EXPECT_DEATH(Format("{"), "");
  EXPECT_DEATH(Format("{?}"), "");
  EXPECT_DEATH(Format("{0"), "");
  EXPECT_DEATH(Format("{}"), "");
}

TEST(Format, ArgErrorsWithArgs) {
  EXPECT_DEATH(Format("{", {1}), "");
  EXPECT_DEATH(Format("{?}", {1}), "");
  EXPECT_DEATH(Format("{0", {1}), "");
}

TEST(Format, Variadic) { EXPECT_EQ(Format("{}c{}", {"ab", 1}), "abc1"); }

TEST(Format, MaxArgs) {
  EXPECT_EQ(Format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{} {} {}",
                   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'a', 'b', 'c', 'd', 'e', 1.23,
                    1.23f}),
            "0123456789abcde 1.23 1.23");
}

TEST(Format, ExtraArgument) {
  EXPECT_DEATH(Format("", {1}), "");
  EXPECT_DEATH(Format("1", {1}), "");
  EXPECT_DEATH(Format("string", {1}), "");
  EXPECT_DEATH(Format("string{}{}", {1}), "");
}

TEST(Format, Strings) {
  const std::string s = "string";
  EXPECT_EQ(Format("{}", {s}), "string");
  EXPECT_EQ(Format("{}{}", {s, s}), "stringstring");
  EXPECT_EQ(Format("{}", {std::string("temp")}), "temp");
  EXPECT_EQ(Format("{}", {std::string_view("temp")}), "temp");
}

TEST(Format, Common) {
  EXPECT_EQ(Format(""), std::string());
  EXPECT_EQ(Format("{} {}, {}{}{}", {1234, "Hello", "wor", 'l', 'd'}),
            "1234 Hello, world");
}

TEST(Format, Numbers) {
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
      Format("{}{}{}{}{}{}{}{}{}{}{}",
             {kShort, kInt, kLong, kLongLong, kUnsignedShort, kUnsignedInt,
              kUnsignedLong, kUnsignedLongLong, kFloat, kDouble, kLongDouble}),
      "012345678910");
}

TEST(Format, MinMax) {
  const auto string = Format(
      "{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}",
      {std::numeric_limits<short>::min(),  // NOLINT(runtime/int)
       std::numeric_limits<short>::max(),  // NOLINT(runtime/int)
       std::numeric_limits<int>::min(),
       std::numeric_limits<int>::max(),
       std::numeric_limits<long>::min(),            // NOLINT(runtime/int)
       std::numeric_limits<long>::max(),            // NOLINT(runtime/int)
       std::numeric_limits<long long>::min(),       // NOLINT(runtime/int)
       std::numeric_limits<long long>::max(),       // NOLINT(runtime/int)
       std::numeric_limits<unsigned short>::min(),  // NOLINT(runtime/int)
       std::numeric_limits<unsigned short>::max(),  // NOLINT(runtime/int)
       std::numeric_limits<unsigned int>::min(),
       std::numeric_limits<unsigned int>::max(),
       std::numeric_limits<unsigned long>::min(),       // NOLINT(runtime/int)
       std::numeric_limits<unsigned long>::max(),       // NOLINT(runtime/int)
       std::numeric_limits<unsigned long long>::min(),  // NOLINT(runtime/int)
       std::numeric_limits<unsigned long long>::max(),  // NOLINT(runtime/int)
       std::numeric_limits<float>::min(),
       std::numeric_limits<float>::max(),
       std::numeric_limits<double>::min(),
       std::numeric_limits<double>::max(),
       std::numeric_limits<long double>::min(),
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

TEST(Format, EmptyStdString) {
  const std::string str;
  EXPECT_EQ(Format("{}", {str}), std::string());
}

TEST(Format, EmptyStringView) {
  const std::string_view str;
  EXPECT_EQ(Format("{}", {str}), std::string());
}

TEST(Format, EmptyCharPtr) {
  static constexpr auto kStr = "";
  EXPECT_EQ(Format("{}", {kStr}), std::string());
}

TEST(Format, NormalStdString) {
  const std::string str = "Normal";
  EXPECT_EQ(Format("{}", {str}), "Normal");
}

TEST(Format, NormalCharPtr) {
  static constexpr auto kStr = "Normal";
  EXPECT_EQ(Format("{}", {kStr}), "Normal");
}

TEST(Format, StringView) {
  static constexpr std::string_view kStr = "Normal";
  EXPECT_EQ(Format("{}", {kStr}), "Normal");
}

TEST(Format, NormalChar) {
  static constexpr auto kC = 'C';
  EXPECT_EQ(Format("{}", {kC}), "C");
}

TEST(Format, BigStdString) {
  const std::string str(1024, 'A');
  EXPECT_EQ(Format("{}", {str}), str);
}

TEST(Format, BigCharPtr) {
  const std::string str(1024, 'A');
  EXPECT_EQ(Format("{}", {str.c_str()}), str);
}

TEST(Format, AppendBigStdString) {
  const std::string initial_str = "Initial";
  const std::string str(1024, 'A');
  EXPECT_EQ(Format("{}{}{}", {initial_str, str, initial_str}),
            initial_str + str + initial_str);
}

TEST(Format, AppendBigCharPtr) {
  static constexpr auto kInitialStr = "Initial";
  const std::string str(1024, 'A');
  EXPECT_EQ(Format("{}{}{}", {kInitialStr, str.c_str(), kInitialStr}),
            kInitialStr + str + kInitialStr);
}

TEST(Format, AppendBigChar) {
  const std::string str(1024, 'A');
  static constexpr auto kC = 'C';
  EXPECT_EQ(Format("{}{}", {str, kC}), str + kC);
}

TEST(Format, Bool) {
  EXPECT_EQ(Format("{}", {false}), "false");
  EXPECT_EQ(Format("{}", {true}), "true");
}

TEST(Format, Doubles) {
  std::ostringstream stream;

  for (int64_t i = -11000; i < 11000; i++) {
    stream.str(std::string());
    stream << static_cast<double>(i);
    EXPECT_EQ(Format("{}", {static_cast<double>(i)}), stream.str());
  }
}

TEST(Format, Enum) {
  enum Old {
    kZero,
    kOne,
    kTwo,
    kThree,
  };

  EXPECT_EQ(Format("{}{}{}{}", {kZero, kOne, kTwo, kThree}), "0123");
}

TEST(Format, ScopedEnum) {
  enum class New {
    kZero,
    kOne,
    kTwo,
    kThree,
  };

  EXPECT_EQ(Format("{}{}{}{}", {New::kZero, New::kOne, New::kTwo, New::kThree}),
            "0123");
}

TEST(Format, StringCapacity) {
  static constexpr char kStr[] = "abcdefghijklmno";
  EXPECT_EQ(Format(kStr), kStr);
}

TEST(Format, NotNullStr) {
  static constexpr char kStr[] = "abcdefghijklmno";
  EXPECT_EQ(Format("{}", {NotNull(kStr)}), kStr);
}

}  // namespace rst
