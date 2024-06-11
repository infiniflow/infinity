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

export module chinese_analyzer;

import stl;
import jieba;
import term;
import common_analyzer;
import third_party;
import status;

namespace infinity {

export class ChineseAnalyzer : public CommonLanguageAnalyzer {
public:
    ChineseAnalyzer(const String &path);

    ChineseAnalyzer(const ChineseAnalyzer &other);

    ~ChineseAnalyzer();

    Status Load();

protected:
    void Parse(const String &input) override;

    bool NextToken() override;

    bool IsAlpha() override { return jieba_->IsAlpha(cut_words_[cursor_].word); }

    bool IsSpecialChar() override { return jieba_->IsPunch(cut_words_[cursor_].word); }

private:
    void LoadStopwordsDict(const String &stopwords_path);

    bool Accept_token(const String &term) { return !stopwords_->contains(term); }

    bool DoNext();

private:
    cppjieba::Jieba *jieba_{nullptr};
    String dict_path_;
    bool own_jieba_{};
    Vector<cppjieba::Word> cut_words_;
    i32 cursor_{};
    i32 cut_size_{};
    SharedPtr<FlatHashSet<String>> stopwords_{};
};
} // namespace infinity
