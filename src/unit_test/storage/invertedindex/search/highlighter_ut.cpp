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
import :status;

using namespace infinity;

class HighlighterTest : public BaseTest {
public:
    void SetUp() override {
        BaseTest::SetUp();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif

        fs::path RESOURCE_DIR = "/usr/share/infinity/resource";
        if (!fs::exists(RESOURCE_DIR)) {
            FAIL() << "Resource directory doesn't exist: " << RESOURCE_DIR;
        }

        analyzer_ = new RAGAnalyzer(RESOURCE_DIR.string());
        auto status = analyzer_->Load();
        if (!status.ok()) {
            FAIL() << "Failed to load RAGAnalyzer for HighlighterTest: " << status.message();
            delete analyzer_;
            analyzer_ = nullptr;
            return;
        }

        analyzer_->SetEnablePosition(true);
        analyzer_->SetFineGrained(false);
    }

    RAGAnalyzer *analyzer_;
};

TEST_F(HighlighterTest, test1) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string raw_text = R"##(
once upon a time there lives an dog.this is a sentence,That is another.你好 世界 dog 你好dog

真的好么？
    )##";
    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;

    std::string query = "dog 你好";
    std::cout << "Query: " << query << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output, analyzer_);
    std::cout << "Output: " << output << std::endl;
}

TEST_F(HighlighterTest, test2) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string raw_text = R"##(The runner is running and jumped over jumps happily
    )##";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;

    std::string query("run jump happy");
    std::cout << "Query: " << query << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output, analyzer_);
    std::cout << "Output: " << output << std::endl;
}

TEST_F(HighlighterTest, test3) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string raw_text =
        R"##({{Redirect|Anarchist|the fictional character|Anarchist (comics)}} {{Redirect|Anarchists}} {{Anarchism sidebar}} {{Libertarianism sidebar}}  '''Anarchism''' is generally defined as the [[political philosophy]] which holds the [[state (polity)|state]] to be undesirable, unnecessary, and harmful,<ref name="definition"> {{Cite journal|last=Malatesta|first=Errico|title=Towards Anarchism|journal=MAN!|publisher=International Group of San Francisco|location=Los Angeles|oclc=3930443|url=http://www.marxists.org/archive/malatesta/1930s/xx/toanarchy.htm|authorlink=Errico Malatesta}} {{Cite journal|url=http://www.theglobeandmail.com/servlet/story/RTGAM.20070514.wxlanarchist14/BNStory/lifeWork/home/ |title=Working for The Man |journal=[[The Globe and Mail]] |accessdate=2008-04-14 |last=Agrell |first=Siri |date=2007-05-14}} {{cite web|url=http://www.britannica.com/eb/article-9117285|title=Anarchism|year=2006|work=Encyclopædia Britannica|publisher=Encyclopædia Britannica Premium Service|accessdate=2006-08-29| archiveurl=)##";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;

    std::string query("harmful chemical anarchism Premium");
    std::cout << "Query: " << query << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output, analyzer_);
    std::cout << "Output: " << output << std::endl << std::endl;
}

TEST_F(HighlighterTest, test4) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string raw_text = "南京的南京长江大桥是中国的一条重要的交通. 南京长江大桥很壮观";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;

    std::string query("南京长江大桥");
    std::cout << "Query: " << query << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output, analyzer_);
    std::cout << "Output: " << output << std::endl << std::endl;
}

// A document stored decomposed (NFD) is highlighted over its own bytes when the query
// term is NFC, which is what the analyzer emits for any query.
TEST_F(HighlighterTest, nfd_document_nfc_query) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string output;
    Highlighter::instance().GetHighlight("ti\u1ebfng", "tie\u0302\u0301ng vie\u0323\u0302t", output, analyzer_);
    EXPECT_EQ(output, "<em>tie\u0302\u0301ng</em> vie\u0323\u0302t");
}

// An NFD query goes through the analyzer first, so it reaches the highlighter as NFC and
// still finds the NFD document.
TEST_F(HighlighterTest, nfd_document_nfd_query) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string output;
    Highlighter::instance().GetHighlight("vie\u0323\u0302t", "tie\u0302\u0301ng vie\u0323\u0302t", output, analyzer_);
    EXPECT_EQ(output, "tie\u0302\u0301ng <em>vie\u0323\u0302t</em>");
}

// Two decomposed words with an ASCII-only run between them are both highlighted over
// their own bytes.
TEST_F(HighlighterTest, nfd_document_two_words) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string output;
    Highlighter::instance().GetHighlight("ti\u1ebfng vi\u1ec7t", "tie\u0302\u0301ng and vie\u0323\u0302t", output, analyzer_);
    EXPECT_EQ(output, "<em>tie\u0302\u0301ng</em> and <em>vie\u0323\u0302t</em>");
}

// A document that is already NFC highlights exactly as before.
TEST_F(HighlighterTest, nfc_document_unchanged) {
    if (!analyzer_)
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    std::string output;
    Highlighter::instance().GetHighlight("ti\u1ebfng", "ti\u1ebfng vi\u1ec7t", output, analyzer_);
    EXPECT_EQ(output, "<em>ti\u1ebfng</em> vi\u1ec7t");
}

// Only the RAG analyzer composes its query terms, so another analyzer highlights a decomposed (NFD)
// document as before NFC: its NFC query terms do not occur in the document's bytes.
TEST_F(HighlighterTest, nfd_document_with_other_analyzer_unchanged) {
    auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer("ngram-3");
    ASSERT_TRUE(status.ok()) << status.message();
    const std::string raw_text = "tie\u0302\u0301ng vie\u0323\u0302t";
    std::string output;
    Highlighter::instance().GetHighlight("ti\u1ebfng", raw_text, output, analyzer.get());
    EXPECT_EQ(output, raw_text);
}
