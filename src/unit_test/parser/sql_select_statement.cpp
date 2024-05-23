// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "unit_test/base_test.h"

import infinity_exception;

import global_resource_usage;
import third_party;
import sql_parser;
import logger;
import stl;
import infinity_context;
import logical_type;
import internal_types;
import base_statement;
import select_statement;
import parsed_expr;
import column_expr;
import constant_expr;
import function_expr;
import knn_expr;
import match_expr;
import search_expr;
import case_expr;
import subquery_expr;
import between_expr;
import cast_expr;
import knn_expr;
import table_reference;
import join_reference;
import cross_product_reference;
import subquery_reference;
import parser_result;

class SelectStatementParsingTest : public BaseTest {};

TEST_F(SelectStatementParsingTest, good_test1) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "select a from t1;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            auto *table_ref = (TableReference *)(select_statement->table_ref_);
            EXPECT_EQ(table_ref->db_name_, "");
            EXPECT_EQ(table_ref->table_name_, "t1");
            EXPECT_EQ(table_ref->alias_, nullptr);

            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1u);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
            auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[0];
            EXPECT_EQ(col_expr->names_[0], "a");
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
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_EQ(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1u);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kConstant);
            auto *const_expr = (ConstantExpr *)(*select_statement->select_list_)[0];
            EXPECT_EQ(const_expr->date_value_, nullptr);
            EXPECT_EQ(const_expr->str_value_, nullptr);
            EXPECT_EQ(const_expr->integer_value_, 1);
            EXPECT_FLOAT_EQ(const_expr->double_value_, 0);
            EXPECT_EQ(const_expr->interval_type_, TimeUnit::kInvalidUnit);
            EXPECT_EQ(const_expr->date_value_, nullptr);
            EXPECT_EQ(const_expr->bool_value_, false);
        }

        result->Reset();
    }

    {
        String input_sql = "select a, sum(b), function(c, nested(d)) from t1;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->table_name_, "t1");
                EXPECT_EQ(table_ref_ptr->db_name_, "");
            }

            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 3u);

            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col0_expr = (ColumnExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(col0_expr->star_, false);
                EXPECT_EQ(col0_expr->names_.size(), 1u);
                EXPECT_EQ(col0_expr->names_[0], "a");
            }
            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
                auto *col1_expr = (FunctionExpr *)(*select_statement->select_list_)[1];
                EXPECT_EQ(col1_expr->func_name_, "sum");
                auto &args = *col1_expr->arguments_;
                EXPECT_EQ(args.size(), 1u);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kColumn);
                auto *arg_expr = (ColumnExpr *)args[0];
                EXPECT_EQ(arg_expr->names_.size(), 1u);
                EXPECT_EQ(arg_expr->names_[0], "b");
            }
            {
                EXPECT_EQ((*select_statement->select_list_)[2]->type_, ParsedExprType::kFunction);
                auto *col1_expr = (FunctionExpr *)(*select_statement->select_list_)[2];
                EXPECT_EQ(col1_expr->func_name_, "function");
                auto &args = *col1_expr->arguments_;
                EXPECT_EQ(args.size(), 2u);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kColumn);
                {
                    auto *arg_expr = (ColumnExpr *)args[0];
                    EXPECT_EQ(arg_expr->names_.size(), 1u);
                    EXPECT_EQ(arg_expr->names_[0], "c");
                }

                EXPECT_EQ(args[1]->type_, ParsedExprType::kFunction);
                {
                    auto *arg_expr = (FunctionExpr *)args[1];
                    EXPECT_EQ(arg_expr->func_name_, "nested");
                    EXPECT_EQ(arg_expr->arguments_->size(), 1u);

                    auto &func_args = *arg_expr->arguments_;
                    EXPECT_EQ(func_args.size(), 1u);
                    EXPECT_EQ(func_args[0]->type_, ParsedExprType::kColumn);
                    auto *func_arg_expr = (ColumnExpr *)func_args[0];
                    EXPECT_EQ(func_arg_expr->names_.size(), 1u);
                    EXPECT_EQ(func_arg_expr->names_[0], "d");
                }
            }
        }

        result->Reset();
    }

    {
        String input_sql = "select * from t2;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
                EXPECT_EQ(table_ref_ptr->db_name_, "");
            }

            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1u);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
            auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[0];
            EXPECT_EQ(col_expr->star_, true);
            EXPECT_EQ(col_expr->names_.size(), 0u);
        }

        result->Reset();
    }

    {
        String input_sql = "SELECT 10 - 20, 10 + -20, 10+-5.2, 9223372036854775807, -9223372036854775808";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);
            EXPECT_EQ(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->group_by_list_, nullptr);
            EXPECT_EQ(select_statement->having_expr_, nullptr);
            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 5u);

            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kFunction);
                auto *col0_expr = (FunctionExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(col0_expr->func_name_, "-");
                auto &args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2u);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto *arg0_expr = (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kConstant);
                auto *arg1_expr = (ConstantExpr *)args[1];
                EXPECT_EQ(arg1_expr->integer_value_, 20);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
                auto *col0_expr = (FunctionExpr *)(*select_statement->select_list_)[1];
                EXPECT_EQ(col0_expr->func_name_, "+");
                auto &args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2u);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto *arg0_expr = (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kConstant);
                auto *arg1_expr = (ConstantExpr *)args[1];
                EXPECT_EQ(arg1_expr->integer_value_, -20);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[2]->type_, ParsedExprType::kFunction);
                auto *col0_expr = (FunctionExpr *)(*select_statement->select_list_)[2];
                EXPECT_EQ(col0_expr->func_name_, "+");
                auto &args = *col0_expr->arguments_;
                EXPECT_EQ(args.size(), 2u);
                EXPECT_EQ(args[0]->type_, ParsedExprType::kConstant);
                auto *arg0_expr = (ConstantExpr *)args[0];
                EXPECT_EQ(arg0_expr->integer_value_, 10);

                EXPECT_EQ(args[1]->type_, ParsedExprType::kConstant);
                auto *arg1_expr = (ConstantExpr *)args[1];
                EXPECT_FLOAT_EQ(arg1_expr->double_value_, -5.2);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[3]->type_, ParsedExprType::kConstant);
                auto *col0_expr = (ConstantExpr *)(*select_statement->select_list_)[3];
                EXPECT_EQ(col0_expr->integer_value_, INT64_MAX);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[4]->type_, ParsedExprType::kConstant);
                auto *col0_expr = (ConstantExpr *)(*select_statement->select_list_)[4];
                EXPECT_EQ(col0_expr->integer_value_, INT64_MIN);
            }
        }
        result->Reset();
    }
    {
        String input_sql = "SELECT a, AVG(b) AS c FROM t1 GROUP BY a HAVING AVG(b) < 3.2";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->table_name_, "t1");
                EXPECT_EQ(table_ref_ptr->db_name_, "");
            }

            EXPECT_NE(select_statement->group_by_list_, nullptr);
            {
                auto &arg_expr = (*select_statement->group_by_list_)[0];
                EXPECT_EQ(arg_expr->type_, ParsedExprType::kColumn);
                auto *column_arg_expr = (ColumnExpr *)arg_expr;
                EXPECT_EQ(column_arg_expr->names_.size(), 1u);
                EXPECT_EQ(column_arg_expr->names_[0], "a");
            }

            EXPECT_NE(select_statement->having_expr_, nullptr);
            {
                EXPECT_EQ(select_statement->having_expr_->type_, ParsedExprType::kFunction);
                auto *having_expr = (FunctionExpr *)select_statement->having_expr_;
                EXPECT_EQ(having_expr->func_name_, "<");

                EXPECT_EQ(having_expr->arguments_->size(), 2u);
                auto child0_args = (*having_expr->arguments_)[0];
                auto child1_args = (*having_expr->arguments_)[1];
                EXPECT_EQ(child0_args->type_, ParsedExprType::kFunction);
                auto *child0_function_expr = (FunctionExpr *)child0_args;
                EXPECT_EQ(child0_function_expr->func_name_, "avg");
                EXPECT_EQ(child0_function_expr->arguments_->size(), 1u);
                auto child0_function_arg_expr = (*child0_function_expr->arguments_)[0];
                EXPECT_EQ(child0_function_arg_expr->type_, ParsedExprType::kColumn);
                auto *child0_function_arg_col_expr = (ColumnExpr *)child0_function_arg_expr;
                EXPECT_EQ(child0_function_arg_col_expr->names_.size(), 1u);
                EXPECT_EQ(child0_function_arg_col_expr->names_[0], "b");

                EXPECT_EQ(child1_args->type_, ParsedExprType::kConstant);
                auto *child1_args_expr = (ConstantExpr *)child1_args;
                EXPECT_FLOAT_EQ(child1_args_expr->double_value_, 3.2);
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2u);
            EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
            auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[0];
            EXPECT_EQ(col_expr->star_, false);
            EXPECT_EQ(col_expr->names_.size(), 1u);

            EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
            auto *func_expr = (FunctionExpr *)(*select_statement->select_list_)[1];
            EXPECT_EQ(func_expr->func_name_, "avg");
            EXPECT_EQ(func_expr->arguments_->size(), 1u);
            auto func_arg_expr = (*func_expr->arguments_)[0];
            EXPECT_EQ(func_arg_expr->type_, ParsedExprType::kColumn);
            auto *func_arg_col_expr = (ColumnExpr *)func_arg_expr;
            EXPECT_EQ(func_arg_col_expr->names_.size(), 1u);
            EXPECT_EQ(func_arg_col_expr->names_[0], "b");
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT a AS c FROM s1.t1;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s1");
                EXPECT_EQ(table_ref_ptr->table_name_, "t1");
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT count(distinct a), count(b) AS c FROM s3.t2;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2u);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(func_expr->func_name_, "count");
                EXPECT_EQ(func_expr->distinct_, true);
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *)(*select_statement->select_list_)[1];
                EXPECT_EQ(func_expr->func_name_, "count");
                EXPECT_EQ(func_expr->distinct_, false);
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT a, b FROM s3.t2 ORDER BY a, b DESC;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->where_expr_, nullptr);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2u);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(col_expr->names_[0], "a");
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[1];
                EXPECT_EQ(col_expr->names_[0], "b");
            }

            EXPECT_NE(select_statement->order_by_list, nullptr);
            EXPECT_EQ(select_statement->order_by_list->size(), 2u);
            {
                EXPECT_EQ((*select_statement->order_by_list)[0]->type_, OrderType::kAsc);
                EXPECT_EQ((*select_statement->order_by_list)[0]->expr_->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)((*select_statement->order_by_list)[0]->expr_);
                EXPECT_EQ(col_expr->names_[0], "a");
            }
            {
                EXPECT_EQ((*select_statement->order_by_list)[1]->type_, OrderType::kDesc);
                EXPECT_EQ((*select_statement->order_by_list)[1]->expr_->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)((*select_statement->order_by_list)[1]->expr_);
                EXPECT_EQ(col_expr->names_[0], "b");
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT a, b FROM s3.t2 WHERE a BETWEEN 1 AND 4;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 2u);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(col_expr->names_[0], "a");
            }

            {
                EXPECT_EQ((*select_statement->select_list_)[1]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[1];
                EXPECT_EQ(col_expr->names_[0], "b");
            }

            EXPECT_NE(select_statement->where_expr_, nullptr);
            {
                EXPECT_EQ(select_statement->where_expr_->type_, ParsedExprType::kBetween);
                auto *between_expr = (BetweenExpr *)(select_statement->where_expr_);
                {
                    auto *arg0_expr = between_expr->value_;
                    EXPECT_EQ(arg0_expr->type_, ParsedExprType::kColumn);
                    auto *arg0_col_expr = (ColumnExpr *)(arg0_expr);
                    EXPECT_EQ(arg0_col_expr->names_[0], "a");
                }
                {
                    auto *arg1_expr = between_expr->lower_bound_;
                    EXPECT_EQ(arg1_expr->type_, ParsedExprType::kConstant);
                    auto *arg1_constant_expr = (ConstantExpr *)(arg1_expr);
                    EXPECT_EQ(arg1_constant_expr->literal_type_, LiteralType::kInteger);
                    EXPECT_EQ(arg1_constant_expr->integer_value_, 1);
                }
                {
                    auto *arg3_expr = between_expr->upper_bound_;
                    EXPECT_EQ(arg3_expr->type_, ParsedExprType::kConstant);
                    auto *arg3_constant_expr = (ConstantExpr *)(arg3_expr);
                    EXPECT_EQ(arg3_constant_expr->literal_type_, LiteralType::kInteger);
                    EXPECT_EQ(arg3_constant_expr->integer_value_, 4);
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM s3.t2 WHERE a = (SELECT MIN(c) FROM t1) AND EXISTS (SELECT * FROM test WHERE x < y);";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "t2");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1u);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(col_expr->names_.size(), 0u);
                EXPECT_EQ(col_expr->star_, true);
            }

            EXPECT_NE(select_statement->where_expr_, nullptr);
            {
                EXPECT_EQ(select_statement->where_expr_->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *)(select_statement->where_expr_);
                EXPECT_EQ(func_expr->func_name_, "and");
                {
                    auto *arg0_expr = (*func_expr->arguments_)[0];
                    EXPECT_EQ(arg0_expr->type_, ParsedExprType::kFunction);
                    auto *arg0_func_expr = (FunctionExpr *)(arg0_expr);
                    EXPECT_EQ(arg0_func_expr->func_name_, "=");
                    {
                        auto *arg01_expr = (SubqueryExpr *)(*arg0_func_expr->arguments_)[1];
                        EXPECT_EQ(arg01_expr->subquery_type_, SubqueryType::kScalar);
                        EXPECT_EQ(arg01_expr->left_, nullptr);
                        auto *subquery_select = arg01_expr->select_;
                        {
                            EXPECT_EQ((*subquery_select->select_list_)[0]->type_, ParsedExprType::kFunction);
                            auto *subquery_func_expr = (FunctionExpr *)(*subquery_select->select_list_)[0];
                            EXPECT_EQ(subquery_func_expr->func_name_, "min");
                            EXPECT_EQ((*subquery_func_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                            auto *subquery_func_arg_col_expr = (ColumnExpr *)(*subquery_func_expr->arguments_)[0];
                            EXPECT_EQ(subquery_func_arg_col_expr->names_[0], "c");
                        }
                        {
                            auto *table_ref_ptr = (TableReference *)(subquery_select->table_ref_);
                            EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                            EXPECT_EQ(table_ref_ptr->table_name_, "t1");
                        }
                    }
                }
                {
                    auto *arg1_expr = (*func_expr->arguments_)[1];
                    EXPECT_EQ(arg1_expr->type_, ParsedExprType::kSubquery);
                    auto *arg1_expr_expr = (SubqueryExpr *)(arg1_expr);
                    EXPECT_EQ(arg1_expr_expr->subquery_type_, SubqueryType::kExists);
                    auto *subquery_select = arg1_expr_expr->select_;
                    {
                        EXPECT_EQ((*subquery_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                        auto *subquery_col_expr = (ColumnExpr *)(*subquery_select->select_list_)[0];
                        EXPECT_EQ(subquery_col_expr->star_, true);
                        EXPECT_EQ(subquery_col_expr->names_.size(), 0u);
                    }
                    {
                        auto *table_ref_ptr = (TableReference *)(subquery_select->table_ref_);
                        EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                        EXPECT_EQ(table_ref_ptr->table_name_, "test");
                    }
                    {
                        EXPECT_EQ(subquery_select->where_expr_->type_, ParsedExprType::kFunction);
                        auto *sub_where_func_expr = (FunctionExpr *)(subquery_select->where_expr_);
                        EXPECT_EQ(sub_where_func_expr->func_name_, "<");
                    }
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT MIN(CASE WHEN c = 'xxx' THEN d ELSE 1 END) FROM s3.tx;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "tx");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1u);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kFunction);
                auto *func_expr = (FunctionExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(func_expr->func_name_, "min");
                EXPECT_EQ((*func_expr->arguments_)[0]->type_, ParsedExprType::kCase);
                CaseExpr *case_expr = (CaseExpr *)(*func_expr->arguments_)[0];
                EXPECT_EQ(case_expr->expr_, nullptr);
                WhenThen *when_then = (*case_expr->case_check_array_)[0];
                EXPECT_EQ(when_then->when_->type_, ParsedExprType::kFunction);
                {
                    auto *f_expr = (FunctionExpr *)(when_then->when_);
                    EXPECT_EQ((*f_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                    auto *col_expr = (ColumnExpr *)((*f_expr->arguments_)[0]);
                    EXPECT_EQ(col_expr->names_[0], "c");
                    EXPECT_EQ((*f_expr->arguments_)[1]->type_, ParsedExprType::kConstant);
                    auto *const_expr = (ConstantExpr *)((*f_expr->arguments_)[1]);
                    EXPECT_STREQ(const_expr->str_value_, "xxx");
                }
                EXPECT_EQ(when_then->then_->type_, ParsedExprType::kColumn);
                {
                    auto *col_expr = (ColumnExpr *)(when_then->then_);
                    EXPECT_EQ(col_expr->names_[0], "d");
                }
                EXPECT_EQ(case_expr->else_expr_->type_, ParsedExprType::kConstant);
                {
                    auto *const_expr = (ConstantExpr *)(case_expr->else_expr_);
                    EXPECT_EQ(const_expr->integer_value_, 1);
                }
            }
            EXPECT_EQ(select_statement->where_expr_, nullptr);
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT CASE WHEN a = 0 THEN 1 WHEN b > 3.5 THEN 2 END FROM s3.tx;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "tx");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1u);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kCase);

                CaseExpr *case_expr = (CaseExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(case_expr->expr_, nullptr);
                {
                    WhenThen *when_then = (*case_expr->case_check_array_)[0];
                    EXPECT_EQ(when_then->when_->type_, ParsedExprType::kFunction);
                    {
                        auto *f_expr = (FunctionExpr *)(when_then->when_);
                        EXPECT_EQ(f_expr->func_name_, "=");
                        EXPECT_EQ((*f_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                        auto *col_expr = (ColumnExpr *)((*f_expr->arguments_)[0]);
                        EXPECT_EQ(col_expr->names_[0], "a");
                        EXPECT_EQ((*f_expr->arguments_)[1]->type_, ParsedExprType::kConstant);
                        auto *const_expr = (ConstantExpr *)((*f_expr->arguments_)[1]);
                        EXPECT_EQ(const_expr->integer_value_, 0);
                    }
                    EXPECT_EQ(when_then->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *)(when_then->then_);
                        EXPECT_EQ(c_expr->integer_value_, 1);
                    }
                    when_then = (*case_expr->case_check_array_)[1];
                    EXPECT_EQ(when_then->when_->type_, ParsedExprType::kFunction);
                    {
                        auto *f_expr = (FunctionExpr *)(when_then->when_);
                        EXPECT_EQ(f_expr->func_name_, ">");
                        EXPECT_EQ((*f_expr->arguments_)[0]->type_, ParsedExprType::kColumn);
                        auto *col_expr = (ColumnExpr *)((*f_expr->arguments_)[0]);
                        EXPECT_EQ(col_expr->names_[0], "b");
                        EXPECT_EQ((*f_expr->arguments_)[1]->type_, ParsedExprType::kConstant);
                        auto *const_expr = (ConstantExpr *)((*f_expr->arguments_)[1]);
                        EXPECT_FLOAT_EQ(const_expr->double_value_, 3.5);
                    }
                    EXPECT_EQ(when_then->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *)(when_then->then_);
                        EXPECT_EQ(c_expr->integer_value_, 2);
                    }
                }
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT CASE b WHEN 1 THEN 10 WHEN 2 THEN 20 END FROM s3.tx;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);
            {
                auto *table_ref_ptr = (TableReference *)(select_statement->table_ref_);
                EXPECT_EQ(table_ref_ptr->alias_, nullptr);
                EXPECT_EQ(table_ref_ptr->db_name_, "s3");
                EXPECT_EQ(table_ref_ptr->table_name_, "tx");
            }

            EXPECT_EQ(select_statement->select_distinct_, false);
            EXPECT_NE(select_statement->select_list_, nullptr);
            EXPECT_EQ(select_statement->select_list_->size(), 1u);
            {
                EXPECT_EQ((*select_statement->select_list_)[0]->type_, ParsedExprType::kCase);

                CaseExpr *case_expr = (CaseExpr *)(*select_statement->select_list_)[0];
                EXPECT_EQ(case_expr->expr_->type_, ParsedExprType::kColumn);
                auto *case_col_expr = (ColumnExpr *)(case_expr->expr_);
                EXPECT_EQ(case_col_expr->names_[0], "b");
                {
                    WhenThen *when_then = (*case_expr->case_check_array_)[0];
                    EXPECT_EQ(when_then->when_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *)(when_then->when_);
                        EXPECT_EQ(c_expr->integer_value_, 1);
                    }
                    EXPECT_EQ(when_then->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *)(when_then->then_);
                        EXPECT_EQ(c_expr->integer_value_, 10);
                    }
                    when_then = (*case_expr->case_check_array_)[1];
                    EXPECT_EQ(when_then->when_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *)(when_then->when_);
                        EXPECT_EQ(c_expr->integer_value_, 2);
                    }
                    EXPECT_EQ(when_then->then_->type_, ParsedExprType::kConstant);
                    {
                        auto *c_expr = (ConstantExpr *)(when_then->then_);
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
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kJoin);
            {
                auto *join_ref_ptr = (JoinReference *)(select_statement->table_ref_);
                {
                    EXPECT_EQ(join_ref_ptr->alias_, nullptr);
                    EXPECT_EQ(join_ref_ptr->condition_->type_, ParsedExprType::kFunction);
                    auto *func_expr = (FunctionExpr *)(join_ref_ptr->condition_);
                    EXPECT_EQ(func_expr->func_name_, "=");
                    ColumnExpr *col_left = (ColumnExpr *)(*func_expr->arguments_)[0];
                    ColumnExpr *col_right = (ColumnExpr *)(*func_expr->arguments_)[1];
                    EXPECT_EQ(col_left->names_[0], "t1");
                    EXPECT_EQ(col_left->names_[1], "d");
                    EXPECT_EQ(col_right->names_[0], "t2");
                    EXPECT_EQ(col_right->names_[1], "d");
                    EXPECT_EQ(join_ref_ptr->join_type_, JoinType::kFull);
                    EXPECT_EQ(join_ref_ptr->right_->type_, TableRefType::kTable);
                    {
                        auto *right_ref_ptr = (TableReference *)(join_ref_ptr->right_);
                        EXPECT_EQ(right_ref_ptr->alias_, nullptr);
                        EXPECT_EQ(right_ref_ptr->table_name_, "t2");
                    }
                    EXPECT_EQ(join_ref_ptr->left_->type_, TableRefType::kJoin);
                    auto *nested_join_ref = (JoinReference *)(join_ref_ptr->left_);
                    {
                        EXPECT_EQ(nested_join_ref->alias_, nullptr);
                        EXPECT_EQ(nested_join_ref->condition_->type_, ParsedExprType::kFunction);
                        auto *func1_expr = (FunctionExpr *)(nested_join_ref->condition_);
                        EXPECT_EQ(func_expr->func_name_, "=");
                        ColumnExpr *col1_left = (ColumnExpr *)(*func1_expr->arguments_)[0];
                        ColumnExpr *col1_right = (ColumnExpr *)(*func1_expr->arguments_)[1];
                        EXPECT_EQ(col1_left->names_[0], "t3");
                        EXPECT_EQ(col1_left->names_[1], "c");
                        EXPECT_EQ(col1_right->names_[0], "t1");
                        EXPECT_EQ(col1_right->names_[1], "c");
                        EXPECT_EQ(nested_join_ref->join_type_, JoinType::kInner);
                        EXPECT_EQ(nested_join_ref->right_->type_, TableRefType::kTable);
                        {
                            auto *right_ref_ptr = (TableReference *)(nested_join_ref->right_);
                            EXPECT_EQ(right_ref_ptr->alias_, nullptr);
                            EXPECT_EQ(right_ref_ptr->table_name_, "t1");
                        }
                        EXPECT_EQ(nested_join_ref->left_->type_, TableRefType::kTable);
                        {
                            auto *left_ref_ptr = (TableReference *)(nested_join_ref->left_);
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
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kCrossProduct);

            CrossProductReference *cross1 = (CrossProductReference *)(select_statement->table_ref_);
            TableReference *t1 = (TableReference *)(cross1->tables_[0]);
            EXPECT_EQ(t1->table_name_, "t1");

            EXPECT_EQ(cross1->tables_[1]->type_, TableRefType::kSubquery);
            SubqueryReference *t2 = (SubqueryReference *)(cross1->tables_[1]);
            EXPECT_STREQ(t2->alias_->alias_, "t2");
            SelectStatement *t2_select = (SelectStatement *)(t2->select_statement_);
            EXPECT_EQ(t2_select->select_list_->size(), 1u);
            {
                EXPECT_EQ((*t2_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)(*t2_select->select_list_)[0];
                EXPECT_EQ(col_expr->names_[0], "a");
                EXPECT_EQ(col_expr->alias_, "aa");
                auto *t2_table = (TableReference *)(t2_select->table_ref_);
                EXPECT_EQ(t2_table->table_name_, "aaa");
            }

            EXPECT_EQ(cross1->tables_[2]->type_, TableRefType::kSubquery);
            SubqueryReference *t3 = (SubqueryReference *)(cross1->tables_[2]);
            EXPECT_STREQ(t3->alias_->alias_, "t3");
            SelectStatement *t3_select = (SelectStatement *)(t3->select_statement_);
            EXPECT_EQ(t3_select->select_list_->size(), 1u);
            {
                EXPECT_EQ((*t3_select->select_list_)[0]->type_, ParsedExprType::kColumn);
                auto *col_expr = (ColumnExpr *)(*t3_select->select_list_)[0];
                EXPECT_EQ(col_expr->names_[0], "b");
                EXPECT_EQ(col_expr->alias_, "bb");
                auto *t3_table = (TableReference *)(t3_select->table_ref_);
                EXPECT_EQ(t3_table->table_name_, "bbb");
            }
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM t1 AS xxx(a, b)";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);

            EXPECT_NE(select_statement->table_ref_, nullptr);
            EXPECT_EQ(select_statement->table_ref_->type_, TableRefType::kTable);

            TableReference *t1 = (TableReference *)(select_statement->table_ref_);
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
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        SelectStatement *select1 = (SelectStatement *)((*result->statements_ptr_)[0]);
        EXPECT_NE(select1->table_ref_, nullptr);
        EXPECT_EQ(select1->table_ref_->type_, TableRefType::kTable);
        TableReference *t1 = (TableReference *)(select1->table_ref_);
        EXPECT_EQ(t1->table_name_, "t1");

        SelectStatement *select2 = (SelectStatement *)((*result->statements_ptr_)[1]);
        EXPECT_NE(select2->table_ref_, nullptr);
        EXPECT_EQ(select2->table_ref_->type_, TableRefType::kTable);
        TableReference *t2 = (TableReference *)(select2->table_ref_);
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
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kInner);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[1]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kInner);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[2]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kLeft);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[3]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kRight);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[4]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kFull);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[5]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kFull);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[6]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kNatural);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[7]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kJoin);
            JoinReference *join = (JoinReference *)(select->table_ref_);
            EXPECT_EQ(join->join_type_, JoinType::kCross);
        }
        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[8]);
            EXPECT_EQ(select->table_ref_->type_, TableRefType::kCrossProduct);
        }
        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM t1 limit 1 + 1 offset 2 + 2; \
		                    SELECT * FROM t1 limit (SELECT MIN(x) FROM t2) offset (SELECT MAX(y) FROM t2); ";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            {
                EXPECT_EQ(select->limit_expr_->type_, ParsedExprType::kFunction);
                auto *limit_func_expr = (FunctionExpr *)(select->limit_expr_);
                EXPECT_EQ(limit_func_expr->func_name_, "+");
                auto *left_func_arg = (ConstantExpr *)((*limit_func_expr->arguments_)[0]);
                auto *right_func_arg = (ConstantExpr *)((*limit_func_expr->arguments_)[1]);
                EXPECT_EQ(left_func_arg->integer_value_, 1);
                EXPECT_EQ(right_func_arg->integer_value_, 1);
            }

            {
                EXPECT_EQ(select->offset_expr_->type_, ParsedExprType::kFunction);
                auto *offset_func_expr = (FunctionExpr *)(select->offset_expr_);
                EXPECT_EQ(offset_func_expr->func_name_, "+");
                auto *left_func_arg = (ConstantExpr *)((*offset_func_expr->arguments_)[0]);
                auto *right_func_arg = (ConstantExpr *)((*offset_func_expr->arguments_)[1]);
                EXPECT_EQ(left_func_arg->integer_value_, 2);
                EXPECT_EQ(right_func_arg->integer_value_, 2);
            }
        }

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[1]);
            { EXPECT_EQ(select->limit_expr_->type_, ParsedExprType::kSubquery); }

            { EXPECT_EQ(select->offset_expr_->type_, ParsedExprType::kSubquery); }
        }

        result->Reset();
    }

    {
        String input_sql = "SELECT extract('year' from date '2025-03-04'); \
		                    SELECT * FROM t1 WHERE extract('year' from a) < 2023";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            auto *func_expr = (FunctionExpr *)(*(select->select_list_))[0];
            EXPECT_EQ(func_expr->func_name_, "extract_year");
            auto *arg1_expr = (ConstantExpr *)(*(func_expr->arguments_))[0];
            EXPECT_STREQ(arg1_expr->date_value_, "2025-03-04");
        }

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[1]);
            auto *func_expr = (FunctionExpr *)(select->where_expr_);
            EXPECT_EQ(func_expr->func_name_, "<");
            auto *arg1_expr = (FunctionExpr *)(*(func_expr->arguments_))[0];
            auto *arg2_expr = (ConstantExpr *)(*(func_expr->arguments_))[1];
            EXPECT_EQ(arg1_expr->func_name_, "extract_year");
            {
                auto *arg12_expr = (ColumnExpr *)(*(arg1_expr->arguments_))[0];
                EXPECT_EQ(arg12_expr->names_[0], "a");
            }
            EXPECT_EQ(arg2_expr->integer_value_, 2023);
        }

        result->Reset();
    }

    {
        String input_sql = "SELECT CAST(5.3 AS BIGINT);";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            auto *cast_expr = (CastExpr *)(*(select->select_list_))[0];
            auto *arg1_expr = (ConstantExpr *)(cast_expr->expr_);
            EXPECT_FLOAT_EQ(arg1_expr->double_value_, 5.3);
            EXPECT_EQ(cast_expr->data_type_.type(), LogicalType::kBigInt);
        }

        result->Reset();
    }

    {
        String input_sql = "WITH t1 AS (SELECT a FROM x), t2 AS (SELECT b FROM y) SELECT * FROM t1, t2;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            WithExpr *with0 = (*select->with_exprs_)[0];
            WithExpr *with1 = (*select->with_exprs_)[1];

            EXPECT_EQ(with0->alias_, "t1");
            ColumnExpr *with0_select_expr = (ColumnExpr *)((*((SelectStatement *)with0->select_)->select_list_)[0]);
            EXPECT_EQ(with0_select_expr->names_[0], "a");
            TableReference *with0_table_ref = (TableReference *)(((SelectStatement *)with0->select_)->table_ref_);
            EXPECT_EQ(with0_table_ref->table_name_, "x");

            EXPECT_EQ(with1->alias_, "t2");
            ColumnExpr *with1_select_expr = (ColumnExpr *)((*((SelectStatement *)with1->select_)->select_list_)[0]);
            EXPECT_EQ(with1_select_expr->names_[0], "b");
            TableReference *with1_table_ref = (TableReference *)(((SelectStatement *)with1->select_)->table_ref_);
            EXPECT_EQ(with1_table_ref->table_name_, "y");

            EXPECT_EQ(select->table_ref_->type_, TableRefType::kCrossProduct);
            CrossProductReference *cross = (CrossProductReference *)(select->table_ref_);
            TableReference *cross_left = (TableReference *)(cross->tables_[0]);
            TableReference *cross_right = (TableReference *)(cross->tables_[1]);
            EXPECT_EQ(cross_left->table_name_, "t1");
            EXPECT_EQ(cross_right->table_name_, "t2");
        }

        result->Reset();
    }

    {
        String input_sql = "select a + 3 day from t1;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            FunctionExpr *func_expr = (FunctionExpr *)(*select->select_list_)[0];
            EXPECT_EQ(func_expr->func_name_, "+");
            ColumnExpr *arg0 = (ColumnExpr *)(*func_expr->arguments_)[0];
            EXPECT_EQ(arg0->names_[0], "a");

            ConstantExpr *arg1 = (ConstantExpr *)(*func_expr->arguments_)[1];
            EXPECT_EQ(arg1->interval_type_, TimeUnit::kDay);
            EXPECT_EQ(arg1->integer_value_, 3);
        }

        result->Reset();
    }

    {
        String input_sql = "SELECT * FROM t where a = cast ('2023-01-01' as date) - INTERVAL 15 days;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            FunctionExpr *func_expr = (FunctionExpr *)select->where_expr_;
            EXPECT_EQ(func_expr->func_name_, "=");
            ColumnExpr *arg0 = (ColumnExpr *)(*func_expr->arguments_)[0];
            EXPECT_EQ(arg0->names_[0], "a");

            FunctionExpr *arg1 = (FunctionExpr *)(*func_expr->arguments_)[1];
            EXPECT_EQ(arg1->func_name_, "-");
            CastExpr *arg10 = (CastExpr *)(*arg1->arguments_)[0];
            EXPECT_EQ(arg10->data_type_.type(), LogicalType::kDate);
            ConstantExpr *arg11 = (ConstantExpr *)(*arg1->arguments_)[1];
            EXPECT_EQ(arg11->interval_type_, TimeUnit::kDay);
            EXPECT_EQ(arg11->integer_value_, 15);
        }

        result->Reset();
    }
}

