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
#pragma clang diagnostic ignored "-W#pragma-messages"
#pragma clang diagnostic ignored "-Wall"

// wait for clang fix
import std.compat;

#define _GLIBCXX_FILESYSTEM 1
#define _GLIBCXX_CODECVT 1
#define _GLIBCXX_STRING 1
#define _GLIBCXX_STRING_VIEW 1
#define _GLIBCXX_VECTOR 1
#define _GLIBCXX_ALGORITHM 1
#define _GLIBCXX_IOMANIP 1
#define _GLIBCXX_LOCALE 1
#define _GLIBCXX_SSTREAM 1
#define _GLIBCXX_UTILITY 1
#define _GLIBCXX_CMATH 1
#define _GLIBCXX_MEMORY 1
#define _GLIBCXX_TUPLE 1
#define _GLIBCXX_FSTREAM 1
#define _GLIBCXX_IOSTREAM 1
#define _GLIBCXX_FUNCTIONAL 1
#define _GLIBCXX_MAP 1
#define _GLIBCXX_SET 1
#define _GLIBCXX_ITERATOR 1
#define _GLIBCXX_NUMERIC 1
#define _GLIBCXX_ATOMIC 1
#define _GLIBCXX_CHRONO 1
#define _GLIBCXX_CONDITION_VARIABLE 1
#define _GLIBCXX_MUTEX 1
#define _GLIBCXX_THREAD 1
#define _GLIBCXX_UNORDERED_MAP 1
#define _GLIBCXX_ARRAY 1
#define _GLIBCXX_FORWARD_LIST 1
#define _GLIBCXX_OPTIONAL 1
#define _GLIBCXX_RANGES 1
#define _GLIBCXX_ISTREAM 1
#define _GLIBCXX_IOS 1
#define _GLIBCXX_OSTREAM 1
#define _GLIBCXX_CHARCONV 1
#define _GLIBCXX_SHARED_MUTEX 1
#define _GLIBCXX_LIST 1
#define _GLIBCXX_UNORDERED_SET 1
#define _GLIBCXX_VARIANT 1
#define _GLIBXX_STREAMBUF 1
#define _GLIBCXX_BITSET 1
#define _GLIBCXX_DEQUE 1
#define _GLIBCXX_SPAN 1
#define _EXT_NUMERIC_TRAITS 1
#define _GLIBCXX_FUTURE 1
#define _GLIBCXX_QUEUE 1

#include <cerrno>

#include <CLI/CLI.hpp>

#include "spdlog/details/registry.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/ranges.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <nlohmann/json.hpp>
#include <simdjson.h>

#include "toml.hpp"

#include <magic_enum/magic_enum.hpp>

#include "pgm/pgm_index.hpp"
#include <parallel_hashmap/phmap.h>

#include <oatpp/network/Server.hpp>
#include <oatpp/network/tcp/server/ConnectionProvider.hpp>
#include <oatpp/web/server/HttpConnectionHandler.hpp>

#include "Python.h"
#include "miniocpp/client.h"
#include <arrow/api.h>
#include <arrow/io/file.h>
#include <parquet/arrow/writer.h>
#include <parquet/properties.h>

#include <parquet/arrow/reader.h>

#include <rocksdb/db.h>
#include <rocksdb/env.h>
#include <rocksdb/io_status.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <rocksdb/utilities/backup_engine.h>
#include <rocksdb/utilities/transaction.h>
#include <rocksdb/utilities/transaction_db.h>

#include <re2/re2.h>

#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/TDispatchProcessor.h>
#include <thrift/Thrift.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>

// #include "inc/mlas.h"

#include "new_pfordelta_compressor.h"

#include <ctpl_stl.h>

#pragma clang diagnostic pop

export module third_party;

import std;

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
export using minio::s3::PutObjectArgs;
export using minio::s3::PutObjectResponse;
export using minio::s3::BucketExistsArgs;
export using minio::s3::BucketExistsResponse;
export using minio::s3::MakeBucketArgs;
export using minio::s3::MakeBucketResponse;
export using minio::s3::ListObjectsArgs;
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
export using spdlog::flush_every;

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
export using ::nlohmann::json;
namespace detail {
export using ::nlohmann::detail::enable_if_t;
export using ::nlohmann::detail::is_basic_json;
export using ::nlohmann::detail::value_t;
} // namespace detail
} // namespace nlohmann

namespace simdjson {
export using ::simdjson::padded_string;
export using ::simdjson::simdjson_result;
export using ::simdjson::deserialize_tag;
export using ::simdjson::error_code;
export using ::simdjson::simdjson_error;
export using ondemand::parser;
export using ondemand::document;
export using ondemand::object;
export using ondemand::array;
export using ondemand::field;
export using ondemand::value;
export using ondemand::number;
export using ondemand::json_type;
export using ondemand::number_type;
} // namespace simdjson

namespace magic_enum {
export using magic_enum::underlying_type_t;
}

