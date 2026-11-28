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

#include "common/analyzer/unicode_nfc.h"
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

    // Runs rag_tokenizer.py on one text and returns its output line.
    std::string PythonTokenize(const std::string &text, const std::string &options = "") {
        std::string python_cmd = "uv run " + rag_tokenizer_path_ + "/rag_tokenizer.py " + options + " \"" + text + "\"";
        FILE *pipe = popen(python_cmd.c_str(), "r");
        std::string python_result;
        char buffer[128];
        if (pipe) {
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                python_result += buffer;
            }
            pclose(pipe);
        }
        python_result.erase(python_result.find_last_not_of(" \n\r\t") + 1);
        return python_result;
    }

    RAGAnalyzer *analyzer_ = nullptr;
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
    // Folding is deliberately limited to U+00C0-U+017F. Decomposable letters
    // outside that range (Latin Extended Additional, precomposed Hangul, …)
    // are left alone so the table stays in step with rag_tokenizer.py, which
    // applies the same range guard before consulting unicodedata.
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("ḍṣṭḥ ḌỊ"), "ḍṣṭḥ ḌỊ");
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("한국어"), "한국어");
    // Combining marks are not stripped either: an already-decomposed "š"
    // (s + U+030C) keeps both code points on both sides.
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("s\u030Ckola"), "s\u030Ckola");
    // ASCII and non-Latin text is untouched.
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("hello world 123"), "hello world 123");
    EXPECT_EQ(RAGAnalyzer::FoldDiacritics("中文测试"), "中文测试");
}

TEST_F(RAGAnalyzerTest, test_out_of_range_diacritics_parity_with_python) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // The C++ fold table covers U+00C0-U+017F only; rag_tokenizer.py applies
    // the same range guard before NFD. Characters that decompose but sit
    // outside the range (ḍ, ṣṭḥ) and precomposed Hangul must therefore survive
    // identically on both sides - this is what keeps the hard-coded table and
    // the runtime unicodedata lookup from drifting apart.
    const std::string input = "škola ḍamage 한국어 ṣṭḥ";
    std::string python_cmd = "uv run " + rag_tokenizer_path_ + "/rag_tokenizer.py " + "-l slovak \"" + input + "\"";
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
    python_result.erase(python_result.find_last_not_of(" \n\r\t") + 1);
    std::cout << "Python tokenized (Slovak): " << python_result << std::endl;

    analyzer_->SetLanguage("slovak");
    std::string cxx_result = analyzer_->Tokenize(input);
    std::cout << "C++ tokenized (Slovak): " << cxx_result << std::endl;

    // In-range diacritics fold, out-of-range ones are carried through as-is.
    EXPECT_TRUE(cxx_result.find("skola") != std::string::npos);
    EXPECT_TRUE(cxx_result.find("한국어") != std::string::npos);
    EXPECT_TRUE(cxx_result.find("ṣṭḥ") != std::string::npos);
    EXPECT_EQ(cxx_result, python_result);
}

TEST_F(RAGAnalyzerTest, test_latin_diacritics_parity_with_python) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Latin letters with diacritics stay inside their word instead of being
    // split off ("tiếng" used to become "ti ế ng", "café" -> "caf é"). The
    // last three inputs tokenize exactly as before the change.
    const std::pair<std::string, std::string> cases[] = {
        {"tôi đang học tiếng Việt", "tôi đang học tiếng việt"},
        {"bữa sáng ở quán phở", "bữa sáng ở quán phở"},
        {"le café est très bon", "le café est très bon"},
        // Accented words that end in an ASCII letter are not stemmed.
        {"ngày máy này giấy", "ngày máy này giấy"},
        {"vậy thấy chạy đây", "vậy thấy chạy đây"},
        // Capital accented letters are lowercased, as in Python.
        {"Über die Brücke", "über die brücke"},
        {"ĐƯỜNG PHỐ HÀ NỘI", "đường phố hà nội"},
        {"Đà Nẵng PHẢI", "đà nẵng phải"},
        {"ÀÁÂ àáâ ỸỸ", "àáâ àáâ ỹỹ"},
        // Tokens with digits are not stemmed, as in Python.
        {"abc123s win10s 100mbps", "abc123s win10s 100mbps"},
        {"南京市长江大桥 cà phê 测试", "南京市 长江大桥 cà phê 测试"},
        {"The quick brown foxes are running quickly", "the quick brown fox are run quick"},
        {"RAGFlow 支持 English 和 中文 mixed text", "ragflow 支持 english 和 中文 mix text"},
        {"version 1.2.3 costs 4,500", "version 123 cost 4 500"},
    };
    for (const auto &[input, expected] : cases) {
        std::string cxx_result = analyzer_->Tokenize(input);
        EXPECT_EQ(cxx_result, expected);
        EXPECT_EQ(cxx_result, PythonTokenize(input));
    }
}

