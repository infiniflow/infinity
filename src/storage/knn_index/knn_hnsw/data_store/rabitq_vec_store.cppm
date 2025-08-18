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

#include <cassert>
#include <ostream>
#include <random>

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <xmmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse.h>
#endif

export module infinity_core:rabitq_vec_store;

import :local_file_handle;
import :data_store_util;
import :infinity_exception;
import :hnsw_common;
import :mlas_matrix_multiply;

import std;
import std.compat;
import serialize;

namespace infinity {

size_t AlignUp(size_t num, size_t align_size) { return (num + align_size - 1) & ~(align_size - 1); }

template <typename DataType>
void GenerateRandomOrthogonalMatrix(DataType *rom, size_t dim) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<DataType> dist(-1.0, 1.0);

    // Initialize the identity matrix
    std::memset(rom, 0, dim * dim * sizeof(DataType));
    for (size_t i = 0; i < dim; ++i) {
        rom[i * dim + i] = 1.0;
    }

    // Random Givens Rotation
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = i + 1; j < dim; ++j) {
            DataType angle = 2 * M_PI * dist(gen);
            DataType c = std::cos(angle);
            DataType s = std::sin(angle);
            for (size_t k = 0; k < dim; ++k) {
                DataType q_ik = rom[k * dim + i] * c - rom[k * dim + j] * s;
                DataType q_jk = rom[k * dim + i] * s + rom[k * dim + j] * c;
                rom[k * dim + i] = q_ik;
                rom[k * dim + j] = q_jk;
            }
        }
    }
}

/////////////////////////// compress data content ///////////////////////////

export template <typename DataType, typename AlignType>
struct RabitqStoreData {
    DataType raw_norm_{0}; // Norm of raw vector: ||o||^2
    DataType norm_{0};     // Norm of normalize vector: ||o_r - c||
    DataType sum_{0};      // sum of bin code: \sum{x_b}
    DataType error_{0};    // error of <o, q>: <\bar{o}, o>
    AlignType compress_vec_[];
};

export template <typename DataType, typename CompressType>
struct RabitqQueryData {
    DataType query_raw_norm_{0};
    DataType query_norm_{0};
    DataType query_sum_{0};
    DataType query_lower_bound_{0};
    DataType query_delta_{0};
    CompressType query_compress_vec_[];
};

/////////////////////////// meta info of rabitq ///////////////////////////

export template <typename DataType, bool OwnMem>
class RabitqVecStoreInner;

export template <typename DataType>
class RabitqVecStoreMetaType {
public:
    using AlignType = u8;    // Align for 1-bit quantizer
    using CompressType = u8; // Compress query per dimension size
    using DistanceType = f32;
    using StoreData = RabitqStoreData<DataType, AlignType>;
    using QueryData = RabitqQueryData<DataType, CompressType>;
    using StoreType = const StoreData *;
    struct QueryType {
        std::unique_ptr<QueryData> inner_;
        QueryData *operator->() const { return inner_.get(); }

        QueryType(size_t compress_data_size) : inner_(new(new char[compress_data_size]) QueryData) {}
        QueryType(QueryType &&other) = default;
        ~QueryType() { delete[] reinterpret_cast<char *>(inner_.release()); }
    };

    constexpr static DataType tolerance_ = 1e-5;
    constexpr static size_t align_size_ = sizeof(AlignType) * 8;                                                                         // 8 for u8
    constexpr static size_t compress_bucket_size_ = std::numeric_limits<CompressType>::max() - std::numeric_limits<CompressType>::min(); // 255 for u8
    static bool IsApproxZero(auto num) { return std::fabs(num) < tolerance_; }

    static inline DistanceType IpDistanceBetweenQueryAndBinaryCode(const CompressType *query, const AlignType *data, size_t dim) {
        // RaBitQ equation: estimate <\bar{x_b}, \bar{q_u}>
        DistanceType ip_estimate = 0;
        for (size_t d = 0; d < dim; ++d) {
            if ((data[d / align_size_] >> (d % align_size_)) & 1) {
                ip_estimate += static_cast<DistanceType>(query[d]);
            }
        }
        return ip_estimate;
    };

