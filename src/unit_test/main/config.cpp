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

static size_t GetAvailableMem() {
    size_t pages = sysconf(_SC_PHYS_PAGES);
    size_t page_size = sysconf(_SC_PAGE_SIZE); // Byte
    return pages * page_size;
}

TEST_F(ConfigTest, test1) {
    using namespace infinity;
    SharedPtr<String> path = nullptr;
    Config config;
    config.Init(path);

    EXPECT_EQ(config.version(), "0.1.0");
    EXPECT_EQ(config.time_zone(), "UTC");
    EXPECT_EQ(config.time_zone_bias(), 8);

    EXPECT_EQ(config.worker_cpu_limit(), std::thread::hardware_concurrency());
    EXPECT_EQ(config.total_memory_size(), GetAvailableMem());
    EXPECT_EQ(config.query_cpu_limit(), std::thread::hardware_concurrency());
    EXPECT_EQ(config.query_memory_limit(), GetAvailableMem());

    // Network
    EXPECT_EQ(config.listen_address(), "0.0.0.0");
    EXPECT_EQ(config.pg_port(), 5432);
    EXPECT_EQ(config.http_port(), 23820u);
    EXPECT_EQ(config.sdk_port(), 23817u);

    // Log
    EXPECT_EQ(*config.log_filename(), "infinity.log");
    EXPECT_EQ(*config.log_dir(), "/var/infinity/log");
    EXPECT_EQ(*config.log_file_path(), "/var/infinity/log/infinity.log");
    EXPECT_EQ(config.log_to_stdout(), false);
    EXPECT_EQ(config.log_max_size(), 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(config.log_file_rotate_count(), 10ul);
    // EXPECT_EQ(config.log_level(), LogLevel::kTrace);

    EXPECT_EQ(*config.data_dir(), "/var/infinity/data");
    EXPECT_EQ(*config.wal_dir(), "/var/infinity/wal");
    EXPECT_EQ(config.default_row_size(), 8192u);

    EXPECT_EQ(config.buffer_pool_size(), 4 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(*config.temp_dir(), "/var/infinity/temp");
}

TEST_F(ConfigTest, test2) {
    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/infinity_conf.toml");
    Config config;
    config.Init(path);

    EXPECT_EQ(config.version(), "0.1.0");
    EXPECT_EQ(config.time_zone(), "UTC");
    EXPECT_EQ(config.time_zone_bias(), -9);

    EXPECT_EQ(config.worker_cpu_limit(), 2u);
    EXPECT_EQ(config.total_memory_size(), 8 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(config.query_cpu_limit(), 2ul);
    EXPECT_EQ(config.query_memory_limit(), 4 * 1024ul * 1024ul);

    EXPECT_EQ(config.listen_address(), "127.0.0.1");
    EXPECT_EQ(config.pg_port(), 25432);
    EXPECT_EQ(config.http_port(), 24821u);
    EXPECT_EQ(config.sdk_port(), 24817u);

    EXPECT_EQ(*config.log_filename(), "info.log");
    EXPECT_EQ(*config.log_dir(), "/var/infinity/log");
    EXPECT_EQ(*config.log_file_path(), "/var/infinity/log/info.log");
    EXPECT_EQ(config.log_to_stdout(), true);
    EXPECT_EQ(config.log_max_size(), 2 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(config.log_file_rotate_count(), 3ul);
    EXPECT_EQ(config.log_level(), LogLevel::kTrace);

    EXPECT_EQ(*config.data_dir(), "/var/infinity/data");
    EXPECT_EQ(*config.wal_dir(), "/var/infinity/wal");
    EXPECT_EQ(config.default_row_size(), 4096u);

    EXPECT_EQ(config.buffer_pool_size(), 3 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(*config.temp_dir(), "/tmp");
}
