// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#include "gtest/gtest.h"
import base_test;

#include <chrono>
#include <definition/column_def.h>
import new_txn_manager;
import new_txn;
import status;
import infinity_context;
import stl;
import txn_state;
import infinity;
import search_expr;
import match_expr;
import function_expr;
import parsed_expr;
import query_options;
import logger;
import third_party;
import compilation_config;
import profiler;
import internal_types;
import logical_type;
import create_index_info;
import column_def;
import data_type;
import extra_ddl_info;
import statement_common;
import constant_expr;
import column_expr;
import virtual_store;
import insert_row_expr;
import embedding_info;
import compilation_config;
import global_resource_usage;
import expression_parser_result;
import expr_parser;
import update_statement;
import knn_expr;

using namespace infinity;

class ParallelTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         ParallelTest,
                         ::testing::Values(ParallelTest::NEW_CONFIG_PATH, ParallelTest::NEW_VFS_OFF_CONFIG_PATH));

constexpr SizeT kDataSize = 100000;
constexpr SizeT kRunningTime = 30;
constexpr SizeT kNumThread = 4;
constexpr SizeT data_size = 100000;
constexpr SizeT insert_delete_size = 100;

struct DataRow {
    int index_;
    String body_;
    Vector<double> others_;
};

SizeT RandInt(SizeT low, SizeT high) {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<SizeT> dist(low, high);
    return dist(gen);
}

Vector<DataRow> DataPreprocessing(const String &filepath) {
    Vector<std::string> texts;
    std::ifstream fin(filepath);
    if (!fin) {
        LOG_ERROR(fmt::format("Cannot open text data: {}", filepath));
    }
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream ss(line);
        std::string tmp;
        std::getline(ss, tmp, '\t');
        std::getline(ss, tmp, '\t');
        if (std::getline(ss, tmp, '\t')) {
            texts.push_back(tmp);
        }
    }
    fin.close();

    Vector<std::vector<double>> vectors = {{0.0, 0.0, 0.0, 0.0},
                                           {1.1, 1.1, 1.1, 1.1},
                                           {2.2, 2.2, 2.2, 2.2},
                                           {3.3, 3.3, 3.3, 3.3},
                                           {4.4, 4.4, 4.4, 4.4},
                                           {5.5, 5.5, 5.5, 5.5},
                                           {6.6, 6.6, 6.6, 6.6},
                                           {7.7, 7.7, 7.7, 7.7},
                                           {8.8, 8.8, 8.8, 8.8},
                                           {9.9, 9.9, 9.9, 9.9}};

    Vector<DataRow> data_rows;

    int base_size = std::min((int)texts.size(), (int)vectors.size());
    int repeat_times = kDataSize / base_size;

    data_rows.reserve(kDataSize);
    int index_counter = 0;

    for (int i = 0; i < repeat_times; ++i) {
        for (int j = 0; j < base_size; ++j) {
            DataRow row;
            row.index_ = index_counter++;
            row.body_ = texts[j % texts.size()];
            row.others_ = vectors[j % vectors.size()];
            data_rows.push_back(row);
        }
    }

    for (size_t i = 0; i < kDataSize - data_rows.size(); ++i) {
        DataRow row;
        row.index_ = index_counter++;
        row.body_ = texts[i % texts.size()];
        row.others_ = vectors[i % vectors.size()];
        data_rows.push_back(row);
    }

    return data_rows;
}

void FullTextSearch(const String &db_name, const String &table_name) {
    SharedPtr<Infinity> infinity = Infinity::LocalConnect();
    auto *search_expr = new SearchExpr();
    auto *exprs = new std::vector<ParsedExpr *>();
    auto *match_expr = new MatchExpr();

    match_expr->fields_ = "body^5";
    match_expr->matching_text_ = "harmful chemical";
    exprs->push_back(match_expr);
    search_expr->SetExprs(exprs);

    auto *output_columns = new std::vector<ParsedExpr *>();

    auto *select_rowid_expr = new FunctionExpr();
    select_rowid_expr->func_name_ = "row_id";

    auto *select_score_expr = new FunctionExpr();
    select_score_expr->func_name_ = "score";

    output_columns->emplace_back(select_rowid_expr);
    output_columns->emplace_back(select_score_expr);

    auto result =
        infinity->Search(db_name, table_name, search_expr, nullptr, nullptr, nullptr, output_columns, nullptr, nullptr, nullptr, nullptr, false);
    if (!result.IsOk()) {
        LOG_ERROR(fmt::format("Search failed: {}", result.ToString()));
        return;
    }
    infinity->LocalDisconnect();
}

