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

#include "unit_test/gtest_expand.h"

module infinity_core:ut.config;

import :ut.base_test;
import :config;
import :infinity_exception;
import third_party;
import :status;
import :virtual_store;
import :default_values;

import compilation_config;

using namespace infinity;
class ConfigTest : public BaseTest {};

TEST_F(ConfigTest, test1) {
    using namespace infinity;
    std::shared_ptr<std::string> path = nullptr;
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(config.Version(), "0.7.5");
    EXPECT_EQ(config.TimeZone(), "UTC");
    EXPECT_EQ(config.TimeZoneBias(), 8);
    EXPECT_EQ(config.CPULimit(), std::thread::hardware_concurrency());

    // Network
    EXPECT_EQ(config.ServerAddress(), "0.0.0.0");
    EXPECT_EQ(config.PostgresPort(), 5432);
    EXPECT_EQ(config.HTTPPort(), 23820u);
    EXPECT_EQ(config.ClientPort(), 23817u);

    EXPECT_EQ(config.PeerRetryDelay(), DEFAULT_PEER_RETRY_DELAY);
    EXPECT_EQ(config.PeerRetryCount(), DEFAULT_PEER_RETRY_COUNT);
    EXPECT_EQ(config.PeerConnectTimeout(), DEFAULT_PEER_CONNECT_TIMEOUT);
    EXPECT_EQ(config.PeerRecvTimeout(), DEFAULT_PEER_RECV_TIMEOUT);
    EXPECT_EQ(config.PeerSendTimeout(), DEFAULT_PEER_SEND_TIMEOUT);

    // Log
    EXPECT_EQ(config.LogFileName(), "infinity.log");
    EXPECT_EQ(config.LogDir(), "/var/infinity/log");
    EXPECT_EQ(config.LogFilePath(), "/var/infinity/log/infinity.log");
    EXPECT_EQ(config.LogToStdout(), true);
    EXPECT_EQ(config.LogFileMaxSize(), 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LogFileRotateCount(), 8l);
    EXPECT_EQ(config.GetLogLevel(), LogLevel::kInfo);

    // Storage
    EXPECT_EQ(config.DataDir(), "/var/infinity/data");
    EXPECT_EQ(config.WALDir(), "/var/infinity/wal");
    EXPECT_EQ(config.StorageType(), StorageType::kLocal);

    // buffer
    EXPECT_EQ(config.BufferManagerSize(), 8 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LRUNum(), 7);
    EXPECT_EQ(config.TempDir(), "/var/infinity/tmp");
    EXPECT_EQ(config.MemIndexMemoryQuota(), 4 * 1024l * 1024l * 1024l);

    EXPECT_EQ(config.ResultCache(), "off");
    EXPECT_EQ(config.CacheResultNum(), 10000);
}

TEST_F(ConfigTest, test2) {
    using namespace infinity;
    std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/infinity_conf.toml");
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(config.Version(), "0.7.5");
    EXPECT_EQ(config.TimeZone(), "UTC");
    EXPECT_EQ(config.TimeZoneBias(), -8);

    EXPECT_EQ(config.CPULimit(), 2u);

    EXPECT_EQ(config.ServerAddress(), "127.0.0.1");
    EXPECT_EQ(config.PostgresPort(), 25432);
    EXPECT_EQ(config.HTTPPort(), 24821);
    EXPECT_EQ(config.ClientPort(), 24817);

    EXPECT_EQ(config.PeerRetryDelay(), 1000);
    EXPECT_EQ(config.PeerRetryCount(), 2);
    EXPECT_EQ(config.PeerConnectTimeout(), 2000);
    EXPECT_EQ(config.PeerRecvTimeout(), 2000);
    EXPECT_EQ(config.PeerSendTimeout(), 2000);

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
    EXPECT_EQ(config.StorageType(), StorageType::kLocal);
    EXPECT_EQ(config.ObjectStorageUrl(), "0.0.0.0:9000");
    EXPECT_EQ(config.ObjectStorageBucket(), "infinity");
    EXPECT_EQ(config.ObjectStorageAccessKey(), "minioadmin");
    EXPECT_EQ(config.ObjectStorageSecretKey(), "minioadmin");
    EXPECT_EQ(config.ObjectStorageHttps(), false);

    // buffer
    EXPECT_EQ(config.BufferManagerSize(), 3 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LRUNum(), 8);
    EXPECT_EQ(config.TempDir(), "/var/infinity/tmp");
    EXPECT_EQ(config.MemIndexMemoryQuota(), 2 * 1024l * 1024l * 1024l);

    EXPECT_EQ(config.ResultCache(), "off");
    // EXPECT_EQ(config.CacheResultNum(), 100);
}

TEST_F(ConfigTest, TestWrongParamNames) {
    using namespace infinity;
    std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_invalid_param.toml");
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_FALSE(status.ok());
}

TEST_F(ConfigTest, TestConfInvalidValues) {
    using namespace infinity;
    std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_invalid_version.toml");
    Config config_invalid_version;
    auto status = config_invalid_version.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kMismatchVersion);

    Config config_invalid_timezone;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_invalid_timezone.toml");
    status = config_invalid_timezone.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidTimezone);

    Config config_invalid_server_address;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_invalid_server_address.toml");
    status = config_invalid_server_address.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidIPAddr);

    Config config_invalid_bytesize;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_invalid_bytesize.toml");
    status = config_invalid_bytesize.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidByteSize);

    Config config_invalid_log_level;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_invalid_log_level.toml");
    status = config_invalid_log_level.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidLogLevel);

    Config config_invalid_timeinfo;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_invalid_timeinfo.toml");
    status = config_invalid_timeinfo.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidTimeInfo);
}

