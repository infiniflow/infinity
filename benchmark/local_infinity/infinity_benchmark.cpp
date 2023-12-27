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

import stl;
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

constexpr u64 second_unit = 1000 * 1000 * 1000;

double Measurement(String name, SizeT thread_num, SizeT times, const StdFunction<void(SizeT, SharedPtr<Infinity>, std::thread::id)> &closure) {
    infinity::BaseProfiler profiler(name);
    Vector<std::thread> threads;
    threads.reserve(thread_num);

    assert(times % thread_num == 0);

    SizeT shared_size = times / thread_num;
    for (SizeT i = 0; i < thread_num; ++i) {
        threads.emplace_back([=]() {
            std::thread::id thread_id = std::this_thread::get_id();
            std::cout << ">>> Thread ID: " << thread_id << " <<<" << std::endl;
            for (SizeT j = 0; j < shared_size; ++j) {
                SharedPtr<Infinity> infinity = Infinity::LocalConnect();
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
    SizeT thread_num = 16;
    // For Sift1M
    SizeT total_times = 100 * 100;

    String path = "/tmp/infinity";

    LocalFileSystem fs;
    if (fs.Exists(path)) {
        fs.DeleteDirectory(path);
    }
    fs.CreateDirectory(path);

    Infinity::LocalInit(path);

    std::cout << ">>> Infinity Benchmark Start <<<" << std::endl;
    std::cout << "Thread Num: " << thread_num << ", Times: " << total_times << std::endl;

    Vector<String> results;
    // Database
    {
        auto tims_costing_second = Measurement("Get Database", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
            __attribute__((unused)) auto ignored = infinity->GetDatabase("default");
        });
        results.push_back(Format("-> Get Database QPS: {}", total_times / tims_costing_second));
    }
    {
        auto tims_costing_second = Measurement("List Databases", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
            __attribute__((unused)) auto ignored = infinity->ListDatabases();
        });
        results.push_back(Format("-> List Databases QPS: {}", total_times / tims_costing_second));
    }
    {
        CreateDatabaseOptions create_db_opts;
        auto tims_costing_second = Measurement("Create Database", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
            __attribute__((unused)) auto ignored = infinity->CreateDatabase(ToStr(i), create_db_opts);
        });
        results.push_back(Format("-> Create Database QPS: {}", total_times / tims_costing_second));
    }

    {
        DropDatabaseOptions drop_db_opts;
        auto tims_costing_second = Measurement("Drop Database", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
            __attribute__((unused)) auto ignored = infinity->DropDatabase(ToStr(i), drop_db_opts);
        });
        results.push_back(Format("-> Drop Database QPS: {}", total_times / tims_costing_second));
    }
    // Table
    {
        CreateTableOptions create_table_opts;
        DropTableOptions drop_table_options;

        SizeT column_count = 2;
        Vector<ColumnDef *> column_defs;
        column_defs.reserve(column_count);

        SharedPtr<DataType> col_type = MakeShared<DataType>(LogicalType::kInteger);
        String col_name_1 = "col1";
        auto col_def_1 = new ColumnDef(0, col_type, col_name_1, HashSet<ConstraintType>());
        column_defs.emplace_back(col_def_1);

        col_type = MakeShared<DataType>(LogicalType::kInteger);
        String col_name_2 = "col2";
        auto col_def_2 = new ColumnDef(1, col_type, col_name_2, HashSet<ConstraintType>());
        column_defs.emplace_back(col_def_2);
        {
            // Init Table
            SharedPtr<Infinity> infinity = Infinity::LocalConnect();
            __attribute__((unused)) auto ignored =
                infinity->GetDatabase("default")->CreateTable("benchmark_test", column_defs, Vector<TableConstraint *>(), create_table_opts);
            infinity->LocalDisconnect();
        }
        // {
        //     auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id
        // thread_id) {
        //         __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->ListTables();
        //     });
        //     results.push_back(Format("-> List Tables QPS: {}", total_times / tims_costing_second));
        // }
        {
            auto tims_costing_second = Measurement("Get Tables", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->GetTable("benchmark_test");
            });
            results.push_back(Format("-> Get Tables QPS: {}", total_times / tims_costing_second));
        }
        {
            auto tims_costing_second = Measurement("Describe Tables", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->DescribeTable("benchmark_test");
            });
            results.push_back(Format("-> Describe Tables QPS: {}", total_times / tims_costing_second));
        }
        {
            auto tims_costing_second = Measurement("Create Tables", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                SizeT column_count = 2;
                Vector<ColumnDef *> column_definitions;
                column_definitions.reserve(column_count);

                SharedPtr<DataType> col_type = MakeShared<DataType>(LogicalType::kInteger);
                String col_name_1 = "col1";
                auto col_def_1 = new ColumnDef(0, col_type, col_name_1, HashSet<ConstraintType>());
                column_definitions.emplace_back(col_def_1);

                col_type = MakeShared<DataType>(LogicalType::kInteger);
                String col_name_2 = "col2";
                auto col_def_2 = new ColumnDef(1, col_type, col_name_2, HashSet<ConstraintType>());
                column_definitions.emplace_back(col_def_2);

                __attribute__((unused)) auto ignored =
                    infinity->GetDatabase("default")->CreateTable(ToStr(i), column_definitions, Vector<TableConstraint *>(), create_table_opts);
            });
            results.push_back(Format("-> Create Table QPS: {}", total_times / tims_costing_second));
        }
        {
            auto tims_costing_second = Measurement("Drop Table", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->DropTable(ToStr(i), drop_table_options);
            });
            results.push_back(Format("-> Drop Table QPS: {}", total_times / tims_costing_second));
        }
        {
            {
                auto tims_costing_second =
                    Measurement("Select", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                        Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
                        ColumnExpr *col1 = new ColumnExpr();
                        col1->names_.emplace_back("col1");
                        output_columns->emplace_back(col1);

                        ColumnExpr *col2 = new ColumnExpr();
                        col2->names_.emplace_back("col2");
                        output_columns->emplace_back(col2);

                        __attribute__((unused)) auto ignored =
                            infinity->GetDatabase("default")->GetTable("benchmark_test")->Search(nullptr, nullptr, output_columns);
                    });
                results.push_back(Format("-> Select QPS: {}", total_times / tims_costing_second));
            }
    {
        auto tims_costing_second =
            Measurement("Insert", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                Vector<Vector<ParsedExpr *> *> *values = new Vector<Vector<ParsedExpr *> *>();
                values->emplace_back(new Vector<ParsedExpr *>());

                Vector<String> *columns = new Vector<String>();
                columns->emplace_back(col_name_1);
                columns->emplace_back(col_name_2);

                ConstantExpr *value1 = new ConstantExpr(LiteralType::kInteger);
                value1->integer_value_ = i;
                values->at(0)->emplace_back(value1);

                ConstantExpr *value2 = new ConstantExpr(LiteralType::kInteger);
                value2->integer_value_ = i;
                values->at(0)->emplace_back(value2);

                __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->GetTable("benchmark_test")->Insert(columns, values);
            });
        results.push_back(Format("-> Insert QPS: {}", total_times / tims_costing_second));
    }
            {
                auto tims_costing_second =
                    Measurement("Update", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                        Vector<UpdateExpr *> *values = new Vector<UpdateExpr *>();

                        Vector<String> *columns = new Vector<String>();
                        columns->emplace_back(col_name_1);
                        columns->emplace_back(col_name_2);

                        ConstantExpr *value1 = new ConstantExpr(LiteralType::kInteger);
                        value1->integer_value_ = i;
                        UpdateExpr *update_expr1 = new UpdateExpr();
                        update_expr1->column_name = col_name_1;
                        update_expr1->value = value1;

                        ConstantExpr *value2 = new ConstantExpr(LiteralType::kInteger);
                        value1->integer_value_ = i;
                        UpdateExpr *update_expr2 = new UpdateExpr();
                        update_expr2->column_name = col_name_2;
                        update_expr2->value = value2;

                        values->push_back(update_expr1);
                        values->push_back(update_expr2);

                        __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->GetTable("benchmark_test")->Update(nullptr,
                        values);
                    });
                results.push_back(Format("-> Update QPS: {}", total_times / tims_costing_second));
            }
            {
                auto tims_costing_second =
                    Measurement("Delete", thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                        __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->GetTable("benchmark_test")->Delete(nullptr);
                    });
                results.push_back(Format("-> Delete QPS: {}", total_times / tims_costing_second));
            }
        }
    }
    {
        SizeT sort_row = 1000 * 1000;
        SizeT sort_times = 48 * 10;

        String col_name_1 = "c1";
        String col_name_2 = "c2";

        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        SharedPtr<DataType> col_type = MakeShared<DataType>(LogicalType::kInteger);
        {
            CreateTableOptions create_table_opts;

            SizeT column_count = 2;
            Vector<ColumnDef *> column_definitions;
            column_definitions.reserve(column_count);

            auto col_def_1 = new ColumnDef(0, col_type, col_name_1, HashSet<ConstraintType>());
            column_definitions.emplace_back(col_def_1);

            auto col_def_2 = new ColumnDef(1, col_type, col_name_2, HashSet<ConstraintType>());
            column_definitions.emplace_back(col_def_2);

            SharedPtr<Infinity> infinity = Infinity::LocalConnect();
            __attribute__((unused)) auto ignored =
                infinity->GetDatabase("default")->CreateTable("benchmark_sort", column_definitions, Vector<TableConstraint *>(), create_table_opts);
            infinity->LocalDisconnect();
        }
        {
            auto tims_costing_second =
                Measurement("Insert for Select Sort", thread_num, sort_row, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                    Vector<Vector<ParsedExpr *> *> *values = new Vector<Vector<ParsedExpr *> *>();
                    values->emplace_back(new Vector<ParsedExpr *>());

                    Vector<String> *columns = new Vector<String>();
                    columns->emplace_back(col_name_1);
                    columns->emplace_back(col_name_2);

                    ConstantExpr *value1 = new ConstantExpr(LiteralType::kInteger);
                    value1->integer_value_ = std::rand();
                    values->at(0)->emplace_back(value1);

                    ConstantExpr *value2 = new ConstantExpr(LiteralType::kInteger);
                    value2->integer_value_ = std::rand();
                    values->at(0)->emplace_back(value2);

                    __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->GetTable("benchmark_sort")->Insert(columns, values);
                });
            results.push_back(Format("-> Insert for Sort Time: {}s", tims_costing_second));
        }
        {
            auto tims_costing_second =
                Measurement("Select Sort", thread_num, sort_times, [&](SizeT i, SharedPtr<Infinity> infinity, std::thread::id thread_id) {
                    Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
                    ColumnExpr *col1 = new ColumnExpr();
                    col1->names_.emplace_back("col1");
                    output_columns->emplace_back(col1);

                    ColumnExpr *col2 = new ColumnExpr();
                    col2->names_.emplace_back("col2");
                    output_columns->emplace_back(col2);

                    __attribute__((unused)) auto ignored = infinity->Query("select c1, c2 from benchmark_sort order by c1");
                });
            results.push_back(Format("-> Select Sort Time QPS: {}", sort_times / tims_costing_second));
        }
    }

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

        std::string sift_base_path = std::string(test_data_path()) + "/benchmark/sift/base.fvecs";
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
    } while (false);

    //    {
    //        std::cout << "--- Start to run search benchmark: ";
    //        auto tims_costing_second = Measurement(thread_num, total_times, [&](size_t i, std::shared_ptr<Infinity> infinity,
    //        std::thread::id thread_id) {
    //            __attribute__((unused)) auto ignored = infinity->GetDatabase("default")->GetTable("benchmark_test")->Search(nullptr, nullptr,
    //            nullptr);
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