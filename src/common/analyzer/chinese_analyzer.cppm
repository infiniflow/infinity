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

export module infinity_core:chinese_analyzer;

import :jieba;
import :term;
import :analyzer;
import :common_analyzer;
import :status;

import third_party;

namespace infinity {

export class ChineseAnalyzer : public CommonLanguageAnalyzer {
public:
    ChineseAnalyzer(const std::string &path);

    ChineseAnalyzer(const ChineseAnalyzer &other);

    virtual ~ChineseAnalyzer();

    Status Load();

    void SetCutGrain(CutGrain cut_grain) { cut_grain_ = cut_grain; }

protected:
    void Parse(const std::string &input) override;

    bool NextToken() override;

    bool IsAlpha() override { return jieba_->IsAlpha(token_); }

    bool IsSpecialChar() override { return false; }

private:
    void LoadStopwordsDict(const std::string &stopwords_path);

    bool Accept_token(const std::string &term) { return !stopwords_->contains(term); }

    bool DoNext();

protected:
    cppjieba::Jieba *jieba_{nullptr};
    std::string dict_path_;
    bool own_jieba_{};
    std::vector<cppjieba::Word> cut_words_;
    i32 cursor_{};
    i32 cut_size_{};
    std::shared_ptr<FlatHashSet<std::string>> stopwords_{};
    CutGrain cut_grain_{CutGrain::kCoarse};
};
} // namespace infinity