TEST_F(SelectStatementParsingTest, good_test4) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "SELECT * FROM t where a = cast ('2023-01-01' as date) - INTERVAL 15 DAYS;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        {
            SelectStatement *select = (SelectStatement *)((*result->statements_ptr_)[0]);
            FunctionExpr *func_expr = (FunctionExpr *)select->where_expr_;
            EXPECT_EQ(func_expr->func_name_, "=");
            ColumnExpr *arg0 = (ColumnExpr *)(*func_expr->arguments_)[0];
            EXPECT_EQ(arg0->names_[0], "a");

            FunctionExpr *arg1 = (FunctionExpr *)(*func_expr->arguments_)[1];
            EXPECT_EQ(arg1->func_name_, "-");
            CastExpr *arg10 = (CastExpr *)(*arg1->arguments_)[0];
            EXPECT_EQ(arg10->data_type_.type(), LogicalType::kDate);
            ConstantExpr *arg11 = (ConstantExpr *)(*arg1->arguments_)[1];
            EXPECT_EQ(arg11->interval_type_, TimeUnit::kDay);
            EXPECT_EQ(arg11->integer_value_, 15);
        }

        result->Reset();
    }
}

TEST_F(SelectStatementParsingTest, bad_test1) {

    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        // bad dimension type double not match hamming
        String input_sql = "SELECT b FROM t1 WHERE a > 0 OFFSET 3;";
        parser->Parse(input_sql, result.get());
        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
    }
}

