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
import options;
import status;

namespace infinity {

export constexpr std::string_view profile_history_capacity_name = "profile_history_capacity";
export constexpr std::string_view enable_profiling_name = "enable_profile";
export constexpr std::string_view worker_cpu_limit = "cpu_count";
export constexpr std::string_view log_level = "log_level";

export struct Config {
public:
    Status Init(const SharedPtr<String> &config_path);

    void PrintAll() const;

    // General
    [[nodiscard]] inline String version() const { return system_option_.version; }

    [[nodiscard]] inline String time_zone() const { return system_option_.time_zone; }

    [[nodiscard]] inline i32 time_zone_bias() const { return system_option_.time_zone_bias; }

    inline void set_worker_cpu_number(u64 new_cpu_limit) { system_option_.worker_cpu_limit = new_cpu_limit; }

    [[nodiscard]] inline u64 worker_cpu_limit() const { return system_option_.worker_cpu_limit; }

    [[nodiscard]] inline u64 total_memory_size() const { return system_option_.total_memory_size; }

    [[nodiscard]] inline u64 query_cpu_limit() const { return system_option_.query_cpu_limit; }

    [[nodiscard]] inline u64 query_memory_limit() const { return system_option_.query_memory_limit; }

    // Network
    [[nodiscard]] inline String listen_address() const { return system_option_.listen_address; }

    [[nodiscard]] inline u16 pg_port() const { return system_option_.pg_port; }

    [[nodiscard]] inline u32 http_port() const { return system_option_.http_port; }

    [[nodiscard]] inline u32 sdk_port() const { return system_option_.sdk_port; }

    [[nodiscard]] inline i32 connection_limit() const { return system_option_.connection_limit_; }

    // Profiler
    [[nodiscard]] inline bool enable_profiler() const { return system_option_.enable_profiler; }

    [[nodiscard]] inline SizeT profile_record_capacity() const { return system_option_.profile_record_capacity; }

    // Log
    [[nodiscard]] inline SharedPtr<String> log_filename() const { return system_option_.log_filename; }

    [[nodiscard]] inline SharedPtr<String> log_dir() const { return system_option_.log_dir; }

    [[nodiscard]] inline SharedPtr<String> log_file_path() const { return system_option_.log_file_path; }

    [[nodiscard]] inline bool log_to_stdout() const { return system_option_.log_to_stdout; }

    [[nodiscard]] inline SizeT log_max_size() const { return system_option_.log_max_size; }

    [[nodiscard]] inline SizeT log_file_rotate_count() const { return system_option_.log_file_rotate_count; }

    [[nodiscard]] inline LogLevel log_level() const { return system_option_.log_level; }

    // Storage
    [[nodiscard]] inline SharedPtr<String> data_dir() const { return system_option_.data_dir; }

    [[nodiscard]] inline u64 default_row_size() const { return system_option_.default_row_size; }

    [[nodiscard]] inline u64 storage_capacity() const { return system_option_.storage_capacity_; }

    [[nodiscard]] inline u64 garbage_collection_interval() const { return system_option_.garbage_collection_interval_; }

    [[nodiscard]] inline double garbage_collection_storage_ratio() const { return system_option_.garbage_collection_storage_ratio_; }

    // Buffer
    [[nodiscard]] inline u64 buffer_pool_size() const { return system_option_.buffer_pool_size; }

    [[nodiscard]] inline SharedPtr<String> temp_dir() const { return system_option_.temp_dir; }

    // Wal
    [[nodiscard]] inline SharedPtr<String> wal_dir() const { return system_option_.wal_dir; }

    [[nodiscard]] inline u64 full_checkpoint_interval_sec() const { return system_option_.full_checkpoint_interval_sec_; }

    [[nodiscard]] inline u64 full_checkpoint_txn_interval() const { return system_option_.full_checkpoint_txn_interval_; }

    [[nodiscard]] inline u64 delta_checkpoint_interval_sec() const { return system_option_.delta_checkpoint_interval_sec_; }

    [[nodiscard]] inline u64 delta_checkpoint_interval_wal_bytes() const { return system_option_.delta_checkpoint_interval_wal_bytes_; }

    [[nodiscard]] inline u64 wal_size_threshold() const { return system_option_.wal_size_threshold_; }

    [[nodiscard]] inline FlushOption flush_at_commit() const { return system_option_.flush_at_commit_; }

    // Resource
    [[nodiscard]] inline String resource_dict_path() const { return system_option_.resource_dict_path_; }

    [[nodiscard]] inline u64 cleanup_interval_sec() const { return system_option_.cleanup_interval_sec_; }

    [[nodiscard]] inline bool enable_compaction() const { return system_option_.enable_compaction_; }

private:
    static void ParseTimeZoneStr(const String &time_zone_str, String &parsed_time_zone, i32 &parsed_time_zone_bias);

    static Status ParseByteSize(const String &byte_size_str, u64 &byte_size);

    static Status ParseTimeInfo(const String &time_info, u64 &time_seconds);

    static u64 GetAvailableMem();

private:
    SystemOptions system_option_;
    SessionOptions default_session_options_;
};

export enum class SysVar {
    kQueryCount,
    kSessionCount,
    kBufferPoolUsage,
    kVersion,
    kQueryMemoryLimit,
    kQueryCpuLimit,
    kLogLevel,
    kSchedulePolicy,
    kListenAddress,
    kSQLPort,
    kSDKPort,
    kHttpAPIPort,
    kDataURL,
    kTimezone,
    kLogFlushPolicy,
    kInvalid,
};

export struct SystemVariables {
    static HashMap<String, SysVar> map_;

    static void InitVariablesMap();
    static SysVar GetSysVarEnumByName(const String &var_name);
};

} // namespace infinity
