//
// Created by jinhai on 23-10-14.
//

module;

#include <string>

export module default_values;

import stl;

namespace infinity {

export {
constexpr i64 DEFAULT_VECTOR_SIZE = 8192;
//constexpr i64 MAX_VARCHAR_SIZE = 65535;
constexpr i64 MAX_BLOB_SIZE = 65536L * 65536L;
constexpr i64 MAX_BITMAP_SIZE = 65536;
constexpr i64 EMBEDDING_LIMIT = 65536;
constexpr auto PG_MSG_BUFFER_SIZE = 4096u;

constexpr SizeT DEFAULT_SEGMENT_CAPACITY = 128;
constexpr SizeT DEFAULT_SEGMENT_ROW = 1024 * 1024;
constexpr SizeT DEFAULT_READER_PREPARE_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_WRITER_PREPARE_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_READER_COMMIT_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_WRITER_COMMIT_QUEUE_SIZE = 1024;

constexpr u64 MAX_TXN_ID = u64_max;
constexpr u64 MAX_TIMESTAMP = u64_max;

constexpr TxnTimeStamp UNCOMMIT_TS = u64_max;
constexpr i32 INVALID_SEGMENT_ID = i32_max;
constexpr i16 INVALID_BLOCK_ID = i16_max;

constexpr SizeT KB = 1024;
constexpr SizeT MB = 1024 * KB;
constexpr SizeT GB = 1024 * MB;

constexpr SizeT DEFAULT_RANDOM_SEGMENT_NAME_LEN = 10;
constexpr SizeT DEFAULT_OUTLINE_FILE_MAX_SIZE = 16 * 1024 * 1024;

constexpr SizeT DEFAULT_WAL_FILE_SIZE_THRESHOLD = 10 * 1024;
constexpr SizeT FULL_CHECKPOINT_TIME_INTERVAL = 12 * 60 * 60 * 1000; // 12 hours
constexpr SizeT FULL_CHECKPOINT_TXN_INTERVAL = 100 * 1000;           // txn count
constexpr SizeT DELTA_CHECKPOINT_TIME_INTERVAL = 20000;              // 20 seconds
constexpr SizeT DELTA_CHECKPOINT_TXN_INTERVAL = 3;                   // txn count
constexpr std::string WAL_FILE_TEMP_FILE = "wal.log";
constexpr std::string WAL_FILE_PREFIX = "wal.log.";

}

// constexpr SizeT DEFAULT_BUFFER_SIZE = 8192;
} // namespace infinity