    static inline DistanceType RecoverIpDistance(DistanceType ip_xb_qu,
                                                 DistanceType dim_,
                                                 DistanceType base_sum,
                                                 DistanceType query_sum,
                                                 DistanceType lower_bound,
                                                 DistanceType delta) {
        // RaBitQ equation: estimate <\bar{x}, \bar{q}>
        DistanceType inv_sqrt_d = 1 / std::sqrt(dim_);
        DistanceType p1 = inv_sqrt_d * 2 * delta * ip_xb_qu;
        DistanceType p2 = inv_sqrt_d * 2 * lower_bound * base_sum;
        DistanceType p3 = inv_sqrt_d * delta * query_sum;
        DistanceType p4 = dim_ * inv_sqrt_d * lower_bound;
        return p1 + p2 - p3 - p4;
    };

    static inline DistanceType RecoverL2DistanceSqr(DistanceType ip_o_q, DistanceType base_norm, DistanceType query_norm) {
        // RaBitQ equation: estimate <\bar{o}, \bar{q}>
        DistanceType p1 = base_norm * base_norm;
        DistanceType p2 = query_norm * query_norm;
        DistanceType p3 = 2 * base_norm * query_norm * ip_o_q;
        return p1 + p2 - p3;
    };
};

template <typename DataType, bool OwnMem>
class RabitqVecStoreMetaBase {
public:
    // DataType type must be i8 & float temporarily
    static_assert(std::is_same<DataType, i8>() || std::is_same<DataType, float>());

    using This = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using Inner = RabitqVecStoreInner<DataType, OwnMem>;
    using MetaType = RabitqVecStoreMetaType<DataType>;
    using StoreData = typename MetaType::StoreData;
    using QueryData = typename MetaType::QueryData;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using AlignType = typename MetaType::AlignType;
    using CompressType = typename MetaType::CompressType;
    using DistanceType = typename MetaType::DistanceType;

public:
    RabitqVecStoreMetaBase() : origin_dim_(0), dim_(0), compress_data_size_(0), compress_query_size_(0) {}
    RabitqVecStoreMetaBase(This &&other)
        : origin_dim_(std::exchange(other.origin_dim_, 0)), rom_(std::move(other.rom_)), rot_centroid_(std::move(other.rot_centroid_)),
          dim_(std::exchange(other.dim_, 0)), compress_data_size_(std::exchange(other.compress_data_size_, 0)),
          compress_query_size_(std::exchange(other.compress_query_size_, 0)) {}
    RabitqVecStoreMetaBase &operator=(This &&other) {
        if (this != &other) {
            origin_dim_ = std::exchange(other.origin_dim_, 0);
            rom_ = std::move(other.rom_);
            rot_centroid_ = std::move(other.rot_centroid_);
            dim_ = std::exchange(other.dim_, 0);
            compress_data_size_ = std::exchange(other.compress_data_size_, 0);
            compress_query_size_ = std::exchange(other.compress_query_size_, 0);
        }
        return *this;
    }

    void Save(LocalFileHandle &file_handle) const {
        file_handle.Append(&origin_dim_, sizeof(origin_dim_));
        file_handle.Append(rom_.get(), sizeof(DataType) * dim_ * dim_);
        file_handle.Append(rot_centroid_.get(), sizeof(DataType) * dim_);
    }

    QueryType MakeQuery(const DataType *vec) const {
        QueryType query(compress_query_size_);
        CompressToQuery(vec, query.inner_.get());
        return query;
    }