void VectorSearch(const String &db_name, const String &table_name) {
    SharedPtr<Infinity> infinity = Infinity::LocalConnect();
    KnnExpr *knn_expr = new KnnExpr();
    knn_expr->dimension_ = 4;
    knn_expr->distance_type_ = KnnDistanceType::kL2;
    knn_expr->topn_ = 3;
    knn_expr->opt_params_ = new std::vector<InitParameter *>();
    knn_expr->embedding_data_type_ = EmbeddingDataType::kElemFloat;
    auto embedding_data_ptr = new float[4];
    knn_expr->embedding_data_ptr_ = embedding_data_ptr;
    std::unique_ptr<float[]> queries_ptr(new float[4]);
    auto src_ptr = queries_ptr.get();
    memmove(knn_expr->embedding_data_ptr_, src_ptr, 4 * sizeof(float));

    ColumnExpr *column_expr = new ColumnExpr();
    column_expr->names_.emplace_back("other_vector");
    knn_expr->column_expr_ = column_expr;
    std::vector<ParsedExpr *> *exprs = new std::vector<ParsedExpr *>();
    exprs->emplace_back(knn_expr);
    SearchExpr *search_expr = new SearchExpr();
    search_expr->SetExprs(exprs);

    std::vector<ParsedExpr *> *output_columns = new std::vector<ParsedExpr *>;
    auto select_rowid_expr = new FunctionExpr();
    select_rowid_expr->func_name_ = "row_id";
    output_columns->emplace_back(select_rowid_expr);
    auto result =
        infinity->Search(db_name, table_name, search_expr, nullptr, nullptr, nullptr, output_columns, nullptr, nullptr, nullptr, nullptr, false);
    infinity->LocalDisconnect();
}

void Insert(const String &db_name, const String &table_name, Vector<DataRow> data) {
    SharedPtr<Infinity> infinity = Infinity::LocalConnect();

    SizeT max_start = data.size() > insert_delete_size ? data.size() - insert_delete_size : 0;
    SizeT pos = RandInt(0, max_start);

    auto insert_rows = new Vector<InsertRowExpr *>();
    insert_rows->reserve(insert_delete_size);

    for (SizeT i = 0; i < insert_delete_size && (pos + i) < data.size(); ++i) {
        const auto &row = data[pos + i];
        auto insert_row = MakeUnique<InsertRowExpr>();
        insert_row->columns_ = {"index", "body", "other_vector"};

        auto index_expr = MakeUnique<ConstantExpr>(LiteralType::kInteger);
        index_expr->integer_value_ = row.index_;
        insert_row->values_.emplace_back(std::move(index_expr));

        auto body_expr = MakeUnique<ConstantExpr>(LiteralType::kString);
        body_expr->str_value_ = const_cast<char *>(row.body_.c_str());
        insert_row->values_.emplace_back(std::move(body_expr));

        auto vector_expr = MakeUnique<ConstantExpr>(LiteralType::kDoubleArray);
        vector_expr->double_array_ = row.others_;
        insert_row->values_.emplace_back(std::move(vector_expr));

        insert_rows->emplace_back(insert_row.release());
    }

    auto result = infinity->Insert(db_name, table_name, insert_rows);
    if (result.IsOk()) {
        result = infinity->Flush();
    } else {
        LOG_ERROR(fmt::format("Insert failed: {}", result.ToString()));
        return;
    }

    infinity->LocalDisconnect();
}

void Delete(const String &db_name, const String &table_name) {
    SharedPtr<Infinity> infinity = Infinity::LocalConnect();
    SizeT pos = RandInt(0, data_size / insert_delete_size - 1);
    pos = pos * insert_delete_size;

    std::string filter = fmt::format("index >= {} and index < {}", pos, pos + insert_delete_size);

    auto expr_parsed_result = MakeUnique<ExpressionParserResult>();
    ExprParser expr_parser;
    expr_parser.Parse(filter, expr_parsed_result.get());

    if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() != 1) {
        LOG_ERROR(fmt::format("Invalid filter expression: {}", filter));
        return;
    }

    auto result = infinity->Delete(db_name, table_name, expr_parsed_result->exprs_ptr_->at(0));
    if (!result.IsOk()) {
        LOG_ERROR(fmt::format("Delete failed: {}", result.ToString()));
    }
    expr_parsed_result->exprs_ptr_->at(0) = nullptr;
    infinity->LocalDisconnect();
}

void Update(const String &db_name, const String &table_name) {
    SharedPtr<Infinity> infinity = Infinity::LocalConnect();
    SizeT pos = RandInt(0, data_size - 1);

    std::string filter = fmt::format("index = {}", pos);

    auto expr_parsed_result = MakeUnique<ExpressionParserResult>();
    ExprParser expr_parser;
    expr_parser.Parse(filter, expr_parsed_result.get());

    if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() != 1) {
        LOG_ERROR(fmt::format("Invalid filter expression: {}", filter));
        return;
    }

    Vector<UpdateExpr *> *values = new Vector<UpdateExpr *>();

    Vector<String> *columns = new Vector<String>();
    columns->emplace_back("index");
    columns->emplace_back("body");
    columns->emplace_back("other_vector");

    auto *value1 = new ConstantExpr(LiteralType::kInteger);
    value1->integer_value_ = pos;
    UpdateExpr *update_expr1 = new UpdateExpr();
    update_expr1->column_name = "index";
    update_expr1->value = value1;

    auto *value2 = new ConstantExpr(LiteralType::kString);
    value2->str_value_ = const_cast<char *>("infinity");
    UpdateExpr *update_expr2 = new UpdateExpr();
    update_expr2->column_name = "body";
    update_expr2->value = value2;

    auto *value3 = new ConstantExpr(LiteralType::kDoubleArray);
    value3->double_array_ = {0.0, 0.0, 0.0, 0.0};
    UpdateExpr *update_expr3 = new UpdateExpr();
    update_expr3->column_name = "other_vector";
    update_expr3->value = value3;

    values->push_back(update_expr1);
    values->push_back(update_expr2);
    values->push_back(update_expr3);

    auto result = infinity->Update(db_name, table_name, expr_parsed_result->exprs_ptr_->at(0), values);
    if (!result.IsOk()) {
        LOG_ERROR(fmt::format("Update failed: {}", result.ToString()));
    }

    expr_parsed_result->exprs_ptr_->at(0) = nullptr;
    infinity->LocalDisconnect();
}

