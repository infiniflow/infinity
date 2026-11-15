// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.rag_regex;

namespace infinity {

std::pair<std::string, std::vector<std::pair<unsigned, unsigned>>>
PCRE2GlobalReplaceWithPosition(const std::string &text, const std::string &pattern, const std::string &replacement);

TEST(RAGRegexTest, unicode_replacement_preserves_byte_positions) {
    const auto [text, positions] = PCRE2GlobalReplaceWithPosition("A<中>é🙂Z", R"(\W+)", " ");
    EXPECT_EQ(text, "A 中 é Z");
    const std::vector<std::pair<unsigned, unsigned>> expected = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {12, 12}};
    EXPECT_EQ(positions, expected);
}

TEST(RAGRegexTest, empty_and_unmatched_input) {
    const auto [empty, empty_positions] = PCRE2GlobalReplaceWithPosition("", R"(\W+)", " ");
    EXPECT_TRUE(empty.empty());
    EXPECT_TRUE(empty_positions.empty());

    const auto [text, positions] = PCRE2GlobalReplaceWithPosition("abc", R"(\W+)", " ");
    EXPECT_EQ(text, "abc");
    const std::vector<std::pair<unsigned, unsigned>> expected = {{0, 0}, {1, 1}, {2, 2}};
    EXPECT_EQ(positions, expected);
}

TEST(RAGRegexTest, invalid_utf8_is_checked_before_any_replacement) {
    // An invalid suffix must be detected even when an earlier punctuation match exists.
    for (const auto &suffix : {std::string("\xff", 1), std::string("\xe4\xb8", 2)}) {
        const std::string input = "a!b" + suffix;
        const auto [text, positions] = PCRE2GlobalReplaceWithPosition(input, R"(\W+)", " ");
        EXPECT_EQ(text, input);
        ASSERT_EQ(positions.size(), input.size());
        for (unsigned i = 0; i < positions.size(); ++i) {
            EXPECT_EQ(positions[i], std::make_pair(i, i));
        }
    }
}

TEST(RAGRegexTest, non_character_offsets_remain_checked) {
    // Empty matches and byte-wise patterns can advance into a continuation byte.
    // Preserve the checked matcher's fallback instead of passing an unsafe offset.
    const auto [empty_match, empty_positions] = PCRE2GlobalReplaceWithPosition("中!", "", "_");
    EXPECT_EQ(empty_match, "_中!");
    const std::vector<std::pair<unsigned, unsigned>> expected_empty = {{0, 0}, {0, 0}, {1, 1}, {2, 2}, {3, 3}};
    EXPECT_EQ(empty_positions, expected_empty);

    const auto [byte_match, byte_positions] = PCRE2GlobalReplaceWithPosition("中!", R"(\C)", "_");
    EXPECT_EQ(byte_match, std::string("_\xb8\xad!", 4));
    const std::vector<std::pair<unsigned, unsigned>> expected_byte = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    EXPECT_EQ(byte_positions, expected_byte);
}

TEST(RAGRegexTest, large_html_replacement) {
    // Repeated UTF validation used to make this dense-match input quadratic.
    const std::string row = "<td>物料123</td>";
    const std::string replaced_row = " td 物料123 td ";
    const size_t rows = 4 * 1024 * 1024 / row.size();
    std::string input;
    std::string expected;
    input.reserve(rows * row.size());
    expected.reserve(rows * replaced_row.size());
    for (size_t i = 0; i < rows; ++i) {
        input += row;
        // Adjacent closing/opening angle brackets form one non-word match.
        expected += i == 0 ? replaced_row : replaced_row.substr(1);
    }

    const auto start = std::chrono::steady_clock::now();
    const auto [text, positions] = PCRE2GlobalReplaceWithPosition(input, R"(\W+)", " ");
    const auto elapsed = std::chrono::steady_clock::now() - start;
    EXPECT_EQ(text, expected);
    ASSERT_EQ(positions.size(), text.size());
    EXPECT_EQ(positions.back().first, input.size() - 1);
    // Generous for a linear scan of 4 MiB, including debug builds.
    EXPECT_LT(elapsed, std::chrono::seconds(5));
}

} // namespace infinity
