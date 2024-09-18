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

#include "gtest/gtest.h"

#include <deque>
#include <iostream>

import base_test;
import stl;
import standard_analyzer;
import highlighter;
import term;

using namespace infinity;

class HighlighterTest : public BaseTest {};

TEST_F(HighlighterTest, test1) {
    Vector<String> query;
    query.push_back("Dog");
    query.push_back("你好");
    String raw_text = R"##(
once upon a time there lives an dog.this is a sentence,That is another.你好世界。

真的好么？
    )##";
    String output;
    Highlighter::instance().GetHighlightWithoutStemmer(query, raw_text, output);
    std::cout << output << std::endl;
}

TEST_F(HighlighterTest, test2) {
    StandardAnalyzer analyzer;
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(true);
    analyzer.SetCharOffset(true);
    String query_str("Dog 你好");
    TermList term_list;
    analyzer.Analyze(query_str, term_list);
    Vector<String> query;
    for (auto &term : term_list)
        query.push_back(term.text_);
    String raw_text = R"##(
once upon a time there lives an dog.this is a sentence,That is another.你好 世界 dog 你好 dog
真的好么？
    )##";
    String output;
    Highlighter::instance().GetHighlightWithStemmer(query, raw_text, output, &analyzer);
    std::cout << output << std::endl;
}
