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
    // infinity related constants
    constexpr u64 MAX_IDENTIFIER_NAME_LENGTH = 65536;

    // hidden columns' id
    constexpr ColumnID COLUMN_IDENTIFIER_ROW_ID = (ColumnID)(std::numeric_limits<u64>::max() - 1);
    constexpr ColumnID COLUMN_IDENTIFIER_CREATE = (ColumnID)(std::numeric_limits<u64>::max() - 2);
    constexpr ColumnID COLUMN_IDENTIFIER_DELETE = (ColumnID)(std::numeric_limits<u64>::max() - 3);
    constexpr ColumnID COLUMN_IDENTIFIER_SCORE = (ColumnID)(std::numeric_limits<u64>::max() - 4);
    constexpr ColumnID COLUMN_IDENTIFIER_DISTANCE = (ColumnID)(std::numeric_limits<u64>::max() - 5);
    constexpr std::string_view COLUMN_NAME_ROW_ID = "__rowid";
    constexpr std::string_view COLUMN_NAME_CREATE = "__create";
    constexpr std::string_view COLUMN_NAME_DELETE = "__delete";
    constexpr std::string_view COLUMN_NAME_SCORE = "__score";
    constexpr std::string_view COLUMN_NAME_DISTANCE = "__distance";

    // type related constants
    constexpr i64 MAX_VARCHAR_SIZE = 65536;
    constexpr i64 MAX_BLOB_SIZE = 65536L * 65536L;
    constexpr i64 MAX_BITMAP_SIZE = 65536;
    constexpr i64 EMBEDDING_LIMIT = 16384;
    constexpr auto PG_MSG_BUFFER_SIZE = 4096u;

    // column vector related constants
    constexpr i64 MAX_BLOCK_CAPACITY = 65536L;
    constexpr i64 DEFAULT_BLOCK_CAPACITY = 8192;
    constexpr u64 BLOCK_OFFSET_SHIFT = 13;    // it should be adjusted together with DEFAULT_BLOCK_CAPACITY
    constexpr u64 BLOCK_OFFSET_MASK = 0x1FFF; // it should be adjusted together with DEFAULT_BLOCK_CAPACITY
    constexpr i64 MIN_BLOCK_CAPACITY = 8192;
    constexpr i16 INVALID_BLOCK_ID = std::numeric_limits<i16>::max();
    constexpr i64 MAX_BLOCK_COUNT_IN_SEGMENT = 65536L;

    // column vector related constants
    constexpr i64 DEFAULT_VECTOR_SIZE = DEFAULT_BLOCK_CAPACITY;
    constexpr u64 INITIAL_VECTOR_CHUNK_ID = std::numeric_limits<u64>::max();

    constexpr u64 MIN_VECTOR_CHUNK_SIZE = 4096UL;
    constexpr u64 MAX_VECTOR_CHUNK_SIZE = 1024 * 1024UL;
    constexpr u64 MAX_VECTOR_CHUNK_COUNT = std::numeric_limits<u64>::max();

    // Each row has one chunk.
    constexpr u64 DEFAULT_FIXLEN_CHUNK_SIZE = 65536L; // 1MB

    // segment related constants
    constexpr SizeT DEFAULT_SEGMENT_CAPACITY = 1024 * 8192; // 1024 * 8192 = 8M rows
    constexpr SizeT SEGMENT_OFFSET_IN_DOCID = 23;           // it should be adjusted together with DEFAULT_SEGMENT_CAPACITY
    constexpr u64 SEGMENT_MASK_IN_DOCID = 0x7FFFFF;         // it should be adjusted together with DEFAULT_SEGMENT_CAPACITY
    constexpr u32 INVALID_SEGMENT_ID = std::numeric_limits<u32>::max();

    // queue related constants, TODO: double check the necessary
    constexpr SizeT BG_GROUND_TASK_QUEUE_SIZE = 65536;
    constexpr SizeT EXECUTOR_TASK_QUEUE_SIZE = 1024;
    constexpr SizeT DEFAULT_BLOCKING_QUEUE_SIZE = 1024;

    // transaction related constants
    constexpr u64 MAX_TXN_ID = std::numeric_limits<u64>::max();
    constexpr u64 MAX_TIMESTAMP = std::numeric_limits<u64>::max();
    constexpr TxnTimeStamp UNCOMMIT_TS = std::numeric_limits<u64>::max();

    constexpr SizeT KB = 1024;
    constexpr SizeT MB = 1024 * KB;
    constexpr SizeT GB = 1024 * MB;

    constexpr SizeT DEFAULT_RANDOM_NAME_LEN = 10;

    constexpr SizeT DEFAULT_BASE_NUM = 2;
    constexpr SizeT DEFAULT_BASE_FILE_SIZE = 8 * 1024;
    constexpr SizeT DEFAULT_OUTLINE_FILE_MAX_SIZE = 16 * 1024 * 1024;

    constexpr SizeT DEFAULT_CHUNK_SIZE = 10 * 1024 * 1024;
    constexpr SizeT DEFAULT_ALIGN_SIZE = sizeof(char *);

    constexpr SizeT MIN_CLEANUP_INTERVAL_SEC = 0; // 0 means disable the function
    constexpr SizeT DEFAULT_CLEANUP_INTERVAL_SEC = 10;
    constexpr std::string_view DEFAULT_CLEANUP_INTERVAL_SEC_STR = "10s"; // 10 seconds
    constexpr SizeT MAX_CLEANUP_INTERVAL_SEC = 60 * 60 * 24 * 30; // 1 month

    constexpr SizeT MIN_COMPACT_INTERVAL_SEC = 0; // 0 means disable the function
    constexpr SizeT DEFAULT_COMPACT_INTERVAL_SEC = 10;
    constexpr std::string_view DEFAULT_COMPACT_INTERVAL_SEC_STR = "10s"; // 10 seconds
    constexpr SizeT MAX_COMPACT_INTERVAL_SEC = 60 * 60 * 24 * 30; // 1 month

    constexpr SizeT MIN_OPTIMIZE_INTERVAL_SEC = 1;
    constexpr SizeT DEFAULT_OPTIMIZE_INTERVAL_SEC = 10;
    constexpr std::string_view DEFAULT_OPTIMIZE_INTERVAL_SEC_STR = "10s"; // 10 seconds
    constexpr SizeT MAX_OPTIMIZE_INTERVAL_SEC = 60 * 60 * 24 * 30; // 1 month

    constexpr SizeT MIN_MEMINDEX_CAPACITY = DEFAULT_BLOCK_CAPACITY;           // 1 Block
    constexpr SizeT DEFAULT_MEMINDEX_CAPACITY = 128 * DEFAULT_BLOCK_CAPACITY; // 128 * 8192 = 1M rows
    constexpr SizeT MAX_MEMINDEX_CAPACITY = DEFAULT_SEGMENT_CAPACITY;         // 1 Segment

    constexpr i64 MIN_WAL_FILE_SIZE_THRESHOLD = 1024;                                    // 1KB
    constexpr i64 DEFAULT_WAL_FILE_SIZE_THRESHOLD = 1 * 1024l * 1024l * 1024l;           // 1GB
    constexpr std::string_view DEFAULT_WAL_FILE_SIZE_THRESHOLD_STR = "1GB";           // 1GB
    constexpr i64 MAX_WAL_FILE_SIZE_THRESHOLD = 1024l * DEFAULT_WAL_FILE_SIZE_THRESHOLD; // 1TB

    constexpr i64 MIN_FULL_CHECKPOINT_INTERVAL_SEC = 0; // 0 means disable full checkpoint
    constexpr i64 DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC = 30; // 30 seconds
    constexpr std::string_view DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC_STR = "30s"; // 30 seconds
    constexpr i64 MAX_FULL_CHECKPOINT_INTERVAL_SEC = 60 * 60 * 24 * 30; // 1 month

    constexpr i64 MIN_DELTA_CHECKPOINT_INTERVAL_SEC = 0; // 0 means disable delta checkpoint
    constexpr i64 DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC = 5; // 5 seconds
    constexpr std::string_view DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC_STR = "5s"; // 5 seconds
    constexpr i64 MAX_DELTA_CHECKPOINT_INTERVAL_SEC = 60 * 60 * 24 * 30; // 1 month

    constexpr i64 MIN_CHECKPOINT_INTERVAL_WAL_BYTES = 1024; // 1K
    constexpr i64 DELTA_CHECKPOINT_INTERVAL_WAL_BYTES = 64 * 1024l * 1024l; // 64 MB
    constexpr std::string_view DELTA_CHECKPOINT_INTERVAL_WAL_BYTES_STR = "64MB"; // 64 MB
    constexpr i64 MAX_CHECKPOINT_INTERVAL_WAL_BYTES = 1024l * 1024l * 1024l; // 1GB


    constexpr std::string_view WAL_FILE_TEMP_FILE = "wal.log";
    constexpr std::string_view WAL_FILE_PREFIX = "wal.log";
    constexpr std::string_view CATALOG_FILE_DIR = "catalog";

    constexpr std::string_view SYSTEM_DB_NAME = "system";
    constexpr std::string_view DEFAULT_DB_NAME = "default_db";
    constexpr std::string_view SYSTEM_CONFIG_TABLE_NAME = "config";
    constexpr SizeT DEFAULT_PROFILER_HISTORY_SIZE = 128;

    // default emvb parameter
    constexpr u32 EMVB_CENTROID_NPROBE = 3;
    constexpr f32 EMVB_THRESHOLD_FIRST = 0.0f;
    constexpr u32 EMVB_N_DOC_TO_SCORE_FACTOR = 100;
    constexpr u32 EMVB_N_DOC_OUT_SECOND_STAGE_FACTOR = 20;
    constexpr f32 EMVB_THRESHOLD_FINAL = 0.0f;

    // default diskann parameter
    constexpr SizeT DISKANN_R = 16;
    constexpr SizeT DISKANN_L = 200;
    constexpr SizeT DISKANN_NUM_PQ_CHUNKS = 4;
    constexpr SizeT DISKANN_NUM_PARTS = 1;

    // default hnsw parameter
    constexpr SizeT HNSW_M = 16;
    constexpr SizeT HNSW_EF_CONSTRUCTION = 200;
    constexpr SizeT HNSW_BLOCK_SIZE = 8192;

    constexpr SizeT BMP_BLOCK_SIZE = 16;

    // default distance compute blas parameter
    constexpr SizeT DISTANCE_COMPUTE_BLAS_QUERY_BS = 4096;
    constexpr SizeT DISTANCE_COMPUTE_BLAS_DATABASE_BS = 1024;

    constexpr SizeT DBT_COMPACTION_M = 4;
    constexpr SizeT DBT_COMPACTION_C = 4;
    constexpr SizeT DBT_COMPACTION_S = DEFAULT_BLOCK_CAPACITY;

    // default query option parameter
    constexpr u32 DEFAULT_MATCH_TEXT_OPTION_TOP_N = 10;
    constexpr u32 DEFAULT_MATCH_TENSOR_OPTION_TOP_N = 10;
    constexpr u32 DEFAULT_FUSION_OPTION_TOP_N = 100;

    constexpr SizeT DEFAULT_BUFFER_MANAGER_SIZE = 8 * 1024lu * 1024lu * 1024lu; // 8Gib
    constexpr SizeT DEFAULT_BUFFER_MANAGER_LRU_COUNT = 7;
    constexpr std::string_view DEFAULT_BUFFER_MANAGER_SIZE_STR = "8GB"; // 8Gib

    constexpr SizeT DEFAULT_MEMINDEX_MEMORY_QUOTA = 4 * 1024lu * 1024lu * 1024lu; // 4GB
    constexpr std::string_view DEFAULT_MEMINDEX_MEMORY_QUOTA_STR = "4GB"; // 4GB

    constexpr SizeT DEFAULT_LOG_FILE_SIZE = 64 * 1024lu * 1024lu; // 64MB
    constexpr std::string_view DEFAULT_LOG_FILE_SIZE_STR = "64MB"; // 64MB

    constexpr SizeT INSERT_BATCH_ROW_LIMIT = 8192;

    // default persistence parameter
    constexpr std::string_view DEFAULT_PERSISTENCE_DIR = "/var/infinity/persistence"; // Empty means disabled
    constexpr std::string_view DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT_STR = "100MB"; // 100MB
    constexpr SizeT DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT = 100 * 1024lu * 1024lu;  // 100MB

    // config name
    constexpr std::string_view VERSION_OPTION_NAME = "version";
    constexpr std::string_view SERVER_MODE_OPTION_NAME = "server_mode";
    constexpr std::string_view TIME_ZONE_OPTION_NAME = "time_zone";
    constexpr std::string_view TIME_ZONE_BIAS_OPTION_NAME = "time_zone_bias";
    constexpr std::string_view CPU_LIMIT_OPTION_NAME = "cpu_limit";
    constexpr std::string_view SERVER_ADDRESS_OPTION_NAME = "server_address";
    constexpr std::string_view PEER_SERVER_IP_OPTION_NAME = "peer_ip";
    constexpr std::string_view PEER_SERVER_PORT_OPTION_NAME = "peer_port";

    constexpr std::string_view POSTGRES_PORT_OPTION_NAME = "postgres_port";
    constexpr std::string_view HTTP_PORT_OPTION_NAME = "http_port";
    constexpr std::string_view CLIENT_PORT_OPTION_NAME = "client_port";
    constexpr std::string_view CONNECTION_POOL_SIZE_OPTION_NAME = "connection_pool_size";
    constexpr std::string_view PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME = "peer_server_connection_pool_size";
    constexpr std::string_view LOG_FILENAME_OPTION_NAME = "log_filename";

    constexpr std::string_view LOG_DIR_OPTION_NAME = "log_dir";
    constexpr std::string_view LOG_TO_STDOUT_OPTION_NAME = "log_to_stdout";
    constexpr std::string_view LOG_FILE_MAX_SIZE_OPTION_NAME = "log_file_max_size";
    constexpr std::string_view LOG_FILE_ROTATE_COUNT_OPTION_NAME = "log_file_rotate_count";
    constexpr std::string_view LOG_LEVEL_OPTION_NAME = "log_level";

    constexpr std::string_view DATA_DIR_OPTION_NAME = "data_dir";
    constexpr std::string_view CLEANUP_INTERVAL_OPTION_NAME = "cleanup_interval";
    constexpr std::string_view COMPACT_INTERVAL_OPTION_NAME = "compact_interval";
    constexpr std::string_view OPTIMIZE_INTERVAL_OPTION_NAME = "optimize_interval";
    constexpr std::string_view MEM_INDEX_CAPACITY_OPTION_NAME = "mem_index_capacity";

    constexpr std::string_view PERSISTENCE_DIR_OPTION_NAME = "persistence_dir";
    constexpr std::string_view PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME = "persistence_object_size_limit";

    constexpr std::string_view BUFFER_MANAGER_SIZE_OPTION_NAME = "buffer_manager_size";
    constexpr std::string_view LRU_NUM_OPTION_NAME = "lru_num";
    constexpr std::string_view TEMP_DIR_OPTION_NAME = "temp_dir";
    constexpr std::string_view MEMINDEX_MEMORY_QUOTA_OPTION_NAME = "memindex_memory_quota";

    constexpr std::string_view WAL_DIR_OPTION_NAME = "wal_dir";
    constexpr std::string_view WAL_COMPACT_THRESHOLD_OPTION_NAME = "wal_compact_threshold";
    constexpr std::string_view FULL_CHECKPOINT_INTERVAL_OPTION_NAME = "full_checkpoint_interval";

    constexpr std::string_view DELTA_CHECKPOINT_INTERVAL_OPTION_NAME = "delta_checkpoint_interval";
    constexpr std::string_view DELTA_CHECKPOINT_THRESHOLD_OPTION_NAME = "delta_checkpoint_threshold";
    constexpr std::string_view WAL_FLUSH_OPTION_NAME = "wal_flush";
    constexpr std::string_view RESOURCE_DIR_OPTION_NAME = "resource_dir";

    constexpr std::string_view RECORD_RUNNING_QUERY_OPTION_NAME = "record_running_query";

    // Variable name
    constexpr std::string_view QUERY_COUNT_VAR_NAME = "query_count";        // global and session
    constexpr std::string_view SESSION_COUNT_VAR_NAME = "session_count";    // global
    constexpr std::string_view BUFFER_USAGE_VAR_NAME = "buffer_usage";    // global
    constexpr std::string_view SCHEDULE_POLICY_VAR_NAME = "schedule_policy";    // global
    constexpr std::string_view DELTA_LOG_COUNT_VAR_NAME = "delta_log_count";    // global
    constexpr std::string_view NEXT_TXN_ID_VAR_NAME = "next_transaction_id";    // global
    constexpr std::string_view BUFFER_OBJECT_COUNT_VAR_NAME = "buffer_object_count";    // global
    constexpr std::string_view UNUSED_BUFFER_OBJECT_COUNT_VAR_NAME = "unused_buffer_object";    // global
    constexpr std::string_view ACTIVE_TXN_COUNT_VAR_NAME = "active_txn_count";    // global
    constexpr std::string_view CURRENT_TS_VAR_NAME = "current_timestamp";    // global
    constexpr std::string_view TOTAL_COMMIT_COUNT_VAR_NAME = "total_commit_count"; // global and session
    constexpr std::string_view TOTAL_ROLLBACK_COUNT_VAR_NAME = "total_rollback_count"; // global and session
    constexpr std::string_view CONNECTED_TS_VAR_NAME = "connected_timestamp"; // session
    constexpr std::string_view CATALOG_VERSION_VAR_NAME = "catalog_version";   // global
    constexpr std::string_view ACTIVE_WAL_FILENAME_VAR_NAME = "active_wal_filename";   // global
    constexpr std::string_view ENABLE_PROFILE_VAR_NAME = "enable_profile";  // session
    constexpr std::string_view PROFILE_RECORD_CAPACITY_VAR_NAME = "profile_record_capacity";  // session
    constexpr std::string_view BG_TASK_COUNT_VAR_NAME = "bg_task_count";  // global
    constexpr std::string_view RUNNING_BG_TASK_VAR_NAME = "running_bg_task";  // global
    constexpr std::string_view RUNNING_COMPACT_TASK_VAR_NAME = "running_compact_task";  // global
    constexpr std::string_view SYSTEM_MEMORY_USAGE_VAR_NAME = "system_memory_usage";  // global
    constexpr std::string_view OPEN_FILE_COUNT_VAR_NAME = "open_file_count";  // global
    constexpr std::string_view CPU_USAGE_VAR_NAME = "cpu_usage";  // global
    constexpr std::string_view FOLLOWER_NUMBER = "follower_number";  // global

}

} // namespace infinity
