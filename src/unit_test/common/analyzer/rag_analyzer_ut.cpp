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

module infinity_core:ut.rag_analyzer;

import :ut.base_test;
import :term;
import :analyzer;
import :rag_analyzer;
import :darts_trie;
import :darts;
import :status;

using namespace infinity;

namespace fs = std::filesystem;

class RAGAnalyzerTest : public BaseTest {
public:
    void SetUp() override {
        BaseTest::SetUp();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif

        fs::path RESOURCE_DIR = "/usr/share/infinity/resource";
        if (!fs::exists(RESOURCE_DIR)) {
            std::cerr << "Resource directory doesn't exist: " << RESOURCE_DIR << std::endl;
            return;
        }

        analyzer_ = new RAGAnalyzer(RESOURCE_DIR.string());
        auto status = analyzer_->Load();
        if (!status.ok()) {
            std::cerr << "Failed to load RAGAnalyzer: " << status.message() << std::endl;
            delete analyzer_;
            analyzer_ = nullptr;
            return;
        }

        analyzer_->SetEnablePosition(false);
        analyzer_->SetFineGrained(false);
    }

    void TearDown() override {
        delete analyzer_;
        analyzer_ = nullptr;
    }

    std::vector<std::string> SplitString(const std::string &str) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;

        while (ss >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

    RAGAnalyzer *analyzer_;
    std::string rag_tokenizer_path_ = "python/infinity_sdk/infinity";
    std::string input_file_ = rag_tokenizer_path_ + "/tokenizer_input.txt";
};

TEST_F(RAGAnalyzerTest, test_analyze_enable_position) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    analyzer_->SetEnablePosition(true);
    analyzer_->SetFineGrained(false);

    std::ifstream infile(input_file_);
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty())
            continue;

        TermList term_list;
        analyzer_->Analyze(line, term_list);
        std::cout << "Input text: " << std::endl << line << std::endl;

        std::cout << "Analyze result: " << std::endl;
        for (unsigned i = 0; i < term_list.size(); ++i) {
            std::cout << "[" << term_list[i].text_ << "@" << term_list[i].word_offset_ << "," << term_list[i].end_offset_ << "] ";
        }
        std::cout << std::endl;
    }
    infile.close();
}

TEST_F(RAGAnalyzerTest, test_analyze_enable_position_fine_grained) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    analyzer_->SetEnablePosition(true);
    analyzer_->SetFineGrained(true);

    std::ifstream infile(input_file_);
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty())
            continue;

        TermList term_list;
        analyzer_->Analyze(line, term_list);
        std::cout << "Input text: " << std::endl << line << std::endl;

        std::cout << "Analyze result: " << std::endl;
        for (unsigned i = 0; i < term_list.size(); ++i) {
            std::cout << "[" << term_list[i].text_ << "@" << term_list[i].word_offset_ << "," << term_list[i].end_offset_ << "] ";
        }
        std::cout << std::endl;
    }
    infile.close();
}

// Test Tokenize vs TokenizeWithPosition consistency
TEST_F(RAGAnalyzerTest, test_tokenize_consistency_with_posiiton) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    std::ifstream infile(input_file_);
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty())
            continue;

        // Test Tokenize (returns string)
        std::string tokens_str = analyzer_->Tokenize(line);
        std::istringstream iss(tokens_str);
        std::string token;
        std::vector<std::string> tokenize_result;
        while (iss >> token) {
            tokenize_result.push_back(token);
        }

        std::cout << "Input text: " << std::endl << line << std::endl;
        std::cout << "Tokenize result: " << std::endl << tokens_str << std::endl;

        // Test TokenizeWithPosition (returns vector of tokens and positions)
        auto [tokenize_with_pos_result, positions] = analyzer_->TokenizeWithPosition(line);

        // Check if results are identical
        bool tokens_match = (tokenize_result.size() == tokenize_with_pos_result.size());
        if (tokens_match) {
            for (size_t i = 0; i < tokenize_result.size(); ++i) {
                if (tokenize_result[i] != tokenize_with_pos_result[i]) {
                    tokens_match = false;
                    break;
                }
            }
        }

        EXPECT_TRUE(tokens_match);
        if (!tokens_match) {
            std::cout << "Tokenize count: " << tokenize_result.size() << ", TokenizeWithPosition count: " << tokenize_with_pos_result.size()
                      << std::endl;

            std::cout << "TokenizeWithPosition result: " << std::endl;
            std::string result_str = std::accumulate(tokenize_with_pos_result.begin(),
                                                     tokenize_with_pos_result.end(),
                                                     std::string(""),
                                                     [](const std::string &a, const std::string &b) { return a + (a.empty() ? "" : " ") + b; });
            std::cout << result_str << std::endl;
        }
    }
    infile.close();
}

