#include "base_profiler.h"
#include "helper.h"

import stl;
import std;
import knn_hnsw;
import dist_func;
import local_file_system;

static const char *sift1m_base = "./test/data/fvecs/sift_base.fvecs";
static const char *sift1m_query = "./test/data/fvecs/sift_query.fvecs";
static const char *sift1m_ground_truth = "./test/data/fvecs/sift_groundtruth.ivecs";

using namespace infinity;

int main() {
    size_t dimension = 128;
    size_t M = 16;
    size_t ef_construction = 200;

    std::unique_ptr<KnnHnsw<float>> knn_hnsw = nullptr;
    std::string save_place = "./tmp/my.hnsw";
    std::ifstream f(save_place);
    if (!f.good()) {
        std::cout << "Build index" << std::endl;
        size_t embedding_count = 1000000;
        float *input_embeddings = fvecs_read(sift1m_base, &dimension, &embedding_count);
        assert(dimension == 128 || !"embedding dimension isn't 128");
        assert(embedding_count == 1000000 || !"embedding size isn't 1000000");

        DistFuncL2<float> space(dimension);
        knn_hnsw = std::make_unique<KnnHnsw<float>>(embedding_count, dimension, space, M, ef_construction);

        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        for (size_t idx = 0; idx < embedding_count; ++idx) {
            // insert data into index
            const float *query = input_embeddings + idx * dimension;
            knn_hnsw->Insert(query, idx);
            if (idx % 10000 == 0) {
                std::cout << idx << ", " << get_current_rss() / 1000000 << " MB, " << profiler.ElapsedToString() << std::endl;
            }
        }
        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;

        delete[] input_embeddings;
        knn_hnsw->SaveIndex(save_place, MakeUnique<LocalFileSystem>());
    } else {
        std::cout << "Load index from " << save_place << std::endl;
        DistFuncL2<float> space(dimension);
        knn_hnsw = KnnHnsw<float>::LoadIndex(save_place, MakeUnique<LocalFileSystem>(), space);
    }

    size_t number_of_queries;
    const float *queries = nullptr;
    {
        size_t dim = -1;
        queries = const_cast<const float *>(fvecs_read(sift1m_query, &dim, &number_of_queries));
        assert(dimension == dim || !"query does not have same dimension as train set");
    }

    size_t top_k;                           // nb of results per query in the GT
    Vector<HashSet<int>> ground_truth_sets; // number_of_queries * top_k matrix of ground-truth nearest-neighbors

    {
        // size_t *ground_truth;
        // load ground-truth and convert int to long
        size_t nq2;
        int *gt_int = ivecs_read(sift1m_ground_truth, &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth_sets.resize(number_of_queries);
        for (int i = 0; i < number_of_queries; ++i) {
            for (int j = 0; j < top_k; ++j) {
                ground_truth_sets[i].insert(gt_int[i * top_k + j]);
            }
        }
    }

    {
        size_t round_n = 100;
        size_t thread_n = 16;
        Vector<std::thread> threads;
        au64 global_idx = 0;
        au64 n_valid = 0;

        knn_hnsw->SetEf(ef_construction);
        infinity::BaseProfiler profiler;
        profiler.Begin();

        for (size_t thread_idx = 0; thread_idx < thread_n; ++thread_idx) {
            threads.emplace_back([&]() {
                while (true) {
                    u64 idx = global_idx.fetch_add(1);
                    if (idx >= round_n * number_of_queries) {
                        break;
                    }
                    if (idx % 10000 == 0) {
                        std::cout << idx << ", " << profiler.ElapsedToString() << std::endl;
                    }

                    const float *query = queries + (idx % number_of_queries) * dimension;
                    MaxHeap<Pair<float, unsigned int>> result = knn_hnsw->KnnSearch(query, top_k);
                    int correct = 0;
                    while (!result.empty()) {
                        if (ground_truth_sets[idx % number_of_queries].contains(result.top().second)) {
                            ++correct;
                        }
                        result.pop();
                    }
                    n_valid.fetch_add(correct);
                }
            });
        }

        for (auto &thread : threads) {
            thread.join();
        }

        profiler.End();
        printf("Recall = %.4f, Spend: %s\n", n_valid / float(top_k * number_of_queries * round_n), profiler.ElapsedToString().c_str());
    }

    delete[] queries;
}