TEST_F(ConfigTest, TestOutofRangeValues) {

    using namespace infinity;
    std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_out_of_bound_bytesize.toml");
    Config config_bytesize;
    auto status = config_bytesize.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);

    Config config_number;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_out_of_bound_number.toml");
    status = config_number.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);

    Config config_timeinfo;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_out_of_bound_timeinfo.toml");
    status = config_timeinfo.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);

    Config config_timezone;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_out_of_bound_timezone.toml");
    status = config_timezone.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);

    Config config_walflush;
    path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_out_of_bound_walflush.toml");
    status = config_walflush.Init(path, nullptr);
    EXPECT_EQ(status.code(), ErrorCode::kInvalidConfig);
}

TEST_F(ConfigTest, TestValidValues) {
    using namespace infinity;
    std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_conf_valid_value.toml");
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(config.Version(), "0.7.5");
    EXPECT_EQ(config.TimeZone(), "UTC");
    EXPECT_EQ(config.TimeZoneBias(), -8);
    EXPECT_EQ(config.CPULimit(), 2);

    // Network
    EXPECT_EQ(config.ServerAddress(), "0.0.0.0");
    EXPECT_EQ(config.PostgresPort(), 5432);
    EXPECT_EQ(config.HTTPPort(), 23820u);
    EXPECT_EQ(config.ClientPort(), 23817u);
    EXPECT_EQ(config.ConnectionPoolSize(), 128);

    EXPECT_EQ(config.PeerRetryDelay(), 100);
    EXPECT_EQ(config.PeerRetryCount(), 1);
    EXPECT_EQ(config.PeerConnectTimeout(), 200);
    EXPECT_EQ(config.PeerRecvTimeout(), 200);
    EXPECT_EQ(config.PeerSendTimeout(), 200);

    // Log
    EXPECT_EQ(config.LogFileName(), "infinity.log");
    EXPECT_EQ(config.LogDir(), "/var/infinity/log");
    EXPECT_EQ(config.LogFilePath(), "/var/infinity/log/infinity.log");
    EXPECT_EQ(config.LogToStdout(), false);
    EXPECT_EQ(config.LogFileMaxSize(), 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LogFileRotateCount(), 10l);
    EXPECT_EQ(config.GetLogLevel(), LogLevel::kInfo);

    // storage
    EXPECT_EQ(config.DataDir(), "/var/infinity/data");
    EXPECT_EQ(config.OptimizeIndexInterval(), 10);
    EXPECT_EQ(config.CleanupInterval(), 60);
    EXPECT_EQ(config.CompactInterval(), 120);
    EXPECT_EQ(config.MemIndexCapacity(), 1048576);
    EXPECT_EQ(config.SnapshotDir(), "/var/infinity/snapshot");

    // buffer
    EXPECT_EQ(config.BufferManagerSize(), 4 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LRUNum(), 7);
    EXPECT_EQ(config.TempDir(), "/var/infinity/tmp");
    EXPECT_EQ(config.MemIndexMemoryQuota(), 1024l * 1024l * 1024l);

    // wal
    EXPECT_EQ(config.WALDir(), "/var/infinity/wal");
    EXPECT_EQ(config.CheckpointInterval(), 86400l);
    EXPECT_EQ(config.WALCompactThreshold(), 1024l * 1024l * 1024l);
    // resource
    EXPECT_EQ(config.ResourcePath(), "/usr/share/infinity/resource");
    // persistence
    EXPECT_EQ(config.PersistenceDir(), "/var/infinity/persistence");
}

TEST_F(ConfigTest, TestDefaultConfWithRustUnsealedOptions) {
    using namespace infinity;
    std::shared_ptr<std::string> path = std::make_shared<std::string>("conf/infinity_conf.toml");
    Config config;
    auto status = config.Init(path, nullptr);
    ASSERT_TRUE(status.ok()) << status.message();
    EXPECT_EQ(config.MemIndexMemoryQuota(), 1024l * 1024l * 1024l);
}

TEST_F(ConfigTest, TestUnsealedMemoryQuotaZeroAndDuplicate) {
    using namespace infinity;
    std::ifstream base_in("conf/infinity_conf.toml");
    ASSERT_TRUE(base_in.is_open());
    std::string base_content((std::istreambuf_iterator<char>(base_in)), std::istreambuf_iterator<char>());

    const std::string zero_path = "/tmp/test_unsealed_zero.toml";
    {
        std::string zero_content = base_content;
        auto pos = zero_content.find("unsealed_memory_quota    = \"1GB\"");
        ASSERT_NE(pos, std::string::npos);
        zero_content.replace(pos, std::string("unsealed_memory_quota    = \"1GB\"").size(), "unsealed_memory_quota = 0");
        std::ofstream out(zero_path);
        out << zero_content;
    }
    Config zero_cfg;
    auto zero_status = zero_cfg.Init(std::make_shared<std::string>(zero_path), nullptr);
    ASSERT_TRUE(zero_status.ok()) << zero_status.message();
    EXPECT_EQ(zero_cfg.MemIndexMemoryQuota(), 0);
    std::remove(zero_path.c_str());

    const std::string dup_path = "/tmp/test_unsealed_dup.toml";
    {
        std::string dup_content = base_content;
        auto pos = dup_content.find("unsealed_memory_quota    = \"1GB\"");
        ASSERT_NE(pos, std::string::npos);
        dup_content.insert(pos, "memindex_memory_quota = \"4GB\"\n");
        std::ofstream out(dup_path);
        out << dup_content;
    }
    Config dup_cfg;
    auto dup_status = dup_cfg.Init(std::make_shared<std::string>(dup_path), nullptr);
    EXPECT_EQ(dup_status.code(), ErrorCode::kInvalidConfig);
    std::remove(dup_path.c_str());
}
