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

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <random>

import hnsw_alg;
import file_system;
import file_system_type;
import local_file_system;
import file_system_type;
import plain_store;
import lvq_store;
import dist_func_l2;
import dist_func_ip;
import compilation_config;

using namespace infinity;

int main() {
    using LabelT = uint64_t;

    std::string save_dir = tmp_data_path();

    int dim = 16;
    int element_size = 1000;
    int M = 16;
    int ef_construction = 200;

    using Hnsw = KnnHnsw<float, LabelT, PlainStore<float>, PlainL2Dist<float>>;
    // using Hnsw = KnnHnsw<float, LabelT, LVQStore<float, int8_t, LVQL2Cache<float, int8_t>>, LVQL2Dist<float, int8_t>>;

    // NOTE: inner product correct rate is not 1. (the vector and itself's distance is not the smallest)
    // using Hnsw = KnnHnsw<float, LabelT, PlainStore<float>, PlainIPDist<float>>;
    // using Hnsw = KnnHnsw<float, LabelT, LVQStore<float, int8_t, LVQIPCache<float, int8_t>>, LVQIPDist<float, int8_t>>;

    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;

    auto data = std::make_unique<float[]>(dim * element_size);
    for (int i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    LocalFileSystem fs;

    {
        auto hnsw_index = Hnsw::Make(element_size, dim, M, ef_construction, {});

        auto labels = std::make_unique<LabelT[]>(element_size);
        std::iota(labels.get(), labels.get() + element_size, 0);
        hnsw_index->InsertVecs(data.get(), labels.get(), element_size);
        std::ofstream os("tmp/dump.txt");
        hnsw_index->Dump(os);
        hnsw_index->Check();
        return 0;

        hnsw_index->SetEf(10);
        int correct = 0;
        for (int i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            auto result = hnsw_index->KnnSearch(query, 1);
            if (result[0].second == (LabelT)i) {
                ++correct;
            }
        }
        printf("correct rage: %f\n", float(correct) / element_size);

        uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kWriteLock);
        hnsw_index->Save(*file_handler);
        file_handler->Close();
    }

    {
        uint8_t file_flags = FileFlags::READ_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kReadLock);

        auto hnsw_index = Hnsw::Load(*file_handler, {});
        hnsw_index->SetEf(10);

        // hnsw_index->Dump(std::cout);
        hnsw_index->Check();
        int correct = 0;
        for (int i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            auto result = hnsw_index->KnnSearch(query, 1);
            if (result[0].second == (LabelT)i) {
                ++correct;
            }
        }
        printf("correct rage: %f\n", float(correct) / element_size);
        file_handler->Close();
    }
}