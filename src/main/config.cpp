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
            time_number += time_number * 10 + (time_info[i] - '0');
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

Status Config::Init(const SharedPtr<String> &config_path) {

    LocalFileSystem fs;

    toml::table config_toml{};
    if (config_path.get() == nullptr || !fs.Exists(*config_path)) {
        if (config_path.get() == nullptr) {
            fmt::print("No config file is given, use default configs.\n");
        } else {
            if (!fs.Exists(*config_path)) {
                fmt::print("Config file: {} is not existent.\n", *config_path);
            }
        }

        // Version
        String current_version = fmt::format("{}.{}.{}", version_major(), version_minor(), version_patch());
        UniquePtr<StringOption> version_option = MakeUnique<StringOption>("version", current_version);
        global_options_.AddOption(std::move(version_option), GlobalOptionIndex::kVersion);

        // Timezone
        String time_zone_str = "UTC";
        UniquePtr<StringOption> time_zone_option = MakeUnique<StringOption>("time_zone", time_zone_str);
        global_options_.AddOption(std::move(time_zone_option), GlobalOptionIndex::kTimeZone);

        // Timezone Bias
        i64 time_zone_bias = 8;
        UniquePtr<IntegerOption> time_zone_bias_option = MakeUnique<IntegerOption>("time_zone_bias", time_zone_bias, 12, -12);
        global_options_.AddOption(std::move(time_zone_bias_option), GlobalOptionIndex::kTimeZoneBias);

        // CPU limit
        UniquePtr<IntegerOption> cpu_limit_option = MakeUnique<IntegerOption>("cpu_limit", Thread::hardware_concurrency(), 16384, 1);
        global_options_.AddOption(std::move(cpu_limit_option), GlobalOptionIndex::kWorkerCPULimit);

        // Server address
        String server_address_str = "0.0.0.0";
        UniquePtr<StringOption> server_address_option = MakeUnique<StringOption>("server_address", server_address_str);
        global_options_.AddOption(std::move(server_address_option), GlobalOptionIndex::kServerAddress);

        // Postgres port
        i64 pg_port = 5432;
        UniquePtr<IntegerOption> pg_port_option = MakeUnique<IntegerOption>("postgres_port", pg_port, 65535, 1024);
        global_options_.AddOption(std::move(pg_port_option), GlobalOptionIndex::kPostgresPort);

        // HTTP port
        i64 http_port = 23820;
        UniquePtr<IntegerOption> http_port_option = MakeUnique<IntegerOption>("http_port", http_port, 65535, 1024);
        global_options_.AddOption(std::move(http_port_option), GlobalOptionIndex::kHTTPPort);

        // RPC Client port
        i64 rpc_client_port = 23817;
        UniquePtr<IntegerOption> client_port_option = MakeUnique<IntegerOption>("client_port", rpc_client_port, 65535, 1024);
        global_options_.AddOption(std::move(client_port_option), GlobalOptionIndex::kClientPort);

        // Client pool size
        i64 connection_pool_size = 256;
        UniquePtr<IntegerOption> connection_pool_size_option = MakeUnique<IntegerOption>("connection_pool_size", connection_pool_size, 65536, 1);
        global_options_.AddOption(std::move(connection_pool_size_option), GlobalOptionIndex::kConnectionPoolSize);

        // Log file name
        String log_filename = "infinity.log";
        UniquePtr<StringOption> log_file_name_option = MakeUnique<StringOption>("log_filename", log_filename);
        global_options_.AddOption(std::move(log_file_name_option), GlobalOptionIndex::kLogFileName);

        // Log dir
        String log_dir = "/var/infinity/log";
        UniquePtr<StringOption> log_dir_option = MakeUnique<StringOption>("log_dir", log_dir);
        global_options_.AddOption(std::move(log_dir_option), GlobalOptionIndex::kLogDir);

        // Log To Stdout
        bool log_to_stdout = false;
        UniquePtr<BooleanOption> log_to_stdout_option = MakeUnique<BooleanOption>("log_to_stdout", log_to_stdout);
        global_options_.AddOption(std::move(log_to_stdout_option), GlobalOptionIndex::kLogToStdout);

        // Log File Max Size
        i64 log_file_max_size = 1024lu * 1024lu * 1024lu;
        UniquePtr<IntegerOption> log_file_max_size_option =
            MakeUnique<IntegerOption>("log_file_max_size", log_file_max_size, std::numeric_limits<i64>::max(), 1024lu * 1024lu);
        global_options_.AddOption(std::move(log_file_max_size_option), GlobalOptionIndex::kLogFileMaxSize);

        // Log File Rotate count
        i64 log_file_rotate_count = 8;
        UniquePtr<IntegerOption> log_file_rotate_count_option = MakeUnique<IntegerOption>("log_file_rotate_count", log_file_rotate_count, 65536, 1);
        global_options_.AddOption(std::move(log_file_rotate_count_option), GlobalOptionIndex::kLogFileRotateCount);

        // Log Level
        LogLevel log_level = LogLevel::kInfo;
        UniquePtr<LogLevelOption> log_level_option = MakeUnique<LogLevelOption>("log_level", log_level);
        global_options_.AddOption(std::move(log_level_option), GlobalOptionIndex::kLogLevel);

        // Data Dir
        String data_dir = "/var/infinity/data";
        UniquePtr<StringOption> data_dir_option = MakeUnique<StringOption>("data_dir", data_dir);
        global_options_.AddOption(std::move(data_dir_option), GlobalOptionIndex::kDataDir);

        // Cleanup Interval
        i64 cleanup_interval = DEFAULT_CLEANUP_INTERVAL_SEC;
        UniquePtr<IntegerOption> cleanup_interval_option =
            MakeUnique<IntegerOption>("cleanup_interval", cleanup_interval, MAX_CLEANUP_INTERVAL_SEC, MIN_CLEANUP_INTERVAL_SEC);
        global_options_.AddOption(std::move(cleanup_interval_option), GlobalOptionIndex::kCleanupInterval);

        // Compact Interval
        i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
        UniquePtr<IntegerOption> compact_interval_option =
            MakeUnique<IntegerOption>("compact_interval", compact_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
        global_options_.AddOption(std::move(compact_interval_option), GlobalOptionIndex::kCompactInterval);

        // Optimize Index Interval
        i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
        UniquePtr<IntegerOption> optimize_interval_option =
            MakeUnique<IntegerOption>("optimize_interval", optimize_index_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
        global_options_.AddOption(std::move(optimize_interval_option), GlobalOptionIndex::kOptimizeIndexInterval);

        // Mem Index Capacity
        i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
        UniquePtr<IntegerOption> mem_index_capacity_option =
            MakeUnique<IntegerOption>("mem_index_capacity", mem_index_capacity, MAX_MEMINDEX_CAPACITY, MIN_MEMINDEX_CAPACITY);
        global_options_.AddOption(std::move(mem_index_capacity_option), GlobalOptionIndex::kMemIndexCapacity);

        // Buffer Manager Size
        i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
        UniquePtr<IntegerOption> buffer_manager_size_option =
            MakeUnique<IntegerOption>("buffer_manager_size", buffer_manager_size, std::numeric_limits<i64>::max(), 0);
        global_options_.AddOption(std::move(buffer_manager_size_option), GlobalOptionIndex::kBufferManagerSize);

        // Temp Dir
        String temp_dir = "/var/infinity/tmp";
        UniquePtr<StringOption> temp_dir_option = MakeUnique<StringOption>("temp_dir", temp_dir);
        global_options_.AddOption(std::move(temp_dir_option), GlobalOptionIndex::kTempDir);

        // WAL Dir
        String wal_dir = "/var/infinity/wal";
        UniquePtr<StringOption> wal_dir_option = MakeUnique<StringOption>("wal_dir", wal_dir);
        global_options_.AddOption(std::move(wal_dir_option), GlobalOptionIndex::kWALDir);

        // WAL Compact Threshold
        i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;
        UniquePtr<IntegerOption> wal_compact_threshold_option =
            MakeUnique<IntegerOption>("wal_compact_threshold", wal_compact_threshold, MAX_WAL_FILE_SIZE_THRESHOLD, MIN_WAL_FILE_SIZE_THRESHOLD);
        global_options_.AddOption(std::move(wal_compact_threshold_option), GlobalOptionIndex::kWALCompactThreshold);

        // Full Checkpoint Interval
        i64 full_checkpoint_interval = DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC;
        UniquePtr<IntegerOption> full_checkpoint_interval_option = MakeUnique<IntegerOption>("full_checkpoint_interval",
                                                                                             full_checkpoint_interval,
                                                                                             MAX_FULL_CHECKPOINT_INTERVAL_SEC,
                                                                                             MIN_FULL_CHECKPOINT_INTERVAL_SEC);
        global_options_.AddOption(std::move(full_checkpoint_interval_option), GlobalOptionIndex::kFullCheckpointInterval);

        // Delta Checkpoint Interval
        i64 delta_checkpoint_interval = DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC;
        UniquePtr<IntegerOption> delta_checkpoint_interval_option = MakeUnique<IntegerOption>("delta_checkpoint_interval",
                                                                                              delta_checkpoint_interval,
                                                                                              MAX_DELTA_CHECKPOINT_INTERVAL_SEC,
                                                                                              MIN_DELTA_CHECKPOINT_INTERVAL_SEC);
        global_options_.AddOption(std::move(delta_checkpoint_interval_option), GlobalOptionIndex::kDeltaCheckpointInterval);

        // Delta Checkpoint Threshold
        i64 delta_checkpoint_threshold = DELTA_CHECKPOINT_INTERVAL_WAL_BYTES;
        UniquePtr<IntegerOption> delta_checkpoint_threshold_option = MakeUnique<IntegerOption>("delta_checkpoint_threshold",
                                                                                               delta_checkpoint_threshold,
                                                                                               MAX_CHECKPOINT_INTERVAL_WAL_BYTES,
                                                                                               MIN_CHECKPOINT_INTERVAL_WAL_BYTES);
        global_options_.AddOption(std::move(delta_checkpoint_threshold_option), GlobalOptionIndex::kDeltaCheckpointThreshold);

        // Flush Method At Commit
        FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
        UniquePtr<FlushOption> flush_option = MakeUnique<FlushOption>("flush_option", flush_option_type);
        global_options_.AddOption(std::move(flush_option), GlobalOptionIndex::kFlushMethodAtCommit);

        // Resource Dir
        String resource_dir = "/var/infinity/resource";
        UniquePtr<StringOption> resource_dir_option = MakeUnique<StringOption>("resource_dir", resource_dir);
        global_options_.AddOption(std::move(resource_dir_option), GlobalOptionIndex::kResourcePath);

    } else {
        config_toml = toml::parse_file(*config_path);

        // General
        {
            if (config_toml.contains("general")) {
                auto general_config = config_toml["general"];

                String invalid_str = "invalid";
                {
                    // Version
                    String version_str = general_config["version"].value_or("invalid");
                    ToLower(version_str);
                    if (IsEqual(version_str, invalid_str)) {
                        return Status::InvalidConfig("Invalid version field");
                    } else {
                        String current_version = fmt::format("{}.{}.{}", version_major(), version_minor(), version_patch());
                        if (IsEqual(version_str, current_version)) {
                            UniquePtr<StringOption> version_option = MakeUnique<StringOption>("version", current_version);
                            global_options_.AddOption(std::move(version_option), GlobalOptionIndex::kVersion);
                        } else {
                            Status::MismatchVersion(version_str, current_version);
                        }
                    }
                }

                {
                    // Timezone
                    String time_zone_str = general_config["timezone"].value_or("invalid");
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
                        UniquePtr<StringOption> time_zone_option = MakeUnique<StringOption>("time_zone", time_zone);
                        global_options_.AddOption(std::move(time_zone_option), GlobalOptionIndex::kTimeZone);

                        // Timezone Bias
                        UniquePtr<IntegerOption> time_zone_bias_option = MakeUnique<IntegerOption>("time_zone_bias", time_zone_bias, 12, -12);
                        if (!time_zone_bias_option->Validate()) {
                            return Status::InvalidConfig(fmt::format("Invalid timezone bias: {}", time_zone_bias));
                        }
                        global_options_.AddOption(std::move(time_zone_bias_option), GlobalOptionIndex::kTimeZoneBias);
                    }
                }

                {
                    // Worker CPU Limit
                    i64 total_cpu_number = Thread::hardware_concurrency();
                    if (general_config["cpu_limit"]) {
                        if (general_config["cpu_limit"].is_integer()) {
                            total_cpu_number = general_config["cpu_limit"].value_or(total_cpu_number);
                        } else {
                            return Status::InvalidConfig("'cpu_limit' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> cpu_limit_option = MakeUnique<IntegerOption>("cpu_limit", total_cpu_number, 16384, 1);
                    if (!cpu_limit_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid cpu limit: {}", total_cpu_number));
                    }
                    global_options_.AddOption(std::move(cpu_limit_option), GlobalOptionIndex::kWorkerCPULimit);
                }
            } else {
                return Status::InvalidConfig("No 'general' section in configure file.");
            }
        }

        // Network
        {
            if (config_toml.contains("network")) {
                auto network_config = config_toml["network"];
                {
                    // Server address
                    String server_address = "0.0.0.0";
                    if (network_config["server_address"]) {
                        if (network_config["server_address"].is_string()) {
                            server_address = network_config["server_address"].value_or(server_address);
                        } else {
                            return Status::InvalidConfig("'server_address' field isn't string.");
                        }
                    } else {
                        ;
                    }

                    // Validate the address format
                    boost::system::error_code error;
                    boost::asio::ip::make_address(server_address, error);
                    if (error) {
                        return Status::InvalidIPAddr(server_address);
                    }

                    UniquePtr<StringOption> server_address_option = MakeUnique<StringOption>("server_address", server_address);
                    global_options_.AddOption(std::move(server_address_option), GlobalOptionIndex::kServerAddress);
                }
                {
                    // Postgres port
                    i64 pg_port = 5432;
                    if (network_config["postgres_port"]) {
                        if (network_config["postgres_port"].is_integer()) {
                            pg_port = network_config["postgres_port"].value_or(pg_port);
                        } else {
                            return Status::InvalidConfig("'postgres_port' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> pg_port_option = MakeUnique<IntegerOption>("postgres_port", pg_port, 65535, 1024);
                    if (!pg_port_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid postgres port: {}", pg_port));
                    }
                    global_options_.AddOption(std::move(pg_port_option), GlobalOptionIndex::kPostgresPort);
                }
                {
                    // HTTP port
                    i64 http_port = 23820;
                    if (network_config["http_port"]) {
                        if (network_config["http_port"].is_integer()) {
                            http_port = network_config["http_port"].value_or(http_port);
                        } else {
                            return Status::InvalidConfig("'http_port' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> http_port_option = MakeUnique<IntegerOption>("http_port", http_port, 65535, 1024);
                    if (!http_port_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid HTTP port: {}", http_port));
                    }
                    global_options_.AddOption(std::move(http_port_option), GlobalOptionIndex::kHTTPPort);
                }

                {
                    // RPC Client port
                    i64 rpc_client_port = 23817;
                    if (network_config["client_port"]) {
                        if (network_config["client_port"].is_integer()) {
                            rpc_client_port = network_config["client_port"].value_or(rpc_client_port);
                        } else {
                            return Status::InvalidConfig("'client_port' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> client_port_option = MakeUnique<IntegerOption>("client_port", rpc_client_port, 65535, 1024);
                    if (!client_port_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid client RPC port: {}", rpc_client_port));
                    }
                    global_options_.AddOption(std::move(client_port_option), GlobalOptionIndex::kClientPort);
                }

                {
                    // Client pool size
                    i64 connection_pool_size = 256;
                    if (network_config["connection_pool_size"]) {
                        if (network_config["connection_pool_size"].is_integer()) {
                            connection_pool_size = network_config["connection_pool_size"].value_or(connection_pool_size);
                        } else {
                            return Status::InvalidConfig("'connection_pool_size' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> connection_pool_size_option =
                        MakeUnique<IntegerOption>("connection_pool_size", connection_pool_size, 65536, 1);
                    if (!connection_pool_size_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid connection pool size: {}", connection_pool_size));
                    }

                    global_options_.AddOption(std::move(connection_pool_size_option), GlobalOptionIndex::kConnectionPoolSize);
                }
            } else {
                return Status::InvalidConfig("No 'network' section in configure file.");
            }
        }

        // Log
        {
            if (config_toml.contains("log")) {
                auto log_config = config_toml["log"];

                {
                    // Log file name
                    String log_filename = "infinity.log";
                    if (log_config["log_filename"]) {
                        if (log_config["log_filename"].is_string()) {
                            log_filename = log_config["log_filename"].value_or(log_filename);
                        } else {
                            return Status::InvalidConfig("'log_filename' field isn't string.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<StringOption> log_file_name_option = MakeUnique<StringOption>("log_filename", log_filename);
                    global_options_.AddOption(std::move(log_file_name_option), GlobalOptionIndex::kLogFileName);
                }

                {
                    // Log dir
                    String log_filename = "/var/infinity/log";
                    if (log_config["log_dir"]) {
                        if (log_config["log_dir"].is_string()) {
                            log_filename = log_config["log_dir"].value_or(log_filename);
                        } else {
                            return Status::InvalidConfig("'log_dir' field isn't string.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<StringOption> log_dir_option = MakeUnique<StringOption>("log_dir", log_filename);
                    global_options_.AddOption(std::move(log_dir_option), GlobalOptionIndex::kLogDir);
                }

                {
                    // Log To Stdout
                    bool log_to_stdout = false;
                    if (log_config["log_to_stdout"]) {
                        if (log_config["log_to_stdout"].is_boolean()) {
                            log_to_stdout = log_config["log_to_stdout"].value_or(log_to_stdout);
                        } else {
                            return Status::InvalidConfig("'log_to_stdout' field isn't boolean.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<BooleanOption> log_to_stdout_option = MakeUnique<BooleanOption>("log_to_stdout", log_to_stdout);
                    global_options_.AddOption(std::move(log_to_stdout_option), GlobalOptionIndex::kLogToStdout);
                }

                {
                    // Log File Max Size
                    i64 log_file_max_size = DEFAULT_LOG_FILE_SIZE;
                    if (log_config["log_file_max_size"]) {
                        if (log_config["log_file_max_size"].is_string()) {
                            String log_file_max_size_str = log_config["log_file_max_size"].value_or(DEFAULT_LOG_FILE_SIZE_STR);
                            auto res = ParseByteSize(log_file_max_size_str, log_file_max_size);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'log_file_max_size' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> log_file_max_size_option =
                        MakeUnique<IntegerOption>("log_file_max_size", log_file_max_size, std::numeric_limits<i64>::max(), 1024lu * 1024lu);

                    if (!log_file_max_size_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid max log file size: {}", log_file_max_size));
                    }
                    global_options_.AddOption(std::move(log_file_max_size_option), GlobalOptionIndex::kLogFileMaxSize);
                }

                {
                    // Log File Rotate count
                    i64 log_file_rotate_count = 8;
                    if (log_config["log_file_rotate_count"]) {
                        if (log_config["log_file_rotate_count"].is_integer()) {
                            log_file_rotate_count = log_config["log_file_rotate_count"].value_or(log_file_rotate_count);
                        } else {
                            return Status::InvalidConfig("'log_file_rotate_count' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> log_file_rotate_count_option =
                        MakeUnique<IntegerOption>("log_file_rotate_count", log_file_rotate_count, 65536, 1);

                    if (!log_file_rotate_count_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid log file rotate count: {}", log_file_rotate_count));
                    }
                    global_options_.AddOption(std::move(log_file_rotate_count_option), GlobalOptionIndex::kLogFileRotateCount);
                }

                {
                    // Log Level
                    LogLevel log_level = LogLevel::kInfo;
                    if (log_config["log_level"]) {
                        if (log_config["log_level"].is_string()) {
                            String log_level_str = log_config["log_level"].value_or("info");
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
                    } else {
                        ;
                    }

                    UniquePtr<LogLevelOption> log_level_option = MakeUnique<LogLevelOption>("log_level", log_level);
                    global_options_.AddOption(std::move(log_level_option), GlobalOptionIndex::kLogLevel);
                }

            } else {
                return Status::InvalidConfig("No 'Log' section in configure file.");
            }
        }

        // Storage
        {
            if (config_toml.contains("storage")) {
                auto storage_config = config_toml["storage"];
                {
                    // Data Dir
                    String data_dir = "/var/infinity/data";
                    if (storage_config["data_dir"]) {
                        if (storage_config["data_dir"].is_string()) {
                            data_dir = storage_config["data_dir"].value_or(data_dir);
                        } else {
                            return Status::InvalidConfig("'data_dir' field isn't string.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<StringOption> data_dir_option = MakeUnique<StringOption>("data_dir", data_dir);
                    global_options_.AddOption(std::move(data_dir_option), GlobalOptionIndex::kDataDir);
                }

                {
                    // Cleanup Interval
                    i64 cleanup_interval = DEFAULT_CLEANUP_INTERVAL_SEC;
                    if (storage_config["cleanup_interval"]) {
                        if (storage_config["cleanup_interval"].is_string()) {
                            String cleanup_interval_str = storage_config["cleanup_interval"].value_or(DEFAULT_CLEANUP_INTERVAL_SEC_STR);
                            auto res = ParseTimeInfo(cleanup_interval_str, cleanup_interval);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'cleanup_interval' field isn't string, such as \"1m\".");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> cleanup_interval_option =
                        MakeUnique<IntegerOption>("cleanup_interval", cleanup_interval, MAX_CLEANUP_INTERVAL_SEC, MIN_CLEANUP_INTERVAL_SEC);
                    if (!cleanup_interval_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid cleanup interval: {}", cleanup_interval));
                    }
                    global_options_.AddOption(std::move(cleanup_interval_option), GlobalOptionIndex::kCleanupInterval);
                }

                {
                    // Compact Interval
                    i64 compact_interval = DEFAULT_COMPACT_INTERVAL_SEC;
                    if (storage_config["compact_interval"]) {
                        if (storage_config["compact_interval"].is_string()) {
                            String compact_interval_str = storage_config["compact_interval"].value_or(DEFAULT_COMPACT_INTERVAL_SEC_STR);
                            auto res = ParseTimeInfo(compact_interval_str, compact_interval);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'compact_interval' field isn't string, such as \"1m\".");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> compact_interval_option =
                        MakeUnique<IntegerOption>("compact_interval", compact_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
                    if (!compact_interval_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid compact interval: {}", compact_interval));
                    }
                    global_options_.AddOption(std::move(compact_interval_option), GlobalOptionIndex::kCompactInterval);
                }

                {
                    // Optimize Index Interval
                    i64 optimize_index_interval = DEFAULT_OPTIMIZE_INTERVAL_SEC;
                    if (storage_config["optimize_interval"]) {
                        if (storage_config["optimize_interval"].is_string()) {
                            String optimize_index_interval_str = storage_config["buffer_manager_size"].value_or(DEFAULT_OPTIMIZE_INTERVAL_SEC_STR);
                            auto res = ParseTimeInfo(optimize_index_interval_str, optimize_index_interval);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'optimize_interval' field isn't string, such as \"1m\".");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> optimize_interval_option =
                        MakeUnique<IntegerOption>("optimize_interval", optimize_index_interval, MAX_COMPACT_INTERVAL_SEC, MIN_COMPACT_INTERVAL_SEC);
                    if (!optimize_interval_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid optimize interval: {}", optimize_index_interval));
                    }
                    global_options_.AddOption(std::move(optimize_interval_option), GlobalOptionIndex::kOptimizeIndexInterval);
                }

                {
                    // Mem Index Capacity
                    i64 mem_index_capacity = DEFAULT_MEMINDEX_CAPACITY;
                    if (storage_config["mem_index_capacity"]) {
                        if (storage_config["mem_index_capacity"].is_integer()) {
                            mem_index_capacity = storage_config["mem_index_capacity"].value_or(mem_index_capacity);
                        } else {
                            return Status::InvalidConfig("'mem_index_capacity' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> mem_index_capacity_option =
                        MakeUnique<IntegerOption>("mem_index_capacity", mem_index_capacity, MAX_MEMINDEX_CAPACITY, MIN_MEMINDEX_CAPACITY);
                    if (!mem_index_capacity_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid memory index capacity: {}", mem_index_capacity));
                    }
                    global_options_.AddOption(std::move(mem_index_capacity_option), GlobalOptionIndex::kMemIndexCapacity);
                }

            } else {
                return Status::InvalidConfig("No 'storage' section in configure file.");
            }
        }

        // Buffer
        {
            if (config_toml.contains("buffer")) {
                auto buffer_config = config_toml["buffer"];

                {
                    // Buffer Manager Size
                    i64 buffer_manager_size = DEFAULT_BUFFER_MANAGER_SIZE;
                    if (buffer_config["buffer_manager_size"]) {
                        if (buffer_config["buffer_manager_size"].is_string()) {
                            String buffer_manager_size_str = buffer_config["buffer_manager_size"].value_or(DEFAULT_BUFFER_MANAGER_SIZE_STR);
                            auto res = ParseByteSize(buffer_manager_size_str, buffer_manager_size);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'buffer_manager_size' field isn't string, such as \"4GB\"");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> buffer_manager_size_option =
                        MakeUnique<IntegerOption>("buffer_manager_size", buffer_manager_size, std::numeric_limits<i64>::max(), 0);
                    if (!buffer_manager_size_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid buffer manager size: {}", buffer_manager_size));
                    }
                    global_options_.AddOption(std::move(buffer_manager_size_option), GlobalOptionIndex::kBufferManagerSize);
                }

                {
                    // Temp Dir
                    String temp_dir = "/var/infinity/tmp";
                    if (buffer_config["temp_dir"]) {
                        if (buffer_config["temp_dir"].is_string()) {
                            temp_dir = buffer_config["temp_dir"].value_or(temp_dir);
                        } else {
                            return Status::InvalidConfig("'temp_dir' field isn't string.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<StringOption> temp_dir_option = MakeUnique<StringOption>("temp_dir", temp_dir);
                    global_options_.AddOption(std::move(temp_dir_option), GlobalOptionIndex::kTempDir);
                }

            } else {
                return Status::InvalidConfig("No 'buffer' section in configure file.");
            }
        }

        // WAL
        {
            if (config_toml.contains("wal")) {
                auto wal_config = config_toml["wal"];

                {
                    // WAL Dir
                    String wal_dir = "/var/infinity/wal";
                    if (wal_config["wal_dir"]) {
                        if (wal_config["wal_dir"].is_string()) {
                            wal_dir = wal_config["wal_dir"].value_or(wal_dir);
                        } else {
                            return Status::InvalidConfig("'wal_dir' field isn't string.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<StringOption> wal_dir_option = MakeUnique<StringOption>("wal_dir", wal_dir);
                    global_options_.AddOption(std::move(wal_dir_option), GlobalOptionIndex::kWALDir);
                }

                {
                    // WAL Compact Threshold
                    i64 wal_compact_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;
                    if (wal_config["wal_compact_threshold"]) {
                        if (wal_config["wal_compact_threshold"].is_string()) {
                            String wal_compact_threshold_str = wal_config["wal_compact_threshold"].value_or(DEFAULT_WAL_FILE_SIZE_THRESHOLD_STR);
                            auto res = ParseByteSize(wal_compact_threshold_str, wal_compact_threshold);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'wal_compact_threshold' field isn't string, such as \"1GB\".");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> wal_compact_threshold_option = MakeUnique<IntegerOption>("wal_compact_threshold",
                                                                                                      wal_compact_threshold,
                                                                                                      MAX_WAL_FILE_SIZE_THRESHOLD,
                                                                                                      MIN_WAL_FILE_SIZE_THRESHOLD);
                    if (!wal_compact_threshold_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid WAL compact threshold: {}", wal_compact_threshold));
                    }
                    global_options_.AddOption(std::move(wal_compact_threshold_option), GlobalOptionIndex::kWALCompactThreshold);
                }

                {
                    // Full Checkpoint Interval
                    i64 full_checkpoint_interval = DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC;
                    if (wal_config["full_checkpoint_interval"]) {
                        if (wal_config["full_checkpoint_interval"].is_string()) {
                            String full_checkpoint_interval_str =
                                wal_config["full_checkpoint_interval"].value_or(DEFAULT_FULL_CHECKPOINT_INTERVAL_SEC_STR);
                            auto res = ParseTimeInfo(full_checkpoint_interval_str, full_checkpoint_interval);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'full_checkpoint_interval' field isn't string, such as \"30s\".");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> full_checkpoint_interval_option = MakeUnique<IntegerOption>("full_checkpoint_interval",
                                                                                                         full_checkpoint_interval,
                                                                                                         MAX_FULL_CHECKPOINT_INTERVAL_SEC,
                                                                                                         MIN_FULL_CHECKPOINT_INTERVAL_SEC);
                    if (!full_checkpoint_interval_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid full checkpoint interval: {}", full_checkpoint_interval));
                    }

                    global_options_.AddOption(std::move(full_checkpoint_interval_option), GlobalOptionIndex::kFullCheckpointInterval);
                }

                {
                    // Delta Checkpoint Interval
                    i64 delta_checkpoint_interval = DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC;
                    if (wal_config["delta_checkpoint_interval"]) {
                        if (wal_config["delta_checkpoint_interval"].is_string()) {
                            String delta_checkpoint_interval_str =
                                wal_config["delta_checkpoint_interval"].value_or(DEFAULT_DELTA_CHECKPOINT_INTERVAL_SEC_STR);
                            auto res = ParseTimeInfo(delta_checkpoint_interval_str, delta_checkpoint_interval);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'delta_checkpoint_interval' field isn't string, such as \"30s\".");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> delta_checkpoint_interval_option = MakeUnique<IntegerOption>("delta_checkpoint_interval",
                                                                                                          delta_checkpoint_interval,
                                                                                                          MAX_DELTA_CHECKPOINT_INTERVAL_SEC,
                                                                                                          MIN_DELTA_CHECKPOINT_INTERVAL_SEC);
                    if (!delta_checkpoint_interval_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid delta checkpoint interval: {}", delta_checkpoint_interval));
                    }

                    global_options_.AddOption(std::move(delta_checkpoint_interval_option), GlobalOptionIndex::kDeltaCheckpointInterval);
                }

                {
                    // Delta Checkpoint Threshold
                    i64 delta_checkpoint_threshold = DELTA_CHECKPOINT_INTERVAL_WAL_BYTES;
                    if (wal_config["delta_checkpoint_threshold"]) {
                        if (wal_config["delta_checkpoint_threshold"].is_string()) {
                            String delta_checkpoint_threshold_str =
                                wal_config["delta_checkpoint_threshold"].value_or(DELTA_CHECKPOINT_INTERVAL_WAL_BYTES_STR);
                            auto res = ParseByteSize(delta_checkpoint_threshold_str, delta_checkpoint_threshold);
                            if (!res.ok()) {
                                return res;
                            }
                        } else {
                            return Status::InvalidConfig("'delta_checkpoint_threshold' field isn't integer.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<IntegerOption> delta_checkpoint_threshold_option = MakeUnique<IntegerOption>("delta_checkpoint_threshold",
                                                                                                           delta_checkpoint_threshold,
                                                                                                           MAX_CHECKPOINT_INTERVAL_WAL_BYTES,
                                                                                                           MIN_CHECKPOINT_INTERVAL_WAL_BYTES);
                    if (!delta_checkpoint_threshold_option->Validate()) {
                        return Status::InvalidConfig(fmt::format("Invalid delta checkpoint interval: {}", delta_checkpoint_threshold));
                    }

                    global_options_.AddOption(std::move(delta_checkpoint_threshold_option), GlobalOptionIndex::kDeltaCheckpointThreshold);
                }

                {
                    // Flush Method At Commit
                    FlushOptionType flush_option_type = FlushOptionType::kFlushAtOnce;
                    if (wal_config["flush_option"]) {
                        if (wal_config["flush_option"].is_string()) {
                            String flush_option_str = wal_config["flush_option"].value_or("flush_at_once");
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
                    } else {
                        ;
                    }

                    UniquePtr<FlushOption> flush_option = MakeUnique<FlushOption>("flush_option", flush_option_type);
                    global_options_.AddOption(std::move(flush_option), GlobalOptionIndex::kFlushMethodAtCommit);
                }

            } else {
                return Status::InvalidConfig("No 'wal' section in configure file.");
            }
        }

        // Resource
        {
            if (config_toml.contains("resource")) {
                auto resource_config = config_toml["resource"];

                {
                    // Resource Dir
                    String resource_dir = "/var/infinity/resource";
                    if (resource_config["resource_dir"]) {
                        if (resource_config["resource_dir"].is_string()) {
                            resource_dir = resource_config["resource_dir"].value_or(resource_dir);
                        } else {
                            return Status::InvalidConfig("'resource_dir' field isn't string.");
                        }
                    } else {
                        ;
                    }

                    UniquePtr<StringOption> resource_dir_option = MakeUnique<StringOption>("resource_dir", resource_dir);
                    global_options_.AddOption(std::move(resource_dir_option), GlobalOptionIndex::kResourcePath);
                }

            } else {
                return Status::InvalidConfig("No 'resource' section in configure file.");
            }
        }
    }

    return Status::OK();
}

// General
String Config::Version() { return global_options_.GetStringValue(GlobalOptionIndex::kVersion); }

String Config::TimeZone() { return global_options_.GetStringValue(GlobalOptionIndex::kTimeZone); }

i64 Config::TimeZoneBias() { return global_options_.GetIntegerValue(GlobalOptionIndex::kTimeZoneBias); }

// inline void set_worker_cpu_number(u64 new_cpu_limit) { system_option_.worker_cpu_limit = new_cpu_limit; }

i64 Config::CPULimit() { return global_options_.GetIntegerValue(GlobalOptionIndex::kWorkerCPULimit); }

// Network
String Config::ServerAddress() { return global_options_.GetStringValue(GlobalOptionIndex::kServerAddress); }

i64 Config::PostgresPort() { return global_options_.GetIntegerValue(GlobalOptionIndex::kPostgresPort); }

i64 Config::HTTPPort() { return global_options_.GetIntegerValue(GlobalOptionIndex::kHTTPPort); }

i64 Config::ClientPort() { return global_options_.GetIntegerValue(GlobalOptionIndex::kClientPort); }

i64 Config::ConnectionPoolSize() { return global_options_.GetIntegerValue(GlobalOptionIndex::kConnectionPoolSize); }

// Log
String Config::LogFileName() { return global_options_.GetStringValue(GlobalOptionIndex::kLogFileName); }

String Config::LogDir() { return global_options_.GetStringValue(GlobalOptionIndex::kLogDir); }

String Config::LogFilePath() { return fmt::format("{}/{}", LogDir(), LogFileName()); }

bool Config::LogToStdout() { return global_options_.GetBoolValue(GlobalOptionIndex::kLogToStdout); }

i64 Config::LogFileMaxSize() { return global_options_.GetIntegerValue(GlobalOptionIndex::kLogFileMaxSize); }

i64 Config::LogFileRotateCount() { return global_options_.GetIntegerValue(GlobalOptionIndex::kLogFileRotateCount); }

LogLevel Config::GetLogLevel() {
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kLogLevel);
    if (base_option->data_type_ != BaseOptionDataType::kLogLevel) {
        UnrecoverableError("Attempt to fetch log level value from log level data type option");
    }
    LogLevelOption *log_level_option = static_cast<LogLevelOption *>(base_option);
    return log_level_option->value_;
}

// Storage
String Config::DataDir() { return global_options_.GetStringValue(GlobalOptionIndex::kDataDir); }

i64 Config::CleanupInterval() { return global_options_.GetIntegerValue(GlobalOptionIndex::kCleanupInterval); }

i64 Config::CompactInterval() { return global_options_.GetIntegerValue(GlobalOptionIndex::kCompactInterval); }

i64 Config::OptimizeIndexInterval() { return global_options_.GetIntegerValue(GlobalOptionIndex::kOptimizeIndexInterval); }

i64 Config::MemIndexCapacity() { return global_options_.GetIntegerValue(GlobalOptionIndex::kMemIndexCapacity); }

// Buffer
i64 Config::BufferManagerSize() { return global_options_.GetIntegerValue(GlobalOptionIndex::kBufferManagerSize); }

String Config::TempDir() { return global_options_.GetStringValue(GlobalOptionIndex::kTempDir); }

// WAL
String Config::WALDir() { return global_options_.GetStringValue(GlobalOptionIndex::kWALDir); }

i64 Config::WALCompactThreshold() { return global_options_.GetIntegerValue(GlobalOptionIndex::kWALCompactThreshold); }

i64 Config::FullCheckpointInterval() { return global_options_.GetIntegerValue(GlobalOptionIndex::kFullCheckpointInterval); }

i64 Config::DeltaCheckpointInterval() { return global_options_.GetIntegerValue(GlobalOptionIndex::kDeltaCheckpointInterval); }

i64 Config::DeltaCheckpointThreshold() { return global_options_.GetIntegerValue(GlobalOptionIndex::kDeltaCheckpointThreshold); }

FlushOptionType Config::FlushMethodAtCommit() {
    BaseOption *base_option = global_options_.GetOptionByIndex(GlobalOptionIndex::kFlushMethodAtCommit);
    if (base_option->data_type_ != BaseOptionDataType::kFlush) {
        UnrecoverableError("Attempt to fetch flush option value from flush option data type option");
    }
    FlushOption *flush_option = static_cast<FlushOption *>(base_option);
    return flush_option->value_;
}

// Resource
String Config::ResourcePath() { return global_options_.GetStringValue(GlobalOptionIndex::kResourcePath); }

//// Profiler
// bool enable_profiler() const { return system_option_.enable_profiler; }
//
// SizeT profile_record_capacity() const { return system_option_.profile_record_capacity; }

void Config::PrintAll() {
    fmt::print("Infinity system configs: \n");

    // General
    fmt::print(" - version: {}\n", Version());
    fmt::print(" - timezone: {}{}\n", TimeZone(), TimeZoneBias());
    fmt::print(" - cpu_limit: {}\n", CPULimit());

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
    fmt::print(" - data_dir: {}\n", DataDir());
    fmt::print(" - cleanup_interval: {}\n", Utility::FormatTimeInfo(CleanupInterval()));
    fmt::print(" - compact_interval: {}\n", Utility::FormatTimeInfo(CompactInterval()));
    fmt::print(" - optimize_index_interval: {}\n", Utility::FormatTimeInfo(OptimizeIndexInterval()));
    fmt::print(" - memindex_capacity: {}\n", Utility::FormatByteSize(MemIndexCapacity()));

    // Buffer manager
    fmt::print(" - buffer_manager_size: {}\n", Utility::FormatByteSize(BufferManagerSize()));
    fmt::print(" - temp_dir: {}\n", TempDir());

    // WAL
    fmt::print(" - wal_dir: {}\n", WALDir());
    fmt::print(" - buffer_manager_size: {}\n", Utility::FormatByteSize(WALCompactThreshold()));
    fmt::print(" - full_checkpoint_interval: {}\n", Utility::FormatTimeInfo(FullCheckpointInterval()));
    fmt::print(" - delta_checkpoint_interval: {}\n", Utility::FormatTimeInfo(DeltaCheckpointInterval()));
    fmt::print(" - delta_checkpoint_threshold: {}\n", Utility::FormatByteSize(DeltaCheckpointThreshold()));
    fmt::print(" - flush_method_at_commit: {}\n", FlushOptionTypeToString(FlushMethodAtCommit()));

    // Resource dir
    fmt::print(" - wal_dir: {}\n", ResourcePath());
}

} // namespace infinity
