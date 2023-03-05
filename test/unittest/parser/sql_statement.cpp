//
// Created by jinhai on 23-3-5.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/logger.h"
#include "common/types/internal_types.h"
#include "parser/parser_result.h"
#include "parser/sql_parser.h"


class StatementParsingTest : public BaseTest {
    void
    SetUp() override {
    }

    void
    TearDown() override {
    }
};

TEST_F(StatementParsingTest, good_test1) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "CREATE TABLE t1 AS SELECT a, b FROM t2;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCreate);
            auto *create_statement = (CreateStatement *) (statement);
            EXPECT_EQ(create_statement->create_info_->type_, DDLType::kTable);
            CreateTableInfo* table_info = (CreateTableInfo*)create_statement->create_info_.get();
            EXPECT_EQ(table_info->table_name_, "t1");
            EXPECT_EQ(table_info->schema_name_, "Default");
            auto *select = (SelectStatement*)(table_info->select_);
            {
                ColumnExpr* col0 = (ColumnExpr* )(*select->select_list_)[0];
                ColumnExpr* col1 = (ColumnExpr* )(*select->select_list_)[1];
                EXPECT_STREQ(col0->names_[0], "a");
                EXPECT_STREQ(col1->names_[0], "b");

                EXPECT_EQ(select->table_ref_->type_, TableRefType::kTable);
                auto* from_table = (TableReference*)select->table_ref_;
                EXPECT_EQ(from_table->table_name_, "t2");
            }
        }

        result->Reset();
    }

    {
        String input_sql = "UPDATE t1 SET a = 1, b = 2 WHERE c = 'O''K';";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kUpdate);
            auto *update_statement = (UpdateStatement *) (statement);
            EXPECT_EQ(update_statement->table_name_, "t1");
            EXPECT_EQ(update_statement->schema_name_, "Default");
            FunctionExpr* where_func = (FunctionExpr*)(update_statement->where_expr_);
            EXPECT_EQ(where_func->func_name_, "=");
            ColumnExpr* where_left_expr = (ColumnExpr*)(*where_func->arguments_)[0];
            ConstantExpr* where_right_expr = (ConstantExpr*)(*where_func->arguments_)[1];
            EXPECT_STREQ(where_left_expr->names_[0], "c");
            EXPECT_STREQ(where_right_expr->str_value_, "O'K");

            UpdateExpr* up0_expr = (*update_statement->update_expr_array_)[0];
            EXPECT_EQ(up0_expr->column_name, "a");
            EXPECT_EQ(((ConstantExpr*)(up0_expr->value))->integer_value_, 1);
            UpdateExpr* up1_expr = (*update_statement->update_expr_array_)[1];
            EXPECT_EQ(up1_expr->column_name, "b");
            EXPECT_EQ(((ConstantExpr*)(up1_expr->value))->integer_value_, 2);
        }

        result->Reset();
    }

    {
        String input_sql = "INSERT INTO t1 VALUES ('abc', 333);";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kInsert);
            auto *insert_statement = (InsertStatement *) (statement);
            EXPECT_EQ(insert_statement->table_name_, "t1");
            EXPECT_EQ(insert_statement->schema_name_, "Default");

            ConstantExpr* insert0_expr = (ConstantExpr*)(*insert_statement->values_)[0];
            EXPECT_STREQ(insert0_expr->str_value_, "abc");
            ConstantExpr* insert1_expr = (ConstantExpr*)(*insert_statement->values_)[1];
            EXPECT_EQ(insert1_expr->integer_value_, 333);
        }

        result->Reset();
    }
}