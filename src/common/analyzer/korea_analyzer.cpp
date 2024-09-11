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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#include <mecab_wrapper.h>
#pragma clang diagnostic pop

#include <cstring>
#include <filesystem>
#include <iostream>
import stl;
import term;
import analyzer;
import common_analyzer;
import logger;
import status;
import ijma;

module korea_analyzer;

namespace fs = std::filesystem;

namespace infinity {
static const String KNOWLEDGE_PATH = "mecab/ko-dic";

KoreaAnalyzer::KoreaAnalyzer(const String &base_path) {
    cjk_ = true;
    own_mecab_ = true;
    fs::path root(base_path);
    fs::path knowledge_path(root / KNOWLEDGE_PATH);
    knowledge_path_ = "-d " + knowledge_path.string();
}

KoreaAnalyzer::KoreaAnalyzer(const KoreaAnalyzer &other) {
    cjk_ = true;
    knowledge_path_ = other.knowledge_path_;
    own_mecab_ = false;
    SetCaseSensitive(false);
}

KoreaAnalyzer::~KoreaAnalyzer() {
    if (own_mecab_)
        delete mecab_;
}

Status KoreaAnalyzer::Load() {
    try {
        mecab_ = new jma::MeCab(knowledge_path_);
    } catch (std::logic_error) {
        return Status::InvalidAnalyzerFile("Failed to load Korea analyzer");
    }

    SetCaseSensitive(false);

    return Status::OK();
}

} // namespace infinity