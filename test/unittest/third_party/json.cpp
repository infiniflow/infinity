//
// Created by jinhai on 23-8-21.
//

#include "base_test.h"
#include "json.hpp"
#include "bin/compilation_config.h"
#include "main/infinity.h"

class JsonTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(JsonTest, test1) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    using namespace nlohmann;

    String json_path = String(TEST_DATA_PATH) + "/json/twitter.json";
    LOG_TRACE("JSON Path: {}", json_path);
    std::ifstream f(json_path);

    json data = json::parse(f);
    EXPECT_EQ(data["search_metadata"]["count"], 100);
}
