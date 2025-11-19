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

class HighlighterTest : public BaseTest {
public:
    void SetUp() override {
        BaseTest::SetUp();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif

        // Get the path to the executable using the /proc/self/exe symlink
        fs::path executablePath = "/proc/self/exe";
        std::error_code ec;
        // Resolve the symlink to get the actual path
        executablePath = fs::canonical(executablePath, ec);
        if (ec) {
            std::cerr << "Error resolving the path: " << executablePath << " " << ec.message() << std::endl;
            return;
        }

        fs::path ROOT_PATH = executablePath.parent_path().parent_path().parent_path().parent_path() / "resource";
        if (!fs::exists(ROOT_PATH)) {
            std::cerr << "Resource directory doesn't exist: " << ROOT_PATH << std::endl;
            return;
        }

        analyzer_ = new RAGAnalyzer(ROOT_PATH.string());
        analyzer_->Load();

        analyzer_->SetEnablePosition(true);
        analyzer_->SetFineGrained(false);
    }

    RAGAnalyzer *analyzer_;
};

TEST_F(HighlighterTest, test1) {
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
    std::string raw_text =
        R"##({{Redirect|Anarchist|the fictional character|Anarchist (comics)}} {{Redirect|Anarchists}} {{Anarchism sidebar}} {{Libertarianism sidebar}}  '''Anarchism''' is generally defined as the [[political philosophy]] which holds the [[state (polity)|state]] to be undesirable, unnecessary, and harmful,<ref name="definition"> {{Cite journal|last=Malatesta|first=Errico|title=Towards Anarchism|journal=MAN!|publisher=International Group of San Francisco|location=Los Angeles|oclc=3930443|url=http://www.marxists.org/archive/malatesta/1930s/xx/toanarchy.htm|authorlink=Errico Malatesta}} {{Cite journal|url=http://www.theglobeandmail.com/servlet/story/RTGAM.20070514.wxlanarchist14/BNStory/lifeWork/home/ |title=Working for The Man |journal=[[The Globe and Mail]] |accessdate=2008-04-14 |last=Agrell |first=Siri |date=2007-05-14}} {{cite web|url=http://www.britannica.com/eb/article-9117285|title=Anarchism|year=2006|work=Encyclopædia Britannica|publisher=Encyclopædia Britannica Premium Service|accessdate=2006-08-29| archiveurl=)##";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;

    std::string query("harmful chemical anarchism");
    std::cout << "Query: " << query << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output, analyzer_);
    std::cout << "Output: " << output << std::endl << std::endl;
}

TEST_F(HighlighterTest, test4) {
    std::string raw_text = "南京的南京长江大桥是中国的一条重要的交通";

    std::cout << "-----------------" << std::endl;
    std::cout << "Raw_text: " << raw_text << std::endl;

    std::string query("南京长江大桥");
    std::cout << "Query: " << query << std::endl;

    std::string output;
    Highlighter::instance().GetHighlight(query, raw_text, output, analyzer_);
    std::cout << "Output: " << output << std::endl << std::endl;
}
