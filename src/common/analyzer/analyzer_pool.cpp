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
import config;
import infinity_context;
import analyzer;
import chinese_analyzer;
import standard_analyzer;
import ngram_analyzer;

module analyzer_pool;

namespace infinity {

constexpr StringView CHINESE = "chinese";
constexpr StringView STANDARD = "standard";
constexpr StringView NGRAM = "ngram";

constexpr u64 basis = 0xCBF29CE484222325ull;
constexpr u64 prime = 0x100000001B3ull;

constexpr u64 Str2Int(char const *str, u64 last_value = basis) { return *str ? Str2Int(str + 1, (*str ^ last_value) * prime) : last_value; }

void AnalyzerPool::Set(const StringView &name) {
    Analyzer *try_analyzer = cache_[name].get();
    if (!try_analyzer) {
        switch (Str2Int(name.data())) {
            case Str2Int(CHINESE.data()): {
                String path = InfinityContext::instance().config()->resource_dict_path();
                UniquePtr<ChineseAnalyzer> analyzer = MakeUnique<ChineseAnalyzer>(std::move(path));
                if (analyzer->Load())
                    cache_[CHINESE] = std::move(analyzer);
            } break;
            case Str2Int(STANDARD.data()): {
                UniquePtr<StandardAnalyzer> analyzer = MakeUnique<StandardAnalyzer>();
                cache_[STANDARD] = std::move(analyzer);
            } break;
            case Str2Int(NGRAM.data()): {
                u32 ngram = 2; /// TODO config
                UniquePtr<NGramAnalyzer> analyzer = MakeUnique<NGramAnalyzer>(ngram);
                cache_[NGRAM] = std::move(analyzer);
            } break;
            default:
                break;
        }
    }
}

UniquePtr<Analyzer> AnalyzerPool::Get(const StringView &name) {
    Analyzer *analyzer = cache_[name].get();
    if (!analyzer)
        return nullptr;
    switch (Str2Int(name.data())) {
        case Str2Int(CHINESE.data()): {
            return MakeUnique<ChineseAnalyzer>(*reinterpret_cast<ChineseAnalyzer *>(analyzer));
        } break;
        case Str2Int(STANDARD.data()): {
            return MakeUnique<StandardAnalyzer>(*reinterpret_cast<StandardAnalyzer *>(analyzer));
        } break;
        case Str2Int(NGRAM.data()): {
            return MakeUnique<NGramAnalyzer>(*reinterpret_cast<NGramAnalyzer *>(analyzer));
        } break;
        default:
            return nullptr;
    }
}

} // namespace infinity