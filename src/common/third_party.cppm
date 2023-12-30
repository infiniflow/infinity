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

#include "cxxopts.hpp"
#include "spdlog/details/registry.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "json.hpp"

#include "toml.hpp"

#include "magic_enum.hpp"

#include "concurrentqueue.h"
#include "blockingconcurrentqueue.h"

#include "analysis/analyzers.hpp"
#include "analysis/jieba_analyzer.hpp"
#include "analysis/segmentation_token_stream.hpp"
#include "analysis/token_attributes.hpp"
#include "analysis/token_streams.hpp"
#include "index/index_writer.hpp"
#include "index/norm.hpp"
#include "utils/noncopyable.hpp"
#include "utils/object_pool.hpp"
#include "utils/type_info.hpp"

#include "parallel_hashmap/phmap.h"
#include "pgm/pgm_index.hpp"

export module third_party;

namespace infinity {

// spdlog
export enum class LogLevel { kTrace, kInfo, kWarning, kError, kFatal };

export std::string LogLevel2Str(LogLevel log_level) {
    switch (log_level) {

        case LogLevel::kTrace: {
            return "Trace";
        }
        case LogLevel::kInfo: {
            return "Info";
        }
        case LogLevel::kWarning: {
            return "Warning";
        }
        case LogLevel::kError: {
            return "Error";
        }
        case LogLevel::kFatal: {
            return "Fatal";
        }
    }
}

export template <typename... T>
FMT_INLINE void Printf(fmt::format_string<T...> fmt, T &&...args) {
    const auto &vargs = fmt::make_format_args(args...);
    return fmt::detail::is_utf8() ? vprint(fmt, vargs) : fmt::detail::vprint_mojibake(stdout, fmt, vargs);
}

export template <typename... T>
FMT_NODISCARD FMT_INLINE auto Format(fmt::format_string<T...> fmt, T &&...args) -> std::string {
    return vformat(fmt, fmt::make_format_args(args...));
}

export void RegisterLogger(const std::shared_ptr<spdlog::logger> &logger) { return spdlog::details::registry::instance().register_logger(logger); }

export void SetLogLevel(LogLevel log_level) {
    switch (log_level) {
        case LogLevel::kTrace:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::trace);
        case LogLevel::kInfo:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::info);
        case LogLevel::kWarning:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::warn);
        case LogLevel::kError:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::err);
        case LogLevel::kFatal:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::critical);
    }
}

export void ShutdownLogger() { spdlog::shutdown(); }

export using spd_sink_ptr = spdlog::sink_ptr;
export using spd_stdout_color_sink = spdlog::sinks::stdout_color_sink_mt;
export using spd_rotating_file_sink = spdlog::sinks::rotating_file_sink_mt;

export using spd_logger = spdlog::logger;
export using spd_log_level = spdlog::level::level_enum;

export template <typename T>
inline void spd_log(const T &msg, spd_log_level log_level) {
    switch (log_level) {
        case spdlog::level::trace: {
            return spdlog::default_logger_raw()->trace(msg);
        }
        case spdlog::level::info: {
            return spdlog::default_logger_raw()->info(msg);
        }
        case spdlog::level::warn: {
            return spdlog::default_logger_raw()->warn(msg);
        }
        case spdlog::level::err: {
            return spdlog::default_logger_raw()->error(msg);
        }
        case spdlog::level::critical: {
            return spdlog::default_logger_raw()->critical(msg);
        }
        default:
            assert(false);
    }
}

// cxxopts
export using CxxOptions = cxxopts::Options;

export template <typename T>
std::shared_ptr<cxxopts::Value> cxx_value() {
    return std::make_shared<cxxopts::values::standard_value<T>>();
}

export using ParseResult = cxxopts::ParseResult;

// Toml parser
export using TomlTable = toml::table;
//
export TomlTable TomlParseFile(const std::string &file_path) { return toml::parse_file(file_path); }

// Returns integer value from enum value.
export template <typename E>
constexpr auto EnumInteger(E value) noexcept -> magic_enum::detail::enable_if_t<E, magic_enum::underlying_type_t<E>> {
    return static_cast<magic_enum::underlying_type_t<E>>(value);
}

// Json Parser
export using Json = nlohmann::json;

// ConcurrentQueue

export template<typename T>
using ConcurrentQueue = moodycamel::ConcurrentQueue<T>;

export template<typename T>
using BlockingConcurrentQueue = moodycamel::BlockingConcurrentQueue<T>;

export using Features = irs::features_t;
export using IndexFeatures = irs::IndexFeatures;
export using TokenStream = irs::token_stream;
export using StringTokenStream = irs::string_token_stream;
export using NumericTokenStream = irs::numeric_token_stream;
export using DataOutput = irs::data_output;
export using IRSAnalyzer = irs::analysis::analyzer;
export using IRSJiebaAnalyzer = irs::analysis::jieba_analyzer;
export using IRSSegmentationAnalyzer = irs::analysis::segmentation_token_stream;
export using IRSIndexWriter = irs::IndexWriter;
export using IRSIndexReader = irs::IndexReader;
export using IRSDirectory = irs::directory;
export using IRSDirectoryReader = irs::DirectoryReader;
export using IRSSubReader = irs::SubReader;
export using IRSIndexWriterOptions = irs::IndexWriterOptions;
export using IRSIndexReaderOptions = irs::IndexReaderOptions;
export using Norm = irs::Norm2;
export using GranularityPrefix = irs::granularity_prefix;
export template <typename T>
using IRSType = irs::type<T>;
export using IRSTypeInfo = irs::type_info;
export using OpenMode = irs::OpenMode;
export inline void IRSWriteString(DataOutput &out, const char *s, size_t len) { return irs::write_string(out, s, len); }
export inline void IRSWriteZVlong(DataOutput &out, int64_t v) { return irs::write_zvlong(out, v); }

export template <class T>
using HashDefaultHash = phmap::priv::hash_default_hash<T>;
export template <class T>
using HashDefaultEQ = phmap::priv::hash_default_eq<T>;
export template <typename K, typename V>
using PHPair = phmap::priv::Pair<const K, V>;
export template <class T>
using PHAlloc = phmap::priv::Allocator<T>;
export template <class K,
                 class V,
                 class Hash = HashDefaultHash<K>,
                 class Eq = HashDefaultEQ<K>,
                 class Alloc = PHAlloc<PHPair<const K, V>>> // alias for std::allocator
using FlatHashMap = phmap::flat_hash_map<K, V, Hash, Eq, Alloc>;

export template <class T,
                 class Hash = HashDefaultHash<T>,
                 class Eq = HashDefaultEQ<T>,
                 class Alloc = PHAlloc<T>> // alias for std::allocator
using FlatHashSet = phmap::flat_hash_set<T, Hash, Eq, Alloc>;

export template <typename K, size_t Epsilon = 64, size_t EpsilonRecursive = 4, typename Floating = float>
using PGMIndex = pgm::PGMIndex<K, Epsilon, EpsilonRecursive, Floating>;

} // namespace infinity
