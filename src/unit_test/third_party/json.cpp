//
// Created by jinhai on 23-8-21.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import compilation_config;
import infinity_context;

class JsonTest : public BaseTest {
    void SetUp() override {
        system("rm -rf /tmp/infinity/log /tmp/infinity/data /tmp/infinity/wal");
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(JsonTest, test1) {
    using namespace infinity;


    String json_path = String(test_data_path()) + "/json/twitter.json";
    std::ifstream f(json_path);

    Json data = Json::parse(f);
    EXPECT_EQ(data["search_metadata"]["count"], 100);
}
