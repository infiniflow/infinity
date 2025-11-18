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

#include <unistd.h>

module infinity_core:config.impl;

import :config;
import :logger;
import :infinity_exception;
import :boost;
import :utility;

import std;

import compilation_config;
import global_resource_usage;

namespace infinity {
using namespace std::chrono;

Config::Config() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Config");
#endif
}

Config::~Config() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("Config");
#endif
}

u64 Config::GetAvailableMem() {
    u64 pages = sysconf(_SC_PHYS_PAGES);
    u64 page_size = sysconf(_SC_PAGE_SIZE); // Byte
    return pages * page_size;
}

void Config::ParseTimeZoneStr(const std::string &time_zone_str, std::string &parsed_time_zone, i32 &parsed_time_zone_bias) {
    parsed_time_zone = time_zone_str.substr(0, 3);
    ToUpper(parsed_time_zone);
    parsed_time_zone_bias = std::stoi(time_zone_str.substr(3, std::string::npos));
}

Status Config::ParseByteSize(const std::string &byte_size_str, i64 &byte_size) {

    std::unordered_map<std::string, u64> byte_unit = {{"kb", 1024ul}, {"mb", 1024ul * 1024ul}, {"gb", 1024ul * 1024ul * 1024ul}};
    if (byte_size_str.empty()) {
        return Status::InvalidByteSize(byte_size_str);
    }

    u64 factor;
    auto res = std::from_chars(byte_size_str.data(), byte_size_str.data() + byte_size_str.size(), factor);
    if (res.ec == std::errc()) {
        std::string unit = res.ptr;
        ToLower(unit);
        if (auto it = byte_unit.find(unit); it != byte_unit.end()) {
            byte_size = factor * it->second;
            return Status::OK();
        } else {
            return Status::InvalidByteSize(byte_size_str);
        }
    } else {
        return Status::InvalidByteSize(byte_size_str);
    }
}

Status Config::ParseTimeInfo(const std::string &time_info, i64 &time_seconds) {
    if (time_info.empty()) {
        return Status::EmptyConfigParameter();
    }

    size_t info_size = time_info.size();
    if (info_size == 1) {
        return Status::InvalidTimeInfo(time_info);
    }

    u64 time_number = 0;
    for (size_t i = 0; i < info_size - 1; ++i) {
        if (std::isdigit(time_info[i])) {
            time_number = time_number * 10 + (time_info[i] - '0');
        } else {
            return Status::InvalidTimeInfo(time_info);
        }
    }

    switch (time_info[info_size - 1]) {
        case 's':
        case 'S': {
            time_seconds = time_number;
            break;
        }
        case 'm':
        case 'M': {
            time_seconds = time_number * 60u;
            break;
        }
        case 'h':
        case 'H': {
            time_seconds = time_number * 3600u;
            break;
        }
        default: {
            return Status::InvalidTimeInfo(time_info);
        }
    }

    return Status::OK();
}

