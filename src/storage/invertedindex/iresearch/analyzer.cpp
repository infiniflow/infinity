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

import stl;
import third_party;

module iresearch_analyzer;

namespace infinity {

constexpr StringView JIEBA = "jieba";
constexpr StringView SEGMENT = "segmentation";

constexpr u64 basis = 0xCBF29CE484222325ull;
constexpr u64 prime = 0x100000001B3ull;

constexpr u64 Str2Int(char const *str, u64 last_value = basis) { return *str ? Str2Int(str + 1, (*str ^ last_value) * prime) : last_value; }

void AnalyzerPool::Set(const String &name, const String &args) {
    if (!Get(name).get()) {
        switch (Str2Int(name.c_str())) {
            case Str2Int(JIEBA.data()): {
                IRSJiebaAnalyzer::options_t opt;
                opt.path_ = args;
                IRSAnalyzer::ptr analyzer = MakeUnique<IRSJiebaAnalyzer>(Move(opt));
                cache_.emplace(JIEBA, Move(analyzer));
            } break;
            case Str2Int(SEGMENT.data()): {
                IRSSegmentationAnalyzer::options_t opt;
                IRSAnalyzer::ptr analyzer = MakeUnique<IRSSegmentationAnalyzer>(Move(opt));
                cache_.emplace(SEGMENT, Move(analyzer));
            } break;
            default:
                break;
        }
    }
}

IRSAnalyzer::ptr AnalyzerPool::Get(const String &name) { return Move(cache_[name.c_str()]); }

} // namespace infinity