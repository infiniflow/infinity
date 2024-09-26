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

#include "Python.h"
#include "arrow/api.h"
#include "arrow/array.h"
#include "arrow/array/array_base.h"
#include "arrow/array/builder_primitive.h"
#include "arrow/chunked_array.h"
#include "arrow/io/api.h"
#include "arrow/io/caching.h"
#include "arrow/io/file.h"
#include "arrow/memory_pool.h"
#include "arrow/record_batch.h"
#include "arrow/result.h"
#include "arrow/status.h"
#include "arrow/table.h"
#include "arrow/type_fwd.h"
#include "miniocpp/client.h"
#include "parquet/arrow/reader.h"
#include "parquet/arrow/writer.h"
#include <arrow/array/array_nested.h>
#include <arrow/array/array_primitive.h>
#include <arrow/io/interfaces.h>
#include <arrow/type.h>

#pragma clang diagnostic pop

export module third_party;

namespace minio {
    namespace s3 {
        export using minio::s3::BaseUrl;
        export using minio::s3::Client;
        export using minio::s3::RemoveObjectArgs;
        export using minio::s3::RemoveObjectResponse;
        export using minio::s3::CopyObjectArgs;
        export using minio::s3::CopySource;
        export using minio::s3::CopyObjectResponse;
        export using minio::s3::DownloadObjectArgs;
        export using minio::s3::DownloadObjectResponse;
        export using minio::s3::UploadObjectArgs;
        export using minio::s3::UploadObjectResponse;
    } // namespace s3

    namespace creds {
        export using minio::creds::StaticProvider;
    } // namespace creds

} // namespace minio

namespace fmt {

export using fmt::format;
export using fmt::print;
export using fmt::join;

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
export using moodycamel::BlockingConcurrentQueue;
} // namespace moodycamel

namespace arrow {
export using Status = arrow::Status;
export using ReadableFile = arrow::io::ReadableFile;
export using RandomAccessFile = arrow::io::RandomAccessFile;
export using Table = arrow::Table;

export using ChunkedArray = arrow::ChunkedArray;
export using ArrayBuilder = arrow::ArrayBuilder;
export using Array = arrow::Array;
export using BooleanArray = arrow::BooleanArray;
export using UInt8Array = arrow::UInt8Array;
export using Int8Array = arrow::Int8Array;
export using Int16Array = arrow::Int16Array;
export using Int32Array = arrow::Int32Array;
export using Int64Array = arrow::Int64Array;
export using HalfFloatArray = arrow::HalfFloatArray;
export using FloatArray = arrow::FloatArray;
export using DoubleArray = arrow::DoubleArray;
export using Decimal128Array = arrow::Decimal128Array;
export using Date32Array = arrow::Date32Array;
export using Time32Array = arrow::Time32Array;
export using TimestampArray = arrow::TimestampArray;
export using DurationArray = arrow::DurationArray;
export using StringArray = arrow::StringArray;
export using ListArray = arrow::ListArray;
export using FixedSizeListArray = arrow::FixedSizeListArray;
export using BinaryArray = arrow::BinaryArray;
export using StructArray = arrow::StructArray;

export using BooleanBuilder = arrow::BooleanBuilder;
export using UInt8Builder = arrow::UInt8Builder;
export using Int8Builder = arrow::Int8Builder;
export using Int16Builder = arrow::Int16Builder;
export using Int32Builder = arrow::Int32Builder;
export using Int64Builder = arrow::Int64Builder;
export using HalfFloatBuilder = arrow::HalfFloatBuilder;
export using FloatBuilder = arrow::FloatBuilder;
export using DoubleBuilder = arrow::DoubleBuilder;
export using Decimal128Builder = arrow::Decimal128Builder;
export using Date32Builder = arrow::Date32Builder;
export using Time32Builder = arrow::Time32Builder;
export using TimestampBuilder = arrow::TimestampBuilder;
export using DurationBuilder = arrow::DurationBuilder;
export using StringBuilder = arrow::StringBuilder;
export using ListBuilder = arrow::ListBuilder;
export using FixedSizeListBuilder = arrow::FixedSizeListBuilder;
export using StructBuilder = arrow::StructBuilder;

export using RecordBatchReader = arrow::RecordBatchReader;
export using RecordBatch = arrow::RecordBatch;
export using MemoryPool = arrow::MemoryPool;
export MemoryPool *DefaultMemoryPool() { return arrow::default_memory_pool(); }

export using DataType = arrow::DataType;
export using Field = arrow::Field;
export using FieldVector = arrow::FieldVector;
export using Schema = arrow::Schema;
export using ParquetFileReader = parquet::arrow::FileReader;
export using ParquetFileWriter = parquet::arrow::FileWriter;
export using ArrowWriterProperties = parquet::ArrowWriterProperties;
} // namespace arrow

namespace parquet {
export ::arrow::Status
OpenFile(std::shared_ptr<::arrow::RandomAccessFile> file, ::arrow::MemoryPool *pool, std::unique_ptr<parquet::arrow::FileReader> *reader) {
    return parquet::arrow::OpenFile(file, pool, reader);
}
} // namespace parquet

namespace infinity {

// spdlog
export enum class LogLevel {
    kTrace,
    kDebug,
    kInfo,
    kWarning,
    kError,
    kCritical,
    kOff,
};

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
        case LogLevel::kOff: {
            return "Off";
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
        case LogLevel::kOff:
            return spdlog::details::registry::instance().set_level(spdlog::level::level_enum::off);
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

// Python
export using PyObject = PyObject;

} // namespace infinity
