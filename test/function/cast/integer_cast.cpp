//
// Created by JinHai on 2022/12/17.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/integer_cast.h"

class IntegerCastTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(IntegerCastTest, integer_cast1) {
    using namespace infinity;

    {
        DataType source(LogicalType::kTinyInt);
        DataType target(LogicalType::kSmallInt);
        auto func_ptr = BindIntegerCast<TinyIntT>(source, target);
        EXPECT_NE(func_ptr.function, nullptr);

        // Construct column vector
//        func_ptr.function()
    }

    {
        DataType source(LogicalType::kSmallInt);
        DataType target(LogicalType::kBigInt);
        auto func_ptr = BindIntegerCast<SmallIntT>(source, target);
        EXPECT_NE(func_ptr.function, nullptr);
    }

    {
        DataType source(LogicalType::kSmallInt);
        DataType target(LogicalType::kArray);
        EXPECT_THROW(BindIntegerCast<SmallIntT>(source, target), TypeException);
    }

}