TEST_F(SelectStatementParsingTest, bad_knn_test) {

    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        // bad dimension type double not match hamming
        String input_sql = "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1.0, 2.0], 'double', 'hamming', 3) WHERE a > 0;";
        parser->Parse(input_sql, result.get());
        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
    }

    {
        // bit which length should be aligned with 8
        String input_sql = "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1,0,1,0,1,1,0], 'bit', 'hamming', 3) WHERE a > 0;";
        parser->Parse(input_sql, result.get());
        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
    }

    {
        // bit only support hamming
        String input_sql = "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1,0,1,0,1,1,0,0,1,0,1,0,1,1,0,0], 'bit', 'cosine', 3) WHERE a > 0;";
        parser->Parse(input_sql, result.get());
        std::cout << result->error_message_ << std::endl;
        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
    }
}

TEST_F(SelectStatementParsingTest, bad_search_test) {

    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        // bad dimension type double not match hamming
        String input_sql = "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1.0, 2.0], 'double', 'hamming', 3) WHERE a > 0 ORDER BY c2;";
        parser->Parse(input_sql, result.get());
        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
    }

    {
        // bit which length should be aligned with 8
        String input_sql = "SELECT b FROM t1 SEARCH MATCH VECTOR (c1, [1,0,1,0,1,1,0], 'bit', 'hamming', 3) WHERE a > 0 LIMIT 5;";
        parser->Parse(input_sql, result.get());
        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
    }

    {
        // bit only support hamming
        String input_sql = "SELECT b FROM t1 SEARCH MATCH TEXT ('author^2,name^5', 'frank dune') LIMIT 5;";
        parser->Parse(input_sql, result.get());
        std::cout << result->error_message_ << std::endl;
        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
    }
}

