//
// Created by jinhai on 22-12-25.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/info/varchar_info.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

#include "storage/column_def.h"

class ColumnDefTest : public BaseTest {
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


TEST_F(ColumnDefTest, test1) {
    using namespace infinity;

    Set<ConstrainType> constraints;
    constraints.insert(ConstrainType::kUnique);
    constraints.insert(ConstrainType::kNotNull);
    auto column_def_ptr = ColumnDef::Make("c1", 0, DataType(LogicalType::kTinyInt), constraints);
    EXPECT_EQ(column_def_ptr->type(), DataType(LogicalType::kTinyInt));
    EXPECT_EQ(column_def_ptr->id(), 0);
    EXPECT_STREQ(column_def_ptr->name().c_str(), "c1");
    EXPECT_EQ(column_def_ptr->ToString(), "(c1 TinyInt Unique Not nullable)");
}

TEST_F(ColumnDefTest, test2) {
    using namespace infinity;

    auto type_info_ptr = VarcharInfo::Make(128);

    Set<ConstrainType> constraints;
    constraints.insert(ConstrainType::kPrimaryKey);
    auto column_def_ptr = ColumnDef::Make("c2", 3, DataType(LogicalType::kVarchar, type_info_ptr), constraints);
    EXPECT_EQ(column_def_ptr->type(), DataType(LogicalType::kVarchar, type_info_ptr));
    EXPECT_EQ(column_def_ptr->id(), 3);
    EXPECT_EQ(column_def_ptr->name(), "c2");
    EXPECT_EQ(column_def_ptr->ToString(), "(c2 Varchar PrimaryKey)");
}