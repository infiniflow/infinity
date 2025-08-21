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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.ngram_analyzer;

import :ut.base_test;
import :term;
import :ngram_analyzer;
import :standard_analyzer;

using namespace infinity;

class NGramAnalyzerTest : public BaseTest {};

TEST_F(NGramAnalyzerTest, test1) {
    NGramAnalyzer analyzer(2);
    TermList term_list;
    std::string input("hello world  123");
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 10U);
    ASSERT_EQ(term_list[0].text_, std::string("he"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, std::string("el"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, std::string("ll"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    ASSERT_EQ(term_list[3].text_, std::string("lo"));
    ASSERT_EQ(term_list[3].word_offset_, 3U);
    ASSERT_EQ(term_list[4].text_, std::string("wo"));
    ASSERT_EQ(term_list[4].word_offset_, 4U);
    ASSERT_EQ(term_list[5].text_, std::string("or"));
    ASSERT_EQ(term_list[5].word_offset_, 5U);
    ASSERT_EQ(term_list[6].text_, std::string("rl"));
    ASSERT_EQ(term_list[6].word_offset_, 6U);
    ASSERT_EQ(term_list[7].text_, std::string("ld"));
    ASSERT_EQ(term_list[7].word_offset_, 7U);
    ASSERT_EQ(term_list[8].text_, std::string("12"));
    ASSERT_EQ(term_list[8].word_offset_, 8U);
    ASSERT_EQ(term_list[9].text_, std::string("23"));
    ASSERT_EQ(term_list[9].word_offset_, 9U);
}

TEST_F(NGramAnalyzerTest, test2) {
    NGramAnalyzer analyzer(1);
    TermList term_list;
    std::string input("abc de  fg");
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 7U);
    ASSERT_EQ(term_list[0].text_, std::string("a"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, std::string("b"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, std::string("c"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    ASSERT_EQ(term_list[3].text_, std::string("d"));
    ASSERT_EQ(term_list[3].word_offset_, 3U);
    ASSERT_EQ(term_list[4].text_, std::string("e"));
    ASSERT_EQ(term_list[4].word_offset_, 4U);
    ASSERT_EQ(term_list[5].text_, std::string("f"));
    ASSERT_EQ(term_list[5].word_offset_, 5U);
    ASSERT_EQ(term_list[6].text_, std::string("g"));
    ASSERT_EQ(term_list[6].word_offset_, 6U);
}
