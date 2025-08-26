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

module infinity_core:ut.test_rabitq;

import :ut.base_test;
import :rabitq_vec_store;
import :dist_func_l2;

using namespace infinity;

class RabitqTest : public BaseTest {
public:
    void SetUp() override {
        system(("rm -rf " + file_dir_).c_str());
        system(("mkdir -p " + file_dir_).c_str());
    }

    void TearDown() override { system(("rm -rf " + file_dir_).c_str()); }

    template <typename DataType>
    bool CheckOrthogonalMatrix(const DataType *matrix, int dim, DataType tolerance = 1e-5f) {
        for (int i = 0; i < dim; ++i) {
            DataType dot_self = 0.0;
            for (int k = 0; k < dim; ++k) {
                dot_self += matrix[k * dim + i] * matrix[k * dim + i];
            }
            if (std::abs(dot_self - 1.0) > tolerance) {
                LOG_WARN(fmt::format("Column {} non-unit length: |v|^2 = {:.6f} (should be 1.000000)", i, dot_self));
                return false;
            }

            for (int j = i + 1; j < dim; ++j) {
                DataType dot_product = 0.0;
                for (int k = 0; k < dim; ++k) {
                    // 行优先存储：matrix[row][col] = matrix[row*dim + col]
                    dot_product += matrix[k * dim + i] * matrix[k * dim + j];
                }

                if (std::abs(dot_product) > tolerance) {
                    LOG_WARN(fmt::format("Columns {} and {} are not orthogonal: dot product = {:.6f} (should be 0.000000)", i, j, dot_product));
                    return false;
                }
            }
        }

        return true;
    }

public:
    using DataType = f32;
    using LabelType = i64;
    using AlignType = u8;

    static constexpr size_t dim_ = 128;
    static constexpr size_t vec_n_ = 8192;
    const std::string file_dir_ = GetFullTmpDir();
    MetricType metric_type_ = MetricType::kMetricL2;
};

