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

module infinity_core:ut.highlighter;

import :ut.base_test;
import :standard_analyzer;
import :rag_analyzer;
import :highlighter;
import :term;
import :analyzer_pool;

using namespace infinity;

class HighlighterTest : public BaseTest {};

TEST_F(HighlighterTest, test1) {
    std::vector<std::string> query;
    query.push_back("dog");
    query.push_back("你好");
    std::string raw_text = R"##(
once upon a time there lives an dog.this is a sentence,That is another.你好世界。

真的好么？
    )##";
    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;
    std::cout << "Query: ";
    for (auto &q : query) {
        std::cout << q << " ";
    }
    std::cout << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output);
    std::cout << "Output: " << output << std::endl;
}

TEST_F(HighlighterTest, test2) {
    StandardAnalyzer analyzer;
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(true);
    analyzer.SetCharOffset(true);
    std::string query_str("你好 Dog");
    TermList term_list;
    analyzer.Analyze(query_str, term_list);
    std::vector<std::string> query;
    for (auto &term : term_list)
        query.push_back(term.text_);
    std::string raw_text = R"##(
once upon a time there lives an dog.this is a sentence,That is another.你好 世界 dog 你好 dog
真的好么？
    )##";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;
    std::cout << "Query: ";
    for (auto &q : query) {
        std::cout << q << " ";
    }
    std::cout << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output);
    std::cout << "Output: " << output << std::endl;
}

TEST_F(HighlighterTest, test3) {
    StandardAnalyzer analyzer;
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(true);
    analyzer.SetStemOnly(true);
    analyzer.SetCharOffset(true);
    std::string query_str("harmful chemical anarchism");
    TermList term_list;
    analyzer.Analyze(query_str, term_list);
    std::vector<std::string> query;
    for (auto &term : term_list) {
        query.push_back(term.text_);
    }
    std::string raw_text =
        R"##({{Redirect|Anarchist|the fictional character|Anarchist (comics)}} {{Redirect|Anarchists}} {{Anarchism sidebar}} {{Libertarianism sidebar}}  '''Anarchism''' is generally defined as the [[political philosophy]] which holds the [[state (polity)|state]] to be undesirable, unnecessary, and harmful,<ref name="definition"> {{Cite journal|last=Malatesta|first=Errico|title=Towards Anarchism|journal=MAN!|publisher=International Group of San Francisco|location=Los Angeles|oclc=3930443|url=http://www.marxists.org/archive/malatesta/1930s/xx/toanarchy.htm|authorlink=Errico Malatesta}} {{Cite journal|url=http://www.theglobeandmail.com/servlet/story/RTGAM.20070514.wxlanarchist14/BNStory/lifeWork/home/ |title=Working for The Man |journal=[[The Globe and Mail]] |accessdate=2008-04-14 |last=Agrell |first=Siri |date=2007-05-14}} {{cite web|url=http://www.britannica.com/eb/article-9117285|title=Anarchism|year=2006|work=Encyclopædia Britannica|publisher=Encyclopædia Britannica Premium Service|accessdate=2006-08-29| archiveurl=)##";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;
    std::cout << "Query: ";
    for (auto &q : query) {
        std::cout << q << " ";
    }
    std::cout << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output);
    std::cout << "Output: " << output << std::endl << std::endl;
}

TEST_F(HighlighterTest, test4) {
    auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer("rag-coarse");
    if (!status.ok()) {
        RecoverableError(status);
    }

    std::string query_str("南京长江大桥");
    TermList term_list;
    analyzer->Analyze(query_str, term_list);
    std::vector<std::string> query;
    for (auto &term : term_list) {
        query.push_back(term.text_);
    }
    std::string raw_text = "南京的南京长江大桥是中国的一条重要的交通";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;
    std::cout << "Query: ";
    for (auto &q : query) {
        std::cout << q << " ";
    }
    std::cout << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output);
    std::cout << "Output: " << output << std::endl << std::endl;
}
