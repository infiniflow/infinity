//
// Created by jinhai on 23-2-26.
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

class SQLParserTest : public BaseTest {
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

TEST_F(SQLParserTest, good_test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

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

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    for (const String &input : inputs) {
        parser->Parse(input, result);
        std::cout << result->ToString() << std::endl;
        result->Reset();
    }
}

TEST_F(SQLParserTest, good_test2) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
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
                           "                 s path, "
                           "                 t polygon, "
                           "                 u circle, "
                           "                 v char(10), "
                           "                 w varchar(100), "
                           "                 x decimal, "
                           "                 y decimal(10), "
                           "                 z decimal(14, 12), "
                           "                 aa blob(10), "
                           "                 ab bitmap(16), "
                           "                 ac embedding(bit, 256), "
                           "                 ad vector(float, 512), "
                           "                 primary key (a, b), "
                           "                 unique (c, d), "
                           "                 ae int not null, "
                           "                 af embedding(int, 32)); ";

        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        for (auto &statement : *result->statements_ptr_) {
            EXPECT_EQ(statement->type_, StatementType::kCreate);
            auto *create_statement = (CreateStatement *)(statement);
            EXPECT_EQ(create_statement->create_info_->type_, DDLType::kTable);
            EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kError);

            auto *create_table_info = (CreateTableInfo *)(create_statement->create_info_.get());
            EXPECT_EQ(create_table_info->schema_name_, String("default"));
            EXPECT_EQ(create_table_info->table_name_, String("t1"));
            EXPECT_EQ(create_table_info->column_defs_.size(), 32);