    void CompressToCode(const DataType *src, StoreData *dest) const {
        size_t align_size = MetaType::align_size_;
        size_t bin_code_size = dim_ / align_size;

        // 1.Init
        memset(dest, 0, compress_data_size_);
        std::vector<DataType> align_src(dim_, 0);
        std::copy(src, src + origin_dim_, align_src.begin());
        std::vector<DataType> rot_src(dim_, 0);
        auto bin_src = std::make_unique<AlignType[]>(bin_code_size);
        memset(bin_src.get(), 0, bin_code_size); // 1 dim -> 1 bit

        // 2.Compute raw vector Norm for IP or Cos distance
        DataType raw_norm = 0;
        for (size_t d = 0; d < dim_; ++d) {
            raw_norm += align_src[d] * align_src[d];
        }

        // 3.Rotation align_src by rom
        matrixA_multiply_matrixB_output_to_C(align_src.data(), rom_.get(), 1, dim_, dim_, rot_src.data());

        // 4.normalize rot_src
        DataType norm = 0;
        for (size_t d = 0; d < dim_; ++d) {
            norm += (rot_src[d] - rot_centroid_[d]) * (rot_src[d] - rot_centroid_[d]);
        }
        norm = MetaType::IsApproxZero(norm) ? 1 : std::sqrt(norm);
        for (size_t d = 0; d < dim_; d++) {
            rot_src[d] = (rot_src[d] - rot_centroid_[d]) / norm;
        }

        // 5.encode rot_src (has normalized) to u8
        DataType sum = 0;
        for (size_t d = 0; d < dim_; ++d) {
            if (rot_src[d] >= 0) {
                sum += 1;
                bin_src[d / align_size] |= (1 << (d % align_size));
            }
        }

        // 6.compute encode error
        DataType error = 0;
        DataType inv_sqrt_d = 1 / std::sqrt(dim_);
        for (size_t d = 0; d < dim_; ++d) {
            DataType x_i = ((bin_src[d / align_size] >> (d % align_size)) & 1) ? inv_sqrt_d : -inv_sqrt_d;
            error += x_i * rot_src[d];
        }
        error = MetaType::IsApproxZero(error) ? 0.8 : error;

        // 7.store data in dest
        dest->raw_norm_ = raw_norm;
        dest->norm_ = norm;
        dest->sum_ = sum;
        dest->error_ = error;
        std::copy(bin_src.get(), bin_src.get() + bin_code_size, dest->compress_vec_);
    }

    void CompressToQuery(const DataType *src, QueryData *dest) const {
        // 1.Init
        memset(dest, 0, compress_query_size_);
        std::vector<DataType> align_src(dim_, 0);
        std::copy(src, src + origin_dim_, align_src.begin());
        std::vector<DataType> rot_src(dim_, 0);
        auto compress_src = std::make_unique<CompressType[]>(dim_);

        // 2.Compute raw vector Norm for IP or Cos distance
        DataType raw_norm = 0;
        for (size_t d = 0; d < dim_; ++d) {
            raw_norm += align_src[d] * align_src[d];
        }

        // 3.Rotation align_src by rom
        matrixA_multiply_matrixB_output_to_C(align_src.data(), rom_.get(), 1, dim_, dim_, rot_src.data());

        // 4.normalize rot_src
        DataType norm = 0;
        for (size_t d = 0; d < dim_; ++d) {
            norm += (rot_src[d] - rot_centroid_[d]) * (rot_src[d] - rot_centroid_[d]);
        }
        norm = MetaType::IsApproxZero(norm) ? 1 : std::sqrt(norm);
        for (size_t d = 0; d < dim_; ++d) {
            rot_src[d] = (rot_src[d] - rot_centroid_[d]) / norm;
        }

        // 5.Quantized query (Mean Scalar Quantization)
        DataType sum = 0;
        DataType lower_bound = std::numeric_limits<DataType>::max();
        DataType upper_bound = std::numeric_limits<DataType>::lowest();
        DataType delta = 0;
        for (size_t d = 0; d < dim_; ++d) {
            const DataType val = rot_src[d];
            lower_bound = std::min(lower_bound, val);
            upper_bound = std::max(upper_bound, val);
        }
        delta = (upper_bound - lower_bound) / MetaType::compress_bucket_size_;
        const DataType inv_delta = MetaType::IsApproxZero(delta) ? 0 : 1 / delta;
        for (size_t d = 0; d < dim_; ++d) {
            const DataType val = std::round((rot_src[d] - lower_bound) * inv_delta);
            sum += val;
            compress_src[d] = static_cast<CompressType>(val);
        }

        // 6.store data in dest
        dest->query_raw_norm_ = raw_norm;
        dest->query_norm_ = norm;
        dest->query_sum_ = sum;
        dest->query_lower_bound_ = lower_bound;
        dest->query_delta_ = delta;
        std::copy(compress_src.get(), compress_src.get() + dim_, dest->query_compress_vec_);
    }

