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

module analyzer_pool;

import stl;
import third_party;
import config;
import infinity_context;
import analyzer;
import stemmer;
import chinese_analyzer;
import traditional_chinese_analyzer;
import japanese_analyzer;
import korean_analyzer;
import standard_analyzer;
import ngram_analyzer;
import logger;

namespace infinity {


constexpr u64 basis = 0xCBF29CE484222325ull;
constexpr u64 prime = 0x100000001B3ull;

constexpr u64 Str2Int(const char *str, u64 last_value = basis) {
    return (*str != '\0' && *str != '-') ? Str2Int(str + 1, (*str ^ last_value) * prime) : last_value;
}

bool IcharEquals(char a, char b) { return ToLower(static_cast<int>(a)) == ToLower(static_cast<int>(b)); }

bool IEquals(std::string_view lhs, std::string_view rhs) { return std::ranges::equal(lhs, rhs, IcharEquals); }

constexpr std::string_view DANISH = "-danish";
constexpr std::string_view DUTCH = "-dutch";
constexpr std::string_view ENGLISH = "-english";
constexpr std::string_view FINNISH = "-finnish";
constexpr std::string_view FRENCH = "-french";
constexpr std::string_view GERMAN = "-german";
constexpr std::string_view HUNGARIAN = "-hungarian";
constexpr std::string_view ITALIAN = "-italian";
constexpr std::string_view NORWEGIAN = "-norwegian";
constexpr std::string_view PORTER = "-porter";
constexpr std::string_view PORTUGUESE = "-portuguese";
constexpr std::string_view ROMANIAN = "-romanian";
constexpr std::string_view RUSSIAN = "-russian";
constexpr std::string_view SPANISH = "-spanish";
constexpr std::string_view SWEDISH = "-swedish";
constexpr std::string_view TURKISH = "-turkish";

Tuple<UniquePtr<Analyzer>, Status> AnalyzerPool::GetAnalyzer(const std::string_view &name) {
    switch (Str2Int(name.data())) {
        case Str2Int(CHINESE.data()): {
            // chinese-{coarse|fine}
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
            CutGrain cut_grain = CutGrain::kCoarse;
            const char *str = name.data();
            while (*str != '\0' && *str != '-') {
                str++;
            }
            if (strcmp(str, "-fine") == 0) {
                cut_grain = CutGrain::kFine;
            }
            UniquePtr<ChineseAnalyzer> analyzer = MakeUnique<ChineseAnalyzer>(*reinterpret_cast<ChineseAnalyzer *>(prototype));
            analyzer->SetCutGrain(cut_grain);
            return {std::move(analyzer), Status::OK()};
        }
        case Str2Int(TRADITIONALCHINESE.data()): {
            // chinese-{coarse|fine}
            Analyzer *prototype = cache_[TRADITIONALCHINESE].get();
            if (prototype == nullptr) {
                String path;
                Config *config = InfinityContext::instance().config();
                if (config == nullptr) {
                    // InfinityContext has not been initialized.
                    path = "/var/infinity/resource";
                } else {
                    path = config->ResourcePath();
                }
                UniquePtr<TraditionalChineseAnalyzer> analyzer = MakeUnique<TraditionalChineseAnalyzer>(std::move(path));
                Status load_status = analyzer->Load();
                if (!load_status.ok()) {
                    return {nullptr, load_status};
                }
                prototype = analyzer.get();
                cache_[TRADITIONALCHINESE] = std::move(analyzer);
            }
            CutGrain cut_grain = CutGrain::kCoarse;
            const char *str = name.data();
            while (*str != '\0' && *str != '-') {
                str++;
            }
            if (strcmp(str, "-fine") == 0) {
                cut_grain = CutGrain::kFine;
            }
            UniquePtr<TraditionalChineseAnalyzer> analyzer =
                MakeUnique<TraditionalChineseAnalyzer>(*reinterpret_cast<TraditionalChineseAnalyzer *>(prototype));
            analyzer->SetCutGrain(cut_grain);
            return {std::move(analyzer), Status::OK()};
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
        case Str2Int(KOREAN.data()): {
            Analyzer *prototype = cache_[KOREAN].get();
            if (prototype == nullptr) {
                String path;
                Config *config = InfinityContext::instance().config();
                if (config == nullptr) {
                    // InfinityContext has not been initialized.
                    path = "/var/infinity/resource";
                } else {
                    path = config->ResourcePath();
                }
                UniquePtr<KoreanAnalyzer> analyzer = MakeUnique<KoreanAnalyzer>(std::move(path));
                Status load_status = analyzer->Load();
                if (!load_status.ok()) {
                    return {nullptr, load_status};
                }
                prototype = analyzer.get();
                cache_[KOREAN] = std::move(analyzer);
            }
            return {MakeUnique<KoreanAnalyzer>(*reinterpret_cast<KoreanAnalyzer *>(prototype)), Status::OK()};
        }
        case Str2Int(STANDARD.data()): {
            UniquePtr<StandardAnalyzer> analyzer = MakeUnique<StandardAnalyzer>();
            Language lang = STEM_LANG_ENGLISH;
            const char *str = name.data();
            while (*str != '\0' && *str != '-') {
                str++;
            }

            if (IEquals(std::string_view(str), DANISH)) {
                lang = STEM_LANG_DANISH;
            } else if (IEquals(std::string_view(str), DUTCH)) {
                lang = STEM_LANG_DUTCH;
            } else if (IEquals(std::string_view(str), ENGLISH)) {
                lang = STEM_LANG_ENGLISH;
            } else if (IEquals(std::string_view(str), FINNISH)) {
                lang = STEM_LANG_FINNISH;
            } else if (IEquals(std::string_view(str), FRENCH)) {
                lang = STEM_LANG_FRENCH;
            } else if (IEquals(std::string_view(str), GERMAN)) {
                lang = STEM_LANG_GERMAN;
            } else if (IEquals(std::string_view(str), HUNGARIAN)) {
                lang = STEM_LANG_HUNGARIAN;
            } else if (IEquals(std::string_view(str), ITALIAN)) {
                lang = STEM_LANG_ITALIAN;
            } else if (IEquals(std::string_view(str), NORWEGIAN)) {
                lang = STEM_LANG_NORWEGIAN;
            } else if (IEquals(std::string_view(str), PORTER)) {
                lang = STEM_LANG_PORT;
            } else if (IEquals(std::string_view(str), PORTUGUESE)) {
                lang = STEM_LANG_PORTUGUESE;
            } else if (IEquals(std::string_view(str), ROMANIAN)) {
                lang = STEM_LANG_ROMANIAN;
            } else if (IEquals(std::string_view(str), RUSSIAN)) {
                lang = STEM_LANG_RUSSIAN;
            } else if (IEquals(std::string_view(str), SPANISH)) {
                lang = STEM_LANG_SPANISH;
            } else if (IEquals(std::string_view(str), SWEDISH)) {
                lang = STEM_LANG_SWEDISH;
            } else if (IEquals(std::string_view(str), TURKISH)) {
                lang = STEM_LANG_TURKISH;
            }
            analyzer->InitStemmer(lang);
            return {std::move(analyzer), Status::OK()};
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
            if(std::filesystem::is_regular_file(name)) {
                // Suppose it is a customized Python script analyzer
            }
            return {nullptr, Status::AnalyzerNotFound(name.data())};
        }
    }
}

} // namespace infinity