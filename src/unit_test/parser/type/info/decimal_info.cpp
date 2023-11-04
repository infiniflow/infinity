//
// Created by JinHai on 2022/11/29.
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

class DecimalInfoTest : public BaseTest {};

TEST_F(DecimalInfoTest, decimal_info_A) {
    using namespace infinity;

    EXPECT_THROW(DecimalInfo::Make(39, 39), ParserException);
    EXPECT_THROW(DecimalInfo::Make(38, 39), ParserException);

    auto decimal_info = DecimalInfo::Make(38, 38);
    EXPECT_EQ(decimal_info->scale(), 38);
    EXPECT_EQ(decimal_info->precision(), 38);
    EXPECT_EQ(decimal_info->Size(), 16);

    nlohmann::json json;
    json["type_info"] = decimal_info->Serialize();
    EXPECT_EQ(json.dump(), "{\"type_info\":{\"precision\":38,\"scale\":38}}");
}
