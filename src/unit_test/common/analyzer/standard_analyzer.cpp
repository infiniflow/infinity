// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include "unit_test/base_test.h"

import stl;
import term;
import standard_analyzer;
using namespace infinity;

namespace fs = std::filesystem;

class StandardAnalyzerTest : public BaseTest {};

TEST_F(StandardAnalyzerTest, test1) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.SetExtractEngStem(false);
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 3U);
    ASSERT_EQ(term_list[0].text_, String("boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test2) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.SetExtractEngStem(false);
    analyzer.SetCaseSensitive(true, false);
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 3U);
    ASSERT_EQ(term_list[0].text_, String("Boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test3) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 4U);
    ASSERT_EQ(term_list[0].text_, String("boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    ASSERT_EQ(term_list[3].text_, String("test"));
    ASSERT_EQ(term_list[3].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test4) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.SetExtractEngStem(false);
    analyzer.SetCaseSensitive(true, true);
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 4U);
    ASSERT_EQ(term_list[0].text_, String("Boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("boost"));
    ASSERT_EQ(term_list[1].word_offset_, 0U);
    ASSERT_EQ(term_list[2].text_, String("unit"));
    ASSERT_EQ(term_list[2].word_offset_, 1U);
    ASSERT_EQ(term_list[3].text_, String("tests"));
    ASSERT_EQ(term_list[3].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test5) {
    StandardAnalyzer analyzer;
    TermList term_list;
    analyzer.SetExtractEngStem(false);
    String input("BoostBoostboostBoostboost unit tests.");
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 3U);
    ASSERT_EQ(term_list[0].text_, String("boostboostboostboostboost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}
