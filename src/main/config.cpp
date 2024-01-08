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

module config;
//
import third_party;
import stl;
import boost;
import compilation_config;
import default_values;
import logger;
import local_file_system;
import utility;

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

SharedPtr<String> Config::ParseByteSize(const String &byte_size_str, u64 &byte_size) {

    HashMap<String, u64> byte_unit = {{"kb", 1024ul}, {"mb", 1024ul * 1024ul}, {"gb", 1024ul * 1024ul * 1024ul}};
    if (byte_size_str.empty()) {
        return MakeShared<String>("No byte size is given");
    }

    u64 factor;
    auto res = std::from_chars(byte_size_str.data(), byte_size_str.data() + byte_size_str.size(), factor);
    if(res.ec == std::errc()) {
        String unit = res.ptr;
        ToLower(unit);
        auto it = byte_unit.find(unit);
        if (it != byte_unit.end()) {
            byte_size = factor * it->second;
            return nullptr;
        } else {
            return MakeShared<String>("Unrecognized byte size");
        }
    } else {
        return MakeShared<String>("Unrecognized byte size");
    }
}

// extern SharedPtr<spdlogger> infinity_logger;

SharedPtr<String> Config::Init(const SharedPtr<String> &config_path) {
    SharedPtr<String> result;

    // Default general config
    String default_version = fmt::format("{}.{}.{}", version_major(), version_minor(), version_patch());

    String default_time_zone = "UTC";
    i32 default_time_zone_bias = 8;

    // Default system config
    u64 default_total_cpu_number = Thread::hardware_concurrency();
    u64 default_total_memory_size = GetAvailableMem();
    u64 default_query_cpu_limit = default_total_cpu_number;
    u64 default_query_memory_limit = default_total_memory_size;

    // Default profiler config
    bool default_enable_profiler = false;
    u64 default_profile_record_capacity = 100;

    // Default network config
    String default_listen_address = "0.0.0.0";
    u32 default_pg_port = 5432;
    u32 default_http_port = 8088;
    u32 default_sdk_port = 23817;

    // Default log config
    SharedPtr<String> default_log_filename = MakeShared<String>("infinity.log");
    SharedPtr<String> default_log_dir = MakeShared<String>("/tmp/infinity/log");
    bool default_log_to_stdout = false;
    u64 default_log_max_size = 1024lu * 1024lu * 1024lu; // 1Gib
    u64 default_log_file_rotate_count = 10;

    // Set the log level before performance test of DB
    LogLevel default_log_level = LogLevel::kInfo;

    // Default storage config
    SharedPtr<String> default_data_dir = MakeShared<String>("/tmp/infinity/data");
    SharedPtr<String> default_wal_dir = MakeShared<String>("/tmp/infinity/wal");
    u64 default_row_size = 8192lu;

    // Default buffer config
    u64 default_buffer_pool_size = 4 * 1024lu * 1024lu * 1024lu; // 4Gib
    SharedPtr<String> default_temp_dir = MakeShared<String>("/tmp/infinity/temp");

    // Default wal config
    u64 wal_size_threshold = DEFAULT_WAL_FILE_SIZE_THRESHOLD;
    // Attention: this phase full checkpoint interval is used for testing,
    //            it should be set to a larger value in production environment.
    u64 full_checkpoint_interval_sec = FULL_CHECKPOINT_INTERVAL_SEC;
    u64 delta_checkpoint_interval_sec = DELTA_CHECKPOINT_INTERVAL_SEC;
    u64 delta_checkpoint_interval_wal_bytes = DELTA_CHECKPOINT_INTERVAL_WAL_BYTES;

    // Default resource config
    String default_resource_dict_path = String("/tmp/infinity/resource");

    LocalFileSystem fs;
    if (config_path.get() == nullptr || !fs.Exists(*config_path)) {
        fmt::print("No config file is given, use default configs.\n");

        // General
        {
            system_option_.version = default_version;
            system_option_.time_zone = default_time_zone;
            system_option_.time_zone_bias = default_time_zone_bias;
        }

        // System
        {
            system_option_.worker_cpu_limit = default_total_cpu_number;
            system_option_.total_memory_size = default_total_memory_size;
            system_option_.query_cpu_limit = default_query_cpu_limit;
            system_option_.query_memory_limit = default_query_memory_limit;
        }

        // Profiler
        {
            system_option_.enable_profiler = default_enable_profiler;
            system_option_.profile_record_capacity = default_profile_record_capacity;
        }

        // Network
        {
            system_option_.listen_address = default_listen_address;
            system_option_.pg_port = default_pg_port;
            system_option_.http_port = default_http_port;
            system_option_.sdk_port = default_sdk_port;
        }

        // Log
        {
            system_option_.log_filename = default_log_filename;
            system_option_.log_dir = default_log_dir;
            system_option_.log_file_path = MakeShared<String>(fmt::format("{}/{}", *system_option_.log_dir, *system_option_.log_filename));
            system_option_.log_to_stdout = default_log_to_stdout;
            system_option_.log_max_size = default_log_max_size; // 1Gib
            system_option_.log_file_rotate_count = default_log_file_rotate_count;
            system_option_.log_level = default_log_level;
        }

        // Storage
        {
            system_option_.data_dir = MakeShared<String>(*default_data_dir);
            system_option_.wal_dir = MakeShared<String>(*default_wal_dir);
            system_option_.default_row_size = default_row_size;
        }

        // Buffer
        {
            system_option_.buffer_pool_size = default_buffer_pool_size; // 4Gib
            system_option_.temp_dir = MakeShared<String>(*default_temp_dir);
        }

        // Wal
        {
            system_option_.wal_size_threshold_ = wal_size_threshold;
            system_option_.full_checkpoint_interval_sec_ = full_checkpoint_interval_sec;
            system_option_.delta_checkpoint_interval_sec_ = delta_checkpoint_interval_sec;
            system_option_.delta_checkpoint_interval_wal_bytes_ = delta_checkpoint_interval_wal_bytes;
        }

        // Resource
        { system_option_.resource_dict_path_ = default_resource_dict_path; }
    } else {
        fmt::print("Read config from: {}\n", *config_path);
        toml::table config = toml::parse_file(*config_path);
        // General
        {
            auto general_config = config["general"];

            String infinity_version = general_config["version"].value_or("invalid");
            if (!IsEqual(default_version, infinity_version)) {
                return MakeShared<String>("Unmatched version in config file.");
            }
            system_option_.version = infinity_version;

            String time_zone_str = general_config["timezone"].value_or("invalid");
            if (IsEqual(time_zone_str, "invalid")) {
                result = MakeShared<String>("Timezone isn't given in config file.");
                return result;
            }

            try {
                ParseTimeZoneStr(time_zone_str, system_option_.time_zone, system_option_.time_zone_bias);
            } catch (...) {
                result = MakeShared<String>(fmt::format("Timezone can't be recognized: {}", time_zone_str));
                return result;
            }
        }

        // System
        {
            auto system_config = config["system"];
            system_option_.worker_cpu_limit = system_config["worker_cpu_limit"].value_or(default_total_cpu_number);
            if (system_option_.worker_cpu_limit == 0) {
                system_option_.worker_cpu_limit = default_total_cpu_number;
            }

            if (system_config["total_memory_size"].as_integer() != 0) {
                i64 total_memory_size_int = system_config["total_memory_size"].value_or(default_total_memory_size);
                if (total_memory_size_int <= 0) {
                    system_option_.total_memory_size = default_total_memory_size;
                } else {
                    system_option_.total_memory_size = total_memory_size_int;
                }
            } else {
                String total_memory_size_str = system_config["total_memory_size"].value_or("8GB");
                result = ParseByteSize(total_memory_size_str, system_option_.total_memory_size);
                if (result.get() != nullptr) {
                    return result;
                }
            }

            if (system_option_.total_memory_size > default_total_memory_size) {
                system_option_.total_memory_size = default_total_memory_size;
            }

            system_option_.query_cpu_limit = system_config["query_cpu_limit"].value_or(default_query_cpu_limit);
            if (system_option_.query_cpu_limit == 0) {
                system_option_.query_cpu_limit = default_query_cpu_limit;
            }

            system_option_.query_memory_limit = system_config["query_memory_limit"].value_or(default_query_memory_limit);
            if (system_option_.query_memory_limit == 0) {
                system_option_.query_memory_limit = default_query_memory_limit;
            }

            if (system_config["query_memory_limit"].as_integer() != 0) {
                i64 query_memory_size_int = system_config["query_memory_limit"].value_or(default_query_memory_limit);
                if (query_memory_size_int <= 0) {
                    system_option_.query_memory_limit = default_query_memory_limit;
                } else {
                    system_option_.query_memory_limit = query_memory_size_int;
                }
            } else {
                String query_memory_limit_str = system_config["query_memory_limit"].value_or("4MB");
                result = ParseByteSize(query_memory_limit_str, system_option_.query_memory_limit);
                if (result.get() != nullptr) {
                    return result;
                }
            }

            if (system_option_.query_memory_limit > default_query_memory_limit) {
                system_option_.query_memory_limit = default_query_memory_limit;
            }
        }

        // Profiler
        {
            auto profiler_config = config["profiler"];
            system_option_.enable_profiler = profiler_config["enable"].value_or(default_enable_profiler);
            system_option_.profile_record_capacity = profiler_config["profile_record_capacity"].value_or(default_profile_record_capacity);
        }

        // Network
        {
            auto network_config = config["network"];
            system_option_.listen_address = network_config["listen_address"].value_or(default_listen_address);

            // Validate the address format
            BoostErrorCode error;
            asio_make_address(system_option_.listen_address, error);
            if (error) {
                String err_msg = fmt::format("Not a valid IPv4 address: {}", system_option_.listen_address);
                result = MakeShared<String>(err_msg);
                return result;
            }

            system_option_.pg_port = network_config["pg_port"].value_or(default_pg_port);
            system_option_.http_port = network_config["http_port"].value_or(default_http_port);
            system_option_.sdk_port = network_config["sdk_port"].value_or(default_sdk_port);
        }

        // Log
        {
            auto log_config = config["log"];
            system_option_.log_filename = MakeShared<String>(log_config["log_filename"].value_or(*default_log_filename));
            system_option_.log_dir = MakeShared<String>(log_config["log_dir"].value_or(*default_log_dir));

            String log_file_path = fmt::format("{}/{}", *system_option_.log_dir, *system_option_.log_filename);
            system_option_.log_file_path = MakeShared<String>(log_file_path);
            system_option_.log_to_stdout = log_config["log_to_stdout"].value_or(default_log_to_stdout);

            String log_max_size_str = log_config["log_max_size"].value_or("1GB");
            result = ParseByteSize(log_max_size_str, system_option_.log_max_size);
            if (result.get() != nullptr) {
                return result;
            }

            system_option_.log_file_rotate_count = log_config["log_file_rotate_count"].value_or(default_log_file_rotate_count);

            String log_level = log_config["log_level"].value_or("invalid");
            if (IsEqual(log_level, "trace")) {
                system_option_.log_level = LogLevel::kTrace;
            } else if (IsEqual(log_level, "info")) {
                system_option_.log_level = LogLevel::kInfo;
            } else if (IsEqual(log_level, "warning")) {
                system_option_.log_level = LogLevel::kWarning;
            } else if (IsEqual(log_level, "error")) {
                system_option_.log_level = LogLevel::kError;
            } else if (IsEqual(log_level, "critical")) {
                system_option_.log_level = LogLevel::kFatal;
            } else {
                result = MakeShared<String>("Invalid log level in config file");
                return result;
            }
        }

        // Storage
        {
            auto storage_config = config["storage"];
            system_option_.data_dir = MakeShared<String>(storage_config["data_dir"].value_or(*default_data_dir));
            system_option_.wal_dir = MakeShared<String>(storage_config["wal_dir"].value_or(*default_wal_dir));
            system_option_.default_row_size = storage_config["default_row_size"].value_or(default_row_size);
        }

        // Buffer
        {
            auto buffer_config = config["buffer"];
            String buffer_pool_size_str = buffer_config["buffer_pool_size"].value_or("4GB");
            result = ParseByteSize(buffer_pool_size_str, system_option_.buffer_pool_size);
            if (result.get() != nullptr) {
                return result;
            }
            system_option_.temp_dir = MakeShared<String>(buffer_config["temp_dir"].value_or("invalid"));
        }

        // Wal
        {
            auto wal_config = config["wal"];
            system_option_.full_checkpoint_interval_sec_ = wal_config["full_checkpoint_interval_sec"].value_or(full_checkpoint_interval_sec);
            system_option_.delta_checkpoint_interval_sec_ = wal_config["delta_checkpoint_interval_sec"].value_or(delta_checkpoint_interval_sec);
            system_option_.delta_checkpoint_interval_wal_bytes_ =
                wal_config["delta_checkpoint_interval_wal_bytes"].value_or(delta_checkpoint_interval_wal_bytes);
            auto wal_file_size_threshold_str = wal_config["wal_file_size_threshold"].value_or("10KB");
            result = ParseByteSize(wal_file_size_threshold_str, system_option_.wal_size_threshold_);
            if (result.get() != nullptr) {
                return result;
            }
        }

        // Resource
        {
            auto resource_config = config["resource"];
            system_option_.resource_dict_path_ = resource_config["dictionary_dir"].value_or(default_resource_dict_path);
        }
    }

    return result;
}