TEST_F(RAGAnalyzerTest, test_math_signs_in_latin1_unchanged) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // × and ÷ sit inside U+00C0-U+024F but are not letters: they stay out of
    // the word run and tokenize as before the diacritics change.
    const std::pair<std::string, std::string> cases[] = {
        {"2×3", "23"},
        {"a÷b", "ab"},
    };
    for (const auto &[input, expected] : cases) {
        std::string cxx_result = analyzer_->Tokenize(input);
        EXPECT_EQ(cxx_result, expected);
        EXPECT_EQ(cxx_result, PythonTokenize(input));
    }
}

TEST_F(RAGAnalyzerTest, test_large_token_tokenize) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // A token longer than the 12 288-byte lowercase buffer must neither
    // overflow it nor be cut: 13 000 ASCII letters, and 4 334 x "Ầ" (13 002
    // bytes).
    std::string latin_upper;
    std::string latin_lower;
    for (int i = 0; i < 4334; ++i) {
        latin_upper += "Ầ";
        latin_lower += "ầ";
    }
    const std::pair<std::string, std::string> cases[] = {
        {std::string(13000, 'A'), std::string(13000, 'a')},
        {latin_upper, latin_lower},
    };
    for (const auto &[input, expected] : cases) {
        EXPECT_EQ(analyzer_->Tokenize(input), expected);
        EXPECT_EQ(PythonTokenize(input), expected);
        auto [tokens, positions] = analyzer_->TokenizeWithPosition(input);
        ASSERT_EQ(tokens.size(), 1u);
        EXPECT_EQ(tokens[0], expected);
        ASSERT_EQ(positions.size(), 1u);
        EXPECT_EQ(positions[0].first, 0u);
        EXPECT_EQ(positions[0].second, static_cast<unsigned>(input.size()));
    }
    // The fine-grained pass lowercases the ASCII run again in EnglishNormalize.
    const std::string mixed = "中文测试 " + std::string(13000, 'A');
    const std::string expected = "中文 测试 " + std::string(13000, 'a');
    std::vector<std::string> fine;
    analyzer_->FineGrainedTokenize(analyzer_->Tokenize(mixed), fine);
    std::string fine_tokens;
    for (const auto &token : fine) {
        fine_tokens += (fine_tokens.empty() ? "" : " ") + token;
    }
    EXPECT_EQ(fine_tokens, expected);
    EXPECT_EQ(PythonTokenize(mixed, "--fine-grained"), expected);
}

TEST_F(RAGAnalyzerTest, test_latin_uppercase_tokenize_with_position) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Lowercasing an accented capital keeps its UTF-8 length, so the offsets
    // still point into the original input ("Ờ" and "Ố" are 3 bytes, "Đ" and
    // "Ư" are 2).
    auto [tokens, positions] = analyzer_->TokenizeWithPosition("ĐƯỜNG PHỐ");
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0], "đường");
    EXPECT_EQ(tokens[1], "phố");
    ASSERT_EQ(positions.size(), 2u);
    EXPECT_EQ(positions[0].first, 0u);
    EXPECT_EQ(positions[0].second, 9u);
    EXPECT_EQ(positions[1].first, 10u);
    EXPECT_EQ(positions[1].second, 15u);
}

TEST_F(RAGAnalyzerTest, test_lowercase_changes_class_parity_with_python) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // rag_tokenizer.py lowercases before it splits, so a capital whose
    // lowercase is outside the word class splits off its word and one whose
    // lowercase is inside joins it. The split equals Python's; only letters
    // C++ does not lowercase keep their case.
    struct Case {
        std::string input;
        std::string expected;
        std::string python_expected;
    };
    const Case cases[] = {
        {"Ɛwe Ɔkɔ ƁAMA Ɗaka", "Ɛ we Ɔ k ɔ Ɓ ama Ɗ aka", "ɛ we ɔ k ɔ ɓ ama ɗ aka"},
        {"Ⱥb Ⱦc", "Ⱥ b Ⱦ c", "ⱥ b ⱦ c"},
        {"Ȿx Ɀy", "Ȿx Ɀy", "ȿx ɀy"},
    };
    for (const auto &[input, expected, python_expected] : cases) {
        EXPECT_EQ(analyzer_->Tokenize(input), expected);
        EXPECT_EQ(PythonTokenize(input), python_expected);
    }
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

