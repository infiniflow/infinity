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

#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_set>

import compilation_config;
import internal_types;
import infinity;
import logical_type;

import profiler;
import local_file_system;
import third_party;
import logical_node_type;
import embedding_info;
import create_index_info;
import query_options;
import query_result;
import extra_ddl_info;
import knn_expr;
import column_def;
import statement_common;
import data_type;

using namespace infinity;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "import sift or gist, with optional test_data_path (default to /infinity/test/data in docker) and optional infinity path "
                     "(default to /var/infinity)"
                  << std::endl;
        return 1;
    }
    bool sift = true;
    if (strcmp(argv[1], "sift") && strcmp(argv[1], "gist")) {
        return 1;
    }
    sift = strcmp(argv[1], "sift") == 0;

    std::string data_path = "/var/infinity";
    if (argc >= 4) {
        data_path = std::string(argv[3]);
    }

    LocalFileSystem fs;
    if (fs.Exists(data_path)) {
        std::cout << "Data path: " << data_path << " is already existed." << std::endl;
    } else {
        fs.CreateDirectory(data_path);
        std::cout << "Data path: " << data_path << " is created." << std::endl;
    }

    Infinity::LocalInit(data_path);

    std::cout << ">>> Import Benchmark Start <<<" << std::endl;

    std::vector<std::string> results;

    // hnsw benchmark
    do {
        std::vector<ColumnDef *> column_defs;

        // init column defs
        std::shared_ptr<DataType> col1_type = nullptr;
        std::string base_path = std::string(test_data_path());
        if (argc >= 3) {
            base_path = std::string(argv[2]);
        }
        std::string table_name;
        if (sift) {
            col1_type = std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128));
            base_path += "/benchmark/sift_1m/sift_base.fvecs";
            table_name = "sift_benchmark";
        } else {
            col1_type = std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 960));
            base_path += "/benchmark/gist_1m/gist_base.fvecs";
            table_name = "gist_benchmark";
        }
        std::cout << "Import from: " << base_path << std::endl;

        std::string col1_name = "col1";
        auto col1_def = std::make_unique<ColumnDef>(0, col1_type, col1_name, std::set<ConstraintType>());
        column_defs.emplace_back(col1_def.release());

        std::string db_name = "default_db";
        std::string index_name = "hnsw_index";

        std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
        CreateDatabaseOptions create_db_options;
        create_db_options.conflict_type_ = ConflictType::kIgnore;
        auto r1 = infinity->CreateDatabase(db_name, std::move(create_db_options));

//        auto [ data_base, status1 ] = infinity->GetDatabase(db_name);
        CreateTableOptions create_tb_options;
        create_tb_options.conflict_type_ = ConflictType::kIgnore;
        auto r2 = infinity->CreateTable(db_name, table_name, std::move(column_defs), std::vector<TableConstraint *>{}, std::move(create_tb_options));

//        auto [ table, status2 ] = data_base->GetTable(table_name);

        if (!fs.Exists(base_path)) {
            std::cout << "File: " << base_path << " doesn't exist" << std::endl;
            break;
        }

        ImportOptions import_options;
        import_options.copy_file_type_ = CopyFileType::kFVECS;

        infinity::BaseProfiler profiler;
        profiler.Begin();
        QueryResult query_result = infinity->Import(db_name, table_name, base_path, import_options);
        std::cout << "Import data cost: " << profiler.ElapsedToString() << std::endl;

        auto index_info_list = new std::vector<IndexInfo *>();
        {
            auto index_info = new IndexInfo();
            index_info->index_type_ = IndexType::kHnsw;
            index_info->column_name_ = col1_name;

            {
                auto index_param_list = new std::vector<InitParameter *>();
                index_param_list->emplace_back(new InitParameter("M", std::to_string(16)));
                index_param_list->emplace_back(new InitParameter("ef_construction", std::to_string(200)));
                index_param_list->emplace_back(new InitParameter("ef", std::to_string(200)));
                index_param_list->emplace_back(new InitParameter("metric", "l2"));
                index_param_list->emplace_back(new InitParameter("encode", "lvq"));
                index_info->index_param_list_ = index_param_list;
            }
            index_info_list->emplace_back(index_info);
        }

        query_result = infinity->CreateIndex(db_name, table_name, index_name, index_info_list, CreateIndexOptions());

        if (query_result.IsOk()) {
            std::cout << "Create Index cost: " << profiler.ElapsedToString() << std::endl;
            query_result = infinity->Flush();
            profiler.End();
            std::cout << "Flush data cost: " << profiler.ElapsedToString() << std::endl;
        } else {
            std::cout << "Fail to create index." << profiler.ElapsedToString() << std::endl;
            profiler.End();
            break;
        }
    } while (false);

    std::cout << ">>> Knn Import Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}