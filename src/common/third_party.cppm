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

#include "CLI11.hpp"

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
#include "utils/index_utils.hpp"
#include "utils/noncopyable.hpp"
#include "utils/object_pool.hpp"
#include "utils/text_format.hpp"
#include "utils/type_info.hpp"

#include "parallel_hashmap/phmap.h"
#include "pgm/pgm_index.hpp"

#include "vespalib/btree/btree.h"
#include "vespalib/btree/btree.hpp"
#include "vespalib/btree/btreeroot.h"
#include "vespalib/btree/btreeroot.hpp"
#include "vespalib/btree/btreestore.h"
#include "vespalib/btree/btreestore.hpp"

export module third_party;

namespace fmt {

    export using fmt::format;
    export using fmt::print;
}

namespace spdlog {
    export using spdlog::shutdown;
    export using spdlog::logger;
    export using spdlog::sink_ptr;

    namespace sinks {
        export using spdlog::sinks::stdout_color_sink_mt;
        export using spdlog::sinks::rotating_file_sink_mt;
    }

    namespace details {
        export using spdlog::details::registry;
    }
}

namespace CLI {
export using CLI::App;
export using CLI::ParseError;
export using CLI::FileError;
export using CLI::ConversionError;
export using CLI::ValidationError;
export using CLI::RequiredError;
export using CLI::RequiresError;
export using CLI::ExcludesError;
export using CLI::ExtrasError;
export using CLI::ConfigError;
export using CLI::InvalidError;
export using CLI::HorribleError;
export using CLI::OptionNotFound;
export using CLI::ArgumentMismatch;
} // namespace CLI

namespace cxxopts {
    // cxxopts
    export using cxxopts::Options;
    export using cxxopts::Value;

    namespace values {
        export using cxxopts::values::standard_value;
    }

    export using cxxopts::ParseResult;
}

namespace toml {
    // Toml parser
    export using toml::table;
    export using toml::parse_file;
}

namespace nlohmann {
    export using nlohmann::json;
}

namespace magic_enum {
    export using magic_enum::underlying_type_t;
}

namespace moodycamel {
    export using moodycamel::ConcurrentQueue;
}

namespace irs {
export using irs::Norm;
export using irs::Norm2;
namespace index_utils {
export using irs::index_utils::ConsolidateByOrder;
export using irs::index_utils::ConsolidateCount;
export using irs::index_utils::MakePolicy;
} // namespace index_utils
} // namespace irs

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

export template <typename KeyT,
                 typename DataT,
                 typename AggrT = vespalib::btree::NoAggregated,
                 typename CompareT = std::less<KeyT>,
                 typename TraitsT = vespalib::btree::BTreeDefaultTraits,
                 typename AggrCalcT = vespalib::btree::NoAggrCalc>
using Btree = vespalib::btree::BTree<KeyT, DataT, AggrT, CompareT, TraitsT, AggrCalcT>;

export template <typename KeyT,
                 typename DataT,
                 typename AggrT = vespalib::btree::NoAggregated,
                 typename CompareT = std::less<KeyT>,
                 typename TraitsT = vespalib::btree::BTreeDefaultTraits,
                 typename AggrCalcT = vespalib::btree::NoAggrCalc>
using BtreeRoot = vespalib::btree::BTreeRoot<KeyT, DataT, AggrT, CompareT, TraitsT, AggrCalcT>;

export template <typename KeyT,
                 typename DataT,
                 typename AggrT = vespalib::btree::NoAggregated,
                 typename CompareT = std::less<KeyT>,
                 typename TraitsT = vespalib::btree::BTreeDefaultTraits,
                 typename AggrCalcT = vespalib::btree::NoAggrCalc>
using BtreeStore = vespalib::btree::BTreeStore<KeyT, DataT, AggrT, CompareT, TraitsT, AggrCalcT>;

} // namespace infinity
