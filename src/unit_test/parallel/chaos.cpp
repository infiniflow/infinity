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

using namespace infinity;

class ParallelTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         ParallelTest,
                         ::testing::Values(ParallelTest::NEW_CONFIG_PATH, ParallelTest::NEW_VFS_OFF_CONFIG_PATH));

const int data_size = 100000;

struct DataRow {
    String body;
    Vector<float> others;
};

Vector<DataRow> DataPreprocessing(const String &filepath) {
    std::vector<std::string> texts;
    std::ifstream fin(filepath);
    if (!fin) {
        std::cerr << "Cannot open text data: " << filepath << std::endl;
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

    Vector<std::vector<float>> vectors = {{0.0, 0.0, 0.0, 0.0},
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
    int repeat_times = data_size / base_size;

    data_rows.reserve(data_size);
    for (int i = 0; i < repeat_times; ++i) {
        for (int j = 0; j < base_size; ++j) {
            DataRow row;
            row.body = texts[j % texts.size()];
            row.others = vectors[j % vectors.size()];
            data_rows.push_back(row);
        }
    }

    for (size_t i = 0; i < data_size - data_rows.size(); ++i) {
        DataRow row;
        row.body = texts[i % texts.size()];
        row.others = vectors[i % vectors.size()];
        data_rows.push_back(row);
    }

    return data_rows;
}

void FullTextSearch(SharedPtr<Infinity> infinity, const String &db, const String &table) {
    auto *search_expr = new SearchExpr();
    auto *exprs = new std::vector<ParsedExpr *>();
    auto *match_expr = new MatchExpr();

    match_expr->fields_ = "body^5";
    match_expr->matching_text_ = "harmful chemical";
    exprs->push_back(match_expr);
    search_expr->SetExprs(exprs);

    auto *output_columns = new std::vector<ParsedExpr *>();

    auto *select_index_expr = new FunctionExpr();
    select_index_expr->func_name_ = "index";

    auto *select_body_expr = new FunctionExpr();
    select_body_expr->func_name_ = "body";

    auto *select_other_vector_expr = new FunctionExpr();
    select_other_vector_expr->func_name_ = "other_vector";

    auto *select_rowid_expr = new FunctionExpr();
    select_rowid_expr->func_name_ = "row_id";

    auto *select_score_expr = new FunctionExpr();
    select_score_expr->func_name_ = "score";

    output_columns->emplace_back(select_rowid_expr);
    output_columns->emplace_back(select_score_expr);
    output_columns->emplace_back(select_index_expr);
    output_columns->emplace_back(select_body_expr);
    output_columns->emplace_back(select_other_vector_expr);

    infinity->Search(db, table, search_expr, nullptr, nullptr, nullptr, output_columns, nullptr, nullptr, nullptr, nullptr, false);
}

// void insert(const String &db_name, const String &table_name, Vector<DataRow> data) {
//     SharedPtr<Infinity> infinity = Infinity::LocalConnect();
//
//     infinity->LocalDisconnect();
// }

TEST_P(ParallelTest, ChaosTest) {
    auto db_name = "default_db";
    auto table_name = "chaos_test";

    String data_path = "/var/infinity";

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
            LOG_ERROR(fmt::format("Fail to create index {}", result.ToString()));
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

    Infinity::LocalUnInit();
}
