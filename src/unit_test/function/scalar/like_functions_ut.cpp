// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.like_functions;

import :ut.base_test;
import :like;
import third_party;

using namespace infinity;

class LikeFunctionsTest : public BaseTest {};

// Test re2::RE2::QuoteMeta behavior
// This test shows how different characters are escaped by QuoteMeta.
// QuoteMeta conservatively escapes any character that MIGHT have special meaning in regex, not just the ones that actually do.
TEST_F(LikeFunctionsTest, re2_quote_meta_test) {
    EXPECT_EQ(re2::RE2::QuoteMeta("%"), "\\%");
    EXPECT_EQ(re2::RE2::QuoteMeta("_"), "_");
    EXPECT_EQ(re2::RE2::QuoteMeta("@"), "\\@");
    EXPECT_EQ(re2::RE2::QuoteMeta("a"), "a");
    EXPECT_EQ(re2::RE2::QuoteMeta("."), "\\.");
    EXPECT_EQ(re2::RE2::QuoteMeta("*"), "\\*");
    EXPECT_EQ(re2::RE2::QuoteMeta("["), "\\[");
    EXPECT_EQ(re2::RE2::QuoteMeta("\\"), "\\\\");
}

TEST_F(LikeFunctionsTest, like_to_regex_pattern_special_regex_chars_escape) {
    EXPECT_EQ(LikeToRegexPattern(".", '\\'), "\\.");
    EXPECT_EQ(LikeToRegexPattern("^", '\\'), "\\^");
    EXPECT_EQ(LikeToRegexPattern("$", '\\'), "\\$");
    EXPECT_EQ(LikeToRegexPattern("*", '\\'), "\\*");
    EXPECT_EQ(LikeToRegexPattern("+", '\\'), "\\+");
    EXPECT_EQ(LikeToRegexPattern("?", '\\'), "\\?");
    EXPECT_EQ(LikeToRegexPattern("{", '\\'), "\\{");
    EXPECT_EQ(LikeToRegexPattern("}", '\\'), "\\}");
    EXPECT_EQ(LikeToRegexPattern("[", '\\'), "\\[");
    EXPECT_EQ(LikeToRegexPattern("]", '\\'), "\\]");
    EXPECT_EQ(LikeToRegexPattern("\\", '\\'), "\\\\");
    EXPECT_EQ(LikeToRegexPattern("|", '\\'), "\\|");
    EXPECT_EQ(LikeToRegexPattern("(", '\\'), "\\(");
    EXPECT_EQ(LikeToRegexPattern(")", '\\'), "\\)");
}

TEST_F(LikeFunctionsTest, like_to_regex_pattern_basic_sanity) { EXPECT_EQ(LikeToRegexPattern("hello", '\\'), "hello"); }

TEST_F(LikeFunctionsTest, like_to_regex_pattern_percent_wildcard) {
    EXPECT_EQ(LikeToRegexPattern("%", '\\'), ".*");
    EXPECT_EQ(LikeToRegexPattern("%hello", '\\'), ".*hello");
    EXPECT_EQ(LikeToRegexPattern("hello%", '\\'), "hello.*");
    EXPECT_EQ(LikeToRegexPattern("hello%world", '\\'), "hello.*world");
    EXPECT_EQ(LikeToRegexPattern("%a%b%c%", '\\'), ".*a.*b.*c.*");
    EXPECT_EQ(LikeToRegexPattern("%%", '\\'), ".*.*");
}

TEST_F(LikeFunctionsTest, like_to_regex_pattern_underscore_wildcard) {
    EXPECT_EQ(LikeToRegexPattern("_", '\\'), ".");
    EXPECT_EQ(LikeToRegexPattern("_ello", '\\'), ".ello");
    EXPECT_EQ(LikeToRegexPattern("hello_", '\\'), "hello.");
    EXPECT_EQ(LikeToRegexPattern("h_llo", '\\'), "h.llo");
    EXPECT_EQ(LikeToRegexPattern("___", '\\'), "...");
    EXPECT_EQ(LikeToRegexPattern("%a_b%c_", '\\'), ".*a.b.*c.");
}

TEST_F(LikeFunctionsTest, like_to_regex_pattern_escape_character) {
    EXPECT_EQ(LikeToRegexPattern("\\%abc", '\\'), "\\%abc");
    EXPECT_EQ(LikeToRegexPattern("\\_abc", '\\'), "_abc");
    EXPECT_EQ(LikeToRegexPattern("\\\\abc", '\\'), "\\\\abc");
    EXPECT_EQ(LikeToRegexPattern("\\%hello\\_%world", '\\'), "\\%hello_.*world");
    EXPECT_EQ(LikeToRegexPattern("\\abc", '\\'), "abc");
}

TEST_F(LikeFunctionsTest, like_to_regex_pattern_custom_escape_char) {
    EXPECT_EQ(LikeToRegexPattern("!%abc", '!'), "\\%abc");
    EXPECT_EQ(LikeToRegexPattern("!_abc", '!'), "_abc");
    EXPECT_EQ(LikeToRegexPattern("!!abc", '!'), "\\!abc");
    EXPECT_EQ(LikeToRegexPattern("\\a%b_c", '!'), "\\\\a.*b.c");
    EXPECT_EQ(LikeToRegexPattern("!%!%!%", '!'), "\\%\\%\\%");
}

TEST_F(LikeFunctionsTest, like_to_regex_pattern_empty_pattern) { EXPECT_EQ(LikeToRegexPattern("", '\\'), ""); }

TEST_F(LikeFunctionsTest, like_to_regex_pattern_complex_real_world_patterns) {
    EXPECT_EQ(LikeToRegexPattern("%@%.%", '\\'), ".*\\@.*\\..*");
    EXPECT_EQ(LikeToRegexPattern("%.txt", '\\'), ".*\\.txt");
    EXPECT_EQ(LikeToRegexPattern("___-___-____", '\\'), "...\\-...\\-....");
    EXPECT_EQ(LikeToRegexPattern("test\\%value\\_here", '\\'), "test\\%value_here");
    EXPECT_EQ(LikeToRegexPattern("\\%\\_\\%", '\\'), "\\%_\\%");
    EXPECT_EQ(LikeToRegexPattern("a\\%b%c_d\\\\e", '\\'), "a\\%b.*c.d\\\\e");
}