#include "unit_test/base_test.h"

import std;
// import stl;
import knn_knsw;
import dist_func;

class KnnHnswTest : public BaseTest {};

TEST_F(KnnHnswTest, test2) {
#if 0
    using namespace infinity;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real;

    int dim = 16;
    int element_size = 1000;

    auto data = std::make_unique<float[]>(dim * element_size);
    for (int i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    int M = 16;
    int ef_construction = 200;
    DistFuncL2<float> space(dim);
    KnnHnsw<float> hnsw_index(element_size, dim, space, M, ef_construction);

    for (int i = 0; i < element_size; ++i) {
        const float *query = data.get() + i * dim;
        hnsw_index.Insert(query, unsigned(i));
        hnsw_index.CheckGraph();
    }

    hnsw_index.SetEf(ef_construction);
    int correct = 0;
    for (int i = 0; i < element_size; ++i) {
        const float *query = data.get() + i * dim;
        // std::priority_queue<std::pair<float, unsigned int>> result = hnsw_index.KnnSearch(query, 1);
        // unsigned int label = result.top().second;
        // if (label == i) {
        //     ++correct;
        // }
    }
    float recall = (float)correct / element_size;
    // assert(recall > 0.9);
    // std::cout << "Recall: " << recall << std::endl;
#endif
}