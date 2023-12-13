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
import parser;
import logger;
import stl;
import infinity_context;

class SQLParserTest : public BaseTest {};

TEST_F(SQLParserTest, good_test1) {
    using namespace infinity;

    Vector<String> inputs;
    inputs.emplace_back("create collection c1;");
    inputs.emplace_back("create schema s1;");
    inputs.emplace_back("create collection if not exists c2;");
    inputs.emplace_back("create schema if not exists s2;");

    inputs.emplace_back("drop collection c1;");
    inputs.emplace_back("drop schema s1;");
    inputs.emplace_back("drop collection if exists c2;");
    inputs.emplace_back("drop schema if exists s2;");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv);");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv, header);");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv, header, delimiter '|');");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv);");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv, header);");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv, header, delimiter '|');");

    inputs.emplace_back("copy t2 from '/usr/filename' with (format xml);");
    inputs.emplace_back("copy t2 to '/usr/filename' with (format xml);");
    inputs.emplace_back("show tables;");
    inputs.emplace_back("describe t1;");
    inputs.emplace_back("describe s1.t1;");
    inputs.emplace_back("flush data;");
    inputs.emplace_back("flush log;");
    inputs.emplace_back("flush buffer;");
    inputs.emplace_back("optimize t1;");

    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'integer', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'integer', 'cosine') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'integer', 'ip') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");

    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'smallint', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'smallint', 'cosine') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'smallint', 'ip') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");

    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'bigint', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'bigint', 'cosine') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'bigint', 'ip') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");

    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'tinyint', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'tinyint', 'cosine') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1, 2], 2, 'tinyint', 'ip') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");

    inputs.emplace_back("SELECT KNN(c1, [1.0, 2.0], 2, 'float', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1.0, 2.0], 2, 'float', 'cosine') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1.0, 2.0], 2, 'float', 'ip') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");

    inputs.emplace_back(
        "SELECT KNN(c1, [1.222222222222, 2.11111111111111], 2, 'double', 'l2') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back(
        "SELECT KNN(c1, [1.222222222222, 2.11111111111111], 2, 'double', 'cosine') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back(
        "SELECT KNN(c1, [1.222222222222, 2.11111111111111], 2, 'double', 'ip') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");

    inputs.emplace_back("SELECT KNN(c1, [1,0,1,0,1,1,0,0], 8, 'bit', 'hamming') AS distance1 FROM t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");
    inputs.emplace_back("SELECT KNN(c1, [1,0,1,0,1,1,0,0,1,0,1,0,1,1,0,0,1,0,1,0,1,1,0,0,1,0,1,0,1,1,0,0], 32, 'bit', 'hamming') AS distance1 FROM "
                        "t1 WHERE a > 0 ORDER BY distance1 LIMIT 3;");

    inputs.emplace_back("DELETE FROM products");
    inputs.emplace_back("DELETE FROM products WHERE price = 10");
    inputs.emplace_back("delete from s1.products where obsoletion_date = 'today'");
    inputs.emplace_back("UPDATE products SET price = price * 1.10 WHERE price <= 99.99;");

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    for (const String &input : inputs) {
        parser->Parse(input, result.get());
        std::cout << result->ToString() << std::endl;
        result->Reset();
    }
}

TEST_F(SQLParserTest, good_test2) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "create table t1 (a boolean primary key not null null unique,"
                           "                 b tinyint not null null unique, "
                           "                 c smallint null not null, "
                           "                 d integer not null, "
                           "                 e bigint null,"
                           "                 f hugeint unique,"
                           "                 g float primary key,"
                           "                 h double primary key not null,"
                           "                 i real primary key null,"
                           "                 j date primary key unique,"
                           "                 k time primary key not null null,"
                           "                 l datetime primary key not null unique, "
                           "                 m timestamp not null unique, "
                           "                 n uuid null unique, "
                           "                 o point, "
                           "                 p line, "
                           "                 q lseg, "
                           "                 r box, "
//                           "                 s path, "
//                           "                 t polygon, "
                           "                 u circle, "
                           "                 v char(10), "
                           "                 w varchar, "
                           "                 x decimal, "
                           "                 y decimal(10), "
                           "                 z decimal(14, 12), "
