//
// Created by JinHai on 2022/11/27.
//

#include "common/types/info/embedding_info.h"
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include <gtest/gtest.h>

class EmbeddingInfoTest : public BaseTest {
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

TEST_F(EmbeddingInfoTest, embedding_info_A) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    EXPECT_THROW(EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, EMBEDDING_LIMIT + 1), TypeException);

    auto embedding_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemBit, 256);
    EXPECT_EQ(embedding_info_ptr->Size(), 32);
    EXPECT_EQ(embedding_info_ptr->Dimension(), 256);
    EXPECT_EQ(embedding_info_ptr->Type(), EmbeddingDataType::kElemBit);

    embedding_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemInt64, EMBEDDING_LIMIT);
    EXPECT_EQ(embedding_info_ptr->Size(), EMBEDDING_LIMIT * 8);
    EXPECT_EQ(embedding_info_ptr->Dimension(), EMBEDDING_LIMIT);
    EXPECT_EQ(embedding_info_ptr->Type(), EmbeddingDataType::kElemInt64);

    nlohmann::json json;
    json["type_info"] = embedding_info_ptr->Serialize();
    LOG_TRACE("{}", json.dump());
    //    EXPECT_EQ(json.dump(), "{\"type_info\":{\"precision\":38,\"scale\":38}}");
}