namespace arrow {
export using Status = arrow::Status;
export using ReadableFile = arrow::io::ReadableFile;
export using RandomAccessFile = arrow::io::RandomAccessFile;
export using Table = arrow::Table;
export template <class T>
using ArrowResult = arrow::Result<T>;

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
export using ParquetFileReaderBuilder = parquet::arrow::FileReaderBuilder;
export using ArrowWriterProperties = parquet::ArrowWriterProperties;
export using ParquetReaderProperties = parquet::ReaderProperties;
export using ParquetArrowReaderProperties = parquet::ArrowReaderProperties;
export using ::arrow::field;
export using ::arrow::schema;
export using ::arrow::boolean;
export using ::arrow::uint8;
export using ::arrow::int8;
export using ::arrow::int16;
export using ::arrow::int32;
export using ::arrow::int64;
export using ::arrow::float16;
export using ::arrow::float32;
export using ::arrow::float64;
export using ::arrow::date32;
export using ::arrow::time32;
export using ::arrow::timestamp;
export using ::arrow::utf8;
export using ::arrow::TimeUnit;
export using ::arrow::list;
export using ::arrow::fixed_size_list;
export using ::arrow::struct_;
export using ::arrow::Type;
namespace io {
export using ::arrow::io::FileOutputStream;
}
} // namespace arrow

namespace parquet {
export using ::parquet::default_writer_properties;
namespace arrow {
export using ::parquet::arrow::FileWriter;
}
}; // namespace parquet

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

export namespace rocksdb {
using Options = ::ROCKSDB_NAMESPACE::Options;
using ReadOptions = ::ROCKSDB_NAMESPACE::ReadOptions;
using Snapshot = ::ROCKSDB_NAMESPACE::Snapshot;
using Status = ::ROCKSDB_NAMESPACE::Status;
using Transaction = ::ROCKSDB_NAMESPACE::Transaction;
using TransactionDB = ::ROCKSDB_NAMESPACE::TransactionDB;
using TransactionDBOptions = ::ROCKSDB_NAMESPACE::TransactionDBOptions;
using TransactionOptions = ::ROCKSDB_NAMESPACE::TransactionOptions;
using WriteOptions = ::ROCKSDB_NAMESPACE::WriteOptions;
using FlushOptions = ::ROCKSDB_NAMESPACE::FlushOptions;
using Iterator = ::ROCKSDB_NAMESPACE::Iterator;
using BackupEngine = ::ROCKSDB_NAMESPACE::BackupEngine;
using BackupInfo = ::ROCKSDB_NAMESPACE::BackupInfo;
using BackupEngineReadOnly = ::ROCKSDB_NAMESPACE::BackupEngineReadOnly;
using BackupEngineOptions = ::ROCKSDB_NAMESPACE::BackupEngineOptions;
using Env = ::ROCKSDB_NAMESPACE::Env;
using IOStatus = ::ROCKSDB_NAMESPACE::IOStatus;
using Slice = ::ROCKSDB_NAMESPACE::Slice;
using AssociativeMergeOperator = ::ROCKSDB_NAMESPACE::AssociativeMergeOperator;
using MergeOperator = ::ROCKSDB_NAMESPACE::MergeOperator;
// using MergeOperators = ::ROCKSDB_NAMESPACE::MergeOperators;
using Logger = ::ROCKSDB_NAMESPACE::Logger;
} // namespace rocksdb

namespace rocksdb {
export using ::rocksdb::DestroyDB;
export using ::rocksdb::EventListener;
export using ::rocksdb::DB;
export using ::rocksdb::FlushJobInfo;
export using ::rocksdb::CompactionJobInfo;
} // namespace rocksdb

namespace re2 {
export using ::re2::RE2;
};

namespace apache {
namespace thrift {
export using ::apache::thrift::TConnectionInfo;
export using ::apache::thrift::TDispatchProcessor;
export using ::apache::thrift::TProcessorContextFreer;
export using ::apache::thrift::TBase;
export using ::apache::thrift::TApplicationException;
export using ::apache::thrift::TConfiguration;
namespace protocol {
export using ::apache::thrift::protocol::TProtocol;
export using ::apache::thrift::protocol::TOutputRecursionTracker;
export using ::apache::thrift::protocol::TTransport;
export using ::apache::thrift::protocol::TBinaryProtocolFactory;
export using ::apache::thrift::protocol::TBinaryProtocol;
} // namespace protocol
namespace server {
export using ::apache::thrift::server::TServer;
export using ::apache::thrift::server::TThreadPoolServer;
} // namespace server
namespace transport {
export using ::apache::thrift::transport::TServerSocket;
export using ::apache::thrift::transport::TBufferedTransportFactory;
export using ::apache::thrift::transport::TSocket;
export using ::apache::thrift::transport::TBufferedTransport;
export using ::apache::thrift::transport::TTransportException;
export using ::apache::thrift::transport::TTransportFactory;
} // namespace transport
namespace concurrency {
export using ::apache::thrift::concurrency::ThreadFactory;
export using ::apache::thrift::concurrency::ThreadManager;
} // namespace concurrency
} // namespace thrift
} // namespace apache

export using NewPForDeltaCompressor = indexlib::NewPForDeltaCompressor;

// mlas
// export using CBLAS_TRANSPOSE

namespace ctpl {
export using ::ctpl::thread_pool;
}

} // namespace infinity