    void Dump(std::ostream &os) const {
        os << fmt::format("[CONST] origin_dim: {}, dim: {}, compress_data_size: {}, compress_query_size: {}",
                          origin_dim_,
                          dim_,
                          compress_data_size_,
                          compress_query_size_)
           << std::endl;

        os << "rom: " << std::endl;
        for (size_t i = 0; i < dim_; ++i) {
            os << "\t";
            for (size_t j = 0; j < dim_; ++j) {
                os << rom_[i * dim_ + j] << " ";
            }
            os << std::endl;
        }

        os << "rot_centroid: ";
        for (size_t i = 0; i < dim_; ++i) {
            os << rot_centroid_[i] << " ";
        }
        os << std::endl;
    }

public:
    size_t GetSizeInBytes() const { return sizeof(origin_dim_) + dim_ * dim_ * sizeof(DataType) + dim_ * sizeof(DataType); }
    size_t GetVecSizeInBytes() const { return compress_data_size_; }
    size_t origin_dim() const { return origin_dim_; }
    size_t dim() const { return dim_; }
    size_t compress_data_size() const { return compress_data_size_; }
    size_t compress_query_size() const { return compress_query_size_; }
    const DataType *rom() const { return rom_.get(); }
    const DataType *rot_centroid() const { return rot_centroid_.get(); }

protected:
    void DecompressCode(StoreType src, DataType *dest, const DataType *rot_centroid) {
        DataType inv_sqrt_d_ = 1.0f / std::sqrt(dim_);

        // 1.Init
        std::vector<DataType> rot_src(dim_, 0);

        // 2.Decode to real-valued vector
        for (size_t d = 0; d < dim_; ++d) {
            bool bit = (src->compress_vec_[d / 8] >> (d % 8)) & 1;
            rot_src[d] = bit ? inv_sqrt_d_ : -inv_sqrt_d_;
        }

        // 3.Recover real-valued vector to rot_src vector
        for (size_t d = 0; d < dim_; d++) {
            rot_src[d] = rot_src[d] * src->norm_ + rot_centroid[d];
        }

        // 4.Inverse random projection
        matrixA_multiply_transpose_matrixB_output_to_C(rot_src.data(), rom_.get(), 1, dim_, dim_, dest);
    }

    void DecompressCode(StoreType src, DataType *dest) { DecompressCode(src, dest, rot_centroid_.get()); }

protected:
    size_t origin_dim_;
    ArrayPtr<DataType, OwnMem> rom_;          // Random orthogonal matrix
    ArrayPtr<DataType, OwnMem> rot_centroid_; // Rotation centroid of all vector in DataStore

    size_t dim_;
    size_t compress_data_size_;
    size_t compress_query_size_;
};

export template <typename DataType, bool OwnMem>
class RabitqVecStoreMeta : public RabitqVecStoreMetaBase<DataType, OwnMem> {
public:
    using This = RabitqVecStoreMeta<DataType, OwnMem>;
    using Base = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using Inner = RabitqVecStoreInner<DataType, OwnMem>;
    using MetaType = typename Base::MetaType;
    using StoreData = typename Base::StoreData;
    using QueryData = typename Base::QueryData;
    using AlignType = typename Base::AlignType;
    using CompressType = typename Base::CompressType;

private:
    RabitqVecStoreMeta(size_t origin_dim) {
        this->origin_dim_ = origin_dim;
        size_t dim = AlignUp(origin_dim, MetaType::align_size_);
        this->dim_ = dim;
        this->rom_ = std::make_unique<DataType[]>(dim * dim);
        this->rot_centroid_ = std::make_unique<DataType[]>(dim);
        GenerateRandomOrthogonalMatrix<DataType>(this->rom_.get(), this->dim_);
        this->compress_data_size_ = sizeof(StoreData) + dim / MetaType::align_size_;
        this->compress_query_size_ = sizeof(QueryData) + dim * sizeof(CompressType);
    }

public:
    RabitqVecStoreMeta() = default;
    static This Make(size_t origin_dim) { return This(origin_dim); }
    static This Make(size_t origin_dim, bool normalize) { return This(origin_dim); }

    static This Load(LocalFileHandle &file_handle) {
        size_t origin_dim;
        file_handle.Read(&origin_dim, sizeof(origin_dim));
        This meta(origin_dim);
        size_t dim = meta.dim_;
        file_handle.Read(meta.rom_.get(), dim * dim * sizeof(DataType));
        file_handle.Read(meta.rot_centroid_.get(), dim * sizeof(DataType));
        return meta;
    }

