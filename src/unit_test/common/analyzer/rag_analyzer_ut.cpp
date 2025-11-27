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
        analyzer_->Load();

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
    std::string rag_tokenizer_path_ = "python/rag_tokenizer";
    std::string input_file_ = rag_tokenizer_path_ + "/tokenizer_input.txt";

    // Mismatch tokens for: is, data, CCS, experiencing, faster, LLMs, largest
    std::unordered_set<std::string> mismatch_tokens_ = {"be", "datum", "ccs", "experi", "fast", "llms", "larg"};
};

TEST_F(RAGAnalyzerTest, test_analyze_enable_position) {
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
        bool is_bad_token = false;
        if (is_size_match) {
            for (size_t i = 0; i < tokenize_result.size(); ++i) {
                if (tokenize_result[i] != python_tokenize_result[i]) {
                    is_bad_token = mismatch_tokens_.contains(tokenize_result[i]);
                    if (!is_bad_token) {
                        is_match = false;
                        break;
                    }
                }
            }
            EXPECT_TRUE(is_match);
        }
        if (!is_size_match || !is_match || is_bad_token) {
            std::cout << "Tokenize count: " << tokenize_result.size() << ", Python tokenize count: " << python_tokenize_result.size() << std::endl;

            std::cout << "Python tokenize result: " << std::endl << python_tokens << std::endl;
        }
    }
    infile.close();
}
