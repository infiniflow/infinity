//
// Created by jinhai on 22-12-25.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/info/varchar_info.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

#include "storage/column_def.h"

class ColumnDefTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};


TEST_F(ColumnDefTest, test1) {
    using namespace infinity;

    HashSet<ConstraintType> constraints;
    constraints.insert(ConstraintType::kUnique);
    constraints.insert(ConstraintType::kNotNull);
    auto column_def_ptr = MakeShared<ColumnDef>(0, DataType(LogicalType::kTinyInt), "c1", constraints);
    EXPECT_EQ(column_def_ptr->type(), DataType(LogicalType::kTinyInt));
    EXPECT_EQ(column_def_ptr->id(), 0);
    EXPECT_STREQ(column_def_ptr->name().c_str(), "c1");
    EXPECT_EQ(column_def_ptr->ToString(), "c1 TinyInt Not nullable Unique");
}

TEST_F(ColumnDefTest, test2) {
    using namespace infinity;

    auto type_info_ptr = VarcharInfo::Make(128);

    HashSet<ConstraintType> constraints;
    constraints.insert(ConstraintType::kPrimaryKey);
    auto column_def_ptr = MakeShared<ColumnDef>(3, DataType(LogicalType::kVarchar, type_info_ptr), "c2", constraints);
    EXPECT_EQ(column_def_ptr->type(), DataType(LogicalType::kVarchar, type_info_ptr));
    EXPECT_EQ(column_def_ptr->id(), 3);
    EXPECT_EQ(column_def_ptr->name(), "c2");
    EXPECT_EQ(column_def_ptr->ToString(), "c2 Varchar PrimaryKey");
}