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

export module default_values;

import stl;

namespace infinity {

export {
    constexpr SizeT COLUMN_IDENTIFIER_ROW_ID = (SizeT)(-1);

    constexpr i64 DEFAULT_BLOCK_CAPACITY = 8192;
    constexpr i64 DEFAULT_VECTOR_SIZE = DEFAULT_BLOCK_CAPACITY;
    // constexpr i64 MAX_VARCHAR_SIZE = 65535;
    constexpr i64 MAX_BLOB_SIZE = 65536L * 65536L;
    constexpr i64 MAX_BITMAP_SIZE = 65536;
    constexpr i64 EMBEDDING_LIMIT = 65536;
    constexpr auto PG_MSG_BUFFER_SIZE = 4096u;

    constexpr SizeT DEFAULT_SEGMENT_CAPACITY = 16384;
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
    constexpr SizeT FULL_CHECKPOINT_INTERVAL_SEC = 60;          // 60 seconds
    constexpr SizeT DELTA_CHECKPOINT_INTERVAL_SEC = 20;         // 20 seconds
    constexpr SizeT DELTA_CHECKPOINT_INTERVAL_WAL_BYTES = 1000; // wal size
    constexpr String WAL_FILE_TEMP_FILE = "wal.log";
    constexpr String WAL_FILE_PREFIX = "wal.log.";
    constexpr String CATALOG_FILE_DIR = "catalog";
}

// constexpr SizeT DEFAULT_BUFFER_SIZE = 8192;
} // namespace infinity
