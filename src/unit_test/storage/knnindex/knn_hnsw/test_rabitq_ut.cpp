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
import :data_store;
import :vec_store_type;
import :rabitq_vec_store;
import :dist_func_l2;

using namespace infinity;

class RabitqTest : public BaseTest {
public:
    using DataType = f32;
    using LabelType = i64;

public:
    void SetUp() override {
        system(("rm -rf " + file_dir_).c_str());
        system(("mkdir -p " + file_dir_).c_str());
    }

    void TearDown() override { system(("rm -rf " + file_dir_).c_str()); }

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
    const std::string file_dir_ = GetFullTmpDir();

    static constexpr size_t dim_ = 128;
    static constexpr size_t vec_n_ = 8192;
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
        size_t mem_usage = 0;
        meta.Optimize<LabelType>(DenseVectorIter(iter), {}, mem_usage);
        meta.Dump(std::cout);
        const DataType *rom = meta.rom();
        const DataType *rot_centroid = meta.rot_centroid();
        auto centroid = std::make_unique<DataType[]>(dim_);
        matrixA_multiply_transpose_matrixB_output_to_C(rot_centroid, rom, 1, meta.dim(), meta.dim(), centroid.get());
        write_rom = std::make_unique<DataType[]>(dim_ * dim_);
        std::copy(rom, rom + dim_ * dim_, write_rom.get());
        for (int d = 0; d < dim_; ++d) {
            EXPECT_LT(std::fabs(centroid[d] - truth_centroid[d]), std::fabs(truth_centroid[d]) * 1e-5);
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
        const DataType *rot_centroid = meta.rot_centroid();
        auto centroid = std::make_unique<DataType[]>(dim_);
        matrixA_multiply_transpose_matrixB_output_to_C(rot_centroid, rom, 1, meta.dim(), meta.dim(), centroid.get());
        for (int i = 0; i < dim_; ++i) {
            for (int j = 0; j < dim_; ++j) {
                ASSERT_EQ(rom[i * dim_ + j], write_rom[i * dim_ + j]);
            }
        }
        for (int d = 0; d < dim_; ++d) {
            EXPECT_LT(std::fabs(centroid[d] - truth_centroid[d]), std::fabs(truth_centroid[d]) * 1e-5);
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
        const DataType *rot_centroid = meta.rot_centroid();
        auto centroid = std::make_unique<DataType[]>(dim_);
        matrixA_multiply_transpose_matrixB_output_to_C(rot_centroid, rom, 1, meta.dim(), meta.dim(), centroid.get());
        for (int i = 0; i < dim_; ++i) {
            for (int j = 0; j < dim_; ++j) {
                ASSERT_EQ(rom[i * dim_ + j], write_rom[i * dim_ + j]);
            }
        }
        for (int d = 0; d < dim_; ++d) {
            EXPECT_LT(std::fabs(centroid[d] - truth_centroid[d]), std::fabs(truth_centroid[d]) * 1e-5);
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
            size_t mem_usage = 0;
            meta.Optimize<LabelType>(split_iter, {}, mem_usage);
            RabitqVecStoreInner inner = RabitqVecStoreInner::Make(row_count, meta, inner_size);
            LOG_INFO(fmt::format("split_iter: mem_usage {}", inner_size));
        }
        ASSERT_EQ(inner_size, write_mem);

        const DataType *rom = meta.rom();
        const DataType *rot_centroid = meta.rot_centroid();
        auto centroid = std::make_unique<DataType[]>(dim_);
        matrixA_multiply_transpose_matrixB_output_to_C(rot_centroid, rom, 1, meta.dim(), meta.dim(), centroid.get());
        for (int d = 0; d < dim_; ++d) {
            std::cout << fmt::format("d: {}, truth centroid: {}, latest centroid: {}, loss: {}",
                                     d,
                                     truth_centroid[d],
                                     centroid[d],
                                     std::fabs(centroid[d] - truth_centroid[d]) / truth_centroid[d])
                      << std::endl;
            EXPECT_LT(std::fabs(centroid[d] - truth_centroid[d]), std::fabs(truth_centroid[d]) * 0.015);
        }
    }
}

TEST_F(RabitqTest, test_compress) {
    using namespace infinity;
    using VecStoreType = RabitqL2VecStoreType<DataType>;
    using RabitqVecStoreMeta = VecStoreType::Meta<true>;
    using RabitqVecStoreInner = VecStoreType::Inner<true>;
    using MetaType = VecStoreType::MetaType;
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
    size_t mem_usage = 0;
    meta.Optimize<LabelType>(DenseVectorIter(iter), {}, mem_usage);
    meta.Dump(std::cout);

    // Compress data
    mem_usage = 0;
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
    using VecStoreType = RabitqL2VecStoreType<DataType>;
    using DataStore = DataStore<VecStoreType, LabelType>;
    using Distance = VecStoreType::Distance;
    auto SIMDFuncL2 = GetSIMD_FUNCTIONS().L2Distance_func_ptr_;
    Distance distance(dim_);

    // generate base
    auto data = std::make_unique<DataType[]>(dim_ * vec_n_);
    std::default_random_engine rng;
    std::uniform_real_distribution<DataType> distrib_real(100, 200);
    for (size_t i = 0; i < dim_ * vec_n_; ++i) {
        data[i] = distrib_real(rng);
    }

    // Init DataStore
    auto iter = DenseVectorIter<DataType, LabelType>(data.get(), dim_, vec_n_);
    auto rabitq_store = DataStore::Make(vec_n_, 1, dim_, 0, 0);
    auto [start_i, end_i] = rabitq_store.OptAddVec(std::move(iter));

    // generate query
    auto query = std::make_unique<float[]>(dim_);
    for (size_t d = 0; d < dim_; ++d) {
        query[d] = distrib_real(rng);
    }
    auto rabitq_query = rabitq_store.MakeQuery(query.get());

    // Compute recall
    for (LabelType id = start_i; id < end_i; ++id) {
        // Compute truth distance
        auto truth_dis = SIMDFuncL2(query.get(), data.get() + id * dim_, dim_);

        // Estimate l2 distance by rabitq
        auto estimate_dis = distance(rabitq_query, id, rabitq_store);

        // output
        std::cout << fmt::format("id: {}, truth distance: {:.2f}, estimate distance: {:.2f}", id, truth_dis, estimate_dis) << std::endl;
    }
}