TEST_F(RAGAnalyzerTest, test_tokenize_consistency_with_python) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    std::string call_python_tokenizer_command = "uv run " + rag_tokenizer_path_ + "/rag_tokenizer.py " + "-f \"" + input_file_ + "\" -o \"" +
                                                rag_tokenizer_path_ + "/tokenizer_python_output.txt\"";

    std::cout << "Call Python tokenizer: " << std::endl << call_python_tokenizer_command << std::endl;

    int result = system(call_python_tokenizer_command.c_str());
    EXPECT_EQ(result, 0) << "Call Python tokenizer failed.";

    if (result != 0) {
        return;
    }

    std::ifstream infile(input_file_);
    std::ifstream infile_python(rag_tokenizer_path_ + "/tokenizer_python_output.txt");
    std::string line;
    std::string python_tokens;
    while (std::getline(infile, line)) {
        if (line.empty())
            continue;

        std::string tokens = analyzer_->Tokenize(line);
        std::cout << "Input text: " << std::endl << line << std::endl;
        std::cout << "Tokenize result: " << std::endl << tokens << std::endl;

        std::getline(infile_python, python_tokens);

        std::vector<std::string> tokenize_result = SplitString(tokens);
        std::vector<std::string> python_tokenize_result = SplitString(python_tokens);

        bool is_size_match = tokenize_result.size() == python_tokenize_result.size();
        EXPECT_TRUE(is_size_match);

        bool is_match = true;
        if (is_size_match) {
            for (size_t i = 0; i < tokenize_result.size(); ++i) {
                if (tokenize_result[i] != python_tokenize_result[i]) {
                    std::cout << "MISMATCH at " << i << ": C++='" << tokenize_result[i] << "' Python='" << python_tokenize_result[i] << "'"
                              << std::endl;
                    is_match = false;
                    break;
                }
            }
            EXPECT_TRUE(is_match);
        }
        if (!is_size_match || !is_match) {
            std::cout << "Tokenize count: " << tokenize_result.size() << ", Python tokenize count: " << python_tokenize_result.size() << std::endl;

            std::cout << "Python tokenize result: " << std::endl << python_tokens << std::endl;
        }
    }
    infile.close();
}

TEST_F(RAGAnalyzerTest, test_fine_grained_tokenize_consistency_with_python) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    std::string call_python_tokenizer_command = "uv run " + rag_tokenizer_path_ + "/rag_tokenizer.py " + "-f \"" + input_file_ + "\" -o \"" +
                                                rag_tokenizer_path_ + "/fine_grained_tokenizer_python_output.txt\"" + " --fine-grained";

    std::cout << "Call Python tokenizer: " << std::endl << call_python_tokenizer_command << std::endl;

    int result = system(call_python_tokenizer_command.c_str());
    EXPECT_EQ(result, 0) << "Call Python tokenizer failed.";

    if (result != 0) {
        return;
    }

    analyzer_->SetEnablePosition(false);
    analyzer_->SetFineGrained(true);

    std::ifstream infile(input_file_);
    std::ifstream infile_python(rag_tokenizer_path_ + "/fine_grained_tokenizer_python_output.txt");
    std::string line;
    std::string python_tokens;
    while (std::getline(infile, line)) {
        if (line.empty())
            continue;

        std::string coarse = analyzer_->Tokenize(line);
        std::vector<std::string> term_list;
        analyzer_->FineGrainedTokenize(coarse, term_list);

        std::string fine_grained_tokens =
            std::accumulate(term_list.begin(), term_list.end(), std::string(""), [](const std::string &a, const std::string &b) {
                return a + (a.empty() ? "" : " ") + b;
            });

        std::cout << "Input text: " << std::endl << line << std::endl;
        std::cout << "Fine grained tokenize result: " << std::endl << fine_grained_tokens << std::endl;

        std::getline(infile_python, python_tokens);
        std::vector<std::string> python_tokenize_result = SplitString(python_tokens);

        bool is_size_match = term_list.size() == python_tokenize_result.size();
        EXPECT_TRUE(is_size_match);

        bool is_match = true;
        if (is_size_match) {
            for (size_t i = 0; i < term_list.size(); ++i) {
                if (term_list[i] != python_tokenize_result[i]) {
                    std::cout << "MISMATCH at " << i << ": C++='" << term_list[i] << "' Python='" << python_tokenize_result[i] << "'" << std::endl;
                    is_match = false;
                    break;
                }
            }
            EXPECT_TRUE(is_match);
        }
        if (!is_size_match || !is_match) {
            std::cout << "Tokenize count: " << term_list.size() << ", Python tokenize count: " << python_tokenize_result.size() << std::endl;

            std::cout << "Python tokenize result: " << std::endl << python_tokens << std::endl;
        }
    }
    infile.close();
}

