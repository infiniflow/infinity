//
// Created by jinhai on 22-12-25.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/table_def.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class TableDefTest : public BaseTest {
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


TEST_F(TableDefTest, test1) {
    using namespace infinity;

    Vector<SharedPtr<ColumnDef>> columns;

    i64 column_id = 0;
    {
        HashSet<ConstraintType> constraints;
        constraints.insert(ConstraintType::kUnique);
        constraints.insert(ConstraintType::kNotNull);
        auto column_def_ptr = MakeShared<ColumnDef>(column_id ++,
                                                    DataType(LogicalType::kTinyInt),
                                                    "c1",
                                                    constraints);
        columns.emplace_back(column_def_ptr);

    }
    {
        HashSet<ConstraintType> constraints;
        constraints.insert(ConstraintType::kPrimaryKey);
        auto column_def_ptr = MakeShared<ColumnDef>(column_id ++,
                                                    DataType(LogicalType::kVarchar),
                                                    "c2",
                                                    constraints);
        columns.emplace_back(column_def_ptr);
    }

    TableDef table_def("t1", columns);

    EXPECT_EQ(table_def.table_name(), "t1");
    EXPECT_EQ(table_def.column_count(), 2);
    EXPECT_EQ(table_def.GetColIdByName("c1"), 0);
    EXPECT_EQ(table_def.GetColIdByName("c2"), 1);
    LOG_TRACE("\n{}", table_def.ToString());
}