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

#include <spdlog/details/registry.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

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

export namespace minio {
namespace s3 {
using ::minio::s3::BaseUrl;
using ::minio::s3::BucketExistsArgs;
using ::minio::s3::BucketExistsResponse;
using ::minio::s3::Client;
using ::minio::s3::CopyObjectArgs;
using ::minio::s3::CopyObjectResponse;
using ::minio::s3::CopySource;
using ::minio::s3::DownloadObjectArgs;
using ::minio::s3::DownloadObjectResponse;
using ::minio::s3::ListObjectsArgs;
using ::minio::s3::MakeBucketArgs;
using ::minio::s3::MakeBucketResponse;
using ::minio::s3::PutObjectArgs;
using ::minio::s3::PutObjectResponse;
using ::minio::s3::RemoveObjectArgs;
using ::minio::s3::RemoveObjectResponse;
using ::minio::s3::UploadObjectArgs;
using ::minio::s3::UploadObjectResponse;
} // namespace s3

namespace creds {
using ::minio::creds::StaticProvider;
} // namespace creds

} // namespace minio

export namespace fmt {

using ::fmt::format;
using ::fmt::join;
using ::fmt::print;

} // namespace fmt

export namespace spdlog {
using ::spdlog::flush_every;
using ::spdlog::logger;
using ::spdlog::shutdown;
using ::spdlog::sink_ptr;

namespace sinks {
using ::spdlog::sinks::rotating_file_sink_mt;
using ::spdlog::sinks::stdout_color_sink_mt;
} // namespace sinks

namespace details {
using ::spdlog::details::registry;
}

namespace level {
using ::spdlog::level::level_enum;
}
} // namespace spdlog

export namespace CLI {
using ::CLI::App;
using ::CLI::ArgumentMismatch;
using ::CLI::CheckedTransformer;
using ::CLI::ConfigError;
using ::CLI::ConversionError;
using ::CLI::ExcludesError;
using ::CLI::ExtrasError;
using ::CLI::FileError;
using ::CLI::HorribleError;
using ::CLI::ignore_case;
using ::CLI::InvalidError;
using ::CLI::OptionNotFound;
using ::CLI::ParseError;
using ::CLI::RequiredError;
using ::CLI::RequiresError;
using ::CLI::ValidationError;
} // namespace CLI

export namespace toml {
// Toml parser
using ::toml::parse_file;
using ::toml::table;
} // namespace toml

export namespace nlohmann {
using nlohmann::json;
namespace detail {
using ::nlohmann::detail::enable_if_t;
using ::nlohmann::detail::is_basic_json;
using ::nlohmann::detail::value_t;
} // namespace detail
} // namespace nlohmann

export namespace simdjson {
using ondemand::array;
using ondemand::document;
using ondemand::field;
using ondemand::json_type;
using ondemand::number;
using ondemand::number_type;
using ondemand::object;
using ondemand::parser;
using ondemand::value;
using ::simdjson::deserialize_tag;
using ::simdjson::error_code;
using ::simdjson::padded_string;
using ::simdjson::simdjson_error;
using ::simdjson::simdjson_result;
} // namespace simdjson

export namespace magic_enum {
using magic_enum::underlying_type_t;
}

