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

#include "InfinityService.h"
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <unordered_set>

import compilation_config;

import profiler;
import local_file_system;
import third_party;
import statement_common;
import internal_types;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace infinity_thrift_rpc;

struct InfinityClient {
    std::shared_ptr<TTransport> socket;
    std::shared_ptr<TTransport> transport;
    std::shared_ptr<TProtocol> protocol;
    std::unique_ptr<InfinityServiceClient> client;
    int64_t session_id;
    InfinityClient() {
        socket.reset(new TSocket("127.0.0.1", 23817));
        transport.reset(new TBufferedTransport(socket));
        protocol.reset(new TBinaryProtocol(transport));
        client = std::make_unique<InfinityServiceClient>(protocol);
        transport->open();
        CommonResponse response;
        ConnectRequest request;
        request.__set_client_version(9); // 0.2.1
        client->Connect(response, request);
        session_id = response.session_id;
    }
    ~InfinityClient() {
        CommonResponse ret;
        CommonRequest req;
        req.session_id = session_id;
        client->Disconnect(ret, req);
        transport->close();
    }
};

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

inline void LoopFor(size_t id_begin, size_t id_end, size_t threadId, auto fn) {
    std::cout << "threadId = " << threadId << " [" << id_begin << ", " << id_end << ")" << std::endl;
    InfinityClient client;
    for (auto id = id_begin; id < id_end; ++id) {
        fn(id, client, threadId);
    }
}

inline void ParallelFor(size_t start, size_t end, size_t numThreads, auto fn) {
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    size_t avg_cnt = (end - start) / numThreads;
    size_t extra_cnt = (end - start) % numThreads;
    for (size_t id_begin = start, threadId = 0; threadId < numThreads; ++threadId) {
        size_t id_end = id_begin + avg_cnt + (threadId < extra_cnt);
        threads.emplace_back([id_begin, id_end, threadId, fn] { LoopFor(id_begin, id_end, threadId, fn); });
        id_begin = id_end;
    }

    for(auto& thread: threads) {
        thread.join();
    }
}

int main() {
    size_t thread_num = 1;
    size_t total_times = 1;
    size_t ef = 100;
    std::cout << "Please input thread_num, 0 means use all resources:" << std::endl;
    std::cin >> thread_num;
    std::cout << "Please input total_times:" << std::endl;
    std::cin >> total_times;
    std::cout << "Please input ef:" << std::endl;
    std::cin >> ef;

    infinity::LocalFileSystem fs;

    std::cout << ">>> Query Benchmark Start <<<" << std::endl;
    std::cout << "Thread Num: " << thread_num << ", Times: " << total_times << std::endl;

    std::vector<std::string> results;

    std::string sift_query_path = std::string(infinity::test_data_path()) + "/benchmark/sift_1m/sift_query.fvecs";
    std::string sift_groundtruth_path = std::string(infinity::test_data_path()) + "/benchmark/sift_1m/sift_groundtruth.ivecs";
    if (!fs.Exists(sift_query_path)) {
        std::cerr << "File: " << sift_query_path << " doesn't exist" << std::endl;
        exit(-1);
    }
    if (!fs.Exists(sift_groundtruth_path)) {
        std::cerr << "File: " << sift_groundtruth_path << " doesn't exist" << std::endl;
        exit(-1);
    }
    std::unique_ptr<float[]> queries_ptr;
    size_t query_count;
    int64_t dimension = 128;
    {
        int dim = -1;
        queries_ptr = load_data<float>(sift_query_path, query_count, dim);
        assert(dimension == dim || !"query vector dim isn't 128");
    }
    auto queries = queries_ptr.get();
    int64_t topk = 100;
    std::vector<std::unordered_set<int>> ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100;
    {
        std::unique_ptr<int[]> gt;
        size_t gt_count;
        int gt_top_k;
        {
            gt = load_data<int>(sift_groundtruth_path, gt_count, gt_top_k);
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
        auto query_function = [ef, dimension, topk, queries, &query_results](size_t query_idx, InfinityClient &client, size_t threadId) {
            SelectRequest req;
            SelectResponse ret;
            {
                req.session_id = client.session_id;
                req.__isset.session_id = true;
                req.db_name = "default_db";
                req.__isset.db_name = true;
                req.table_name = "sift_benchmark";
                req.__isset.table_name = true;
                ParsedExpr expr;
                {
                    expr.type.function_expr = std::make_shared<FunctionExpr>();
                    expr.type.function_expr->function_name = "row_id";
                    expr.type.function_expr->__isset.function_name = true;
                    expr.type.__isset.function_expr = true;
                    expr.__isset.type = true;
                }
                req.select_list.push_back(std::move(expr));
                req.__isset.select_list = true;
                KnnExpr knn_expr;
                {
                    knn_expr.column_expr.column_name.emplace_back("col1");
                    knn_expr.column_expr.__isset.column_name = true;
                    knn_expr.__isset.column_expr = true;
                    auto &q = knn_expr.embedding_data.f32_array_value;
                    q.reserve(dimension);
                    auto src_ptr = queries + query_idx * dimension;
                    for (int64_t i = 0; i < dimension; ++i) {
                        q.push_back(src_ptr[i]);
                    }
                    knn_expr.embedding_data.__isset.f32_array_value = true;
                    knn_expr.__isset.embedding_data = true;
                    knn_expr.embedding_data_type = ElementType::type::ElementFloat32;
                    knn_expr.__isset.embedding_data_type = true;
                    knn_expr.distance_type = KnnDistanceType::type::L2;
                    knn_expr.__isset.distance_type = true;
                    knn_expr.topn = topk;
                    knn_expr.__isset.topn = true;
                    InitParameter init_param;
                    {
                        init_param.param_name = "ef";
                        init_param.__isset.param_name = true;
                        init_param.param_value = std::to_string(ef);
                        init_param.__isset.param_value = true;
                    }
                    knn_expr.opt_params.push_back(std::move(init_param));
                    knn_expr.__isset.opt_params = true;
                }
                req.search_expr.knn_exprs.push_back(std::move(knn_expr));
                req.search_expr.__isset.knn_exprs = true;
                req.__isset.search_expr = true;
            }
            client.client->Select(ret, req);
            {
                auto select_result = (const infinity::RowID *)(ret.column_fields[0].column_vectors[0].data());
                for (int64_t i = 0; i < topk; ++i) {
                    query_results[query_idx].push_back(select_result[i].ToUint64());
                }
            }
        };
        infinity::BaseProfiler profiler;
        profiler.Begin();
        ParallelFor(0, query_count, thread_num, query_function);
        profiler.End();

        results.push_back(fmt::format("Total cost: {}", profiler.ElapsedToString(1000)));
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
            results.push_back(fmt::format("R@1:   {:.3f}", float(correct_1) / float(query_count * 1)));
            results.push_back(fmt::format("R@10:  {:.3f}", float(correct_10) / float(query_count * 10)));
            results.push_back(fmt::format("R@100: {:.3f}", float(correct_100) / float(query_count * 100)));
        }
    } while (--total_times);

    std::cout << ">>> Query Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
}
