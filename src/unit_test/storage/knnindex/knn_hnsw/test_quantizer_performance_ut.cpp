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

template <typename DataType, typename LabelType>
class MaxHeap {
public:
    MaxHeap(size_t max_size) : max_size_(max_size) {}

    std::vector<LabelType> TransfromIdsVec() {
        std::vector<LabelType> ids(pq_.size());
        size_t i = pq_.size() - 1;
        while (pq_.size()) {
            ids[i--] = pq_.top().second;
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

class QuantizerPerformanceTest : public BaseTest {
public:
    using DataType = f32;
    using LabelType = i32;

public:
    template <typename T>
    std::tuple<size_t, size_t, std::unique_ptr<T[]>> DecodeFvecsDataset(const std::filesystem::path &path) {
        auto [file_handle, status] = VirtualStore::Open(path.string(), FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        size_t dim = 0;
        file_handle->Read(&dim, sizeof(dim));
        size_t file_size = file_handle->FileSize();
        size_t vec_num = file_size / (dim * sizeof(T) + sizeof(dim));
        auto data = std::make_unique_for_overwrite<T[]>(vec_num * dim);
        for (size_t i = 0; i < vec_num - 1; ++i) {
            file_handle->Read(data.get() + i * dim, dim * sizeof(T));
            size_t dim1 = 0;
            file_handle->Read(&dim1, sizeof(dim1));
            if (dim1 != dim) {
                UnrecoverableError("dim not match");
            }
        }
        file_handle->Read(data.get() + (vec_num - 1) * dim, dim * sizeof(T));
        return {vec_num, dim, std::move(data)};
    }

    void SetUp() override {
        base_data_ = std::make_unique<DataType[]>(base_dim_ * base_num_);
        query_data_ = std::make_unique<DataType[]>(query_dim_ * query_num_);
        groundtruth_data_ = std::make_unique<LabelType[]>(gt_dim_ * gt_num_);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<DataType> dis(100, 200);
        for (size_t i = 0; i < base_dim_ * base_num_; ++i) {
            base_data_[i] = dis(gen);
        }
        for (size_t i = 0; i < query_dim_ * query_num_; ++i) {
            query_data_[i] = dis(gen);
        }

        auto L2Distance = GetSIMD_FUNCTIONS().HNSW_F32L2_ptr_;
        for (int i = 0; i < query_num_; ++i) {
            DataType *query = query_data_.get() + i * query_dim_;
            LabelType *gt = groundtruth_data_.get() + i * gt_dim_;
            MaxHeap<DataType, LabelType> max_heap(gt_dim_);
            for (int j = 0; j < base_num_; ++j) {
                DataType *base = base_data_.get() + j * base_dim_;
                max_heap.push(j, L2Distance(base, query, base_dim_));
            }

            auto gts = max_heap.TransfromIdsVec();
            std::copy(gts.begin(), gts.end(), gt);
        }
    }

    void TearDown() override {}

public:
    static constexpr size_t chunk_size_ = 8192;
    static constexpr size_t topk_ = 1;

    static constexpr size_t base_dim_ = 128;
    static constexpr size_t query_dim_ = base_dim_;
    static constexpr size_t gt_dim_ = 1;

    static constexpr size_t base_num_ = 8192;
    static constexpr size_t query_num_ = 100;
    static constexpr size_t gt_num_ = base_num_;

    std::unique_ptr<DataType[]> base_data_;
    std::unique_ptr<DataType[]> query_data_;
    std::unique_ptr<LabelType[]> groundtruth_data_;
};

TEST_F(QuantizerPerformanceTest, test_lvq) {
    using namespace infinity;
    using CompressType = i8;
    using VecStoreType = LVQL2VecStoreType<DataType, CompressType>;
    using DataStore = DataStore<VecStoreType, LabelType>;
    using Distance = VecStoreType::Distance;
    Distance distance(base_dim_);

    // Init DataStore
    size_t max_chunk_n = (base_num_ + chunk_size_ - 1) / chunk_size_;
    auto base_iter = DenseVectorIter<DataType, LabelType>(base_data_.get(), base_dim_, base_num_);
    auto rabitq_store = DataStore::Make(chunk_size_, max_chunk_n, base_dim_, 0, 0);
    auto [start_i, end_i] = rabitq_store.OptAddVec(std::move(base_iter));

    // Compute recall
    size_t cnt = 0;
    for (size_t i = 0; i < query_num_; ++i) {
        ASSERT_EQ(base_dim_, query_dim_);
        auto gt_vec = groundtruth_data_.get() + i * gt_dim_;
        auto query_vec = query_data_.get() + i * query_dim_;
        auto rabitq_query = rabitq_store.MakeQuery(query_vec);

        // Compute recall
        MaxHeap<DataType, LabelType> rabitq_heap(topk_);
        for (LabelType id = start_i; id < end_i; ++id) {
            // Estimate l2 distance by lvq
            auto estimate_dis = distance(rabitq_query, id, rabitq_store);
            rabitq_heap.push(id, estimate_dis);
        }

        std::unordered_set<LabelType> gt(gt_vec, gt_vec + gt_dim_);
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
    f32 recall = 1.0f * cnt / (query_num_ * gt_dim_);
    std::cout << "Recall_10@1 = " << recall << std::endl;
}

TEST_F(QuantizerPerformanceTest, test_rabitq) {
    using namespace infinity;
    using VecStoreType = RabitqL2VecStoreType<DataType>;
    using DataStore = DataStore<VecStoreType, LabelType>;
    using Distance = VecStoreType::Distance;
    Distance distance(base_dim_);

    // Init DataStore
    size_t max_chunk_n = (base_num_ + chunk_size_ - 1) / chunk_size_;
    auto base_iter = DenseVectorIter<DataType, LabelType>(base_data_.get(), base_dim_, base_num_);
    auto rabitq_store = DataStore::Make(chunk_size_, max_chunk_n, base_dim_, 0, 0);
    auto [start_i, end_i] = rabitq_store.OptAddVec(std::move(base_iter));

    // Compute recall
    size_t cnt = 0;
    for (size_t i = 0; i < query_num_; ++i) {
        ASSERT_EQ(base_dim_, query_dim_);
        auto gt_vec = groundtruth_data_.get() + i * gt_dim_;
        auto query_vec = query_data_.get() + i * query_dim_;
        auto rabitq_query = rabitq_store.MakeQuery(query_vec);

        // Compute recall
        MaxHeap<DataType, LabelType> rabitq_heap(topk_);
        for (LabelType id = start_i; id < end_i; ++id) {
            // Estimate l2 distance by rabitq
            auto estimate_dis = distance(rabitq_query, id, rabitq_store);
            rabitq_heap.push(id, estimate_dis);
        }

        std::unordered_set<LabelType> gt(gt_vec, gt_vec + gt_dim_);
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
    f32 recall = 1.0f * cnt / (query_num_ * gt_dim_);
    std::cout << "Recall_10@1 = " << recall << std::endl;
}
