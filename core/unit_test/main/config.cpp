//
// Created by jinhai on 23-8-31.
//

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

    EXPECT_EQ(config.total_cpu_number(), std::thread::hardware_concurrency());
    EXPECT_EQ(config.total_memory_size(), GetAvailableMem());
    EXPECT_EQ(config.query_cpu_limit(), std::thread::hardware_concurrency());
    EXPECT_EQ(config.query_memory_limit(), GetAvailableMem());

    // Network
    EXPECT_EQ(config.listen_address(), "0.0.0.0");
    EXPECT_EQ(config.pg_port(), 5432);
    EXPECT_EQ(config.http_port(), 8088);
    EXPECT_EQ(config.sdk_port(), 23817);

    // Log
    EXPECT_EQ(*config.log_filename(), "infinity.log");
    EXPECT_EQ(*config.log_dir(), "/tmp/infinity/log");
    EXPECT_EQ(*config.log_file_path(), "/tmp/infinity/log/infinity.log");
    EXPECT_EQ(config.log_to_stdout(), false);
    EXPECT_EQ(config.log_max_size(), 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(config.log_file_rotate_count(), 10);
    EXPECT_EQ(config.log_level(), LogLevel::kTrace);

    EXPECT_EQ(*config.data_dir(), "/tmp/infinity/data");
    EXPECT_EQ(*config.wal_dir(), "/tmp/infinity/wal");
    EXPECT_EQ(config.default_row_size(), 8192u);

    EXPECT_EQ(config.buffer_pool_size(), 4 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(*config.temp_dir(), "/tmp/infinity/temp");
}

TEST_F(ConfigTest, test2) {
    using namespace infinity;
    SharedPtr<String> path = MakeShared<String>(String(test_data_path()) + "/config/infinity_conf.toml");
    Config config;
    config.Init(path);

    EXPECT_EQ(config.version(), "0.1.0");
    EXPECT_EQ(config.time_zone(), "UTC");
    EXPECT_EQ(config.time_zone_bias(), -9);

    EXPECT_EQ(config.total_cpu_number(), 2);
    EXPECT_EQ(config.total_memory_size(), 8 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(config.query_cpu_limit(), 2);
    EXPECT_EQ(config.query_memory_limit(), 4 * 1024ul * 1024ul);

    EXPECT_EQ(config.listen_address(), "127.0.0.1");
    EXPECT_EQ(config.pg_port(), 25432);
    EXPECT_EQ(config.http_port(), 8089);
    EXPECT_EQ(config.sdk_port(), 24817);

    EXPECT_EQ(*config.log_filename(), "info.log");
    EXPECT_EQ(*config.log_dir(), "/var/infinity/log");
    EXPECT_EQ(*config.log_file_path(), "/var/infinity/log/info.log");
    EXPECT_EQ(config.log_to_stdout(), true);
    EXPECT_EQ(config.log_max_size(), 2 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(config.log_file_rotate_count(), 3);
    EXPECT_EQ(config.log_level(), LogLevel::kTrace);

    EXPECT_EQ(*config.data_dir(), "/var/infinity/data");
    EXPECT_EQ(*config.wal_dir(), "/var/infinity/wal");
    EXPECT_EQ(config.default_row_size(), 4096u);

    EXPECT_EQ(config.buffer_pool_size(), 3 * 1024ul * 1024ul * 1024ul);
    EXPECT_EQ(*config.temp_dir(), "/tmp");
}
