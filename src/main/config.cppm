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

export module config;

import stl;
import third_party;

namespace infinity {

export struct Options {
    // General
    String version{};
    String time_zone{};
    i32 time_zone_bias{};

    // System
    u64 total_cpu_number{};
    u64 total_memory_size{};
    u64 query_cpu_limit{};
    u64 query_memory_limit{};

    // Profiler
    bool enable = false;

    // Network
    String listen_address{};
    u16 pg_port{};
    u32 http_port{};
    u32 sdk_port{};

    // Log
    SharedPtr<String> log_filename{MakeShared<String>("infinity.log")};
    SharedPtr<String> log_dir{};
    SharedPtr<String> log_file_path{};
    bool log_to_stdout{};
    u64 log_max_size{};
    SizeT log_file_rotate_count{};
    LogLevel log_level{};
//    spdlog::level::level_enum log_level{spdlog::level::info};

    // Storage
    SharedPtr<String> data_dir{};
    SharedPtr<String> wal_dir{};
    u64 default_row_size{};

    // Buffer
    u64 buffer_pool_size{};
    SharedPtr<String> temp_dir{};

    // Wal
    u64 wal_size_threshold_{};
    u64 full_checkpoint_interval_sec_{};
    u64 full_checkpoint_txn_interval_{};
    u64 delta_checkpoint_interval_sec_{};
    u64 delta_checkpoint_interval_wal_bytes_{};
};

export struct Config {
public:
    SharedPtr<String> Init(const SharedPtr<String> &config_path);

    void PrintAll() const;

    // General
    [[nodiscard]] inline String version() const { return option_.version; }

    [[nodiscard]] inline String time_zone() const { return option_.time_zone; }

    [[nodiscard]] inline i32 time_zone_bias() const { return option_.time_zone_bias; }

    [[nodiscard]] inline u64 total_cpu_number() const { return option_.total_cpu_number; }

    [[nodiscard]] inline u64 total_memory_size() const { return option_.total_memory_size; }

    [[nodiscard]] inline u64 query_cpu_limit() const { return option_.query_cpu_limit; }

    [[nodiscard]] inline u64 query_memory_limit() const { return option_.query_memory_limit; }

    [[nodiscard]] inline String listen_address() const { return option_.listen_address; }

    [[nodiscard]] inline u16 pg_port() const { return option_.pg_port; }

    [[nodiscard]] inline u32 http_port() const { return option_.http_port; }

    [[nodiscard]] inline u32 sdk_port() const { return option_.sdk_port; }

    // Profiler
    [[nodiscard]] inline bool enable_profiler() const { return option_.enable; }

    // Log
    [[nodiscard]] inline SharedPtr<String> log_filename() const { return option_.log_filename; }

    [[nodiscard]] inline SharedPtr<String> log_dir() const { return option_.log_dir; }

    [[nodiscard]] inline SharedPtr<String> log_file_path() const { return option_.log_file_path; }

    [[nodiscard]] inline bool log_to_stdout() const { return option_.log_to_stdout; }

    [[nodiscard]] inline SizeT log_max_size() const { return option_.log_max_size; }

    [[nodiscard]] inline SizeT log_file_rotate_count() const { return option_.log_file_rotate_count; }

    [[nodiscard]] inline LogLevel log_level() const { return option_.log_level; }

    // Storage
    [[nodiscard]] inline SharedPtr<String> data_dir() const { return option_.data_dir; }

    [[nodiscard]] inline SharedPtr<String> wal_dir() const { return option_.wal_dir; }

    [[nodiscard]] inline u64 default_row_size() const { return option_.default_row_size; }

    [[nodiscard]] inline u64 buffer_pool_size() const { return option_.buffer_pool_size; }

    [[nodiscard]] inline SharedPtr<String> temp_dir() const { return option_.temp_dir; }

    // Wal
    [[nodiscard]] inline u64 full_checkpoint_interval_sec() const { return option_.full_checkpoint_interval_sec_; }

    [[nodiscard]] inline u64 full_checkpoint_txn_interval() const { return option_.full_checkpoint_txn_interval_; }

    [[nodiscard]] inline u64 delta_checkpoint_interval_sec() const { return option_.delta_checkpoint_interval_sec_; }

    [[nodiscard]] inline u64 delta_checkpoint_interval_wal_bytes() const { return option_.delta_checkpoint_interval_wal_bytes_; }

    [[nodiscard]] inline u64 wal_size_threshold() const { return option_.wal_size_threshold_; }

private:
    static void ParseTimeZoneStr(const String &time_zone_str, String &parsed_time_zone, i32 &parsed_time_zone_bias);

    static SharedPtr<String> ParseByteSize(const String &byte_size_str, u64 &byte_size);

    static u64 GetAvailableMem();

private:
    Options option_;
};

} // namespace infinity
