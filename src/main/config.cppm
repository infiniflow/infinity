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

#include <chrono>

export module infinity_core:config;

import :stl;
import :third_party;
import :status;
import :virtual_store;
import :options;

namespace infinity {

using namespace std::chrono;
export constexpr std::string_view profile_history_capacity_name = "profile_history_capacity";
export constexpr std::string_view enable_profile_name = "enable_profile";
export constexpr std::string_view worker_cpu_limit = "cpu_count";
export constexpr std::string_view log_level = "log_level";

export struct DefaultConfig {
    LogLevel default_log_level_{LogLevel::kInfo};
    bool default_log_to_stdout_{false};
    String default_log_dir_ = "/var/infinity/log";
    String default_catalog_dir_ = "/var/infinity/catalog";
    String default_snapshot_dir_ = "/var/infinity/snapshot";
    String default_data_dir_ = "/var/infinity/data";
    String default_wal_dir_ = "/var/infinity/wal";
    String default_temp_dir_ = "/var/infinity/tmp";
    String default_resource_dir_ = "/var/infinity/resource";
    String default_persistence_dir_ = "/var/infinity/persistence";
};

export struct Config {
public:
    Config();
    ~Config();

    Status Init(const SharedPtr<String> &config_path, DefaultConfig *default_config);

    void PrintAll();

    // General
    String Version();
    String ServerMode();
    String TimeZone();
    i64 TimeZoneBias();

    void SetCPULimit(i64 new_cpu_limit);
    i64 CPULimit();
    inline bool RecordRunningQuery() { return record_running_query_; }
    void SetRecordRunningQuery(bool flag);

    // Network
    String ServerAddress();
    String PeerServerIP();
    i64 PeerServerPort();
    i64 PostgresPort();
    i64 HTTPPort();
    i64 ClientPort();
    i64 ConnectionPoolSize();
    i64 PeerServerConnectionPoolSize();

    i64 PeerRetryDelay();
    i64 PeerRetryCount();
    i64 PeerConnectTimeout();
    i64 PeerRecvTimeout();
    i64 PeerSendTimeout();

    // Log
    String LogFileName();
    String LogDir();
    String LogFilePath();

    void SetLogToStdout(bool log_to_stdout);
    bool LogToStdout();

    i64 LogFileMaxSize();
    i64 LogFileRotateCount();

    void SetLogLevel(LogLevel level);
    LogLevel GetLogLevel();

    // Storage
    bool ReplayWal();
    String DataDir();
    String CatalogDir();
    String SnapshotDir();

    i64 CleanupInterval();
    void SetCleanupInterval(i64);

    i64 CompactInterval();
    void SetCompactInterval(i64);

    i64 OptimizeIndexInterval();
    void SetOptimizeInterval(i64);

    i64 MemIndexCapacity();
    i64 DenseIndexBuildingWorker();
    i64 SparseIndexBuildingWorker();
    i64 FulltextIndexBuildingWorker();
    i64 BottomExecutorWorker();

    StorageType StorageType();
    String ObjectStorageUrl();
    String ObjectStorageBucket();
    String ObjectStorageAccessKey();
    String ObjectStorageSecretKey();
    bool ObjectStorageHttps();

    // Persistence
    String PersistenceDir();
    i64 PersistenceObjectSizeLimit();
    bool UseVFS();

    // Buffer
    i64 BufferManagerSize();
    SizeT LRUNum();
    String TempDir();

    i64 MemIndexMemoryQuota();

    String ResultCache();
    i64 CacheResultNum();
    void SetCacheResult(const String &mode);

    // WAL
    String WALDir();

    i64 WALCompactThreshold();

    i64 CheckpointInterval();
    void SetFullCheckpointInterval(i64);

    i64 DeltaCheckpointInterval();
    void SetDeltaCheckpointInterval(i64);

    i64 DeltaCheckpointThreshold();

    FlushOptionType FlushMethodAtCommit();

    // Resource
    String ResourcePath();

    // Date and Time

    void SetTimeZone(const String &value);

    void SetTimeZoneBias(i64);

public:
    // Get config by name
    Tuple<BaseOption *, Status> GetConfigByName(const String &name);

    GlobalOptionIndex GetOptionIndex(const String &var_name) const { return global_options_.GetOptionIndex(var_name); }

    void SetOption(const String &var_name, const String &value);

    static void SetTimeZoneOuter(const String &value);

    static void ParseTimeZoneStr(const String &time_zone_str, String &parsed_time_zone, i32 &parsed_time_zone_bias);

private:
    // static void ParseTimeZoneStr(const String &time_zone_str);

    static Status ParseByteSize(const String &byte_size_str, i64 &byte_size);

    static Status ParseTimeInfo(const String &time_info, i64 &time_seconds);

    static u64 GetAvailableMem();

private:
    mutable std::mutex mutex_;
    GlobalOptions global_options_;
    // record running query flag
    Atomic<bool> record_running_query_{false};

public:
    hours hour_offset_{};
};
} // namespace infinity
