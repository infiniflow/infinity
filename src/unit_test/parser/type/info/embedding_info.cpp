//
// Created by JinHai on 2022/11/27.
//

#include "unit_test/base_test.h"
#include "json.hpp"

import infinity_exception;

import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity_context;

class EmbeddingInfoTest : public BaseTest {};

TEST_F(EmbeddingInfoTest, embedding_info_A) {
    using namespace infinity;

    EXPECT_THROW(EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, EMBEDDING_LIMIT + 1), ParserException);

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
    std::cout << json.dump() << std::endl;
    //    EXPECT_EQ(json.dump(), "{\"type_info\":{\"precision\":38,\"scale\":38}}");
}
