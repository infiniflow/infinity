//
// Created by JinHai on 2022/11/6.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/strings.h"
#include <limits>

namespace infinity {

constexpr i64 DEFAULT_VECTOR_SIZE = 8192;
constexpr i64 MAX_VARCHAR_SIZE = 65535;
constexpr i64 MAX_BLOB_SIZE = 65536L * 65536L;
constexpr i64 MAX_BITMAP_SIZE = 65536;
constexpr i64 EMBEDDING_LIMIT = 65536;

constexpr SizeT DEFAULT_SEGMENT_CAPACITY = 128;
constexpr SizeT DEFAULT_SEGMENT_ROW = 1024 * 1024;
constexpr SizeT DEFAULT_READER_PREPARE_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_WRITER_PREPARE_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_READER_COMMIT_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_WRITER_COMMIT_QUEUE_SIZE = 1024;

constexpr u64 MAX_TXN_ID = std::numeric_limits<u64>::max();
constexpr u64 MAX_TIMESTAMP = std::numeric_limits<u64>::max();

constexpr TxnTimeStamp UNCOMMIT_TS = std::numeric_limits<TxnTimeStamp>::max();
constexpr i32 INVALID_SEGMENT_ID = std::numeric_limits<i32>::max();
constexpr i16 INVALID_BLOCK_ID = std::numeric_limits<i16>::max();

constexpr SizeT KB = 1024;
constexpr SizeT MB = 1024 * KB;
constexpr SizeT GB = 1024 * MB;

constexpr SizeT DEFAULT_RANDOM_SEGMENT_NAME_LEN = 10;
constexpr SizeT DEFAULT_OUTLINE_FILE_MAX_SIZE = 16 * 1024 * 1024;

constexpr SizeT DEFAULT_WAL_FILE_SIZE_THRESHOLD = 10 * 1024;
constexpr String WAL_FILE_TEMP_FILE = "wal.log";
constexpr String WAL_FILE_PREFIX = "wal.log.";

// constexpr SizeT DEFAULT_BUFFER_SIZE = 8192;
} // namespace infinity
