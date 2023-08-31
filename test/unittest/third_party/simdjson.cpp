//
// Created by jinhai on 23-6-2.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "bin/compilation_config.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "storage/common/async_batch_processor.h"
#include "storage/common/async_dummy_task.h"
#include "simdjson.h"

class SimdJsonTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};


TEST_F(SimdJsonTest, test1) {
    using namespace infinity;
    String json_path = String(TEST_DATA_PATH) + "/json/twitter.json";
//    LOG_TRACE("JSON Path: {}", json_path);
    simdjson::ondemand::parser parser;
    simdjson::padded_string json = simdjson::padded_string::load(json_path);
    simdjson::ondemand::document tweets = parser.iterate(json);
    EXPECT_EQ(uint64_t(tweets["search_metadata"]["count"]), 100);
}