TEST_F(RAGAnalyzerTest, test_nfd_input_parity_with_python) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Decomposed (NFD) input tokenizes like its NFC form, as in
    // rag_tokenizer.py. Without NFC the tokens would keep their combining
    // marks apart from the letters, unlike Python's, and FoldDiacritics and
    // the Latin lowercase table only know composed letters.
    const std::pair<std::string, std::string> cases[] = {
        {"to\u0302i đang ho\u0323c tie\u0302\u0301ng Vie\u0323\u0302t", "tôi đang học tiếng việt"},
        {"Hoa\u0300 gia\u0309i đa\u0303 đu\u031Bo\u031B\u0323c", "hoà giải đã được"},
        // The marks come in either order.
        {"a\u0302\u0323", "ậ"},
        {"a\u0323\u0302", "ậ"},
        {"ĐU\u031BO\u031B\u0300NG PHO\u0302\u0301", "đường phố"},
        // A tone mark after an already composed letter.
        {"ê\u0323 ơ\u0323 Ư\u0300", "ệ ợ ừ"},
        // Hangul conjoining jamo.
        {"\u1112\u1161\u11AB\u1100\u1173\u11AF", "한글"},
    };
    for (const auto &[input, expected] : cases) {
        std::string cxx_result = analyzer_->Tokenize(input);
        EXPECT_EQ(cxx_result, expected);
        EXPECT_EQ(cxx_result, PythonTokenize(input));
    }

    // NFC runs before the Slovak/Czech folding, which only knows composed
    // letters.
    analyzer_->SetLanguage("slovak");
    std::string cxx_result = analyzer_->Tokenize("s\u030Ckola");
    EXPECT_EQ(cxx_result, "skola");
    EXPECT_EQ(cxx_result, PythonTokenize("s\u030Ckola", "-l slovak"));
}

TEST_F(RAGAnalyzerTest, test_protected_code_points_tokenize_as_before) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // NFC leaves the precomposed code points it would replace
    // (Full_Composition_Exclusion) as they are, as rag_tokenizer.py does, so
    // they tokenize as they did before NFC: U+FB2A (shin with shin dot) and
    // U+0958 (qa) are not split into a letter and a mark, and the singletons
    // U+F984, U+F900 and U+212A KELVIN SIGN keep their code points. The tokens
    // match Python's, but for U+212A, which Python lowercases to k and the C++
    // analyzer does not.
    struct Case {
        std::string input;
        std::vector<std::string> tokens;
        std::vector<std::pair<unsigned, unsigned>> positions;
    };
    const Case cases[] = {
        {"\uFB2A\u05DC\u05D5\u05DD", {"\uFB2A\u05DC\u05D5\u05DD"}, {{0, 9}}},
        {"\u0958x", {"\u0958", "x"}, {{0, 3}, {3, 4}}},
        {"\uF984", {"\uF984"}, {{0, 3}}},
        {"中国\uF900人", {"中国", "\uF900", "人"}, {{0, 6}, {6, 9}, {9, 12}}},
        {"a \u212Am", {"a", "\u212Am"}, {{0, 1}, {2, 6}}},
    };
    for (const auto &[input, tokens, positions] : cases) {
        EXPECT_FALSE(NormalizeNFC(input).has_value()) << input;
        std::string expected;
        for (const auto &token : tokens) {
            expected += (expected.empty() ? "" : " ") + token;
        }
        std::string cxx_result = analyzer_->Tokenize(input);
        EXPECT_EQ(cxx_result, expected);
        if (input.find("\u212A") == std::string::npos) {
            EXPECT_EQ(cxx_result, PythonTokenize(input));
        }
        auto [actual_tokens, actual_positions] = analyzer_->TokenizeWithPosition(input);
        EXPECT_EQ(actual_tokens, tokens);
        EXPECT_EQ(actual_positions, positions);
    }
}

