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

export module japanese_analyzer;

import stl;
import ijma;
import term;
import common_analyzer;
import status;

namespace infinity {

export class JapaneseAnalyzer : public CommonLanguageAnalyzer {
public:
    JapaneseAnalyzer(const String &path);

    JapaneseAnalyzer(const JapaneseAnalyzer &other);

    ~JapaneseAnalyzer();

    void SetIndexMode();

    void SetLabelMode();

    void SetNBest(u32 num = 2) { analyzer_->setOption(jma::Analyzer::OPTION_TYPE_NBEST, num); }

protected:
    void Parse(const String &input) override {
        sentence_->setString(input.c_str());
        analyzer_->runWithSentence(*sentence_);

        list_index_ = 0;
        lexicon_index_ = -1;
        local_offset_ = -1;

        ResetToken();
    }

    bool NextToken() override {
        while (DoNext()) {
            native_token_ = sentence_->getLexicon(list_index_, lexicon_index_);
            native_token_len_ = std::strlen(native_token_);

            // if (native_token_len_ > term_ustring_buffer_limit_)
            //     continue;

            morpheme_ = sentence_->getPOS(list_index_, lexicon_index_);
            is_index_ = true; // sentence_->isIndexWord(list_index_, lexicon_index_);
            offset_ = local_offset_;
            return true;
        }
        ResetToken();
        return false;
    }

    bool IsAlpha() override { return morpheme_ == fl_morp_; }

    bool IsSpecialChar() override { return morpheme_ == sc_morp_; }

private:
    bool DoNext() {
        if (list_index_ == sentence_->getListSize()) {
            return false;
        }
        ++lexicon_index_;

        if (lexicon_index_ == sentence_->getCount(list_index_)) {
            do {
                ++list_index_;
                lexicon_index_ = 0;
                local_offset_ = 0;
                if (list_index_ == sentence_->getListSize())
                    return false;
            } while (lexicon_index_ == sentence_->getCount(list_index_));
        } else {
            ++local_offset_;
        }

        return true;
    }

private:
    String dict_path_;

    jma::Knowledge *knowledge_{nullptr};

    jma::Analyzer *analyzer_{nullptr};

    jma::Sentence *sentence_{nullptr};

    i32 list_index_;

    i32 lexicon_index_;

    u32 morpheme_;

    u32 fl_morp_;

    u32 sc_morp_;

    String segment_;
};
} // namespace infinity
