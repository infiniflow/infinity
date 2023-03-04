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

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for(auto& statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto* select_statement = (SelectStatement*)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            auto* table_ref = (TableReference*)(select_statement->table_ref_);
            EXPECT_EQ(table_ref->schema_name_, "");
            EXPECT_EQ(table_ref->table_name_, "t1");
            EXPECT_EQ(table_ref->alias_, nullptr);

            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
            auto* col_expr = (ColumnExpr*)(*select_statement->select_list_)[0];
            EXPECT_STREQ(col_expr->names_[0], "a");
            EXPECT_EQ(col_expr->star_, false);
            EXPECT_EQ(col_expr->alias_.empty(), true);
        }

        result->Reset();
    }
}

TEST_F(SelectStatementParsingTest, good_test2) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

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
                EXPECT_EQ(col1_expr->func_name_, "sum");
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
                EXPECT_EQ(col1_expr->func_name_, "function");
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
                    EXPECT_EQ(arg_expr->func_name_, "nested");
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
                EXPECT_EQ(col0_expr->func_name_, "-");
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
                EXPECT_EQ(col0_expr->func_name_, "+");
                auto& args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto* arg0_expr =  (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kFunction);
                auto* arg1_expr =  (FunctionExpr *)args[1];
                EXPECT_EQ(arg1_expr->func_name_, "-");
                EXPECT_EQ(arg1_expr->arguments_->size(), 1);
                auto child_args = *arg1_expr->arguments_;
                EXPECT_EQ(child_args[0]->type_, ParsedExprType::kConstant);
                auto* child_arg0_expr =  (ConstantExpr *)child_args[0];
                EXPECT_EQ(child_arg0_expr->integer_value_, 20);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[2]->type_, ParsedExprType::kFunction);
                auto *col0_expr = (FunctionExpr *) (*select_statement->select_list_)[2];
                EXPECT_EQ(col0_expr->func_name_, "+");
                auto& args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto* arg0_expr =  (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kFunction);
                auto* arg1_expr =  (FunctionExpr *)args[1];
                EXPECT_EQ(arg1_expr->func_name_, "-");
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
        result->Reset();
    }

    {
        String input_sql = "SELECT a AS c FROM s1.t1;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s1");
                EXPECT_EQ(table_ref_ptr->table_name_, "t1");
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT count(distinct a), count(b) AS c FROM s3.t2;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *) (*select_statement->select_list_)[0];
                EXPECT_EQ(func_expr->func_name_, "count");
                EXPECT_EQ(func_expr->distinct_, true);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *) (*select_statement->select_list_)[1];
                EXPECT_EQ(func_expr->func_name_, "count");
                EXPECT_EQ(func_expr->distinct_, false);
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT a, b FROM s3.t2 ORDER BY a, b DESC;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *) (*select_statement->select_list_)[0];
                EXPECT_STREQ(col_expr->names_[0], "a");
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *) (*select_statement->select_list_)[1];
                EXPECT_STREQ(col_expr->names_[0], "b");
            }

            EXPECT_NE(select_statement->order_by_list, nullptr);
            EXPECT_EQ(select_statement->order_by_list->size(), 2);
            {
                EXPECT_EQ((*select_statement->order_by_list)[0]->type_, OrderType::kAsc);
                EXPECT_EQ((*select_statement->order_by_list)[0]->expr_->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *) ((*select_statement->order_by_list)[0]->expr_);
                EXPECT_STREQ(col_expr->names_[0], "a");
            }
            {
                EXPECT_EQ((*select_statement->order_by_list)[1]->type_, OrderType::kDesc);
                EXPECT_EQ((*select_statement->order_by_list)[1]->expr_->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *) ((*select_statement->order_by_list)[1]->expr_);
                EXPECT_STREQ(col_expr->names_[0], "b");
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT a, b FROM s3.t2 WHERE a BETWEEN 1 AND 4;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *) (*select_statement->select_list_)[0];
                EXPECT_STREQ(col_expr->names_[0], "a");
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *) (*select_statement->select_list_)[1];
                EXPECT_STREQ(col_expr->names_[0], "b");
            }

            EXPECT_NE(select_statement->where_expr_, nullptr);
            {
                EXPECT_EQ(select_statement->where_expr_->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *) (select_statement->where_expr_);
                EXPECT_EQ(func_expr->func_name_, "between_and");
                {
                    auto *arg0_expr = (*func_expr->arguments_)[0];
                    EXPECT_EQ(arg0_expr->type_, ParsedExprType::kColumn);
                    auto *arg0_col_expr = (ColumnExpr *) (arg0_expr);
                    EXPECT_STREQ(arg0_col_expr->names_[0], "a");
                }
                {
                    auto *arg1_expr = (*func_expr->arguments_)[1];
                    EXPECT_EQ(arg1_expr->type_, ParsedExprType::kConstant);
                    auto *arg1_constant_expr = (ConstantExpr *) (arg1_expr);
                    EXPECT_EQ(arg1_constant_expr->literal_type_, LiteralType::kInteger);
                    EXPECT_EQ(arg1_constant_expr->integer_value_, 1);
                }
                {
                    auto *arg3_expr = (*func_expr->arguments_)[2];
                    EXPECT_EQ(arg3_expr->type_, ParsedExprType::kConstant);
                    auto *arg3_constant_expr = (ConstantExpr *) (arg3_expr);
                    EXPECT_EQ(arg3_constant_expr->literal_type_, LiteralType::kInteger);
                    EXPECT_EQ(arg3_constant_expr->integer_value_, 4);
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM s3.t2 WHERE a = (SELECT MIN(c) FROM t1) AND EXISTS (SELECT * FROM test WHERE x < y);";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *) (*select_statement->select_list_)[0];
                EXPECT_EQ(col_expr->names_.size(), 0);
                EXPECT_EQ(col_expr->star_, true);
            }

            EXPECT_NE(select_statement->where_expr_, nullptr);
            {
                EXPECT_EQ(select_statement->where_expr_->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *) (select_statement->where_expr_);
                EXPECT_EQ(func_expr->func_name_, "and");
                {
                    auto *arg0_expr = (*func_expr->arguments_)[0];
                    EXPECT_EQ(arg0_expr->type_, ParsedExprType::kFunction);
                    auto *arg0_func_expr = (FunctionExpr *) (arg0_expr);
                    EXPECT_EQ(arg0_func_expr->func_name_, "=");
                    {
                        auto *arg01_expr = (SubqueryExpr*)(*arg0_func_expr->arguments_)[1];
                        EXPECT_EQ(arg01_expr->subquery_type_, SubqueryType::kScalar);
                        EXPECT_EQ(arg01_expr->left_, nullptr);
                        auto* subquery_select = arg01_expr->select_;
                        {
                            EXPECT_EQ((*subquery_select->select_list_)[0]->type_, ParsedExprType::kFunction);
                            auto *subquery_func_expr = (FunctionExpr *) (*subquery_select->select_list_)[0];
                            EXPECT_EQ(subquery_func_expr->func_name_, "MIN");
                            EXPECT_EQ((*subquery_func_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                            auto* subquery_func_arg_col_expr = (ColumnExpr*)(*subquery_func_expr->arguments_)[0];
                            EXPECT_STREQ(subquery_func_arg_col_expr->names_[0], "c");
                        }
                        {
                            auto *table_ref_ptr = (TableReference *) (subquery_select->table_ref_);
                            EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                            EXPECT_EQ(table_ref_ptr->table_name_, "t1");
                        }
                    }
                }
                {
                    auto *arg1_expr = (*func_expr->arguments_)[1];
                    EXPECT_EQ(arg1_expr->type_, ParsedExprType::kSubquery);
                    auto *arg1_expr_expr = (SubqueryExpr *) (arg1_expr);
                    EXPECT_EQ(arg1_expr_expr->subquery_type_, SubqueryType::kExists);
                    auto* subquery_select = arg1_expr_expr->select_;
                    {
                        EXPECT_EQ((*subquery_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                        auto *subquery_col_expr = (ColumnExpr *) (*subquery_select->select_list_)[0];
                        EXPECT_EQ(subquery_col_expr->star_, true);
                        EXPECT_EQ(subquery_col_expr->names_.size(), 0);
                    }
                    {
                        auto *table_ref_ptr = (TableReference *) (subquery_select->table_ref_);
                        EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                        EXPECT_EQ(table_ref_ptr->table_name_, "test");
                    }
                    {
                        EXPECT_EQ(subquery_select->where_expr_->type_, ParsedExprType::kFunction);
                        auto *sub_where_func_expr = (FunctionExpr *) (subquery_select->where_expr_);
                        EXPECT_EQ(sub_where_func_expr->func_name_, "<");
                    }
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT MIN(CASE WHEN c = 'xxx' THEN d ELSE 1 END) FROM s3.tx;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "tx");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *) (*select_statement->select_list_)[0];
                EXPECT_EQ(func_expr->func_name_, "MIN");
                EXPECT_EQ((*func_expr->arguments_)[0]->type_, ParsedExprType::kCase);
                CaseExpr* case_expr = (CaseExpr*)(*func_expr->arguments_)[0];
                EXPECT_EQ(case_expr->expr_, nullptr);
                CaseCheck* case_check = (*case_expr->case_check_array_)[0];
                EXPECT_EQ(case_check->when_->type_, ParsedExprType::kFunction);
                {
                    auto* f_expr = (FunctionExpr*)(case_check->when_);
                    EXPECT_EQ((*f_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                    auto* col_expr = (ColumnExpr*)((*f_expr->arguments_)[0]);
                    EXPECT_STREQ(col_expr->names_[0], "c");
                    EXPECT_EQ((*f_expr->arguments_)[1]->type_, ParsedExprType::kConstant);
                    auto* const_expr = (ConstantExpr*)((*f_expr->arguments_)[1]);
                    EXPECT_STREQ(const_expr->str_value_, "xxx");
                }
                EXPECT_EQ(case_check->then_->type_, ParsedExprType::kColumn);
                {
                    auto* col_expr = (ColumnExpr*)(case_check->then_);
                    EXPECT_STREQ(col_expr->names_[0], "d");
                }
                EXPECT_EQ(case_expr->else_expr_->type_, ParsedExprType::kConstant);
                {
                    auto* const_expr = (ConstantExpr*)(case_expr->else_expr_);
                    EXPECT_EQ(const_expr->integer_value_, 1);
                }
            }
            EXPECT_EQ(select_statement->where_expr_, nullptr);
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT CASE WHEN a = 0 THEN 1 WHEN b > 3.5 THEN 2 END FROM s3.tx;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "tx");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kCase);


                CaseExpr* case_expr = (CaseExpr*)(*select_statement->select_list_)[0];
                EXPECT_EQ(case_expr->expr_, nullptr);
                {
                    CaseCheck *case_check = (*case_expr->case_check_array_)[0];
                    EXPECT_EQ(case_check->when_->type_, ParsedExprType::kFunction);
                    {
                        auto *f_expr = (FunctionExpr *) (case_check->when_);
                        EXPECT_EQ(f_expr->func_name_, "=");
                        EXPECT_EQ((*f_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                        auto *col_expr = (ColumnExpr *) ((*f_expr->arguments_)[0]);
                        EXPECT_STREQ(col_expr->names_[0], "a");
                        EXPECT_EQ((*f_expr->arguments_)[1]->type_, ParsedExprType::kConstant);
                        auto *const_expr = (ConstantExpr *) ((*f_expr->arguments_)[1]);
                        EXPECT_EQ(const_expr->integer_value_, 0);
                    }
                    EXPECT_EQ(case_check->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *) (case_check->then_);
                        EXPECT_EQ(c_expr->integer_value_, 1);
                    }
                    case_check = (*case_expr->case_check_array_)[1];
                    EXPECT_EQ(case_check->when_->type_, ParsedExprType::kFunction);
                    {
                        auto *f_expr = (FunctionExpr *) (case_check->when_);
                        EXPECT_EQ(f_expr->func_name_, ">");
                        EXPECT_EQ((*f_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                        auto *col_expr = (ColumnExpr *) ((*f_expr->arguments_)[0]);
                        EXPECT_STREQ(col_expr->names_[0], "b");
                        EXPECT_EQ((*f_expr->arguments_)[1]->type_, ParsedExprType::kConstant);
                        auto *const_expr = (ConstantExpr *) ((*f_expr->arguments_)[1]);
                        EXPECT_FLOAT_EQ(const_expr->float_value_, 3.5);
                    }
                    EXPECT_EQ(case_check->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *) (case_check->then_);
                        EXPECT_EQ(c_expr->integer_value_, 2);
                    }
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT CASE b WHEN 1 THEN 10 WHEN 2 THEN 20 END FROM s3.tx;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *) (select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->schema_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "tx");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kCase);


                CaseExpr* case_expr = (CaseExpr*)(*select_statement->select_list_)[0];
                EXPECT_EQ(case_expr->expr_->type_, ParsedExprType::kColumn);
                auto* case_col_expr = (ColumnExpr*)(case_expr->expr_);
                EXPECT_STREQ(case_col_expr->names_[0], "b");
                {
                    CaseCheck *case_check = (*case_expr->case_check_array_)[0];
                    EXPECT_EQ(case_check->when_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *) (case_check->when_);
                        EXPECT_EQ(c_expr->integer_value_, 1);
                    }
                    EXPECT_EQ(case_check->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *) (case_check->then_);
                        EXPECT_EQ(c_expr->integer_value_, 10);
                    }
                    case_check = (*case_expr->case_check_array_)[1];
                    EXPECT_EQ(case_check->when_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *) (case_check->when_);
                        EXPECT_EQ(c_expr->integer_value_, 2);
                    }
                    EXPECT_EQ(case_check->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *) (case_check->then_);
                        EXPECT_EQ(c_expr->integer_value_, 20);
                    }
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT t1.a, t2.b, SUM(t2.price) FROM t3 INNER JOIN t1 ON t3.c = t1.c \
                           OUTER JOIN t2 ON t1.d = t2.d GROUP BY t1.a, t2.b;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kJoin);
            {
                auto *join_ref_ptr = (JoinReference *) (select_statement->table_ref_);
                {
                    EXPECT_EQ(join_ref_ptr->alias_, nullptr);
                    EXPECT_EQ(join_ref_ptr->condition_->type_, ParsedExprType::kFunction);
                    auto *func_expr = (FunctionExpr *) (join_ref_ptr->condition_);
                    EXPECT_EQ(func_expr->func_name_, "=");
                    ColumnExpr* col_left = (ColumnExpr*)(*func_expr->arguments_)[0];
                    ColumnExpr* col_right = (ColumnExpr*)(*func_expr->arguments_)[1];
                    EXPECT_STREQ(col_left->names_[0], "t1");
                    EXPECT_STREQ(col_left->names_[1], "d");
                    EXPECT_STREQ(col_right->names_[0], "t2");
                    EXPECT_STREQ(col_right->names_[1], "d");
                    EXPECT_EQ(join_ref_ptr->join_type_, JoinType::kFull);
                    EXPECT_EQ(join_ref_ptr->right_->type_, TableRefType::kTable);
                    {
                        auto *right_ref_ptr = (TableReference *) (join_ref_ptr->right_);
                        EXPECT_EQ(right_ref_ptr->alias_, nullptr);
                        EXPECT_EQ(right_ref_ptr->table_name_, "t2");
                    }
                    EXPECT_EQ(join_ref_ptr->left_->type_, TableRefType::kJoin);
                    auto *nested_join_ref = (JoinReference *) (join_ref_ptr->left_);
                    {
                        EXPECT_EQ(nested_join_ref->alias_, nullptr);
                        EXPECT_EQ(nested_join_ref->condition_->type_, ParsedExprType::kFunction);
                        auto *func1_expr = (FunctionExpr *) (nested_join_ref->condition_);
                        EXPECT_EQ(func_expr->func_name_, "=");
                        ColumnExpr *col1_left = (ColumnExpr *) (*func1_expr->arguments_)[0];
                        ColumnExpr *col1_right = (ColumnExpr *) (*func1_expr->arguments_)[1];
                        EXPECT_STREQ(col1_left->names_[0], "t3");
                        EXPECT_STREQ(col1_left->names_[1], "c");
                        EXPECT_STREQ(col1_right->names_[0], "t1");
                        EXPECT_STREQ(col1_right->names_[1], "c");
                        EXPECT_EQ(nested_join_ref->join_type_, JoinType::kInner);
                        EXPECT_EQ(nested_join_ref->right_->type_, TableRefType::kTable);
                        {
                            auto *right_ref_ptr = (TableReference *) (nested_join_ref->right_);
                            EXPECT_EQ(right_ref_ptr->alias_, nullptr);
                            EXPECT_EQ(right_ref_ptr->table_name_, "t1");
                        }
                        EXPECT_EQ(nested_join_ref->left_->type_, TableRefType::kTable);
                        {
                            auto *left_ref_ptr = (TableReference *) (nested_join_ref->left_);
                            EXPECT_EQ(left_ref_ptr->alias_, nullptr);
                            EXPECT_EQ(left_ref_ptr->table_name_, "t3");
                        }
                    }
                }
            }
        }

        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM t1, (SELECT a AS aa FROM aaa) AS t2, (SELECT b AS bb FROM bbb) AS t3";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kCrossProduct);

            CrossProductReference* cross1 = (CrossProductReference*)(select_statement->table_ref_);
            {
                EXPECT_EQ(cross1->left_->type_, TableRefType::kCrossProduct);
                CrossProductReference* cross2 = (CrossProductReference*)(cross1->left_);
                {
                    EXPECT_EQ(cross2->left_->type_, TableRefType::kTable);
                    TableReference* t1 = (TableReference*)(cross2->left_);
                    EXPECT_EQ(t1->table_name_, "t1");
                    EXPECT_EQ(cross2->right_->type_, TableRefType::kSubquery);
                    SubqueryReference* t2 = (SubqueryReference*)(cross2->right_);
                    EXPECT_STREQ(t2->alias_->alias_, "t2");
                    SelectStatement* t2_select = (SelectStatement*) (t2->select_statement_);
                    EXPECT_EQ(t2_select->select_list_->size(), 1);
                    {
                        EXPECT_EQ((*t2_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                        auto *col_expr = (ColumnExpr *) (*t2_select->select_list_)[0];
                        EXPECT_STREQ(col_expr->names_[0], "a");
                        EXPECT_EQ(col_expr->alias_, "aa");
                        auto* t2_table = (TableReference*)(t2_select->table_ref_);
                        EXPECT_EQ(t2_table->table_name_, "aaa");
                    }
                }

                EXPECT_EQ(cross1->right_->type_, TableRefType::kSubquery);
                SubqueryReference* t3 = (SubqueryReference*)(cross1->right_);
                EXPECT_STREQ(t3->alias_->alias_, "t3");
                SelectStatement* t3_select = (SelectStatement*) (t3->select_statement_);
                EXPECT_EQ(t3_select->select_list_->size(), 1);
                {
                    EXPECT_EQ((*t3_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                    auto *col_expr = (ColumnExpr *) (*t3_select->select_list_)[0];
                    EXPECT_STREQ(col_expr->names_[0], "b");
                    EXPECT_EQ(col_expr->alias_, "bb");
                    auto* t3_table = (TableReference*)(t3_select->table_ref_);
                    EXPECT_EQ(t3_table->table_name_, "bbb");
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM t1, (SELECT a AS aa FROM aaa) AS t2, (SELECT b AS bb FROM bbb) AS t3";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kCrossProduct);

            CrossProductReference* cross1 = (CrossProductReference*)(select_statement->table_ref_);
            {
                EXPECT_EQ(cross1->left_->type_, TableRefType::kCrossProduct);
                CrossProductReference* cross2 = (CrossProductReference*)(cross1->left_);
                {
                    EXPECT_EQ(cross2->left_->type_, TableRefType::kTable);
                    TableReference* t1 = (TableReference*)(cross2->left_);
                    EXPECT_EQ(t1->table_name_, "t1");
                    EXPECT_EQ(cross2->right_->type_, TableRefType::kSubquery);
                    SubqueryReference* t2 = (SubqueryReference*)(cross2->right_);
                    EXPECT_STREQ(t2->alias_->alias_, "t2");
                    SelectStatement* t2_select = (SelectStatement*) (t2->select_statement_);
                    EXPECT_EQ(t2_select->select_list_->size(), 1);
                    {
                        EXPECT_EQ((*t2_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                        auto *col_expr = (ColumnExpr *) (*t2_select->select_list_)[0];
                        EXPECT_STREQ(col_expr->names_[0], "a");
                        EXPECT_EQ(col_expr->alias_, "aa");
                        auto* t2_table = (TableReference*)(t2_select->table_ref_);
                        EXPECT_EQ(t2_table->table_name_, "aaa");
                    }
                }

                EXPECT_EQ(cross1->right_->type_, TableRefType::kSubquery);
                SubqueryReference* t3 = (SubqueryReference*)(cross1->right_);
                EXPECT_STREQ(t3->alias_->alias_, "t3");
                SelectStatement* t3_select = (SelectStatement*) (t3->select_statement_);
                EXPECT_EQ(t3_select->select_list_->size(), 1);
                {
                    EXPECT_EQ((*t3_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                    auto *col_expr = (ColumnExpr *) (*t3_select->select_list_)[0];
                    EXPECT_STREQ(col_expr->names_[0], "b");
                    EXPECT_EQ(col_expr->alias_, "bb");
                    auto* t3_table = (TableReference*)(t3_select->table_ref_);
                    EXPECT_EQ(t3_table->table_name_, "bbb");
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM t1 AS xxx(a, b)";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement: *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *) (statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);

            TableReference *t1 = (TableReference *) (select_statement->table_ref_);
            EXPECT_EQ(t1->table_name_, "t1");
            EXPECT_STREQ(t1->alias_->alias_, "xxx");
            EXPECT_EQ((*t1->alias_->column_alias_array_)[0], "a");
            EXPECT_EQ((*t1->alias_->column_alias_array_)[1], "b");
        }
        result->Reset();
    }
}

TEST_F(SelectStatementParsingTest, good_test3) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "SELECT * FROM t1; \
                            SELECt * FROM t2; ";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        SelectStatement *select1 = (SelectStatement *) ((*result->statements_ptr_)[0]);
        EXPECT_NE(select1->table_ref_, nullptr);
        EXPECT_EQ(select1->table_ref_->type_, TableRefType::kTable);
        TableReference *t1 = (TableReference *) (select1->table_ref_);
        EXPECT_EQ(t1->table_name_, "t1");

        SelectStatement *select2 = (SelectStatement *) ((*result->statements_ptr_)[1]);
        EXPECT_NE(select2->table_ref_, nullptr);
        EXPECT_EQ(select2->table_ref_->type_, TableRefType::kTable);
        TableReference *t2 = (TableReference *) (select2->table_ref_);
        EXPECT_EQ(t2->table_name_, "t2");

        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM t1 join t2 on x = y; \
		                    SELECT * FROM t1 inner join t2 on x = y; \
		                    SELECT * FROM t1 left join t2 on x = y; \
		                    SELECT * FROM t1 right join t2 on x = y; \
		                    SELECT * FROM t1 full join t2 on x = y; \
                            SELECT * FROM t1 outer join t2 on x = y; \
		                    SELECT * FROM t1 natural join t2; \
		                    SELECT * FROM t1 cross join t2 on x = y; \
		                    SELECT * FROM t1, t2 where x = y;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[0]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kInner);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[1]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kInner);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[2]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kLeft);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[3]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kRight);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[4]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kFull);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[5]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kFull);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[6]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kNatural);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[7]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *) (select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kCross);
        }
        {
            SelectStatement *select = (SelectStatement *) ((*result->statements_ptr_)[8]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kCrossProduct);
        }
        result->Reset();
    }
}
