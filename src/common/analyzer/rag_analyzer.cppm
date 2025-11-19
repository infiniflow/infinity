// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include "opencc/openccxx.h"

export module infinity_core:rag_analyzer;

import :term;
import :status;
import :darts_trie;
import :lemmatizer;
import :stemmer;
import :analyzer;

import third_party;

namespace infinity {

// C++ reimplementation of
// https://github.com/infiniflow/ragflow/blob/main/rag/nlp/rag_tokenizer.py

class NLTKWordTokenizer;
export class RAGAnalyzer : public Analyzer {
public:
    RAGAnalyzer(const std::string &path);

    RAGAnalyzer(const RAGAnalyzer &other);

    ~RAGAnalyzer();

    void InitStemmer(Language language) { stemmer_->Init(language); }

    Status Load();

    void SetFineGrained(bool fine_grained) { fine_grained_ = fine_grained; }

    void SetEnablePosition(bool enable_position) { enable_position_ = enable_position; }

    std::pair<std::vector<std::string>, std::vector<std::pair<unsigned, unsigned>>> TokenizeWithPosition(const std::string &line);
    std::string Tokenize(const std::string &line);

    void FineGrainedTokenize(const std::string &tokens, std::vector<std::string> &result);

    void TokenizeInnerWithPosition(const std::string &L,
                                   std::vector<std::string> &tokens,
                                   std::vector<std::pair<unsigned, unsigned>> &positions,
                                   unsigned base_pos,
                                   const std::vector<unsigned> *pos_mapping = nullptr);
    void FineGrainedTokenizeWithPosition(const std::string &tokens_str,
                                         const std::vector<std::pair<unsigned, unsigned>> &positions,
                                         std::vector<std::string> &fine_tokens,
                                         std::vector<std::pair<unsigned, unsigned>> &fine_positions);
    void EnglishNormalizeWithPosition(const std::vector<std::string> &tokens,
                                      const std::vector<std::pair<unsigned, unsigned>> &positions,
                                      std::vector<std::string> &normalize_tokens,
                                      std::vector<std::pair<unsigned, unsigned>> &normalize_positions);
    unsigned MapToOriginalPosition(unsigned processed_pos, const std::vector<std::pair<unsigned, unsigned>> &mapping);
    void MergeWithPosition(const std::vector<std::string> &tokens,
                           const std::vector<std::pair<unsigned, unsigned>> &positions,
                           std::vector<std::string> &merged_tokens,
                           std::vector<std::pair<unsigned, unsigned>> &merged_positions);

protected:
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;

private:
    static constexpr float DENOMINATOR = 1000000;

    static std::string StrQ2B(const std::string &input);

    static void BuildPositionMapping(const std::string &original, const std::string &converted, std::vector<unsigned> &pos_mapping);

    i32 Freq(std::string_view key) const;

    static std::string Key(std::string_view line);

    static std::string RKey(std::string_view line);

    static std::pair<std::vector<std::string>, double> Score(const std::vector<std::pair<std::string, int>> &token_freqs);

    static void SortTokens(const std::vector<std::vector<std::pair<std::string, int>>> &token_list,
                           std::vector<std::pair<std::vector<std::string>, double>> &res);

    std::pair<std::vector<std::string>, double> MaxForward(const std::string &line) const;

    std::pair<std::vector<std::string>, double> MaxBackward(const std::string &line) const;

    int DFS(const std::string &chars,
            int s,
            std::vector<std::pair<std::string, int>> &pre_tokens,
            std::vector<std::vector<std::pair<std::string, int>>> &token_list,
            std::vector<std::string> &best_tokens,
            double &max_score,
            bool memo_all) const;

    void TokenizeInner(std::vector<std::string> &res, const std::string &L) const;

    void SplitLongText(const std::string &L, u32 length, std::vector<std::string> &sublines) const;

    std::string Merge(const std::string &tokens) const;

    void EnglishNormalize(const std::vector<std::string> &tokens, std::vector<std::string> &res);

public:
    std::vector<std::pair<std::vector<std::string_view>, double>> GetBestTokensTopN(std::string_view chars, u32 n) const;

    static const size_t term_string_buffer_limit_ = 4096 * 3;

    std::string dict_path_;

    bool own_dict_{};

    DartsTrie *trie_{nullptr};

    POSTable *pos_table_{nullptr};

    Lemmatizer *lemma_{nullptr};

    std::unique_ptr<Stemmer> stemmer_;

    OpenCC *opencc_{nullptr};

    std::vector<char> lowercase_string_buffer_;

    bool fine_grained_{false};

    bool enable_position_{false};

    std::unique_ptr<NLTKWordTokenizer> nltk_tokenizer_;

    re2::RE2 pattern1_{"[a-zA-Z_-]+$"};

    re2::RE2 pattern2_{"[a-z\\.-]+$"};

    re2::RE2 pattern3_{"[0-9\\.-]+$"};

    re2::RE2 pattern4_{"[0-9,\\.-]+$"};

    re2::RE2 pattern5_{"[a-z\\.-]+"};

    re2::RE2 regex_split_pattern_{R"#(([ ,\.<>/?;'\[\]\`!@#$%^&*$$\{\}\|_+=《》，。？、；‘’：“”【】~！￥%……（）——-]+|[a-zA-Z\.-]+|[0-9,\.-]+))#"};

    re2::RE2 blank_pattern_{"( )"};

    re2::RE2 replace_space_pattern_{R"#(([ ]+))#"};

    std::unordered_map<std::string, std::string> text_stem_map_;
};
} // namespace infinity