TEST_F(RAGAnalyzerTest, test_set_language_dutch) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Dutch word "huizen" (houses) should stem to "huiz" with Dutch stemmer
    // Compare C++ result with Python result
    std::string python_cmd = "uv run " + rag_tokenizer_path_ + "/rag_tokenizer.py " + "-l dutch \"huizen\"";
    std::cout << "Call Python tokenizer: " << python_cmd << std::endl;

    FILE *pipe = popen(python_cmd.c_str(), "r");
    std::string python_result;
    char buffer[128];
    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            python_result += buffer;
        }
        pclose(pipe);
    }
    // Remove trailing newline
    python_result.erase(python_result.find_last_not_of(" \n\r\t") + 1);
    std::cout << "Python 'huizen' tokenized (Dutch): " << python_result << std::endl;

    analyzer_->SetLanguage("dutch");
    std::string cxx_result = analyzer_->Tokenize("huizen");
    std::cout << "C++ 'huizen' tokenized (Dutch): " << cxx_result << std::endl;

    EXPECT_TRUE(cxx_result.find("huiz") != std::string::npos);
    EXPECT_EQ(cxx_result, python_result);
}

TEST_F(RAGAnalyzerTest, test_set_language_slovak) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Slovak has no Snowball stemmer; diacritics are folded to ASCII instead.
    // Compare C++ result with Python result.
    std::string python_cmd = "uv run " + rag_tokenizer_path_ + "/rag_tokenizer.py " + "-l slovak \"škola daňové priznanie\"";
    std::cout << "Call Python tokenizer: " << python_cmd << std::endl;

    FILE *pipe = popen(python_cmd.c_str(), "r");
    std::string python_result;
    char buffer[128];
    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            python_result += buffer;
        }
        pclose(pipe);
    }
    // Remove trailing newline
    python_result.erase(python_result.find_last_not_of(" \n\r\t") + 1);
    std::cout << "Python 'škola daňové priznanie' tokenized (Slovak): " << python_result << std::endl;

    analyzer_->SetLanguage("slovak");
    std::string cxx_result = analyzer_->Tokenize("škola daňové priznanie");
    std::cout << "C++ 'škola daňové priznanie' tokenized (Slovak): " << cxx_result << std::endl;

    EXPECT_TRUE(cxx_result.find("skola") != std::string::npos);
    EXPECT_TRUE(cxx_result.find("danove") != std::string::npos);
    EXPECT_EQ(cxx_result, python_result);
}

TEST_F(RAGAnalyzerTest, test_set_language_czech) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    std::string python_cmd = "uv run " + rag_tokenizer_path_ + "/rag_tokenizer.py " + "-l czech \"příliš žluťoučký kůň\"";
    std::cout << "Call Python tokenizer: " << python_cmd << std::endl;

    FILE *pipe = popen(python_cmd.c_str(), "r");
    std::string python_result;
    char buffer[128];
    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            python_result += buffer;
        }
        pclose(pipe);
    }
    // Remove trailing newline
    python_result.erase(python_result.find_last_not_of(" \n\r\t") + 1);
    std::cout << "Python 'příliš žluťoučký kůň' tokenized (Czech): " << python_result << std::endl;

    analyzer_->SetLanguage("czech");
    std::string cxx_result = analyzer_->Tokenize("příliš žluťoučký kůň");
    std::cout << "C++ 'příliš žluťoučký kůň' tokenized (Czech): " << cxx_result << std::endl;

    EXPECT_TRUE(cxx_result.find("prilis") != std::string::npos);
    EXPECT_TRUE(cxx_result.find("zlutoucky") != std::string::npos);
    EXPECT_TRUE(cxx_result.find("kun") != std::string::npos);
    EXPECT_EQ(cxx_result, python_result);
}

