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
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-W#pragma-messages"
#pragma clang diagnostic ignored "-Wpessimizing-move"

#include "CLI11.hpp"

#include "spdlog/details/registry.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "json.hpp"

#include "toml.hpp"

#include "magic_enum.hpp"

#include "blockingconcurrentqueue.h"
#include "concurrentqueue.h"

#include "parallel_hashmap/phmap.h"
#include "pgm/pgm_index.hpp"

#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#pragma clang diagnostic pop

export module third_party;

namespace fmt {

export using fmt::format;
export using fmt::print;

} // namespace fmt

namespace spdlog {
export using spdlog::shutdown;
export using spdlog::logger;
export using spdlog::sink_ptr;

namespace sinks {
export using spdlog::sinks::stdout_color_sink_mt;
export using spdlog::sinks::rotating_file_sink_mt;
} // namespace sinks

namespace details {
export using spdlog::details::registry;
}

namespace level {
export using spdlog::level::level_enum;
}
} // namespace spdlog

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
export using CLI::CheckedTransformer;
export using CLI::ignore_case;
} // namespace CLI

namespace toml {
// Toml parser
export using toml::table;
export using toml::parse_file;
} // namespace toml

namespace nlohmann {
export using nlohmann::json;
}

namespace magic_enum {
export using magic_enum::underlying_type_t;
}

namespace moodycamel {
export using moodycamel::ConcurrentQueue;
}

namespace infinity {

// spdlog
export enum class LogLevel { kTrace, kDebug, kInfo, kWarning, kError, kCritical };

export std::string LogLevel2Str(LogLevel log_level) {
    switch (log_level) {

        case LogLevel::kTrace: {
            return "Trace";
        }
        case LogLevel::kDebug: {
            return "Debug";
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
        case LogLevel::kCritical: {
            return "Critical";
        }
    }
}

export void SetLogLevel(LogLevel log_level) {
    switch (log_level) {
        case LogLevel::kTrace:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::trace);
        case LogLevel::kDebug:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::debug);
        case LogLevel::kInfo:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::info);
        case LogLevel::kWarning:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::warn);
        case LogLevel::kError:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::err);
        case LogLevel::kCritical:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::critical);
    }
}

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

// Http
export using HttpRequestHandler = oatpp::web::server::HttpRequestHandler;
export using HttpRouter = oatpp::web::server::HttpRouter;
export using HttpConnectionHandler = oatpp::web::server::HttpConnectionHandler;
export using HttpConnectionProvider = oatpp::network::tcp::server::ConnectionProvider;
export using WebServer = oatpp::network::Server;
export using WebEnvironment = oatpp::base::Environment;
export using WebAddress = oatpp::network::Address;
export using HTTPStatus = oatpp::web::protocol::http::Status;

} // namespace infinity
