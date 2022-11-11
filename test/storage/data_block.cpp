//
// Created by JinHai on 2022/11/12.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/data_block.h"

class DataBlockTest : public BaseTest {

};

TEST_F(DataBlockTest, test1) {
    using namespace infinity;

    DataBlock data_block;
    std::vector<DataType> column_types;
    column_types.emplace_back(LogicalType::kBoolean);

    data_block.Init(column_types);

    for(size_t i = 0; i < 10; ++ i) {
        data_block.SetValue(0, i, Value::MakeBool(i % 2 == 0));
    }

    for(size_t i = 0; i < 10; ++ i) {
        Value value = data_block.GetValue(0, i);
        EXPECT_EQ(value.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(value.value_.boolean, (i % 2 == 0));
    }
}