TEST_F(RAGAnalyzerTest, test_decomposed_dotted_capital_i_tokenizes_as_before) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // I + U+0307 does not compose to U+0130, which Python lowercases to
    // i + U+0307: Python gives main's tokens. NFC leaves the input as it is, so
    // the C++ tokenizer sees what it saw on main; it keeps U+0307 inside its
    // tokens (PCRE2 counts Mn as a word character), as on main.
    const std::string input = "abI\u0307cd I\u0307 xI\u0307";
    EXPECT_FALSE(NormalizeNFC(input).has_value());
    EXPECT_EQ(PythonTokenize(input), "abi cd i xi");
}

TEST_F(RAGAnalyzerTest, test_nfd_tokenize_with_position) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // The tokens of the NFC form, with byte offsets in the original input,
    // which is longer than its NFC form ("Vie" U+0323 U+0302 "t" is 8 bytes,
    // "Việt" 6). Stray marks that NFC cannot compose are left out: PCRE2
    // counts them as word characters, while Python splits at them, and the
    // Rust port at all but Other_Alphabetic marks.
    struct Case {
        std::string input;
        std::vector<std::string> tokens;
        std::vector<std::pair<unsigned, unsigned>> positions;
    };
    const Case cases[] = {
        {"Vie\u0323\u0302t Nam", {"việt", "nam"}, {{0, 8}, {9, 12}}},
        {"ĐU\u031BO\u031B\u0300NG PHO\u0302\u0301", {"đường", "phố"}, {{0, 12}, {13, 20}}},
    };
    for (const auto &[input, tokens, positions] : cases) {
        auto [actual_tokens, actual_positions] = analyzer_->TokenizeWithPosition(input);
        EXPECT_EQ(actual_tokens, tokens);
        EXPECT_EQ(actual_positions, positions);
    }

    // Composition, then folding: two position mappings chained.
    analyzer_->SetLanguage("slovak");
    auto [tokens, positions] = analyzer_->TokenizeWithPosition("s\u030Ckola");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0], "skola");
    ASSERT_EQ(positions.size(), 1u);
    EXPECT_EQ(positions[0].first, 0u);
    EXPECT_EQ(positions[0].second, 7u);
}

TEST_F(RAGAnalyzerTest, test_reordered_marks_tokenize_with_position) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Marks typed out of canonical order that NFC reorders and that make a
    // token of their own (PCRE2 counts every Mn as a word character, the Rust
    // port Other_Alphabetic marks such as the Arabic harakat; rag_tokenizer.py
    // drops them). U+0651 SHADDA (ccc 33) U+064E FATHA (ccc 30) become U+064E
    // U+0651, and the token's span covers the input bytes of both.
    auto [tokens, positions] = analyzer_->TokenizeWithPosition("+\u0651\u064E x");
    EXPECT_EQ(tokens, (std::vector<std::string>{"\u064E\u0651", "x"}));
    EXPECT_EQ(positions, (std::vector<std::pair<unsigned, unsigned>>{{1, 5}, {6, 7}}));
}

