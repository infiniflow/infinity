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

#include <cstring>
#include <filesystem>

#include <openccxx.h>

import stl;
import logger;
import third_party;
import chinese_analyzer;

module traditional_chinese_analyzer;

namespace fs = std::filesystem;

namespace infinity {
static const String OPENCC_PATH = "opencc";

namespace fs = std::filesystem;

TraditionalChineseAnalyzer::TraditionalChineseAnalyzer(const String &path) : ChineseAnalyzer(path) {}

TraditionalChineseAnalyzer::TraditionalChineseAnalyzer(const TraditionalChineseAnalyzer &other) : ChineseAnalyzer(other) { own_opencc_ = false; }

TraditionalChineseAnalyzer::~TraditionalChineseAnalyzer() {
    if (own_opencc_) {
        delete opencc_;
    }
}

Status TraditionalChineseAnalyzer::Load() {
    try {
        ChineseAnalyzer::Load();
    } catch (const std::exception &e) {
        return Status::InvalidAnalyzerFile("Failed to load jieba analyzer");
    }
    fs::path root(dict_path_);
    fs::path opencc_path(root / OPENCC_PATH);

    if (!fs::exists(opencc_path)) {
        return Status::InvalidAnalyzerFile(opencc_path);
    }
    try {
        opencc_ = new ::OpenCC(opencc_path.string());
    } catch (const std::exception &e) {
        return Status::InvalidAnalyzerFile("Failed to load OpenCC");
    }
    own_opencc_ = true;
    return Status::OK();
}

void TraditionalChineseAnalyzer::Parse(const String &input) {
    String out;
    opencc_->convert(input, out);
    ChineseAnalyzer::Parse(out);
}

} // namespace infinity