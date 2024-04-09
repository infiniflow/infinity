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
#include <fstream>

import stl;
import hnsw_alg;
import file_system;
import file_system_type;
import local_file_system;
import file_system_type;
import data_store;
import dist_func_l2;
import dist_func_ip;
import compilation_config;
import vec_store_type;

using namespace infinity;

class HnswAlgTest : public BaseTest {
public:
    using LabelT = u64;

    template <typename Hnsw>
    void Test() {
        String save_dir = tmp_data_path();

        int dim = 16;
        int element_size = 1000;
        int M = 8;
        int ef_construction = 200;
        int chunk_size = 128;
        int max_chunk_n = 10;

        std::mt19937 rng;
        rng.seed(0);
        std::uniform_real_distribution<float> distrib_real;

        auto data = MakeUnique<float[]>(dim * element_size);
        for (int i = 0; i < dim * element_size; ++i) {
            data[i] = distrib_real(rng);
        }

        LocalFileSystem fs;

        {
            auto hnsw_index = Hnsw::Make(chunk_size, max_chunk_n, dim, M, ef_construction);

            hnsw_index->InsertVecsRaw(data.get(), element_size);
            // std::ofstream os("tmp/dump.txt");
            // hnsw_index->Dump(os);
            hnsw_index->Check();

            hnsw_index->SetEf(10);
            int correct = 0;
            for (int i = 0; i < element_size; ++i) {
                const float *query = data.get() + i * dim;
                auto result = hnsw_index->KnnSearchSorted(query, 1);
                if (result[0].second == (LabelT)i) {
                    ++correct;
                }
            }
            float correct_rate = float(correct) / element_size;
            // std::printf("correct rage: %f\n", correct_rate);
            EXPECT_GE(correct_rate, 0.95);

            u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            UniquePtr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);
            hnsw_index->Save(*file_handler);
            file_handler->Close();
        }

        {
            u8 file_flags = FileFlags::READ_FLAG;
            UniquePtr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);

            auto hnsw_index = Hnsw::Load(*file_handler);
            hnsw_index->SetEf(10);

            // std::ofstream os("tmp/dump2.txt");
            // hnsw_index->Dump(os);
            hnsw_index->Check();
            int correct = 0;
            for (int i = 0; i < element_size; ++i) {
                const float *query = data.get() + i * dim;
                auto result = hnsw_index->KnnSearchSorted(query, 1);
                if (result[0].second == (LabelT)i) {
                    ++correct;
                }
            }
            float correct_rate = float(correct) / element_size;
            // std::printf("correct rage: %f\n", correct_rate);
            EXPECT_GE(correct_rate, 0.95);

            file_handler->Close();
        }
    }
};

TEST_F(HnswAlgTest, test1) {
    // NOTE: inner product correct rate is not 1. (the vector and itself's distance is not the smallest)
    {
        using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
        Test<Hnsw>();
    }
    {
        using Hnsw = KnnHnsw<LVQL2VecStoreType<float, int8_t>, LabelT>;
        Test<Hnsw>();
    }
}