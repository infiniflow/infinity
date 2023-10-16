//
// Created by jinhai on 23-1-5.
//

#include "base_test.h"
#include "main/infinity.h"
#include "function/cast/cast_table.h"

class CastTableTest : public BaseTest {};

TEST_F(CastTableTest, casttable_boolean) {
    using namespace infinity;
    for (i8 to = LogicalType::kBoolean; to < LogicalType::kInvalid; ++to) {
        switch (to) {
            case LogicalType::kBoolean: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kBoolean), 0);
                break;
            }
            case LogicalType::kVarchar: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kVarchar), 1);
                break;
            }
            default: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, static_cast<LogicalType>(to)), -1);
            }
        }
    }
}