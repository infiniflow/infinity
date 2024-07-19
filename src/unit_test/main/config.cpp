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

#include "unit_test/base_test.h"

import stl;
import config;
import infinity_exception;
import third_party;
import compilation_config;

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
    config.Init(path, nullptr);

    EXPECT_EQ(config.Version(), "0.2.1");
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

    EXPECT_EQ(config.DataDir(), "/var/infinity/data");
    EXPECT_EQ(config.WALDir(), "/var/infinity/wal");

    EXPECT_EQ(config.BufferManagerSize(), 8 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.TempDir(), "/var/infinity/tmp");
}

TEST_F(ConfigTest, test2) {
    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/infinity_conf.toml");
    Config config;
    config.Init(path, nullptr);

    EXPECT_EQ(config.Version(), "0.2.1");
    EXPECT_EQ(config.TimeZone(), "UTC");
    EXPECT_EQ(config.TimeZoneBias(), -9);

    EXPECT_EQ(config.CPULimit(), 2u);

    EXPECT_EQ(config.ServerAddress(), "127.0.0.1");
    EXPECT_EQ(config.PostgresPort(), 25432);
    EXPECT_EQ(config.HTTPPort(), 24821);
    EXPECT_EQ(config.ClientPort(), 24817);

    EXPECT_EQ(config.LogFileName(), "info.log");
    EXPECT_EQ(config.LogDir(), "/var/infinity/log");
    EXPECT_EQ(config.LogFilePath(), "/var/infinity/log/info.log");
    EXPECT_EQ(config.LogToStdout(), true);
    EXPECT_EQ(config.LogFileMaxSize(), 2 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.LogFileRotateCount(), 3l);
    EXPECT_EQ(config.GetLogLevel(), LogLevel::kTrace);

    EXPECT_EQ(config.DataDir(), "/var/infinity/data");
    EXPECT_EQ(config.WALDir(), "/var/infinity/wal");

    EXPECT_EQ(config.BufferManagerSize(), 3 * 1024l * 1024l * 1024l);
    EXPECT_EQ(config.TempDir(), "/tmp");
}
