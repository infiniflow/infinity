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
    // hidden columns' id
    constexpr SizeT COLUMN_IDENTIFIER_ROW_ID = (SizeT)(-1);
    constexpr SizeT COLUMN_IDENTIFIER_CREATE = (SizeT)(-2);
    constexpr SizeT COLUMN_IDENTIFIER_DELETE = (SizeT)(-3);
    constexpr SizeT COLUMN_IDENTIFIER_SCORE = (SizeT)(-4);
    constexpr SizeT COLUMN_IDENTIFIER_DISTANCE = (SizeT)(-5);
    constexpr StringView COLUMN_NAME_ROW_ID = "__rowid";
    constexpr StringView COLUMN_NAME_CREATE = "__create";
    constexpr StringView COLUMN_NAME_DELETE = "__delete";
    constexpr StringView COLUMN_NAME_SCORE = "__score";
    constexpr StringView COLUMN_NAME_DISTANCE = "__distance";

    // type related constants
    constexpr i64 MAX_VARCHAR_SIZE = 65536;
    constexpr i64 MAX_BLOB_SIZE = 65536L * 65536L;
    constexpr i64 MAX_BITMAP_SIZE = 65536;
    constexpr i64 EMBEDDING_LIMIT = 65536;
    constexpr auto PG_MSG_BUFFER_SIZE = 4096u;

    // column vector related constants
    constexpr i64 MAX_BLOCK_CAPACITY = 65536L;
    constexpr i64 DEFAULT_BLOCK_CAPACITY = 8192;
    constexpr i64 MIN_BLOCK_CAPACITY = 8192;
    constexpr i16 INVALID_BLOCK_ID = i16_max;
    constexpr i64 MAX_BLOCK_COUNT_IN_SEGMENT = 65536L;

    // column vector related constants
    constexpr i64 DEFAULT_VECTOR_SIZE = DEFAULT_BLOCK_CAPACITY;
    constexpr u64 INITIAL_VECTOR_CHUNK_ID = u64_max;
    constexpr u64 MIN_VECTOR_CHUNK_SIZE = 4096UL;
    constexpr u64 MAX_VECTOR_CHUNK_SIZE = 1024 * 1024UL;
    constexpr u64 MAX_VECTOR_CHUNK_COUNT = u64_max; // Each row has one chunk.
    constexpr u64 DEFAULT_FIXLEN_CHUNK_SIZE = 65536L;

    // segment related constants
    constexpr SizeT DEFAULT_SEGMENT_CAPACITY = 1024 * 8192; // 1024 * 8192 = 8M rows
    constexpr SizeT SEGMENT_OFFSET_IN_DOCID = 23;           // it should be adjusted together with DEFAULT_SEGMENT_CAPACITY
    constexpr u64 SEGMENT_MASK_IN_DOCID = 0x7FFFFF;         // it should be adjusted together with DEFAULT_SEGMENT_CAPACITY
    constexpr i32 INVALID_SEGMENT_ID = i32_max;

    // queue related constants, TODO: double check the necessary
    constexpr SizeT DEFAULT_READER_PREPARE_QUEUE_SIZE = 1024;
    constexpr SizeT DEFAULT_WRITER_PREPARE_QUEUE_SIZE = 1024;
    constexpr SizeT DEFAULT_READER_COMMIT_QUEUE_SIZE = 1024;
    constexpr SizeT DEFAULT_WRITER_COMMIT_QUEUE_SIZE = 1024;
    constexpr SizeT DEFAULT_BLOCKING_QUEUE_SIZE = 1024;

    // transaction related constants
    constexpr u64 MAX_TXN_ID = u64_max;
    constexpr u64 MAX_TIMESTAMP = u64_max;
    constexpr TxnTimeStamp UNCOMMIT_TS = u64_max;

    constexpr SizeT KB = 1024;
    constexpr SizeT MB = 1024 * KB;
    constexpr SizeT GB = 1024 * MB;

    constexpr SizeT DEFAULT_RANDOM_NAME_LEN = 10;

    constexpr SizeT DEFAULT_BASE_NUM = 2;
    constexpr SizeT DEFAULT_BASE_FILE_SIZE = 8 * 1024;
    constexpr SizeT DEFAULT_OUTLINE_FILE_MAX_SIZE = 16 * 1024 * 1024;

    constexpr SizeT DEFAULT_WAL_FILE_SIZE_THRESHOLD = 10 * 1024;
    constexpr SizeT FULL_CHECKPOINT_INTERVAL_SEC = 60;          // 60 seconds
    constexpr SizeT DELTA_CHECKPOINT_INTERVAL_SEC = 20;         // 20 seconds
    constexpr SizeT DELTA_CHECKPOINT_INTERVAL_WAL_BYTES = 1000; // wal size
    constexpr StringView WAL_FILE_TEMP_FILE = "wal.log";
    constexpr StringView WAL_FILE_PREFIX = "wal.log.";
    constexpr StringView CATALOG_FILE_DIR = "catalog";

    constexpr StringView SYSTEM_DB_NAME = "system";
    constexpr StringView DEFAULT_DB_NAME = "default";
    constexpr StringView SYSTEM_CONFIG_TABLE_NAME = "config";
    constexpr SizeT DEFAULT_PROFILER_HISTORY_SIZE = 128;

    // default hnsw parameter
    constexpr SizeT HNSW_M = 16;
    constexpr SizeT HNSW_EF_CONSTRUCTION = 200;
    constexpr SizeT HNSW_EF = 200;

    // default distance compute blas parameter
    constexpr SizeT DISTANCE_COMPUTE_BLAS_QUERY_BS = 4096;
    constexpr SizeT DISTANCE_COMPUTE_BLAS_DATABASE_BS = 1024;
}

// constexpr SizeT DEFAULT_BUFFER_SIZE = 8192;
} // namespace infinity