//                           "                 aa blob(10), "
//                           "                 ab bitmap(16), "
                           "                 ac embedding(bit, 256), "
                           "                 ad vector(float, 512), "
                           "                 primary key (a, b), "
                           "                 unique (c, d), "
                           "                 ae int not null, "
                           "                 af embedding(int, 32)); ";

        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCreate);
            auto *create_statement = (CreateStatement *)(statement);
            EXPECT_EQ(create_statement->create_info_->type_, DDLType::kTable);
            EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kError);

            auto *create_table_info = (CreateTableInfo *)(create_statement->create_info_.get());
            EXPECT_EQ(create_table_info->schema_name_, String("default"));
            EXPECT_EQ(create_table_info->table_name_, String("t1"));
            EXPECT_EQ(create_table_info->column_defs_.size(), 28);

            u64 column_id = 0;
            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "a");
                DataType column_type(LogicalType::kBoolean, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 4);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "b");
                DataType column_type(LogicalType::kTinyInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 3);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "c");
                DataType column_type(LogicalType::kSmallInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "d");
                DataType column_type(LogicalType::kInteger, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "e");
                DataType column_type(LogicalType::kBigInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "f");
                DataType column_type(LogicalType::kHugeInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "g");
                DataType column_type(LogicalType::kFloat, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "h");
                DataType column_type(LogicalType::kDouble, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "i");
                DataType column_type(LogicalType::kFloat, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "j");
                DataType column_type(LogicalType::kDate, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "k");
                DataType column_type(LogicalType::kTime, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 3);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "l");
                DataType column_type(LogicalType::kDateTime, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 3);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "m");
                DataType column_type(LogicalType::kTimestamp, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "n");
                DataType column_type(LogicalType::kUuid, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "o");
                DataType column_type(LogicalType::kPoint, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "p");
                DataType column_type(LogicalType::kLine, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "q");
                DataType column_type(LogicalType::kLineSeg, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "r");
                DataType column_type(LogicalType::kBox, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

//            {
//                auto &column_def = create_table_info->column_defs_[column_id++];
//                EXPECT_EQ(column_def->name_, "s");
//                DataType column_type(LogicalType::kPath, nullptr);
//                EXPECT_EQ(*column_def->column_type_, column_type);
//                EXPECT_EQ(column_def->constraints_.size(), 0);
//            }
//
//            {
//                auto &column_def = create_table_info->column_defs_[column_id++];
//                EXPECT_EQ(column_def->name_, "t");
//                DataType column_type(LogicalType::kPolygon, nullptr);
//                EXPECT_EQ(*column_def->column_type_, column_type);
//                EXPECT_EQ(column_def->constraints_.size(), 0);
//            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "u");
                DataType column_type(LogicalType::kCircle, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "v");
                DataType column_type(LogicalType::kVarchar);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "w");
                DataType column_type(LogicalType::kVarchar);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "x");
                SharedPtr<TypeInfo> type_info = DecimalInfo::Make(0, 0);
                DataType column_type(LogicalType::kDecimal, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "y");
                SharedPtr<TypeInfo> type_info = DecimalInfo::Make(10, 0);
                DataType column_type(LogicalType::kDecimal, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "z");
                SharedPtr<TypeInfo> type_info = DecimalInfo::Make(14, 12);
                DataType column_type(LogicalType::kDecimal, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

//            {
//                auto &column_def = create_table_info->column_defs_[column_id++];
//                EXPECT_EQ(column_def->name_, "aa");
//                DataType column_type(LogicalType::kBlob);
//                EXPECT_EQ(*column_def->column_type_, column_type);
//                EXPECT_EQ(column_def->constraints_.size(), 0);
//            }
//
//            {
//                auto &column_def = create_table_info->column_defs_[column_id++];
//                EXPECT_EQ(column_def->name_, "ab");
//                SharedPtr<TypeInfo> type_info = BitmapInfo::Make(16);
//                DataType column_type(LogicalType::kBitmap, type_info);
//                EXPECT_EQ(*column_def->column_type_, column_type);
//                EXPECT_EQ(column_def->constraints_.size(), 0);
//            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "ac");
                SharedPtr<TypeInfo> type_info = EmbeddingInfo::Make(kElemBit, 256);
                DataType column_type(LogicalType::kEmbedding, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "ad");
                SharedPtr<TypeInfo> type_info = EmbeddingInfo::Make(kElemFloat, 512);
                DataType column_type(LogicalType::kEmbedding, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            EXPECT_EQ(create_table_info->constraints_.size(), 2);
            {
                EXPECT_EQ(create_table_info->constraints_[0]->constraint_, ConstraintType::kPrimaryKey);
                const String &column1 = (*(create_table_info->constraints_[0]->names_ptr_))[0];
                const String &column2 = (*(create_table_info->constraints_[0]->names_ptr_))[1];
                EXPECT_EQ(column1, "a");
                EXPECT_EQ(column2, "b");
            }

            {
                EXPECT_EQ(create_table_info->constraints_[1]->constraint_, ConstraintType::kUnique);
                const String &column3 = (*(create_table_info->constraints_[1]->names_ptr_))[0];
                const String &column4 = (*(create_table_info->constraints_[1]->names_ptr_))[1];
                EXPECT_EQ(column3, "c");
                EXPECT_EQ(column4, "d");
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "ae");
                DataType column_type(LogicalType::kInteger, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[column_id++];
                EXPECT_EQ(column_def->name_, "af");
                SharedPtr<TypeInfo> type_info = EmbeddingInfo::Make(kElemInt32, 32);
                DataType column_type(LogicalType::kEmbedding, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }
        }

        result->Reset();
    }
}

