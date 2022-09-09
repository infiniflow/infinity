//
// Created by JinHai on 2022/9/9.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/vector/vector_operations.h"
#include "main/infinity.h"

class VectorOperationTest : public BaseTest {

};

TEST_F(VectorOperationTest, BoolToOther) {
    using namespace infinity;
    try {
        // Create bool chunk with 2 rows;
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk source_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        source_bool.Append(true);
        source_bool.Append(false);

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}