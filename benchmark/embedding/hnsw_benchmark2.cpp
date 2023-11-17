#include "base_profiler.h"
#include "helper.h"
#include <fstream>
#include <iostream>
#include <thread>

import stl;
import knn_hnsw;
import dist_func_l2;
import local_file_system;

static const char *sift1m_base = "/home/shenyushi/Documents/Code/infiniflow/infinity/test/data/fvecs/sift_base.fvecs";
static const char *sift1m_query = "/home/shenyushi/Documents/Code/infiniflow/infinity/test/data/fvecs/sift_query.fvecs";
static const char *sift1m_groundtruth = "/home/shenyushi/Documents/Code/infiniflow/infinity/test/data/fvecs/sift_groundtruth.ivecs";

static const char *deep10m_base = "/home/shenyushi/Downloads/data/base.10M.fvecs";
static const char *deep10m_query = "/home/shenyushi/Downloads/data/query.public.10K.fvecs";
static const char *deep10m_groundtruth = "/home/shenyushi/Downloads/data/groundtruth.public.10K.ivecs";

static const char *base_file = deep10m_base;
static const char *query_file = deep10m_query;
static const char *groundtruth_file = deep10m_groundtruth;

using namespace infinity;

int main() {
    size_t dimension = 96;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t embedding_count = 10000000;
    size_t test_top = 100;

    using LabelT = uint64_t;

    std::unique_ptr<KnnHnsw<float, LabelT>> knn_hnsw = nullptr;
    std::string save_place = "./tmp/my.hnsw";
    std::ifstream f(save_place);
    if (!f.good()) {
        std::cout << "Build index" << std::endl;

        float *input_embeddings = fvecs_read(base_file, &dimension, &embedding_count);
        assert(dimension == 96 || !"embedding dimension isn't correct");
        assert(embedding_count == 10000000 || !"embedding size isn't correct");

        DistFuncL2<float> space(dimension);
        knn_hnsw = std::make_unique<KnnHnsw<float, LabelT>>(embedding_count, dimension, space, M, ef_construction);

        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() << "B" << std::endl;
        profiler.Begin();
        for (size_t idx = 0; idx < embedding_count; ++idx) {
            // insert data into index
            const float *query = input_embeddings + idx * dimension;
            knn_hnsw->Insert(query, idx);
            if (idx % 100000 == 0) {
                std::cout << idx << ", " << get_current_rss() << " B, " << profiler.ElapsedToString() << std::endl;
            }
        }
        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() << "B" << std::endl;

        delete[] input_embeddings;
        knn_hnsw->SaveIndex(save_place, MakeUnique<LocalFileSystem>());
    } else {
        std::cout << "Load index from " << save_place << std::endl;
        DistFuncL2<float> space(dimension);
        knn_hnsw = KnnHnsw<float, LabelT>::LoadIndex(save_place, MakeUnique<LocalFileSystem>(), space);
    }

    size_t number_of_queries;
    const float *queries = nullptr;
    {
        size_t dim = -1;
        queries = const_cast<const float *>(fvecs_read(query_file, &dim, &number_of_queries));
        assert(dimension == dim || !"query does not have same dimension as train set");
    }

    size_t top_k; // nb of results per query in the GT
    Vector<HashSet<int>> ground_truth_sets; // number_of_queries * top_k matrix of ground-truth nearest-neighbors

    {
        // size_t *ground_truth;
        // load ground-truth and convert int to long
        size_t nq2;
        int *gt_int = ivecs_read(groundtruth_file, &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");
        assert(top_k >= test_top || !"dataset does not provide enough ground truth data");

        ground_truth_sets.resize(number_of_queries);
        for (int i = 0; i < number_of_queries; ++i) {
            for (int j = 0; j < test_top; ++j) {
                ground_truth_sets[i].insert(gt_int[i * top_k + j]);
            }
        }
    }

    // if (false) {
    //     size_t round_n = 100;
    //     size_t thread_n = 16;
    //     Vector<std::thread> threads;
    //     atomic_u64 global_idx = 0;
    //     atomic_u64 n_valid = 0;

    //     knn_hnsw->SetEf(ef_construction);
    //     infinity::BaseProfiler profiler;
    //     profiler.Begin();

    //     for (size_t thread_idx = 0; thread_idx < thread_n; ++thread_idx) {
    //         threads.emplace_back([&]() {
    //             while (true) {
    //                 u64 idx = global_idx.fetch_add(1);
    //                 if (idx >= round_n * number_of_queries) {
    //                     break;
    //                 }
    //                 if (idx % 10000 == 0) {
    //                     std::cout << idx << ", " << profiler.ElapsedToString() << std::endl;
    //                 }

    //                 const float *query = queries + (idx % number_of_queries) * dimension;
    //                 MaxHeap<Pair<float, LabelT>> result = knn_hnsw->KnnSearch(query, test_top);
    //                 int correct = 0;
    //                 while (!result.empty()) {
    //                     if (ground_truth_sets[idx % number_of_queries].contains(result.top().second)) {
    //                         ++correct;
    //                     }
    //                     result.pop();
    //                 }
    //                 n_valid.fetch_add(correct);
    //             }
    //         });
    //     }

    //     for (auto &thread : threads) {
    //         thread.join();
    //     }

    //     profiler.End();
    //     printf("Recall = %.4f, Spend: %s\n", n_valid / float(test_top * number_of_queries * round_n), profiler.ElapsedToString().c_str());
    // }

    if (true) {
        infinity::BaseProfiler profiler;
        int round = 10;
        for (int ef = 100; ef <= 800; ef += 100) {
            int correct = 0;

            knn_hnsw->SetEf(ef);
            Vector<MaxHeap<Pair<float, LabelT>>> results;
            profiler.Begin();
            for (int idx = 0; idx < number_of_queries * round; ++idx) {
                const float *query = queries + (idx % number_of_queries) * dimension;
                MaxHeap<Pair<float, LabelT>> result = knn_hnsw->KnnSearch(query, test_top);
                results.push_back(std::move(result));
            }
            profiler.End();
            for (int idx = 0; idx < number_of_queries; ++idx) {
                auto &result = results[idx % number_of_queries];
                while (!result.empty()) {
                    if (ground_truth_sets[idx % number_of_queries].contains(result.top().second)) {
                        ++correct;
                    }
                    result.pop();
                }
            }
            printf("ef = %d, Spend: %s\n", ef, profiler.ElapsedToString().c_str());
            printf("Recall = %.4f\n", correct / float(test_top * number_of_queries));
        }
    }

    delete[] queries;
}