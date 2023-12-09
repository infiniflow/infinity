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
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_set>

import compilation_config;

import infinity;
import database;
import table;

import parser;
import profiler;
import local_file_system;
import third_party;

import query_options;
import query_result;

using namespace infinity;

constexpr uint64_t second_unit = 1000 * 1000 * 1000;

double Measurement(size_t thread_num, size_t times, const std::function<void(size_t, std::shared_ptr<Infinity>, std::thread::id)> &closure) {
    infinity::BaseProfiler profiler;
    std::vector<std::thread> threads;
    threads.reserve(thread_num);

    assert(times % thread_num == 0);

    size_t shared_size = times / thread_num;
    for (size_t i = 0; i < thread_num; ++i) {
        threads.emplace_back([=]() {
            std::thread::id thread_id = std::this_thread::get_id();
            std::cout << ">>> Thread ID: " << thread_id << " <<<" << std::endl;
            for (size_t j = 0; j < shared_size; ++j) {
                std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
                closure(i * shared_size + j, infinity, thread_id);
                infinity->LocalDisconnect();
            }
        });
    }

    profiler.Begin();
    for (auto &thread : threads) {
        thread.join();
    }
    profiler.End();

    return static_cast<double>(profiler.Elapsed()) / second_unit;
}

int main() {
    size_t thread_num = 8;
    size_t total_times = 8192 * 4;

    std::string data_path = "/tmp/infinity";

    LocalFileSystem fs;
    if (fs.Exists(data_path)) {
        fs.DeleteDirectory(data_path);
    }
    fs.CreateDirectory(data_path);

    Infinity::LocalInit(data_path);

    std::cout << ">>> Infinity Benchmark Start <<<" << std::endl;
    std::cout << "Thread Num: " << thread_num << ", Times: " << total_times << std::endl;

    std::vector<std::string> results;
    // // Database
    // {
    //     auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //     thread_id) {
    //         auto _ = infinity->GetDatabase("default");
    //     });
    //     results.push_back(Format("-> Get Database QPS: {}", total_times / tims_costing_second));
    // }
    // {
    //     auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //     thread_id) {
    //         auto _ = infinity->ListDatabases();
    //     });
    //     results.push_back(Format("-> List Databases QPS: {}", total_times / tims_costing_second));
    // }
    // {
    //     CreateDatabaseOptions create_db_opts;
    //     auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //     thread_id) {
    //         auto _ = infinity->CreateDatabase(std::to_string(i), create_db_opts);
    //     });
    //     results.push_back(Format("-> Create Database QPS: {}", total_times / tims_costing_second));
    // }
    //
    //    {
    //        DropDatabaseOptions drop_db_opts;
    //        auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //        thread_id) {
    //            auto _ = infinity->DropDatabase(std::to_string(i), drop_db_opts);
    //        });
    //        results.push_back(Format("-> Drop Database QPS: {}", total_times / tims_costing_second));
    //    }
    //    // Table
    //    {
    //        CreateTableOptions create_table_opts;
    //        DropTableOptions drop_table_options;
    //
    //        size_t column_count = 2;
    //        std::vector<ColumnDef *> column_defs;
    //        column_defs.reserve(column_count);
    //
    //        std::shared_ptr<DataType> col_type = std::make_shared<DataType>(LogicalType::kInteger);
    //        std::string col_name_1 = "col1";
    //        auto col_def_1 = new ColumnDef(0, col_type, col_name_1, HashSet<ConstraintType>());
    //        column_defs.emplace_back(col_def_1);
    //
    //        col_type = std::make_shared<DataType>(LogicalType::kInteger);
    //        std::string col_name_2 = "col2";
    //        auto col_def_2 = new ColumnDef(1, col_type, col_name_2, HashSet<ConstraintType>());
    //        column_defs.emplace_back(col_def_2);
    //        {
    //            // Init Table
    //            std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
    //            auto _ = infinity->GetDatabase("default")->CreateTable("benchmark_test", column_defs, std::vector<TableConstraint *>(),
    //            create_table_opts); infinity->LocalDisconnect();
    //        }
    //        {
    //            auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //            thread_id) {
    //                auto _ = infinity->GetDatabase("default")->ListTables();
    //            });
    //            results.push_back(Format("-> List Tables QPS: {}", total_times / tims_costing_second));
    //        }
    //        {
    //            auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //            thread_id) {
    //                auto _ = infinity->GetDatabase("default")->GetTable("benchmark_test");
    //            });
    //            results.push_back(Format("-> Get Tables QPS: {}", total_times / tims_costing_second));
    //        }
    //        {
    //            auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //            thread_id) {
    //                auto _ = infinity->GetDatabase("default")->DescribeTable("benchmark_test");
    //            });
    //            results.push_back(Format("-> Describe Tables QPS: {}", total_times / tims_costing_second));
    //        }
    //        {
    //            auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //            thread_id) {
    //                size_t column_count = 2;
    //                std::vector<ColumnDef *> column_definitions;
    //                column_definitions.reserve(column_count);
    //
    //                std::shared_ptr<DataType> col_type = std::make_shared<DataType>(LogicalType::kInteger);
    //                std::string col_name_1 = "col1";
    //                auto col_def_1 = new ColumnDef(0, col_type, col_name_1, HashSet<ConstraintType>());
    //                column_definitions.emplace_back(col_def_1);
    //
    //                col_type = std::make_shared<DataType>(LogicalType::kInteger);
    //                std::string col_name_2 = "col2";
    //                auto col_def_2 = new ColumnDef(1, col_type, col_name_2, HashSet<ConstraintType>());
    //                column_definitions.emplace_back(col_def_2);
    //
    //                auto _ = infinity->GetDatabase("default")->CreateTable(std::to_string(i), column_definitions, std::vector<TableConstraint *>(),
    //                create_table_opts);
    //            });
    //            results.push_back(Format("-> Create Table QPS: {}", total_times / tims_costing_second));
    //        }
    //        {
    //            auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity, std::thread::id
    //            thread_id) {
    //                auto _ = infinity->GetDatabase("default")->DropTable(std::to_string(i), drop_table_options);
    //            });
    //            results.push_back(Format("-> Drop Table QPS: {}", total_times / tims_costing_second));
    //        }
    //        {
    //            std::vector<Pair<ParsedExpr *, ParsedExpr *>> vec_search_exprs;
    //            std::vector<Pair<ParsedExpr *, ParsedExpr *>> fts_search_exprs;
    //
    //            {
    //                auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity,
    //                std::thread::id thread_id) {
    //                    auto _ = infinity->GetDatabase("default")->GetTable("benchmark_test")->Search(vec_search_exprs, fts_search_exprs, nullptr,
    //                    nullptr, nullptr, nullptr);
    //                });
    //                results.push_back(Format("-> Select QPS: {}", total_times / tims_costing_second));
    //            }
    //            {
    //                auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity,
    //                std::thread::id thread_id) {
    //                    std::vector<std::vector<ParsedExpr *> *> *values = new std::vector<std::vector<ParsedExpr *> *>();
    //                    values->emplace_back(new std::vector<ParsedExpr *>());
    //
    //                    std::vector<std::string> *columns = new std::vector<std::string>();
    //                    columns->emplace_back(col_name_1);
    //                    columns->emplace_back(col_name_2);
    //
    //                    ConstantExpr *value1 = new ConstantExpr(LiteralType::kInteger);
    //                    value1->integer_value_ = i;
    //                    values->at(0)->emplace_back(value1);
    //
    //                    ConstantExpr *value2 = new ConstantExpr(LiteralType::kInteger);
    //                    value2->integer_value_ = i;
    //                    values->at(0)->emplace_back(value2);
    //                    auto _ = infinity->GetDatabase("default")->GetTable("benchmark_test")->Insert(columns, values);
    //                });
    //                results.push_back(Format("-> Insert QPS: {}", total_times / tims_costing_second));
    //            }
    //        }
    //    }
    //
    //    {
    //        std::shared_ptr <Infinity> infinity = Infinity::LocalConnect();
    //        auto result = infinity->Query("select * from benchmark_test;");
    //        infinity->LocalDisconnect();
    //    }

    // hnsw benchmark
    do {
        std::vector<ColumnDef *> column_defs;

        // init column defs
        std::shared_ptr<DataType> col1_type =
            std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128));
        std::string col1_name = "col1";
        auto col1_def = std::make_unique<ColumnDef>(0, col1_type, col1_name, std::unordered_set<ConstraintType>());
        column_defs.emplace_back(col1_def.release());

        std::string db_name = "default";
        std::string table_name = "knn_benchmark";
        std::string index_name = "hnsw_index";

        std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
        CreateDatabaseOptions create_db_options;
        create_db_options.conflict_type_ = ConflictType::kIgnore;
        auto r1 = infinity->CreateDatabase(db_name, std::move(create_db_options));

        std::shared_ptr<Database> data_base = infinity->GetDatabase(db_name);
        CreateTableOptions create_tb_options;
        create_tb_options.conflict_type_ = ConflictType::kIgnore;
        auto r2 = data_base->CreateTable(table_name, std::move(column_defs), std::vector<TableConstraint *>{}, std::move(create_tb_options));

        std::shared_ptr<Table> table = data_base->GetTable(table_name);

        std::string sift_base_path = std::string(benchmark_data_path()) + "/sift/base.fvecs";
        if (!fs.Exists(sift_base_path)) {
            std::cout << "File: " << sift_base_path << " doesn't exist" << std::endl;
            break;
        }

        ImportOptions import_options;
        import_options.copy_file_type_ = CopyFileType::kFVECS;
        auto r3 = table->Import(sift_base_path, import_options);

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

        table->CreateIndex(index_name, index_info_list, CreateIndexOptions());
    } while(false);

//    {
//        std::cout << "--- Start to run search benchmark: ";
//        auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity,
//        std::thread::id thread_id) {
//            auto _ = infinity->GetDatabase("default")->GetTable("benchmark_test")->Search(nullptr, nullptr, nullptr);
//        });
//        results.push_back(Format("-> SEARCH QPS: {}", total_times / tims_costing_second));
//        std::cout << "OK"  << std::endl;
//    }

    std::cout << ">>> Infinity Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}