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

#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

import compilation_config;
import local_file_system;
import profiler;
import infinity;
import database;
import table;
import query_options;
import parser;
import data_type;
import logical_type;

using namespace infinity;

int main() {
    std::string import_from = test_data_path();
    import_from += "/benchmark/dbpedia-entity/corpus.jsonl";

    LocalFileSystem fs;
    if (!fs.Exists(import_from)) {
        std::cout << "Data file: " << import_from << " is not existed." << std::endl;
        return 1;
    }

    std::vector<ColumnDef *> column_defs;
    {
        std::string col1_name = "_id";
        auto col1_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col1_def = new ColumnDef(0, col1_type, std::move(col1_name), std::unordered_set<ConstraintType>());
        column_defs.push_back(col1_def);
    }
    {
        std::string col2_name = "title";
        auto col2_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col2_def = new ColumnDef(0, col2_type, std::move(col2_name), std::unordered_set<ConstraintType>());
        column_defs.push_back(col2_def);
    }
    {
        std::string col3_name = "text";
        auto col3_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col3_def = new ColumnDef(0, col3_type, std::move(col3_name), std::unordered_set<ConstraintType>());
        column_defs.push_back(col3_def);
    }

    std::string db_name = "default";
    std::string table_name = "ft_dbpedia_benchmark";
    std::string index_name = "ft_dbpedia_index";

    std::string data_path = "/tmp/infinity";

    Infinity::LocalInit(data_path);

    std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
    CreateDatabaseOptions create_db_options;
    create_db_options.conflict_type_ = ConflictType::kIgnore;
    infinity->CreateDatabase(db_name, std::move(create_db_options));

    std::shared_ptr<Database> data_base = infinity->GetDatabase(db_name);
    CreateTableOptions create_tb_options;
    create_tb_options.conflict_type_ = ConflictType::kIgnore;
    data_base->CreateTable(table_name, std::move(column_defs), std::vector<TableConstraint *>{}, std::move(create_tb_options));

    BaseProfiler profiler;

    profiler.Begin();
    std::shared_ptr<Table> table = data_base->GetTable(table_name);
    ImportOptions import_options;
    import_options.copy_file_type_ = CopyFileType::kJSONL;
    table->Import(import_from, std::move(import_options));
    std::cout << "Import data cost: " << profiler.ElapsedToString();

    profiler.Begin();
    auto index_info_list = new std::vector<IndexInfo *>();
    {
        auto index_info = new IndexInfo();
        index_info->index_type_ = IndexType::kIRSFullText;
        index_info->column_name_ = "text";

        {
            auto index_param_list = new std::vector<InitParameter *>();
            index_param_list->push_back(new InitParameter("ANALYZER", "segmentation"));
            index_info->index_param_list_ = index_param_list;
        }
        index_info_list->push_back(index_info);
    }

    auto r = table->CreateIndex(index_name, index_info_list, CreateIndexOptions());
    if (r.IsOk()) {
        r = infinity->Flush();
    } else {
        std::cout << "Fail to create index." << r.ToString() << std::endl;
    }
    std::cout << "Create index cost: " << profiler.ElapsedToString();
    profiler.End();

    Infinity::LocalUnInit();
}