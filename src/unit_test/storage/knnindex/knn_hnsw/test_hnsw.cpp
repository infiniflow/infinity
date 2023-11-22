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

#include "unit_test/base_test.h"

#include <cstdio>
#include <memory>
#include <queue>
#include <random>

import local_file_system;
import hnsw_alg;
import dist_func;

using namespace infinity;

class KnnHnswTest : public BaseTest {};

TEST_F(KnnHnswTest, test1) {
    using namespace infinity;

    using LabelT = uint64_t;
    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real;

    int dim = 16;
    int element_size = 300;

    // auto data = std::make_unique<float[]>(dim * element_size);
    // for (int i = 0; i < dim * element_size; ++i) {
    //     data[i] = distrib_real(rng);
    // }

    // int M = 16;
    // int ef_construction = 200;
    // DistFuncL2<float> space(dim);
    // {
    //     auto hnsw_index = std::make_unique<KnnHnsw<float, LabelT>>(element_size, dim, space, M, ef_construction);

    //     for (int i = 0; i < element_size; ++i) {
    //         const float *query = data.get() + i * dim;
    //         hnsw_index->Insert(query, unsigned(i));
    //         hnsw_index->CheckGraph();
    //     }

    //     hnsw_index->SaveIndex("./tmp/test.index", std::make_unique<LocalFileSystem>());
    // }

    // {
    //     auto hnsw_index = KnnHnsw<float, LabelT>::LoadIndex("./tmp/test.index", std::make_unique<LocalFileSystem>(), space);
    //     hnsw_index->CheckGraph();
    //     hnsw_index->SetEf(ef_construction);
    //     int correct = 0;
    //     for (int i = 0; i < element_size; ++i) {
    //         const float *query = data.get() + i * dim;
    //         std::priority_queue<std::pair<float, LabelT>> result = hnsw_index->KnnSearch(query, 1);
    //         if (result.top().second == i) {
    //             ++correct;
    //         }
    //     }
    //     printf("correct rage: %f\n", float(correct) / element_size);
    // }
}