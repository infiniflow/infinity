//
// Created by JinHai on 2022/12/16.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/bool_cast.h"

class BoolCastTest : public BaseTest {
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

TEST_F(BoolCastTest, bool_cast0) {
    using namespace infinity;

    // BooleanT to BooleanT, throw exception
    {
        BooleanT source = true;
        BooleanT target;
        EXPECT_THROW(BoolTryCast::Run(source, target), TypeException);
    }
    // BooleanT to VarcharT
    {
        BooleanT source = true;
        VarcharT target;
        EXPECT_TRUE(BoolTryCast::Run(source, target));
        String result = String(target.prefix, 4);
        EXPECT_STREQ("true", result.c_str());

        source = false;
        EXPECT_TRUE(BoolTryCast::Run(source, target));
        result = String(target.prefix, 5);
        EXPECT_STREQ("false", result.c_str());
    }
}

TEST_F(BoolCastTest, bool_cast1) {
    using namespace infinity;

    {
        DataType source(LogicalType::kBoolean);
        DataType target(LogicalType::kVarchar);
        auto func_ptr = BindBoolCast(source, target);
        EXPECT_NE(func_ptr.function, nullptr);
    }

    {
        DataType source(LogicalType::kBoolean);
        DataType target(LogicalType::kBoolean);
        EXPECT_THROW(BindBoolCast(source, target), TypeException);
    }

}