//
// Created by jinhai on 23-3-1.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/logger.h"
#include "common/types/internal_types.h"
#include "parser/parser_result.h"
#include "parser/sql_parser.h"


class SelectStatementParsingTest : public BaseTest {
    void
    SetUp() override {
    }

    void
    TearDown() override {
    }
};

TEST_F(SelectStatementParsingTest, good_test1) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "select a from t1;";
        parser->Parse(input_sql, result);

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);

        result->Reset();
    }
}

TEST_F(SelectStatementParsingTest, good_test2) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();
#if 0
    {
        String input_sql = "select 1;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto* select_statement = (SelectStatement*)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_EQ(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kConstant);
            auto* const_expr = (ConstantExpr*)(*select_statement->select_list_)[0];
            EXPECT_EQ(const_expr->date_value_, nullptr);
            EXPECT_EQ(const_expr->str_value_, nullptr);
            EXPECT_EQ(const_expr->integer_value_, 1);
            EXPECT_FLOAT_EQ(const_expr->float_value_, 0);
            EXPECT_EQ(const_expr->interval_type_, IntervalExprType::kSecond);
            EXPECT_EQ(const_expr->date_value_, nullptr);
            EXPECT_EQ(const_expr->bool_value_, false);
        }

        result->Reset();
    }

    {
        String input_sql = "select a, sum(b), function(c, nested(d)) from t1;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto* select_statement = (SelectStatement*)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto* table_ref_ptr = (TableReference*)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->table_name_, "t1");
                EXPECT_EQ(table_ref_ptr->schema_name_.empty(), true);
            }

            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 3);

            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col0_expr = (ColumnExpr *) (*select_statement->select_list_)[0];
                EXPECT_EQ(col0_expr->star_, false);
                EXPECT_EQ(col0_expr->names_.size(), 1);
                EXPECT_STREQ(col0_expr->names_[0], "a");
            }
            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
                auto *col1_expr = (FunctionExpr *) (*select_statement->select_list_)[1];
                EXPECT_STREQ(col1_expr->func_name_, "sum");
                auto& args = *col1_expr->arguments_;
                EXPECT_EQ(args.size(), 1);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kColumn);
                auto* arg_expr =  (ColumnExpr *)args[0];
                EXPECT_EQ(arg_expr->names_.size(), 1);
                EXPECT_STREQ(arg_expr->names_[0], "b");
            }
            {
                EXPECT_EQ((*select_statement->select_list_)[2]->type_, ParsedExprType::kFunction);
                auto *col1_expr = (FunctionExpr *) (*select_statement->select_list_)[2];
                EXPECT_STREQ(col1_expr->func_name_, "function");
                auto& args = *col1_expr->arguments_;
                EXPECT_EQ(args.size(), 2);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kColumn);
                {
                    auto *arg_expr = (ColumnExpr *) args[0];
                    EXPECT_EQ(arg_expr->names_.size(), 1);
                    EXPECT_STREQ(arg_expr->names_[0], "c");
                }

                EXPECT_EQ(args[1]->type_, ParsedExprType::kFunction);
                {
                    auto *arg_expr = (FunctionExpr *) args[1];
                    EXPECT_STREQ(arg_expr->func_name_, "nested");
                    EXPECT_EQ(arg_expr->arguments_->size(), 1);

                    auto& func_args = *arg_expr->arguments_;
                    EXPECT_EQ(func_args.size(), 1);
                    EXPECT_EQ(func_args[0]->type_, ParsedExprType::kColumn);
                    auto* func_arg_expr =  (ColumnExpr *)func_args[0];
                    EXPECT_EQ(func_arg_expr->names_.size(), 1);
                    EXPECT_STREQ(func_arg_expr->names_[0], "d");
                }
            }
        }

        result->Reset();
    }

    {
        String input_sql = "select * from t2;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto* select_statement = (SelectStatement*)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto* table_ref_ptr = (TableReference*)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
                EXPECT_EQ(table_ref_ptr->schema_name_.empty(), true);
            }

            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
            auto* col_expr = (ColumnExpr*)(*select_statement->select_list_)[0];
            EXPECT_EQ(col_expr->star_, true);
            EXPECT_EQ(col_expr->names_.size(), 0);
        }

        result->Reset();
    }

    {
        String input_sql = "SELECT 10 - 20, 10 + -20, 10+-5.2, 9223372036854775807, -9223372036854775808";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto* select_statement = (SelectStatement*)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_EQ(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 5);

            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kFunction);
                auto *col0_expr = (FunctionExpr *) (*select_statement->select_list_)[0];
                EXPECT_STREQ(col0_expr->func_name_, "-");
                auto& args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto* arg0_expr =  (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kConstant);
                auto* arg1_expr =  (ConstantExpr *)args[1];
                EXPECT_EQ(arg1_expr->integer_value_, 20);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
                auto *col0_expr = (FunctionExpr *) (*select_statement->select_list_)[1];
                EXPECT_STREQ(col0_expr->func_name_, "+");
                auto& args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto* arg0_expr =  (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kFunction);
                auto* arg1_expr =  (FunctionExpr *)args[1];
                EXPECT_STREQ(arg1_expr->func_name_, "-");
                EXPECT_EQ(arg1_expr->arguments_->size(), 1);
                auto child_args = *arg1_expr->arguments_;
                EXPECT_EQ(child_args[0]->type_, ParsedExprType::kConstant);
                auto* child_arg0_expr =  (ConstantExpr *)child_args[0];
                EXPECT_EQ(child_arg0_expr->integer_value_, 20);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[2]->type_, ParsedExprType::kFunction);
                auto *col0_expr = (FunctionExpr *) (*select_statement->select_list_)[2];
                EXPECT_STREQ(col0_expr->func_name_, "+");
                auto& args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto* arg0_expr =  (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kFunction);
                auto* arg1_expr =  (FunctionExpr *)args[1];
                EXPECT_STREQ(arg1_expr->func_name_, "-");
                EXPECT_EQ(arg1_expr->arguments_->size(), 1);
                auto child_args = *arg1_expr->arguments_;
                EXPECT_EQ(child_args[0]->type_, ParsedExprType::kConstant);
                auto* child_arg0_expr =  (ConstantExpr *)child_args[0];
                EXPECT_FLOAT_EQ(child_arg0_expr->float_value_, 5.2);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[3]->type_, ParsedExprType::kConstant);
                auto *col0_expr = (ConstantExpr *) (*select_statement->select_list_)[3];
                EXPECT_EQ(col0_expr->integer_value_, 9223372036854775807L);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[4]->type_, ParsedExprType::kConstant);
                auto *col0_expr = (ConstantExpr *) (*select_statement->select_list_)[4];
                EXPECT_EQ(col0_expr->integer_value_, -9223372036854775808L);
            }
        }
        result->Reset();
    }