TEST_F(SelectStatementParsingTest, good_search_test) {

    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // integer with l2
    String input_sql = R"##(
    SELECT *
    FROM t1
    SEARCH
        MATCH TEXT ('author^2,name^5', 'frank dune'),
        MATCH TEXT ('name', 'to the star', 'operator=OR;fuzziness=AUTO:1,5;minimum_should_match=1'),
        QUERY('name:dune'),
        QUERY('_exists_:"author" AND page_count:>200 AND (name:/star./ OR name:duna~)', 'default_operator=and;default_field=name'),
        MATCH VECTOR (c1, [1, 2], 'integer', 'l2', 3),
        MATCH VECTOR (c1, [3, 10, 1111], 'bigint', 'cosine', 3),
        MATCH VECTOR (c1, [1.0, 2.0], 'double', 'cosine', 3),
        MATCH VECTOR (c1, [1.00, 2.00], 'double', 'ip', 3),
        MATCH VECTOR (c1, [1,0,1,0,1,1,0,0], 'bit', 'hamming', 3),
        MATCH VECTOR (c1, [1.00, 2.00], 'float', 'ip', 3),
        FUSION('rrf', 'rank_constant=60')
    WHERE a > 0
    )##";
    parser->Parse(input_sql, result.get());

    EXPECT_TRUE(result->error_message_.empty());
    EXPECT_TRUE(result->statements_ptr_ != nullptr);
    EXPECT_TRUE(result->statements_ptr_->size() == 1);
    auto &statement = result->statements_ptr_->at(0);

    EXPECT_EQ(statement->type_, StatementType::kSelect);
    auto *select_statement = (SelectStatement *)(statement);
    EXPECT_NE(select_statement->search_expr_, nullptr);

    EXPECT_EQ(select_statement->search_expr_->type_, ParsedExprType::kSearch);
    auto *search_expr = (SearchExpr *)(select_statement->search_expr_);

    EXPECT_EQ(search_expr->match_exprs_.size(), 4u);
    auto *match_expr0 = (MatchExpr *)(search_expr->match_exprs_[0]);
    EXPECT_EQ(match_expr0->fields_, String("author^2,name^5"));
    EXPECT_EQ(match_expr0->matching_text_, String("frank dune"));
    EXPECT_EQ(match_expr0->options_text_.empty(), true);
    auto *match_expr1 = (MatchExpr *)(search_expr->match_exprs_[1]);
    EXPECT_EQ(match_expr1->fields_, String("name"));
    EXPECT_EQ(match_expr1->matching_text_, String("to the star"));

    auto *query_expr0 = (MatchExpr *)(search_expr->match_exprs_[2]);
    EXPECT_EQ(query_expr0->matching_text_, String("name:dune"));
    EXPECT_EQ(query_expr0->options_text_.empty(), true);
    auto *query_expr1 = (MatchExpr *)(search_expr->match_exprs_[3]);
    EXPECT_EQ(query_expr1->matching_text_, String(R"##(_exists_:"author" AND page_count:>200 AND (name:/star./ OR name:duna~))##"));

    EXPECT_EQ(search_expr->knn_exprs_.size(), 6u);
    auto *knn_expr0 = (KnnExpr *)(search_expr->knn_exprs_[0]);
    EXPECT_EQ(knn_expr0->distance_type_, KnnDistanceType::kL2);
    auto *knn0_col_expr = (ColumnExpr *)(knn_expr0->column_expr_);
    EXPECT_EQ(knn0_col_expr->names_[0], "c1");
    EXPECT_EQ(knn_expr0->column_expr_->type_, ParsedExprType::kColumn);
    EXPECT_EQ(knn_expr0->dimension_, 2);
    EXPECT_EQ(knn_expr0->embedding_data_type_, EmbeddingDataType::kElemInt32);
    Vector<i32> vec0 = {1, 2};
    for (long i = 0; i < knn_expr0->dimension_; ++i) {
        EXPECT_EQ(((i32 *)knn_expr0->embedding_data_ptr_)[i], vec0[i]);
    }
    EXPECT_EQ(knn_expr0->topn_, 3);

    auto *knn_expr1 = (KnnExpr *)(search_expr->knn_exprs_[1]);
    EXPECT_EQ(knn_expr1->distance_type_, KnnDistanceType::kCosine);
    auto *knn1_col_expr = (ColumnExpr *)(knn_expr1->column_expr_);
    EXPECT_EQ(knn1_col_expr->names_[0], "c1");
    EXPECT_EQ(knn_expr1->column_expr_->type_, ParsedExprType::kColumn);
    EXPECT_EQ(knn_expr1->dimension_, 3);
    EXPECT_EQ(knn_expr1->embedding_data_type_, EmbeddingDataType::kElemInt64);
    Vector<i64> vec1 = {3, 10, 1111};
    for (long i = 0; i < knn_expr1->dimension_; ++i) {
        EXPECT_EQ(((i64 *)knn_expr1->embedding_data_ptr_)[i], vec1[i]);
    }
    EXPECT_EQ(knn_expr1->topn_, 3);

    auto *knn_expr2 = (KnnExpr *)(search_expr->knn_exprs_[2]);
    EXPECT_EQ(knn_expr2->distance_type_, KnnDistanceType::kCosine);
    auto *knn2_col_expr = (ColumnExpr *)(knn_expr2->column_expr_);
    EXPECT_EQ(knn2_col_expr->names_[0], "c1");
    EXPECT_EQ(knn_expr2->column_expr_->type_, ParsedExprType::kColumn);
    EXPECT_EQ(knn_expr2->dimension_, 2);
    EXPECT_EQ(knn_expr2->embedding_data_type_, EmbeddingDataType::kElemDouble);
    Vector<f64> vec2 = {1.00, 2.00};
    for (long i = 0; i < knn_expr2->dimension_; ++i) {
        EXPECT_EQ(((f64 *)knn_expr2->embedding_data_ptr_)[i], vec2[i]);
    }
    EXPECT_EQ(knn_expr2->topn_, 3);

    auto *knn_expr3 = (KnnExpr *)(search_expr->knn_exprs_[3]);
    EXPECT_EQ(knn_expr3->distance_type_, KnnDistanceType::kInnerProduct);
    auto *knn3_col_expr = (ColumnExpr *)(knn_expr3->column_expr_);
    EXPECT_EQ(knn3_col_expr->names_[0], "c1");
    EXPECT_EQ(knn_expr3->column_expr_->type_, ParsedExprType::kColumn);
    EXPECT_EQ(knn_expr3->dimension_, 2);
    EXPECT_EQ(knn_expr3->embedding_data_type_, EmbeddingDataType::kElemDouble);
    Vector<f64> vec3 = {1.00, 2.00};
    for (long i = 0; i < knn_expr3->dimension_; ++i) {
        EXPECT_EQ(((f64 *)knn_expr3->embedding_data_ptr_)[i], vec3[i]);
    }
    EXPECT_EQ(knn_expr3->topn_, 3);

    auto *knn_expr4 = (KnnExpr *)(search_expr->knn_exprs_[4]);
    EXPECT_EQ(knn_expr4->distance_type_, KnnDistanceType::kHamming);
    auto *knn4_col_expr = (ColumnExpr *)(knn_expr0->column_expr_);
    EXPECT_EQ(knn4_col_expr->names_[0], "c1");
    EXPECT_EQ(knn_expr4->column_expr_->type_, ParsedExprType::kColumn);
    EXPECT_EQ(knn_expr4->dimension_, 8);
    EXPECT_EQ(knn_expr4->embedding_data_type_, EmbeddingDataType::kElemBit);
    // [1,0,1,0,1,1,0,0], 8
    long embedding_size = knn_expr4->dimension_ / 8;
    Vector<i64> vec4 = {1, 0, 1, 0, 1, 1, 0, 0};
    for (long i = 0; i < embedding_size; ++i) {
        uint8_t embedding_unit = 0;
        for (long bit_idx = 0; bit_idx < 8; ++bit_idx) {
            if (vec4[i * 8 + bit_idx] == 1) {
                embedding_unit |= (uint8_t(1) << bit_idx);
            } else if (vec4[i * 8 + bit_idx] == 0) {
                // no-op
            } else {
                UnrecoverableError("bit value should be 0 or 1");
            }
        }
        EXPECT_EQ(((char *)knn_expr4->embedding_data_ptr_)[i], static_cast<char>(embedding_unit));
    }
    EXPECT_EQ(knn_expr4->topn_, 3);

    auto *knn_expr5 = (KnnExpr *)(search_expr->knn_exprs_[5]);
    EXPECT_EQ(knn_expr5->distance_type_, KnnDistanceType::kInnerProduct);
    auto *knn5_col_expr = (ColumnExpr *)(knn_expr5->column_expr_);
    EXPECT_EQ(knn5_col_expr->names_[0], "c1");
    EXPECT_EQ(knn_expr5->column_expr_->type_, ParsedExprType::kColumn);
    EXPECT_EQ(knn_expr5->dimension_, 2);
    EXPECT_EQ(knn_expr5->embedding_data_type_, EmbeddingDataType::kElemFloat);
    Vector<f32> vec5 = {1.00, 2.00};
    for (long i = 0; i < knn_expr5->dimension_; ++i) {
        EXPECT_EQ(((f32 *)knn_expr5->embedding_data_ptr_)[i], vec5[i]);
    }
    EXPECT_EQ(knn_expr5->topn_, 3);

    EXPECT_NE(search_expr->fusion_expr_, nullptr);
    auto *fusion_expr = search_expr->fusion_expr_;
    EXPECT_EQ(fusion_expr->method_, String("rrf"));

    EXPECT_NE(select_statement->where_expr_, nullptr);
    EXPECT_EQ(select_statement->where_expr_->type_, ParsedExprType::kFunction);

    result->Reset();
}