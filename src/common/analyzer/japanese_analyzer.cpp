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

#include "string_utils.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#include <ijma.h>
#pragma clang diagnostic pop

#include <cstring>
#include <filesystem>

import stl;
import term;
import analyzer;
import common_analyzer;
import logger;

module japanese_analyzer;

namespace fs = std::filesystem;

namespace infinity {
static const String KNOWLEDGE_PATH = "mecab/ipadic";

// Foreign Language is Japanese POS
static constexpr const char *JAPANESE_FL = "S-A";

// Special character is Japanese POS
static constexpr const char *JAPANESE_SC = "S-G";

class JMAKnowledge {
private:
    JMAKnowledge(const char *knowledge_path) {
        if (knowledge_path != nullptr) {
            knowledge_ = jma::JMA_Factory::instance()->createKnowledge();
            // load system dictioanry files
            knowledge_->setSystemDict(knowledge_path);

            if (knowledge_->loadDict() == 0) {
                String msg = "Failed to load JMA knowledge from path: ";
                msg.append(knowledge_path);
                throw std::logic_error(msg);
            }
            std::cout << "finish!" << std::endl;
        }
    }
    ~JMAKnowledge() { delete knowledge_; }

public:
    static JMAKnowledge &instance(const char *knowledge_path) {
        static JMAKnowledge instance(knowledge_path);
        return instance;
    }

    jma::Knowledge *knowledge_{nullptr};
};

JapaneseAnalyzer::JapaneseAnalyzer(const String &base_path) {
    chinese_ = true;
    jma::JMA_Factory *factory = jma::JMA_Factory::instance();
    analyzer_ = factory->createAnalyzer();
    fs::path root(base_path);
    fs::path knowledge_path(root / KNOWLEDGE_PATH);
    knowledge_ = JMAKnowledge::instance(knowledge_path.string().c_str()).knowledge_;

    analyzer_->setKnowledge(knowledge_);
    sentence_ = new jma::Sentence();
    fl_morp_ = analyzer_->getCodeFromStr(JAPANESE_FL);
    sc_morp_ = analyzer_->getCodeFromStr(JAPANESE_SC);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);

    SetCaseSensitive(false);
    SetIndexMode(); // Index mode is set by default
}

JapaneseAnalyzer::JapaneseAnalyzer(const JapaneseAnalyzer &other) : knowledge_(other.knowledge_) {
    chinese_ = true;
    jma::JMA_Factory *factory = jma::JMA_Factory::instance();
    analyzer_ = factory->createAnalyzer();
    analyzer_->setKnowledge(knowledge_);
    sentence_ = new jma::Sentence();
    fl_morp_ = analyzer_->getCodeFromStr(JAPANESE_FL);
    sc_morp_ = analyzer_->getCodeFromStr(JAPANESE_SC);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);

    SetCaseSensitive(false);
    SetIndexMode(); // Index mode is set by default
}

JapaneseAnalyzer::~JapaneseAnalyzer() {
    delete analyzer_;
    delete sentence_;
}

void JapaneseAnalyzer::SetIndexMode() {
    if (analyzer_ == nullptr) {
        throw std::logic_error("JapaneseAnalyzer::SetIndexMode() is call with analyzer_ nullptr");
    }

    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_NBEST, 1);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 0);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 0);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH, 1);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE, 0);
}

void JapaneseAnalyzer::SetLabelMode() {
    if (analyzer_ == nullptr) {
        throw std::logic_error("JapaneseAnalyzer::SetLabelMode() is call with analyzer_ nullptr");
    }

    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_NBEST, 1);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 1);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 1);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH, 1);
    analyzer_->setOption(jma::Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE, 0);
}

} // namespace infinity