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
import query_options;
import database;
import profiler;
import third_party;

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
    SizeT total_times = 1200 * 1000;

    Infinity::LocalInit("/tmp/infinity");

    std::cout << ">>> Infinity Benchmark Start <<<" << std::endl;
    std::cout << "Thread Num: " << thread_num << ", Tims: " << total_times << std::endl;
    
    Vector<String> results;
    {
        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
            auto _ = infinity->GetDatabase("default");
        });
        results.push_back(Format("-> Get Database QPS: {}", total_times / tims_costing_second));
    }
//    {
//        CreateDatabaseOptions create_db_opts;
//        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i) {
//            auto _ = infinity->CreateDatabase(ToStr(i), create_db_opts);
//        });
//        results.push_back(Format("-> Create Database QPS: {}", total_times / tims_costing_second));
//    }
//    {
//        DropDatabaseOptions drop_db_opts;
//        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i) {
//            auto _ = infinity->DropDatabase(ToStr(i), drop_db_opts);
//        });
//        results.push_back(Format("-> Drop Database QPS: {}", total_times / tims_costing_second));
//    }
//    {
//        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
//            auto _ = infinity->ListDatabases();
//        });
//        results.push_back(Format("-> List Database QPS: {}", total_times / tims_costing_second));
//    }
//    {
//        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i) {
//            auto _ = infinity->Query(Format("create table t{} (v1 int, v2 int, v3 int)", i));
//        });
//        results.push_back(Format("-> Create Table QPS: {}", total_times / tims_costing_second));
//    }
    // CRUD
//    {
//        // Init Table
//        SharedPtr<Infinity> infinity = Infinity::LocalConnect();
//        auto _ = infinity->Query("create table t(v1 int, v2 int, v3 int)");
//        infinity->LocalDisconnect();
//    }
//    {
//        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
//            auto _ = infinity->Query(Format("insert into t values ({}, {}, {})", i, i + 1, i + 2));
//        });
//        results.push_back(Format("-> Insert QPS: {}", total_times / tims_costing_second));
//    }
//    {
//        auto tims_costing_second = Measurement(thread_num, total_times, [&](SizeT i, SharedPtr<Infinity> infinity) {
//            auto _ = infinity->Query("select * from t");
//        });
//        results.push_back(Format("-> Select QPS: {}", total_times / tims_costing_second));
//    }

//    {
//        auto tims_costing_second = Measurement(1, total_times, [&](SizeT i) {
//            auto _ = infinity->Query(Format("drop table t{}", i));
//        });
//        results.push_back(Format("-> Drop Table QPS: {}", total_times / tims_costing_second));
//    }

    std::cout << ">>> Infinity Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}