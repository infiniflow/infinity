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

    std::string CallPythonTokenizer(const std::string &text, bool fineGrained = false) {
        std::string rag_tokenizer_path = std::filesystem::current_path().parent_path().parent_path() / "rag_tokenizer";
        std::string command = "uv run " + rag_tokenizer_path + "/rag_tokenizer.py " + "\"" + text + "\"";

        if (fineGrained) {
            command += " --fine-grained";
        }

        char buffer[128];
        std::string result = "";

        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe) {
            throw std::runtime_error("popen failed");
        }

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }

        pclose(pipe);

        if (!result.empty() && result[result.length() - 1] == '\n') {
            result.erase(result.length() - 1);
        }

        return result;
    }

    RAGAnalyzer *analyzer_;
    std::string rag_tokenizer_path_ = std::filesystem::current_path().parent_path().parent_path() / "rag_tokenizer";
    std::string input_file_ = rag_tokenizer_path_ + "/tokenizer_input.txt";
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
    std::ifstream infile(input_file_);
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty())
            continue;

        std::string tokens = analyzer_->Tokenize(line);

        std::cout << "Input text: " << std::endl << line << std::endl;
        std::cout << "Tokenize result: " << std::endl << tokens << std::endl;

        std::string python_tokens = CallPythonTokenizer(line, false);

        bool is_match = (tokens == python_tokens);
        EXPECT_TRUE(is_match) << "Python tokenize result: " << std::endl << python_tokens << std::endl;
    }
    infile.close();
}
