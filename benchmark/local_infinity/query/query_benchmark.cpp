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
#include <iomanip>
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
import stl;

using namespace infinity;

template <typename T>
std::unique_ptr<T[]> load_data(const std::string &filename, size_t &num, int &dim) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "open file error" << std::endl;
        exit(-1);
    }
    in.read((char *)&dim, 4);
    in.seekg(0, std::ios::end);
    auto ss = in.tellg();
    num = ((size_t)ss) / (dim + 1) / 4;
    auto data = std::make_unique_for_overwrite<T[]>(num * dim);

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char *)(data.get() + i * dim), dim * 4);
    }
    in.close();
    return data;
}

template <class Function>
inline void LoopFor(size_t id_begin, size_t id_end, size_t threadId, Function fn, const std::string &table_name) {
    std::cout << "threadId = " << threadId << " [" << id_begin << ", " << id_end << ")" << std::endl;
    std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
    std::shared_ptr<Database> data_base = infinity->GetDatabase("default");
    std::shared_ptr<Table> table = data_base->GetTable(table_name);
    if(table == nullptr) {
        std::cerr << table_name << " isn't found." << std::endl;
        exit(1);
    }
    for (auto id = id_begin; id < id_end; ++id) {
        fn(id, table, threadId);
    }
}

template <class Function>
inline void ParallelFor(size_t start, size_t end, size_t numThreads, Function fn, const std::string &table_name) {
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    std::vector<std::jthread> threads;
    threads.reserve(numThreads);
    size_t avg_cnt = (end - start) / numThreads;
    size_t extra_cnt = (end - start) % numThreads;
    for (size_t id_begin = start, threadId = 0; threadId < numThreads; ++threadId) {
        size_t id_end = id_begin + avg_cnt + (threadId < extra_cnt);
        threads.emplace_back([id_begin, id_end, threadId, fn, table_name] { LoopFor(id_begin, id_end, threadId, fn, table_name); });
        id_begin = id_end;
    }
}

int main(int argc, char *argv[]) {
    size_t thread_num = 1;
    size_t total_times = 1;
    std::cout << "Please input thread_num, 0 means use all resources:" << std::endl;
    std::cin >> thread_num;
    std::cout << "Please input total_times:" << std::endl;
    std::cin >> total_times;

    std::string path = "/tmp/infinity";
    LocalFileSystem fs;

    Infinity::LocalInit(path);

    std::cout << ">>> Query Benchmark Start <<<" << std::endl;
    std::cout << "Thread Num: " << thread_num << ", Times: " << total_times << std::endl;

    std::vector<std::string> results;

    std::string query_path = std::string(test_data_path());
    std::string groundtruth_path = std::string(test_data_path());
    size_t dimension = 0;
    int64_t topk = 100;

    std::string table_name{"test_hnsw"};

    std::unique_ptr<float[]> queries_ptr;
    size_t query_count = 100000;
    do {
        std::cout << "--- Start to run search benchmark: " << std::endl;
        auto query_function = [&](size_t query_idx, std::shared_ptr<Table> &table, size_t threadId) {
            UniquePtr<CheckTable> command_info = MakeUnique<CheckTable>(table_name.c_str());
            auto result = table->Command(Move(command_info));
            if(!result.IsOk()) {
                std::cout << "Error result" << std::endl;
                exit(1);
            }
        };
        BaseProfiler profiler;
        profiler.Begin();
        ParallelFor(0, query_count, thread_num, query_function, table_name);
        profiler.End();
        results.push_back(Format("Total cost : {}", profiler.ElapsedToString(1000)));
    } while (--total_times);

    std::cout << ">>> Query Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}
