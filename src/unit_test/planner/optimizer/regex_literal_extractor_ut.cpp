// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

#include "common/utility/sparse_gram.h"
#include "planner/optimizer/regex_literal_extractor.h"
#include "unit_test/gtest_expand.h"

module infinity_core:ut.regex_literal_extractor;

using namespace infinity;

namespace {

std::vector<std::string> Runs(const RegexExtractionResult &result) {
    std::vector<std::string> texts;
    for (const auto &run : result.runs) {
        texts.push_back(run.text);
    }
    return texts;
}

} // namespace

TEST(RegexLiteralExtractorTest, top_level_alternation_requires_nothing) {
    EXPECT_TRUE(ExtractRegexLiteralRuns("Nobel|nothing relevant", false, false).runs.empty());
    EXPECT_TRUE(ExtractRegexLiteralRuns("a|bcd|efg", false, false).runs.empty());
    // A grouped alternation still keeps the literals around the group.
    EXPECT_EQ(Runs(ExtractRegexLiteralRuns("(?:Nobel|nothing) Prize", false, false)), std::vector<std::string>{" Prize"});
}

TEST(RegexLiteralExtractorTest, scoped_case_insensitive_group_keeps_its_mode) {
    // Without a folded index the group's text cannot be required.
    EXPECT_EQ(Runs(ExtractRegexLiteralRuns("(?i:THE NOBEL) Prize", false, false)), std::vector<std::string>{" Prize"});

    const auto folded = ExtractRegexLiteralRuns("(?i:THE NOBEL) Prize", false, true);
    ASSERT_EQ(folded.runs.size(), 2u);
    EXPECT_EQ(folded.runs[0].text, "the nobel");
    EXPECT_TRUE(folded.runs[0].lowered);
    EXPECT_EQ(folded.runs[1].text, " Prize");
    EXPECT_FALSE(folded.runs[1].lowered);
}

TEST(RegexLiteralExtractorTest, re2_escapes) {
    EXPECT_TRUE(ExtractRegexLiteralRuns(R"(\Qmain()\E)", false, false).runs.empty());
    EXPECT_EQ(Runs(ExtractRegexLiteralRuns(R"(ma\Cn)", false, false)), (std::vector<std::string>{"ma", "n"}));
    EXPECT_EQ(Runs(ExtractRegexLiteralRuns(R"(x\ay)", false, false)), std::vector<std::string>{"x\ay"});
}