TEST_F(RabitqTest, test_simple) {
    using namespace infinity;

    size_t write_mem = 0;
    std::unique_ptr<DataType[]> write_rom;
    auto truth_centroid = std::make_unique<DataType[]>(dim_);

    std::string filepath = file_dir_ + "/test_rabitq.bin";
    auto data = std::make_unique<float[]>(dim_ * vec_n_);
    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real(100, 200);
    for (size_t i = 0; i < dim_ * vec_n_; ++i) {
        data[i] = distrib_real(rng);
    }
    auto iter = DenseVectorIter<DataType, LabelType>(data.get(), dim_, vec_n_);
    for (size_t j = 0; j < dim_; ++j) {
        for (size_t i = 0; i < vec_n_; ++i) {
            truth_centroid[j] += data[i * dim_ + j];
        }
        truth_centroid[j] /= vec_n_;
    }

    // test function
    {
        using RabitqVecStoreMeta = RabitqVecStoreMeta<DataType, true>;
        using RabitqVecStoreInner = RabitqVecStoreInner<DataType, true>;

        for (int i = 0; i < 20; ++i) {
            RabitqVecStoreMeta meta = RabitqVecStoreMeta::Make(dim_);
            bool is_rom = CheckOrthogonalMatrix(meta.rom(), meta.dim());
            ASSERT_EQ(is_rom, true);
        }

        RabitqVecStoreMeta meta = RabitqVecStoreMeta::Make(dim_);
        meta.Optimize(DenseVectorIter(iter), 0);
        meta.Dump(std::cout);
        const DataType *rom = meta.rom();
        const DataType *centroid = meta.centroid();
        write_rom = std::make_unique<DataType[]>(dim_ * dim_);
        std::copy(rom, rom + dim_ * dim_, write_rom.get());
        for (int d = 0; d < dim_; ++d) {
            ASSERT_EQ(centroid[d], truth_centroid[d]);
        }

        RabitqVecStoreInner inner = RabitqVecStoreInner::Make(vec_n_, meta, write_mem);
        LOG_INFO(fmt::format("inner: mem_usage {}", write_mem));

        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        meta.Save(*file_handle);
        inner.Save(*file_handle, vec_n_, meta);
    }

    // load to memory
    {
        using RabitqVecStoreMeta = RabitqVecStoreMeta<DataType, true>;
        using RabitqVecStoreInner = RabitqVecStoreInner<DataType, true>;

        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        RabitqVecStoreMeta meta = RabitqVecStoreMeta::Load(*file_handle);
        const DataType *rom = meta.rom();
        const DataType *centroid = meta.centroid();
        for (int i = 0; i < dim_; ++i) {
            for (int j = 0; j < dim_; ++j) {
                ASSERT_EQ(rom[i * dim_ + j], write_rom[i * dim_ + j]);
            }
        }
        for (int d = 0; d < dim_; ++d) {
            ASSERT_EQ(centroid[d], truth_centroid[d]);
        }

        size_t mem_usage = 0;
        RabitqVecStoreInner inner = RabitqVecStoreInner::Load(*file_handle, 0, vec_n_, meta, mem_usage);
        ASSERT_EQ(mem_usage, write_mem);
    }

    // load to mmap
    {
        using RabitqVecStoreMeta = RabitqVecStoreMeta<DataType, false>;
        using RabitqVecStoreInner = RabitqVecStoreInner<DataType, false>;

        unsigned char *data_ptr = nullptr;
        size_t file_size = VirtualStore::GetFileSize(filepath);
        int ret = VirtualStore::MmapFile(filepath, data_ptr, file_size);
        if (ret < 0) {
            UnrecoverableError("mmap failed");
        }
        const char *ptr = reinterpret_cast<const char *>(data_ptr);

        RabitqVecStoreMeta meta = RabitqVecStoreMeta::LoadFromPtr(ptr);
        const DataType *rom = meta.rom();
        const DataType *centroid = meta.centroid();
        for (int i = 0; i < dim_; ++i) {
            for (int j = 0; j < dim_; ++j) {
                ASSERT_EQ(rom[i * dim_ + j], write_rom[i * dim_ + j]);
            }
        }
        for (int d = 0; d < dim_; ++d) {
            ASSERT_EQ(centroid[d], truth_centroid[d]);
        }

        RabitqVecStoreInner inner = RabitqVecStoreInner::LoadFromPtr(ptr, 0, meta);
        ASSERT_EQ(inner.GetSizeInBytes(vec_n_, meta), write_mem);
    }

    // test optimize by multiple iter
    {
        using RabitqVecStoreMeta = RabitqVecStoreMeta<DataType, true>;
        using RabitqVecStoreInner = RabitqVecStoreInner<DataType, true>;

        auto iters = DenseVectorIter(iter).split();

        size_t inner_size = 0;
        RabitqVecStoreMeta meta = RabitqVecStoreMeta::Make(dim_);
        for (auto split_iter : iters) {
            size_t row_count = split_iter.GetRowCount();
            meta.Optimize(split_iter, 0);
            RabitqVecStoreInner inner = RabitqVecStoreInner::Make(row_count, meta, inner_size);
            LOG_INFO(fmt::format("split_iter: mem_usage {}", inner_size));
        }
        ASSERT_EQ(inner_size, write_mem);

        const DataType *centroid = meta.centroid();
        for (int d = 0; d < dim_; ++d) {
            ASSERT_LT(std::fabs(centroid[d] - truth_centroid[d]), truth_centroid[d] * 0.05);
        }
    }
}

