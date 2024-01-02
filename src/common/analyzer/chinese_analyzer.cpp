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
#include <cppjieba/Jieba.hpp>

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

ChineseAnalyzer::ChineseAnalyzer(const String &path) : dict_path_(path) {}

ChineseAnalyzer::ChineseAnalyzer(const ChineseAnalyzer &other) {
    own_jieba_ = false;
    jieba_ = other.jieba_;
}
ChineseAnalyzer::~ChineseAnalyzer() {
    if (own_jieba_ && jieba_)
        delete jieba_;
}

bool ChineseAnalyzer::Load() {
    fs::path root(dict_path_);
    fs::path dict_path(root / DICT_PATH);
    fs::path hmm_path(root / HMM_PATH);
    fs::path userdict_path(root / USER_DICT_PATH);
    fs::path idf_path(root / IDF_PATH);
    fs::path stopwords_path(root / STOP_WORD_PATH);

    if (!fs::exists(dict_path)) {
        LOG_INFO(Format("Invalid jieba config {} dict for jieba_analyzer does not exist", dict_path.string()));
        return false;
    }
    if (!fs::exists(hmm_path)) {
        LOG_INFO(Format("Invalid jieba config {} hmm for jieba_analyzer does not exist", hmm_path.string()));
        return false;
    }
    if (!fs::exists(userdict_path)) {
        LOG_INFO(Format("Invalid jieba config {} user_dict for jieba_analyzer does not exist", userdict_path.string()));
        return false;
    }
    if (!fs::exists(idf_path)) {
        LOG_INFO(Format("Invalid jieba config {} idf for jieba_analyzer does not exist", idf_path.string()));
        return false;
    }
    if (!fs::exists(stopwords_path)) {
        LOG_INFO(Format("Invalid jieba config {} stopword for jieba_analyzer does not exist", stopwords_path.string()));
        return false;
    }

    try {
        jieba_ = new cppjieba::Jieba(dict_path.string(), hmm_path.string(), userdict_path.string(), idf_path.string(), stopwords_path.string());
    } catch (const std::exception &e) {
        return false;
    }
    own_jieba_ = true;
    LoadStopwordsDict(stopwords_path.string());
    return true;
}

void ChineseAnalyzer::LoadStopwordsDict(const String &stopwords_path) {
    std::ifstream ifs(stopwords_path);
    String line;
    while (getline(ifs, line)) {
        stopwords_.insert(line);
    }
}

int ChineseAnalyzer::AnalyzeImpl(const Term &input, void *data, HookTypeForJieba func) {
    Parse(input.text_);
    for (u32 i = 0; i < cut_words_.size(); ++i) {
        if (!Accept_token(cut_words_[i].word))
            continue;
        func(data, cut_words_[i]);
    }
    return cut_words_.back().offset + 1;
}

} // namespace infinity