    static This LoadFromPtr(const char *&ptr) {
        size_t origin_dim = ReadBufAdv<size_t>(ptr);
        This meta(origin_dim);
        size_t dim = meta.dim_;
        std::memcpy(meta.rom_.get(), ptr, dim * dim * sizeof(DataType));
        ptr += dim * dim * sizeof(DataType);
        std::memcpy(meta.rot_centroid_.get(), ptr, dim * sizeof(DataType));
        ptr += dim * sizeof(DataType);
        return meta;
    }

    template <typename LabelType, DataIteratorConcept<const DataType *, LabelType> Iterator>
    void Optimize(Iterator &&query_iter, const std::vector<std::pair<Inner *, size_t>> &inners, size_t &mem_usage) {
        size_t dim = this->dim_;
        // Decompress old vector
        auto new_centroid = std::make_unique<DataType[]>(dim);
        auto temp_decompress = std::make_unique<DataType[]>(dim);
        size_t cur_vec_num = 0;
        for (const auto [inner, size] : inners) {
            for (size_t i = 0; i < size; ++i) {
                this->DecompressCode(inner->GetVec(i, *this), temp_decompress.get());
                for (size_t j = 0; j < dim; ++j) {
                    new_centroid[j] += temp_decompress[j];
                }
            }
            cur_vec_num += size;
        }

        // Add new vector
        while (true) {
            if (auto ret = query_iter.Next(); ret) {
                auto &[vec, _] = *ret;
                for (size_t i = 0; i < dim; ++i) {
                    new_centroid[i] += vec[i];
                }
                ++cur_vec_num;
            } else {
                break;
            }
        }
        for (size_t i = 0; i < dim; ++i) {
            new_centroid[i] /= cur_vec_num;
        }

        // Save rot_centroid
        auto new_rot_centroid = std::make_unique<DataType[]>(dim);
        matrixA_multiply_matrixB_output_to_C(new_centroid.get(), this->rom_.get(), 1, dim, dim, new_rot_centroid.get());
        new_rot_centroid = this->rot_centroid_.exchange(std::move(new_rot_centroid));

        // Update old vector code
        for (auto [inner, size] : inners) {
            for (size_t i = 0; i < size; ++i) {
                this->DecompressCode(inner->GetVec(i, *this), temp_decompress.get(), new_rot_centroid.get());
                inner->SetVec(i, temp_decompress.get(), *this, mem_usage);
            }
        }
    }
};

export template <typename DataType>
class RabitqVecStoreMeta<DataType, false> : public RabitqVecStoreMetaBase<DataType, false> {
public:
    using This = RabitqVecStoreMeta<DataType, false>;
    using Base = RabitqVecStoreMetaBase<DataType, false>;
    using MetaType = typename Base::MetaType;
    using StoreData = typename Base::StoreData;
    using QueryData = typename Base::QueryData;
    using AlignType = typename Base::AlignType;
    using CompressType = typename Base::CompressType;

private:
    RabitqVecStoreMeta(size_t origin_dim, DataType *rom, DataType *rot_centroid) {
        this->origin_dim_ = origin_dim;
        this->rom_ = rom;
        this->rot_centroid_ = rot_centroid;
        size_t dim = AlignUp(origin_dim, MetaType::align_size_);
        this->dim_ = dim;
        this->compress_data_size_ = sizeof(StoreData) + dim / MetaType::align_size_;
        this->compress_query_size_ = sizeof(QueryData) + dim * sizeof(CompressType);
    }

public:
    RabitqVecStoreMeta() = default;

    static This LoadFromPtr(const char *&ptr) {
        size_t origin_dim = ReadBufAdv<size_t>(ptr);
        size_t dim = AlignUp(origin_dim, MetaType::align_size_);
        auto *rom = reinterpret_cast<DataType *>(const_cast<char *>(ptr));
        ptr += dim * dim * sizeof(DataType);
        auto *rot_centroid = reinterpret_cast<DataType *>(const_cast<char *>(ptr));
        ptr += dim * sizeof(DataType);
        return This(origin_dim, rom, rot_centroid);
    }
};