TEST_F(RAGAnalyzerTest, test_fine_grained_tokenize_with_position) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // A fine-grained token NFC changed keeps the span of its coarse token ("Vie"
    // U+0323 U+0302 "t" is 8 bytes, "việt" 6). Every other token gets the spans
    // it had before NFC, stemmed or not ("running" is 7 bytes, "run" 3).
    using Spans = std::vector<std::pair<unsigned, unsigned>>;
    struct Case {
        std::string input;
        std::vector<std::string> tokens;
        Spans positions;
    };
    const Case cases[] = {
        {"Vie\u0323\u0302t Nam", {"việt", "nam"}, {{0, 8}, {9, 12}}},
        {"The running dogs are barking loudly",
         {"the", "run", "dog", "are", "bark", "loud"},
         {{0, 3}, {4, 7}, {12, 15}, {17, 20}, {21, 25}, {29, 33}}},
    };
    for (const auto &[input, tokens, positions] : cases) {
        bool composed = false;
        auto [coarse_tokens, coarse_positions] = analyzer_->TokenizeWithPosition(input, &composed);
        EXPECT_EQ(composed, NormalizeNFC(input).has_value()) << input;
        std::string coarse;
        for (const auto &token : coarse_tokens) {
            coarse += (coarse.empty() ? "" : " ") + token;
        }
        std::vector<std::string> fine_tokens;
        Spans fine_positions;
        analyzer_->FineGrainedTokenizeWithPosition(input, composed, coarse, coarse_positions, fine_tokens, fine_positions);
        EXPECT_EQ(fine_tokens, tokens) << input;
        EXPECT_EQ(fine_positions, positions) << input;
    }

    // The parts of a token NFC left as it is add up from the start of its span,
    // also when NFC changed the rest of the input.
    for (const char *text : {"ab/cd", "ab/cd e\u0301"}) {
        std::vector<std::string> fine_tokens;
        Spans fine_positions;
        analyzer_->FineGrainedTokenizeWithPosition(text, NormalizeNFC(text).has_value(), "ab/cd", Spans{{0, 2}}, fine_tokens, fine_positions);
        EXPECT_EQ(fine_tokens, (std::vector<std::string>{"ab", "cd"})) << text;
        EXPECT_EQ(fine_positions, (Spans{{0, 2}, {3, 5}})) << text;
    }
    // Every part of a token NFC changed gets the span of the whole token.
    std::vector<std::string> fine_tokens;
    Spans fine_positions;
    analyzer_->FineGrainedTokenizeWithPosition("e\u0301", true, "abc/de", Spans{{0, 3}}, fine_tokens, fine_positions);
    EXPECT_EQ(fine_tokens, (std::vector<std::string>{"abc", "de"}));
    EXPECT_EQ(fine_positions, (Spans{{0, 3}, {0, 3}}));
}

TEST_F(RAGAnalyzerTest, test_fine_grained_span_cuts_character) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // A span that cuts a character counts as changed only when NFC changes the
    // input; then every part gets that span. The span (0, 2) ends inside U+00E9.
    using Spans = std::vector<std::pair<unsigned, unsigned>>;
    const std::pair<std::string, Spans> cases[] = {
        {"x\u00e9", {{0, 2}, {3, 5}}},
        {"x\u00e9 e\u0301", {{0, 2}, {0, 2}}},
    };
    for (const auto &[text, positions] : cases) {
        std::vector<std::string> fine_tokens;
        Spans fine_positions;
        analyzer_->FineGrainedTokenizeWithPosition(text, NormalizeNFC(text).has_value(), "ab/cd", Spans{{0, 2}}, fine_tokens, fine_positions);
        EXPECT_EQ(fine_tokens, (std::vector<std::string>{"ab", "cd"})) << text;
        EXPECT_EQ(fine_positions, positions) << text;
    }
}

TEST_F(RAGAnalyzerTest, test_fine_grained_chinese_split_span) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // The parts of a Chinese token add up their lengths from the start of the
    // coarse span, as before NFC, unless NFC changed the token; then every part
    // gets the coarse span.
    using Spans = std::vector<std::pair<unsigned, unsigned>>;
    using TextToken = std::pair<std::string, std::string>;
    const auto fine_grained = [this](const std::string &text, const std::string &token, unsigned end) {
        std::vector<std::string> fine_tokens;
        Spans fine_positions;
        analyzer_->FineGrainedTokenizeWithPosition(text, NormalizeNFC(text).has_value(), token, Spans{{0, end}}, fine_tokens, fine_positions);
        return std::pair{fine_tokens, fine_positions};
    };
    {
        auto [tokens, positions] = fine_grained("矩阵多元", "矩阵多元", 12);
        EXPECT_EQ(tokens, (std::vector<std::string>{"矩阵", "多元"}));
        EXPECT_EQ(positions, (Spans{{0, 6}, {6, 12}}));
    }
    // NFC leaves these tokens as they are, so their parts add up as before, marks
    // or not; in the last input NFC changes another word. C++ may split the marks
    // differently from the Rust port, so the spans are added up from its parts.
    for (const auto &[text, token] : {TextToken{"矩阵\u0301多元", "矩阵\u0301多元"},
                                      TextToken{"矩阵\u1e00\u0301多元", "矩阵\u1e00\u0301多元"},
                                      TextToken{"矩阵\u0316\u0301多元", "矩阵\u0316\u0301多元"},
                                      TextToken{"矩阵\u0301多元 e\u0301", "矩阵\u0301多元"}}) {
        auto [tokens, positions] = fine_grained(text, token, static_cast<unsigned>(token.size()));
        EXPECT_GT(tokens.size(), 1u) << text;
        Spans expected;
        unsigned sub_pos = 0;
        for (const auto &part : tokens) {
            const auto sub_end = sub_pos + static_cast<unsigned>(part.size());
            expected.emplace_back(sub_pos, sub_end);
            sub_pos = sub_end;
        }
        EXPECT_EQ(positions, expected) << text;
    }
    // Inputs NFC changes: e U+0301 becomes é, one byte shorter; A U+0325 becomes
    // U+1E00 with no mark left, and A U+0301 U+0325 becomes U+1E00 U+0301, the
    // same length either way; U+0301 U+0316 swap.
    for (const auto &[text, token] : {TextToken{"矩阵e\u0301多元", "矩阵é多元"},
                                      TextToken{"矩阵A\u0325多元", "矩阵\u1e00多元"},
                                      TextToken{"矩阵A\u0301\u0325多元", "矩阵\u1e00\u0301多元"},
                                      TextToken{"矩阵\u0301\u0316多元", "矩阵\u0316\u0301多元"}}) {
        const auto end = static_cast<unsigned>(text.size());
        auto [tokens, positions] = fine_grained(text, token, end);
        EXPECT_GT(tokens.size(), 1u) << text;
        EXPECT_EQ(positions, Spans(tokens.size(), {0, end})) << text;
    }
}

