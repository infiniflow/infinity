//
// Created by jinhai on 23-1-12.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/char_info.h"

class ValueCastTest : public BaseTest {
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

TEST_F(ValueCastTest, bool_value_cast) {
    using namespace infinity;

    DataType varchar_type(LogicalType::kVarchar, VarcharInfo::Make(64));
    Value varchar_value(LogicalType::kVarchar, VarcharInfo::Make(64));


    // Boolean
    Value value = Value::MakeBool(true);
    {
        EXPECT_EQ(value.GetValue<BooleanT>(), true);
        value.TryCastAs(varchar_type, varchar_value);
        EXPECT_EQ(varchar_value.value_.varchar.ToString(), "true");
    }

    {
        value = Value::MakeBool(false);
        EXPECT_EQ(value.GetValue<BooleanT>(), false);
        value.TryCastAs(varchar_type, varchar_value);
        EXPECT_EQ(varchar_value.value_.varchar.ToString(), "false");
    }
}