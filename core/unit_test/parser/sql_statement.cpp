//
// Created by jinhai on 23-3-5.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity;

class StatementParsingTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(StatementParsingTest, good_test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "CREATE TABLE t1 AS SELECT a, b FROM t2;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCreate);
            auto *create_statement = (CreateStatement *)(statement);
            EXPECT_EQ(create_statement->create_info_->type_, DDLType::kTable);
            CreateTableInfo *table_info = (CreateTableInfo *)create_statement->create_info_.get();
            EXPECT_EQ(table_info->table_name_, "t1");
            EXPECT_EQ(table_info->schema_name_, "default");
            auto *select = (SelectStatement *)(table_info->select_);
            {
                ColumnExpr *col0 = (ColumnExpr *)(*select->select_list_)[0];
                ColumnExpr *col1 = (ColumnExpr *)(*select->select_list_)[1];
                EXPECT_EQ(col0->names_[0], "a");
                EXPECT_EQ(col1->names_[0], "b");

                EXPECT_EQ(select->table_ref_->type_, TableRefType::kTable);
                auto *from_table = (TableReference *)select->table_ref_;
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

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kUpdate);
            auto *update_statement = (UpdateStatement *)(statement);
            EXPECT_EQ(update_statement->table_name_, "t1");
            EXPECT_EQ(update_statement->schema_name_, "default");
            FunctionExpr *where_func = (FunctionExpr *)(update_statement->where_expr_);
            EXPECT_EQ(where_func->func_name_, "=");
            ColumnExpr *where_left_expr = (ColumnExpr *)(*where_func->arguments_)[0];
            ConstantExpr *where_right_expr = (ConstantExpr *)(*where_func->arguments_)[1];
            EXPECT_EQ(where_left_expr->names_[0], "c");
            EXPECT_STREQ(where_right_expr->str_value_, "O'K");

            UpdateExpr *up0_expr = (*update_statement->update_expr_array_)[0];
            EXPECT_EQ(up0_expr->column_name, "a");
            EXPECT_EQ(((ConstantExpr *)(up0_expr->value))->integer_value_, 1);
            UpdateExpr *up1_expr = (*update_statement->update_expr_array_)[1];
            EXPECT_EQ(up1_expr->column_name, "b");
            EXPECT_EQ(((ConstantExpr *)(up1_expr->value))->integer_value_, 2);
        }

        result->Reset();
    }

    {
        String input_sql = "INSERT INTO t1 VALUES ('abc', 333);";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kInsert);
            auto *insert_statement = (InsertStatement *)(statement);
            EXPECT_EQ(insert_statement->table_name_, "t1");
            EXPECT_EQ(insert_statement->schema_name_, "default");
            EXPECT_EQ(insert_statement->values_->size(), 1);

            ConstantExpr *insert0_expr = (ConstantExpr *)(*insert_statement->values_->at(0))[0];
            EXPECT_STREQ(insert0_expr->str_value_, "abc");
            ConstantExpr *insert1_expr = (ConstantExpr *)(*insert_statement->values_->at(0))[1];
            EXPECT_EQ(insert1_expr->integer_value_, 333);
        }

        result->Reset();
    }

    {
        String input_sql = "INSERT INTO t1 VALUES ('abc', 333), ('def', 444);";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kInsert);
            auto *insert_statement = (InsertStatement *)(statement);
            EXPECT_EQ(insert_statement->table_name_, "t1");
            EXPECT_EQ(insert_statement->schema_name_, "default");
            EXPECT_EQ(insert_statement->values_->size(), 2);

            ConstantExpr *insert0_expr = (ConstantExpr *)(*insert_statement->values_->at(0))[0];
            EXPECT_STREQ(insert0_expr->str_value_, "abc");
            ConstantExpr *insert1_expr = (ConstantExpr *)(*insert_statement->values_->at(0))[1];
            EXPECT_EQ(insert1_expr->integer_value_, 333);

            ConstantExpr *insert2_expr = (ConstantExpr *)(*insert_statement->values_->at(1))[0];
            EXPECT_STREQ(insert2_expr->str_value_, "def");
            ConstantExpr *insert3_expr = (ConstantExpr *)(*insert_statement->values_->at(1))[1];
            EXPECT_EQ(insert3_expr->integer_value_, 444);
        }

        result->Reset();
    }

    {
        String input_sql = "CREATE INDEX IF NOT EXISTS idx1 ON t1 (c1, c2) USING IVFFlat WITH(metric = l2);";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        BaseStatement *statement = (*result->statements_ptr_)[0];
        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kIgnore);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "idx1");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");
        EXPECT_EQ(create_index_info->method_type_, "IVFFlat");
        EXPECT_EQ(create_index_info->column_names_->size(), 2);
        EXPECT_EQ((*create_index_info->column_names_)[0], "c1");
        EXPECT_EQ((*create_index_info->column_names_)[1], "c2");
        EXPECT_EQ(create_index_info->index_para_list_->size(), 1);
        EXPECT_EQ((*create_index_info->index_para_list_)[0]->para_name_, "metric");
        EXPECT_EQ((*create_index_info->index_para_list_)[0]->para_value_, "l2");

        result->Reset();
    }

    {
        String input_sql = "DROP INDEX index1;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kDrop);
            auto *drop_statement = (DropStatement *)(statement);
            EXPECT_EQ(drop_statement->drop_info_->type_, DDLType::kIndex);
            DropIndexInfo *drop_info = (DropIndexInfo *)drop_statement->drop_info_.get();
            EXPECT_EQ(drop_info->index_name_, "index1");
            EXPECT_EQ(drop_info->conflict_type_, ConflictType::kError);
        }

        result->Reset();
    }

    {
        String input_sql = "DROP TABLE t1;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kDrop);
            auto *drop_statement = (DropStatement *)(statement);
            EXPECT_EQ(drop_statement->drop_info_->type_, DDLType::kTable);
            DropTableInfo *drop_info = (DropTableInfo *)drop_statement->drop_info_.get();
            EXPECT_EQ(drop_info->table_name_, "t1");
            EXPECT_EQ(drop_info->schema_name_, "default");
            EXPECT_EQ(drop_info->conflict_type_, ConflictType::kError);
        }

        result->Reset();
    }

    {
        String input_sql = "SHOW TABLES;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kShow);
            auto *show_statement = (ShowStatement *)(statement);
            EXPECT_EQ(show_statement->show_type_, ShowStmtType::kTables);
            EXPECT_EQ(show_statement->table_name_, "");
            EXPECT_EQ(show_statement->schema_name_, "default");
        }

        result->Reset();
    }

    {
        String input_sql = "DESCRIBE t1;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kShow);
            auto *show_statement = (ShowStatement *)(statement);
            EXPECT_EQ(show_statement->show_type_, ShowStmtType::kColumns);
            EXPECT_EQ(show_statement->table_name_, "t1");
            EXPECT_EQ(show_statement->schema_name_, "default");
        }

        result->Reset();
    }

    {
        String input_sql = "copy t1 to '/usr/filename' with (format csv, header, delimiter '|');";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCopy);
            auto *copy_statement = (CopyStatement *)(statement);
            EXPECT_EQ(copy_statement->copy_file_type_, CopyFileType::kCSV);
            EXPECT_EQ(copy_statement->copy_from_, false);
            EXPECT_EQ(copy_statement->schema_name_, "default");
            EXPECT_EQ(copy_statement->table_name_, "t1");
            EXPECT_EQ(copy_statement->file_path_, "/usr/filename");
            EXPECT_EQ(copy_statement->header_, true);
            EXPECT_EQ(copy_statement->delimiter_, '|');
        }

        result->Reset();
    }

    {
        String input_sql = "copy t1 to '/usr/filename' with (format json, header, delimiter '|');";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCopy);
            auto *copy_statement = (CopyStatement *)(statement);
            EXPECT_EQ(copy_statement->copy_file_type_, CopyFileType::kJSON);
            EXPECT_EQ(copy_statement->copy_from_, false);
            EXPECT_EQ(copy_statement->schema_name_, "default");
            EXPECT_EQ(copy_statement->table_name_, "t1");
            EXPECT_EQ(copy_statement->file_path_, "/usr/filename");
            EXPECT_EQ(copy_statement->header_, true);
            EXPECT_EQ(copy_statement->delimiter_, '|');
        }

        result->Reset();
    }

    {
        String input_sql = "copy t2 from '/usr/filename' with (format csv, header, delimiter '|');";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCopy);
            auto *copy_statement = (CopyStatement *)(statement);
            EXPECT_EQ(copy_statement->copy_file_type_, CopyFileType::kCSV);
            EXPECT_EQ(copy_statement->copy_from_, true);
            EXPECT_EQ(copy_statement->schema_name_, "default");
            EXPECT_EQ(copy_statement->table_name_, "t2");
            EXPECT_EQ(copy_statement->file_path_, "/usr/filename");
            EXPECT_EQ(copy_statement->header_, true);
            EXPECT_EQ(copy_statement->delimiter_, '|');
        }
        result->Reset();
    }

    {
        String input_sql = "copy t2 from '/usr/filename' with (format json, header, delimiter '|');";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCopy);
            auto *copy_statement = (CopyStatement *)(statement);
            EXPECT_EQ(copy_statement->copy_file_type_, CopyFileType::kJSON);
            EXPECT_EQ(copy_statement->copy_from_, true);
            EXPECT_EQ(copy_statement->schema_name_, "default");
            EXPECT_EQ(copy_statement->table_name_, "t2");
            EXPECT_EQ(copy_statement->file_path_, "/usr/filename");
            EXPECT_EQ(copy_statement->header_, true);
            EXPECT_EQ(copy_statement->delimiter_, '|');
        }
        result->Reset();
    }

    {
        String input_sql = "select * from t1 except select * from t2;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->set_op_, SetOperatorType::kExcept);
            TableReference *t1 = (TableReference *)select_statement->table_ref_;
            EXPECT_EQ(t1->table_name_, "t1");
            EXPECT_EQ(t1->db_name_, "default");

            TableReference *t2 = (TableReference *)select_statement->nested_select_->table_ref_;
            EXPECT_EQ(t2->table_name_, "t2");
            EXPECT_EQ(t2->db_name_, "default");
        }
        result->Reset();
    }

    {
        String input_sql = "select * from t1 intersect select * from t2 union all select * from t3;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        EXPECT_FALSE(result->statements_ptr_ == nullptr);

        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kSelect);
            auto *select_statement = (SelectStatement *)(statement);
            EXPECT_EQ(select_statement->set_op_, SetOperatorType::kIntersect);
            TableReference *t1 = (TableReference *)select_statement->table_ref_;
            EXPECT_EQ(t1->table_name_, "t1");
            EXPECT_EQ(t1->db_name_, "default");

            TableReference *t2 = (TableReference *)select_statement->nested_select_->table_ref_;
            EXPECT_EQ(t2->table_name_, "t2");
            EXPECT_EQ(t2->db_name_, "default");

            EXPECT_EQ(select_statement->nested_select_->set_op_, SetOperatorType::kUnionAll);
            TableReference *t3 = (TableReference *)select_statement->nested_select_->nested_select_->table_ref_;
            EXPECT_EQ(t3->table_name_, "t3");
            EXPECT_EQ(t3->db_name_, "default");
        }
        result->Reset();
    }
}