TEST_F(RAGAnalyzerTest, test_analyze_fine_grained_positions) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // Fine-grained spans through Analyze: the words NFC left as they are keep the
    // spans they had before NFC, also in an input NFC changed.
    analyzer_->SetEnablePosition(true);
    analyzer_->SetFineGrained(true);
    using Spans = std::vector<std::pair<unsigned, unsigned>>;
    const auto spans = [this](const std::string &input) {
        TermList terms;
        analyzer_->Analyze(input, terms);
        Spans out;
        for (const auto &term : terms) {
            out.emplace_back(term.word_offset_, term.end_offset_);
        }
        return out;
    };
    EXPECT_EQ(spans("The running dogs are barking loudly"), (Spans{{0, 3}, {4, 7}, {12, 15}, {17, 20}, {21, 25}, {29, 33}}));
    EXPECT_EQ(spans("The running dogs Vie\u0323\u0302t"), (Spans{{0, 3}, {4, 7}, {12, 15}, {17, 25}}));
    // C++ can keep the marks as a token of their own (PCRE2 counts Mn as a word
    // character); the words before them keep their spans.
    const Spans with_marks = spans("The running dogs +\u0301\u0316");
    ASSERT_GE(with_marks.size(), 3u);
    EXPECT_EQ(Spans(with_marks.begin(), with_marks.begin() + 3), (Spans{{0, 3}, {4, 7}, {12, 15}}));
}

TEST_F(RAGAnalyzerTest, test_nfd_large_token_tokenize) {
    if (!analyzer_) {
        FAIL() << "RAGAnalyzer not loaded, skipping test";
    }
    // 7 000 NFD "é" (21 000 bytes, 14 000 once composed, more than the
    // 12 288-byte lowercase buffer) stay one token.
    std::string input;
    std::string expected;
    for (int i = 0; i < 7000; ++i) {
        input += "e\u0301";
        expected += "é";
    }
    EXPECT_EQ(analyzer_->Tokenize(input), expected);
    EXPECT_EQ(PythonTokenize(input), expected);
    auto [tokens, positions] = analyzer_->TokenizeWithPosition(input);
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0], expected);
    ASSERT_EQ(positions.size(), 1u);
    EXPECT_EQ(positions[0].first, 0u);
    EXPECT_EQ(positions[0].second, static_cast<unsigned>(input.size()));
    std::vector<std::string> fine;
    analyzer_->FineGrainedTokenize(analyzer_->Tokenize(input), fine);
    ASSERT_EQ(fine.size(), 1u);
    EXPECT_EQ(fine[0], expected);
    EXPECT_EQ(PythonTokenize(input, "--fine-grained"), expected);
}