TEST_F(RabitqTest, test_compress) {
    using namespace infinity;
    using RabitqVecStoreMeta = RabitqVecStoreMeta<DataType, true>;
    using RabitqVecStoreInner = RabitqVecStoreInner<DataType, true>;
    using MetaType = RabitqVecStoreMeta::MetaType;
    constexpr size_t align_size = MetaType::align_size_;

    // generate dataset
    auto data = std::make_unique<float[]>(dim_ * vec_n_);
    auto query = std::make_unique<float[]>(dim_);
    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real(100, 200);
    for (size_t i = 0; i < dim_ * vec_n_; ++i) {
        data[i] = distrib_real(rng);
    }
    auto iter = DenseVectorIter<DataType, LabelType>(data.get(), dim_, vec_n_);
    for (size_t i = 0; i < dim_; ++i) {
        query[i] = distrib_real(rng);
    }

    // Init meta
    RabitqVecStoreMeta meta = RabitqVecStoreMeta::Make(dim_);
    meta.Optimize(DenseVectorIter(iter), 0);
    meta.Dump(std::cout);

    // Compress data
    size_t mem_usage = 0;
    RabitqVecStoreInner inner = RabitqVecStoreInner::Make(vec_n_, meta, mem_usage);
    size_t insert_n = 0;
    for (auto val = iter.Next(); val; val = iter.Next()) {
        const auto &[embedding, offset] = val.value();
        inner.SetVec(insert_n++, embedding, meta, mem_usage);
    }
    ASSERT_EQ(insert_n, vec_n_);

    // Get vector
    for (size_t i = 0; i < vec_n_; ++i) {
        auto vec = inner.GetVec(i, meta);
        std::cout << fmt::format("raw_norm = {}, norm = {}, sum = {}, error = {}", vec->raw_norm_, vec->norm_, vec->sum_, vec->error_);
        std::cout << ", compress_vec_ =";
        auto code = vec->compress_vec_;
        size_t sum = 0;
        for (size_t d = 0; d < meta.dim(); ++d) {
            bool c_i = code[d / align_size] >> (align_size - 1 - d % align_size) & 1;
            sum += c_i;
            if (d % align_size == 0) {
                std::cout << " ";
            }
            std::cout << c_i;
        }
        std::cout << std::endl;
        ASSERT_EQ(sum, vec->sum_);
    }

    // Compress query
    auto query_code = meta.MakeQuery(query.get());
    std::cout << fmt::format("query_raw_norm = {}, query_norm = {}, query_sum = {}, query_lower_bound = {}, query_delta = {}",
                             query_code->query_raw_norm_,
                             query_code->query_norm_,
                             query_code->query_sum_,
                             query_code->query_lower_bound_,
                             query_code->query_delta_);
    std::cout << ", query_compress_vec_ =";
    for (size_t d = 0; d < meta.dim(); ++d) {
        std::cout << " " << (i32)query_code->query_compress_vec_[d];
    }
    std::cout << std::endl;
}

