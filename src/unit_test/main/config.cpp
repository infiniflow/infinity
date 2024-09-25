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

#include <gtest/gtest.h>
import base_test;

import stl;
import config;
import infinity_exception;
import third_party;
import status;
import compilation_config;

using namespace infinity;
class ConfigTest : public BaseTest {};

//static size_t GetAvailableMem() {
//    size_t pages = sysconf(_SC_PHYS_PAGES);
//    size_t page_size = sysconf(_SC_PAGE_SIZE); // Byte
//    return pages * page_size;
//}

TEST_F(ConfigTest, test1) {
    using namespace infinity;
    SharedPtr<String> path = nullptr;
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(config.Version(), "0.4.0");
    EXPECT_EQ(config.TimeZone(), "UTC");
    EXPECT_EQ(config.TimeZoneBias(), 8);
    EXPECT_EQ(config.CPULimit(), std::thread::hardware_concurrency());

    // Network
    EXPECT_EQ(config.ServerAddress(), "0.0.0.0");
    EXPECT_EQ(config.PostgresPort(), 5432);
    EXPECT_EQ(config.HTTPPort(), 23820u);
    EXPECT_EQ(config.ClientPort(), 23817u);

    // Log
    EXPECT_EQ(config.LogFileName(), "infinity.log");
    EXPECT_EQ(config.LogDir(), "/var/infinity/log");
    EXPECT_EQ(config.LogFilePath(), "/var/infinity/log/infinity.log");
    EXPECT_EQ(config.LogToStdout(), false);
    EXPECT_EQ(config.LogFileMaxSize(), 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LogFileRotateCount(), 8l);
    EXPECT_EQ(config.GetLogLevel(), LogLevel::kInfo);

    // Storage
    EXPECT_EQ(config.DataDir(), "/var/infinity/data");
    EXPECT_EQ(config.WALDir(), "/var/infinity/wal");
    EXPECT_EQ(config.FileSystemType(), "local");

    // buffer
    EXPECT_EQ(config.BufferManagerSize(), 8 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LRUNum(), 7);
    EXPECT_EQ(config.TempDir(), "/var/infinity/tmp");
    EXPECT_EQ(config.MemIndexMemoryQuota(), 4 * 1024l * 1024l * 1024l);
}

TEST_F(ConfigTest, test2) {
    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/infinity_conf.toml");
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(config.Version(), "0.4.0");
    EXPECT_EQ(config.TimeZone(), "UTC");
    EXPECT_EQ(config.TimeZoneBias(), -8);

    EXPECT_EQ(config.CPULimit(), 2u);

    EXPECT_EQ(config.ServerAddress(), "127.0.0.1");
    EXPECT_EQ(config.PostgresPort(), 25432);
    EXPECT_EQ(config.HTTPPort(), 24821);
    EXPECT_EQ(config.ClientPort(), 24817);

    EXPECT_EQ(config.LogFileName(), "infinity.log");
    EXPECT_EQ(config.LogDir(), "/var/infinity/log");
    EXPECT_EQ(config.LogFilePath(), "/var/infinity/log/infinity.log");
    EXPECT_EQ(config.LogToStdout(), false);
    EXPECT_EQ(config.LogFileMaxSize(), 2 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LogFileRotateCount(), 3l);
    EXPECT_EQ(config.GetLogLevel(), LogLevel::kTrace);

    // Storage
    EXPECT_EQ(config.DataDir(), "/var/infinity/data");
    EXPECT_EQ(config.WALDir(), "/var/infinity/wal");
    EXPECT_EQ(config.FileSystemType(), "minio");
    EXPECT_EQ(config.ObjectStorageHost(), "0.0.0.0");
    EXPECT_EQ(config.ObjectStoragePort(), 9000);
    EXPECT_EQ(config.ObjectStorageBucket(), "infinity");

    // buffer
    EXPECT_EQ(config.BufferManagerSize(), 3 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LRUNum(), 8);
    EXPECT_EQ(config.TempDir(), "/var/infinity/tmp");
    EXPECT_EQ(config.MemIndexMemoryQuota(), 2 * 1024l * 1024l * 1024l);
}

