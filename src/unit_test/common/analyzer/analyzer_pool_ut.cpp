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

module infinity_core:ut.analyzer_pool;

import :ut.base_test;
import :term;
import :analyzer;
import :analyzer_pool;
import :rag_analyzer;
import :status;

using namespace infinity;

namespace fs = std::filesystem;

// Covers AnalyzerPool::GetAnalyzer() name parsing for the RAG analyzer:
// "rag" optionally followed by '-'-separated segments that are either "fine"
// or a language name, in any order. The RAGAnalyzer behaviour itself is
// covered by RAGAnalyzerTest; here we only assert that the name string is
// translated into the right SetLanguage() / SetFineGrained() calls.
class AnalyzerPoolTest : public BaseTest {
public:
    void SetUp() override {
        BaseTest::SetUp();
        fs::path resource_dir = "/usr/share/infinity/resource";
        resource_available_ = fs::exists(resource_dir);
        if (!resource_available_) {
            std::cerr << "Resource directory doesn't exist: " << resource_dir << std::endl;
        }
    }

    // Fetches a RAG analyzer from the pool, or nullptr when the resource
    // directory is missing (the analyzer dictionaries cannot be loaded).
    std::unique_ptr<RAGAnalyzer> GetRAGAnalyzer(const std::string &name) {
        auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer(name);
        EXPECT_TRUE(status.ok()) << "GetAnalyzer(\"" << name << "\") failed: " << status.message();
        if (!status.ok() || analyzer == nullptr) {
            return nullptr;
        }
        auto *rag = dynamic_cast<RAGAnalyzer *>(analyzer.get());
        EXPECT_NE(rag, nullptr) << "GetAnalyzer(\"" << name << "\") did not return a RAGAnalyzer";
        if (rag == nullptr) {
            return nullptr;
        }
        analyzer.release();
        return std::unique_ptr<RAGAnalyzer>(rag);
    }

    // Analyze() is the only place fine-grained tokenization is observable:
    // Tokenize() is always coarse.
    static std::vector<std::string> AnalyzeToTokens(RAGAnalyzer &analyzer, const std::string &text) {
        TermList term_list;
        analyzer.Analyze(text, term_list);
        std::vector<std::string> tokens;
        tokens.reserve(term_list.size());
        for (const auto &term : term_list) {
            tokens.push_back(term.text_);
        }
        return tokens;
    }

    bool resource_available_{false};

    // Coarse tokenization keeps this as one token, fine-grained tokenization
    // splits it into "中华" + "人民共和国".
    static constexpr const char *GRAIN_PROBE = "中华人民共和国";
};

TEST_F(AnalyzerPoolTest, test_rag_default_is_english_and_coarse) {
    if (!resource_available_) {
        FAIL() << "Resource directory not available, skipping test";
    }
    auto analyzer = GetRAGAnalyzer("rag");
    ASSERT_NE(analyzer, nullptr);

    // English stemming is on by default.
    EXPECT_EQ(analyzer->Tokenize("running"), "run");
    // Diacritics are not folded for the default language.
    EXPECT_NE(analyzer->Tokenize("škola").find("kola"), std::string::npos);
    EXPECT_EQ(analyzer->Tokenize("škola").find("skola"), std::string::npos);
    // Coarse grain.
    EXPECT_EQ(AnalyzeToTokens(*analyzer, GRAIN_PROBE), (std::vector<std::string>{"中华人民共和国"}));
}

TEST_F(AnalyzerPoolTest, test_rag_fine_keeps_english_stemming) {
    if (!resource_available_) {
        FAIL() << "Resource directory not available, skipping test";
    }
    auto analyzer = GetRAGAnalyzer("rag-fine");
    ASSERT_NE(analyzer, nullptr);

    EXPECT_EQ(analyzer->Tokenize("running"), "run");
    EXPECT_EQ(AnalyzeToTokens(*analyzer, GRAIN_PROBE), (std::vector<std::string>{"中华", "人民共和国"}));
}

