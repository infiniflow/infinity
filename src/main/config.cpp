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

namespace infinity {

u64 Config::GetAvailableMem() {
    u64 pages = sysconf(_SC_PHYS_PAGES);
    u64 page_size = sysconf(_SC_PAGE_SIZE); // Byte
    return pages * page_size;
}

void Config::ParseTimeZoneStr(const String &time_zone_str, String &parsed_time_zone, i32 &parsed_time_zone_bias) {
    parsed_time_zone = time_zone_str.substr(0, 3);
    ToUpper(parsed_time_zone);
    parsed_time_zone_bias = StrToInt(time_zone_str.substr(3, String::npos));
}

SharedPtr<String> Config::ParseByteSize(const String &byte_size_str, u64 &byte_size) {

    HashMap<String, u64> byte_unit = {{"kb", 1024ul}, {"mb", 1024ul * 1024ul}, {"gb", 1024ul * 1024ul * 1024ul}};
    if (byte_size_str.empty()) {
        return MakeShared<String>("No byte size is given");
        ;
    }

    u64 factor;
    const char *ptr = FromChars(byte_size_str.data(), byte_size_str.data() + byte_size_str.size(), factor);
    if (ptr == nullptr) {
        return MakeShared<String>("Unrecognized byte size");
    } else {
        String unit = ptr;
        ToLower(unit);
        auto it = byte_unit.find(unit);
        if (it != byte_unit.end()) {
            byte_size = factor * it->second;
            return nullptr;
        } else {
            return MakeShared<String>("Unrecognized byte size");
        }
    }
}

// extern SharedPtr<spdlogger> infinity_logger;

SharedPtr<String> Config::Init(const SharedPtr<String> &config_path) {
    SharedPtr<String> result;

    // Default general config
    String default_version = Format("{}.{}.{}", version_major(), version_minor(), version_patch());

    String default_time_zone = "UTC";
    i32 default_time_zone_bias = 8;

    // Default system config
    u64 default_total_cpu_number = Thread::hardware_concurrency();
    u64 default_total_memory_size = GetAvailableMem();
    u64 default_query_cpu_limit = default_total_cpu_number;
    u64 default_query_memory_limit = default_total_memory_size;

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
    LogLevel default_log_level = LogLevel::kTrace;

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

    LocalFileSystem fs;
    if (config_path.get() == nullptr || !fs.Exists(*config_path)) {
        Printf("No config file is given, use default configs.");

        // General
        {
            option_.version = default_version;
            option_.time_zone = default_time_zone;
            option_.time_zone_bias = default_time_zone_bias;
        }

        // System
        {
            option_.total_cpu_number = default_total_cpu_number;
            option_.total_memory_size = default_total_memory_size;
            option_.query_cpu_limit = default_query_cpu_limit;
            option_.query_memory_limit = default_query_memory_limit;
        }

        // Network
        {
            option_.listen_address = default_listen_address;
            option_.pg_port = default_pg_port;
            option_.http_port = default_http_port;
            option_.sdk_port = default_sdk_port;
        }

        // Log
        {
            option_.log_filename = default_log_filename;
            option_.log_dir = default_log_dir;
            option_.log_file_path = MakeShared<String>(Format("{}/{}", *option_.log_dir, *option_.log_filename));
            option_.log_to_stdout = default_log_to_stdout;
            option_.log_max_size = default_log_max_size; // 1Gib
            option_.log_file_rotate_count = default_log_file_rotate_count;
            option_.log_level = default_log_level;
        }

        // Storage
        {
            option_.data_dir = MakeShared<String>(*default_data_dir);
            option_.wal_dir = MakeShared<String>(*default_wal_dir);
            option_.default_row_size = default_row_size;
        }

        // Buffer
        {
            option_.buffer_pool_size = default_buffer_pool_size; // 4Gib
            option_.temp_dir = MakeShared<String>(*default_temp_dir);
        }

        // Wal
        {
            option_.wal_size_threshold_ = wal_size_threshold;
            option_.full_checkpoint_interval_sec_ = full_checkpoint_interval_sec;
            option_.delta_checkpoint_interval_sec_ = delta_checkpoint_interval_sec;
            option_.delta_checkpoint_interval_wal_bytes_ = delta_checkpoint_interval_wal_bytes;
        }
    } else {
        Printf("Read config from: {}", *config_path);
        TomlTable config = TomlParseFile(*config_path);
        // General
        {
            auto general_config = config["general"];

            String infinity_version = general_config["version"].value_or("invalid");
            if (!IsEqual(default_version, infinity_version)) {
                return MakeShared<String>("Unmatched version in config file.");
            }
            option_.version = infinity_version;

            String time_zone_str = general_config["timezone"].value_or("invalid");
            if (IsEqual(time_zone_str, "invalid")) {
                result = MakeShared<String>("Timezone isn't given in config file.");
                return result;
            }

            try {
                ParseTimeZoneStr(time_zone_str, option_.time_zone, option_.time_zone_bias);
            } catch (...) {
                result = MakeShared<String>(Format("Timezone can't be recognized: {}", time_zone_str));
                return result;
            }
        }

        // System
        {
            auto system_config = config["system"];
            option_.total_cpu_number = system_config["total_cpu_number"].value_or(default_total_cpu_number);

            String total_memory_size_str = system_config["total_memory_size"].value_or("8GB");
            result = ParseByteSize(total_memory_size_str, option_.total_memory_size);
            if (result.get() != nullptr) {
                return result;
            }
            if (option_.total_memory_size > default_total_memory_size) {
                option_.total_memory_size = default_total_memory_size;
            }

            option_.query_cpu_limit = system_config["query_cpu_limit"].value_or(default_query_cpu_limit);
            option_.query_memory_limit = system_config["query_memory_limit"].value_or(default_query_memory_limit);

            String query_memory_limit_str = system_config["query_memory_limit"].value_or("4MB");
            result = ParseByteSize(query_memory_limit_str, option_.query_memory_limit);
            if (result.get() != nullptr) {
                return result;
            }
            if (option_.query_memory_limit > default_query_memory_limit) {
                option_.query_memory_limit = default_query_memory_limit;
            }
        }

        // Network
        {
            auto network_config = config["network"];
            option_.listen_address = network_config["listen_address"].value_or(default_listen_address);

            // Validate the address format
            BoostErrorCode error;
            asio_make_address(option_.listen_address, error);
            if (error) {
                String err_msg = Format("Not a valid IPv4 address: {}", option_.listen_address);
                result = MakeShared<String>(err_msg);
                return result;
            }

            option_.pg_port = network_config["pg_port"].value_or(default_pg_port);
            option_.http_port = network_config["http_port"].value_or(default_http_port);
            option_.sdk_port = network_config["sdk_port"].value_or(default_sdk_port);
        }

        // Log
        {
            auto log_config = config["log"];
            option_.log_filename = MakeShared<String>(log_config["log_filename"].value_or(*default_log_filename));
            option_.log_dir = MakeShared<String>(log_config["log_dir"].value_or(*default_log_dir));

            String log_file_path = Format("{}/{}", *option_.log_dir, *option_.log_filename);
            option_.log_file_path = MakeShared<String>(log_file_path);
            option_.log_to_stdout = log_config["log_to_stdout"].value_or(default_log_to_stdout);

            String log_max_size_str = log_config["log_max_size"].value_or("1GB");
            result = ParseByteSize(log_max_size_str, option_.log_max_size);
            if (result.get() != nullptr) {
                return result;
            }

            option_.log_file_rotate_count = log_config["log_file_rotate_count"].value_or(default_log_file_rotate_count);

            String log_level = log_config["log_level"].value_or("invalid");
            if (IsEqual(log_level, "trace")) {
                option_.log_level = LogLevel::kTrace;
            } else if (IsEqual(log_level, "info")) {
                option_.log_level = LogLevel::kInfo;
            } else if (IsEqual(log_level, "warning")) {
                option_.log_level = LogLevel::kWarning;
            } else if (IsEqual(log_level, "error")) {
                option_.log_level = LogLevel::kError;
            } else if (IsEqual(log_level, "critical")) {
                option_.log_level = LogLevel::kFatal;
            } else {
                result = MakeShared<String>("Invalid log level in config file");
                return result;
            }
        }

        // Storage
        {
            auto storage_config = config["storage"];
            option_.data_dir = MakeShared<String>(storage_config["data_dir"].value_or(*default_data_dir));
            option_.wal_dir = MakeShared<String>(storage_config["wal_dir"].value_or(*default_wal_dir));
            option_.default_row_size = storage_config["default_row_size"].value_or(default_row_size);
        }

        // Buffer
        {
            auto buffer_config = config["buffer"];
            String buffer_pool_size_str = buffer_config["buffer_pool_size"].value_or("4GB");
            result = ParseByteSize(buffer_pool_size_str, option_.buffer_pool_size);
            if (result.get() != nullptr) {
                return result;
            }
            option_.temp_dir = MakeShared<String>(buffer_config["temp_dir"].value_or("invalid"));
        }

        // Wal
        {
            auto wal_config = config["wal"];
            option_.full_checkpoint_interval_sec_ = wal_config["full_checkpoint_interval_sec"].value_or(full_checkpoint_interval_sec);
            option_.delta_checkpoint_interval_sec_ = wal_config["delta_checkpoint_interval_sec"].value_or(delta_checkpoint_interval_sec);
            option_.delta_checkpoint_interval_wal_bytes_ =
                wal_config["delta_checkpoint_interval_wal_bytes"].value_or(delta_checkpoint_interval_wal_bytes);
            auto wal_file_size_threshold_str = wal_config["wal_file_size_threshold"].value_or("10KB");
            result = ParseByteSize(wal_file_size_threshold_str, option_.wal_size_threshold_);
            if (result.get() != nullptr) {
                return result;
            }
        }
    }

    return result;
}

void Config::PrintAll() const {
    Printf("Infinity system parameters: \n");

    // General
    Printf(" - version: {}\n", option_.version);
    Printf(" - timezone: {}{}\n", option_.time_zone, option_.time_zone_bias);

    // System
    Printf(" - total_cpu_number: {}\n", option_.total_cpu_number);
    Printf(" - total_memory_size: {}\n", option_.total_memory_size);
    Printf(" - query_cpu_limit: {}\n", option_.query_cpu_limit);
    Printf(" - query_memory_limit: {}\n", option_.query_memory_limit);

    // Network
    Printf(" - listen address: {}\n", option_.listen_address);
    Printf(" - postgres port: {}\n", option_.pg_port);
    Printf(" - http port: {}\n", option_.http_port);
    Printf(" - sdk port: {}\n", option_.sdk_port);

    // Log
    Printf(" - log_file_path: {}\n", option_.log_file_path->c_str());
    Printf(" - log_to_stdout: {}\n", option_.log_to_stdout);
    Printf(" - log_max_size: {}\n", option_.log_max_size);
    Printf(" - log_file_rotate_count: {}\n", option_.log_file_rotate_count);
    Printf(" - log_level: {}\n", LogLevel2Str(option_.log_level));

    // Storage
    Printf(" - data_dir: {}\n", option_.data_dir->c_str());
    Printf(" - wal_dir: {}\n", option_.wal_dir->c_str());
    Printf(" - default_row_size: {}\n", option_.default_row_size);

    // Buffer
    Printf(" - buffer_pool_size: {}\n", option_.buffer_pool_size);
    Printf(" - temp_dir: {}\n", option_.temp_dir->c_str());

    // Wal
    Printf(" - full_checkpoint_interval_sec: {}\n", option_.full_checkpoint_interval_sec_);
    Printf(" - full_checkpoint_txn_interval: {}\n", option_.full_checkpoint_txn_interval_);
    Printf(" - delta_checkpoint_interval_sec: {}\n", option_.delta_checkpoint_interval_sec_);
    Printf(" - delta_checkpoint_interval_wal_bytes: {}\n", option_.delta_checkpoint_interval_wal_bytes_);
    Printf(" - wal_size_threshold: {}\n", option_.wal_size_threshold_);
}

} // namespace infinity