TEST_F(RabitqTest, test_distance) {
    using namespace infinity;
    using RabitqVecStoreMeta = RabitqVecStoreMeta<DataType, true>;
    using RabitqVecStoreInner = RabitqVecStoreInner<DataType, true>;
    using StoreType = RabitqVecStoreMeta::StoreType;
    using QueryType = RabitqVecStoreMeta::QueryType;
    using StoreType = RabitqVecStoreMeta::StoreType;
    using QueryType = RabitqVecStoreMeta::QueryType;
    using DistanceType = RabitqVecStoreMeta::DistanceType;
    using MetaType = RabitqVecStoreMeta::MetaType;
    constexpr size_t query_vec_n = 100;
    constexpr size_t recall_at = 1;
    constexpr size_t topk = 10;

    // generate data
    auto data = std::make_unique<float[]>(dim_ * vec_n_);
    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real(100, 200);
    for (size_t i = 0; i < dim_ * vec_n_; ++i) {
        data[i] = distrib_real(rng);
    }
    auto iter = DenseVectorIter<DataType, LabelType>(data.get(), dim_, vec_n_);

    // Init meta
    RabitqVecStoreMeta meta = RabitqVecStoreMeta::Make(dim_);
    meta.Optimize(DenseVectorIter(iter), 0);

    // Compress data
    size_t mem_usage = 0;
    RabitqVecStoreInner inner = RabitqVecStoreInner::Make(vec_n_, meta, mem_usage);
    size_t insert_n = 0;
    for (auto val = iter.Next(); val; val = iter.Next()) {
        const auto &[embedding, offset] = val.value();
        inner.SetVec(insert_n++, embedding, meta, mem_usage);
    }
    std::cout << "Insert num = " << insert_n << std::endl;
    ASSERT_EQ(insert_n, vec_n_);

    auto l2_distance_sqr = [&](const DataType *query, const DataType *data, size_t dim) {
        DistanceType res = 0;
        for (size_t d = 0; d < dim; ++d) {
            res += (data[d] - query[d]) * (data[d] - query[d]);
        }
        return res;
    };

    auto estimate_l2_distance_sqr = [&](const QueryType &query, const StoreType &data) {
        // estimate <x, q>
        DistanceType ip_estimate = MetaType::IpDistanceBetweenQueryAndBinaryCode(query->query_compress_vec_, data->compress_vec_, meta.dim());
        DistanceType ip_recover =
            MetaType::RecoverIpDistance(ip_estimate, meta.dim(), data->sum_, query->query_sum_, query->query_lower_bound_, query->query_delta_);

        // estimate <o, q>
        DataType error = data->error_;
        if (MetaType::IsApproxZero(error)) {
            error = error > 0 ? 1 : -1;
        }
        ip_recover = ip_recover / error;

        // estimate ||o_r, q_r||^2
        DistanceType res = MetaType::RecoverL2DistanceSqr(ip_recover, data->norm_, query->query_norm_);

        // estimate other metric
        if (metric_type_ == MetricType::kMetricCosine) {
            if (MetaType::IsApproxZero(data->raw_norm_) || MetaType::IsApproxZero(query->query_raw_norm_)) {
                res = 1;
            } else {
                res = 1 - (query->query_raw_norm_ * query->query_raw_norm_ + data->raw_norm_ * data->raw_norm_ - res) * 0.5f /
                              (query->query_raw_norm_ * data->raw_norm_);
            }
        }
        if (metric_type_ == MetricType::kMetricInnerProduct) {
            if (MetaType::IsApproxZero(data->raw_norm_) || MetaType::IsApproxZero(query->query_raw_norm_)) {
                res = 1;
            } else {
                res = 1 - (query->query_raw_norm_ * query->query_raw_norm_ + data->raw_norm_ * data->raw_norm_ - res) * 0.5f;
            }
        }

        return res;
    };

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

        std::set<LabelType> TransfromIdsSet() {
            std::set<LabelType> ids;
            while (pq_.size()) {
                ids.insert(pq_.top().second);
                pq_.pop();
            }
            return ids;
        }

        void push(LabelType id, DistanceType dist) {
            if (pq_.size() < max_size_) {
                pq_.emplace(dist, id);
                return;
            }
            if (dist < pq_.top().first) {
                pq_.pop();
                pq_.emplace(dist, id);
            }
        }

        std::pair<DistanceType, LabelType> top() { return pq_.top(); }
        void pop() { pq_.pop(); }
        size_t size() { return pq_.size(); }

    private:
        size_t max_size_;
        std::priority_queue<std::pair<DistanceType, LabelType>> pq_;
    };

    // Compute recall
    size_t cnt = 0;
    for (size_t i = 0; i < query_vec_n; ++i) {
        // generate query
        auto query = std::make_unique<float[]>(dim_);
        for (size_t d = 0; d < dim_; ++d) {
            query[d] = distrib_real(rng);
        }
        auto query_code = meta.MakeQuery(query.get());

        // Compute recall
        MaxHeap truth_heap(recall_at);
        MaxHeap rabitq_heap(topk);
        for (LabelType id = 0; id < vec_n_; ++id) {
            // Compute truth distance
            auto truth_dis = l2_distance_sqr(query.get(), data.get() + id * dim_, dim_);
            truth_heap.push(id, truth_dis);

            // Estimate l2 distance by rabitq
            auto vec = inner.GetVec(id, meta);
            auto estimate_dis = estimate_l2_distance_sqr(query_code, vec);
            rabitq_heap.push(id, estimate_dis);

            // output
            // std::cout << fmt::format("id: {}, truth distance: {:.2f}, estimate distance: {:.2f}", id, truth_dis, estimate_dis) << std::endl;
        }

        std::set<LabelType> gt = truth_heap.TransfromIdsSet();
        std::vector<LabelType> ids = rabitq_heap.TransfromIdsVec();
        for (LabelType id : ids) {
            if (gt.contains(id)) {
                ++cnt;
            }
        }
        std::cout << "gt:";
        for (LabelType id : gt) {
            std::cout << " " << id;
        }
        std::cout << ", ids:";
        for (LabelType id : ids) {
            std::cout << " " << id;
        }
        std::cout << std::endl;
    }
    f32 recall = 1.0f * cnt / (query_vec_n * recall_at);
    std::cout << "Recall@1 = " << recall << std::endl;
    ASSERT_GE(recall, 0.9);
}
