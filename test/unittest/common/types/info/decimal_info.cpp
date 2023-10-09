//
// Created by JinHai on 2022/11/29.
//

#include "common/types/info/decimal_info.h"
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include <gtest/gtest.h>

class DecimalInfoTest : public BaseTest {
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

TEST_F(DecimalInfoTest, decimal_info_A) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    EXPECT_THROW(DecimalInfo::Make(39, 39), TypeException);
    EXPECT_THROW(DecimalInfo::Make(38, 39), TypeException);

    auto decimal_info = DecimalInfo::Make(38, 38);
    EXPECT_EQ(decimal_info->scale(), 38);
    EXPECT_EQ(decimal_info->precision(), 38);
    EXPECT_EQ(decimal_info->Size(), 16);

    nlohmann::json json;
    json["type_info"] = decimal_info->Serialize();
    EXPECT_EQ(json.dump(), "{\"type_info\":{\"precision\":38,\"scale\":38}}");
}
