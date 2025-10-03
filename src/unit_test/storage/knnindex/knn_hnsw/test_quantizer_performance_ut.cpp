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
import :hnsw_alg;
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
    using DistanceType = f32;

private:
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

    void GenerateRandomDataset() {
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

    void LoadSift1M() {
        base_path_ = "test/data/benchmark/sift_1m/sift_base.fvecs";
        query_path_ = "test/data/benchmark/sift_1m/sift_query.fvecs";
        groundtruth_path_ = "test/data/benchmark/sift_1m/sift_groundtruth.ivecs";

        std::tie(base_num_, base_dim_, base_data_) = DecodeFvecsDataset<DataType>(base_path_);
        std::tie(query_num_, query_dim_, query_data_) = DecodeFvecsDataset<DataType>(query_path_);
        std::tie(gt_num_, gt_dim_, groundtruth_data_) = DecodeFvecsDataset<LabelType>(groundtruth_path_);

        query_num_ = 100;
    }

    void LoadSift10K() {
        base_path_ = "test/data/benchmark/sift10K/sift10K_base.fvecs";
        query_path_ = "test/data/benchmark/sift10K/sift10K_query.fvecs";
        groundtruth_path_ = "test/data/benchmark/sift10K/sift10K_groundtruth.ivecs";

        std::tie(base_num_, base_dim_, base_data_) = DecodeFvecsDataset<DataType>(base_path_);
        std::tie(query_num_, query_dim_, query_data_) = DecodeFvecsDataset<DataType>(query_path_);
        std::tie(gt_num_, gt_dim_, groundtruth_data_) = DecodeFvecsDataset<LabelType>(groundtruth_path_);
    }

public:
    void SetUp() override {
        GenerateRandomDataset();
        // LoadSift1M();
        // LoadSift10K();
    }

    void TearDown() override {}

    std::unique_ptr<float[]> GetAvgBF(size_t vec_num, size_t dim, const float *data, size_t ls_k, size_t sample_num) {
        auto avg = std::make_unique<float[]>(vec_num);
        std::vector<size_t> sample_idx(sample_num);
        for (size_t i = 0; i < sample_num; ++i) {
            sample_idx[i] = rand() % vec_num;
        }
        auto task = [&](size_t start_i, size_t end_i) {
            std::vector<float> distances(sample_num);
            for (size_t i = start_i; i < end_i; ++i) {
                if (i % 1000 == 0) {
                    std::cout << fmt::format("Sample {} / {}", i, vec_num) << std::endl;
                }
                const float *v = data + i * dim;
                for (size_t j = 0; j < sample_num; ++j) {
                    const float *v2 = data + sample_idx[j] * dim;

                    float distance = 0;
                    for (size_t k = 0; k < dim; ++k) {
                        float diff = v[k] - v2[k];
                        distance += diff * diff;
                    }
                    distances[j] = distance;
                }
                std::sort(distances.begin(), distances.end());
                avg[i] = 0;
                for (size_t j = 0; j < ls_k; ++j) {
                    avg[i] += distances[j];
                }
                avg[i] /= ls_k;
            }
        };
        std::vector<std::thread> threads;
        size_t thread_num = 16;
        size_t task_size = (vec_num - 1) / thread_num + 1;

        for (size_t i = 0; i < thread_num; ++i) {
            size_t start_i = i * task_size;
            size_t end_i = std::min(start_i + task_size, vec_num);
            threads.emplace_back(task, start_i, end_i);
        }
        for (auto &thread : threads) {
            thread.join();
        }

        return avg;
    }

public:
    size_t base_dim_ = 128;
    size_t query_dim_ = base_dim_;
    size_t gt_dim_ = 100;

    size_t base_num_ = 8192;
    size_t query_num_ = 100;
    size_t gt_num_ = base_num_;

    size_t recall_at_ = 10;
    size_t topk_ = 10;
    size_t chunk_size_ = 8192;

    std::string base_path_;
    std::string query_path_;
    std::string groundtruth_path_;

    std::unique_ptr<DataType[]> base_data_;
    std::unique_ptr<DataType[]> query_data_;
    std::unique_ptr<LabelType[]> groundtruth_data_;
};

TEST_F(QuantizerPerformanceTest, DISABLED_SLOW_flat_lvq) {
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

        std::unordered_set<LabelType> gt(gt_vec, gt_vec + recall_at_);
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
    f32 recall = 1.0f * cnt / (query_num_ * recall_at_);
    std::cout << "Recall_10@1 = " << recall << std::endl;
}

TEST_F(QuantizerPerformanceTest, DISABLED_SLOW_flat_rabitq) {
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

        std::unordered_set<LabelType> gt(gt_vec, gt_vec + recall_at_);
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
    f32 recall = 1.0f * cnt / (query_num_ * recall_at_);
    std::cout << "Recall_10@1 = " << recall << std::endl;
}