void Config::PrintAll() const {
    fmt::print("Infinity system parameters: \n");

    // General
    fmt::print(" - version: {}\n", system_option_.version);
    fmt::print(" - timezone: {}{}\n", system_option_.time_zone, system_option_.time_zone_bias);

    // System
    fmt::print(" - worker_cpu_limit: {}\n", system_option_.worker_cpu_limit);
    fmt::print(" - total_memory_size: {}\n", Utility::FormatByteSize(system_option_.total_memory_size));
    fmt::print(" - query_cpu_limit: {}\n", system_option_.query_cpu_limit);
    fmt::print(" - query_memory_limit: {}\n", Utility::FormatByteSize(system_option_.query_memory_limit));

    // Profiler
    fmt::print(" - enable_profiler: {}\n", system_option_.enable_profiler);
    fmt::print(" - profile_record_capacity: {}\n", system_option_.profile_record_capacity);

    // Network
    fmt::print(" - listen address: {}\n", system_option_.listen_address);
    fmt::print(" - postgres port: {}\n", system_option_.pg_port);
    fmt::print(" - http port: {}\n", system_option_.http_port);
    fmt::print(" - sdk port: {}\n", system_option_.sdk_port);

    // Log
    fmt::print(" - log_file_path: {}\n", system_option_.log_file_path->c_str());
    fmt::print(" - log_to_stdout: {}\n", system_option_.log_to_stdout);
    fmt::print(" - log_max_size: {}\n", Utility::FormatByteSize(system_option_.log_max_size));
    fmt::print(" - log_file_rotate_count: {}\n", system_option_.log_file_rotate_count);
    fmt::print(" - log_level: {}\n", LogLevel2Str(system_option_.log_level));

    // Storage
    fmt::print(" - data_dir: {}\n", system_option_.data_dir->c_str());
    fmt::print(" - wal_dir: {}\n", system_option_.wal_dir->c_str());
    fmt::print(" - default_row_size: {}\n", system_option_.default_row_size);

    // Buffer
    fmt::print(" - buffer_pool_size: {}\n", Utility::FormatByteSize(system_option_.buffer_pool_size));
    fmt::print(" - temp_dir: {}\n", system_option_.temp_dir->c_str());

    // Wal
    fmt::print(" - full_checkpoint_interval_sec: {}\n", system_option_.full_checkpoint_interval_sec_);
    fmt::print(" - full_checkpoint_txn_interval: {}\n", system_option_.full_checkpoint_txn_interval_);
    fmt::print(" - delta_checkpoint_interval_sec: {}\n", system_option_.delta_checkpoint_interval_sec_);
    fmt::print(" - delta_checkpoint_interval_wal_bytes: {}\n", system_option_.delta_checkpoint_interval_wal_bytes_);
    fmt::print(" - wal_size_threshold: {}\n", system_option_.wal_size_threshold_);

    // Resource
    fmt::print(" - dictionary_dir: {}\n", system_option_.resource_dict_path_.c_str());
}

} // namespace infinity
