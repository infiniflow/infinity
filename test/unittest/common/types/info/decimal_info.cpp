//
// Created by JinHai on 2022/11/29.
//

#include "base_test.h"
#include "main/infinity.h"
#include "common/types/info/decimal_info.h"

class DecimalInfoTest : public BaseTest {};

TEST_F(DecimalInfoTest, decimal_info_A) {
    using namespace infinity;

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