TEST_F(QuantizerPerformanceTest, DISABLED_SLOW_hnsw_lvq) {
    using Hnsw = KnnHnsw<LVQL2VecStoreType<DataType, i8>, LabelType>;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t ef_search = 200;
    size_t max_chunk_n = (base_num_ + chunk_size_ - 1) / chunk_size_;

    auto hnsw_index = Hnsw::Make(chunk_size_, max_chunk_n, base_dim_, M, ef_construction);
    auto iter = DenseVectorIter<DataType, LabelType>(base_data_.get(), base_dim_, base_num_);
    auto t0 = std::chrono::high_resolution_clock::now();
    hnsw_index->InsertVecs(std::move(iter));
    auto t1 = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Build index use time: " << seconds << std::endl;

    KnnSearchOption search_option{.ef_ = ef_search};
    int correct = 0;
    for (int i = 0; i < query_num_; ++i) {
        const auto &query = query_data_.get() + i * query_dim_;
        const auto &gt = groundtruth_data_.get() + i * gt_dim_;
        auto result = hnsw_index->KnnSearchSorted(query, topk_, search_option);
        result.resize(topk_);

        std::unordered_set<LabelType> gt_set(gt, gt + recall_at_);
        for (auto item : result) {
            if (gt_set.contains(item.second)) {
                ++correct;
            }
        }

        std::cout << "query: " << i;
        std::cout << ", gt:";
        for (LabelType id : gt_set) {
            std::cout << " " << id;
        }
        std::cout << ", ids:";
        for (auto item : result) {
            std::cout << " " << item.second;
        }
        std::cout << std::endl;
    }
    float correct_rate = float(correct) / query_num_ / recall_at_;
    std::printf("correct rage: %f\n", correct_rate);
    // EXPECT_GE(correct_rate, 0.9);
}

TEST_F(QuantizerPerformanceTest, DISABLED_SLOW_hnsw_rabitq) {
    using Hnsw = KnnHnsw<RabitqL2VecStoreType<DataType>, LabelType>;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t ef_search = 200;
    size_t max_chunk_n = (base_num_ + chunk_size_ - 1) / chunk_size_;

    auto hnsw_index = Hnsw::Make(chunk_size_, max_chunk_n, base_dim_, M, ef_construction);
    auto iter = DenseVectorIter<DataType, LabelType>(base_data_.get(), base_dim_, base_num_);
    auto t0 = std::chrono::high_resolution_clock::now();
    hnsw_index->InsertVecs(std::move(iter), {true});
    auto t1 = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Build index use time: " << seconds << std::endl;

    KnnSearchOption search_option{.ef_ = ef_search};
    auto KnnSearchSortedByFlat = [&](const auto &hnsw_index, const DataType *query) -> std::vector<std::pair<DistanceType, LabelType>> {
        auto L2Distance = GetSIMD_FUNCTIONS().HNSW_F32L2_ptr_;
        auto [result_n, d_ptr, v_ptr] = hnsw_index->KnnSearch(query, topk_, search_option);
        std::vector<std::pair<DistanceType, LabelType>> result(result_n);
        for (size_t i = 0; i < result_n; ++i) {
            LabelType id = hnsw_index->GetLabel(v_ptr[i]);
            const DataType *ori_vec = base_data_.get() + id * base_dim_;
            DistanceType dis = L2Distance(query, ori_vec, base_dim_);
            result[i] = {dis, id};
        }
        std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        return result;
    };

    int correct = 0;
    for (int i = 0; i < query_num_; ++i) {
        const auto &query = query_data_.get() + i * query_dim_;
        const auto &gt = groundtruth_data_.get() + i * gt_dim_;
        auto result = KnnSearchSortedByFlat(hnsw_index, query);
        result.resize(topk_);

        std::unordered_set<LabelType> gt_set(gt, gt + recall_at_);
        for (auto item : result) {
            if (gt_set.contains(item.second)) {
                ++correct;
            }
        }

        std::cout << "query: " << i;
        std::cout << ", gt:";
        for (LabelType id : gt_set) {
            std::cout << " " << id;
        }
        std::cout << ", ids:";
        for (auto item : result) {
            std::cout << " " << item.second;
        }
        std::cout << std::endl;
    }
    float correct_rate = float(correct) / query_num_ / recall_at_;
    std::printf("correct rage: %f\n", correct_rate);
    // EXPECT_GE(correct_rate, 0.9);
}

