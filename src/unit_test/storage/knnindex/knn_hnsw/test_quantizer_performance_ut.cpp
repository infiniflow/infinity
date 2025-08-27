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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.test_quantizer_performance;

import :ut.base_test;
import :data_store;
import :vec_store_type;
import :dist_func_l2;

using namespace infinity;

class QuantizerPerformanceTest : public BaseTest {
public:
    using DataType = f32;
    using LabelType = i64;

public:
    void SetUp() override {}
    void TearDown() override {}

    template <typename T>
    std::tuple<size_t, size_t, std::unique_ptr<T[]>> DecodeFvecsDataset(const std::filesystem::path &path) {
        auto [file_handle, status] = VirtualStore::Open(path.string(), FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        i32 dim = 0;
        file_handle->Read(&dim, sizeof(dim));
        size_t file_size = file_handle->FileSize();
        size_t vec_num = file_size / (dim * sizeof(T) + sizeof(dim));
        auto data = std::make_unique_for_overwrite<T[]>(vec_num * dim);
        for (size_t i = 0; i < vec_num - 1; ++i) {
            file_handle->Read(data.get() + i * dim, dim * sizeof(T));
            i32 dim1 = 0;
            file_handle->Read(&dim1, sizeof(dim1));
            if (dim1 != dim) {
                UnrecoverableError("dim not match");
            }
        }
        file_handle->Read(data.get() + (vec_num - 1) * dim, dim * sizeof(T));
        return {vec_num, dim, std::move(data)};
    }

public:
    static constexpr size_t chunk_size_ = 8192;
    static constexpr size_t recall_at_ = 1;
    static constexpr size_t topk_ = 10;

    const std::string base_path_ = "test/data/benchmark/sift_1m/sift_base.fvecs";
    const std::string query_path_ = "test/data/benchmark/sift_1m/sift_query.fvecs";
    const std::string groundtruth_path_ = "test/data/benchmark/sift_1m/sift_groundtruth.ivecs";
};

template <typename DataType, typename LabelType>
class MaxHeap {
public:
    MaxHeap(size_t max_size) : max_size_(max_size) {}

    std::vector<LabelType> TransfromIdsVec() {
        std::vector<LabelType> ids;
        while (pq_.size()) {
            ids.push_back(pq_.top().second);
            pq_.pop();
        }
        return ids;
    }

    void push(LabelType id, DataType dist) {
        if (pq_.size() < max_size_) {
            pq_.emplace(dist, id);
            return;
        }
        if (dist < pq_.top().first) {
            pq_.pop();
            pq_.emplace(dist, id);
        }
    }

