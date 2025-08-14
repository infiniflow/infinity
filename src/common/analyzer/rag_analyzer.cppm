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

import :stl;
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
    RAGAnalyzer(const String &path);

    RAGAnalyzer(const RAGAnalyzer &other);

    ~RAGAnalyzer();

    void InitStemmer(Language language) { stemmer_->Init(language); }

    Status Load();

    void SetFineGrained(bool fine_grained) { fine_grained_ = fine_grained; }

    String Tokenize(const String &line);

    void FineGrainedTokenize(const String &tokens, Vector<String> &result);

protected:
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;

private:
    static constexpr float DENOMINATOR = 1000000;

    static String StrQ2B(const String &input);

    i32 Freq(std::string_view key) const;

    static String Key(std::string_view line);

    static String RKey(std::string_view line);

    static Pair<Vector<String>, double> Score(const Vector<Pair<String, int>> &token_freqs);

    static void SortTokens(const Vector<Vector<Pair<String, int>>> &token_list, Vector<Pair<Vector<String>, double>> &res);

    Pair<Vector<String>, double> MaxForward(const String &line) const;

    Pair<Vector<String>, double> MaxBackward(const String &line) const;

    int DFS(const String &chars,
            int s,
            Vector<Pair<String, int>> &pre_tokens,
            Vector<Vector<Pair<String, int>>> &token_list,
            Vector<String> &best_tokens,
            double &max_score,
            bool memo_all) const;

    void TokenizeInner(Vector<String> &res, const String &L) const;

    void SplitLongText(const String &L, u32 length, Vector<String> &sublines) const;

    String Merge(const String &tokens) const;

    void EnglishNormalize(const Vector<String> &tokens, Vector<String> &res);

public:
    Vector<Pair<Vector<std::string_view>, double>> GetBestTokensTopN(std::string_view chars, u32 n) const;

    static const SizeT term_string_buffer_limit_ = 4096 * 3;

    String dict_path_;

    bool own_dict_{};

    DartsTrie *trie_{nullptr};

    POSTable *pos_table_{nullptr};

    Lemmatizer *lemma_{nullptr};

    UniquePtr<Stemmer> stemmer_;

    OpenCC *opencc_{nullptr};

    Vector<char> lowercase_string_buffer_;

    bool fine_grained_{false};

    UniquePtr<NLTKWordTokenizer> nltk_tokenizer_;

    re2::RE2 pattern1_{"[a-zA-Z_-]+$"};

    re2::RE2 pattern2_{"[a-z\\.-]+$"};

    re2::RE2 pattern3_{"[0-9\\.-]+$"};

    re2::RE2 pattern4_{"[0-9,\\.-]+$"};

    re2::RE2 pattern5_{"[a-z\\.-]+"};

    re2::RE2 regex_split_pattern_{R"#(([ ,\.<>/?;'\[\]\`!@#$%^&*$$\{\}\|_+=《》，。？、；‘’：“”【】~！￥%……（）——-]+|[a-zA-Z\.-]+|[0-9,\.-]+))#"};

    re2::RE2 blank_pattern_{"( )"};

    re2::RE2 replace_space_pattern_{R"#(([ ]+))#"};
};
} // namespace infinity