#endif
#if 1
    {
        String input_sql = "SELECT a, AVG(b) AS c FROM t1 GROUP BY a HAVING AVG(b) < 3.2";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto* table_ref_ptr = (TableReference*)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->table_name_, "t1");
                EXPECT_EQ(table_ref_ptr->schema_name_.empty(), true);
            }

            EXPECT_NE(select_statement->group_by_list_, nullptr);
            {
                auto& arg_expr = (*select_statement->group_by_list_)[0];
                EXPECT_EQ(arg_expr->type_, ParsedExprType::kColumn);
                auto* column_arg_expr =  (ColumnExpr *)arg_expr;
                EXPECT_EQ(column_arg_expr->names_.size(), 1);
                EXPECT_STREQ(column_arg_expr->names_[0], "a");
            }

            EXPECT_NE(select_statement->having_expr_, nullptr);
            {
                EXPECT_EQ(select_statement->having_expr_->type_, ParsedExprType::kFunction);
                auto* having_expr =  (FunctionExpr *)select_statement->having_expr_;
                EXPECT_EQ(having_expr->func_name_, "<");

                EXPECT_EQ(having_expr->arguments_->size(), 2);
                auto child0_args = (*having_expr->arguments_)[0];
                auto child1_args = (*having_expr->arguments_)[1];
                EXPECT_EQ(child0_args->type_, ParsedExprType::kFunction);
                auto* child0_function_expr =  (FunctionExpr *)child0_args;
                EXPECT_EQ(child0_function_expr->func_name_, "AVG");
                EXPECT_EQ(child0_function_expr->arguments_->size(), 1);
                auto child0_function_arg_expr = (*child0_function_expr->arguments_)[0];
                EXPECT_EQ(child0_function_arg_expr->type_, ParsedExprType::kColumn);
                auto* child0_function_arg_col_expr =  (ColumnExpr *)child0_function_arg_expr;
                EXPECT_EQ(child0_function_arg_col_expr->names_.size(), 1);
                EXPECT_STREQ(child0_function_arg_col_expr->names_[0], "b");

                EXPECT_EQ(child1_args->type_, ParsedExprType::kConstant);
                auto* child1_args_expr =  (ConstantExpr *)child1_args;
                EXPECT_FLOAT_EQ(child1_args_expr->float_value_, 3.2);
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
            auto* col_expr = (ColumnExpr*)(*select_statement->select_list_)[0];
            EXPECT_EQ(col_expr->star_, false);
            EXPECT_EQ(col_expr->names_.size(), 1);

            EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
            auto* func_expr = (FunctionExpr*)(*select_statement->select_list_)[1];
            EXPECT_EQ(func_expr->func_name_, "AVG");
            EXPECT_EQ(func_expr->arguments_->size(), 1);
            auto func_arg_expr = (*func_expr->arguments_)[0];
            EXPECT_EQ(func_arg_expr->type_, ParsedExprType::kColumn);
            auto* func_arg_col_expr =  (ColumnExpr *)func_arg_expr;
            EXPECT_EQ(func_arg_col_expr->names_.size(), 1);
            EXPECT_STREQ(func_arg_col_expr->names_[0], "b");
        }
    }
#endif
}