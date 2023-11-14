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
#include <thread>
#include <cassert>

import stl;
import infinity;
import database;
import parser;
import profiler;
import local_file_system;
import third_party;

import query_options;
import query_result;

using namespace infinity;

f32 Measurement(SizeT thread_num, SizeT times, const StdFunction<void(SizeT, SharedPtr<Infinity>)>& closure) {
    infinity::BaseProfiler profiler;
    Vector<std::thread> threads;
    threads.reserve(thread_num);

    assert(times % thread_num == 0);

    SizeT shared_size = times / thread_num;
    for (SizeT i = 0; i < thread_num; ++i) {
        threads.emplace_back([&]() {
            for (SizeT j = 0; j < shared_size; ++j) {
                SharedPtr<Infinity> infinity = Infinity::LocalConnect();
                closure(i * shared_size + j, infinity);
                infinity->LocalDisconnect();
            }
        });
    }

    profiler.Begin();
    for (auto& thread : threads) {
        thread.join();
    }
    profiler.End();

    return static_cast<float>(profiler.Elapsed()) / (1000 * 1000 * 1000);
}

int main() {
    SizeT thread_num = 8;
    SizeT total_times = 2 * 10 * 1000;

    String path = "/tmp/infinity";

    LocalFileSystem fs;
    if (fs.Exists(path)) {
        fs.DeleteDirectory(path);
    }
    fs.CreateDirectory(path);

    Infinity::LocalInit(path);

    std::cout << ">>> Infinity Benchmark Start <<<" << std::endl;
    std::cout << "Thread Num: " << thread_num << ", Tims: " << total_times << std::endl;
    
    Vector<String> results;
    // Database
    {
        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
            auto _ = infinity->GetDatabase("default");
        });
        results.push_back(Format("-> Get Database QPS: {}", total_times / tims_costing_second));
    }
    {
        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
            auto _ = infinity->ListDatabases();
        });
        results.push_back(Format("-> List Databases QPS: {}", total_times / tims_costing_second));
    }
    {
        CreateDatabaseOptions create_db_opts;
        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
            auto _ = infinity->CreateDatabase(ToStr(i), create_db_opts);
        });
        results.push_back(Format("-> Create Database QPS: {}", total_times / tims_costing_second));
    }
    {
        DropDatabaseOptions drop_db_opts;
        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
            auto _ = infinity->DropDatabase(ToStr(i), drop_db_opts);
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
            auto _ = infinity->GetDatabase("default")->CreateTable("benchmark_test", column_defs, Vector<TableConstraint *>(), create_table_opts);
            infinity->LocalDisconnect();
        }
        {
            auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
                auto _ = infinity->GetDatabase("default")->ListTables();
            });
            results.push_back(Format("-> List Tables QPS: {}", total_times / tims_costing_second));
        }
        {
            auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
                auto _ = infinity->GetDatabase("default")->GetTable("benchmark_test");
            });
            results.push_back(Format("-> Get Tables QPS: {}", total_times / tims_costing_second));
        }
        {
            auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
                auto _ = infinity->GetDatabase("default")->DescribeTable("benchmark_test");
            });
            results.push_back(Format("-> Describe Tables QPS: {}", total_times / tims_costing_second));
        }
        {
            auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
                auto _ = infinity->GetDatabase("default")->CreateTable(ToStr(i), column_defs, Vector<TableConstraint *>(), create_table_opts);
            });
            results.push_back(Format("-> Create Table QPS: {}", total_times / tims_costing_second));
        }
        {
            auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
                auto _ = infinity->GetDatabase("default")->DropTable(ToStr(i), drop_table_options);
            });
            results.push_back(Format("-> Drop Table QPS: {}", total_times / tims_costing_second));
        }
        {
            Vector<Pair<ParsedExpr *, ParsedExpr *>> vec_search_exprs;
            Vector<Pair<ParsedExpr *, ParsedExpr *>> fts_search_exprs;

            {
                auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
                    auto _ = infinity->GetDatabase("default")->GetTable("benchmark_test")->Search(vec_search_exprs, fts_search_exprs, nullptr, nullptr, nullptr, nullptr);
                });
                results.push_back(Format("-> Select QPS: {}", total_times / tims_costing_second));
            }
            {
                auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
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

                    auto _ = infinity->GetDatabase("default")->GetTable("benchmark_test")->Insert(columns, values);
                });
                results.push_back(Format("-> Insert QPS: {}", total_times / tims_costing_second));
            }
        }
    }

    std::cout << ">>> Infinity Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}