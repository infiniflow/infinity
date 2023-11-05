#include "base_profiler.h"
#include "helper.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>

import knn_knsw;
import dist_func;

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
    {
        size_t embedding_count = 1000000;
        float *input_embeddings = fvecs_read(sift1m_base, &dimension, &embedding_count);
        assert(dimension == 128 || !"embedding dimension isn't 128");
        assert(embedding_count == 1000000 || !"embedding size isn't 1000000");

        DistFuncL2 space(dimension);
        knn_hnsw = std::make_unique<KnnHnsw<float>>(embedding_count, space, M, ef_construction);

        infinity::BaseProfiler profiler;
        std::cout << "Begin data cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        for (size_t idx = 0; idx < embedding_count; ++idx) {
            // insert data into index
            knn_hnsw->Insert(input_embeddings + idx * dimension, idx);
            if (idx % 10000 == 0) {
                std::cout << idx << ", " << get_current_rss() / 1000000 << " MB, " << profiler.ElapsedToString() << std::endl;
            }
        }
        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;

        delete[] input_embeddings;
    }

    size_t number_of_queries;
    const float *queries = nullptr;
    {
        size_t dim = -1;
        queries = const_cast<const float *>(fvecs_read(sift1m_query, &dim, &number_of_queries));
        assert(dimension == dim || !"query does not have same dimension as train set");
    }

    size_t top_k;         // nb of results per query in the GT
    size_t *ground_truth; // number_of_queries * top_k matrix of ground-truth nearest-neighbors

    {
        // load ground-truth and convert int to long
        size_t nq2;
        int *gt_int = ivecs_read(sift1m_ground_truth, &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth = new size_t[top_k * number_of_queries];
        for (int i = 0; i < top_k * number_of_queries; ++i) {
            ground_truth[i] = gt_int[i];
        }
        delete[] gt_int;
    }

    {
        // Query and get result;
        size_t n_valid = 0;
        knn_hnsw->SetEf(ef_construction);
        infinity::BaseProfiler profiler;
        profiler.Begin();
        for (int i = 0; i < number_of_queries; i++) {
            std::priority_queue<std::pair<float, unsigned int>> result = knn_hnsw->KnnSearch(queries + i * dimension, top_k);
            assert(top_k == result.size() || !"incorrect topk value");

            std::unordered_set<int> ground_truth_set;
            for (int j = 0; j < top_k; ++j) {
                ground_truth_set.insert(ground_truth[i * top_k + j]);
            }

            for (int j = 0; j < top_k; ++j) {
                if (ground_truth_set.contains(result.top().second)) {
                    ++n_valid;
                }
                result.pop();
            }
        }
        profiler.End();
        printf("Recall = %.4f, Spend: %s\n", n_valid / float(top_k * number_of_queries), profiler.ElapsedToString().c_str());
    }
    delete[] ground_truth;
    delete[] queries;
}