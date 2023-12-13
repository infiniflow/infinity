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

int main(int argc, char **argv) {
    if (argc > 2) {
        return 1;
    }
    bool sift = true;
    if (argc == 2) {
        sift = std::string(argv[1]) == "sift";
    }

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

    std::string table_name;
    if (sift) {
        dimension = 128;
        query_path += "/benchmark/sift_1m/sift_query.fvecs";
        groundtruth_path += "/benchmark/sift_1m/sift_groundtruth.ivecs";
        table_name = "sift_benchmark";
    } else {
        dimension = 960;
        query_path += "/benchmark/gist_1m/gist_query.fvecs";
        groundtruth_path += "/benchmark/gist_1m/gist_groundtruth.ivecs";
        table_name = "gist_benchmark";
    }
    std::cout << "query from: " << query_path << std::endl;
    std::cout << "groundtruth is: " << groundtruth_path << std::endl;

    if (!fs.Exists(query_path)) {
        std::cerr << "File: " << query_path << " doesn't exist" << std::endl;
        exit(-1);
    }
    if (!fs.Exists(groundtruth_path)) {
        std::cerr << "File: " << groundtruth_path << " doesn't exist" << std::endl;
        exit(-1);
    }
    std::unique_ptr<float[]> queries_ptr;
    size_t query_count;
    {
        int dim = -1;
        queries_ptr = load_data<float>(query_path, query_count, dim);
        assert(dimension == dim || !"query vector dim isn't 128");
    }
    auto queries = queries_ptr.get();
    std::vector<std::unordered_set<int>> ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100;
    {
        std::unique_ptr<int[]> gt;
        size_t gt_count;
        int gt_top_k;
        {
            gt = load_data<int>(groundtruth_path, gt_count, gt_top_k);
            assert(gt_top_k == topk || !"gt_top_k != topk");
            assert(gt_count == query_count || !"gt_count != query_count");
        }
        ground_truth_sets_1.resize(gt_count);
        ground_truth_sets_10.resize(gt_count);
        ground_truth_sets_100.resize(gt_count);
        for (size_t i = 0; i < gt_count; ++i) {
            for (int j = 0; j < gt_top_k; ++j) {
                auto x = gt[i * gt_top_k + j];
                if (j < 1) {
                    ground_truth_sets_1[i].insert(x);
                }
                if (j < 10) {
                    ground_truth_sets_10[i].insert(x);
                }
                if (j < 100) {
                    ground_truth_sets_100[i].insert(x);
                }
            }
        }
    }
    do {
        std::cout << "--- Start to run search benchmark: " << std::endl;
        std::vector<std::vector<uint64_t>> query_results(query_count);
        for (auto &v : query_results) {
            v.reserve(100);
        }
        auto query_function = [dimension, topk, queries, &query_results](size_t query_idx, std::shared_ptr<Table> &table, size_t threadId) {
            KnnExpr *knn_expr = new KnnExpr();
            knn_expr->dimension_ = dimension;
            knn_expr->distance_type_ = KnnDistanceType::kL2;
            knn_expr->topn_ = topk;
            knn_expr->embedding_data_type_ = EmbeddingDataType::kElemFloat;
            auto embedding_data_ptr = new float[dimension];
            knn_expr->embedding_data_ptr_ = embedding_data_ptr;
            auto src_ptr = queries + query_idx * dimension;
            memmove(knn_expr->embedding_data_ptr_, src_ptr, dimension * sizeof(float));

            ColumnExpr *column_expr = new ColumnExpr();
            column_expr->names_.emplace_back("col1");
            knn_expr->column_expr_ = column_expr;
            std::vector<ParsedExpr *> *exprs = new std::vector<ParsedExpr *>();
            exprs->emplace_back(knn_expr);
            SearchExpr *search_expr = new SearchExpr();
            search_expr->SetExprs(exprs);

            std::vector<ParsedExpr *> *output_columns = new std::vector<ParsedExpr *>;
            auto select_rowid_expr = new FunctionExpr();
            select_rowid_expr->func_name_ = "row_id";
            output_columns->emplace_back(select_rowid_expr);
            auto result = table->Search(search_expr, nullptr, output_columns);
            {
                auto &cv = result.result_table_->GetDataBlockById(0)->column_vectors;
                auto &column = *cv[0];
                auto data = reinterpret_cast<const RowID *>(column.data());
                auto cnt = column.Size();
                for (size_t i = 0; i < cnt; ++i) {
                    query_results[query_idx].emplace_back(data[i].ToUint64());
                }
            }
            delete[] embedding_data_ptr;
        };
        infinity::BaseProfiler profiler;
        profiler.Begin();
        ParallelFor(0, query_count, thread_num, query_function, table_name);
        profiler.End();
        results.push_back(Format("Total cost : {}", profiler.ElapsedToString(1000)));
        {
            size_t correct_1 = 0, correct_10 = 0, correct_100 = 0;
            for (size_t query_idx = 0; query_idx < query_count; ++query_idx) {
                auto &result = query_results[query_idx];
                auto &ground_truth_1 = ground_truth_sets_1[query_idx];
                auto &ground_truth_10 = ground_truth_sets_10[query_idx];
                auto &ground_truth_100 = ground_truth_sets_100[query_idx];
                for (size_t i = 0; i < result.size(); ++i) {
                    if (i < 1 and ground_truth_1.contains(result[i])) {
                        ++correct_1;
                    }
                    if (i < 10 and ground_truth_10.contains(result[i])) {
                        ++correct_10;
                    }
                    if (i < 100 and ground_truth_100.contains(result[i])) {
                        ++correct_100;
                    }
                }
            }
            results.push_back(Format("R@1:   {:.3f}", float(correct_1) / float(query_count * 1)));
            results.push_back(Format("R@10:  {:.3f}", float(correct_10) / float(query_count * 10)));
            results.push_back(Format("R@100: {:.3f}", float(correct_100) / float(query_count * 100)));
        }
    } while (--total_times);

    std::cout << ">>> Query Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}
