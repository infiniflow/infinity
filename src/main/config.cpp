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

// #include <bits/chrono.h>
// #include <cctype>
#include <string>
#include <unistd.h>

module config;

import third_party;
import stl;
import boost;
import compilation_config;
import default_values;
import logger;
import local_file_system;
import utility;
import status;
import options;
import command_statement;
import infinity_exception;

namespace infinity {

u64 Config::GetAvailableMem() {
    u64 pages = sysconf(_SC_PHYS_PAGES);
    u64 page_size = sysconf(_SC_PAGE_SIZE); // Byte
    return pages * page_size;
}

void Config::ParseTimeZoneStr(const String &time_zone_str, String &parsed_time_zone, i32 &parsed_time_zone_bias) {
    parsed_time_zone = time_zone_str.substr(0, 3);
    ToUpper(parsed_time_zone);
    parsed_time_zone_bias = std::stoi(time_zone_str.substr(3, String::npos));
}

Status Config::ParseByteSize(const String &byte_size_str, i64 &byte_size) {

    HashMap<String, u64> byte_unit = {{"kb", 1024ul}, {"mb", 1024ul * 1024ul}, {"gb", 1024ul * 1024ul * 1024ul}};
    if (byte_size_str.empty()) {
        return Status::InvalidByteSize(byte_size_str);
    }

    u64 factor;
    auto res = std::from_chars(byte_size_str.data(), byte_size_str.data() + byte_size_str.size(), factor);
    if (res.ec == std::errc()) {
        String unit = res.ptr;
        ToLower(unit);
        auto it = byte_unit.find(unit);
        if (it != byte_unit.end()) {
            byte_size = factor * it->second;
            return Status::OK();
        } else {
            return Status::InvalidByteSize(byte_size_str);
        }
    } else {
        return Status::InvalidByteSize(byte_size_str);
    }
}

Status Config::ParseTimeInfo(const String &time_info, i64 &time_seconds) {
    if (time_info.empty()) {
        return Status::EmptyConfigParameter();
    }

    SizeT info_size = time_info.size();
    if (info_size == 1) {
        return Status::InvalidTimeInfo(time_info);
    }

    u64 time_number = 0;
    for (SizeT i = 0; i < info_size - 1; ++i) {
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

// extern SharedPtr<spdlogger> infinity_logger;

Status Config::Init(const SharedPtr<String> &config_path, DefaultConfig *default_config) {
    LocalFileSystem fs;

    toml::table config_toml{};
    if (config_path.get() == nullptr || config_path->empty() || !fs.Exists(std::filesystem::absolute(*config_path))) {
        if (config_path.get() == nullptr || config_path->empty()) {
//            fmt::print("No config file is given, use default configs.\n");
            ;
        } else {
            fmt::print("Config file: {} is not existent.\n", *config_path);
        }

        Status status;

        // Version
        String current_version = fmt::format("{}.{}.{}", version_major(), version_minor(), version_patch());
        UniquePtr<StringOption> version_option = MakeUnique<StringOption>(VERSION_OPTION_NAME, current_version);
        status = global_options_.AddOption(std::move(version_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Server mode
        String server_mode = "standalone";
        UniquePtr<StringOption> server_mode_option = MakeUnique<StringOption>(SERVER_MODE_OPTION_NAME, server_mode);
        status = global_options_.AddOption(std::move(server_mode_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Timezone
        String time_zone_str = "UTC";
        UniquePtr<StringOption> time_zone_option = MakeUnique<StringOption>(TIME_ZONE_OPTION_NAME, time_zone_str);
        status = global_options_.AddOption(std::move(time_zone_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Timezone Bias
        i64 time_zone_bias = 8;
        UniquePtr<IntegerOption> time_zone_bias_option = MakeUnique<IntegerOption>(TIME_ZONE_BIAS_OPTION_NAME, time_zone_bias, 12, -12);
        status = global_options_.AddOption(std::move(time_zone_bias_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // CPU limit
        UniquePtr<IntegerOption> cpu_limit_option = MakeUnique<IntegerOption>(CPU_LIMIT_OPTION_NAME, Thread::hardware_concurrency(), 16384, 1);
        status = global_options_.AddOption(std::move(cpu_limit_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Record running query
        bool record_running_query = false;
        record_running_query_ = record_running_query;
        UniquePtr<BooleanOption> record_running_query_option = MakeUnique<BooleanOption>(RECORD_RUNNING_QUERY_OPTION_NAME, record_running_query);
        status = global_options_.AddOption(std::move(record_running_query_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Server address
        String server_address_str = "0.0.0.0";
        UniquePtr<StringOption> server_address_option = MakeUnique<StringOption>(SERVER_ADDRESS_OPTION_NAME, server_address_str);
        status = global_options_.AddOption(std::move(server_address_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Postgres port
        i64 pg_port = 5432;
        UniquePtr<IntegerOption> pg_port_option = MakeUnique<IntegerOption>(POSTGRES_PORT_OPTION_NAME, pg_port, 65535, 1024);
        status = global_options_.AddOption(std::move(pg_port_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // HTTP port
        i64 http_port = 23820;
        UniquePtr<IntegerOption> http_port_option = MakeUnique<IntegerOption>(HTTP_PORT_OPTION_NAME, http_port, 65535, 1024);
        status = global_options_.AddOption(std::move(http_port_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // RPC Client port
        i64 rpc_client_port = 23817;
        UniquePtr<IntegerOption> client_port_option = MakeUnique<IntegerOption>(CLIENT_PORT_OPTION_NAME, rpc_client_port, 65535, 1024);
        status = global_options_.AddOption(std::move(client_port_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Client pool size
        i64 connection_pool_size = 256;
        UniquePtr<IntegerOption> connection_pool_size_option = MakeUnique<IntegerOption>(CONNECTION_POOL_SIZE_OPTION_NAME, connection_pool_size, 65536, 1);
        status = global_options_.AddOption(std::move(connection_pool_size_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log file name
        String log_filename = "infinity.log";
        UniquePtr<StringOption> log_file_name_option = MakeUnique<StringOption>(LOG_FILENAME_OPTION_NAME, log_filename);
        status = global_options_.AddOption(std::move(log_file_name_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log dir
        String log_dir = "/var/infinity/log";
        if(default_config != nullptr) {
            log_dir = default_config->default_log_dir_;
        }
        UniquePtr<StringOption> log_dir_option = MakeUnique<StringOption>(LOG_DIR_OPTION_NAME, log_dir);
        status = global_options_.AddOption(std::move(log_dir_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log To Stdout
        bool log_to_stdout = false;
        if(default_config != nullptr) {
            log_to_stdout = default_config->default_log_to_stdout_;
        }
        UniquePtr<BooleanOption> log_to_stdout_option = MakeUnique<BooleanOption>(LOG_TO_STDOUT_OPTION_NAME, log_to_stdout);
        status = global_options_.AddOption(std::move(log_to_stdout_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log File Max Size
        i64 log_file_max_size = 1024lu * 1024lu * 1024lu;
        UniquePtr<IntegerOption> log_file_max_size_option =
            MakeUnique<IntegerOption>(LOG_FILE_MAX_SIZE_OPTION_NAME, log_file_max_size, std::numeric_limits<i64>::max(), 1024lu * 1024lu);
        status = global_options_.AddOption(std::move(log_file_max_size_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log File Rotate count
        i64 log_file_rotate_count = 8;
        UniquePtr<IntegerOption> log_file_rotate_count_option = MakeUnique<IntegerOption>(LOG_FILE_ROTATE_COUNT_OPTION_NAME, log_file_rotate_count, 65536, 1);
        status = global_options_.AddOption(std::move(log_file_rotate_count_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Log Level
        LogLevel log_level = LogLevel::kInfo;
        if(default_config != nullptr) {
            log_level = default_config->default_log_level_;
        }
        UniquePtr<LogLevelOption> log_level_option = MakeUnique<LogLevelOption>(LOG_LEVEL_OPTION_NAME, log_level);
        status = global_options_.AddOption(std::move(log_level_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Data Dir
        String data_dir = "/var/infinity/data";
        if(default_config != nullptr) {
            data_dir = default_config->default_data_dir_;
        }
        UniquePtr<StringOption> data_dir_option = MakeUnique<StringOption>(DATA_DIR_OPTION_NAME, data_dir);
        status = global_options_.AddOption(std::move(data_dir_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Persistence Dir
        String persistence_dir = DEFAULT_PERSISTENCE_DIR.data();
        UniquePtr<StringOption> persistence_dir_option = MakeUnique<StringOption>(PERSISTENCE_DIR_OPTION_NAME, persistence_dir);
        global_options_.AddOption(std::move(persistence_dir_option));

        // Persistence Object Size Limit
        i64 persistence_object_size_limit = DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT;
        UniquePtr<IntegerOption> persistence_object_size_limit_option =
            MakeUnique<IntegerOption>(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME, persistence_object_size_limit, std::numeric_limits<i64>::max(), 0);
        global_options_.AddOption(std::move(persistence_object_size_limit_option));

        // Cleanup Interval
        i64 cleanup_interval = DEFAULT_CLEANUP_INTERVAL_SEC;
        UniquePtr<IntegerOption> cleanup_interval_option =
            MakeUnique<IntegerOption>(CLEANUP_INTERVAL_OPTION_NAME, cleanup_interval, MAX_CLEANUP_INTERVAL_SEC, MIN_CLEANUP_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(cleanup_interval_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Compact Interval
        i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
        UniquePtr<IntegerOption> compact_interval_option =
            MakeUnique<IntegerOption>(COMPACT_INTERVAL_OPTION_NAME, compact_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(compact_interval_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Optimize Index Interval
        i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
        UniquePtr<IntegerOption> optimize_interval_option =
            MakeUnique<IntegerOption>(OPTIMIZE_INTERVAL_OPTION_NAME, optimize_index_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(optimize_interval_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Mem Index Capacity
        i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
        UniquePtr<IntegerOption> mem_index_capacity_option =
            MakeUnique<IntegerOption>(MEM_INDEX_CAPACITY_OPTION_NAME, mem_index_capacity, MAX_MEMINDEX_CAPACITY, MIN_MEMINDEX_CAPACITY);
        status = global_options_.AddOption(std::move(mem_index_capacity_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Buffer Manager Size
        i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
        UniquePtr<IntegerOption> buffer_manager_size_option =
            MakeUnique<IntegerOption>(BUFFER_MANAGER_SIZE_OPTION_NAME, buffer_manager_size, std::numeric_limits<i64>::max(), 0);
        status = global_options_.AddOption(std::move(buffer_manager_size_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        SizeT lru_num = DEFAULT_BUFFER_MANAGER_LRU_COUNT;
        UniquePtr<IntegerOption> lru_num_option = MakeUnique<IntegerOption>(LRU_NUM_OPTION_NAME, lru_num, 100, 1);
        status = global_options_.AddOption(std::move(lru_num_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        i64 memindex_memory_quota = DEFAULT_MEMINDEX_MEMORY_QUOTA;
        UniquePtr<IntegerOption> memindex_memory_quota_option = MakeUnique<IntegerOption>(MEMINDEX_MEMORY_QUOTA_OPTION_NAME,
                                                                                          memindex_memory_quota, std::numeric_limits<i64>::max(), 0);
        status = global_options_.AddOption(std::move(memindex_memory_quota_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Temp Dir
        String temp_dir = "/var/infinity/tmp";
        if(default_config != nullptr) {
            temp_dir = default_config->default_temp_dir_;
        }
        UniquePtr<StringOption> temp_dir_option = MakeUnique<StringOption>(TEMP_DIR_OPTION_NAME, temp_dir);
        status = global_options_.AddOption(std::move(temp_dir_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // WAL Dir
        String wal_dir = "/var/infinity/wal";
        if(default_config != nullptr) {
            wal_dir = default_config->default_wal_dir_;
        }
        UniquePtr<StringOption> wal_dir_option = MakeUnique<StringOption>(WAL_DIR_OPTION_NAME, wal_dir);
        status = global_options_.AddOption(std::move(wal_dir_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // WAL Compact Threshold
        i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;
        UniquePtr<IntegerOption> wal_compact_threshold_option =
            MakeUnique<IntegerOption>(WAL_COMPACT_THRESHOLD_OPTION_NAME, wal_compact_threshold, MAX_WAL_FILE_SIZE_THRESHOLD, MIN_WAL_FILE_SIZE_THRESHOLD);
        status = global_options_.AddOption(std::move(wal_compact_threshold_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Full Checkpoint Interval
        i64 full_checkpoint_interval = DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC;
        UniquePtr<IntegerOption> full_checkpoint_interval_option = MakeUnique<IntegerOption>(FULL_CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                             full_checkpoint_interval,
                                                                                             MAX_FULL_CHECKPOINT_INTERVAL_SEC,
                                                                                             MIN_FULL_CHECKPOINT_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(full_checkpoint_interval_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Delta Checkpoint Interval
        i64 delta_checkpoint_interval = DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC;
        UniquePtr<IntegerOption> delta_checkpoint_interval_option = MakeUnique<IntegerOption>(DELTA_CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                              delta_checkpoint_interval,
                                                                                              MAX_DELTA_CHECKPOINT_INTERVAL_SEC,
                                                                                              MIN_DELTA_CHECKPOINT_INTERVAL_SEC);
        status = global_options_.AddOption(std::move(delta_checkpoint_interval_option));
        if(!status.ok()) {
            fmt::print("Fatal: {}", status.message());
            UnrecoverableError(status.message());
        }

        // Delta Checkpoint Threshold
        i64 delta_checkpoint_threshold = DELTA_CHECKPOINT_INTERVAL_WAL_BYTES;
        UniquePtr<IntegerOption> delta_checkpoint_threshold_option = MakeUnique<IntegerOption>(DELTA_CHECKPOINT_THRESHOLD_OPTION_NAME,
                                                                                               delta_checkpoint_threshold,
                                                                                               MAX_CHECKPOINT_INTERVAL_WAL_BYTES,
                                                                                               MIN_CHECKPOINT_INTERVAL_WAL_BYTES);
        status = global_options_.AddOption(std::move(delta_checkpoint_threshold_option));
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }

        // Flush Method At Commit
        FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
        UniquePtr<FlushOption> wal_flush_option = MakeUnique<FlushOption>(WAL_FLUSH_OPTION_NAME, flush_option_type);
        status = global_options_.AddOption(std::move(wal_flush_option));
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }

        // Resource Dir
        String resource_dir = "/var/infinity/resource";
        if(default_config != nullptr) {
            resource_dir = default_config->default_resource_dir_;
        }
        UniquePtr<StringOption> resource_dir_option = MakeUnique<StringOption>("resource_dir", resource_dir);
        status = global_options_.AddOption(std::move(resource_dir_option));
        if(!status.ok()) {
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

                    String var_name = String(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if (option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'general' field", var_name));
                    }

                    switch (option_index) {
                        case GlobalOptionIndex::kVersion: {
                            // Version
                            String invalid_str = "invalid";
                            String version_str;
                            if (elem.second.is_string()) {
                                version_str = elem.second.value_or("invalid");
                                ToLower(version_str);

                                String major_version_str;
                                i8 point_count = 0;
                                for(char c: version_str) {
                                    if(c == '.') {
                                        ++ point_count;
                                        if(point_count > 1) {
                                            break;
                                        }
                                    }
                                    major_version_str += c;
                                }

                                if (IsEqual(major_version_str, invalid_str)) {
                                    return Status::InvalidConfig("Invalid version field");
                                } else {
                                    String current_major_version = fmt::format("{}.{}", version_major(), version_minor());
                                    String current_version = fmt::format("{}.{}.{}", version_major(), version_minor(), version_patch());
                                    if (IsEqual(major_version_str, current_major_version)) {
                                        UniquePtr<StringOption> version_option = MakeUnique<StringOption>(VERSION_OPTION_NAME, current_version);
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
                            String server_mode = "standalone";
                            if(elem.second.is_string()) {
                                server_mode = elem.second.value_or(server_mode);
                            } else {
                                return Status::InvalidConfig("'server_mode' field isn't string.");
                            }

                            ToLower(server_mode);
                            if(server_mode == "standalone" or server_mode == "cluster") {
                                UniquePtr<StringOption> server_mode_option = MakeUnique<StringOption>(SERVER_MODE_OPTION_NAME, server_mode);
                                Status status = global_options_.AddOption(std::move(server_mode_option));
                                if(!status.ok()) {
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
                                String time_zone_str = general_config[TIME_ZONE_OPTION_NAME].value_or("invalid");
                                ToLower(time_zone_str);
                                if (time_zone_str == "invalid") {
                                    return Status::InvalidTimezone(time_zone_str);
                                } else {
                                    String time_zone;
                                    i32 time_zone_bias = 0;
                                    try {
                                        ParseTimeZoneStr(time_zone_str, time_zone, time_zone_bias);
                                    } catch (...) {
                                        return Status::InvalidTimezone(time_zone_str);
                                    }

                                    // Timezone
                                    UniquePtr<StringOption> time_zone_option = MakeUnique<StringOption>(TIME_ZONE_OPTION_NAME, time_zone);
                                    Status status = global_options_.AddOption(std::move(time_zone_option));
                                    if (!status.ok()) {
                                        UnrecoverableError(status.message());
                                    }

                                    // Timezone Bias
                                    UniquePtr<IntegerOption> time_zone_bias_option =
                                        MakeUnique<IntegerOption>(TIME_ZONE_BIAS_OPTION_NAME, time_zone_bias, 12, -12);
                                    if (!time_zone_bias_option->Validate()) {
                                        return Status::InvalidConfig(fmt::format("Invalid timezone bias: {}", time_zone_bias));
                                    }
                                    status = global_options_.AddOption(std::move(time_zone_bias_option));
                                    if (!status.ok()) {
                                        UnrecoverableError(status.message());
                                    }
                                }
                            }
                            break;
                        }
                        case GlobalOptionIndex::kWorkerCPULimit: {
                            i64 total_cpu_number = Thread::hardware_concurrency();

                            if (elem.second.is_integer()) {
                                total_cpu_number = elem.second.value_or(total_cpu_number);
                            } else {
                                return Status::InvalidConfig("'cpu_limit' field isn't integer.");
                            }

                            UniquePtr<IntegerOption> cpu_limit_option = MakeUnique<IntegerOption>(CPU_LIMIT_OPTION_NAME, total_cpu_number, 16384, 1);
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
                            UniquePtr<BooleanOption> record_running_query_option = MakeUnique<BooleanOption>(RECORD_RUNNING_QUERY_OPTION_NAME, record_running_query);
                            Status status = global_options_.AddOption(std::move(record_running_query_option));
                            if(!status.ok()) {
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
                    String error_message = "Missing version field";
                    fmt::print("Fatal: {}",error_message);
                    UnrecoverableError(error_message);
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kServerMode) == nullptr) {
                    // Server mode
                    String server_mode = "standalone";
                    UniquePtr<StringOption> server_mode_option = MakeUnique<StringOption>(SERVER_MODE_OPTION_NAME, server_mode);
                    Status status = global_options_.AddOption(std::move(server_mode_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kTimeZone) == nullptr) {
                    // Time zone
                    String error_message = "Missing time zone field";
                    fmt::print("Fatal: {}",error_message);
                    UnrecoverableError(error_message);
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kTimeZoneBias) == nullptr) {
                    // Time zone bias
                    String error_message = "Missing time zone field";
                    fmt::print("Fatal: {}",error_message);
                    UnrecoverableError(error_message);
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kWorkerCPULimit) == nullptr) {
                    // CPU limit
                    UniquePtr<IntegerOption> cpu_limit_option = MakeUnique<IntegerOption>(CPU_LIMIT_OPTION_NAME, Thread::hardware_concurrency(), 16384, 1);
                    Status status = global_options_.AddOption(std::move(cpu_limit_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kRecordRunningQuery) == nullptr) {
                    // Record running query
                    bool record_running_query = false;
                    record_running_query_ = record_running_query;
                    UniquePtr<BooleanOption> record_running_query_option = MakeUnique<BooleanOption>(RECORD_RUNNING_QUERY_OPTION_NAME, record_running_query);
                    Status status = global_options_.AddOption(std::move(record_running_query_option));
                    if(!status.ok()) {
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
                for(auto& elem: *network_config_table) {

                    String var_name = String(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if(option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'network' field", var_name));
                    }

                    switch(option_index) {
                        case GlobalOptionIndex::kServerAddress: {
                            // Server address
                            String server_address = "0.0.0.0";
                            if(elem.second.is_string()) {
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

                            UniquePtr<StringOption> server_address_option = MakeUnique<StringOption>(SERVER_ADDRESS_OPTION_NAME, server_address);
                            Status status = global_options_.AddOption(std::move(server_address_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPostgresPort: {
                            // Postgres port
                            i64 pg_port = 5432;
                            if (elem.second.is_integer()) {
                                pg_port = elem.second.value_or(pg_port);
                            } else {
                                return Status::InvalidConfig("'postgres_port' field isn't integer.");
                            }

                            UniquePtr<IntegerOption> pg_port_option = MakeUnique<IntegerOption>(POSTGRES_PORT_OPTION_NAME, pg_port, 65535, 1024);
                            if (!pg_port_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid postgres port: {}", pg_port));
                            }
                            Status status = global_options_.AddOption(std::move(pg_port_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kHTTPPort: {
                            // HTTP port
                            i64 http_port = 23820;
                            if (elem.second.is_integer()) {
                                http_port = elem.second.value_or(http_port);
                            } else {
                                return Status::InvalidConfig("'http_port' field isn't integer.");
                            }

                            UniquePtr<IntegerOption> http_port_option = MakeUnique<IntegerOption>(HTTP_PORT_OPTION_NAME, http_port, 65535, 1024);
                            if (!http_port_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid HTTP port: {}", http_port));
                            }
                            Status status = global_options_.AddOption(std::move(http_port_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kClientPort: {
                            // RPC Client port
                            i64 rpc_client_port = 23817;
                            if (elem.second.is_integer()) {
                                rpc_client_port = elem.second.value_or(rpc_client_port);
                            } else {
                                return Status::InvalidConfig("'client_port' field isn't integer.");
                            }

                            UniquePtr<IntegerOption> client_port_option = MakeUnique<IntegerOption>(CLIENT_PORT_OPTION_NAME, rpc_client_port, 65535, 1024);
                            if (!client_port_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid client RPC port: {}", rpc_client_port));
                            }
                            Status status = global_options_.AddOption(std::move(client_port_option));
                            if(!status.ok()) {
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

                            UniquePtr<IntegerOption> connection_pool_size_option =
                                MakeUnique<IntegerOption>(CONNECTION_POOL_SIZE_OPTION_NAME, connection_pool_size, 65536, 1);
                            if (!connection_pool_size_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid connection pool size: {}", connection_pool_size));
                            }

                            Status status = global_options_.AddOption(std::move(connection_pool_size_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'network' field", var_name));
                        }
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kServerAddress) == nullptr) {
                    // Server address
                    String server_address_str = "0.0.0.0";
                    UniquePtr<StringOption> server_address_option = MakeUnique<StringOption>(SERVER_ADDRESS_OPTION_NAME, server_address_str);
                    Status status = global_options_.AddOption(std::move(server_address_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kPostgresPort) == nullptr) {
                    // Postgres port
                    i64 pg_port = 5432;
                    UniquePtr<IntegerOption> pg_port_option = MakeUnique<IntegerOption>(POSTGRES_PORT_OPTION_NAME, pg_port, 65535, 1024);
                    Status status = global_options_.AddOption(std::move(pg_port_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kHTTPPort) == nullptr) {
                    // HTTP port
                    i64 http_port = 23820;
                    UniquePtr<IntegerOption> http_port_option = MakeUnique<IntegerOption>(HTTP_PORT_OPTION_NAME, http_port, 65535, 1024);
                    Status status = global_options_.AddOption(std::move(http_port_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kClientPort) == nullptr) {
                    // RPC Client port
                    i64 rpc_client_port = 23817;
                    UniquePtr<IntegerOption> client_port_option = MakeUnique<IntegerOption>(CLIENT_PORT_OPTION_NAME, rpc_client_port, 65535, 1024);
                    Status status = global_options_.AddOption(std::move(client_port_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kConnectionPoolSize) == nullptr) {
                    // Client pool size
                    i64 connection_pool_size = 256;
                    UniquePtr<IntegerOption> connection_pool_size_option = MakeUnique<IntegerOption>(CONNECTION_POOL_SIZE_OPTION_NAME, connection_pool_size, 65536, 1);
                    Status status = global_options_.AddOption(std::move(connection_pool_size_option));
                    if(!status.ok()) {
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
                for(auto& elem: *log_config_table) {

                    String var_name = String(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if(option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'log' field", var_name));
                    }

                    switch(option_index) {
                        case GlobalOptionIndex::kLogFileName: {
                            // Log file name
                            String log_filename = "infinity.log";
                            if (elem.second.is_string()) {
                                log_filename = elem.second.value_or(log_filename);
                            } else {
                                return Status::InvalidConfig("'log_filename' field isn't string.");
                            }

                            UniquePtr<StringOption> log_file_name_option = MakeUnique<StringOption>(LOG_FILENAME_OPTION_NAME, log_filename);
                            Status status = global_options_.AddOption(std::move(log_file_name_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogDir: {
                            // Log dir
                            String log_filename = "/var/infinity/log";
                            if (elem.second.is_string()) {
                                log_filename = elem.second.value_or(log_filename);
                            } else {
                                return Status::InvalidConfig("'log_dir' field isn't string.");
                            }

                            UniquePtr<StringOption> log_dir_option = MakeUnique<StringOption>(LOG_DIR_OPTION_NAME, log_filename);
                            Status status = global_options_.AddOption(std::move(log_dir_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogToStdout: {
                            // Log To Stdout
                            bool log_to_stdout = false;
                            if (elem.second.is_boolean()) {
                                log_to_stdout = elem.second.value_or(log_to_stdout);
                            } else {
                                return Status::InvalidConfig("'log_to_stdout' field isn't boolean.");
                            }

                            UniquePtr<BooleanOption> log_to_stdout_option = MakeUnique<BooleanOption>(LOG_TO_STDOUT_OPTION_NAME, log_to_stdout);
                            Status status = global_options_.AddOption(std::move(log_to_stdout_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogFileMaxSize: {
                            // Log File Max Size
                            i64 log_file_max_size = DEFAULT_LOG_FILE_SIZE;
                            if (elem.second.is_string()) {
                                String log_file_max_size_str = elem.second.value_or(DEFAULT_LOG_FILE_SIZE_STR.data());
                                auto res = ParseByteSize(log_file_max_size_str, log_file_max_size);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'log_file_max_size' field isn't integer.");
                            }

                            UniquePtr<IntegerOption> log_file_max_size_option =
                                MakeUnique<IntegerOption>(LOG_FILE_MAX_SIZE_OPTION_NAME, log_file_max_size, std::numeric_limits<i64>::max(), 1024lu * 1024lu);

                            if (!log_file_max_size_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid max log file size: {}", log_file_max_size));
                            }
                            Status status = global_options_.AddOption(std::move(log_file_max_size_option));
                            if(!status.ok()) {
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

                            UniquePtr<IntegerOption> log_file_rotate_count_option =
                                MakeUnique<IntegerOption>(LOG_FILE_ROTATE_COUNT_OPTION_NAME, log_file_rotate_count, 65536, 1);

                            if (!log_file_rotate_count_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid log file rotate count: {}", log_file_rotate_count));
                            }
                            Status status = global_options_.AddOption(std::move(log_file_rotate_count_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kLogLevel: {
                            // Log Level
                            LogLevel log_level = LogLevel::kInfo;
                            if(elem.second.is_string()) {
                                String log_level_str = elem.second.value_or("info");
                                ToLower(log_level_str);
                                if (IsEqual(log_level_str, "trace")) {
                                    log_level = LogLevel::kTrace;
                                } else if (IsEqual(log_level_str, "debug")) {
                                    log_level = LogLevel::kDebug;
                                } else if (IsEqual(log_level_str, "info")) {
                                    log_level = LogLevel::kInfo;
                                } else if (IsEqual(log_level_str, "warning")) {
                                    log_level = LogLevel::kWarning;
                                } else if (IsEqual(log_level_str, "error")) {
                                    log_level = LogLevel::kError;
                                } else if (IsEqual(log_level_str, "critical")) {
                                    log_level = LogLevel::kCritical;
                                } else {
                                    return Status::InvalidLogLevel(log_level_str);
                                }
                            } else {
                                return Status::InvalidConfig("'log_level' field isn't string.");
                            }

                            UniquePtr<LogLevelOption> log_level_option = MakeUnique<LogLevelOption>(LOG_LEVEL_OPTION_NAME, log_level);
                            Status status = global_options_.AddOption(std::move(log_level_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'buffer' field", var_name));
                        }
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kLogFileName) == nullptr) {
                    // Log file name
                    String log_filename = "infinity.log";
                    UniquePtr<StringOption> log_file_name_option = MakeUnique<StringOption>(LOG_FILENAME_OPTION_NAME, log_filename);
                    Status status = global_options_.AddOption(std::move(log_file_name_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kLogDir) == nullptr) {
                    // Log dir
                    String log_dir = "/var/infinity/log";
                    UniquePtr<StringOption> log_dir_option = MakeUnique<StringOption>(LOG_DIR_OPTION_NAME, log_dir);
                    Status status = global_options_.AddOption(std::move(log_dir_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kLogToStdout) == nullptr) {
                    // Log To Stdout
                    bool log_to_stdout = false;
                    if(default_config != nullptr) {
                        log_to_stdout = default_config->default_log_to_stdout_;
                    }
                    UniquePtr<BooleanOption> log_to_stdout_option = MakeUnique<BooleanOption>(LOG_TO_STDOUT_OPTION_NAME, log_to_stdout);
                    Status status = global_options_.AddOption(std::move(log_to_stdout_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kLogFileMaxSize) == nullptr) {
                    // Log File Max Size
                    i64 log_file_max_size = 1024lu * 1024lu * 1024lu;
                    UniquePtr<IntegerOption> log_file_max_size_option =
                        MakeUnique<IntegerOption>(LOG_FILE_MAX_SIZE_OPTION_NAME, log_file_max_size, std::numeric_limits<i64>::max(), 1024lu * 1024lu);
                    Status status = global_options_.AddOption(std::move(log_file_max_size_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kLogFileRotateCount) == nullptr) {
                    // Log File Rotate count
                    i64 log_file_rotate_count = 8;
                    UniquePtr<IntegerOption> log_file_rotate_count_option = MakeUnique<IntegerOption>(LOG_FILE_ROTATE_COUNT_OPTION_NAME, log_file_rotate_count, 65536, 1);
                    Status status = global_options_.AddOption(std::move(log_file_rotate_count_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kLogLevel) == nullptr) {
                    // Log Level
                    LogLevel log_level = LogLevel::kInfo;
                    if(default_config != nullptr) {
                        log_level = default_config->default_log_level_;
                    }
                    UniquePtr<LogLevelOption> log_level_option = MakeUnique<LogLevelOption>(LOG_LEVEL_OPTION_NAME, log_level);
                    Status status = global_options_.AddOption(std::move(log_level_option));
                    if(!status.ok()) {
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
                for(auto& elem: *storage_config_table) {

                    String var_name = String(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if(option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'storage' field", var_name));
                    }

                    switch(option_index) {
                        case GlobalOptionIndex::kDataDir: {
                            // Data Dir
                            String data_dir = "/var/infinity/data";
                            if(elem.second.is_string()) {
                                data_dir = elem.second.value_or(data_dir);
                            } else {
                                return Status::InvalidConfig("'data_dir' field isn't string.");
                            }

                            UniquePtr<StringOption> data_dir_option = MakeUnique<StringOption>(DATA_DIR_OPTION_NAME, data_dir);
                            Status status = global_options_.AddOption(std::move(data_dir_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kPersistenceDir: {
                            String persistence_dir;
                            if (elem.second.is_string()) {
                                persistence_dir = elem.second.value_or(DEFAULT_PERSISTENCE_DIR.data());
                            } else {
                                return Status::InvalidConfig("'persistence_dir' field isn't string, such as \"persistence\"");
                            }
                            UniquePtr<StringOption> persistence_dir_option = MakeUnique<StringOption>(PERSISTENCE_DIR_OPTION_NAME, persistence_dir);
                            global_options_.AddOption(std::move(persistence_dir_option));
                            break;
                        }
                        case GlobalOptionIndex::kPersistenceObjectSizeLimit: {
                            i64 persistence_object_size_limit;
                            if (elem.second.is_string()) {
                                String persistence_object_size_limit_str = elem.second.value_or(DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT_STR.data());
                                auto res = ParseByteSize(persistence_object_size_limit_str, persistence_object_size_limit);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'persistence_object_size_limit' field isn't string, such as \"100MB\"");
                            }
                            UniquePtr<IntegerOption> persistence_object_size_limit_option =
                                MakeUnique<IntegerOption>(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME,
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
                            if(elem.second.is_string()) {
                                String cleanup_interval_str = elem.second.value_or(DEFAULT_CLEANUP_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(cleanup_interval_str, cleanup_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'cleanup_interval' field isn't string, such as \"1m\".");
                            }

                            UniquePtr<IntegerOption> cleanup_interval_option =
                                MakeUnique<IntegerOption>(CLEANUP_INTERVAL_OPTION_NAME, cleanup_interval, MAX_CLEANUP_INTERVAL_SEC, MIN_CLEANUP_INTERVAL_SEC);
                            if (!cleanup_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid cleanup interval: {}", cleanup_interval));
                            }
                            Status status = global_options_.AddOption(std::move(cleanup_interval_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kCompactInterval: {
                            // Compact Interval
                            i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
                            if(elem.second.is_string()) {
                                String compact_interval_str = elem.second.value_or(DEFAULT_COMPACT_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(compact_interval_str, compact_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'compact_interval' field isn't string, such as \"1m\".");
                            }

                            UniquePtr<IntegerOption> compact_interval_option =
                                MakeUnique<IntegerOption>(COMPACT_INTERVAL_OPTION_NAME, compact_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
                            if (!compact_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid compact interval: {}", compact_interval));
                            }
                            Status status = global_options_.AddOption(std::move(compact_interval_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kOptimizeIndexInterval: {
                            // Optimize Index Interval
                            i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
                            if(elem.second.is_string()) {
                                String optimize_index_interval_str = elem.second.value_or(DEFAULT_OPTIMIZE_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(optimize_index_interval_str, optimize_index_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'optimize_interval' field isn't string, such as \"1m\".");
                            }

                            UniquePtr<IntegerOption> optimize_interval_option =
                                MakeUnique<IntegerOption>(OPTIMIZE_INTERVAL_OPTION_NAME, optimize_index_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
                            if (!optimize_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid optimize interval: {}", optimize_index_interval));
                            }
                            Status status = global_options_.AddOption(std::move(optimize_interval_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kMemIndexCapacity: {
                            // Mem Index Capacity
                            i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
                            if(elem.second.is_integer()) {
                                mem_index_capacity = elem.second.value_or(mem_index_capacity);
                            } else {
                                return Status::InvalidConfig("'mem_index_capacity' field isn't integer.");
                            }

                            UniquePtr<IntegerOption> mem_index_capacity_option =
                                MakeUnique<IntegerOption>(MEM_INDEX_CAPACITY_OPTION_NAME, mem_index_capacity, MAX_MEMINDEX_CAPACITY, MIN_MEMINDEX_CAPACITY);
                            if (!mem_index_capacity_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid memory index capacity: {}", mem_index_capacity));
                            }
                            Status status = global_options_.AddOption(std::move(mem_index_capacity_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'storage' field", var_name));
                        }
                    }
                }

                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPersistenceDir) == nullptr) {
                    String persistence_dir =
                        (global_options_.GetOptionByIndex(GlobalOptionIndex::kDataDir) == nullptr) ? DEFAULT_PERSISTENCE_DIR.data() : "";
                    UniquePtr<StringOption> persistence_dir_option = MakeUnique<StringOption>(PERSISTENCE_DIR_OPTION_NAME, persistence_dir);
                    global_options_.AddOption(std::move(persistence_dir_option));
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kPersistenceObjectSizeLimit) == nullptr) {
                    i64 persistence_object_size_limit = DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT;
                    UniquePtr<IntegerOption> persistence_object_size_limit_option =
                        MakeUnique<IntegerOption>(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME,
                                                  persistence_object_size_limit,
                                                  std::numeric_limits<i64>::max(),
                                                  0);
                    global_options_.AddOption(std::move(persistence_object_size_limit_option));
                }
                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kDataDir) == nullptr) {
                    // Data Dir
                    String data_dir = "/var/infinity/data";
                    UniquePtr<StringOption> data_dir_option = MakeUnique<StringOption>(DATA_DIR_OPTION_NAME, data_dir);
                    Status status = global_options_.AddOption(std::move(data_dir_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kCleanupInterval) == nullptr) {
                    // Cleanup Interval
                    i64 cleanup_interval = DEFAULT_CLEANUP_INTERVAL_SEC;
                    UniquePtr<IntegerOption> cleanup_interval_option =
                        MakeUnique<IntegerOption>(CLEANUP_INTERVAL_OPTION_NAME, cleanup_interval, MAX_CLEANUP_INTERVAL_SEC, MIN_CLEANUP_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(cleanup_interval_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kCompactInterval) == nullptr) {
                    // Compact Interval
                    i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
                    UniquePtr<IntegerOption> compact_interval_option =
                        MakeUnique<IntegerOption>(COMPACT_INTERVAL_OPTION_NAME, compact_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(compact_interval_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kOptimizeIndexInterval) == nullptr) {
                    // Optimize Index Interval
                    i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
                    UniquePtr<IntegerOption> optimize_interval_option =
                        MakeUnique<IntegerOption>(OPTIMIZE_INTERVAL_OPTION_NAME, optimize_index_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(optimize_interval_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kMemIndexCapacity) == nullptr) {
                    // Mem Index Capacity
                    i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
                    UniquePtr<IntegerOption> mem_index_capacity_option =
                        MakeUnique<IntegerOption>(MEM_INDEX_CAPACITY_OPTION_NAME, mem_index_capacity, MAX_MEMINDEX_CAPACITY, MIN_MEMINDEX_CAPACITY);
                    Status status = global_options_.AddOption(std::move(mem_index_capacity_option));
                    if(!status.ok()) {
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
                for(auto& elem: *buffer_config_table) {

                    String var_name = String(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if(option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'buffer' field", var_name));
                    }

                    switch(option_index) {
                        case GlobalOptionIndex::kBufferManagerSize: {
                            i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
                            if(elem.second.is_string()) {
                                String buffer_manager_size_str = elem.second.value_or(DEFAULT_BUFFER_MANAGER_SIZE_STR.data());
                                auto res = ParseByteSize(buffer_manager_size_str, buffer_manager_size);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'buffer_manager_size' field isn't string, such as \"4GB\"");
                            }
                            UniquePtr<IntegerOption> buffer_manager_size_option =
                                MakeUnique<IntegerOption>(BUFFER_MANAGER_SIZE_OPTION_NAME, buffer_manager_size, std::numeric_limits<i64>::max(), 0);
                            if (!buffer_manager_size_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid buffer manager size: {}", buffer_manager_size));
                            }
                            global_options_.AddOption(std::move(buffer_manager_size_option));
                            break;
                        }
                        case GlobalOptionIndex::kLRUNum: {
                            i64 lru_num = DEFAULT_BUFFER_MANAGER_LRU_COUNT;
                            if(elem.second.is_integer()) {
                                lru_num = elem.second.value_or(lru_num);
                            } else {
                                return Status::InvalidConfig("'lru_num' field isn't integer.");
                            }
                            UniquePtr<IntegerOption> lru_num_option = MakeUnique<IntegerOption>(LRU_NUM_OPTION_NAME, lru_num, 100, 1);
                            if (!lru_num_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid LRU num: {}", 0));
                            }
                            global_options_.AddOption(std::move(lru_num_option));
                            break;
                        }
                        case GlobalOptionIndex::kTempDir: {
                            String temp_dir = "/var/infinity/tmp";
                            if (elem.second.is_string()) {
                                temp_dir = elem.second.value_or(temp_dir);
                            } else {
                                return Status::InvalidConfig("'temp_dir' field isn't string.");
                            }

                            UniquePtr<StringOption> temp_dir_option = MakeUnique<StringOption>(TEMP_DIR_OPTION_NAME, temp_dir);
                            global_options_.AddOption(std::move(temp_dir_option));
                            break;
                        }
                        case GlobalOptionIndex::kMemIndexMemoryQuota: {
                            i64 mem_index_memory_quota = DEFAULT_MEMINDEX_MEMORY_QUOTA;
                            if (elem.second.is_string()) {
                                String mem_index_memory_quota_str = elem.second.value_or(DEFAULT_MEMINDEX_MEMORY_QUOTA_STR.data());
                                auto res = ParseByteSize(mem_index_memory_quota_str, mem_index_memory_quota);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'mem_index_memory_quota' field isn't string.");
                            }
                            UniquePtr<IntegerOption> mem_index_memory_quota_option =
                                MakeUnique<IntegerOption>(MEMINDEX_MEMORY_QUOTA_OPTION_NAME, mem_index_memory_quota, std::numeric_limits<i64>::max(), 0);
                            global_options_.AddOption(std::move(mem_index_memory_quota_option));
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'buffer' field", var_name));
                        }
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kBufferManagerSize) == nullptr) {
                    // Buffer Manager Size
                    i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
                    UniquePtr<IntegerOption> buffer_manager_size_option =
                        MakeUnique<IntegerOption>(BUFFER_MANAGER_SIZE_OPTION_NAME, buffer_manager_size, std::numeric_limits<i64>::max(), 0);
                    Status status = global_options_.AddOption(std::move(buffer_manager_size_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kLRUNum) == nullptr) {
                    // LRU Num
                    i64 lru_num = DEFAULT_BUFFER_MANAGER_LRU_COUNT;
                    UniquePtr<IntegerOption> lru_num_option = MakeUnique<IntegerOption>(LRU_NUM_OPTION_NAME, lru_num, 100, 1);
                    Status status = global_options_.AddOption(std::move(lru_num_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kTempDir) == nullptr) {
                    // Temp Dir
                    String temp_dir = "/var/infinity/tmp";
                    UniquePtr<StringOption> temp_dir_option = MakeUnique<StringOption>(TEMP_DIR_OPTION_NAME, temp_dir);
                    Status status = global_options_.AddOption(std::move(temp_dir_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }
                if (global_options_.GetOptionByIndex(GlobalOptionIndex::kMemIndexMemoryQuota) == nullptr) {
                    // Mem Index Memory Quota
                    i64 mem_index_memory_quota = DEFAULT_MEMINDEX_MEMORY_QUOTA;
                    UniquePtr<IntegerOption> mem_index_memory_quota_option =
                        MakeUnique<IntegerOption>(MEMINDEX_MEMORY_QUOTA_OPTION_NAME, mem_index_memory_quota, std::numeric_limits<i64>::max(), 0);
                    Status status = global_options_.AddOption(std::move(mem_index_memory_quota_option));
                    if(!status.ok()) {
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
                for(auto& elem: *wal_config_table) {

                    String var_name = String(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if(option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'wal' field", var_name));
                    }

                    switch(option_index) {
                        case GlobalOptionIndex::kWALDir: {
                            // WAL Dir
                            String wal_dir = "/var/infinity/wal";

                            if (elem.second.is_string()) {
                                wal_dir = elem.second.value_or(wal_dir);
                            } else {
                                return Status::InvalidConfig("'wal_dir' field isn't string.");
                            }

                            UniquePtr<StringOption> wal_dir_option = MakeUnique<StringOption>(WAL_DIR_OPTION_NAME, wal_dir);
                            Status status = global_options_.AddOption(std::move(wal_dir_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kWALCompactThreshold: {
                            // WAL Compact Threshold
                            i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;

                            if (elem.second.is_string()) {
                                String wal_compact_threshold_str = elem.second.value_or(DEFAULT_WAL_FILE_SIZE_THRESHOLD_STR.data());
                                auto res = ParseByteSize(wal_compact_threshold_str, wal_compact_threshold);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'wal_dir' field isn't string.");
                            }

                            UniquePtr<IntegerOption> wal_compact_threshold_option = MakeUnique<IntegerOption>(WAL_COMPACT_THRESHOLD_OPTION_NAME,
                                                                                                              wal_compact_threshold,
                                                                                                              MAX_WAL_FILE_SIZE_THRESHOLD,
                                                                                                              MIN_WAL_FILE_SIZE_THRESHOLD);
                            if (!wal_compact_threshold_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid WAL compact threshold: {}", wal_compact_threshold));
                            }
                            Status status = global_options_.AddOption(std::move(wal_compact_threshold_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kFullCheckpointInterval: {
                            // Full Checkpoint Interval
                            i64 full_checkpoint_interval = DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC;
                            if (elem.second.is_string()) {
                                String full_checkpoint_interval_str = elem.second.value_or(DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(full_checkpoint_interval_str, full_checkpoint_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'full_checkpoint_interval' field isn't string, such as \"30s\".");
                            }

                            UniquePtr<IntegerOption> full_checkpoint_interval_option = MakeUnique<IntegerOption>(FULL_CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                                                 full_checkpoint_interval,
                                                                                                                 MAX_FULL_CHECKPOINT_INTERVAL_SEC,
                                                                                                                 MIN_FULL_CHECKPOINT_INTERVAL_SEC);
                            if (!full_checkpoint_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid full checkpoint interval: {}", full_checkpoint_interval));
                            }

                            Status status = global_options_.AddOption(std::move(full_checkpoint_interval_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        case GlobalOptionIndex::kDeltaCheckpointInterval: {
                            // Delta Checkpoint Interval
                            i64 delta_checkpoint_interval = DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC;
                            if (elem.second.is_string()) {
                                String delta_checkpoint_interval_str = elem.second.value_or(DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC_STR.data());
                                auto res = ParseTimeInfo(delta_checkpoint_interval_str, delta_checkpoint_interval);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'delta_checkpoint_interval' field isn't string, such as \"30s\".");
                            }

                            UniquePtr<IntegerOption> delta_checkpoint_interval_option = MakeUnique<IntegerOption>(DELTA_CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                                                  delta_checkpoint_interval,
                                                                                                                  MAX_DELTA_CHECKPOINT_INTERVAL_SEC,
                                                                                                                  MIN_DELTA_CHECKPOINT_INTERVAL_SEC);
                            if (!delta_checkpoint_interval_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid delta checkpoint interval: {}", delta_checkpoint_interval));
                            }

                            Status status = global_options_.AddOption(std::move(delta_checkpoint_interval_option));
                            if (!status.ok()) {
                                return status;
                            }
                            break;
                        }
                        case GlobalOptionIndex::kDeltaCheckpointThreshold: {
                            // Delta Checkpoint Threshold
                            i64 delta_checkpoint_threshold = DELTA_CHECKPOINT_INTERVAL_WAL_BYTES;
                            if (elem.second.is_string()) {
                                String delta_checkpoint_threshold_str = elem.second.value_or(DELTA_CHECKPOINT_INTERVAL_WAL_BYTES_STR.data());
                                auto res = ParseByteSize(delta_checkpoint_threshold_str, delta_checkpoint_threshold);
                                if (!res.ok()) {
                                    return res;
                                }
                            } else {
                                return Status::InvalidConfig("'delta_checkpoint_threshold' field isn't integer.");
                            }

                            UniquePtr<IntegerOption> delta_checkpoint_threshold_option = MakeUnique<IntegerOption>(DELTA_CHECKPOINT_THRESHOLD_OPTION_NAME,
                                                                                                                   delta_checkpoint_threshold,
                                                                                                                   MAX_CHECKPOINT_INTERVAL_WAL_BYTES,
                                                                                                                   MIN_CHECKPOINT_INTERVAL_WAL_BYTES);
                            if (!delta_checkpoint_threshold_option->Validate()) {
                                return Status::InvalidConfig(fmt::format("Invalid delta checkpoint interval: {}", delta_checkpoint_threshold));
                            }

                            Status status = global_options_.AddOption(std::move(delta_checkpoint_threshold_option));
                            if (!status.ok()) {
                                return status;
                            }
                            break;
                        }
                        case GlobalOptionIndex::kFlushMethodAtCommit: {
                            // Flush Method At Commit
                            FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
                            if (elem.second.is_string()) {
                                String flush_option_str = elem.second.value_or("flush_at_once");
                                ToLower(flush_option_str);
                                if (IsEqual(flush_option_str, "flush_at_once")) {
                                    flush_option_type = FlushOptionType::kFlushAtOnce;
                                } else if (IsEqual(flush_option_str, "only_write")) {
                                    flush_option_type = FlushOptionType::kFlushAtOnce;
                                } else if (IsEqual(flush_option_str, "flush_per_second")) {
                                    flush_option_type = FlushOptionType::kFlushAtOnce;
                                } else {
                                    return Status::InvalidConfig(fmt::format("Unsupported flush option: {}", flush_option_str));
                                }
                            } else {
                                return Status::InvalidConfig("'flush_option' field isn't string.");
                            }

                            UniquePtr<FlushOption> flush_option = MakeUnique<FlushOption>(WAL_FLUSH_OPTION_NAME, flush_option_type);
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

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kWALDir) == nullptr) {
                    // WAL Dir
                    String wal_dir = "/var/infinity/wal";
                    UniquePtr<StringOption> wal_dir_option = MakeUnique<StringOption>(WAL_DIR_OPTION_NAME, wal_dir);
                    Status status = global_options_.AddOption(std::move(wal_dir_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kWALCompactThreshold) == nullptr) {
                    // WAL Compact Threshold
                    i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;
                    UniquePtr<IntegerOption> wal_compact_threshold_option =
                        MakeUnique<IntegerOption>(WAL_COMPACT_THRESHOLD_OPTION_NAME, wal_compact_threshold, MAX_WAL_FILE_SIZE_THRESHOLD, MIN_WAL_FILE_SIZE_THRESHOLD);
                    Status status = global_options_.AddOption(std::move(wal_compact_threshold_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kFullCheckpointInterval) == nullptr) {
                    // Full Checkpoint Interval
                    i64 full_checkpoint_interval = DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC;
                    UniquePtr<IntegerOption> full_checkpoint_interval_option = MakeUnique<IntegerOption>(FULL_CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                                         full_checkpoint_interval,
                                                                                                         MAX_FULL_CHECKPOINT_INTERVAL_SEC,
                                                                                                         MIN_FULL_CHECKPOINT_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(full_checkpoint_interval_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kDeltaCheckpointInterval) == nullptr) {
                    // Delta Checkpoint Interval
                    i64 delta_checkpoint_interval = DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC;
                    UniquePtr<IntegerOption> delta_checkpoint_interval_option = MakeUnique<IntegerOption>(DELTA_CHECKPOINT_INTERVAL_OPTION_NAME,
                                                                                                          delta_checkpoint_interval,
                                                                                                          MAX_DELTA_CHECKPOINT_INTERVAL_SEC,
                                                                                                          MIN_DELTA_CHECKPOINT_INTERVAL_SEC);
                    Status status = global_options_.AddOption(std::move(delta_checkpoint_interval_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kDeltaCheckpointThreshold) == nullptr) {
                    // Delta Checkpoint Threshold
                    i64 delta_checkpoint_threshold = DELTA_CHECKPOINT_INTERVAL_WAL_BYTES;
                    UniquePtr<IntegerOption> delta_checkpoint_threshold_option = MakeUnique<IntegerOption>(DELTA_CHECKPOINT_THRESHOLD_OPTION_NAME,
                                                                                                           delta_checkpoint_threshold,
                                                                                                           MAX_CHECKPOINT_INTERVAL_WAL_BYTES,
                                                                                                           MIN_CHECKPOINT_INTERVAL_WAL_BYTES);
                    Status status = global_options_.AddOption(std::move(delta_checkpoint_threshold_option));
                    if(!status.ok()) {
                        UnrecoverableError(status.message());
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kFlushMethodAtCommit) == nullptr) {
                    // Flush Method At Commit
                    FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
                    UniquePtr<FlushOption> wal_flush_option = MakeUnique<FlushOption>(WAL_FLUSH_OPTION_NAME, flush_option_type);
                    Status status = global_options_.AddOption(std::move(wal_flush_option));
                    if(!status.ok()) {
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
                for(auto& elem: *resource_config_table) {

                    String var_name = String(elem.first);
                    GlobalOptionIndex option_index = global_options_.GetOptionIndex(var_name);
                    if(option_index == GlobalOptionIndex::kInvalid) {
                        return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'resource' field", var_name));
                    }

                    switch(option_index) {
                        case GlobalOptionIndex::kResourcePath: {
                            // Resource Dir
                            String resource_dir = "/var/infinity/resource";
                            if(elem.second.is_string()) {
                                resource_dir = elem.second.value_or(resource_dir);
                            } else {
                                return Status::InvalidConfig("'resource_dir' field isn't string.");
                            }

                            UniquePtr<StringOption> resource_dir_option = MakeUnique<StringOption>("resource_dir", resource_dir);
                            Status status = global_options_.AddOption(std::move(resource_dir_option));
                            if(!status.ok()) {
                                UnrecoverableError(status.message());
                            }
                            break;
                        }
                        default: {
                            return Status::InvalidConfig(fmt::format("Unrecognized config parameter: {} in 'resource' field", var_name));
                        }
                    }
                }

                if(global_options_.GetOptionByIndex(GlobalOptionIndex::kResourcePath) == nullptr) {
                    // Resource Dir
                    String resource_dir = "/var/infinity/resource";
                    UniquePtr<StringOption> resource_dir_option = MakeUnique<StringOption>("resource_dir", resource_dir);
                    Status status = global_options_.AddOption(std::move(resource_dir_option));
                    if(!status.ok()) {
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
String Config::Version() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kVersion);
}

String Config::ServerMode() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kServerMode);
}

String Config::TimeZone() {
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
        String error_message = "Attempt to fetch bool value from record running query data type option";
        UnrecoverableError(error_message);
    }
    BooleanOption *record_running_query_option = static_cast<BooleanOption *>(base_option);
    record_running_query_option->value_ = flag;
    record_running_query_ = flag;
}

// Network
String Config::ServerAddress() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kServerAddress);
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

// Log
String Config::LogFileName() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kLogFileName);
}

String Config::LogDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kLogDir);
}

String Config::LogFilePath() {
    return fmt::format("{}/{}", global_options_.GetStringValue(GlobalOptionIndex::kLogDir), global_options_.GetStringValue(GlobalOptionIndex::kLogFileName));
}

void Config::SetLogToStdout(bool log_to_stdout) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kLogToStdout);
    if (base_option->data_type_ != BaseOptionDataType::kBoolean) {
        String error_message = "Attempt to set bool value to log to stdout data type option";
        UnrecoverableError(error_message);
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
        String error_message = "Attempt to fetch log level value from log level data type option";
        UnrecoverableError(error_message);
    }
    LogLevelOption *log_level_option = static_cast<LogLevelOption *>(base_option);
    log_level_option->value_ = level;
    return ;
}

LogLevel Config::GetLogLevel() {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kLogLevel);
    if (base_option->data_type_ != BaseOptionDataType::kLogLevel) {
        String error_message = "Attempt to fetch log level value from log level data type option";
        UnrecoverableError(error_message);
    }
    LogLevelOption *log_level_option = static_cast<LogLevelOption *>(base_option);
    return log_level_option->value_;
}

// Storage
String Config::DataDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kDataDir);
}

i64 Config::CleanupInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kCleanupInterval);
}

void Config::SetCleanupInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kCleanupInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        String error_message = "Attempt to set non-integer value to cleanup interval";
        UnrecoverableError(error_message);
    }
    IntegerOption *cleanup_interval_option = static_cast<IntegerOption *>(base_option);
    cleanup_interval_option->value_ = interval;
    return ;
}

i64 Config::CompactInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kCompactInterval);
}

void Config::SetCompactInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kCompactInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        String error_message = "Attempt to set non-integer value to compact segment interval";
        UnrecoverableError(error_message);
    }
    IntegerOption *compact_interval_option = static_cast<IntegerOption *>(base_option);
    compact_interval_option->value_ = interval;
    return ;
}

i64 Config::OptimizeIndexInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kOptimizeIndexInterval);
}

void Config::SetOptimizeInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kOptimizeIndexInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        String error_message = "Attempt to set non-integer value to optimize interval";
        UnrecoverableError(error_message);
    }
    IntegerOption *optimize_interval_option = static_cast<IntegerOption *>(base_option);
    optimize_interval_option->value_ = interval;
    return ;
}

i64 Config::MemIndexCapacity() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kMemIndexCapacity);
}

// Persistence
String Config::PersistenceDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kPersistenceDir);
}

i64 Config::PersistenceObjectSizeLimit() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kPersistenceObjectSizeLimit);
}

// Buffer
i64 Config::BufferManagerSize() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kBufferManagerSize);
}

SizeT Config::LRUNum() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kLRUNum);
}

String Config::TempDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kTempDir);
}

i64 Config::MemIndexMemoryQuota() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kMemIndexMemoryQuota);
}

// WAL
String Config::WALDir() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kWALDir);
}

i64 Config::WALCompactThreshold() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kWALCompactThreshold);
}

i64 Config::FullCheckpointInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kFullCheckpointInterval);
}

void Config::SetFullCheckpointInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kFullCheckpointInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        String error_message = "Attempt to set non-integer value to full checkpoint interval";
        UnrecoverableError(error_message);
    }
    IntegerOption *full_checkpoint_interval_option = static_cast<IntegerOption *>(base_option);
    full_checkpoint_interval_option->value_ = interval;
    return ;
}

i64 Config::DeltaCheckpointInterval() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kDeltaCheckpointInterval);
}

void Config::SetDeltaCheckpointInterval(i64 interval) {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kDeltaCheckpointInterval);
    if (base_option->data_type_ != BaseOptionDataType::kInteger) {
        String error_message = "Attempt to set non-integer value to delta checkpoint interval";
        UnrecoverableError(error_message);
    }
    IntegerOption *delta_checkpoint_interval_option = static_cast<IntegerOption *>(base_option);
    delta_checkpoint_interval_option->value_ = interval;
    return ;
}

i64 Config::DeltaCheckpointThreshold() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetIntegerValue(GlobalOptionIndex::kDeltaCheckpointThreshold);
}

FlushOptionType Config::FlushMethodAtCommit() {
    std::lock_guard<std::mutex> guard(mutex_);
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kFlushMethodAtCommit);
    if (base_option->data_type_ != BaseOptionDataType::kFlush) {
        String error_message = "Attempt to fetch flush option value from flush option data type option";
        UnrecoverableError(error_message);
    }
    FlushOption *flush_option = static_cast<FlushOption *>(base_option);
    return flush_option->value_;
}

// Resource
String Config::ResourcePath() {
    std::lock_guard<std::mutex> guard(mutex_);
    return global_options_.GetStringValue(GlobalOptionIndex::kResourcePath);
}

//// Profiler
// bool enable_profiler() const { return system_option_.enable_profiler; }
//
// SizeT profile_record_capacity() const { return system_option_.profile_record_capacity; }

Tuple<BaseOption *, Status> Config::GetConfigByName(const String &name) {
    return global_options_.GetOptionByName(name);
}

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
    fmt::print(" - postgres port: {}\n", PostgresPort());
    fmt::print(" - http port: {}\n", HTTPPort());
    fmt::print(" - rpc client port: {}\n", ClientPort());
    fmt::print(" - connection pool size: {}\n", ConnectionPoolSize());

    // Log
    fmt::print(" - log_filename: {}\n", LogFileName());
    fmt::print(" - log_dir: {}\n", LogDir());
    fmt::print(" - log_file_path: {}\n", LogFilePath());
    fmt::print(" - log_to_stdout: {}\n", LogToStdout());
    fmt::print(" - log_file_max_size: {}\n", Utility::FormatByteSize(LogFileMaxSize()));
    fmt::print(" - log_file_rotate_count: {}\n", LogFileRotateCount());
    fmt::print(" - log_level: {}\n", LogLevel2Str(LogLevel()));

    // Storage
    fmt::print(" - persistence_dir: {}\n", PersistenceDir());
    fmt::print(" - persistence_file_size: {}\n", PersistenceObjectSizeLimit());
    fmt::print(" - data_dir: {}\n", DataDir());
    fmt::print(" - cleanup_interval: {}\n", Utility::FormatTimeInfo(CleanupInterval()));
    fmt::print(" - compact_interval: {}\n", Utility::FormatTimeInfo(CompactInterval()));
    fmt::print(" - optimize_index_interval: {}\n", Utility::FormatTimeInfo(OptimizeIndexInterval()));
    fmt::print(" - memindex_capacity: {}\n", Utility::FormatByteSize(MemIndexCapacity()));

    // Buffer manager
    fmt::print(" - buffer_manager_size: {}\n", Utility::FormatByteSize(BufferManagerSize()));
    fmt::print(" - temp_dir: {}\n", TempDir());
    fmt::print(" - memindex_memory_quota: {}\n", Utility::FormatByteSize(MemIndexMemoryQuota()));

    // WAL
    fmt::print(" - wal_dir: {}\n", WALDir());
    fmt::print(" - buffer_manager_size: {}\n", Utility::FormatByteSize(WALCompactThreshold()));
    fmt::print(" - full_checkpoint_interval: {}\n", Utility::FormatTimeInfo(FullCheckpointInterval()));
    fmt::print(" - delta_checkpoint_interval: {}\n", Utility::FormatTimeInfo(DeltaCheckpointInterval()));
    fmt::print(" - delta_checkpoint_threshold: {}\n", Utility::FormatByteSize(DeltaCheckpointThreshold()));
    fmt::print(" - flush_method_at_commit: {}\n", FlushOptionTypeToString(FlushMethodAtCommit()));

    // Resource dir
    fmt::print(" - resource_dir: {}\n", ResourcePath());
}

} // namespace infinity
