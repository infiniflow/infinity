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

constexpr std::string_view CHINESE = "chinese";
constexpr std::string_view STANDARD = "standard";
constexpr std::string_view NGRAM = "ngram";

constexpr u64 basis = 0xCBF29CE484222325ull;
constexpr u64 prime = 0x100000001B3ull;

constexpr u64 Str2Int(const char *str, u64 last_value = basis) {
    return (*str != '\0' && *str != '-') ? Str2Int(str + 1, (*str ^ last_value) * prime) : last_value;
}

UniquePtr<Analyzer> AnalyzerPool::Get(const std::string_view &name) {
    switch (Str2Int(name.data())) {
        case Str2Int(CHINESE.data()): {
            Analyzer *prototype = cache_[CHINESE].get();
            if (prototype == nullptr) {
                String path;
                Config *config = InfinityContext::instance().config();
                if (config == nullptr) {
                    // InfinityContext has not been initialized.
                    path = "/var/infinity/resource";
                } else {
                    path = config->resource_dict_path();
                }
                UniquePtr<ChineseAnalyzer> analyzer = MakeUnique<ChineseAnalyzer>(std::move(path));
                if (!analyzer->Load()) {
                    return nullptr;
                }
                prototype = analyzer.get();
                cache_[CHINESE] = std::move(analyzer);
            }
            return MakeUnique<ChineseAnalyzer>(*reinterpret_cast<ChineseAnalyzer *>(prototype));
        } break;
        case Str2Int(STANDARD.data()): {
            return MakeUnique<StandardAnalyzer>();
        }
        case Str2Int(NGRAM.data()): {
            u32 ngram = 0;
            const char *str = name.data();
            while (*str != '\0' && *str != '-') {
                str++;
            }
            if (*str == '-') {
                str++;
                ngram = std::strtol(str, nullptr, 10);
            } else {
                return nullptr;
            }
            if (ngram <= 0) {
                return nullptr;
            }
            return MakeUnique<NGramAnalyzer>(ngram);
        }
        default:
            return nullptr;
    }
}

} // namespace infinity