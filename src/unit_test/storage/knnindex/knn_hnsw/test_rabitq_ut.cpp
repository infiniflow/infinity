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

    static constexpr size_t dim_ = 128;
    static constexpr size_t vec_n_ = 8192;
    const std::string file_dir_ = GetFullTmpDir();
};

TEST_F(RabitqTest, test_base) {
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
            bool is_rom = CheckOrthogonalMatrix(meta.rom(), meta.align_dim());
            ASSERT_EQ(is_rom, true);
        }

        RabitqVecStoreMeta meta = RabitqVecStoreMeta::Make(dim_);
        auto iter_copy = iter;
        meta.Optimize(iter_copy, 0);
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

    // test multiple iter
    {
        using RabitqVecStoreMeta = RabitqVecStoreMeta<DataType, true>;
        using RabitqVecStoreInner = RabitqVecStoreInner<DataType, true>;

        auto iter_copy = iter;
        auto iters = std::move(iter_copy).split();

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
