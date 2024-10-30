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

#include <fstream>
#include <gtest/gtest.h>

import stl;
import base_test;
import infinity_exception;
import knn_diskann;
import internal_types;
import virtual_store;
import index_base;
import diskann_index_data;
import local_file_handle;
import pq_flash_index;
import diskann_dist_func;

using namespace infinity;

class DiskAnnTest : public BaseTest {
public:
    const std::string save_dir_ = GetFullTmpDir();

    template <typename DiskAnnIndexDataType, typename PqFlashIndexType>
    void TestCreateIndex() {
        u32 dim = 100;
        u32 num_points = 10000;
        u32 R = 32;
        u32 L = 100;
        u32 num_pq_chunks = 4;
        u32 num_parts = 1;
        u32 num_centers = 256;

        Vector<SizeT> labels(num_points);
        for (u32 i = 0; i < num_points; i++) {
            labels[i] = i;
        }
        auto data = MakeUnique<f32[]>(dim * num_points);
        std::mt19937 rng;
        rng.seed(0);
        std::uniform_real_distribution<float> distrib_real;
        for (u32 i = 0; i < dim * num_points; i++) {
            data[i] = distrib_real(rng);
        }

        // Build index
        {
            std::string data_file_path = save_dir_ + "/data.bin";
            std::string mem_index_path = save_dir_ + "/mem_index.bin";
            std::string index_file_path = save_dir_ + "/index.bin";
            std::string pqCompressed_data_path = save_dir_ + "/pqCompressed_data.bin";
            std::string pq_pivot_data_path = save_dir_ + "/pq_pivot.bin";
            std::string sample_data_path = save_dir_;
            auto [data_file_handle, status] = VirtualStore::Open(data_file_path, FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            data_file_handle->Append(data.get(), sizeof(f32) * dim * num_points);
            {
                auto disk_data = DiskAnnIndexDataType::Make(dim, num_points, R, L, num_pq_chunks, num_parts, num_centers);

                disk_data->BuildIndex(dim,
                                      num_points,
                                      labels,
                                      Path(data_file_path),
                                      Path(mem_index_path),
                                      Path(index_file_path),
                                      Path(pqCompressed_data_path),
                                      Path(sample_data_path),
                                      Path(pq_pivot_data_path));
                // disk_data->UnitTest();
            }
        }

        // Test search index
        {
            DiskAnnMetricType metric_type = DiskAnnMetricType::L2;
            u64 num_nodes_to_cache = 1000;
            auto pq_flash_index = PqFlashIndexType::Make(metric_type, dim, num_points, num_pq_chunks);

            std::string index_prefix = save_dir_;
            // 1. load index
            pq_flash_index->Load(index_prefix);
            // 2. cache node by BFS order
            std::vector<SizeT> cache_node_list;
            pq_flash_index->CacheBfsLevels(num_nodes_to_cache, cache_node_list);
            // 3. load cached nodes from cache_node_list
            pq_flash_index->LoadCacheList(cache_node_list);
            // 4. search and coompute recall
            u64 hits = 0;
            u32 num_queries = 10000;
            std::vector<u32> hit_labels;
            u64 beam_width = 2;
            UniquePtr<u64[]> indices = MakeUnique<u64[]>(1);
            UniquePtr<f32[]> distances = MakeUnique<f32[]>(1);
            for (u32 i = 0; i < num_queries; i++) {
                pq_flash_index->CachedBeamSearch(data.get() + i * dim, 1, 100, indices.get(), distances.get(), beam_width);
                if (indices[0] == i) {
                    hits++;
                    hit_labels.push_back(i);
                }
            }
            float recall = (float)hits / num_queries;
            EXPECT_GT(recall, 0.95);
        }
    }
};

TEST_F(DiskAnnTest, test1) {
    using DiskAnnIndexData = DiskAnnIndexData<f32, SizeT, MetricType::kMetricL2>;
    using PqFlashIndexType = PqFlashIndex<f32, SizeT>;
    TestCreateIndex<DiskAnnIndexData, PqFlashIndexType>();
}