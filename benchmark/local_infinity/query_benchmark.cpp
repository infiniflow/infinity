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
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
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

float *fvecs_read(const char *fname, size_t *d_out, size_t *n_out) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
    int d;
    fread(&d, 1, sizeof(int), f);
    assert((d > 0 && d < 1000000) || !"unreasonable dimension");
    fseek(f, 0, SEEK_SET);
    struct stat st;
    fstat(fileno(f), &st);
    size_t sz = st.st_size;
    assert(sz % ((d + 1) * 4) == 0 || !"weird file size");
    size_t n = sz / ((d + 1) * 4);

    *d_out = d;
    *n_out = n;
    float *x = new float[n * (d + 1)];
    size_t nr = fread(x, sizeof(float), n * (d + 1), f);
    assert(nr == n * (d + 1) || !"could not read whole file");

    // shift array to remove row headers
    for (size_t i = 0; i < n; i++)
        memmove(x + i * d, x + 1 + i * (d + 1), d * sizeof(*x));

    fclose(f);
    return x;
}

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
    size_t thread_num = 1;
    size_t total_times = 1;

    std::string path = "/tmp/infinity";
    LocalFileSystem fs;

    Infinity::LocalInit(path);

    std::cout << ">>> Query Benchmark Start <<<" << std::endl;
    std::cout << "Thread Num: " << thread_num << ", Times: " << total_times << std::endl;

    std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
    std::vector<std::string> results;

    do {
        std::cout << "--- Start to run search benchmark: ";

        std::string sift_query_path = std::string(test_data_path()) + "/benchmark/sift_1m/sift_query.fvecs";
        if (!fs.Exists(sift_query_path)) {
            std::cout << "File: " << sift_query_path << " doesn't exist" << std::endl;
            break;
        }

        size_t query_count;
        size_t dimension = 128;
        const float *queries = nullptr;
        {
            size_t dim = -1;
            queries = const_cast<const float *>(fvecs_read(sift_query_path.c_str(), &dim, &query_count));
            assert(dimension == dim || !"query vector dim isn't 128");
        }

        for (size_t query_idx = 0; query_idx < 1; ++query_idx) {
            SearchExpr *search_expr = new SearchExpr();
            KnnExpr *knn_expr = new KnnExpr();
            knn_expr->dimension_ = dimension;
            knn_expr->distance_type_ = KnnDistanceType::kL2;
            knn_expr->topn_ = 100;
            knn_expr->embedding_data_type_ = EmbeddingDataType::kElemFloat;
            knn_expr->embedding_data_ptr_ = new float[dimension];
            memmove(knn_expr->embedding_data_ptr_, queries + query_idx * dimension * sizeof(float), dimension * sizeof(float));

            ColumnExpr* column_expr = new ColumnExpr();
            column_expr->names_.emplace_back("col1");
            knn_expr->column_expr_ = column_expr;
            search_expr->knn_exprs_.emplace_back(knn_expr);

            std::shared_ptr<Database> data_base = infinity->GetDatabase("default");
            std::shared_ptr<Table> table = data_base->GetTable("knn_benchmark");

            std::vector<ParsedExpr *> *output_columns = new std::vector<ParsedExpr *>;
            ColumnExpr* select_column_expr = new ColumnExpr();
            select_column_expr->names_.emplace_back("col1");
            output_columns->emplace_back(select_column_expr);
            auto result = table->Search(search_expr, nullptr, output_columns);
            if(!result.IsOk()) {
                std::cerr << "Error: " << result.ErrorStr() << std::endl;
                return 0;
            }
            std::cout << "Result row count: " << result.ResultTable()->row_count() << std::endl;

            delete[] (float*)(knn_expr->embedding_data_ptr_);
        }

        results.push_back(Format("-> SEARCH QPS: {}", total_times));
        std::cout << "OK" << std::endl;
    } while (false);

    std::cout << ">>> Query Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}