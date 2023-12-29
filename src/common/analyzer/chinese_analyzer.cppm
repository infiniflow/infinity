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

#include <cppjieba/Jieba.hpp>

import stl;
import term;
import analyzer;
import third_party;
export module chinese_analyzer;

namespace infinity {

export class ChineseAnalyzer : public Analyzer {
public:
    ChineseAnalyzer(const String &path);

    ChineseAnalyzer(const ChineseAnalyzer &other);

    ~ChineseAnalyzer();

protected:
    inline void Parse(const String &input) { jieba_->CutForSearch(input, cut_words_, true); }
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;

private:
    void LoadStopwordsDict(const String &stopwords_path);
    bool Load();
    bool Accept_token(const String &term) { return !stopwords_.contains(term); }

private:
    cppjieba::Jieba *jieba_{nullptr};
    String dict_path_;
    bool own_jieba_{};
    Vector<cppjieba::Word> cut_words_;
    FlatHashSet<String> stopwords_;
};
} // namespace infinity
