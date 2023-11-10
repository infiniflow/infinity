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

export module options;

import stl;
import third_party;

namespace infinity {

export const String profile_history_capacity_name = "profile_history_capacity";
export const String enable_profiling_name = "enable_profile";

export struct SessionOptions {
    inline bool enable_profiling() const { return enable_profiling_; }
    inline u64 profile_history_capacity() const { return profile_history_capacity_; }

    bool enable_profiling_{false};      // enable_profile
    u64 profile_history_capacity_{128}; // profile_history_capacity
};

export struct SystemOptions {
    // General
    String version{};
    String time_zone{};
    i32 time_zone_bias{};

    // System
    u64 total_cpu_number{};
    u64 total_memory_size{};
    u64 query_cpu_limit{};
    u64 query_memory_limit{};

    // profiler
    bool enable_profiler{};
    u64 profile_record_capacity{};

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

} // namespace infinity