void ChaosTestExecution(const String &db_name, const String &table_name) {
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(kRunningTime)) {
        int rand_op = RandInt(0, 3);

        switch (rand_op) {
            case 0:
                Delete(db_name, table_name);
                break;
            case 1:
                VectorSearch(db_name, table_name);
                break;
            case 2:
                // Update(db_name, table_name);
                break;
            default:
                FullTextSearch(db_name, table_name);
                break;
        }
    }
}

void RunChaosTestInParallel(const String &db_name, const String &table_name) {
    std::vector<std::thread> threads;

    for (SizeT i = 0; i < kNumThread; ++i) {
        threads.emplace_back([&]() { ChaosTestExecution(db_name, table_name); });
    }

    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

TEST_P(ParallelTest, ChaosTest) {
    auto db_name = "default_db";
    auto table_name = "chaos_test";

    String data_path = GetHomeDir();

    String fulltext_file_path = String(test_data_path()) + "/csv/enwiki_99.csv";

    auto data = DataPreprocessing(fulltext_file_path);

    Infinity::LocalInit(data_path);
    CreateTableOptions create_tb_options;
    create_tb_options.conflict_type_ = ConflictType::kError;
    std::vector<ColumnDef *> column_defs;

    auto embedding_info = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 4);

    auto vector_type = std::make_shared<DataType>(LogicalType::kEmbedding, embedding_info);

    auto index_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto body_type = std::make_shared<DataType>(LogicalType::kVarchar);

    column_defs.push_back(new ColumnDef(0, index_type, "index", std::set<ConstraintType>{}));
    column_defs.push_back(new ColumnDef(1, body_type, "body", std::set<ConstraintType>{}));
    column_defs.push_back(new ColumnDef(2, vector_type, "other_vector", std::set<ConstraintType>{}));

    {
        SharedPtr<Infinity> infinity = Infinity::LocalConnect();
        auto result = infinity->CreateTable(db_name, table_name, column_defs, Vector<TableConstraint *>{}, create_tb_options);
        if (result.IsOk()) {
            result = infinity->Flush();
        } else {
            LOG_ERROR(fmt::format("Fail to create table {}", result.ToString()));
            return;
        }
        infinity->LocalDisconnect();
    }

    {
        SharedPtr<Infinity> infinity = Infinity::LocalConnect();
        const auto index_info = new IndexInfo();
        index_info->index_type_ = IndexType::kFullText;
        index_info->column_name_ = "body";
        index_info->index_param_list_ = new Vector<InitParameter *>();
        auto result = infinity->CreateIndex(db_name, table_name, "body_index", "", index_info, CreateIndexOptions());
        if (result.IsOk()) {
            result = infinity->Flush();
        } else {
            LOG_ERROR(fmt::format("Fail to create index {}", result.ToString()));
            return;
        }
        infinity->LocalDisconnect();
    }

    {
        SharedPtr<Infinity> infinity = Infinity::LocalConnect();
        auto index_info = new IndexInfo();
        index_info->index_type_ = IndexType::kHnsw;
        index_info->column_name_ = "other_vector";
        auto index_param_list = new std::vector<InitParameter *>();
        index_param_list->emplace_back(new InitParameter("m", "16"));
        index_param_list->emplace_back(new InitParameter("ef_construction", "50"));
        index_param_list->emplace_back(new InitParameter("metric", "l2"));
        index_info->index_param_list_ = index_param_list;
        auto result = infinity->CreateIndex(db_name, table_name, "other_index", "", index_info, CreateIndexOptions());
        if (result.IsOk()) {
            result = infinity->Flush();
        } else {
            LOG_ERROR(fmt::format("Fail to create index {}", result.ToString()));
            return;
        }
        infinity->LocalDisconnect();
    }

    RunChaosTestInParallel("default_db", "chaos_test");
    // Update(db_name, table_name);

    {
        SharedPtr<Infinity> infinity = Infinity::LocalConnect();
        DropTableOptions drop_tb_options;
        drop_tb_options.conflict_type_ = ConflictType::kError;
        auto result = infinity->DropTable(db_name, table_name, drop_tb_options);
        if (result.IsOk()) {
            result = infinity->Flush();
        } else {
            LOG_ERROR(fmt::format("Fail to drop table {}", result.ToString()));
            return;
        }
        infinity->LocalDisconnect();
    }

    Infinity::LocalUnInit();
}
