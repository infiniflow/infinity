//
// Created by jinhai on 23-1-22.
//
#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
//#include "common/types/info/varchar_info.h"
//#include "storage/catalog.h"
#include "parser/expression/parsed_column_expression.h"

class ParsedColumnExpressionTest : public BaseTest {
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

TEST_F(ParsedColumnExpressionTest, test1) {
    using namespace infinity;

    {
        ParsedColumnExpression column1("db", "schema", "table", "column");
        EXPECT_EQ(column1.ToString(), String("db.schema.table.column"));
        EXPECT_EQ(column1.GetName(), String("db.schema.table.column"));
        column1.alias_ = "alias";
        EXPECT_EQ(column1.GetName(), String("alias"));
    }

    {
        ParsedColumnExpression column1("", "schema", "table", "column");
        EXPECT_EQ(column1.ToString(), String("schema.table.column"));
        EXPECT_EQ(column1.GetName(), String("schema.table.column"));
        column1.alias_ = "alias";
        EXPECT_EQ(column1.GetName(), String("alias"));
    }

    {
        ParsedColumnExpression column1("", "", "table", "column");
        EXPECT_EQ(column1.ToString(), String("table.column"));
        EXPECT_EQ(column1.GetName(), String("table.column"));
        column1.alias_ = "alias";
        EXPECT_EQ(column1.GetName(), String("alias"));
    }

    {
        ParsedColumnExpression column1("", "", "", "column");
        EXPECT_EQ(column1.ToString(), String("column"));
        EXPECT_EQ(column1.GetName(), String("column"));
        column1.alias_ = "alias";
        EXPECT_EQ(column1.GetName(), String("alias"));
    }
}