TEST_F(RAGAnalyzerTest, test_nfc_position_map) {
    // A token at NFC bytes [a, b) comes from input bytes [start[a], end[b]),
    // which covers every code point it took in; bytes NFC did not change map
    // to themselves.
    struct Case {
        std::string input;
        std::string expected;
        std::vector<unsigned> start;
        std::vector<unsigned> end;
    };
    const Case cases[] = {
        // U+1EC7 (3 bytes) comes from e U+0323 U+0302 (5 bytes).
        {"Vie\u0323\u0302t", "Việt", {0, 1, 2, 2, 2, 7, 8}, {0, 1, 2, 7, 7, 7, 8}},
        // Marks typed out of canonical order: U+1EB9 takes in the U+0323 at
        // bytes [3, 5), the U+0301 after it comes from bytes [1, 3).
        {"e\u0301\u0323", "ẹ́", {0, 0, 0, 1, 1, 5}, {0, 5, 5, 5, 5, 5}},
        // No starter to compose with: U+0316 moves before U+0301, and a token
        // of either mark spans both.
        {"+\u0301\u0316", "+̖́", {0, 1, 1, 1, 1, 5}, {0, 1, 5, 5, 5, 5}},
        // Hangul conjoining jamo (9 bytes) compose to one syllable.
        {"\u1100\u1161\u11A8", "각", {0, 0, 0, 9}, {0, 9, 9, 9}},
        // A protected code point (U+0958) and its mark are copied as they are;
        // the é after them is composed.
        {"\u0958\u0301"
         "e\u0301",
         "\u0958\u0301\u00E9",
         {0, 1, 2, 3, 4, 5, 5, 8},
         {0, 1, 2, 3, 4, 5, 8, 8}},
        // A byte that is not UTF-8 is copied as it is, between two segments.
        {"e\u0301\xFF"
         "e\u0301",
         "é\xFFé",
         {0, 0, 3, 4, 4, 7},
         {0, 3, 3, 4, 7, 7}},
        // A truncated sequence at the end.
        {"e\u0301\xCC", "é\xCC", {0, 0, 3, 4}, {0, 3, 3, 4}},
    };
    for (const auto &[input, expected, start, end] : cases) {
        NFCPositionMap map;
        const std::optional<std::string> actual = NormalizeNFC(input, &map);
        ASSERT_TRUE(actual.has_value()) << input;
        EXPECT_EQ(*actual, expected) << input;
        EXPECT_EQ(map.start, start) << input;
        EXPECT_EQ(map.end, end) << input;
    }
}

TEST_F(RAGAnalyzerTest, test_nfc_matches_python) {
    // NormalizeNFC against _compose() of rag_tokenizer.py on Python 3.13, whose
    // Unicode version (15.1.0) is the one of the generated tables, on the
    // cases generate_unicode_nfc.py --cases prints: one "input;expected" line
    // per case, code points in hex. The Rust port checks the same cases.
    const char *command = "uv run --no-project --python 3.13 tools/generate_unicode_nfc.py --cases";
    FILE *pipe = popen(command, "r");
    ASSERT_NE(pipe, nullptr) << command;
    std::string output;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    ASSERT_EQ(pclose(pipe), 0) << command;

    const auto chars_from_hex = [](const std::string &field) {
        std::string text;
        std::istringstream hex_values(field);
        std::string hex;
        while (hex_values >> hex) {
            nfc_detail::EncodeUTF8(static_cast<char32_t>(std::stoul(hex, nullptr, 16)), text);
        }
        return text;
    };
    std::istringstream lines(output);
    std::string line;
    std::size_t count = 0;
    while (std::getline(lines, line)) {
        const std::size_t semicolon = line.find(';');
        ASSERT_NE(semicolon, std::string::npos) << line;
        const std::string input = chars_from_hex(line.substr(0, semicolon));
        const std::string expected = chars_from_hex(line.substr(semicolon + 1));
        NFCPositionMap map;
        const std::optional<std::string> actual = NormalizeNFC(input, &map);
        EXPECT_EQ(actual.value_or(input), expected) << line;
        // NFC text comes back as nothing, not as a copy.
        EXPECT_EQ(actual.has_value(), input != expected) << line;
        if (actual) {
            for (const auto *entries : {&map.start, &map.end}) {
                ASSERT_EQ(entries->size(), actual->size() + 1) << line;
                EXPECT_EQ(entries->back(), static_cast<unsigned>(input.size())) << line;
                EXPECT_TRUE(std::is_sorted(entries->begin(), entries->end())) << line;
            }
        }
        ++count;
    }
    EXPECT_GT(count, 4000u);
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