TEST_F(AnalyzerPoolTest, test_rag_slovak_folds_and_disables_stemming) {
    if (!resource_available_) {
        FAIL() << "Resource directory not available, skipping test";
    }
    auto analyzer = GetRAGAnalyzer("rag-slovak");
    ASSERT_NE(analyzer, nullptr);

    EXPECT_EQ(analyzer->Tokenize("škola"), "skola");
    EXPECT_EQ(analyzer->Tokenize("daňové priznanie"), "danove priznanie");
    // No stemming for Slovak.
    EXPECT_EQ(analyzer->Tokenize("running"), "running");
    // Still coarse without a "fine" segment.
    EXPECT_EQ(AnalyzeToTokens(*analyzer, GRAIN_PROBE), (std::vector<std::string>{"中华人民共和国"}));
}

TEST_F(AnalyzerPoolTest, test_rag_czech_folds_and_disables_stemming) {
    if (!resource_available_) {
        FAIL() << "Resource directory not available, skipping test";
    }
    auto analyzer = GetRAGAnalyzer("rag-czech");
    ASSERT_NE(analyzer, nullptr);

    EXPECT_EQ(analyzer->Tokenize("škola"), "skola");
    EXPECT_EQ(analyzer->Tokenize("příliš žluťoučký kůň"), "prilis zlutoucky kun");
    EXPECT_EQ(analyzer->Tokenize("running"), "running");
    EXPECT_EQ(AnalyzeToTokens(*analyzer, GRAIN_PROBE), (std::vector<std::string>{"中华人民共和国"}));
}

TEST_F(AnalyzerPoolTest, test_rag_language_and_fine_in_both_orders) {
    if (!resource_available_) {
        FAIL() << "Resource directory not available, skipping test";
    }
    const std::vector<std::string> expected{"skola", "中华", "人民共和国"};
    for (const std::string &name : {"rag-czech-fine", "rag-fine-czech", "rag-slovak-fine", "rag-fine-slovak"}) {
        auto analyzer = GetRAGAnalyzer(name);
        ASSERT_NE(analyzer, nullptr) << name;
        EXPECT_EQ(analyzer->Tokenize("škola"), "skola") << name;
        EXPECT_EQ(analyzer->Tokenize("running"), "running") << name;
        EXPECT_EQ(AnalyzeToTokens(*analyzer, std::string("škola ") + GRAIN_PROBE), expected) << name;
    }
}

TEST_F(AnalyzerPoolTest, test_rag_unknown_segment_falls_back_to_legacy) {
    if (!resource_available_) {
        FAIL() << "Resource directory not available, skipping test";
    }
    // Unknown languages, an empty segment and a stray trailing '-' must not
    // crash and must leave the legacy English/coarse behaviour in place.
    for (const std::string &name : {"rag-", "rag--", "rag-slovakk", "rag-klingon", "rag-nosuchlang"}) {
        auto analyzer = GetRAGAnalyzer(name);
        ASSERT_NE(analyzer, nullptr) << name;
        EXPECT_EQ(analyzer->Tokenize("running"), "run") << name;
        EXPECT_EQ(analyzer->Tokenize("škola").find("skola"), std::string::npos) << name;
        EXPECT_EQ(AnalyzeToTokens(*analyzer, GRAIN_PROBE), (std::vector<std::string>{"中华人民共和国"})) << name;
    }

    // An unknown segment next to "fine" still selects fine-grained.
    auto analyzer = GetRAGAnalyzer("rag-klingon-fine");
    ASSERT_NE(analyzer, nullptr);
    EXPECT_EQ(analyzer->Tokenize("running"), "run");
    EXPECT_EQ(AnalyzeToTokens(*analyzer, GRAIN_PROBE), (std::vector<std::string>{"中华", "人民共和国"}));
}

TEST_F(AnalyzerPoolTest, test_rag_analyzers_are_independent) {
    if (!resource_available_) {
        FAIL() << "Resource directory not available, skipping test";
    }
    // Every GetAnalyzer() call copies the cached prototype; a language-specific
    // instance must not leak its settings into the next one handed out.
    auto slovak = GetRAGAnalyzer("rag-slovak");
    ASSERT_NE(slovak, nullptr);
    EXPECT_EQ(slovak->Tokenize("škola"), "skola");

    auto english = GetRAGAnalyzer("rag");
    ASSERT_NE(english, nullptr);
    EXPECT_EQ(english->Tokenize("running"), "run");
    EXPECT_EQ(english->Tokenize("škola").find("skola"), std::string::npos);

    // The Slovak instance is unaffected by the later English one.
    EXPECT_EQ(slovak->Tokenize("daňové"), "danove");
}
