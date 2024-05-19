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
import japanese_analyzer;
import standard_analyzer;
import ngram_analyzer;

module analyzer_pool;

namespace infinity {


constexpr u64 basis = 0xCBF29CE484222325ull;
constexpr u64 prime = 0x100000001B3ull;

constexpr u64 Str2Int(const char *str, u64 last_value = basis) {
    return (*str != '\0' && *str != '-') ? Str2Int(str + 1, (*str ^ last_value) * prime) : last_value;
}

Tuple<UniquePtr<Analyzer>, Status> AnalyzerPool::GetAnalyzer(const std::string_view &name) {
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
                    path = config->ResourcePath();
                }
                UniquePtr<ChineseAnalyzer> analyzer = MakeUnique<ChineseAnalyzer>(std::move(path));
                Status load_status = analyzer->Load();
                if (!load_status.ok()) {
                    return {nullptr, load_status};
                }
                prototype = analyzer.get();
                cache_[CHINESE] = std::move(analyzer);
            }
            return {MakeUnique<ChineseAnalyzer>(*reinterpret_cast<ChineseAnalyzer *>(prototype)), Status::OK()};
        }
        case Str2Int(JAPANESE.data()): {
            Analyzer *prototype = cache_[JAPANESE].get();
            if (prototype == nullptr) {
                String path;
                Config *config = InfinityContext::instance().config();
                if (config == nullptr) {
                    // InfinityContext has not been initialized.
                    path = "/var/infinity/resource";
                } else {
                    path = config->ResourcePath();
                }
                UniquePtr<JapaneseAnalyzer> analyzer = MakeUnique<JapaneseAnalyzer>(std::move(path));
                Status load_status = analyzer->Load();
                if (!load_status.ok()) {
                    return {nullptr, load_status};
                }
                prototype = analyzer.get();
                cache_[JAPANESE] = std::move(analyzer);
            }
            return {MakeUnique<JapaneseAnalyzer>(*reinterpret_cast<JapaneseAnalyzer *>(prototype)), Status::OK()};
        }
        case Str2Int(STANDARD.data()): {
            return {MakeUnique<StandardAnalyzer>(), Status::OK()};
        }
        case Str2Int(NGRAM.data()): {
            // ngram-{number}
            u32 ngram = 0;
            const char *str = name.data();
            while (*str != '\0' && *str != '-') {
                str++;
            }
            if (*str == '-') {
                str++;
                ngram = std::strtol(str, nullptr, 10);
            } else {
                return {nullptr, Status::InvalidAnalyzerName(fmt::format("NGRAM-number, but it is {}.", name))};
            }
            if (ngram <= 0) {
                return {nullptr, Status::InvalidAnalyzerName(fmt::format("NGRAM-number, number > 0, but it is {}.", name))};
            }
            return {MakeUnique<NGramAnalyzer>(ngram), Status::OK()};
        }
        default: {
            return {nullptr, Status::AnalyzerNotFound(name.data())};
        }
    }
}

} // namespace infinity