TEST_F(QuantizerPerformanceTest, DISABLED_SLOW_hnsw_lsg_rabitq) {
    using Hnsw = KnnHnsw<RabitqL2VecStoreType<DataType, true>, LabelType>;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t ef_search = 200;
    size_t max_chunk_n = (base_num_ + chunk_size_ - 1) / chunk_size_;

    auto hnsw_index = Hnsw::Make(chunk_size_, max_chunk_n, base_dim_, M, ef_construction);
    auto iter = DenseVectorIter<DataType, LabelType>(base_data_.get(), base_dim_, base_num_);

    size_t sample_num = base_num_ * 0.1;
    size_t ls_k = 10;
    float alpha = 1.0;
    std::unique_ptr<float[]> avg = GetAvgBF(base_num_, base_dim_, base_data_.get(), ls_k, sample_num);
    hnsw_index->distance().SetLSGParam(alpha, avg.get());

    auto t0 = std::chrono::high_resolution_clock::now();
    hnsw_index->InsertVecs(std::move(iter), {true});
    auto t1 = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Build index use time: " << seconds << std::endl;

    KnnSearchOption search_option{.ef_ = ef_search};
    auto KnnSearchSortedByFlat = [&](const auto &hnsw_index, const DataType *query) -> std::vector<std::pair<DistanceType, LabelType>> {
        auto L2Distance = GetSIMD_FUNCTIONS().HNSW_F32L2_ptr_;
        auto [result_n, d_ptr, v_ptr] = hnsw_index->KnnSearch(query, topk_, search_option);
        std::vector<std::pair<DistanceType, LabelType>> result(result_n);
        for (size_t i = 0; i < result_n; ++i) {
            LabelType id = hnsw_index->GetLabel(v_ptr[i]);
            const DataType *ori_vec = base_data_.get() + id * base_dim_;
            DistanceType dis = L2Distance(query, ori_vec, base_dim_);
            result[i] = {dis, id};
        }
        std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        return result;
    };

    int correct = 0;
    for (int i = 0; i < query_num_; ++i) {
        const auto &query = query_data_.get() + i * query_dim_;
        const auto &gt = groundtruth_data_.get() + i * gt_dim_;
        auto result = KnnSearchSortedByFlat(hnsw_index, query);
        result.resize(topk_);

        std::unordered_set<LabelType> gt_set(gt, gt + recall_at_);
        for (auto item : result) {
            if (gt_set.contains(item.second)) {
                ++correct;
            }
        }

        std::cout << "query: " << i;
        std::cout << ", gt:";
        for (LabelType id : gt_set) {
            std::cout << " " << id;
        }
        std::cout << ", ids:";
        for (auto item : result) {
            std::cout << " " << item.second;
        }
        std::cout << std::endl;
    }
    float correct_rate = float(correct) / query_num_ / recall_at_;
    std::printf("correct rage: %f\n", correct_rate);
    // EXPECT_GE(correct_rate, 0.9);
}

TEST_F(QuantizerPerformanceTest, DISABLED_SLOW_compress_hnsw_to_rabitq) {
    using Hnsw = KnnHnsw<PlainL2VecStoreType<DataType>, LabelType>;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t max_chunk_n = (base_num_ + chunk_size_ - 1) / chunk_size_;

    auto hnsw_index = Hnsw::Make(chunk_size_, max_chunk_n, base_dim_, M, ef_construction);
    auto iter = DenseVectorIter<DataType, LabelType>(base_data_.get(), base_dim_, base_num_);
    auto t0 = std::chrono::high_resolution_clock::now();
    hnsw_index->InsertVecs(std::move(iter));
    auto t1 = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Build index use time: " << seconds << std::endl;

    auto KnnSearchSortedByFlat =
        [&](const auto &hnsw_index, const DataType *query, const KnnSearchOption &search_option) -> std::vector<std::pair<DistanceType, LabelType>> {
        auto L2Distance = GetSIMD_FUNCTIONS().HNSW_F32L2_ptr_;
        auto [result_n, d_ptr, v_ptr] = hnsw_index->KnnSearch(query, topk_, search_option);
        std::vector<std::pair<DistanceType, LabelType>> result(result_n);
        for (size_t i = 0; i < result_n; ++i) {
            LabelType id = hnsw_index->GetLabel(v_ptr[i]);
            const DataType *ori_vec = base_data_.get() + id * base_dim_;
            DistanceType dis = L2Distance(query, ori_vec, base_dim_);
            result[i] = {dis, id};
        }
        std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        return result;
    };

    auto SearchIndex = [&](const auto &hnsw_index, const KnnSearchOption &search_option) -> void {
        int correct = 0;
        for (int i = 0; i < query_num_; ++i) {
            const auto &query = query_data_.get() + i * query_dim_;
            const auto &gt = groundtruth_data_.get() + i * gt_dim_;
            auto result = KnnSearchSortedByFlat(hnsw_index, query, search_option);
            result.resize(topk_);

            std::unordered_set<LabelType> gt_set(gt, gt + recall_at_);
            for (auto item : result) {
                if (gt_set.contains(item.second)) {
                    ++correct;
                }
            }

            std::cout << "query: " << i;
            std::cout << ", gt:";
            for (LabelType id : gt_set) {
                std::cout << " " << id;
            }
            std::cout << ", ids:";
            for (auto item : result) {
                std::cout << " " << item.second;
            }
            std::cout << std::endl;
        }
        float correct_rate = float(correct) / query_num_ / recall_at_;
        std::printf("correct rage: %f\n", correct_rate);
        // EXPECT_GE(correct_rate, 0.9);
    };

    SearchIndex(hnsw_index, {.ef_ = 200});

    auto compress_hnsw_index = std::move(*hnsw_index).CompressToRabitq();

    using HnswRabitq = KnnHnsw<RabitqL2VecStoreType<DataType>, LabelType>;
    ASSERT_TRUE((std::is_same_v<HnswRabitq, std::decay_t<decltype(*compress_hnsw_index)>>));

    SearchIndex(compress_hnsw_index, {.ef_ = 400});
}