Status Config::Init(const std::shared_ptr<std::string> &config_path, DefaultConfig *default_config) {
    toml::table config_toml{};
    if (config_path.get() != nullptr) {
        LOG_INFO(fmt::format("Config file: {}", *config_path));
    } else {
        LOG_INFO("No config file is given, use default configs.");
    }
    if (config_path.get() == nullptr || config_path->empty() || !VirtualStore::Exists(*config_path)) {
        if (config_path.get() == nullptr || config_path->empty()) {
            fmt::print("No config file is given, use default configs.\n");
        } else {
            fmt::print("Config file: {} is not found.\n", *config_path);
            return Status::NotFound(fmt::format("Config file: {} not found", *config_path));
        }

        Status status;

        // Version
        std::string current_version = fmt::format("{}.{}.{}", version_major(), version_minor(), version_patch());
        auto version_option = std::make_unique<StringOption>(VERSION_OPTION_NAME, current_version);
        status = global_options_.AddOption(std::move(version_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Server mode
        std::string server_mode = "standalone";
        auto server_mode_option = std::make_unique<StringOption>(SERVER_MODE_OPTION_NAME, server_mode);
        status = global_options_.AddOption(std::move(server_mode_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Timezone
        std::string time_zone_str = "UTC";
        auto time_zone_option = std::make_unique<StringOption>(TIME_ZONE_OPTION_NAME, time_zone_str);
        status = global_options_.AddOption(std::move(time_zone_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Timezone Bias
        i64 time_zone_bias = 8;
        auto time_zone_bias_option = std::make_unique<IntegerOption>(TIME_ZONE_BIAS_OPTION_NAME, time_zone_bias, 12, -12);
        status = global_options_.AddOption(std::move(time_zone_bias_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // CPU limit
        auto cpu_limit_option = std::make_unique<IntegerOption>(CPU_LIMIT_OPTION_NAME, std::thread::hardware_concurrency(), 16384, 1);
        status = global_options_.AddOption(std::move(cpu_limit_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Record running query
        bool record_running_query = false;
        record_running_query_ = record_running_query;
        std::unique_ptr<BooleanOption> record_running_query_option =
            std::make_unique<BooleanOption>(RECORD_RUNNING_QUERY_OPTION_NAME, record_running_query);
        status = global_options_.AddOption(std::move(record_running_query_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Server address
        std::string server_address_str = "0.0.0.0";
        auto server_address_option = std::make_unique<StringOption>(SERVER_ADDRESS_OPTION_NAME, server_address_str);
        status = global_options_.AddOption(std::move(server_address_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer server address
        std::string peer_server_ip_str = "0.0.0.0";
        auto peer_server_ip_option = std::make_unique<StringOption>(PEER_SERVER_IP_OPTION_NAME, peer_server_ip_str);
        status = global_options_.AddOption(std::move(peer_server_ip_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer server port
        i64 peer_server_port = DEFAULT_PEER_PORT;
        auto peer_server_port_option = std::make_unique<IntegerOption>(PEER_SERVER_PORT_OPTION_NAME, peer_server_port, 65535, 1024);
        status = global_options_.AddOption(std::move(peer_server_port_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Postgres port
        i64 pg_port = DEFAULT_POSTGRES_PORT;
        auto pg_port_option = std::make_unique<IntegerOption>(POSTGRES_PORT_OPTION_NAME, pg_port, 65535, 1024);
        status = global_options_.AddOption(std::move(pg_port_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // HTTP port
        i64 http_port = DEFAULT_HTTP_PORT;
        auto http_port_option = std::make_unique<IntegerOption>(HTTP_PORT_OPTION_NAME, http_port, 65535, 1024);
        status = global_options_.AddOption(std::move(http_port_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // RPC Client port
        i64 rpc_client_port = DEFAULT_CLIENT_PORT;
        auto client_port_option = std::make_unique<IntegerOption>(CLIENT_PORT_OPTION_NAME, rpc_client_port, 65535, 1024);
        status = global_options_.AddOption(std::move(client_port_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer retry delay
        i64 peer_retry_delay = DEFAULT_PEER_RETRY_DELAY;
        auto peer_retry_delay_option = std::make_unique<IntegerOption>(PEER_RETRY_DELAY_OPTION_NAME, peer_retry_delay, 10000, 0);
        status = global_options_.AddOption(std::move(peer_retry_delay_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer retry num
        i64 peer_retry_count = DEFAULT_PEER_RETRY_COUNT;
        auto peer_retry_count_option = std::make_unique<IntegerOption>(PEER_RETRY_COUNT_OPTION_NAME, peer_retry_count, 10, 0);
        status = global_options_.AddOption(std::move(peer_retry_count_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer connect timeout
        i64 peer_connect_timeout = DEFAULT_PEER_CONNECT_TIMEOUT;
        auto peer_connect_timeout_option = std::make_unique<IntegerOption>(PEER_CONNECT_TIMEOUT_OPTION_NAME, peer_connect_timeout, 10000, 0);
        status = global_options_.AddOption(std::move(peer_connect_timeout_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer recv timeout
        i64 peer_recv_timeout = DEFAULT_PEER_RECV_TIMEOUT;
        auto peer_recv_timeout_option = std::make_unique<IntegerOption>(PEER_RECV_TIMEOUT_OPTION_NAME, peer_recv_timeout, 10000, 0);
        status = global_options_.AddOption(std::move(peer_recv_timeout_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer send timeout
        i64 peer_send_timeout = DEFAULT_PEER_SEND_TIMEOUT;
        auto peer_send_timeout_option = std::make_unique<IntegerOption>(PEER_SEND_TIMEOUT_OPTION_NAME, peer_send_timeout, 10000, 0);
        status = global_options_.AddOption(std::move(peer_send_timeout_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Client pool size
        i64 connection_pool_size = 256;
        auto connection_pool_size_option = std::make_unique<IntegerOption>(CONNECTION_POOL_SIZE_OPTION_NAME, connection_pool_size, 65536, 1);
        status = global_options_.AddOption(std::move(connection_pool_size_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Peer server connection pool size
        i64 peer_server_connection_pool_size = 64;
        auto peer_server_connection_pool_size_option =
            std::make_unique<IntegerOption>(PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME, peer_server_connection_pool_size, 65536, 1);
        status = global_options_.AddOption(std::move(peer_server_connection_pool_size_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log file name
        std::string log_filename = "infinity.log";
        auto log_file_name_option = std::make_unique<StringOption>(LOG_FILENAME_OPTION_NAME, log_filename);
        status = global_options_.AddOption(std::move(log_file_name_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log dir
        std::string log_dir = "/var/infinity/log";
        if (default_config != nullptr) {
            log_dir = default_config->default_log_dir_;
        }
        auto log_dir_option = std::make_unique<StringOption>(LOG_DIR_OPTION_NAME, log_dir);
        status = global_options_.AddOption(std::move(log_dir_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log To Stdout
        bool log_to_stdout = true;
        if (default_config != nullptr) {
            log_to_stdout = default_config->default_log_to_stdout_;
        }
        std::unique_ptr<BooleanOption> log_to_stdout_option = std::make_unique<BooleanOption>(LOG_TO_STDOUT_OPTION_NAME, log_to_stdout);
        status = global_options_.AddOption(std::move(log_to_stdout_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log File Max Size
        i64 log_file_max_size = 1024lu * 1024lu * 1024lu;
        auto log_file_max_size_option =
            std::make_unique<IntegerOption>(LOG_FILE_MAX_SIZE_OPTION_NAME, log_file_max_size, std::numeric_limits<i64>::max(), 1024lu * 1024lu);
        status = global_options_.AddOption(std::move(log_file_max_size_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log File Rotate count
        i64 log_file_rotate_count = 8;
        auto log_file_rotate_count_option = std::make_unique<IntegerOption>(LOG_FILE_ROTATE_COUNT_OPTION_NAME, log_file_rotate_count, 65536, 1);
        status = global_options_.AddOption(std::move(log_file_rotate_count_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log Level
        LogLevel log_level = LogLevel::kInfo;
        if (default_config != nullptr) {
            log_level = default_config->default_log_level_;
        }
        std::unique_ptr<LogLevelOption> log_level_option = std::make_unique<LogLevelOption>(LOG_LEVEL_OPTION_NAME, log_level);
        status = global_options_.AddOption(std::move(log_level_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Data Dir
        std::string data_dir = "/var/infinity/data";
        if (default_config != nullptr) {
            data_dir = default_config->default_data_dir_;
        }
        auto data_dir_option = std::make_unique<StringOption>(DATA_DIR_OPTION_NAME, data_dir);
        status = global_options_.AddOption(std::move(data_dir_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Catalog Dir
        std::string catalog_dir = "/var/infinity/catalog";
        if (default_config != nullptr) {
            catalog_dir = default_config->default_catalog_dir_;
        }
        auto catalog_dir_option = std::make_unique<StringOption>(CATALOG_DIR_OPTION_NAME, catalog_dir);
        status = global_options_.AddOption(std::move(catalog_dir_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Persistence Dir
        std::string persistence_dir = DEFAULT_PERSISTENCE_DIR.data();
        if (default_config != nullptr) {
            persistence_dir = default_config->default_persistence_dir_;
        }
        auto persistence_dir_option = std::make_unique<StringOption>(PERSISTENCE_DIR_OPTION_NAME, persistence_dir);
        global_options_.AddOption(std::move(persistence_dir_option));

        // Persistence Object Size Limit
        i64 persistence_object_size_limit = DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT;
        auto persistence_object_size_limit_option = std::make_unique<IntegerOption>(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME,
                                                                                    persistence_object_size_limit,
                                                                                    std::numeric_limits<i64>::max(),
                                                                                    0);
        global_options_.AddOption(std::move(persistence_object_size_limit_option));

        // Storage type
        std::string storage_type = std::string(DEFAULT_STORAGE_TYPE);
        auto storage_type_option = std::make_unique<StringOption>(STORAGE_TYPE_OPTION_NAME, storage_type);
        status = global_options_.AddOption(std::move(storage_type_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Cleanup Interval
        i64 cleanup_interval = DEFAULT_CLEANUP_INTERVAL_SEC;
        auto cleanup_interval_option =
            std::make_unique<IntegerOption>(CLEANUP_INTERVAL_OPTION_NAME, cleanup_interval, MAX_CLEANUP_INTERVAL_SEC, MIN_CLEANUP_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(cleanup_interval_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Compact Interval
        i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
        auto compact_interval_option =
            std::make_unique<IntegerOption>(COMPACT_INTERVAL_OPTION_NAME, compact_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(compact_interval_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Optimize Index Interval
        // i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
        i64 optimize_index_interval = 0;
        auto optimize_interval_option = std::make_unique<IntegerOption>(OPTIMIZE_INTERVAL_OPTION_NAME,
                                                                        optimize_index_interval,
                                                                        MAX_COMPACT_INTERVAL_SEC,
                                                                        MIN_COMPACT_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(optimize_interval_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Mem Index Capacity
        i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
        auto mem_index_capacity_option =
            std::make_unique<IntegerOption>(MEM_INDEX_CAPACITY_OPTION_NAME, mem_index_capacity, MAX_MEMINDEX_CAPACITY, MIN_MEMINDEX_CAPACITY);
        status = global_options_.AddOption(std::move(mem_index_capacity_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Snapshots Dir
        std::string snapshot_dir = DEFAULT_SNAPSHOT_DIR.data();
        auto snapshot_dir_option = std::make_unique<StringOption>(SNAPSHOT_DIR_OPTION_NAME, snapshot_dir);
        global_options_.AddOption(std::move(snapshot_dir_option));

        // Buffer Manager Size
        i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
        auto buffer_manager_size_option =
            std::make_unique<IntegerOption>(BUFFER_MANAGER_SIZE_OPTION_NAME, buffer_manager_size, std::numeric_limits<i64>::max(), 0);
        status = global_options_.AddOption(std::move(buffer_manager_size_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Buffer manager size
        size_t lru_num = DEFAULT_BUFFER_MANAGER_LRU_COUNT;
        auto lru_num_option = std::make_unique<IntegerOption>(LRU_NUM_OPTION_NAME, lru_num, 100, 1);
        status = global_options_.AddOption(std::move(lru_num_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Memory index capacity
        i64 memindex_memory_quota = DEFAULT_MEMINDEX_MEMORY_QUOTA;
        auto memindex_memory_quota_option =
            std::make_unique<IntegerOption>(MEMINDEX_MEMORY_QUOTA_OPTION_NAME, memindex_memory_quota, std::numeric_limits<i64>::max(), 0);
        status = global_options_.AddOption(std::move(memindex_memory_quota_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Dense index building worker
        i64 dense_index_building_worker = std::thread::hardware_concurrency() / 2;
        if (dense_index_building_worker < 2) {
            dense_index_building_worker = 2;
        }
        auto dense_index_building_worker_option = std::make_unique<IntegerOption>(DENSE_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                  dense_index_building_worker,
                                                                                  std::thread::hardware_concurrency(),
                                                                                  1);
        status = global_options_.AddOption(std::move(dense_index_building_worker_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Sparse index building worker
        i64 sparse_index_building_worker = std::thread::hardware_concurrency() / 2;
        if (sparse_index_building_worker < 2) {
            sparse_index_building_worker = 2;
        }
        auto sparse_index_building_worker_option = std::make_unique<IntegerOption>(SPARSE_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                   sparse_index_building_worker,
                                                                                   std::thread::hardware_concurrency(),
                                                                                   1);
        status = global_options_.AddOption(std::move(sparse_index_building_worker_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Fulltext index building worker
        i64 fulltext_index_building_worker = std::thread::hardware_concurrency() / 2;
        if (fulltext_index_building_worker < 2) {
            fulltext_index_building_worker = 2;
        }
        auto fulltext_index_building_worker_option = std::make_unique<IntegerOption>(FULLTEXT_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                     fulltext_index_building_worker,
                                                                                     std::thread::hardware_concurrency(),
                                                                                     1);
        status = global_options_.AddOption(std::move(fulltext_index_building_worker_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Bottom executor worker
        i64 bottom_executor_worker = std::thread::hardware_concurrency() / 2;
        if (bottom_executor_worker < 2) {
            bottom_executor_worker = 2;
        }
        auto bottom_executor_worker_option =
            std::make_unique<IntegerOption>(BOTTOM_EXECUTOR_WORKER_OPTION_NAME, bottom_executor_worker, std::thread::hardware_concurrency(), 1);
        status = global_options_.AddOption(std::move(bottom_executor_worker_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Result Cache
        std::string result_cache(DEFAULT_RESULT_CACHE);
        auto result_cache_option = std::make_unique<StringOption>(RESULT_CACHE_OPTION_NAME, result_cache);
        status = global_options_.AddOption(std::move(result_cache_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        i64 cache_result_num = DEFAULT_CACHE_RESULT_CAPACITY;
        auto cache_result_num_option =
            std::make_unique<IntegerOption>(CACHE_RESULT_CAPACITY_OPTION_NAME, cache_result_num, std::numeric_limits<i64>::max(), 0);
        status = global_options_.AddOption(std::move(cache_result_num_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Temp Dir
        std::string temp_dir = "/var/infinity/tmp";
        if (default_config != nullptr) {
            temp_dir = default_config->default_temp_dir_;
        }
        auto temp_dir_option = std::make_unique<StringOption>(TEMP_DIR_OPTION_NAME, temp_dir);
        status = global_options_.AddOption(std::move(temp_dir_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // WAL Dir
        std::string wal_dir = "/var/infinity/wal";
        if (default_config != nullptr) {
            wal_dir = default_config->default_wal_dir_;
        }
        auto wal_dir_option = std::make_unique<StringOption>(WAL_DIR_OPTION_NAME, wal_dir);
        status = global_options_.AddOption(std::move(wal_dir_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // WAL Compact Threshold
        i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;
        auto wal_compact_threshold_option = std::make_unique<IntegerOption>(WAL_COMPACT_THRESHOLD_OPTION_NAME,
                                                                            wal_compact_threshold,
                                                                            MAX_WAL_FILE_SIZE_THRESHOLD,
                                                                            MIN_WAL_FILE_SIZE_THRESHOLD);
        status = global_options_.AddOption(std::move(wal_compact_threshold_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Checkpoint Interval
        i64 checkpoint_interval = DEFAULT_CHECKPOINT_INTERVAL_SEC;
        auto checkpoint_interval_option = std::make_unique<IntegerOption>(CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                          checkpoint_interval,
                                                                          MAX_CHECKPOINT_INTERVAL_SEC,
                                                                          MIN_CHECKPOINT_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(checkpoint_interval_option));
        if (!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Flush Method At Commit
        FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
        std::unique_ptr<FlushOption> wal_flush_option = std::make_unique<FlushOption>(WAL_FLUSH_OPTION_NAME, flush_option_type);
        status = global_options_.AddOption(std::move(wal_flush_option));
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        // Resource Dir
        std::string resource_dir = "/var/infinity/resource";
        if (default_config != nullptr) {
            resource_dir = default_config->default_resource_dir_;
        }
        auto resource_dir_option = std::make_unique<StringOption>("resource_dir", resource_dir);
        status = global_options_.AddOption(std::move(resource_dir_option));
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        // Replay WAL
        bool replay_wal = true;
        std::unique_ptr<BooleanOption> replay_wal_option = std::make_unique<BooleanOption>(REPLAY_WAL_OPTION_NAME, replay_wal);
        status = global_options_.AddOption(std::move(replay_wal_option));
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
    } else {
        config_toml = toml::parse_file(*config_path);

        // General
        {
            if (config_toml.contains("general")) {

                auto general_config = config_toml["general"];
                auto general_config_table = general_config.as_table();
                for (auto &elem : *general_config_table) {

                    std::string var_name = std::string(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'general' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kVersion: {
                            // Version
                            std::string invalid_str = "invalid";
                            std::string version_str;
                            if (elem.second.is_string()) {
                                version_str = elem.second.value_or("invalid");
                                ToLower(version_str);

                                std::string major_version_str;
                                i8 point_count = 0;
                                for (char c : version_str) {
                                    if (c == '.') {
                                        ++point_count;
                                        if (point_count > 1) {
                                            break;
                                        }
                                    }
                                    major_version_str += c;
                                }

                                if (major_version_str == invalid_str) {
                                    return Status::InvalidConfig("Invalid version field");
                                } else {
                                    std::string current_major_version = fmt::format("{}.{}", version_major(), version_minor());
                                    std::string current_version = fmt::format("{}.{}.{}", version_major(), version_minor(), version_patch());
                                    if (major_version_str == current_major_version) {
                                        auto version_option = std::make_unique<StringOption>(VERSION_OPTION_NAME, current_version);
                                        Status status = global_options_.AddOption(std::move(version_option));
                                        if (!status.ok()) {
                                            UnrecoverableError(status.message());
                                        }
                                    } else {
                                        return Status::MismatchVersion(major_version_str, current_major_version);
                                    }
                                }
                            } else {
                                return Status::InvalidConfig("'version' field isn't string.");
                            }
                            break;
                        }
                        case GlobalOptionIndex::kServerMode: {
                            // Server Mode
                            std::string server_mode = "standalone";
                            if (elem.second.is_string()) {
                                server_mode = elem.second.value_or(server_mode);
                            } else {
                                return Status::InvalidConfig("'server_mode' field isn't string.");
                            }

                            ToLower(server_mode);
                            if (server_mode == "standalone" or server_mode == "admin") {
                                auto server_mode_option = std::make_unique<StringOption>(SERVER_MODE_OPTION_NAME, server_mode);
                                Status status = global_options_.AddOption(std::move(server_mode_option));
                                if (!status.ok()) {
                                    UnrecoverableError(status.message());
                                }
                            } else {
                                return Status::InvalidConfig(fmt::format("Invalid server mode: {}", server_mode));
                            }
                            break;
                        }
                        case GlobalOptionIndex::kTimeZone: {
                            // Timezone
                            if (elem.second.is_string()) {
                                std::string time_zone_str = general_config[TIME_ZONE_OPTION_NAME].value_or("invalid");
                                ToLower(time_zone_str);
                                if (time_zone_str == "invalid") {
                                    return Status::InvalidTimezone(time_zone_str);
                                } else {
                                    std::string time_zone;
                                    i32 time_zone_bias = 0;
                                    try {
                                        ParseTimeZoneStr(time_zone_str, time_zone, time_zone_bias);
                                    } catch (...) {
                                        return Status::InvalidTimezone(time_zone_str);
                                    }

                                    // Timezone
                                    auto time_zone_option = std::make_unique<StringOption>(TIME_ZONE_OPTION_NAME, time_zone);
                                    Status status = global_options_.AddOption(std::move(time_zone_option));
                                    if (!status.ok()) {
                                        UnrecoverableError(status.message());
                                    }

                                    // Timezone Bias
                                    auto time_zone_bias_option = std::make_unique<IntegerOption>(TIME_ZONE_BIAS_OPTION_NAME, time_zone_bias, 12, -12);
                                    if (!time_zone_bias_option->Validate()) {
                                        return Status::InvalidConfig(fmt::format("Invalid timezone bias: {}", time_zone_bias));
                                    }
                                    status = global_options_.AddOption(std::move(time_zone_bias_option));
                                    if (!status.ok()) {
                                        UnrecoverableError(status.message());
                                    }

                                    hours offset_hour(time_zone_bias);
                                    hour_offset_ = offset_hour;
                                }
                            }
                            break;
                        }
                        case GlobalOptionIndex::kWorkerCPULimit: {
                            i64 total_cpu_number = std::thread::hardware_concurrency();

                            if (elem.second.is_integer()) {
                                total_cpu_number = elem.second.value_or(total_cpu_number);
                            } else {
                                return Status::InvalidConfig("'cpu_limit' field isn't integer.");
                            }

                            auto cpu_limit_option = std::make_unique<IntegerOption>(CPU_LIMIT_OPTION_NAME, total_cpu_number, 16384, 1);
                            if (!cpu_limit_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid cpu limit: {}", total_cpu_number));
                            }
                            Status status = global_options_.AddOption(std::move(cpu_limit_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kRecordRunningQuery: {
                            bool record_running_query = false;
                            if (elem.second.is_boolean()) {
                                record_running_query = elem.second.value_or(record_running_query);
                            } else {
                                return Status::InvalidConfig("'record_running_query' field isn't boolean.");
                            }
                            record_running_query_ = record_running_query;
                            std::unique_ptr<BooleanOption> record_running_query_option =
                                std::make_unique<BooleanOption>(RECORD_RUNNING_QUERY_OPTION_NAME, record_running_query);
                            Status status = global_options_.AddOption(std::move(record_running_query_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'general' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kVersion) == nullptr) {
                    // Version
                    UnrecoverableError("Fatal: Missing version field");
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kServerMode) == nullptr) {
                    // Server mode
                    std::string server_mode = "standalone";
                    auto server_mode_option = std::make_unique<StringOption>(SERVER_MODE_OPTION_NAME, server_mode);
                    Status status = global_options_.AddOption(std::move(server_mode_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kTimeZone) == nullptr) {
                    // Time zone
                    UnrecoverableError("Fatal: Missing time zone field");
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kTimeZoneBias) == nullptr) {
                    // Time zone bias
                    UnrecoverableError("Fatal: Missing time zone field");
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kWorkerCPULimit) == nullptr) {
                    // CPU limit
                    auto cpu_limit_option = std::make_unique<IntegerOption>(CPU_LIMIT_OPTION_NAME, std::thread::hardware_concurrency(), 16384, 1);
                    Status status = global_options_.AddOption(std::move(cpu_limit_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kRecordRunningQuery) == nullptr) {
                    // Record running query
                    bool record_running_query = false;
                    record_running_query_ = record_running_query;
                    std::unique_ptr<BooleanOption> record_running_query_option =
                        std::make_unique<BooleanOption>(RECORD_RUNNING_QUERY_OPTION_NAME, record_running_query);
                    Status status = global_options_.AddOption(std::move(record_running_query_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
            }
        }

        // Network
        {
            if (config_toml.contains("network")) {
                auto network_config = config_toml["network"];
                auto network_config_table = network_config.as_table();
                for (auto &elem : *network_config_table) {

                    std::string var_name = std::string(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'network' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kServerAddress: {
                            // Server address
                            std::string server_address = "0.0.0.0";
                            if (elem.second.is_string()) {
                                server_address = elem.second.value_or(server_address);
                            } else {
                                return Status::InvalidConfig("'server_address' field isn't string.");
                            }

                            // Validate the address format
                            boost::system::error_code error;
                            boost::asio::ip::make_address(server_address, error);
                            if (error) {
                                return Status::InvalidIPAddr(server_address);
                            }

                            auto server_address_option = std::make_unique<StringOption>(SERVER_ADDRESS_OPTION_NAME, server_address);
                            Status status = global_options_.AddOption(std::move(server_address_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerServerIP: {
                            // Server address
                            std::string peer_server_ip = "0.0.0.0";
                            if (elem.second.is_string()) {
                                peer_server_ip = elem.second.value_or(peer_server_ip);
                            } else {
                                return Status::InvalidConfig("'peer_server_ip' field isn't string.");
                            }

                            // Validate the address format
                            boost::system::error_code error;
                            boost::asio::ip::make_address(peer_server_ip, error);
                            if (error) {
                                return Status::InvalidIPAddr(peer_server_ip);
                            }

                            auto peer_server_ip_option = std::make_unique<StringOption>(PEER_SERVER_IP_OPTION_NAME, peer_server_ip);
                            Status status = global_options_.AddOption(std::move(peer_server_ip_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerServerPort: {
                            // Peer server port
                            i64 peer_server_port = DEFAULT_PEER_PORT;
                            if (elem.second.is_integer()) {
                                peer_server_port = elem.second.value_or(peer_server_port);
                            } else {
                                return Status::InvalidConfig("'peer_server_port' field isn't integer.");
                            }

                            auto peer_server_port_option =
                                std::make_unique<IntegerOption>(PEER_SERVER_PORT_OPTION_NAME, peer_server_port, 65535, 1024);
                            if (!peer_server_port_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid peer server port: {}", peer_server_port));
                            }
                            Status status = global_options_.AddOption(std::move(peer_server_port_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPostgresPort: {
                            // Postgres port
                            i64 pg_port = DEFAULT_POSTGRES_PORT;
                            if (elem.second.is_integer()) {
                                pg_port = elem.second.value_or(pg_port);
                            } else {
                                return Status::InvalidConfig("'postgres_port' field isn't integer.");
                            }

                            auto pg_port_option = std::make_unique<IntegerOption>(POSTGRES_PORT_OPTION_NAME, pg_port, 65535, 1024);
                            if (!pg_port_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid postgres port: {}", pg_port));
                            }
                            Status status = global_options_.AddOption(std::move(pg_port_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kHTTPPort: {
                            // HTTP port
                            i64 http_port = DEFAULT_HTTP_PORT;
                            if (elem.second.is_integer()) {
                                http_port = elem.second.value_or(http_port);
                            } else {
                                return Status::InvalidConfig("'http_port' field isn't integer.");
                            }

                            auto http_port_option = std::make_unique<IntegerOption>(HTTP_PORT_OPTION_NAME, http_port, 65535, 1024);
                            if (!http_port_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid HTTP port: {}", http_port));
                            }
                            Status status = global_options_.AddOption(std::move(http_port_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kClientPort: {
                            // RPC Client port
                            i64 rpc_client_port = DEFAULT_CLIENT_PORT;
                            if (elem.second.is_integer()) {
                                rpc_client_port = elem.second.value_or(rpc_client_port);
                            } else {
                                return Status::InvalidConfig("'client_port' field isn't integer.");
                            }

                            auto client_port_option = std::make_unique<IntegerOption>(CLIENT_PORT_OPTION_NAME, rpc_client_port, 65535, 1024);
                            if (!client_port_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid client RPC port: {}", rpc_client_port));
                            }
                            Status status = global_options_.AddOption(std::move(client_port_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerRetryDelay: {
                            // Peer retry delay
                            i64 peer_retry_delay = DEFAULT_PEER_RETRY_DELAY;
                            if (elem.second.is_integer()) {
                                peer_retry_delay = elem.second.value_or(peer_retry_delay);
                            } else {
                                return Status::InvalidConfig("'peer_retry_delay' field isn't integer.");
                            }

                            auto peer_retry_delay_option = std::make_unique<IntegerOption>(PEER_RETRY_DELAY_OPTION_NAME, peer_retry_delay, 10000, 0);
                            if (!peer_retry_delay_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid peer retry delay: {}", peer_retry_delay));
                            }
                            Status status = global_options_.AddOption(std::move(peer_retry_delay_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerRetryCount: {
                            // Peer retry num
                            i64 peer_retry_count = DEFAULT_PEER_RETRY_COUNT;
                            if (elem.second.is_integer()) {
                                peer_retry_count = elem.second.value_or(peer_retry_count);
                            } else {
                                return Status::InvalidConfig("'peer_retry_count' field isn't integer.");
                            }

                            auto peer_retry_count_option = std::make_unique<IntegerOption>(PEER_RETRY_COUNT_OPTION_NAME, peer_retry_count, 10, 0);
                            if (!peer_retry_count_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid peer retry num: {}", peer_retry_count));
                            }
                            Status status = global_options_.AddOption(std::move(peer_retry_count_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerConnectTimeout: {
                            // Peer connect timeout
                            i64 peer_connect_timeout = DEFAULT_PEER_CONNECT_TIMEOUT;
                            if (elem.second.is_integer()) {
                                peer_connect_timeout = elem.second.value_or(peer_connect_timeout);
                            } else {
                                return Status::InvalidConfig("'peer_connect_timeout' field isn't integer.");
                            }

                            auto peer_connect_timeout_option =
                                std::make_unique<IntegerOption>(PEER_CONNECT_TIMEOUT_OPTION_NAME, peer_connect_timeout, 10000, 0);
                            if (!peer_connect_timeout_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid peer connect timeout: {}", peer_connect_timeout));
                            }
                            Status status = global_options_.AddOption(std::move(peer_connect_timeout_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerRecvTimeout: {
                            // Peer recv timeout
                            i64 peer_recv_timeout = DEFAULT_PEER_RECV_TIMEOUT;
                            if (elem.second.is_integer()) {
                                peer_recv_timeout = elem.second.value_or(peer_recv_timeout);
                            } else {
                                return Status::InvalidConfig("'peer_recv_timeout' field isn't integer.");
                            }

                            auto peer_recv_timeout_option =
                                std::make_unique<IntegerOption>(PEER_RECV_TIMEOUT_OPTION_NAME, peer_recv_timeout, 10000, 0);
                            if (!peer_recv_timeout_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid peer recv timeout: {}", peer_recv_timeout));
                            }
                            Status status = global_options_.AddOption(std::move(peer_recv_timeout_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerSendTimeout: {
                            // Peer send timeout
                            i64 peer_send_timeout = DEFAULT_PEER_SEND_TIMEOUT;
                            if (elem.second.is_integer()) {
                                peer_send_timeout = elem.second.value_or(peer_send_timeout);
                            } else {
                                return Status::InvalidConfig("'peer_send_timeout' field isn't integer.");
                            }

                            auto peer_send_timeout_option =
                                std::make_unique<IntegerOption>(PEER_SEND_TIMEOUT_OPTION_NAME, peer_send_timeout, 10000, 0);
                            if (!peer_send_timeout_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid peer send timeout: {}", peer_send_timeout));
                            }
                            Status status = global_options_.AddOption(std::move(peer_send_timeout_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kConnectionPoolSize: {
                            // Client pool size
                            i64 connection_pool_size = 256;
                            if (elem.second.is_integer()) {
                                connection_pool_size = elem.second.value_or(connection_pool_size);
                            } else {
                                return Status::InvalidConfig("'connection_pool_size' field isn't integer.");
                            }

                            auto connection_pool_size_option =
                                std::make_unique<IntegerOption>(CONNECTION_POOL_SIZE_OPTION_NAME, connection_pool_size, 65536, 1);
                            if (!connection_pool_size_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid connection pool size: {}", connection_pool_size));
                            }

                            Status status = global_options_.AddOption(std::move(connection_pool_size_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPeerServerConnectionPoolSize: {
                            // Client pool size
                            i64 peer_server_connection_pool_size = 64;
                            if (elem.second.is_integer()) {
                                peer_server_connection_pool_size = elem.second.value_or(peer_server_connection_pool_size);
                            } else {
                                return Status::InvalidConfig("'peer_server_connection_pool_size' field isn't integer.");
                            }

                            auto peer_server_connection_pool_size_option =
                                std::make_unique<IntegerOption>(PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME,
                                                                peer_server_connection_pool_size,
                                                                65536,
                                                                1);
                            if (!peer_server_connection_pool_size_option->Validate()) {
                                return Status::InvalidConfig(
                                    fmt::format("Invalid peer server connection pool size: {}", peer_server_connection_pool_size));
                            }

                            Status status = global_options_.AddOption(std::move(peer_server_connection_pool_size_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'network' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kServerAddress) == nullptr) {
                    // Server address
                    std::string server_address_str = "0.0.0.0";
                    auto server_address_option = std::make_unique<StringOption>(SERVER_ADDRESS_OPTION_NAME, server_address_str);
                    Status status = global_options_.AddOption(std::move(server_address_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerServerIP) == nullptr) {
                    // Peer server address
                    std::string peer_server_ip_str = "0.0.0.0";
                    auto peer_server_ip_str_option = std::make_unique<StringOption>(PEER_SERVER_IP_OPTION_NAME, peer_server_ip_str);
                    Status status = global_options_.AddOption(std::move(peer_server_ip_str_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerServerPort) == nullptr) {
                    // Peer server port
                    i64 pg_port = DEFAULT_PEER_PORT;
                    auto peer_server_port_option = std::make_unique<IntegerOption>(PEER_SERVER_PORT_OPTION_NAME, pg_port, 65535, 1024);
                    Status status = global_options_.AddOption(std::move(peer_server_port_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPostgresPort) == nullptr) {
                    // Postgres port
                    i64 pg_port = DEFAULT_POSTGRES_PORT;
                    auto pg_port_option = std::make_unique<IntegerOption>(POSTGRES_PORT_OPTION_NAME, pg_port, 65535, 1024);
                    Status status = global_options_.AddOption(std::move(pg_port_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kHTTPPort) == nullptr) {
                    // HTTP port
                    i64 http_port = DEFAULT_HTTP_PORT;
                    auto http_port_option = std::make_unique<IntegerOption>(HTTP_PORT_OPTION_NAME, http_port, 65535, 1024);
                    Status status = global_options_.AddOption(std::move(http_port_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kClientPort) == nullptr) {
                    // RPC Client port
                    i64 rpc_client_port = DEFAULT_CLIENT_PORT;
                    auto client_port_option = std::make_unique<IntegerOption>(CLIENT_PORT_OPTION_NAME, rpc_client_port, 65535, 1024);
                    Status status = global_options_.AddOption(std::move(client_port_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerRetryDelay) == nullptr) {
                    // Peer retry delay
                    i64 peer_retry_delay = DEFAULT_PEER_RETRY_DELAY;
                    auto peer_retry_delay_option = std::make_unique<IntegerOption>(PEER_RETRY_DELAY_OPTION_NAME, peer_retry_delay, 10000, 0);
                    Status status = global_options_.AddOption(std::move(peer_retry_delay_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerRetryCount) == nullptr) {
                    // Peer retry num
                    i64 peer_retry_count = DEFAULT_PEER_RETRY_COUNT;
                    auto peer_retry_count_option = std::make_unique<IntegerOption>(PEER_RETRY_COUNT_OPTION_NAME, peer_retry_count, 10, 0);
                    Status status = global_options_.AddOption(std::move(peer_retry_count_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerConnectTimeout) == nullptr) {
                    // Peer connect timeout
                    i64 peer_connect_timeout = DEFAULT_PEER_CONNECT_TIMEOUT;
                    auto peer_connect_timeout_option =
                        std::make_unique<IntegerOption>(PEER_CONNECT_TIMEOUT_OPTION_NAME, peer_connect_timeout, 10000, 0);
                    Status status = global_options_.AddOption(std::move(peer_connect_timeout_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerRecvTimeout) == nullptr) {
                    // Peer recv timeout
                    i64 peer_recv_timeout = DEFAULT_PEER_RECV_TIMEOUT;
                    auto peer_recv_timeout_option = std::make_unique<IntegerOption>(PEER_RECV_TIMEOUT_OPTION_NAME, peer_recv_timeout, 10000, 0);
                    Status status = global_options_.AddOption(std::move(peer_recv_timeout_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerSendTimeout) == nullptr) {
                    // Peer send timeout
                    i64 peer_send_timeout = DEFAULT_PEER_SEND_TIMEOUT;
                    auto peer_send_timeout_option = std::make_unique<IntegerOption>(PEER_SEND_TIMEOUT_OPTION_NAME, peer_send_timeout, 10000, 0);
                    Status status = global_options_.AddOption(std::move(peer_send_timeout_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kConnectionPoolSize) == nullptr) {
                    // Client pool size
                    i64 connection_pool_size = 256;
                    auto connection_pool_size_option =
                        std::make_unique<IntegerOption>(CONNECTION_POOL_SIZE_OPTION_NAME, connection_pool_size, 65536, 1);
                    Status status = global_options_.AddOption(std::move(connection_pool_size_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPeerServerConnectionPoolSize) == nullptr) {
                    // peer server pool size
                    i64 peer_server_connection_pool_size = 64;
                    auto peer_server_connection_pool_size_option =
                        std::make_unique<IntegerOption>(PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME, peer_server_connection_pool_size, 65536, 1);
                    Status status = global_options_.AddOption(std::move(peer_server_connection_pool_size_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
            } else {
                return Status::InvalidConfig("No 'network' section in configure file.");
            }
        }

        // Log
        {
            if (config_toml.contains("log")) {

                auto log_config = config_toml["log"];
                auto log_config_table = log_config.as_table();
                for (auto &elem : *log_config_table) {

                    std::string var_name = std::string(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'log' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kLogFileName: {
                            // Log file name
                            std::string log_filename = "infinity.log";
                            if (elem.second.is_string()) {
                                log_filename = elem.second.value_or(log_filename);
                            } else {
                                return Status::InvalidConfig("'log_filename' field isn't string.");
                            }

                            auto log_file_name_option = std::make_unique<StringOption>(LOG_FILENAME_OPTION_NAME, log_filename);
                            Status status = global_options_.AddOption(std::move(log_file_name_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogDir: {
                            // Log dir
                            std::string log_filename = "/var/infinity/log";
                            if (elem.second.is_string()) {
                                log_filename = elem.second.value_or(log_filename);
                            } else {
                                return Status::InvalidConfig("'log_dir' field isn't string.");
                            }

                            auto log_dir_option = std::make_unique<StringOption>(LOG_DIR_OPTION_NAME, log_filename);
                            Status status = global_options_.AddOption(std::move(log_dir_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogToStdout: {
                            // Log To Stdout
                            bool log_to_stdout = true;
                            if (elem.second.is_boolean()) {
                                log_to_stdout = elem.second.value_or(log_to_stdout);
                            } else {
                                return Status::InvalidConfig("'log_to_stdout' field isn't boolean.");
                            }

                            std::unique_ptr<BooleanOption> log_to_stdout_option =
                                std::make_unique<BooleanOption>(LOG_TO_STDOUT_OPTION_NAME, log_to_stdout);
                            Status status = global_options_.AddOption(std::move(log_to_stdout_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogFileMaxSize: {
                            // Log File Max Size
                            i64 log_file_max_size = DEFAULT_LOG_FILE_SIZE;
                            if (elem.second.is_string()) {
                                std::string log_file_max_size_str = elem.second.value_or(DEFAULT_LOG_FILE_SIZE_STR.data());
                                auto res = ParseByteSize(log_file_max_size_str, log_file_max_size);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'log_file_max_size' field isn't integer.");
                            }

                            auto log_file_max_size_option = std::make_unique<IntegerOption>(LOG_FILE_MAX_SIZE_OPTION_NAME,
                                                                                            log_file_max_size,
                                                                                            std::numeric_limits<i64>::max(),
                                                                                            1024lu * 1024lu);

                            if (!log_file_max_size_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid max log file size: {}", log_file_max_size));
                            }
                            Status status = global_options_.AddOption(std::move(log_file_max_size_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogFileRotateCount: {
                            // Log File Rotate count
                            i64 log_file_rotate_count = 8;
                            if (elem.second.is_integer()) {
                                log_file_rotate_count = elem.second.value_or(log_file_rotate_count);
                            } else {
                                return Status::InvalidConfig("'log_file_rotate_count' field isn't integer.");
                            }

                            auto log_file_rotate_count_option =
                                std::make_unique<IntegerOption>(LOG_FILE_ROTATE_COUNT_OPTION_NAME, log_file_rotate_count, 65536, 1);

                            if (!log_file_rotate_count_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid log file rotate count: {}", log_file_rotate_count));
                            }
                            Status status = global_options_.AddOption(std::move(log_file_rotate_count_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogLevel: {
                            // Log Level
                            LogLevel log_level = LogLevel::kInfo;
                            if (elem.second.is_string()) {
                                std::string log_level_str = elem.second.value_or("info");
                                ToLower(log_level_str);
                                if (log_level_str == "trace") {
                                    log_level = LogLevel::kTrace;
                                } else if (log_level_str == "debug") {
                                    log_level = LogLevel::kDebug;
                                } else if (log_level_str == "info") {
                                    log_level = LogLevel::kInfo;
                                } else if (log_level_str == "warning") {
                                    log_level = LogLevel::kWarning;
                                } else if (log_level_str == "error") {
                                    log_level = LogLevel::kError;
                                } else if (log_level_str == "critical") {
                                    log_level = LogLevel::kCritical;
                                } else {
                                    return Status::InvalidLogLevel(log_level_str);
                                }
                            } else {
                                return Status::InvalidConfig("'log_level' field isn't string.");
                            }

                            std::unique_ptr<LogLevelOption> log_level_option = std::make_unique<LogLevelOption>(LOG_LEVEL_OPTION_NAME, log_level);
                            Status status = global_options_.AddOption(std::move(log_level_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'buffer' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLogFileName) == nullptr) {
                    // Log file name
                    std::string log_filename = "infinity.log";
                    auto log_file_name_option = std::make_unique<StringOption>(LOG_FILENAME_OPTION_NAME, log_filename);
                    Status status = global_options_.AddOption(std::move(log_file_name_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLogDir) == nullptr) {
                    // Log dir
                    std::string log_dir = "/var/infinity/log";
                    auto log_dir_option = std::make_unique<StringOption>(LOG_DIR_OPTION_NAME, log_dir);
                    Status status = global_options_.AddOption(std::move(log_dir_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLogToStdout) == nullptr) {
                    // Log To Stdout
                    bool log_to_stdout = true;
                    if (default_config != nullptr) {
                        log_to_stdout = default_config->default_log_to_stdout_;
                    }
                    std::unique_ptr<BooleanOption> log_to_stdout_option = std::make_unique<BooleanOption>(LOG_TO_STDOUT_OPTION_NAME, log_to_stdout);
                    Status status = global_options_.AddOption(std::move(log_to_stdout_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLogFileMaxSize) == nullptr) {
                    // Log File Max Size
                    i64 log_file_max_size = 1024lu * 1024lu * 1024lu;
                    auto log_file_max_size_option = std::make_unique<IntegerOption>(LOG_FILE_MAX_SIZE_OPTION_NAME,
                                                                                    log_file_max_size,
                                                                                    std::numeric_limits<i64>::max(),
                                                                                    1024lu * 1024lu);
                    Status status = global_options_.AddOption(std::move(log_file_max_size_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLogFileRotateCount) == nullptr) {
                    // Log File Rotate count
                    i64 log_file_rotate_count = 8;
                    auto log_file_rotate_count_option =
                        std::make_unique<IntegerOption>(LOG_FILE_ROTATE_COUNT_OPTION_NAME, log_file_rotate_count, 65536, 1);
                    Status status = global_options_.AddOption(std::move(log_file_rotate_count_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLogLevel) == nullptr) {
                    // Log Level
                    LogLevel log_level = LogLevel::kInfo;
                    if (default_config != nullptr) {
                        log_level = default_config->default_log_level_;
                    }
                    std::unique_ptr<LogLevelOption> log_level_option = std::make_unique<LogLevelOption>(LOG_LEVEL_OPTION_NAME, log_level);
                    Status status = global_options_.AddOption(std::move(log_level_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
            } else {
                return Status::InvalidConfig("No 'Log' section in configure file.");
            }
        }

        // Storage
        {
            if (config_toml.contains("storage")) {
                auto storage_config = config_toml["storage"];
                auto storage_config_table = storage_config.as_table();
                for (auto &elem : *storage_config_table) {

                    std::string var_name = std::string(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'storage' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kReplayWal: {
                            // Replay wal
                            bool replay_wal = true;
                            if (elem.second.is_boolean()) {
                                replay_wal = elem.second.value_or(replay_wal);
                            } else {
                                return Status::InvalidConfig("'replay_wal' field isn't boolean.");
                            }
                            std::unique_ptr<BooleanOption> replay_wal_option = std::make_unique<BooleanOption>(REPLAY_WAL_OPTION_NAME, replay_wal);
                            Status status = global_options_.AddOption(std::move(replay_wal_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kDataDir: {
                            // Data Dir
                            std::string data_dir = "/var/infinity/data";
                            if (elem.second.is_string()) {
                                data_dir = elem.second.value_or(data_dir);
                            } else {
                                return Status::InvalidConfig("'data_dir' field isn't string.");
                            }

                            auto data_dir_option = std::make_unique<StringOption>(DATA_DIR_OPTION_NAME, data_dir);
                            Status status = global_options_.AddOption(std::move(data_dir_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kCatalogDir: {
                            // Catalog Dir
                            std::string catalog_dir = "/var/infinity/catalog";
                            if (elem.second.is_string()) {
                                catalog_dir = elem.second.value_or(catalog_dir);
                            } else {
                                return Status::InvalidConfig("'catalog_dir' field isn't string.");
                            }

                            auto catalog_dir_option = std::make_unique<StringOption>(CATALOG_DIR_OPTION_NAME, catalog_dir);
                            Status status = global_options_.AddOption(std::move(catalog_dir_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPersistenceDir: {
                            std::string persistence_dir;
                            if (elem.second.is_string()) {
                                persistence_dir = elem.second.value_or(DEFAULT_PERSISTENCE_DIR.data());
                            } else {
                                return Status::InvalidConfig("'persistence_dir' field isn't string, such as \"persistence\"");
                            }
                            auto persistence_dir_option = std::make_unique<StringOption>(PERSISTENCE_DIR_OPTION_NAME, persistence_dir);
                            global_options_.AddOption(std::move(persistence_dir_option));
                            break;
                        }
                        case GlobalOptionIndex::kPersistenceObjectSizeLimit: {
                            i64 persistence_object_size_limit;
                            if (elem.second.is_string()) {
                                std::string persistence_object_size_limit_str =
                                    elem.second.value_or(DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT_STR.data());
                                auto res = ParseByteSize(persistence_object_size_limit_str, persistence_object_size_limit);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'persistence_object_size_limit' field isn't string, such as \"100MB\"");
                            }
                            auto persistence_object_size_limit_option = std::make_unique<IntegerOption>(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME,
                                                                                                        persistence_object_size_limit,
                                                                                                        std::numeric_limits<i64>::max(),
                                                                                                        0);
                            if (!persistence_object_size_limit_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid persistence_object_size_limit: {}", persistence_object_size_limit));
                            }
                            global_options_.AddOption(std::move(persistence_object_size_limit_option));
                            break;
                        }
                        case GlobalOptionIndex::kCleanupInterval: {
                            // Cleanup Interval
                            i64 cleanup_interval = DEFAULT_CLEANUP_INTERVAL_SEC;
                            if (elem.second.is_string()) {
                                std::string cleanup_interval_str = elem.second.value_or(DEFAULT_CLEANUP_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(cleanup_interval_str, cleanup_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'cleanup_interval' field isn't string, such as \"1m\".");
                            }

                            auto cleanup_interval_option = std::make_unique<IntegerOption>(CLEANUP_INTERVAL_OPTION_NAME,
                                                                                           cleanup_interval,
                                                                                           MAX_CLEANUP_INTERVAL_SEC,
                                                                                           MIN_CLEANUP_INTERVAL_SEC);
                            if (!cleanup_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid cleanup interval: {}", cleanup_interval));
                            }
                            Status status = global_options_.AddOption(std::move(cleanup_interval_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kCompactInterval: {
                            // Compact Interval
                            i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
                            if (elem.second.is_string()) {
                                std::string compact_interval_str = elem.second.value_or(DEFAULT_COMPACT_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(compact_interval_str, compact_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'compact_interval' field isn't string, such as \"1m\".");
                            }

                            auto compact_interval_option = std::make_unique<IntegerOption>(COMPACT_INTERVAL_OPTION_NAME,
                                                                                           compact_interval,
                                                                                           MAX_COMPACT_INTERVAL_SEC,
                                                                                           MIN_COMPACT_INTERVAL_SEC);
                            if (!compact_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid compact interval: {}", compact_interval));
                            }
                            Status status = global_options_.AddOption(std::move(compact_interval_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kOptimizeIndexInterval: {
                            // Optimize Index Interval
                            i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
                            if (elem.second.is_string()) {
                                std::string optimize_index_interval_str = elem.second.value_or(DEFAULT_OPTIMIZE_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(optimize_index_interval_str, optimize_index_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'optimize_interval' field isn't string, such as \"1m\".");
                            }

                            auto optimize_interval_option = std::make_unique<IntegerOption>(OPTIMIZE_INTERVAL_OPTION_NAME,
                                                                                            optimize_index_interval,
                                                                                            MAX_COMPACT_INTERVAL_SEC,
                                                                                            MIN_COMPACT_INTERVAL_SEC);
                            if (!optimize_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid optimize interval: {}", optimize_index_interval));
                            }
                            Status status = global_options_.AddOption(std::move(optimize_interval_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kMemIndexCapacity: {
                            // Mem Index Capacity
                            i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
                            if (elem.second.is_integer()) {
                                mem_index_capacity = elem.second.value_or(mem_index_capacity);
                            } else {
                                return Status::InvalidConfig("'mem_index_capacity' field isn't integer.");
                            }

                            auto mem_index_capacity_option = std::make_unique<IntegerOption>(MEM_INDEX_CAPACITY_OPTION_NAME,
                                                                                             mem_index_capacity,
                                                                                             MAX_MEMINDEX_CAPACITY,
                                                                                             MIN_MEMINDEX_CAPACITY);
                            if (!mem_index_capacity_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid memory index capacity: {}", mem_index_capacity));
                            }
                            Status status = global_options_.AddOption(std::move(mem_index_capacity_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kSnapshotDir: {
                            std::string snapshot_dir;
                            if (elem.second.is_string()) {
                                snapshot_dir = elem.second.value_or(DEFAULT_SNAPSHOT_DIR.data());
                            } else {
                                return Status::InvalidConfig("'snapshot_dir' field isn't string, such as \"snapshot\"");
                            }
                            auto snapshot_dir_option = std::make_unique<StringOption>(SNAPSHOT_DIR_OPTION_NAME, snapshot_dir);
                            global_options_.AddOption(std::move(snapshot_dir_option));
                            break;
                        }
                        case GlobalOptionIndex::kStorageType: {
                            // File System Type
                            std::string storage_type_str = std::string(DEFAULT_STORAGE_TYPE);
                            if (elem.second.is_string()) {
                                storage_type_str = elem.second.value_or(storage_type_str);
                            } else {
                                return Status::InvalidConfig("'storage_type' field isn't string.");
                            }

                            auto storage_type_option = std::make_unique<StringOption>(STORAGE_TYPE_OPTION_NAME, storage_type_str);
                            Status status = global_options_.AddOption(std::move(storage_type_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kObjectStorage: {
                            const auto &object_storage_config = elem.second;
                            const auto &object_storage_config_table = object_storage_config.as_table();
                            for (auto &elem : *object_storage_config_table) {
                                std::string var_name = std::string(elem.first);
                                GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                                if (option_index == GlobalOptionIndex::kInvalid) {
                                    return Status::InvalidConfig(
                                        fmt::format("Unrecognized config parameter: {} in 'storage.object_storage' field", var_name));
                                }
                                switch (option_index) {
                                    case GlobalOptionIndex::kObjectStorageUrl: {
                                        std::string object_storage_url_str;
                                        if (elem.second.is_string()) {
                                            std::optional<std::string> str_optional = elem.second.value<std::string>();
                                            if (!str_optional.has_value()) {
                                                return Status::InvalidConfig("'url' field in [storage.object_storage] isn't string");
                                            }
                                            object_storage_url_str = *str_optional;
                                        } else {
                                            return Status::InvalidConfig("'url' field in [storage.object_storage] isn't string");
                                        }
                                        auto object_storage_url_option =
                                            std::make_unique<StringOption>(OBJECT_STORAGE_URL_OPTION_NAME, object_storage_url_str);
                                        global_options_.AddOption(std::move(object_storage_url_option));
                                        break;
                                    }
                                    case GlobalOptionIndex::kObjectStorageBucket: {
                                        std::string object_storage_bucket = std::string(DEFAULT_OBJECT_STORAGE_BUCKET);
                                        if (elem.second.is_string()) {
                                            object_storage_bucket = elem.second.value_or(object_storage_bucket);
                                        } else {
                                            return Status::InvalidConfig("'host' field in [storage.object_storage] isn't string");
                                        }
                                        auto object_bucket_option =
                                            std::make_unique<StringOption>(OBJECT_STORAGE_BUCKET_OPTION_NAME, object_storage_bucket);
                                        global_options_.AddOption(std::move(object_bucket_option));
                                        break;
                                    }
                                    case GlobalOptionIndex::kObjectStorageAccessKey: {
                                        std::string object_storage_access_key_str;
                                        if (elem.second.is_string()) {
                                            std::optional<std::string> str_optional = elem.second.value<std::string>();
                                            if (!str_optional.has_value()) {
                                                return Status::InvalidConfig("'access_key' field in [storage.object_storage] isn't string");
                                            }
                                            object_storage_access_key_str = *str_optional;
                                        } else {
                                            return Status::InvalidConfig("'access_key' field in [storage.object_storage] isn't string");
                                        }
                                        auto object_storage_access_key_option =
                                            std::make_unique<StringOption>(OBJECT_STORAGE_ACCESS_KEY_OPTION_NAME, object_storage_access_key_str);
                                        global_options_.AddOption(std::move(object_storage_access_key_option));
                                        break;
                                    }
                                    case GlobalOptionIndex::kObjectStorageSecretKey: {
                                        std::string object_storage_secret_key_str;
                                        if (elem.second.is_string()) {
                                            std::optional<std::string> str_optional = elem.second.value<std::string>();
                                            if (!str_optional.has_value()) {
                                                return Status::InvalidConfig("'secret_key' field in [storage.object_storage] isn't string");
                                            }
                                            object_storage_secret_key_str = *str_optional;
                                        } else {
                                            return Status::InvalidConfig("'secret_key' field in [storage.object_storage] isn't string");
                                        }
                                        auto object_storage_secret_key_option =
                                            std::make_unique<StringOption>(OBJECT_STORAGE_SECRET_KEY_OPTION_NAME, object_storage_secret_key_str);
                                        global_options_.AddOption(std::move(object_storage_secret_key_option));
                                        break;
                                    }
                                    case GlobalOptionIndex::kObjectStorageHttps: {
                                        bool https = false;
                                        if (elem.second.is_boolean()) {
                                            https = elem.second.value_or(https);
                                        } else {
                                            return Status::InvalidConfig("'enable_https' field isn't boolean.");
                                        }

                                        std::unique_ptr<BooleanOption> object_storage_https_option =
                                            std::make_unique<BooleanOption>(OBJECT_STORAGE_ENABLE_HTTPS_OPTION_NAME, https);
                                        Status status = global_options_.AddOption(std::move(object_storage_https_option));
                                        if (!status.ok()) {
                                            UnrecoverableError(status.message());
                                        }
                                        break;
                                    }
                                    default: {
                                        return Status::InvalidConfig(
                                            fmt::format("Unrecognized config parameter: {} in 'storage.object_storage' field", var_name));
                                    }
                                }
                            }
                            if (global_options_.GetOptionByIndex(GlobalOptionIndex::kObjectStorageUrl) == nullptr) {
                                return Status::InvalidConfig("No 'url' field in [storage.object_storage]");
                            }
                            if (global_options_.GetOptionByIndex(GlobalOptionIndex::kObjectStorageBucket) == nullptr) {
                                std::string object_storage_bucket = std::string(DEFAULT_OBJECT_STORAGE_BUCKET);
                                auto object_bucket_option = std::make_unique<StringOption>(OBJECT_STORAGE_BUCKET_OPTION_NAME, object_storage_bucket);
                                Status status = global_options_.AddOption(std::move(object_bucket_option));
                                if (!status.ok()) {
                                    UnrecoverableError(status.message());
                                }
                            }
                            if (global_options_.GetOptionByIndex(GlobalOptionIndex::kObjectStorageAccessKey) == nullptr) {
                                return Status::InvalidConfig("No 'access_key' field in [storage.object_storage]");
                            }
                            if (global_options_.GetOptionByIndex(GlobalOptionIndex::kObjectStorageSecretKey) == nullptr) {
                                return Status::InvalidConfig("No 'secret_key' field in [storage.object_storage]");
                            }
                            if (global_options_.GetOptionByIndex(GlobalOptionIndex::kObjectStorageHttps) == nullptr) {
                                return Status::InvalidConfig("No 'enable_https' field in [storage.object_storage]");
                            }
                            break;
                        }

                        case GlobalOptionIndex::kDenseIndexBuildingWorker: {
                            i64 dense_index_building_worker = std::thread::hardware_concurrency() / 2;
                            if (elem.second.is_integer()) {
                                dense_index_building_worker = elem.second.value_or(dense_index_building_worker);
                            } else {
                                return Status::InvalidConfig("'lru_num' field isn't integer.");
                            }
                            auto dense_index_building_worker_option = std::make_unique<IntegerOption>(DENSE_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                                      dense_index_building_worker,
                                                                                                      std::thread::hardware_concurrency(),
                                                                                                      1);
                            if (!dense_index_building_worker_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid dense vector index building number: {}", 0));
                            }
                            global_options_.AddOption(std::move(dense_index_building_worker_option));
                            break;
                        }
                        case GlobalOptionIndex::kSparseIndexBuildingWorker: {
                            i64 sparse_index_building_worker = std::thread::hardware_concurrency() / 2;
                            if (elem.second.is_integer()) {
                                sparse_index_building_worker = elem.second.value_or(sparse_index_building_worker);
                            } else {
                                return Status::InvalidConfig("'lru_num' field isn't integer.");
                            }
                            auto sparse_index_building_worker_option = std::make_unique<IntegerOption>(SPARSE_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                                       sparse_index_building_worker,
                                                                                                       std::thread::hardware_concurrency(),
                                                                                                       1);
                            if (!sparse_index_building_worker_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid sparse vector index building number: {}", 0));
                            }
                            global_options_.AddOption(std::move(sparse_index_building_worker_option));
                            break;
                        }
                        case GlobalOptionIndex::kFulltextIndexBuildingWorker: {
                            i64 fulltext_index_building_worker = std::thread::hardware_concurrency() / 2;
                            if (elem.second.is_integer()) {
                                fulltext_index_building_worker = elem.second.value_or(fulltext_index_building_worker);
                            } else {
                                return Status::InvalidConfig("'lru_num' field isn't integer.");
                            }
                            auto fulltext_index_building_worker_option = std::make_unique<IntegerOption>(FULLTEXT_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                                         fulltext_index_building_worker,
                                                                                                         std::thread::hardware_concurrency(),
                                                                                                         1);
                            if (!fulltext_index_building_worker_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid fulltext vector index building number: {}", 0));
                            }
                            global_options_.AddOption(std::move(fulltext_index_building_worker_option));
                            break;
                        }
                        case GlobalOptionIndex::kBottomExecutorWorker: {
                            i64 bottom_executor_worker = std::thread::hardware_concurrency() / 2;
                            if (bottom_executor_worker < 2) {
                                bottom_executor_worker = 2;
                            }
                            if (elem.second.is_integer()) {
                                bottom_executor_worker = elem.second.value_or(bottom_executor_worker);
                            } else {
                                return Status::InvalidConfig("'lru_num' field isn't integer.");
                            }
                            auto bottom_executor_worker_option = std::make_unique<IntegerOption>(BOTTOM_EXECUTOR_WORKER_OPTION_NAME,
                                                                                                 bottom_executor_worker,
                                                                                                 std::thread::hardware_concurrency(),
                                                                                                 1);
                            if (!bottom_executor_worker_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid fulltext vector index building number: {}", 0));
                            }
                            global_options_.AddOption(std::move(bottom_executor_worker_option));
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'storage' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kReplayWal) == nullptr) {
                    bool replay_wal = true;
                    std::unique_ptr<BooleanOption> replay_wal_option = std::make_unique<BooleanOption>(REPLAY_WAL_OPTION_NAME, replay_wal);
                    Status status = global_options_.AddOption(std::move(replay_wal_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPersistenceDir) == nullptr) {
                    std::string persistence_dir =
                        (global_options_.GetOptionByIndex(GlobalOptionIndex::kDataDir) == nullptr) ? DEFAULT_PERSISTENCE_DIR.data() : "";
                    auto persistence_dir_option = std::make_unique<StringOption>(PERSISTENCE_DIR_OPTION_NAME, persistence_dir);
                    global_options_.AddOption(std::move(persistence_dir_option));
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPersistenceObjectSizeLimit) == nullptr) {
                    i64 persistence_object_size_limit = DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT;
                    auto persistence_object_size_limit_option = std::make_unique<IntegerOption>(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME,
                                                                                                persistence_object_size_limit,
                                                                                                std::numeric_limits<i64>::max(),
                                                                                                0);
                    global_options_.AddOption(std::move(persistence_object_size_limit_option));
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kDataDir) == nullptr) {
                    // Data Dir
                    std::string data_dir = "/var/infinity/data";
                    auto data_dir_option = std::make_unique<StringOption>(DATA_DIR_OPTION_NAME, data_dir);
                    Status status = global_options_.AddOption(std::move(data_dir_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kCatalogDir) == nullptr) {
                    // Catalog Dir
                    std::string catalog_dir = "/var/infinity/catalog";
                    auto catalog_dir_option = std::make_unique<StringOption>(CATALOG_DIR_OPTION_NAME, catalog_dir);
                    Status status = global_options_.AddOption(std::move(catalog_dir_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kCleanupInterval) == nullptr) {
                    // Cleanup Interval
                    i64 cleanup_interval = DEFAULT_CLEANUP_INTERVAL_SEC;
                    auto cleanup_interval_option = std::make_unique<IntegerOption>(CLEANUP_INTERVAL_OPTION_NAME,
                                                                                   cleanup_interval,
                                                                                   MAX_CLEANUP_INTERVAL_SEC,
                                                                                   MIN_CLEANUP_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(cleanup_interval_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kCompactInterval) == nullptr) {
                    // Compact Interval
                    i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
                    auto compact_interval_option = std::make_unique<IntegerOption>(COMPACT_INTERVAL_OPTION_NAME,
                                                                                   compact_interval,
                                                                                   MAX_COMPACT_INTERVAL_SEC,
                                                                                   MIN_COMPACT_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(compact_interval_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kOptimizeIndexInterval) == nullptr) {
                    // Optimize Index Interval
                    i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
                    auto optimize_interval_option = std::make_unique<IntegerOption>(OPTIMIZE_INTERVAL_OPTION_NAME,
                                                                                    optimize_index_interval,
                                                                                    MAX_COMPACT_INTERVAL_SEC,
                                                                                    MIN_COMPACT_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(optimize_interval_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kMemIndexCapacity) == nullptr) {
                    // Mem Index Capacity
                    i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
                    auto mem_index_capacity_option = std::make_unique<IntegerOption>(MEM_INDEX_CAPACITY_OPTION_NAME,
                                                                                     mem_index_capacity,
                                                                                     MAX_MEMINDEX_CAPACITY,
                                                                                     MIN_MEMINDEX_CAPACITY);
                    Status status = global_options_.AddOption(std::move(mem_index_capacity_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kSnapshotDir) == nullptr) {
                    std::string snapshot_dir = DEFAULT_SNAPSHOT_DIR.data();
                    auto snapshot_dir_option = std::make_unique<StringOption>(SNAPSHOT_DIR_OPTION_NAME, snapshot_dir);
                    global_options_.AddOption(std::move(snapshot_dir_option));
                }

                if (BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kStorageType); base_option == nullptr) {
                    std::string storage_type_str = std::string(DEFAULT_STORAGE_TYPE);
                    auto storage_type_option = std::make_unique<StringOption>(STORAGE_TYPE_OPTION_NAME, storage_type_str);
                    Status status = global_options_.AddOption(std::move(storage_type_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kDenseIndexBuildingWorker) == nullptr) {
                    // dense index building worker
                    i64 dense_index_building_worker = std::thread::hardware_concurrency() / 2;
                    if (dense_index_building_worker < 2) {
                        dense_index_building_worker = 2;
                    }
                    auto dense_index_building_worker_option = std::make_unique<IntegerOption>(DENSE_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                              dense_index_building_worker,
                                                                                              std::thread::hardware_concurrency(),
                                                                                              1);
                    Status status = global_options_.AddOption(std::move(dense_index_building_worker_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kSparseIndexBuildingWorker) == nullptr) {
                    // sparse index building worker
                    i64 sparse_index_building_worker = std::thread::hardware_concurrency() / 2;
                    if (sparse_index_building_worker < 2) {
                        sparse_index_building_worker = 2;
                    }
                    auto sparse_index_building_worker_option = std::make_unique<IntegerOption>(SPARSE_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                               sparse_index_building_worker,
                                                                                               std::thread::hardware_concurrency(),
                                                                                               1);
                    Status status = global_options_.AddOption(std::move(sparse_index_building_worker_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kFulltextIndexBuildingWorker) == nullptr) {
                    // fulltext index building worker
                    i64 fulltext_index_building_worker = std::thread::hardware_concurrency() / 2;
                    if (fulltext_index_building_worker < 2) {
                        fulltext_index_building_worker = 2;
                    }
                    auto fulltext_index_building_worker_option = std::make_unique<IntegerOption>(FULLTEXT_INDEX_BUILDING_WORKER_OPTION_NAME,
                                                                                                 fulltext_index_building_worker,
                                                                                                 std::thread::hardware_concurrency(),
                                                                                                 1);
                    Status status = global_options_.AddOption(std::move(fulltext_index_building_worker_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kBottomExecutorWorker) == nullptr) {
                    // bottom executor worker
                    i64 bottom_executor_worker = std::thread::hardware_concurrency() / 2;
                    if (bottom_executor_worker < 2) {
                        bottom_executor_worker = 2;
                    }
                    auto bottom_executor_worker_option = std::make_unique<IntegerOption>(BOTTOM_EXECUTOR_WORKER_OPTION_NAME,
                                                                                         bottom_executor_worker,
                                                                                         std::thread::hardware_concurrency(),
                                                                                         1);
                    Status status = global_options_.AddOption(std::move(bottom_executor_worker_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
            } else {
                return Status::InvalidConfig("No 'storage' section in configure file.");
            }
        }

        // Buffer
        {
            if (config_toml.contains("buffer")) {
                auto buffer_config = config_toml["buffer"];
                auto buffer_config_table = buffer_config.as_table();
                for (auto &elem : *buffer_config_table) {

                    std::string var_name = std::string(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'buffer' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kBufferManagerSize: {
                            i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
                            if (elem.second.is_string()) {
                                std::string buffer_manager_size_str = elem.second.value_or(DEFAULT_BUFFER_MANAGER_SIZE_STR.data());
                                auto res = ParseByteSize(buffer_manager_size_str, buffer_manager_size);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'buffer_manager_size' field isn't string, such as \"4GB\"");
                            }
                            auto buffer_manager_size_option = std::make_unique<IntegerOption>(BUFFER_MANAGER_SIZE_OPTION_NAME,
                                                                                              buffer_manager_size,
                                                                                              std::numeric_limits<i64>::max(),
                                                                                              0);
                            if (!buffer_manager_size_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid buffer manager size: {}", buffer_manager_size));
                            }
                            global_options_.AddOption(std::move(buffer_manager_size_option));
                            break;
                        }
                        case GlobalOptionIndex::kLRUNum: {
                            i64 lru_num = DEFAULT_BUFFER_MANAGER_LRU_COUNT;
                            if (elem.second.is_integer()) {
                                lru_num = elem.second.value_or(lru_num);
                            } else {
                                return Status::InvalidConfig("'lru_num' field isn't integer.");
                            }
                            auto lru_num_option = std::make_unique<IntegerOption>(LRU_NUM_OPTION_NAME, lru_num, 100, 1);
                            if (!lru_num_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid LRU num: {}", 0));
                            }
                            global_options_.AddOption(std::move(lru_num_option));
                            break;
                        }
                        case GlobalOptionIndex::kTempDir: {
                            std::string temp_dir = "/var/infinity/tmp";
                            if (elem.second.is_string()) {
                                temp_dir = elem.second.value_or(temp_dir);
                            } else {
                                return Status::InvalidConfig("'temp_dir' field isn't string.");
                            }

                            auto temp_dir_option = std::make_unique<StringOption>(TEMP_DIR_OPTION_NAME, temp_dir);
                            global_options_.AddOption(std::move(temp_dir_option));
                            break;
                        }
                        case GlobalOptionIndex::kMemIndexMemoryQuota: {
                            i64 mem_index_memory_quota = DEFAULT_MEMINDEX_MEMORY_QUOTA;
                            if (elem.second.is_string()) {
                                std::string mem_index_memory_quota_str = elem.second.value_or(DEFAULT_MEMINDEX_MEMORY_QUOTA_STR.data());
                                auto res = ParseByteSize(mem_index_memory_quota_str, mem_index_memory_quota);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'mem_index_memory_quota' field isn't string.");
                            }
                            auto mem_index_memory_quota_option = std::make_unique<IntegerOption>(MEMINDEX_MEMORY_QUOTA_OPTION_NAME,
                                                                                                 mem_index_memory_quota,
                                                                                                 std::numeric_limits<i64>::max(),
                                                                                                 0);
                            global_options_.AddOption(std::move(mem_index_memory_quota_option));
                            break;
                        }
                        case GlobalOptionIndex::kResultCache: {
                            std::string result_cache_str(DEFAULT_RESULT_CACHE);
                            if (elem.second.is_string()) {
                                result_cache_str = elem.second.value_or(result_cache_str);
                            } else {
                                return Status::InvalidConfig("'result_cache' field isn't string.");
                            }
                            auto result_cache_option = std::make_unique<StringOption>(RESULT_CACHE_OPTION_NAME, result_cache_str);
                            global_options_.AddOption(std::move(result_cache_option));
                            break;
                        }
                        case GlobalOptionIndex::kCacheResultCapacity: {
                            i64 cache_result_num = DEFAULT_CACHE_RESULT_CAPACITY;
                            if (elem.second.is_integer()) {
                                cache_result_num = elem.second.value_or(cache_result_num);
                            } else {
                                return Status::InvalidConfig("'cache_result_num' field isn't integer.");
                            }
                            auto cache_result_num_option = std::make_unique<IntegerOption>(CACHE_RESULT_CAPACITY_OPTION_NAME,
                                                                                           cache_result_num,
                                                                                           std::numeric_limits<i64>::max(),
                                                                                           0);
                            global_options_.AddOption(std::move(cache_result_num_option));
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'buffer' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kBufferManagerSize) == nullptr) {
                    // Buffer Manager Size
                    i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
                    auto buffer_manager_size_option =
                        std::make_unique<IntegerOption>(BUFFER_MANAGER_SIZE_OPTION_NAME, buffer_manager_size, std::numeric_limits<i64>::max(), 0);
                    Status status = global_options_.AddOption(std::move(buffer_manager_size_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLRUNum) == nullptr) {
                    // LRU Num
                    i64 lru_num = DEFAULT_BUFFER_MANAGER_LRU_COUNT;
                    auto lru_num_option = std::make_unique<IntegerOption>(LRU_NUM_OPTION_NAME, lru_num, 100, 1);
                    Status status = global_options_.AddOption(std::move(lru_num_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kTempDir) == nullptr) {
                    // Temp Dir
                    std::string temp_dir = "/var/infinity/tmp";
                    auto temp_dir_option = std::make_unique<StringOption>(TEMP_DIR_OPTION_NAME, temp_dir);
                    Status status = global_options_.AddOption(std::move(temp_dir_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kMemIndexMemoryQuota) == nullptr) {
                    // Mem Index Memory Quota
                    i64 mem_index_memory_quota = DEFAULT_MEMINDEX_MEMORY_QUOTA;
                    auto mem_index_memory_quota_option = std::make_unique<IntegerOption>(MEMINDEX_MEMORY_QUOTA_OPTION_NAME,
                                                                                         mem_index_memory_quota,
                                                                                         std::numeric_limits<i64>::max(),
                                                                                         0);
                    Status status = global_options_.AddOption(std::move(mem_index_memory_quota_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kResultCache) == nullptr) {
                    // Result Cache Mode
                    std::string result_cache_str(DEFAULT_RESULT_CACHE);
                    auto result_cache_option = std::make_unique<StringOption>(RESULT_CACHE_OPTION_NAME, result_cache_str);
                    Status status = global_options_.AddOption(std::move(result_cache_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kCacheResultCapacity) == nullptr) {
                    i64 cache_result_num = DEFAULT_CACHE_RESULT_CAPACITY;
                    auto cache_result_num_option =
                        std::make_unique<IntegerOption>(CACHE_RESULT_CAPACITY_OPTION_NAME, cache_result_num, std::numeric_limits<i64>::max(), 0);
                    Status status = global_options_.AddOption(std::move(cache_result_num_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

            } else {
                return Status::InvalidConfig("No 'buffer' section in configure file.");
            }
        }

        // WAL
        {
            if (config_toml.contains("wal")) {
                auto wal_config = config_toml["wal"];

                auto wal_config_table = wal_config.as_table();
                for (auto &elem : *wal_config_table) {

                    std::string var_name = std::string(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'wal' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kWALDir: {
                            // WAL Dir
                            std::string wal_dir = "/var/infinity/wal";

                            if (elem.second.is_string()) {
                                wal_dir = elem.second.value_or(wal_dir);
                            } else {
                                return Status::InvalidConfig("'wal_dir' field isn't string.");
                            }

                            auto wal_dir_option = std::make_unique<StringOption>(WAL_DIR_OPTION_NAME, wal_dir);
                            Status status = global_options_.AddOption(std::move(wal_dir_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kWALCompactThreshold: {
                            // WAL Compact Threshold
                            i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;

                            if (elem.second.is_string()) {
                                std::string wal_compact_threshold_str = elem.second.value_or(DEFAULT_WAL_FILE_SIZE_THRESHOLD_STR.data());
                                auto res = ParseByteSize(wal_compact_threshold_str, wal_compact_threshold);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'wal_dir' field isn't string.");
                            }

                            auto wal_compact_threshold_option = std::make_unique<IntegerOption>(WAL_COMPACT_THRESHOLD_OPTION_NAME,
                                                                                                wal_compact_threshold,
                                                                                                MAX_WAL_FILE_SIZE_THRESHOLD,
                                                                                                MIN_WAL_FILE_SIZE_THRESHOLD);
                            if (!wal_compact_threshold_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid WAL compact threshold: {}", wal_compact_threshold));
                            }
                            Status status = global_options_.AddOption(std::move(wal_compact_threshold_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kCheckpointInterval: {
                            // Checkpoint Interval
                            i64 checkpoint_interval = DEFAULT_CHECKPOINT_INTERVAL_SEC;
                            if (elem.second.is_string()) {
                                std::string checkpoint_interval_str = elem.second.value_or(DEFAULT_CHECKPOINT_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(checkpoint_interval_str, checkpoint_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'checkpoint_interval' field isn't string, such as \"30s\".");
                            }

                            auto checkpoint_interval_option = std::make_unique<IntegerOption>(CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                              checkpoint_interval,
                                                                                              MAX_CHECKPOINT_INTERVAL_SEC,
                                                                                              MIN_CHECKPOINT_INTERVAL_SEC);
                            if (!checkpoint_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid checkpoint interval: {}", checkpoint_interval));
                            }

                            Status status = global_options_.AddOption(std::move(checkpoint_interval_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kFlushMethodAtCommit: {
                            // Flush Method At Commit
                            FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
                            if (elem.second.is_string()) {
                                std::string flush_option_str = elem.second.value_or("flush_at_once");
                                ToLower(flush_option_str);
                                if (flush_option_str == "flush_at_once") {
                                    flush_option_type = FlushOptionType::kFlushAtOnce;
                                } else if (flush_option_str == "only_write") {
                                    flush_option_type = FlushOptionType::kFlushAtOnce;
                                } else if (flush_option_str == "flush_per_second") {
                                    flush_option_type = FlushOptionType::kFlushAtOnce;
                                } else {
                                    return Status::InvalidConfig(fmt::format("Unsupported flush option: {}", flush_option_str));
                                }
                            } else {
                                return Status::InvalidConfig("'flush_option' field isn't string.");
                            }

                            std::unique_ptr<FlushOption> flush_option = std::make_unique<FlushOption>(WAL_FLUSH_OPTION_NAME, flush_option_type);
                            Status status = global_options_.AddOption(std::move(flush_option));
                            if (!status.ok()) {
                                return status;
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'wal' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kWALDir) == nullptr) {
                    // WAL Dir
                    std::string wal_dir = "/var/infinity/wal";
                    auto wal_dir_option = std::make_unique<StringOption>(WAL_DIR_OPTION_NAME, wal_dir);
                    Status status = global_options_.AddOption(std::move(wal_dir_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kWALCompactThreshold) == nullptr) {
                    // WAL Compact Threshold
                    i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;
                    auto wal_compact_threshold_option = std::make_unique<IntegerOption>(WAL_COMPACT_THRESHOLD_OPTION_NAME,
                                                                                        wal_compact_threshold,
                                                                                        MAX_WAL_FILE_SIZE_THRESHOLD,
                                                                                        MIN_WAL_FILE_SIZE_THRESHOLD);
                    Status status = global_options_.AddOption(std::move(wal_compact_threshold_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kCheckpointInterval) == nullptr) {
                    // Checkpoint Interval
                    i64 checkpoint_interval = DEFAULT_CHECKPOINT_INTERVAL_SEC;
                    auto checkpoint_interval_option = std::make_unique<IntegerOption>(CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                      checkpoint_interval,
                                                                                      MAX_CHECKPOINT_INTERVAL_SEC,
                                                                                      MIN_CHECKPOINT_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(checkpoint_interval_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kFlushMethodAtCommit) == nullptr) {
                    // Flush Method At Commit
                    FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
                    std::unique_ptr<FlushOption> wal_flush_option = std::make_unique<FlushOption>(WAL_FLUSH_OPTION_NAME, flush_option_type);
                    Status status = global_options_.AddOption(std::move(wal_flush_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
            } else {
                return Status::InvalidConfig("No 'wal' section in configure file.");
            }
        }

        // Resource
        {
            if (config_toml.contains("resource")) {
                auto resource_config = config_toml["resource"];
                auto resource_config_table = resource_config.as_table();
                for (auto &elem : *resource_config_table) {

                    std::string var_name = std::string(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'resource' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kResourcePath: {
                            // Resource Dir
                            std::string resource_dir = "/var/infinity/resource";
                            if (elem.second.is_string()) {
                                resource_dir = elem.second.value_or(resource_dir);
                            } else {
                                return Status::InvalidConfig("'resource_dir' field isn't string.");
                            }

                            auto resource_dir_option = std::make_unique<StringOption>("resource_dir", resource_dir);
                            Status status = global_options_.AddOption(std::move(resource_dir_option));
                            if (!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'resource' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kResourcePath) == nullptr) {
                    // Resource Dir
                    std::string resource_dir = "/var/infinity/resource";
                    auto resource_dir_option = std::make_unique<StringOption>("resource_dir", resource_dir);
                    Status status = global_options_.AddOption(std::move(resource_dir_option));
                    if (!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
            } else {
                return Status::InvalidConfig("No 'resource' section in configure file.");
            }
        }
    }

    return Status::OK();
}

// General
std::string Config::Version() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kVersion);
}

std::string Config::ServerMode() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kServerMode);
}

std::string Config::TimeZone() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kTimeZone);
}

i64 Config::TimeZoneBias() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kTimeZoneBias);
}

// inline void set_worker_cpu_number(u64 new_cpu_limit) { system_option_.worker_cpu_limit = new_cpu_limit; }

i64 Config::CPULimit() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kWorkerCPULimit);
}

void Config::SetRecordRunningQuery(bool flag) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kRecordRunningQuery);
    if (base_option->data_type_ != BaseOptionDataType::kBoolean) {
        UnrecoverableError("Attempt to fetch bool value from record running query data type option");
    }
    auto *record_running_query_option = static_cast<BooleanOption *>(base_option);
    record_running_query_option->value_ = flag;
    record_running_query_ = flag;
}

// Network
std::string Config::ServerAddress() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kServerAddress);
}

std::string Config::PeerServerIP() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kPeerServerIP);
}

i64 Config::PeerServerPort() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPeerServerPort);
}

i64 Config::PostgresPort() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPostgresPort);
}

i64 Config::HTTPPort() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kHTTPPort);
}

i64 Config::ClientPort() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kClientPort);
}

i64 Config::ConnectionPoolSize() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kConnectionPoolSize);
}

i64 Config::PeerServerConnectionPoolSize() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPeerServerConnectionPoolSize);
}

i64 Config::PeerRetryDelay() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPeerRetryDelay);
}

i64 Config::PeerRetryCount() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPeerRetryCount);
}

i64 Config::PeerConnectTimeout() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPeerConnectTimeout);
}

i64 Config::PeerRecvTimeout() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPeerRecvTimeout);
}

i64 Config::PeerSendTimeout() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPeerSendTimeout);
}

// Log
std::string Config::LogFileName() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kLogFileName);
}

std::string Config::LogDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kLogDir);
}

std::string Config::LogFilePath() {
    return fmt::format("{}/{}",
                       global_options_.GetStringValue(GlobalOptionIndex::kLogDir),
                       global_options_.GetStringValue(GlobalOptionIndex::kLogFileName));
}

void Config::SetLogToStdout(bool log_to_stdout) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kLogToStdout);
    if (base_option->data_type_ != BaseOptionDataType::kBoolean) {
        UnrecoverableError("Attempt to set bool value to log to stdout data type option");
    }
    BooleanOption *log_to_stdout_option = static_cast<BooleanOption *>(base_option);
    log_to_stdout_option->value_ = log_to_stdout;
}

bool Config::LogToStdout() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetBoolValue(GlobalOptionIndex::kLogToStdout);
}

i64 Config::LogFileMaxSize() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kLogFileMaxSize);
}

i64 Config::LogFileRotateCount() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kLogFileRotateCount);
}

void Config::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kLogLevel);
    if (base_option->data_type_ != BaseOptionDataType::kLogLevel) {
        UnrecoverableError("Attempt to fetch log level value from log level data type option");
    }
    auto *log_level_option = static_cast<LogLevelOption *>(base_option);
    log_level_option->value_ = level;
}

LogLevel Config::GetLogLevel() {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kLogLevel);
    if (base_option->data_type_ != BaseOptionDataType::kLogLevel) {
        UnrecoverableError("Attempt to fetch log level value from log level data type option");
    }
    auto *log_level_option = static_cast<LogLevelOption *>(base_option);
    return log_level_option->value_;
}

// Storage
bool Config::ReplayWal() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetBoolValue(GlobalOptionIndex::kReplayWal);
}

std::string Config::DataDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kDataDir);
}

std::string Config::CatalogDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kCatalogDir);
}

i64 Config::CleanupInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kCleanupInterval);
}

void Config::SetCleanupInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kCleanupInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        UnrecoverableError("Attempt to set non-integer value to cleanup interval");
    }
    auto *cleanup_interval_option = static_cast<IntegerOption *>(base_option);
    cleanup_interval_option->value_ = interval;
}

i64 Config::CompactInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kCompactInterval);
}

void Config::SetCompactInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kCompactInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        UnrecoverableError("Attempt to set non-integer value to compact segment interval");
    }
    auto *compact_interval_option = static_cast<IntegerOption *>(base_option);
    compact_interval_option->value_ = interval;
}

i64 Config::OptimizeIndexInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kOptimizeIndexInterval);
}

void Config::SetOptimizeInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kOptimizeIndexInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        UnrecoverableError("Attempt to set non-integer value to optimize interval");
    }
    auto *optimize_interval_option = static_cast<IntegerOption *>(base_option);
    optimize_interval_option->value_ = interval;
}

std::string Config::SnapshotDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kSnapshotDir);
}

i64 Config::MemIndexCapacity() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kMemIndexCapacity);
}

i64 Config::DenseIndexBuildingWorker() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kDenseIndexBuildingWorker);
}

i64 Config::SparseIndexBuildingWorker() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kSparseIndexBuildingWorker);
}

i64 Config::FulltextIndexBuildingWorker() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kFulltextIndexBuildingWorker);
}

i64 Config::BottomExecutorWorker() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kBottomExecutorWorker);
}

StorageType Config::StorageType() {
    std::lock_guard<std::mutex> guard(mutex_);
    std::string storage_type_str = global_options_.GetStringValue(GlobalOptionIndex::kStorageType);
    return String2StorageType(storage_type_str);
}

std::string Config::ObjectStorageUrl() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kObjectStorageUrl);
}

std::string Config::ObjectStorageBucket() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kObjectStorageBucket);
}

std::string Config::ObjectStorageAccessKey() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kObjectStorageAccessKey);
}

std::string Config::ObjectStorageSecretKey() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kObjectStorageSecretKey);
}

bool Config::ObjectStorageHttps() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetBoolValue(GlobalOptionIndex::kObjectStorageHttps);
}

// Persistence
std::string Config::PersistenceDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kPersistenceDir);
}

i64 Config::PersistenceObjectSizeLimit() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPersistenceObjectSizeLimit);
}

bool Config::UseVFS() {
    std::lock_guard guard(mutex_);
    const auto persistence_dir = global_options_.GetStringValue(GlobalOptionIndex::kPersistenceDir);
    return !persistence_dir.empty();
}

// Buffer
i64 Config::BufferManagerSize() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kBufferManagerSize);
}

size_t Config::LRUNum() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kLRUNum);
}

std::string Config::TempDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kTempDir);
}

i64 Config::MemIndexMemoryQuota() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kMemIndexMemoryQuota);
}

std::string Config::ResultCache() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kResultCache);
}

i64 Config::CacheResultNum() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kCacheResultCapacity);
}

void Config::SetCacheResult(const std::string &mode) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kResultCache);
    if (base_option->data_type_ != BaseOptionDataType::kString) {
        UnrecoverableError("Attempt to set string value to result cache mode data type option");
    }
    auto *result_cache_option = static_cast<StringOption *>(base_option);
    result_cache_option->value_ = mode;
}

// WAL
std::string Config::WALDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kWALDir);
}

i64 Config::WALCompactThreshold() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kWALCompactThreshold);
}

i64 Config::CheckpointInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kCheckpointInterval);
}

void Config::SetFullCheckpointInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kCheckpointInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        UnrecoverableError("Attempt to set non-integer value to checkpoint interval");
    }
    auto *checkpoint_interval_option = static_cast<IntegerOption *>(base_option);
    checkpoint_interval_option->value_ = interval;
}

FlushOptionType Config::FlushMethodAtCommit() {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kFlushMethodAtCommit);
    if (base_option->data_type_ != BaseOptionDataType::kFlush) {
        UnrecoverableError("Attempt to fetch flush option value from flush option data type option");
    }
    auto *flush_option = static_cast<FlushOption *>(base_option);
    return flush_option->value_;
}

// Resource
std::string Config::ResourcePath() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kResourcePath);
}

// Date and Time

void Config::SetTimeZone(const std::string &value) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kTimeZone);
    if (base_option->data_type_ != BaseOptionDataType::kString) {
        UnrecoverableError("Attempt to set a non-string value to the time zone");
    }
    auto *time_zone_option = static_cast<StringOption *>(base_option);
    time_zone_option->value_ = value;
}

void Config::SetTimeZoneBias(i64 bias) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kTimeZoneBias);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        UnrecoverableError("Attempt to set non-integer value to the time zone bias");
    }
    auto *time_zone_bias_option = static_cast<IntegerOption *>(base_option);
    time_zone_bias_option->value_ = bias;
}

//// Profiler
// bool enable_profiler() const { return system_option_.enable_profiler; }
//
// size_t profile_record_capacity() const { return system_option_.profile_record_capacity; }

std::tuple<BaseOption *, Status> Config::GetConfigByName(const std::string &name) { return global_options_.GetOptionByName(name); }

// void Config::SetUserTimeZone(const std::string &value) {
//     ParseTimeZoneStr(value);
// }

void Config::PrintAll() {
    fmt::print("Infinity system configs: \n");

    // General
    fmt::print(" - version: {}\n", Version());
    fmt::print(" - timezone: {}{}\n", TimeZone(), TimeZoneBias());
    fmt::print(" - cpu_limit: {}\n", CPULimit());
    fmt::print(" - server mode: {}\n", ServerMode());

    //    // Profiler
    //    fmt::print(" - enable_profiler: {}\n", system_option_.enable_profiler);
    //    fmt::print(" - profile_record_capacity: {}\n", system_option_.profile_record_capacity);

    // Network
    fmt::print(" - server address: {}\n", ServerAddress());
    fmt::print(" - peer server ip: {}\n", PeerServerIP());
    fmt::print(" - peer server port: {}\n", PeerServerPort());
    fmt::print(" - postgres port: {}\n", PostgresPort());
    fmt::print(" - http port: {}\n", HTTPPort());
    fmt::print(" - rpc client port: {}\n", ClientPort());
    fmt::print(" - connection pool size: {}\n", ConnectionPoolSize());
    fmt::print(" - peer server connection pool size: {}\n", ConnectionPoolSize());

    // Log
    fmt::print(" - log_filename: {}\n", LogFileName());
    fmt::print(" - log_dir: {}\n", LogDir());
    fmt::print(" - log_file_path: {}\n", LogFilePath());
    fmt::print(" - log_to_stdout: {}\n", LogToStdout());
    fmt::print(" - log_file_max_size: {}\n", Utility::FormatByteSize(LogFileMaxSize()));
    fmt::print(" - log_file_rotate_count: {}\n", LogFileRotateCount());
    fmt::print(" - log_level: {}\n", LogLevel2Str(GetLogLevel()));

    // Storage
    fmt::print(" - persistence_dir: {}\n", PersistenceDir());
    fmt::print(" - persistence_file_size: {}\n", PersistenceObjectSizeLimit());
    fmt::print(" - data_dir: {}\n", DataDir());
    fmt::print(" - cleanup_interval: {}\n", Utility::FormatTimeInfo(CleanupInterval()));
    fmt::print(" - compact_interval: {}\n", Utility::FormatTimeInfo(CompactInterval()));
    fmt::print(" - optimize_index_interval: {}\n", Utility::FormatTimeInfo(OptimizeIndexInterval()));
    fmt::print(" - memindex_capacity: {}\n", MemIndexCapacity()); // mem index capacity is line number
    fmt::print(" - dense_index_building_worker: {}\n", DenseIndexBuildingWorker());
    fmt::print(" - sparse_index_building_worker: {}\n", SparseIndexBuildingWorker());
    fmt::print(" - fulltext_index_building_worker: {}\n", FulltextIndexBuildingWorker());
    fmt::print(" - storage_type: {}\n", ToString(StorageType()));
    switch (StorageType()) {
        case StorageType::kLocal: {
            break;
        }
        case StorageType::kMinio: {
            fmt::print(" - object_storage_url: {}\n", ObjectStorageUrl());
            fmt::print(" - object_storage_bucket: {}\n", ObjectStorageBucket());
            fmt::print(" - object_storage_access_key: {}\n", ObjectStorageAccessKey());
            fmt::print(" - object_storage_secret_key: {}\n", ObjectStorageSecretKey());
            fmt::print(" - object_storage_enable_https: {}\n", ObjectStorageHttps());
            break;
        }
        default: {
            break;
        }
    }
    fmt::print(" - snapshot_dir: {}\n", SnapshotDir());

    // Buffer manager
    fmt::print(" - buffer_manager_size: {}\n", Utility::FormatByteSize(BufferManagerSize()));
    fmt::print(" - temp_dir: {}\n", TempDir());
    fmt::print(" - memindex_memory_quota: {}\n", Utility::FormatByteSize(MemIndexMemoryQuota()));

    // WAL
    fmt::print(" - wal_dir: {}\n", WALDir());
    fmt::print(" - buffer_manager_size: {}\n", Utility::FormatByteSize(WALCompactThreshold()));
    fmt::print(" - checkpoint_interval: {}\n", Utility::FormatTimeInfo(CheckpointInterval()));
    fmt::print(" - flush_method_at_commit: {}\n", FlushOptionTypeToString(FlushMethodAtCommit()));

    // Resource dir
    fmt::print(" - resource_dir: {}\n", ResourcePath());
}

} // namespace infinity