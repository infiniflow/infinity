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

export module korea_analyzer;

import stl;
import ijma;
import term;
import common_analyzer;
import status;

namespace infinity {

export class KoreaAnalyzer : public CommonLanguageAnalyzer {
public:
    KoreaAnalyzer(const String &path);

    KoreaAnalyzer(const KoreaAnalyzer &other);

    ~KoreaAnalyzer();

    Status Load();

protected:
    void Parse(const String &input) override {
        mecab_->SetSentence(input);
        local_offset_ = -1;

        ResetToken();
    }

    bool NextToken() override {
        while (DoNext()) {
            mecab_->GetToken(token_str_);
            token_ = token_str_.c_str();
            len_ = token_str_.size();
            offset_ = local_offset_;
            is_index_ = true;
            return true;
        }
        ResetToken();
        return false;
    }

    bool IsAlpha() override { return mecab_->IsAlpha(); }

    bool IsSpecialChar() override { return false; }

private:
    bool DoNext() {
        while (!mecab_->IsEnd()) {
            mecab_->Next();
            ++local_offset_;
            return true;
        }
        return false;
    }

    String knowledge_path_;

    jma::MeCab *mecab_{nullptr};

    bool own_mecab_;

    String token_str_;
};
} // namespace infinity
