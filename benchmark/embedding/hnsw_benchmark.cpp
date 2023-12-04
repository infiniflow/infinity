//
// Created by jinhai on 23-5-1.
//

#define ANKERL_NANOBENCH_IMPLEMENT

#include "base_profiler.h"
#include "helper.h"
#include "hnswlib/hnswlib.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <thread>

import compilation_config;

using namespace infinity;

auto main() -> int {
    std::string base_file = std::string(benchmark_data_path()) + "/sift/base.fvecs";
    std::string query_file = std::string(benchmark_data_path()) + "/sift/query.fvecs";
    std::string groundtruth_file = std::string(benchmark_data_path()) + "/sift/l2_groundtruth.ivecs";

    std::string save_dir = tmp_data_path();
    std::string hnsw_index_l2_name = save_dir + "/the_sift_ip.hnsw";
    size_t dimension;
    size_t embedding_count;
    size_t M = 16;
    size_t ef_construction = 200;
    float *input_embeddings = nullptr;
    size_t max_elements = 1000000; // create index
    const int thread_n = 8;

    {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        input_embeddings = fvecs_read(base_file.c_str(), &dimension, &embedding_count);
        profiler.End();
        std::cout << "Load sift1M base data: " << profiler.ElapsedToString() << std::endl;
    }

    // assert(dimension == 128 || !"embedding dimension isn't 128");
    // assert(embedding_count == 1000000 || !"embedding size isn't 1000000");
    // hnswlib::L2Space space(dimension);
    hnswlib::InnerProductSpace space(dimension);
    hnswlib::HierarchicalNSW<float> *hnsw_index = nullptr;

    std::ifstream f(hnsw_index_l2_name);
    if (f.good()) {
        // Found index file
        std::cout << "Found index file ... " << std::endl;
        hnsw_index = new hnswlib::HierarchicalNSW<float>(&space, hnsw_index_l2_name);
    } else {
        hnsw_index = new hnswlib::HierarchicalNSW<float>(&space, max_elements, M, ef_construction);

        infinity::BaseProfiler profiler;
        profiler.Begin();
        // insert data into index
        for (size_t idx = 0; idx < embedding_count; ++idx) {
            hnsw_index->addPoint(input_embeddings + idx * dimension, idx);
            if (idx % 10000 == 0) {
                std::cout << idx << ", " << get_current_rss() / 1000000 << " MB, " << profiler.ElapsedToString() << std::endl;
            }
        }
        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        hnsw_index->saveIndex(hnsw_index_l2_name);
    }

    size_t number_of_queries;
    float *queries;
    {
        size_t dim;
        infinity::BaseProfiler profiler;
        profiler.Begin();
        queries = fvecs_read(query_file.c_str(), &dim, &number_of_queries);
        assert(dimension == dim || !"query does not have same dimension as train set");
        profiler.End();
        std::cout << "Load sift1M query data: " << profiler.ElapsedToString() << std::endl;
    }

    size_t top_k;                                           // nb of results per query in the GT
    std::vector<std::unordered_set<int>> ground_truth_sets; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    {
        // load ground-truth and convert int to long
        size_t nq2;
        infinity::BaseProfiler profiler;
        profiler.Begin();

        int *gt_int = ivecs_read(groundtruth_file.c_str(), &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth_sets.resize(number_of_queries);
        for (int i = 0; i < number_of_queries; ++i) {
            for (int j = 0; j < top_k; ++j) {
                ground_truth_sets[i].insert(gt_int[i * top_k + j]);
            }
        }
        delete[] gt_int;
        profiler.End();
        std::cout << "Load sift1M ground truth and load row id: " << profiler.ElapsedToString() << std::endl;
    }

    infinity::BaseProfiler profiler;
    int round = 10;
    std::vector<std::priority_queue<std::pair<float, unsigned long>>> results(number_of_queries);
    std::cout << "thread number: " << thread_n << std::endl;
    for (int ef = 100; ef <= 300; ef += 25) {
        hnsw_index->setEf(ef);
        int correct = 0;
        int sum_time = 0;
        for (size_t i = 0; i < round; ++i) {
            std::atomic_int idx(0);
            std::vector<std::thread> threads;
            profiler.Begin();
            for (int j = 0; j < thread_n; ++j) {
                threads.emplace_back([&]() {
                    while (true) {
                        int cur_idx = idx.fetch_add(1);
                        if (cur_idx >= number_of_queries) {
                            break;
                        }
                        const float *query = queries + cur_idx * dimension;
                        auto result = hnsw_index->searchKnn(query, top_k);
                        results[cur_idx] = std::move(result);
                    }
                });
            }
            for (auto &thread : threads) {
                thread.join();
            }
            profiler.End();
            if (i == 0) {
                for (int idx = 0; idx < number_of_queries; ++idx) {
                    auto &result = results[idx];
                    while (!result.empty()) {
                        if (ground_truth_sets[idx].contains(result.top().second)) {
                            ++correct;
                        }
                        result.pop();
                    }
                }
                printf("Recall = %.4f\n", correct / float(top_k * number_of_queries));
            }
            sum_time += profiler.ElapsedToMs();
        }
        sum_time /= round;
        printf("ef = %d, Spend: %d\n", ef, sum_time);

        std::cout << "----------------------------" << std::endl;
    }

    {
        delete[] input_embeddings;
        delete[] queries;
        delete hnsw_index;
    }

    return 0;
}