/////////////////////////// data operation of inner ///////////////////////////

template <typename DataType, bool OwnMem>
class RabitqVecStoreInnerBase {
public:
    using This = RabitqVecStoreInnerBase<DataType, OwnMem>;
    using Meta = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using StoreType = typename Meta::StoreType;

public:
    RabitqVecStoreInnerBase() = default;

    size_t GetSizeInBytes(size_t cur_vec_num, const Meta &meta) const { return cur_vec_num * meta.compress_data_size(); }

    void Save(LocalFileHandle &file_handle, size_t cur_vec_num, const Meta &meta) const {
        file_handle.Append(ptr_.get(), cur_vec_num * meta.compress_data_size());
    }

    static void SaveToPtr(LocalFileHandle &file_handle,
                          const std::vector<const This *> &inners,
                          const Meta &meta,
                          size_t ck_size,
                          size_t chunk_num,
                          size_t last_chunk_size) {
        for (size_t i = 0; i < chunk_num; ++i) {
            size_t chunk_size = (i < chunk_num - 1) ? ck_size : last_chunk_size;
            file_handle.Append(inners[i]->ptr_.get(), chunk_size * meta.compress_data_size());
        }
    }

    StoreType GetVec(size_t idx, const Meta &meta) const { return reinterpret_cast<StoreType>(ptr_.get() + idx * meta.compress_data_size()); }

    void Prefetch(VertexType vec_i, const Meta &meta) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i, meta)), _MM_HINT_T0); }

protected:
    ArrayPtr<char, OwnMem> ptr_;
};

export template <typename DataType, bool OwnMem>
class RabitqVecStoreInner : public RabitqVecStoreInnerBase<DataType, OwnMem> {
public:
    using This = RabitqVecStoreInner<DataType, OwnMem>;
    using Meta = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using StoreData = typename Meta::StoreData;

private:
    RabitqVecStoreInner(size_t max_vec_num, const Meta &meta) { this->ptr_ = std::make_unique<char[]>(max_vec_num * meta.compress_data_size()); }

public:
    RabitqVecStoreInner() = default;

    static This Make(size_t max_vec_num, const Meta &meta, size_t &mem_usage) {
        auto ret = This(max_vec_num, meta);
        mem_usage += max_vec_num * meta.compress_data_size();
        return ret;
    }

    static This Load(LocalFileHandle &file_handle, size_t cur_vec_num, size_t max_vec_num, const Meta &meta, size_t &mem_usage) {
        assert(cur_vec_num <= max_vec_num);
        This ret(max_vec_num, meta);
        file_handle.Read(ret.ptr_.get(), cur_vec_num * meta.compress_data_size());
        mem_usage += max_vec_num * meta.compress_data_size();
        return ret;
    }

    static This LoadFromPtr(const char *&ptr, size_t cur_vec_num, size_t max_vec_num, const Meta &meta, size_t &mem_usage) {
        This ret(max_vec_num, meta);
        std::memcpy(ret.ptr_.get(), ptr, cur_vec_num * meta.compress_data_size());
        ptr += cur_vec_num * meta.compress_data_size();
        mem_usage += max_vec_num * meta.compress_data_size();
        return ret;
    }

    void SetVec(size_t idx, const DataType *vec, const Meta &meta, size_t &mem_usage) { meta.CompressToCode(vec, GetVecMut(idx, meta)); }

private:
    StoreData *GetVecMut(size_t idx, const Meta &meta) { return reinterpret_cast<StoreData *>(this->ptr_.get() + idx * meta.compress_data_size()); }
};

export template <typename DataType>
class RabitqVecStoreInner<DataType, false> : public RabitqVecStoreInnerBase<DataType, false> {
public:
    using This = RabitqVecStoreInner<DataType, false>;
    using Meta = RabitqVecStoreMetaBase<DataType, false>;

private:
    RabitqVecStoreInner(const char *ptr) { this->ptr_ = ptr; }

public:
    RabitqVecStoreInner() = default;

    static This LoadFromPtr(const char *&ptr, size_t cur_vec_num, const Meta &meta) {
        const char *p = ptr;
        This ret(p);
        ptr += cur_vec_num * meta.compress_data_size();
        return ret;
    }
};

} // namespace infinity