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
#include <vector>

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
    String query_str("你好 Dog");
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

TEST_F(HighlighterTest, test3) {
    StandardAnalyzer analyzer;
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(true);
    analyzer.SetStemOnly(true);
    analyzer.SetCharOffset(true);
    String query_str("harmful chemical anarchism");
    TermList term_list;
    analyzer.Analyze(query_str, term_list);
    Vector<String> query;
    for (auto &term : term_list) {
        query.push_back(term.text_);
    }
    String raw_text =
        R"##({{Redirect|Anarchist|the fictional character|Anarchist (comics)}} {{Redirect|Anarchists}} {{Anarchism sidebar}} {{Libertarianism sidebar}}  '''Anarchism''' is generally defined as the [[political philosophy]] which holds the [[state (polity)|state]] to be undesirable, unnecessary, and harmful,<ref name="definition"> {{Cite journal|last=Malatesta|first=Errico|title=Towards Anarchism|journal=MAN!|publisher=International Group of San Francisco|location=Los Angeles|oclc=3930443|url=http://www.marxists.org/archive/malatesta/1930s/xx/toanarchy.htm|authorlink=Errico Malatesta}} {{Cite journal|url=http://www.theglobeandmail.com/servlet/story/RTGAM.20070514.wxlanarchist14/BNStory/lifeWork/home/ |title=Working for The Man |journal=[[The Globe and Mail]] |accessdate=2008-04-14 |last=Agrell |first=Siri |date=2007-05-14}} {{cite web|url=http://www.britannica.com/eb/article-9117285|title=Anarchism|year=2006|work=Encyclopædia Britannica|publisher=Encyclopædia Britannica Premium Service|accessdate=2006-08-29| archiveurl=)##";

    String output;
    Highlighter::instance().GetHighlightWithStemmer(query, raw_text, output, &analyzer);
    std::cout << output << std::endl;
}
