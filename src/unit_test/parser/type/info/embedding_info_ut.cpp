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

#ifdef CI
#include "parser/default_value.h"
#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "parser/default_value.h"
#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"

module infinity_core:ut.embedding_info;

import :ut.base_test;
import :infinity_exception;
import :third_party;
import :logger;
import :stl;
import :infinity_context;
#endif

import global_resource_usage;
import internal_types;
import embedding_info;
import knn_expr;
import parser_assert;
import parser_defaults;

using namespace infinity;
class EmbeddingInfoTest : public BaseTest {};

TEST_F(EmbeddingInfoTest, embedding_info_A) {
    using namespace infinity;

    EXPECT_THROW_WITHOUT_STACKTRACE(EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, PARSER_EMBEDDING_LIMIT_INTERNAL + 1), ParserException);

    auto embedding_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemBit, 256);
    EXPECT_EQ(embedding_info_ptr->Size(), 32u);
    EXPECT_EQ(embedding_info_ptr->Dimension(), 256u);
    EXPECT_EQ(embedding_info_ptr->Type(), EmbeddingDataType::kElemBit);

    embedding_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemInt64, PARSER_EMBEDDING_LIMIT_INTERNAL);
    EXPECT_EQ(embedding_info_ptr->Size(), SizeT(PARSER_EMBEDDING_LIMIT_INTERNAL * 8));
    EXPECT_EQ(embedding_info_ptr->Dimension(), (u64)PARSER_EMBEDDING_LIMIT_INTERNAL);
    EXPECT_EQ(embedding_info_ptr->Type(), EmbeddingDataType::kElemInt64);

    nlohmann::json json;
    json["type_info"] = embedding_info_ptr->Serialize();
    //    std::cout << json.dump() << std::endl;
    //    EXPECT_EQ(json.dump(), "{\"type_info\":{\"precision\":38,\"scale\":38}}");
}