TEST_F(QuantizerPerformanceTest, DISABLED_SLOW_compress_lsg_to_rabitq) {
    using HnswLsg = KnnHnsw<PlainL2VecStoreType<DataType, true>, LabelType>;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t max_chunk_n = (base_num_ + chunk_size_ - 1) / chunk_size_;

    auto hnsw_index = HnswLsg::Make(chunk_size_, max_chunk_n, base_dim_, M, ef_construction);
    auto iter = DenseVectorIter<DataType, LabelType>(base_data_.get(), base_dim_, base_num_);

    size_t sample_num = base_num_ * 0.1;
    size_t ls_k = 10;
    float alpha = 1.0;
    std::unique_ptr<float[]> avg = GetAvgBF(base_num_, base_dim_, base_data_.get(), ls_k, sample_num);
    hnsw_index->distance().SetLSGParam(alpha, avg.get());

    auto t0 = std::chrono::high_resolution_clock::now();
    hnsw_index->InsertVecs(std::move(iter));
    auto t1 = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Build index use time: " << seconds << std::endl;

    auto KnnSearchSortedByFlat =
        [&](const auto &hnsw_index, const DataType *query, const KnnSearchOption &search_option) -> std::vector<std::pair<DistanceType, LabelType>> {
        auto L2Distance = GetSIMD_FUNCTIONS().HNSW_F32L2_ptr_;
        auto [result_n, d_ptr, v_ptr] = hnsw_index->KnnSearch(query, topk_, search_option);
        std::vector<std::pair<DistanceType, LabelType>> result(result_n);
        for (size_t i = 0; i < result_n; ++i) {
            LabelType id = hnsw_index->GetLabel(v_ptr[i]);
            const DataType *ori_vec = base_data_.get() + id * base_dim_;
            DistanceType dis = L2Distance(query, ori_vec, base_dim_);
            result[i] = {dis, id};
        }
        std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        return result;
    };

    auto SearchIndex = [&](const auto &hnsw_index, const KnnSearchOption &search_option) -> void {
        int correct = 0;
        for (int i = 0; i < query_num_; ++i) {
            const auto &query = query_data_.get() + i * query_dim_;
            const auto &gt = groundtruth_data_.get() + i * gt_dim_;
            auto result = KnnSearchSortedByFlat(hnsw_index, query, search_option);
            result.resize(topk_);

            std::unordered_set<LabelType> gt_set(gt, gt + recall_at_);
            for (auto item : result) {
                if (gt_set.contains(item.second)) {
                    ++correct;
                }
            }

            std::cout << "query: " << i;
            std::cout << ", gt:";
            for (LabelType id : gt_set) {
                std::cout << " " << id;
            }
            std::cout << ", ids:";
            for (auto item : result) {
                std::cout << " " << item.second;
            }
            std::cout << std::endl;
        }
        float correct_rate = float(correct) / query_num_ / recall_at_;
        std::printf("correct rage: %f\n", correct_rate);
        // EXPECT_GE(correct_rate, 0.9);
    };

    SearchIndex(hnsw_index, {.ef_ = 200});

    auto compress_hnsw_index = std::move(*hnsw_index).CompressToRabitq();

    using HnswRabitq = KnnHnsw<RabitqL2VecStoreType<DataType>, LabelType>;
    ASSERT_TRUE((std::is_same_v<HnswRabitq, std::decay_t<decltype(*compress_hnsw_index)>>));

    SearchIndex(compress_hnsw_index, {.ef_ = 400});
}