TEST_F(ConfigTest, TestWrongParamNames) {
    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_invalid_param.toml");
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_FALSE(status.ok()); 
}

TEST_F(ConfigTest, TestConfInvalidValues) {
    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_invalid_version.toml");
    Config config_invalid_version;
    auto status = config_invalid_version.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kMismatchVersion);

    Config config_invalid_timezone;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_invalid_timezone.toml");
    status = config_invalid_timezone.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidTimezone);

    Config config_invalid_server_address;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_invalid_server_address.toml");
    status = config_invalid_server_address.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidIPAddr);

    Config config_invalid_bytesize;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_invalid_bytesize.toml");
    status = config_invalid_bytesize.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidByteSize);

    Config config_invalid_log_level;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_invalid_log_level.toml");
    status = config_invalid_log_level.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidLogLevel);

    Config config_invalid_timeinfo;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_invalid_timeinfo.toml");
    status = config_invalid_timeinfo.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidTimeInfo);
}

TEST_F(ConfigTest, TestOutofRangeValues) {

    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_out_of_bound_bytesize.toml");
    Config config_bytesize;
    auto status = config_bytesize.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);

    Config config_number;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_out_of_bound_number.toml");
    status = config_number.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);
    
    Config config_timeinfo;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_out_of_bound_timeinfo.toml");
    status = config_timeinfo.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);

    Config config_timezone;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_out_of_bound_timezone.toml");
    status = config_timezone.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);

    Config config_walflush;
    path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_out_of_bound_walflush.toml");
    status = config_walflush.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);
   
}

TEST_F(ConfigTest, TestValidValues) {
    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/test_conf_valid_value.toml");
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(config.Version(), "0.4.0");
    EXPECT_EQ(config.TimeZone(), "UTC");
    EXPECT_EQ(config.TimeZoneBias(), -8);
    EXPECT_EQ(config.CPULimit(), 2);

    // Network
    EXPECT_EQ(config.ServerAddress(), "0.0.0.0");
    EXPECT_EQ(config.PostgresPort(), 5432);
    EXPECT_EQ(config.HTTPPort(), 23820u);
    EXPECT_EQ(config.ClientPort(), 23817u);
    EXPECT_EQ(config.ConnectionPoolSize(), 128);

    // Log
    EXPECT_EQ(config.LogFileName(), "infinity.log");
    EXPECT_EQ(config.LogDir(), "/var/infinity/log");
    EXPECT_EQ(config.LogFilePath(), "/var/infinity/log/infinity.log");
    EXPECT_EQ(config.LogToStdout(), false);
    EXPECT_EQ(config.LogFileMaxSize(), 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LogFileRotateCount(), 10l);
    EXPECT_EQ(config.GetLogLevel(), LogLevel::kInfo);

    //storage
    EXPECT_EQ(config.DataDir(), "/var/infinity/data");
    EXPECT_EQ(config.OptimizeIndexInterval(), 10);
    EXPECT_EQ(config.CleanupInterval(), 60);
    EXPECT_EQ(config.CompactInterval(), 120);
    EXPECT_EQ(config.MemIndexCapacity(), 1048576);

    // buffer
    EXPECT_EQ(config.BufferManagerSize(), 4 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LRUNum(), 7);
    EXPECT_EQ(config.TempDir(), "/var/infinity/tmp");
    EXPECT_EQ(config.MemIndexMemoryQuota(), 1024l * 1024l * 1024l);

    //wal
    EXPECT_EQ(config.WALDir(), "/var/infinity/wal"); 
    EXPECT_EQ(config.FullCheckpointInterval(), 86400l);
    EXPECT_EQ(config.DeltaCheckpointInterval(), 60l);
    EXPECT_EQ(config.WALCompactThreshold(), 1024l * 1024l * 1024l);
    //resource
    EXPECT_EQ(config.ResourcePath(), "/var/infinity/resource");
    //persistence
    EXPECT_EQ(config.PersistenceDir(), "/var/infinity/persistence");
}