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
#include <cppjieba/Jieba.hpp>
#pragma clang diagnostic pop

#include <cstring>
#include <filesystem>

import stl;
import term;
import analyzer;
import common_analyzer;
import logger;
import third_party;

module chinese_analyzer;

namespace fs = std::filesystem;

namespace infinity {
static const String DICT_PATH = "jieba/dict/jieba.dict.utf8";
static const String HMM_PATH = "jieba/dict/hmm_model.utf8";
static const String USER_DICT_PATH = "jieba/dict/user.dict.utf8";
static const String IDF_PATH = "jieba/dict/idf.utf8";
static const String STOP_WORD_PATH = "jieba/dict/stop_words.utf8";

namespace fs = std::filesystem;

ChineseAnalyzer::ChineseAnalyzer(const String &path) : dict_path_(path) { cjk_ = true; }

ChineseAnalyzer::ChineseAnalyzer(const ChineseAnalyzer &other) {
    cjk_ = true;
    own_jieba_ = false;
    jieba_ = other.jieba_;
    stopwords_ = other.stopwords_;
}
ChineseAnalyzer::~ChineseAnalyzer() {
    if (own_jieba_ && jieba_) {
        delete jieba_;
    }
}

Status ChineseAnalyzer::Load() {
    fs::path root(dict_path_);
    fs::path dict_path(root / DICT_PATH);
    fs::path hmm_path(root / HMM_PATH);
    fs::path userdict_path(root / USER_DICT_PATH);
    fs::path idf_path(root / IDF_PATH);
    fs::path stopwords_path(root / STOP_WORD_PATH);

    if (!fs::exists(dict_path)) {
        // LOG_INFO(fmt::format("Invalid jieba config {} dict for jieba_analyzer does not exist", dict_path.string()));
        return Status::InvalidAnalyzerFile(dict_path);
    }
    if (!fs::exists(hmm_path)) {
        // LOG_INFO(fmt::format("Invalid jieba config {} hmm for jieba_analyzer does not exist", hmm_path.string()));
        return Status::InvalidAnalyzerFile(hmm_path);
    }
    if (!fs::exists(userdict_path)) {
        // LOG_INFO(fmt::format("Invalid jieba config {} user_dict for jieba_analyzer does not exist", userdict_path.string()));
        return Status::InvalidAnalyzerFile(userdict_path);
    }
    if (!fs::exists(idf_path)) {
        // LOG_INFO(fmt::format("Invalid jieba config {} idf for jieba_analyzer does not exist", idf_path.string()));
        return Status::InvalidAnalyzerFile(idf_path);
    }
    if (!fs::exists(stopwords_path)) {
        // LOG_INFO(fmt::format("Invalid jieba config {} stopword for jieba_analyzer does not exist", stopwords_path.string()));
        return Status::InvalidAnalyzerFile(stopwords_path);
    }

    try {
        jieba_ = new cppjieba::Jieba(dict_path.string(), hmm_path.string(), userdict_path.string(), idf_path.string(), stopwords_path.string());
    } catch (const std::exception &e) {
        return Status::InvalidAnalyzerFile("Failed to load Jieba analyzer");
    }
    own_jieba_ = true;
    LoadStopwordsDict(stopwords_path.string());
    return Status::OK();
}

void ChineseAnalyzer::LoadStopwordsDict(const String &stopwords_path) {
    std::ifstream ifs(stopwords_path);
    String line;
    stopwords_ = MakeShared<FlatHashSet<String>>();
    while (getline(ifs, line)) {
        stopwords_->insert(line);
    }
}

void ChineseAnalyzer::Parse(const String &input) {
    if (cut_grain_ == CutGrain::kCoarse)
        jieba_->Cut(input, cut_words_, true);
    else if (cut_grain_ == CutGrain::kFine)
        jieba_->CutHMM(input, cut_words_);
    else
        jieba_->CutForSearch(input, cut_words_, true);

    local_offset_ = -1;
    cursor_ = -1;
    cut_size_ = cut_words_.size();
    ResetToken();
}

bool ChineseAnalyzer::NextToken() {
    while (DoNext()) {
        is_index_ = true;
        offset_ = local_offset_;
        token_ = cut_words_[cursor_].word.c_str();
        len_ = cut_words_[cursor_].word.length();

        return true;
    }
    ResetToken();
    return false;
}

bool ChineseAnalyzer::DoNext() {
    while (cursor_ < (cut_size_ - 1)) {
        cursor_++;
        if (Accept_token(cut_words_[cursor_].word)) {
            ++local_offset_;
            return true;
        } else {
            continue;
        }
    }
    return false;
}
} // namespace infinity