            {
                auto &column_def = create_table_info->column_defs_[0];
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
                auto &column_def = create_table_info->column_defs_[1];
                EXPECT_EQ(column_def->name_, "b");
                DataType column_type(LogicalType::kTinyInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 3);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[2];
                EXPECT_EQ(column_def->name_, "c");
                DataType column_type(LogicalType::kSmallInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[3];
                EXPECT_EQ(column_def->name_, "d");
                DataType column_type(LogicalType::kInteger, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[4];
                EXPECT_EQ(column_def->name_, "e");
                DataType column_type(LogicalType::kBigInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[5];
                EXPECT_EQ(column_def->name_, "f");
                DataType column_type(LogicalType::kHugeInt, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[6];
                EXPECT_EQ(column_def->name_, "g");
                DataType column_type(LogicalType::kFloat, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[7];
                EXPECT_EQ(column_def->name_, "h");
                DataType column_type(LogicalType::kDouble, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[8];
                EXPECT_EQ(column_def->name_, "i");
                DataType column_type(LogicalType::kFloat, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[9];
                EXPECT_EQ(column_def->name_, "j");
                DataType column_type(LogicalType::kDate, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[10];
                EXPECT_EQ(column_def->name_, "k");
                DataType column_type(LogicalType::kTime, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 3);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[11];
                EXPECT_EQ(column_def->name_, "l");
                DataType column_type(LogicalType::kDateTime, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 3);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kPrimaryKey), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[12];
                EXPECT_EQ(column_def->name_, "m");
                DataType column_type(LogicalType::kTimestamp, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[13];
                EXPECT_EQ(column_def->name_, "n");
                DataType column_type(LogicalType::kUuid, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 2);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNull), true);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kUnique), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[14];
                EXPECT_EQ(column_def->name_, "o");
                DataType column_type(LogicalType::kPoint, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[15];
                EXPECT_EQ(column_def->name_, "p");
                DataType column_type(LogicalType::kLine, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[16];
                EXPECT_EQ(column_def->name_, "q");
                DataType column_type(LogicalType::kLineSeg, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[17];
                EXPECT_EQ(column_def->name_, "r");
                DataType column_type(LogicalType::kBox, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[18];
                EXPECT_EQ(column_def->name_, "s");
                DataType column_type(LogicalType::kPath, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[19];
                EXPECT_EQ(column_def->name_, "t");
                DataType column_type(LogicalType::kPolygon, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[20];
                EXPECT_EQ(column_def->name_, "u");
                DataType column_type(LogicalType::kCircle, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[22];
                EXPECT_EQ(column_def->name_, "w");
                DataType column_type(LogicalType::kVarchar, VarcharInfo::Make(100));
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[23];
                EXPECT_EQ(column_def->name_, "x");
                SharedPtr<TypeInfo> type_info = DecimalInfo::Make(0, 0);
                DataType column_type(LogicalType::kDecimal, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[24];
                EXPECT_EQ(column_def->name_, "y");
                SharedPtr<TypeInfo> type_info = DecimalInfo::Make(10, 0);
                DataType column_type(LogicalType::kDecimal, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[25];
                EXPECT_EQ(column_def->name_, "z");
                SharedPtr<TypeInfo> type_info = DecimalInfo::Make(14, 12);
                DataType column_type(LogicalType::kDecimal, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[26];
                EXPECT_EQ(column_def->name_, "aa");
                DataType column_type(LogicalType::kBlob);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[27];
                EXPECT_EQ(column_def->name_, "ab");
                SharedPtr<TypeInfo> type_info = BitmapInfo::Make(16);
                DataType column_type(LogicalType::kBitmap, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[28];
                EXPECT_EQ(column_def->name_, "ac");
                SharedPtr<TypeInfo> type_info = EmbeddingInfo::Make(kElemBit, 256);
                DataType column_type(LogicalType::kEmbedding, type_info);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 0);
            }

            {
                auto &column_def = create_table_info->column_defs_[29];
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
                auto &column_def = create_table_info->column_defs_[30];
                EXPECT_EQ(column_def->name_, "ae");
                DataType column_type(LogicalType::kInteger, nullptr);
                EXPECT_EQ(*column_def->column_type_, column_type);
                EXPECT_EQ(column_def->constraints_.size(), 1);
                EXPECT_EQ(column_def->constraints_.contains(ConstraintType::kNotNull), true);
            }

            {
                auto &column_def = create_table_info->column_defs_[31];
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
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "create table t1 (a boolean primary key not null null unique,"
                           "                 b tinyint not null null unique,";
        parser->Parse(input_sql, result);

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);

        result->Reset();
    }

    {
        String input_sql = "create table t1 (a boolean primary key not null null unique,"
                           "                 b tinyint not null null unique "
                           "                 c smallint unique);";
        parser->Parse(input_sql, result);

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);

        result->Reset();
    }

    {
        String input_sql = "create table t1 t2 (a boolean primary key not null null unique);";
        parser->Parse(input_sql, result);

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);

        result->Reset();
    }
}

TEST_F(SQLParserTest, good_create_index_1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    auto parser = MakeShared<SQLParser>();
    auto result = MakeShared<ParserResult>();

    {
        String input_sql = "CREATE INDEX ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kInvalid);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");
        EXPECT_EQ(create_index_info->method_type_, "IVFFlat");
        EXPECT_EQ(create_index_info->column_names_->size(), 1);
        EXPECT_EQ((*create_index_info->column_names_)[0], "a");
        EXPECT_TRUE(create_index_info->index_para_list_->empty());
        result->Reset();
    }
    {
        String input_sql = "CREATE INDEX idx1 ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result);

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
        EXPECT_EQ(create_index_info->method_type_, "IVFFlat");
        EXPECT_EQ(create_index_info->column_names_->size(), 1);
        EXPECT_EQ((*create_index_info->column_names_)[0], "a");
        EXPECT_TRUE(create_index_info->index_para_list_->empty());
        result->Reset();
    }
    {
        String input_sql = "CREATE INDEX IF NOT EXISTS idx1 ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result);

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
        EXPECT_EQ(create_index_info->method_type_, "IVFFlat");
        EXPECT_EQ(create_index_info->column_names_->size(), 1);
        EXPECT_EQ((*create_index_info->column_names_)[0], "a");
        EXPECT_TRUE(create_index_info->index_para_list_->empty());
        result->Reset();
    }
    {
        String input_sql = "CREATE INDEX ON db1.t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kInvalid);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "");
        EXPECT_EQ(create_index_info->schema_name_, "db1");
        EXPECT_EQ(create_index_info->table_name_, "t1");
        EXPECT_EQ(create_index_info->method_type_, "IVFFlat");
        EXPECT_EQ(create_index_info->column_names_->size(), 1);
        EXPECT_EQ((*create_index_info->column_names_)[0], "a");
        EXPECT_TRUE(create_index_info->index_para_list_->empty());
        result->Reset();
    }
    {
        String input_sql = "CREATE INDEX ON t1 (a, b) USING IVFFlat;";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kInvalid);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");
        EXPECT_EQ(create_index_info->method_type_, "IVFFlat");
        EXPECT_EQ(create_index_info->column_names_->size(), 2);
        EXPECT_EQ((*create_index_info->column_names_)[0], "a");
        EXPECT_EQ((*create_index_info->column_names_)[1], "b");
        EXPECT_TRUE(create_index_info->index_para_list_->empty());
        result->Reset();
    }
    {
        String input_sql = "CREATE INDEX ON t1 (a) USING IVFFlat WITH (metric = l2);";
        parser->Parse(input_sql, result);

        EXPECT_TRUE(result->error_message_.empty());
        BaseStatement *statement = (*result->statements_ptr_)[0];

        EXPECT_EQ(statement->type_, StatementType::kCreate);
        auto create_statement = static_cast<CreateStatement *>(statement);
        EXPECT_EQ(create_statement->create_info_->type_, DDLType::kIndex);
        EXPECT_EQ(create_statement->create_info_->conflict_type_, ConflictType::kInvalid);

        auto create_index_info = static_cast<CreateIndexInfo *>(create_statement->create_info_.get());
        EXPECT_EQ(create_index_info->index_name_, "");
        EXPECT_EQ(create_index_info->schema_name_, "default");
        EXPECT_EQ(create_index_info->table_name_, "t1");
        EXPECT_EQ(create_index_info->method_type_, "IVFFlat");
        EXPECT_EQ(create_index_info->column_names_->size(), 1);
        EXPECT_EQ((*create_index_info->column_names_)[0], "a");
        EXPECT_EQ(create_index_info->index_para_list_->size(), 1);
        EXPECT_EQ((*create_index_info->index_para_list_)[0]->para_name_, "metric");
        EXPECT_EQ((*create_index_info->index_para_list_)[0]->para_value_, "l2");
        result->Reset();
    }
}

TEST_F(SQLParserTest, bad_create_index_1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    {
        String input_sql = "CREATE INDEX IF NOT EXISTS ON t1 (a) USING IVFFlat;";
        parser->Parse(input_sql, result);

        EXPECT_FALSE(result->error_message_.empty());
        EXPECT_TRUE(result->statements_ptr_ == nullptr);
        result->Reset();
    }
}