TEST_F(RAGAnalyzerTest, test_slovak_no_stemming) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Slovak disables stemming and lemmatization: "running" must pass
    // through unstemmed and "skoly" must keep its inflected form.
    analyzer_->SetLanguage("slovak");
    EXPECT_EQ(analyzer_->Tokenize("skoly running"), "skoly running");

    // Switching back to a Snowball language re-enables stemming.
    analyzer_->SetLanguage("english");
    EXPECT_EQ(analyzer_->Tokenize("running"), "run");
}

TEST_F(RAGAnalyzerTest, test_diacritics_folding_table) {
    // All Slovak letters fold to their ASCII bases, case preserved.
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("áäčďéíĺľňóôŕšťúýž"), "aacdeillnoorstuyz");
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("ÁÄČĎÉÍĹĽŇÓÔŔŠŤÚÝŽ"), "AACDEILLNOORSTUYZ");
    // Czech letters not in the Slovak alphabet.
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("ěřůý ĚŘŮÝ"), "eruy ERUY");
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("Škola Daňové"), "Skola Danove");
    // Letters without a single-ASCII-letter NFD decomposition pass through.
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("æœßøłđÆŒØŁĐ"), "æœßøłđÆŒØŁĐ");
    // ASCII and non-Latin text is untouched.
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("hello world 123"), "hello world 123");
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("中文测试"), "中文测试");
}

TEST_F(RAGAnalyzerTest, test_slovak_tokenize_with_position) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Folding shrinks 2-byte letters to 1 byte; positions must still refer
    // to byte offsets in the original input ("š" and "ň"/"é" are 2 bytes).
    analyzer_->SetLanguage("slovak");
    auto [tokens, positions] = analyzer_->TokenizeWithPosition("škola daňové");
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0], "skola");
    EXPECT_EQ(tokens[1], "danove");
    ASSERT_EQ(positions.size(), 2u);
    EXPECT_EQ(positions[0].first, 0u);
    // "škola" spans bytes [0, 6) of the original input.
    EXPECT_EQ(positions[0].second, 6u);
    // "daňové" spans bytes [7, 15) of the original input.
    EXPECT_EQ(positions[1].first, 7u);
    EXPECT_EQ(positions[1].second, 15u);
}

TEST_F(RAGAnalyzerTest, test_chinese_stopword_filter) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }

    // IsStopword is a static predicate over the canonical set.
    EXPECT_TRUE(RAGAnalyzer::IsStopword("的"));
    EXPECT_TRUE(RAGAnalyzer::IsStopword("是"));
    EXPECT_TRUE(RAGAnalyzer::IsStopword("了"));
    EXPECT_TRUE(RAGAnalyzer::IsStopword("什么"));
    EXPECT_TRUE(RAGAnalyzer::IsStopword("怎么"));
    EXPECT_FALSE(RAGAnalyzer::IsStopword("贡献"));
    EXPECT_FALSE(RAGAnalyzer::IsStopword("最大"));
    EXPECT_FALSE(RAGAnalyzer::IsStopword("谁"));

    const std::string query = "谁的贡献最大";

    // Path A: position disabled.
    analyzer_->SetEnablePosition(false);
    analyzer_->SetFineGrained(false);
    {
        TermList terms;
        analyzer_->Analyze(query, terms);
        std::cout << "[no-pos] ";
        for (const auto &t : terms)
            std::cout << "[" << t.text_ << "] ";
        std::cout << std::endl;
        for (const auto &t : terms) {
            EXPECT_NE(t.text_, "的") << "Analyzer must drop 的 from token stream";
        }
    }

    // Path B: position enabled (highlighter path).
    analyzer_->SetEnablePosition(true);
    {
        TermList terms;
        analyzer_->Analyze(query, terms);
        std::cout << "[with-pos] ";
        for (const auto &t : terms)
            std::cout << "[" << t.text_ << "@" << t.word_offset_ << "," << t.end_offset_ << "] ";
        std::cout << std::endl;
        for (const auto &t : terms) {
            EXPECT_NE(t.text_, "的") << "Position-enabled path must also drop 的";
        }
    }

    // Path C: the raw Tokenize() string-level API is unaffected by the filter —
    // stopword filtering lives at the AnalyzeImpl boundary so direct tokenizer
    // callers (and unit tests of the tokenizer itself) see the unfiltered stream.
    analyzer_->SetEnablePosition(false);
    std::string raw = analyzer_->Tokenize(query);
    std::cout << "[raw Tokenize] " << raw << std::endl;
    EXPECT_NE(raw.find("的"), std::string::npos) << "Raw Tokenize() must still emit 的 (filter is only at AnalyzeImpl boundary)";
}