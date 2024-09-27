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

export module rag_analyzer;

import stl;
import term;
import status;
import darts_trie;

namespace infinity {

// C++ reimplementation of
// https://github.com/infiniflow/ragflow/blob/main/rag/nlp/rag_tokenizer.py

export class RAGAnalyzer {
public:
    RAGAnalyzer(const String &path);

    RAGAnalyzer(const RAGAnalyzer &other);

    ~RAGAnalyzer();

    Status Load();

    void Tokenize(const String &line, Vector<String> &res);

private:
    static constexpr float DENOMINATOR = 1000000;

    String StrQ2B(const String &input);

    i32 Freq(const String &key);

    String Key(const String &line);

    String RKey(const String &line);

    Pair<Vector<String>, double> Score(const Vector<Pair<String, i64>> &token_freqs);

    void SortTokens(const Vector<Vector<Pair<String, i64>>> &token_list, Vector<Pair<Vector<String>, double>> &res);

    Pair<Vector<String>, double> MaxForward(const String &line);

    Pair<Vector<String>, double> MaxBackward(const String &line);

    int DFS(const String &chars, int s, Vector<Pair<String, i64>> &pre_tokens, Vector<Vector<Pair<String, i64>>> &token_list);

    String Merge(const String &tokens);

private:
    String dict_path_;

    bool own_dict_{};

    DartsTrie *trie_{nullptr};

    POSTable *pos_table_{nullptr};
};
} // namespace infinity