export namespace arrow {
using Status = arrow::Status;
using ReadableFile = arrow::io::ReadableFile;
using RandomAccessFile = arrow::io::RandomAccessFile;
using Table = arrow::Table;
template <class T>
using ArrowResult = arrow::Result<T>;

using ChunkedArray = arrow::ChunkedArray;
using ArrayBuilder = arrow::ArrayBuilder;
using Array = arrow::Array;
using BooleanArray = arrow::BooleanArray;
using UInt8Array = arrow::UInt8Array;
using Int8Array = arrow::Int8Array;
using Int16Array = arrow::Int16Array;
using Int32Array = arrow::Int32Array;
using Int64Array = arrow::Int64Array;
using HalfFloatArray = arrow::HalfFloatArray;
using FloatArray = arrow::FloatArray;
using DoubleArray = arrow::DoubleArray;
using Decimal128Array = arrow::Decimal128Array;
using Date32Array = arrow::Date32Array;
using Time32Array = arrow::Time32Array;
using TimestampArray = arrow::TimestampArray;
using DurationArray = arrow::DurationArray;
using StringArray = arrow::StringArray;
using ListArray = arrow::ListArray;
using FixedSizeListArray = arrow::FixedSizeListArray;
using BinaryArray = arrow::BinaryArray;
using StructArray = arrow::StructArray;

using BooleanBuilder = arrow::BooleanBuilder;
using UInt8Builder = arrow::UInt8Builder;
using Int8Builder = arrow::Int8Builder;
using Int16Builder = arrow::Int16Builder;
using Int32Builder = arrow::Int32Builder;
using Int64Builder = arrow::Int64Builder;
using HalfFloatBuilder = arrow::HalfFloatBuilder;
using FloatBuilder = arrow::FloatBuilder;
using DoubleBuilder = arrow::DoubleBuilder;
using Decimal128Builder = arrow::Decimal128Builder;
using Date32Builder = arrow::Date32Builder;
using Time32Builder = arrow::Time32Builder;
using TimestampBuilder = arrow::TimestampBuilder;
using DurationBuilder = arrow::DurationBuilder;
using StringBuilder = arrow::StringBuilder;
using ListBuilder = arrow::ListBuilder;
using FixedSizeListBuilder = arrow::FixedSizeListBuilder;
using StructBuilder = arrow::StructBuilder;

using RecordBatchReader = arrow::RecordBatchReader;
using RecordBatch = arrow::RecordBatch;
using MemoryPool = arrow::MemoryPool;
MemoryPool *DefaultMemoryPool() { return arrow::default_memory_pool(); }

using DataType = arrow::DataType;
using Field = arrow::Field;
using FieldVector = arrow::FieldVector;
using Schema = arrow::Schema;
using ParquetFileReader = parquet::arrow::FileReader;
using ParquetFileWriter = parquet::arrow::FileWriter;
using ParquetFileReaderBuilder = parquet::arrow::FileReaderBuilder;
using ArrowWriterProperties = parquet::ArrowWriterProperties;
using ParquetReaderProperties = parquet::ReaderProperties;
using ParquetArrowReaderProperties = parquet::ArrowReaderProperties;
using ::arrow::boolean;
using ::arrow::date32;
using ::arrow::field;
using ::arrow::fixed_size_list;
using ::arrow::float16;
using ::arrow::float32;
using ::arrow::float64;
using ::arrow::int16;
using ::arrow::int32;
using ::arrow::int64;
using ::arrow::int8;
using ::arrow::list;
using ::arrow::schema;
using ::arrow::struct_;
using ::arrow::time32;
using ::arrow::timestamp;
using ::arrow::TimeUnit;
using ::arrow::Type;
using ::arrow::uint8;
using ::arrow::utf8;
namespace io {
using arrow::io::FileOutputStream;
}
} // namespace arrow

export namespace parquet {
using parquet::default_writer_properties;
namespace arrow {
using parquet::arrow::FileWriter;
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
using ColumnFamilyDescriptor = ::ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;

using ::rocksdb::CompactionJobInfo;
using ::rocksdb::DB;
using ::rocksdb::DestroyDB;
using ::rocksdb::EventListener;
using ::rocksdb::FlushJobInfo;
} // namespace rocksdb

export namespace re2 {
using ::re2::RE2;
};

export namespace apache {
namespace thrift {
using ::apache::thrift::TApplicationException;
using ::apache::thrift::TBase;
using ::apache::thrift::TConfiguration;
using ::apache::thrift::TConnectionInfo;
using ::apache::thrift::TDispatchProcessor;
using ::apache::thrift::TProcessorContextFreer;
namespace protocol {
using ::apache::thrift::protocol::TBinaryProtocol;
using ::apache::thrift::protocol::TBinaryProtocolFactory;
using ::apache::thrift::protocol::TOutputRecursionTracker;
using ::apache::thrift::protocol::TProtocol;
using ::apache::thrift::protocol::TTransport;
} // namespace protocol
namespace server {
using ::apache::thrift::server::TServer;
using ::apache::thrift::server::TThreadPoolServer;
} // namespace server
namespace transport {
using ::apache::thrift::transport::TBufferedTransport;
using ::apache::thrift::transport::TBufferedTransportFactory;
using ::apache::thrift::transport::TServerSocket;
using ::apache::thrift::transport::TSocket;
using ::apache::thrift::transport::TTransportException;
using ::apache::thrift::transport::TTransportFactory;
} // namespace transport
namespace concurrency {
using ::apache::thrift::concurrency::ThreadFactory;
using ::apache::thrift::concurrency::ThreadManager;
} // namespace concurrency
} // namespace thrift
} // namespace apache

export using NewPForDeltaCompressor = indexlib::NewPForDeltaCompressor;

// mlas
// export using CBLAS_TRANSPOSE

export namespace ctpl {
using ::ctpl::thread_pool;
}

} // namespace infinity