TEST_F(SQLParserTest, bad_test1) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "create table t1 (a boolean primary key not null null unique,"
                           "                 b tinyint not null null unique,";
        parser->Parse(input_sql, result.get());

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);

        result->Reset();
    }

    {
        String input_sql = "create table t1 (a boolean primary key not null null unique,"
                           "                 b tinyint not null null unique "
                           "                 c smallint unique);";
        parser->Parse(input_sql, result.get());

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);

        result->Reset();
    }

    {
        String input_sql = "create table t1 t2 (a boolean primary key not null null unique);";
        parser->Parse(input_sql, result.get());

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);

        result->Reset();
    }
}

TEST_F(SQLParserTest, good_create_index_1) {
    using namespace infinity;
    auto parser = MakeShared<SQLParser>();
    auto result = MakeShared<ParserResult>();

    {
        String input_sql = "CREATE INDEX ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result.get());

        EXPECT_FALSE(result->error_message_.empty());
//        BaseStatement *statement = (*result->statements_ptr_)[0];
//
//        EXPECT_EQ(statement->type_, StatementType::kCreate);
//        auto create_statement = static_cast<CreateStatement *>(statement);
//        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
//        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kIgnore);
//
//        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
//        EXPECT_EQ(create_index_info->index_name_, "");
//        EXPECT_EQ(create_index_info->schema_name_, "default");
//        EXPECT_EQ(create_index_info->table_name_, "t1");
//
//        Vector<IndexInfo *>& index_info_list = *(create_index_info->index_info_list_);
//        EXPECT_EQ(index_info_list.size(), 1);
//        IndexInfo * index_info = index_info_list[0];
//        EXPECT_EQ(index_info->index_type_, IndexType::kIVFFlat);
//        EXPECT_EQ(index_info->column_name_, "a");
//        EXPECT_TRUE(index_info->index_param_list_->empty());
//        EXPECT_EQ(IndexInfo::IndexTypeToString(index_info->index_type_), "IVFFlat");
//        EXPECT_EQ(IndexInfo::StringToIndexType("IVFFlat"), IndexType::kIVFFlat);

        result->Reset();
    }

    {
        String input_sql = "CREATE INDEX idx1 ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kError);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "idx1");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");

        Vector<IndexInfo *>& index_info_list = *(create_index_info->index_info_list_);
        EXPECT_EQ(index_info_list.size(), 1);
        IndexInfo * index_info = index_info_list[0];
        EXPECT_EQ(index_info->index_type_, IndexType::kIVFFlat);
        EXPECT_EQ(index_info->column_name_, "a");
        EXPECT_TRUE(index_info->index_param_list_->empty());
        EXPECT_EQ(IndexInfo::IndexTypeToString(index_info->index_type_), "IVFFlat");
        EXPECT_EQ(IndexInfo::StringToIndexType("IVFFlat"), IndexType::kIVFFlat);

        result->Reset();
    }

    {
        String input_sql = "CREATE INDEX IF NOT EXISTS idx1 ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kIgnore);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "idx1");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");

        Vector<IndexInfo *>& index_info_list = *(create_index_info->index_info_list_);
        EXPECT_EQ(index_info_list.size(), 1);
        IndexInfo * index_info = index_info_list[0];
        EXPECT_EQ(index_info->index_type_, IndexType::kIVFFlat);
        EXPECT_EQ(index_info->column_name_, "a");
        EXPECT_TRUE(index_info->index_param_list_->empty());
        EXPECT_EQ(IndexInfo::IndexTypeToString(index_info->index_type_), "IVFFlat");
        EXPECT_EQ(IndexInfo::StringToIndexType("IVFFlat"), IndexType::kIVFFlat);

        result->Reset();
    }

    {
        String input_sql = "CREATE INDEX idx1 ON db1.t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kError);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "idx1");
        EXPECT_EQ(create_index_info->schema_name_, "db1");
        EXPECT_EQ(create_index_info->table_name_, "t1");

        Vector<IndexInfo *>& index_info_list = *(create_index_info->index_info_list_);
        EXPECT_EQ(index_info_list.size(), 1);
        IndexInfo * index_info = index_info_list[0];
        EXPECT_EQ(index_info->index_type_, IndexType::kIVFFlat);
        EXPECT_EQ(index_info->column_name_, "a");
        EXPECT_TRUE(index_info->index_param_list_->empty());
        EXPECT_EQ(IndexInfo::IndexTypeToString(index_info->index_type_), "IVFFlat");
        EXPECT_EQ(IndexInfo::StringToIndexType("IVFFlat"), IndexType::kIVFFlat);

        result->Reset();
    }

    {
        String input_sql = "CREATE INDEX idx2 ON t1 (a, b) USING IVFFlat;";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kError);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "idx2");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");

        Vector<IndexInfo *>& index_info_list = *(create_index_info->index_info_list_);
        EXPECT_EQ(index_info_list.size(), 2);
        IndexInfo * index_info1 = index_info_list[0];
        EXPECT_EQ(index_info1->index_type_, IndexType::kIVFFlat);
        EXPECT_EQ(index_info1->column_name_, "a");
        EXPECT_TRUE(index_info1->index_param_list_->empty());
        EXPECT_EQ(IndexInfo::IndexTypeToString(index_info1->index_type_), "IVFFlat");
        EXPECT_EQ(IndexInfo::StringToIndexType("IVFFlat"), IndexType::kIVFFlat);

        IndexInfo * index_info2 = index_info_list[1];
        EXPECT_EQ(index_info2->index_type_, IndexType::kIVFFlat);
        EXPECT_EQ(index_info2->column_name_, "b");
        EXPECT_TRUE(index_info2->index_param_list_->empty());
        EXPECT_EQ(IndexInfo::IndexTypeToString(index_info2->index_type_), "IVFFlat");

        result->Reset();
    }

    {
        String input_sql = "CREATE INDEX idx3 ON t1 (a) USING IVFFlat WITH (metric = l2);";
        parser->Parse(input_sql, result.get());

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kError);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "idx3");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");

        Vector<IndexInfo *>& index_info_list = *(create_index_info->index_info_list_);
        EXPECT_EQ(index_info_list.size(), 1);
        IndexInfo * index_info = index_info_list[0];
        EXPECT_EQ(index_info->index_type_, IndexType::kIVFFlat);
        EXPECT_EQ(index_info->column_name_, "a");
        EXPECT_EQ(index_info->index_param_list_->size(), 1);
        EXPECT_EQ((*index_info->index_param_list_)[0]->param_name_, "metric");
        EXPECT_EQ((*index_info->index_param_list_)[0]->param_value_, "l2");

        EXPECT_EQ(IndexInfo::IndexTypeToString(index_info->index_type_), "IVFFlat");
        EXPECT_EQ(IndexInfo::StringToIndexType("IVFFlat"), IndexType::kIVFFlat);

        result->Reset();
    }
}

TEST_F(SQLParserTest, bad_create_index_1) {
    using namespace infinity;
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "CREATE INDEX IF NOT EXISTS ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result.get());

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
        result->Reset();
    }
}
