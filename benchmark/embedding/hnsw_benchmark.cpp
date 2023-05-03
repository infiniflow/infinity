//
// Created by jinhai on 23-5-1.
//

#define ANKERL_NANOBENCH_IMPLEMENT

#include "nanobench.h"
#include "hnswlib/hnswlib.h"
#include "common/types/internal_types.h"
#include "main/profiler/base_profiler.h"
#include "helper.h"

static const char* sift1m_base = "/home/jinhai/Documents/data/sift1M/sift_base.fvecs";
static const char* sift1m_train = "/home/jinhai/Documents/data/sift1M/sift_learn.fvecs";
static const char* sift1m_query = "/home/jinhai/Documents/data/sift1M/sift_query.fvecs";
static const char* sift1m_ground_truth = "/home/jinhai/Documents/data/sift1M/sift_groundtruth.ivecs";

using namespace infinity;

auto main () -> int {
    SizeT dimension;
    SizeT embedding_count;
    SizeT M = 16;
    SizeT ef_construction = 200;
    float* input_embeddings = nullptr;
    {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        input_embeddings = fvecs_read(sift1m_base, &dimension, &embedding_count);
        profiler.End();
        std::cout << "Load sift1M base data: " << profiler.ElapsedToString() << std::endl;
    }

    assert(dimension == 128 || !"embedding dimension isn't 128");
    assert(embedding_count == 10000000 || !"embedding size isn't 1000000");
    hnswlib::L2Space l2space(dimension);
    hnswlib::HierarchicalNSW<float>* hnsw_index = nullptr;
    {
        SizeT max_elements = 10000000;        // create index
        hnsw_index = new hnswlib::HierarchicalNSW<float>(&l2space, max_elements, M, ef_construction);
    }

    {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        // insert data into index
        for(SizeT idx = 0; idx < embedding_count; ++ idx) {
            hnsw_index->addPoint(input_embeddings + idx * dimension, idx);

            if (idx % 100000 == 0) {
                std::cout << idx << ", " << get_current_rss() / 1000000 << " MB, "
                          << profiler.ElapsedToString() << std::endl;
            }
        }
        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString()
                  << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
    }

    size_t number_of_queries;
    float* queries;
    {
        size_t dim;
        infinity::BaseProfiler profiler;
        profiler.Begin();
        queries = fvecs_read(sift1m_query, &dim, &number_of_queries);
        assert(d == dim || !"query does not have same dimension as train set");
        profiler.End();
        std::cout << "Load sift1M query data: " << profiler.ElapsedToString() << std::endl;
    }

    size_t top_k;         // nb of results per query in the GT
    size_t* ground_truth; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    {
        // load ground-truth and convert int to long
        size_t nq2;
        infinity::BaseProfiler profiler;
        profiler.Begin();

        int* gt_int = ivecs_read(sift1m_ground_truth, &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth = new size_t[top_k * number_of_queries];
        for (int i = 0; i < top_k * number_of_queries; ++ i) {
            ground_truth[i] = gt_int[i];
        }
        delete[] gt_int;
        profiler.End();
        std::cout << "Load sift1M ground truth and load row id: " << profiler.ElapsedToString() << std::endl;
    }

    {
        // Query and get result;
        SizeT n_valid = 0;
        hnsw_index->setEf(ef_construction);
        for (int i = 0; i < number_of_queries; i++) {
            std::priority_queue<std::pair<float, hnswlib::labeltype>> result
                = hnsw_index->searchKnn(queries + i * dimension, top_k);
            assert(top_k == result.size()  || !"incorrect topk value");

            std::unordered_set<SizeT> ground_truth_set;
            for(int j = 0; j < top_k; ++ j) {
                ground_truth_set.insert(ground_truth[i * top_k + j]);
            }

            for(int j = 0; j < top_k; ++ j) {
                if(ground_truth_set.contains(result.top().second)) {
                    ++ n_valid;
                }
                result.pop();
            }
        }

        printf("Recall = %.4f\n", n_valid / float(top_k * number_of_queries));
    }

    {
        delete[] input_embeddings;
        delete[] queries;
        delete[] ground_truth;
        delete hnsw_index;
    }

    return 0;
#if 0
    int dim = 16;               // Dimension of the elements
    int max_elements = 10000;   // Maximum number of elements, should be known beforehand
    int M = 16;                 // Tightly connected with internal dimensionality of the data
    // strongly affects the memory consumption
    int ef_construction = 200;  // Controls index search speed/build speed tradeoff

    // Initing index
    hnswlib::L2Space space(dim);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = new hnswlib::HierarchicalNSW<float>(&space, max_elements, M, ef_construction);

    // Generate random data
    std::mt19937 rng;
    rng.seed(47);
    std::uniform_real_distribution<> distrib_real;
    float* data = new float[dim * max_elements];
    for (int i = 0; i < dim * max_elements; i++) {
        data[i] = distrib_real(rng);
    }

    // Add data to index
    for (int i = 0; i < max_elements; i++) {
        alg_hnsw->addPoint(data + i * dim, i);
    }

    // Query the elements for themselves and measure recall
    float correct = 0;
    for (int i = 0; i < max_elements; i++) {
        std::priority_queue<std::pair<float, hnswlib::labeltype>> result = alg_hnsw->searchKnn(data + i * dim, 1);
        hnswlib::labeltype label = result.top().second;
        if (label == i) correct++;
    }
    float recall = correct / max_elements;
    std::cout << "Recall: " << recall << "\n";

    // Serialize index
    std::string hnsw_path = "hnsw.bin";
    alg_hnsw->saveIndex(hnsw_path);
    delete alg_hnsw;

    // Deserialize index and check recall
    alg_hnsw = new hnswlib::HierarchicalNSW<float>(&space, hnsw_path);
    correct = 0;
    for (int i = 0; i < max_elements; i++) {
        std::priority_queue<std::pair<float, hnswlib::labeltype>> result = alg_hnsw->searchKnn(data + i * dim, 1);
        hnswlib::labeltype label = result.top().second;
        if (label == i) correct++;
    }
    recall = (float)correct / max_elements;
    std::cout << "Recall of deserialized index: " << recall << "\n";

    delete[] data;
    delete alg_hnsw;
#endif
    return 0;
}