    std::pair<DataType, LabelType> top() { return pq_.top(); }
    void pop() { pq_.pop(); }
    size_t size() { return pq_.size(); }

private:
    size_t max_size_;
    std::priority_queue<std::pair<DataType, LabelType>> pq_;
};

TEST_F(QuantizerPerformanceTest, test_lvq) {
    using namespace infinity;
    using CompressType = i8;
    using VecStoreType = LVQL2VecStoreType<DataType, CompressType>;
    using DataStore = DataStore<VecStoreType, LabelType>;
    using Distance = VecStoreType::Distance;

    // generate data
    auto [base_num, base_dim, base_data] = DecodeFvecsDataset<f32>(base_path_);
    auto [query_num, query_dim, query_data] = DecodeFvecsDataset<f32>(query_path_);
    auto [gt_num, gt_dim, gt_data] = DecodeFvecsDataset<i32>(groundtruth_path_);
    query_num = 100;
    Distance distance(base_dim);

    // Init DataStore
    size_t max_chunk_n = (base_num + chunk_size_ - 1) / chunk_size_;
    auto base_iter = DenseVectorIter<DataType, LabelType>(base_data.get(), base_dim, base_num);
    auto rabitq_store = DataStore::Make(chunk_size_, max_chunk_n, base_dim, 0, 0);
    auto [start_i, end_i] = rabitq_store.OptAddVec(std::move(base_iter));

    // Compute recall
    size_t cnt = 0;
    for (size_t i = 0; i < query_num; ++i) {
        ASSERT_EQ(base_dim, query_dim);
        auto gt_vec = gt_data.get() + i * gt_dim;
        auto query_vec = query_data.get() + i * query_dim;
        auto rabitq_query = rabitq_store.MakeQuery(query_vec);

        // Compute recall
        MaxHeap<DataType, LabelType> rabitq_heap(topk_);
        for (LabelType id = start_i; id < end_i; ++id) {
            // Estimate l2 distance by lvq
            auto estimate_dis = distance(rabitq_query, id, rabitq_store);
            rabitq_heap.push(id, estimate_dis);
        }

        std::unordered_set<i32> gt(gt_vec, gt_vec + recall_at_);
        std::vector<LabelType> ids = rabitq_heap.TransfromIdsVec();
        for (LabelType id : ids) {
            if (gt.contains(id)) {
                ++cnt;
            }
        }
        std::cout << "query: " << i;
        std::cout << ", gt:";
        for (LabelType id : gt) {
            std::cout << " " << id;
        }
        std::cout << ", ids:";
        for (LabelType id : ids) {
            std::cout << " " << id;
        }
        std::cout << std::endl;
    }
    f32 recall = 1.0f * cnt / (query_num * recall_at_);
    std::cout << "Recall@1 = " << recall << std::endl;
    ASSERT_GE(recall, 0.9);
}

TEST_F(QuantizerPerformanceTest, test_rabitq) {
    using namespace infinity;
    using VecStoreType = RabitqL2VecStoreType<DataType>;
    using DataStore = DataStore<VecStoreType, LabelType>;
    using Distance = VecStoreType::Distance;

    // generate data
    auto [base_num, base_dim, base_data] = DecodeFvecsDataset<f32>(base_path_);
    auto [query_num, query_dim, query_data] = DecodeFvecsDataset<f32>(query_path_);
    auto [gt_num, gt_dim, gt_data] = DecodeFvecsDataset<i32>(groundtruth_path_);
    query_num = 100;
    Distance distance(base_dim);

    // Init DataStore
    size_t max_chunk_n = (base_num + chunk_size_ - 1) / chunk_size_;
    auto base_iter = DenseVectorIter<DataType, LabelType>(base_data.get(), base_dim, base_num);
    auto rabitq_store = DataStore::Make(chunk_size_, max_chunk_n, base_dim, 0, 0);
    auto [start_i, end_i] = rabitq_store.OptAddVec(std::move(base_iter));

    // Compute recall
    size_t cnt = 0;
    for (size_t i = 0; i < query_num; ++i) {
        ASSERT_EQ(base_dim, query_dim);
        auto gt_vec = gt_data.get() + i * gt_dim;
        auto query_vec = query_data.get() + i * query_dim;
        auto rabitq_query = rabitq_store.MakeQuery(query_vec);

        // Compute recall
        MaxHeap<DataType, LabelType> rabitq_heap(topk_);
        for (LabelType id = start_i; id < end_i; ++id) {
            // Estimate l2 distance by rabitq
            auto estimate_dis = distance(rabitq_query, id, rabitq_store);
            rabitq_heap.push(id, estimate_dis);
        }

        std::unordered_set<i32> gt(gt_vec, gt_vec + recall_at_);
        std::vector<LabelType> ids = rabitq_heap.TransfromIdsVec();
        for (LabelType id : ids) {
            if (gt.contains(id)) {
                ++cnt;
            }
        }
        std::cout << "query: " << i;
        std::cout << ", gt:";
        for (LabelType id : gt) {
            std::cout << " " << id;
        }
        std::cout << ", ids:";
        for (LabelType id : ids) {
            std::cout << " " << id;
        }
        std::cout << std::endl;
    }
    f32 recall = 1.0f * cnt / (query_num * recall_at_);
    std::cout << "Recall@1 = " << recall << std::endl;
    ASSERT_GE(recall, 0.9);
}
