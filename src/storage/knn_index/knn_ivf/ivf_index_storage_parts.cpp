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
#include <cmath>
#include <vector>

module ivf_index_storage;

import stl;
import infinity_exception;
import status;
import logger;
import third_party;
import index_ivf;
import column_vector;
import internal_types;
import logical_type;
import data_type;
import kmeans_partition;
import search_top_1;
import search_top_k;
import column_vector;
import knn_scan_data;
import ivf_index_util_func;
import mlas_matrix_multiply;
import vector_distance;
import index_base;
import knn_expr;
import simd_functions;

namespace infinity {

struct SearchIndexPartsReuseContext {
    UniquePtr<f32[]> pq_query_ip_table_ = {};
};

class IVF_Part_Storage {
    const u32 part_id_ = std::numeric_limits<u32>::max();

protected:
    u32 embedding_num_ = 0;
    Vector<SegmentOffset> embedding_segment_offsets_ = {};

public:
    explicit IVF_Part_Storage(const u32 part_id) : part_id_(part_id) {}
    virtual ~IVF_Part_Storage() = default;
    static UniquePtr<IVF_Part_Storage>
    Make(u32 part_id, u32 embedding_dimension, EmbeddingDataType embedding_data_type, const IndexIVFStorageOption &ivf_storage_option);
    u32 part_id() const { return part_id_; }
    u32 embedding_num() const { return embedding_num_; }
    SegmentOffset embedding_segment_offset(const u32 embedding_index) const { return embedding_segment_offsets_[embedding_index]; }

    virtual void Save(LocalFileHandle &file_handle) const {
        file_handle.Append(&embedding_num_, sizeof(embedding_num_));
        static_assert(std::is_same_v<SegmentOffset, typename decltype(embedding_segment_offsets_)::value_type>);
        assert(embedding_num_ == embedding_segment_offsets_.size());
        file_handle.Append(embedding_segment_offsets_.data(), embedding_num_ * sizeof(SegmentOffset));
    }

    virtual void Load(LocalFileHandle &file_handle) {
        file_handle.Read(&embedding_num_, sizeof(embedding_num_));
        embedding_segment_offsets_.resize(embedding_num_);
        file_handle.Read(embedding_segment_offsets_.data(), embedding_num_ * sizeof(SegmentOffset));
    }

    virtual void AppendOneEmbedding(const void *embedding_ptr,
                                    SegmentOffset segment_offset,
                                    const IVF_Centroids_Storage *ivf_centroids_storage,
                                    const IVF_Parts_Storage *ivf_parts_storage) = 0;

    virtual void SearchIndex(const IVF_Index_Storage *ivf_index_storage,
                             const KnnDistanceBase1 *knn_distance,
                             const void *query_ptr,
                             EmbeddingDataType query_element_type,
                             const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                             const std::function<void(f32, SegmentOffset)> &add_result_func,
                             SearchIndexPartsReuseContext &context) const = 0;
};

template <IndexIVFStorageOption::Type t>
class IVF_Parts_Storage_Info;

template <>
class IVF_Parts_Storage_Info<IndexIVFStorageOption::Type::kPlain> : public IVF_Parts_Storage {
public:
    IVF_Parts_Storage_Info(const u32 embedding_dimension,
                           const u32 centroids_num,
                           const EmbeddingDataType embedding_data_type,
                           const IndexIVFStorageOption &ivf_storage_option)
        : IVF_Parts_Storage(embedding_dimension, centroids_num) {}
    ~IVF_Parts_Storage_Info() override = default;
    void Save(LocalFileHandle &file_handle) const override {}
    void Load(LocalFileHandle &file_handle) override {}
    void Train(const u32 training_embedding_num, const f32 *training_data, const IVF_Centroids_Storage *ivf_centroids_storage) final {}
};

UniquePtr<f32[]> GetTrainingResidual(const u32 training_embedding_num,
                                     const f32 *training_data,
                                     const IVF_Centroids_Storage *ivf_centroids_storage,
                                     const u32 embedding_dimension,
                                     const u32 centroids_num) {
    auto residuals = MakeUniqueForOverwrite<f32[]>(training_embedding_num * embedding_dimension);
    const f32 *centroids_data = ivf_centroids_storage->data();
    // (-0.5 * norm) for each centroid
    UniquePtr<f32[]> centroid_norms_neg_half = MakeUniqueForOverwrite<f32[]>(centroids_num);
    {
        // prepare centroid_norms_neg_half_
        const f32 *centroids_data_ptr = centroids_data;
        for (u32 i = 0; i < centroids_num; ++i) {
            centroid_norms_neg_half[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroids_data_ptr, embedding_dimension);
            centroids_data_ptr += embedding_dimension;
        }
    }
    // distance: for every embedding, e * c - 0.5 * c^2, find max
    const auto dist_table = MakeUniqueForOverwrite<f32[]>(training_embedding_num * centroids_num);
    matrixA_multiply_transpose_matrixB_output_to_C(training_data,
                                                   centroids_data,
                                                   training_embedding_num,
                                                   centroids_num,
                                                   embedding_dimension,
                                                   dist_table.get());
    for (u32 i = 0; i < training_embedding_num; ++i) {
        const f32 *embedding_data_ptr = training_data + i * embedding_dimension;
        f32 *output_ptr = residuals.get() + i * embedding_dimension;
        f32 max_neg_distance = std::numeric_limits<f32>::lowest();
        u64 max_id = 0;
        const f32 *dist_ptr = dist_table.get() + i * centroids_num;
        for (u32 k = 0; k < centroids_num; ++k) {
            if (const f32 neg_distance = dist_ptr[k] + centroid_norms_neg_half[k]; neg_distance > max_neg_distance) {
                max_neg_distance = neg_distance;
                max_id = k;
            }
        }
        const f32 *centroids_data_ptr = centroids_data + max_id * embedding_dimension;
        for (u32 j = 0; j < embedding_dimension; ++j) {
            output_ptr[j] = embedding_data_ptr[j] - centroids_data_ptr[j];
        }
    }
    return residuals;
}

// quantize with middle 99% range
// r = a * n + b (*: Hadamard product), v = c + r + e
// common a, b for all residuals
// n in [0, 15) or [0, 255)
// query x, centroid c
// dot(x, v) = dot(x, (c + r + e)) = dot(x, c + b) + dot((x * a), n) + dot(v, e)
// dot(v, v) = dot(c + b + a * n + e, v) = dot(c + b, c + b) + dot(a^2, n^2) + 2 * dot ((c + b) * a, n) + 2 * dot(v, e)
// l2(a * n + b + c - x) = dot(a^2, n^2) + l2(b + c - x) + 2 * dot(a * (b + c - x), n)

template <>
class IVF_Parts_Storage_Info<IndexIVFStorageOption::Type::kScalarQuantization> : public IVF_Parts_Storage {
    const u32 sq_bits_ = 0; // 4 or 8
    Vector<f32> common_vec_a_;
    Vector<f32> common_vec_b_;

public:
    IVF_Parts_Storage_Info(const u32 embedding_dimension,
                           const u32 centroids_num,
                           const EmbeddingDataType embedding_data_type,
                           const IndexIVFStorageOption &ivf_storage_option)
        : IVF_Parts_Storage(embedding_dimension, centroids_num), sq_bits_(ivf_storage_option.scalar_quantization_bits_),
          common_vec_a_(embedding_dimension), common_vec_b_(embedding_dimension) {
        assert(sq_bits_ == 4 || sq_bits_ == 8);
    }
    ~IVF_Parts_Storage_Info() override = default;

    const f32 *data_a() const { return common_vec_a_.data(); }
    const f32 *data_b() const { return common_vec_b_.data(); }

    void Save(LocalFileHandle &file_handle) const override {
        file_handle.Append(common_vec_a_.data(), embedding_dimension() * sizeof(f32));
        file_handle.Append(common_vec_b_.data(), embedding_dimension() * sizeof(f32));
    }
    void Load(LocalFileHandle &file_handle) override {
        file_handle.Read(common_vec_a_.data(), embedding_dimension() * sizeof(f32));
        file_handle.Read(common_vec_b_.data(), embedding_dimension() * sizeof(f32));
    }
    void Train(const u32 training_embedding_num, const f32 *training_data, const IVF_Centroids_Storage *ivf_centroids_storage) final {
        const auto residuals =
            GetTrainingResidual(training_embedding_num, training_data, ivf_centroids_storage, embedding_dimension(), centroids_num());
        const auto exclude_num_each_end = std::min<u32>((training_embedding_num / 200u) + 1u, training_embedding_num);
        for (u32 i = 0; i < embedding_dimension(); ++i) {
            std::priority_queue<f32> min_heap;                              // biggest on top
            std::priority_queue<f32, Vector<f32>, std::greater<>> max_heap; // smallest on top
            const f32 *residual_data = residuals.get() + i;
            for (u32 j = 0; j < exclude_num_each_end; ++j) {
                const f32 x = *residual_data;
                min_heap.push(x);
                max_heap.push(x);
                residual_data += embedding_dimension();
            }
            for (u32 j = exclude_num_each_end; j < training_embedding_num; ++j) {
                const f32 x = *residual_data;
                if (x < min_heap.top()) {
                    min_heap.pop();
                    min_heap.push(x);
                }
                if (x > max_heap.top()) {
                    max_heap.pop();
                    max_heap.push(x);
                }
                residual_data += embedding_dimension();
            }
            const f32 range_start = min_heap.top();
            const f32 range_end = max_heap.top();
            assert(range_start <= range_end);
            const u32 range_parts = 1u << sq_bits_;
            const f32 range_stride = (range_end - range_start) / range_parts;
            common_vec_a_[i] = range_stride;
            common_vec_b_[i] = range_start + 0.5f * range_stride;
        }
    }
};

template <>
class IVF_Parts_Storage_Info<IndexIVFStorageOption::Type::kProductQuantization> : public IVF_Parts_Storage {
    const u32 subspace_num_ = 0;
    const u32 subspace_dimension_ = embedding_dimension() / subspace_num_;
    const u32 subspace_centroid_bits_ = 0;
    const u32 expect_subspace_centroid_num_ = 1u << subspace_centroid_bits_;

    u32 real_subspace_centroid_num_ = 0;
    // centroids for each subspace, size: subspace_dimension_ * real_subspace_centroid_num_ * subspace_num_
    UniquePtr<f32[]> subspace_centroids_data_ = {};
    // size: real_subspace_centroid_num_ * subspace_num_
    UniquePtr<f32[]> subspace_centroid_norms_neg_half_ = {};

public:
    auto real_subspace_centroid_num() const { return real_subspace_centroid_num_; }

    const f32 *subspace_centroids_data_at_subspace(const u32 subspace_id) const {
        return subspace_centroids_data_.get() + subspace_id * subspace_dimension_ * real_subspace_centroid_num_;
    }

    const f32 *subspace_centroid_norms_neg_half_at_subspace(const u32 subspace_id) const {
        return subspace_centroid_norms_neg_half_.get() + subspace_id * real_subspace_centroid_num_;
    }

#define NON_CONST_VERSION_MEMBER_FUNC(func_name)                                                                                                     \
    f32 *func_name(const u32 subspace_id) { return const_cast<f32 *>(const_cast<const IVF_Parts_Storage_Info *>(this)->func_name(subspace_id)); }

    NON_CONST_VERSION_MEMBER_FUNC(subspace_centroids_data_at_subspace);
    NON_CONST_VERSION_MEMBER_FUNC(subspace_centroid_norms_neg_half_at_subspace);

    IVF_Parts_Storage_Info(const u32 embedding_dim,
                           const u32 centroids_num,
                           const EmbeddingDataType embedding_data_type,
                           const IndexIVFStorageOption &ivf_storage_option)
        : IVF_Parts_Storage(embedding_dim, centroids_num), subspace_num_(ivf_storage_option.product_quantization_subspace_num_),
          subspace_centroid_bits_(ivf_storage_option.product_quantization_subspace_bits_) {
        assert(subspace_dimension_ * subspace_num_ == embedding_dimension());
        assert((embedding_dimension() << subspace_centroid_bits_) == subspace_dimension_ * expect_subspace_centroid_num_ * subspace_num_);
    }

    ~IVF_Parts_Storage_Info() override = default;

    void Save(LocalFileHandle &file_handle) const override {
        file_handle.Append(&real_subspace_centroid_num_, sizeof(real_subspace_centroid_num_));
        file_handle.Append(subspace_centroids_data_.get(), embedding_dimension() * real_subspace_centroid_num_ * sizeof(f32));
        file_handle.Append(subspace_centroid_norms_neg_half_.get(), real_subspace_centroid_num_ * subspace_num_ * sizeof(f32));
    }

    void Load(LocalFileHandle &file_handle) override {
        file_handle.Read(&real_subspace_centroid_num_, sizeof(real_subspace_centroid_num_));
        subspace_centroids_data_ = MakeUniqueForOverwrite<f32[]>(embedding_dimension() * real_subspace_centroid_num_);
        file_handle.Read(subspace_centroids_data_.get(), embedding_dimension() * real_subspace_centroid_num_ * sizeof(f32));
        subspace_centroid_norms_neg_half_ = MakeUniqueForOverwrite<f32[]>(real_subspace_centroid_num_ * subspace_num_);
        file_handle.Read(subspace_centroid_norms_neg_half_.get(), real_subspace_centroid_num_ * subspace_num_ * sizeof(f32));
    }

    void Train(const u32 training_embedding_num, const f32 *training_data, const IVF_Centroids_Storage *ivf_centroids_storage) final {
        const auto residuals =
            GetTrainingResidual(training_embedding_num, training_data, ivf_centroids_storage, embedding_dimension(), centroids_num());
        // train residuals
        TrainResidual(training_embedding_num, residuals.get());
    }

    void TrainResidual(const u32 training_embedding_num, const f32 *residual_data) {
        real_subspace_centroid_num_ = std::min(expect_subspace_centroid_num_, training_embedding_num);
        subspace_centroids_data_ = MakeUniqueForOverwrite<f32[]>(embedding_dimension() * real_subspace_centroid_num_);
        subspace_centroid_norms_neg_half_ = MakeUniqueForOverwrite<f32[]>(real_subspace_centroid_num_ * subspace_num_);
        const auto part_train_data = MakeUniqueForOverwrite<f32[]>(training_embedding_num * subspace_dimension_);
        Vector<f32> subspace_centroids_v;
        for (u32 i = 0; i < subspace_num_; ++i) {
            auto *const output_centroids_data = subspace_centroids_data_at_subspace(i);
            auto *const output_centroid_norms_neg_half = subspace_centroid_norms_neg_half_at_subspace(i);
            {
                // copy subspace data
                const f32 *copy_src = residual_data + i * subspace_dimension_;
                f32 *copy_dst = part_train_data.get();
                for (u32 j = 0; j < training_embedding_num; ++j) {
                    std::copy_n(copy_src, subspace_dimension_, copy_dst);
                    copy_src += embedding_dimension();
                    copy_dst += subspace_dimension_;
                }
            }
            if (training_embedding_num <= expect_subspace_centroid_num_) {
                assert(training_embedding_num == real_subspace_centroid_num_);
                // no k-means
                std::copy_n(part_train_data.get(), training_embedding_num * subspace_dimension_, output_centroids_data);
            } else {
                assert(real_subspace_centroid_num_ == expect_subspace_centroid_num_);
                // k-means
                const auto centroid_cnt_result = GetKMeansCentroids(MetricType::kMetricL2,
                                                                    subspace_dimension_,
                                                                    training_embedding_num,
                                                                    part_train_data.get(),
                                                                    subspace_centroids_v,
                                                                    real_subspace_centroid_num_);
                if (centroid_cnt_result != real_subspace_centroid_num_) {
                    UnrecoverableError(fmt::format("KMeans failed to find {} centroids for subspace", real_subspace_centroid_num_));
                }
                if (const u32 subspace_centroid_data_size = real_subspace_centroid_num_ * subspace_dimension_;
                    subspace_centroids_v.size() != subspace_centroid_data_size) {
                    UnrecoverableError(
                        fmt::format("centroids size {} not equal to expected size {}", subspace_centroids_v.size(), subspace_centroid_data_size));
                }
                std::copy(subspace_centroids_v.begin(), subspace_centroids_v.end(), output_centroids_data);
            }
            // compute norms
            const f32 *centroid_data = output_centroids_data;
            for (u32 j = 0; j < real_subspace_centroid_num_; ++j) {
                output_centroid_norms_neg_half[j] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_data, subspace_dimension_);
                centroid_data += subspace_dimension_;
            }
        }
    }

    void EncodeResidual(const f32 *residual, u32 *encode_output) const {
        const auto xy_buffer = MakeUniqueForOverwrite<f32[]>(real_subspace_centroid_num_);
        for (u32 j = 0; j < subspace_num_; ++j) {
            matrixA_multiply_transpose_matrixB_output_to_C(residual + j * subspace_dimension_,
                                                           subspace_centroids_data_at_subspace(j),
                                                           1,
                                                           real_subspace_centroid_num_,
                                                           subspace_dimension_,
                                                           xy_buffer.get());
            // find max id (for every embedding, find centroid with min l2 distance, and equivalently max (x*y - 0.5*y^2))
            const auto *c_norm_data = subspace_centroid_norms_neg_half_at_subspace(j);
            f32 max_neg_distance = std::numeric_limits<f32>::lowest();
            u32 max_id = 0;
            for (u32 k = 0; k < real_subspace_centroid_num_; ++k) {
                if (const f32 neg_distance = xy_buffer[k] + c_norm_data[k]; neg_distance > max_neg_distance) {
                    max_neg_distance = neg_distance;
                    max_id = k;
                }
            }
            encode_output[j] = max_id;
        }
    }

    UniquePtr<f32[]> GetIPTable(const f32 *query) const {
        auto ip_table = MakeUniqueForOverwrite<f32[]>(subspace_num_ * real_subspace_centroid_num_);
        for (u32 i = 0; i < subspace_num_; ++i) {
            matrixA_multiply_matrixB_output_to_C(subspace_centroids_data_at_subspace(i),
                                                 query + i * subspace_dimension_,
                                                 real_subspace_centroid_num_,
                                                 1,
                                                 subspace_dimension_,
                                                 ip_table.get() + i * real_subspace_centroid_num_);
        }
        return ip_table;
    }
};

template <IndexIVFStorageOption::Type t>
class IVF_Parts_Storage_T final : public IVF_Parts_Storage_Info<t> {
    using BaseT = IVF_Parts_Storage_Info<t>;
    static_assert(std::derived_from<BaseT, IVF_Parts_Storage>);

    Vector<UniquePtr<IVF_Part_Storage>> ivf_part_storages_ = {};

public:
    IVF_Parts_Storage_T(const u32 embedding_dimension,
                        const u32 centroids_num,
                        const EmbeddingDataType embedding_data_type,
                        const IndexIVFStorageOption &ivf_storage_option)
        : BaseT(embedding_dimension, centroids_num, embedding_data_type, ivf_storage_option) {
        ivf_part_storages_.resize(centroids_num);
        for (u32 part_id = 0; part_id < centroids_num; ++part_id) {
            ivf_part_storages_[part_id] = IVF_Part_Storage::Make(part_id, embedding_dimension, embedding_data_type, ivf_storage_option);
        }
    }

    ~IVF_Parts_Storage_T() override = default;

    void Save(LocalFileHandle &file_handle) const override {
        BaseT::Save(file_handle);
        for (const auto &ivf_part_storage : ivf_part_storages_) {
            ivf_part_storage->Save(file_handle);
        }
    }

    void Load(LocalFileHandle &file_handle) override {
        BaseT::Load(file_handle);
        for (const auto &ivf_part_storage : ivf_part_storages_) {
            ivf_part_storage->Load(file_handle);
        }
    }

    void AppendOneEmbedding(const u32 part_id,
                            const void *embedding_ptr,
                            const SegmentOffset segment_offset,
                            const IVF_Centroids_Storage *ivf_centroids_storage) override {
        return ivf_part_storages_[part_id]->AppendOneEmbedding(embedding_ptr, segment_offset, ivf_centroids_storage, this);
    }

    void SearchIndex(const Vector<u32> &part_ids,
                     const IVF_Index_Storage *ivf_index_storage,
                     const KnnDistanceBase1 *knn_distance,
                     const void *query_ptr,
                     const EmbeddingDataType query_element_type,
                     const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                     const std::function<void(f32, SegmentOffset)> &add_result_func) const override {
        SearchIndexPartsReuseContext context;
        for (const auto part_id : part_ids) {
            ivf_part_storages_[part_id]
                ->SearchIndex(ivf_index_storage, knn_distance, query_ptr, query_element_type, satisfy_filter_func, add_result_func, context);
        }
    }
};

UniquePtr<IVF_Parts_Storage> IVF_Parts_Storage::Make(const u32 embedding_dimension,
                                                     const u32 centroids_num,
                                                     const EmbeddingDataType embedding_data_type,
                                                     const IndexIVFStorageOption &ivf_storage_option) {
    auto GetPartsStorageT = [&]<IndexIVFStorageOption::Type t>() {
        return MakeUnique<IVF_Parts_Storage_T<t>>(embedding_dimension, centroids_num, embedding_data_type, ivf_storage_option);
    };
    switch (ivf_storage_option.type_) {
        case IndexIVFStorageOption::Type::kPlain: {
            return GetPartsStorageT.template operator()<IndexIVFStorageOption::Type::kPlain>();
        }
        case IndexIVFStorageOption::Type::kScalarQuantization: {
            return GetPartsStorageT.template operator()<IndexIVFStorageOption::Type::kScalarQuantization>();
        }
        case IndexIVFStorageOption::Type::kProductQuantization: {
            return GetPartsStorageT.template operator()<IndexIVFStorageOption::Type::kProductQuantization>();
        }
    }
    return {};
}

// Plain storage
template <EmbeddingDataType plain_data_type, EmbeddingDataType src_embedding_data_type>
class IVF_Part_Storage_Plain final : public IVF_Part_Storage {
    using StorageDataT = EmbeddingDataTypeToCppTypeT<plain_data_type>;
    static_assert(IsAnyOf<StorageDataT, i8, u8, f32, Float16T, BFloat16T>);
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<src_embedding_data_type>;
    static_assert(IsAnyOf<ColumnEmbeddingElementT, i8, u8, f64, f32, Float16T, BFloat16T>);
    static_assert(std::is_same_v<StorageDataT, ColumnEmbeddingElementT> ||
                  (!IsAnyOf<StorageDataT, i8, u8> && !IsAnyOf<ColumnEmbeddingElementT, i8, u8>));

    const u32 embedding_dimension_ = 0;
    Vector<StorageDataT> data_{};

public:
    IVF_Part_Storage_Plain(const u32 part_id, const u32 embedding_dimension) : IVF_Part_Storage(part_id), embedding_dimension_(embedding_dimension) {}

    [[nodiscard]] u32 embedding_dimension() const { return embedding_dimension_; }

    void Save(LocalFileHandle &file_handle) const override {
        IVF_Part_Storage::Save(file_handle);
        const u32 element_cnt = embedding_num() * embedding_dimension();
        assert(element_cnt == data_.size());
        file_handle.Append(data_.data(), element_cnt * sizeof(StorageDataT));
    }
    void Load(LocalFileHandle &file_handle) override {
        IVF_Part_Storage::Load(file_handle);
        const u32 element_cnt = embedding_num() * embedding_dimension();
        data_.resize(element_cnt);
        file_handle.Read(data_.data(), element_cnt * sizeof(StorageDataT));
    }

    void AppendOneEmbedding(const void *embedding_ptr,
                            const SegmentOffset segment_offset,
                            const IVF_Centroids_Storage *,
                            const IVF_Parts_Storage *) override {
        const auto *src_embedding_data = static_cast<const ColumnEmbeddingElementT *>(embedding_ptr);
        if constexpr (std::is_same_v<StorageDataT, ColumnEmbeddingElementT>) {
            data_.insert(data_.end(), src_embedding_data, src_embedding_data + embedding_dimension());
        } else {
            static_assert(IsAnyOf<StorageDataT, f32, Float16T, BFloat16T> && IsAnyOf<ColumnEmbeddingElementT, f64, f32, Float16T, BFloat16T>);
            data_.reserve(data_.size() + embedding_dimension());
            for (u32 i = 0; i < embedding_dimension(); ++i) {
                if constexpr (std::is_same_v<f64, ColumnEmbeddingElementT>) {
                    data_.push_back(static_cast<StorageDataT>(static_cast<f32>(src_embedding_data[i])));
                } else {
                    data_.push_back(static_cast<StorageDataT>(src_embedding_data[i]));
                }
            }
        }
        embedding_segment_offsets_.push_back(segment_offset);
        ++embedding_num_;
    }

    void SearchIndex(const IVF_Index_Storage *,
                     const KnnDistanceBase1 *knn_distance,
                     const void *query_ptr,
                     const EmbeddingDataType query_element_type,
                     const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                     const std::function<void(f32, SegmentOffset)> &add_result_func,
                     SearchIndexPartsReuseContext &) const override {
        auto ReturnT = [&]<EmbeddingDataType query_element_type> {
            if constexpr ((query_element_type == EmbeddingDataType::kElemFloat && IsAnyOf<ColumnEmbeddingElementT, f64, f32, Float16T, BFloat16T>) ||
                          (query_element_type == src_embedding_data_type &&
                           (query_element_type == EmbeddingDataType::kElemInt8 || query_element_type == EmbeddingDataType::kElemUInt8))) {
                return SearchIndexT<query_element_type>(knn_distance,
                                                        static_cast<const EmbeddingDataTypeToCppTypeT<query_element_type> *>(query_ptr),
                                                        satisfy_filter_func,
                                                        add_result_func);
            } else {
                UnrecoverableError("Invalid Query EmbeddingDataType");
            }
        };
        switch (query_element_type) {
            case EmbeddingDataType::kElemFloat: {
                return ReturnT.template operator()<EmbeddingDataType::kElemFloat>();
            }
            case EmbeddingDataType::kElemUInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemUInt8>();
            }
            case EmbeddingDataType::kElemInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemInt8>();
            }
            default: {
                UnrecoverableError("Invalid EmbeddingDataType");
            }
        }
    }

    template <EmbeddingDataType query_element_type>
    void SearchIndexT(const KnnDistanceBase1 *knn_distance,
                      const EmbeddingDataTypeToCppTypeT<query_element_type> *query_ptr,
                      const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                      const std::function<void(f32, SegmentOffset)> &add_result_func) const {
        using QueryDataType = EmbeddingDataTypeToCppTypeT<query_element_type>;
        auto knn_distance_1 = dynamic_cast<const KnnDistance1<QueryDataType, f32> *>(knn_distance);
        if (!knn_distance_1) [[unlikely]] {
            UnrecoverableError("Invalid KnnDistance1");
        }
        auto dist_func = knn_distance_1->dist_func_;
        const auto total_embedding_num = embedding_num();
        for (u32 i = 0; i < total_embedding_num; ++i) {
            const auto segment_offset = embedding_segment_offset(i);
            if (!satisfy_filter_func(segment_offset)) {
                continue;
            }
            auto v_ptr = data_.data() + i * embedding_dimension();
            auto [calc_ptr, _] = GetSearchCalcPtr<QueryDataType>(v_ptr, embedding_dimension());
            auto d = dist_func(calc_ptr, query_ptr, embedding_dimension());
            add_result_func(d, segment_offset);
        }
    }
};

// SQ storage
template <u32 sq_bits, EmbeddingDataType src_embedding_data_type>
class IVF_Part_Storage_SQ final : public IVF_Part_Storage {
    static_assert(sq_bits == 4 || sq_bits == 8);
    static constexpr u32 sq_encode_start = 0;
    static constexpr u32 sq_encode_end = (1u << sq_bits) - 1u;
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<src_embedding_data_type>;
    static_assert(IsAnyOf<ColumnEmbeddingElementT, f64, f32, Float16T, BFloat16T>);

    const u32 embedding_dimension_ = 0;
    const u32 embedding_sq_bytes_ = (sq_bits == 8) ? embedding_dimension_ : ((embedding_dimension_ + 1) / 2);
    Vector<u8> sq_data_{};
    Vector<f32> dot_v_e_{};

public:
    IVF_Part_Storage_SQ(const u32 part_id, const u32 embedding_dimension) : IVF_Part_Storage(part_id), embedding_dimension_(embedding_dimension) {}

    void Save(LocalFileHandle &file_handle) const override {
        IVF_Part_Storage::Save(file_handle);
        const u32 element_cnt = embedding_num() * embedding_sq_bytes_;
        assert(element_cnt == sq_data_.size());
        file_handle.Append(sq_data_.data(), element_cnt * sizeof(u8));
        assert(embedding_num() == dot_v_e_.size());
        file_handle.Append(dot_v_e_.data(), embedding_num() * sizeof(f32));
    }

    void Load(LocalFileHandle &file_handle) override {
        IVF_Part_Storage::Load(file_handle);
        const u32 element_cnt = embedding_num() * embedding_sq_bytes_;
        sq_data_.resize(element_cnt);
        file_handle.Read(sq_data_.data(), element_cnt * sizeof(u8));
        dot_v_e_.resize(embedding_num());
        file_handle.Read(dot_v_e_.data(), embedding_num() * sizeof(f32));
    }

    void AppendOneEmbedding(const void *embedding_ptr,
                            const SegmentOffset segment_offset,
                            const IVF_Centroids_Storage *ivf_centroids_storage,
                            const IVF_Parts_Storage *ivf_parts_storage) override {
        const auto *src_embedding_data = static_cast<const ColumnEmbeddingElementT *>(embedding_ptr);
        const auto [src_embedding_f32, _] = GetF32Ptr(src_embedding_data, embedding_dimension_);
        Vector<u8> encode_output(embedding_sq_bytes_);
        Vector<f32> error_v(embedding_dimension_);
        const auto *ivf_parts_storage_info =
            dynamic_cast<const IVF_Parts_Storage_Info<IndexIVFStorageOption::Type::kScalarQuantization> *>(ivf_parts_storage);
        assert(ivf_parts_storage_info);
        const auto centroid_data = ivf_centroids_storage->data() + part_id() * embedding_dimension_;
        for (u32 i = 0; i < embedding_dimension_; ++i) {
            const f32 residual_i = src_embedding_f32[i] - centroid_data[i];
            const f32 a = ivf_parts_storage_info->data_a()[i];
            const f32 b = ivf_parts_storage_info->data_b()[i];
            u32 n = {};
            if (const f32 n_f = std::round((residual_i - b) / (a > 0.0f ? a : 1.0f)); n_f < sq_encode_start) {
                n = sq_encode_start;
            } else if (n_f >= sq_encode_end) {
                n = sq_encode_end;
            } else {
                n = static_cast<u32>(n_f);
            }
            if constexpr (sq_bits == 8) {
                encode_output[i] = static_cast<u8>(n);
            } else {
                static_assert(sq_bits == 4);
                encode_output[i / 2] |= (i & 1) ? (n << 4) : n;
            }
            error_v[i] = residual_i - (a * n + b);
        }
        const auto v_e_ip = IPDistance<f32>(src_embedding_f32, error_v.data(), embedding_dimension_);
        dot_v_e_.push_back(v_e_ip);
        sq_data_.insert(sq_data_.end(), encode_output.begin(), encode_output.end());
        embedding_segment_offsets_.push_back(segment_offset);
        ++embedding_num_;
    }

    void SearchIndex(const IVF_Index_Storage *ivf_index_storage,
                     const KnnDistanceBase1 *knn_distance,
                     const void *query_ptr,
                     const EmbeddingDataType query_element_type,
                     const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                     const std::function<void(f32, SegmentOffset)> &add_result_func,
                     SearchIndexPartsReuseContext &context) const override {
        auto ReturnT = [&]<EmbeddingDataType query_element_type> {
            if constexpr ((query_element_type == EmbeddingDataType::kElemFloat && IsAnyOf<ColumnEmbeddingElementT, f64, f32, Float16T, BFloat16T>) ||
                          (query_element_type == src_embedding_data_type &&
                           (query_element_type == EmbeddingDataType::kElemInt8 || query_element_type == EmbeddingDataType::kElemUInt8))) {
                return SearchIndexT<query_element_type>(ivf_index_storage,
                                                        knn_distance,
                                                        static_cast<const EmbeddingDataTypeToCppTypeT<query_element_type> *>(query_ptr),
                                                        satisfy_filter_func,
                                                        add_result_func,
                                                        context);
            } else {
                UnrecoverableError("Invalid Query EmbeddingDataType");
            }
        };
        switch (query_element_type) {
            case EmbeddingDataType::kElemFloat: {
                return ReturnT.template operator()<EmbeddingDataType::kElemFloat>();
            }
            case EmbeddingDataType::kElemUInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemUInt8>();
            }
            case EmbeddingDataType::kElemInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemInt8>();
            }
            default: {
                UnrecoverableError("Invalid EmbeddingDataType");
            }
        }
    }

    static inline u32 sq_decode(const u8 *sq_data, const u32 pos)
        requires(sq_bits == 8)
    {
        return sq_data[pos];
    }

    static inline u32 sq_decode(const u8 *sq_data, const u32 pos)
        requires(sq_bits == 4)
    {
        return (pos & 1) ? (sq_data[pos / 2] >> 4) : (sq_data[pos / 2] & 0xf);
    }

    template <EmbeddingDataType query_element_type>
    void SearchIndexT(const IVF_Index_Storage *ivf_index_storage,
                      const KnnDistanceBase1 *knn_distance,
                      const EmbeddingDataTypeToCppTypeT<query_element_type> *query_ptr,
                      const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                      const std::function<void(f32, SegmentOffset)> &add_result_func,
                      SearchIndexPartsReuseContext &context) const {
        using QueryDataType = EmbeddingDataTypeToCppTypeT<query_element_type>;
        static_assert(std::is_same_v<QueryDataType, f32>);
        auto knn_distance_1 = dynamic_cast<const KnnDistance1<QueryDataType, f32> *>(knn_distance);
        if (!knn_distance_1) [[unlikely]] {
            UnrecoverableError("Invalid KnnDistance1");
        }
        const auto &ivf_parts_storage =
            static_cast<const IVF_Parts_Storage_Info<IndexIVFStorageOption::Type::kScalarQuantization> &>(ivf_index_storage->ivf_parts_storage());
        const auto dimension = embedding_dimension_;
        const auto a_ptr = ivf_parts_storage.data_a();
        const auto b_ptr = ivf_parts_storage.data_b();
        const auto c_ptr = ivf_index_storage->ivf_centroids_storage().data() + part_id() * dimension;
        const auto [x_ptr, _] = GetF32Ptr(query_ptr, dimension);
        const auto total_embedding_num = embedding_num();
        switch (const KnnDistanceType dist_type = knn_distance_1->dist_type_; dist_type) {
            case KnnDistanceType::kInnerProduct: {
                // dot(x, v) = dot(x, c + b) + dot((x * a), n) + dot(v, e)
                const auto c_plus_b = MakeUniqueForOverwrite<f32[]>(dimension);
                const auto x_mult_a = MakeUniqueForOverwrite<f32[]>(dimension);
                for (u32 i = 0; i < dimension; ++i) {
                    c_plus_b[i] = c_ptr[i] + b_ptr[i];
                    x_mult_a[i] = x_ptr[i] * a_ptr[i];
                }
                const auto dot_x_c_b = IPDistance<f32>(x_ptr, c_plus_b.get(), dimension);
                for (u32 i = 0; i < total_embedding_num; ++i) {
                    const auto segment_offset = embedding_segment_offset(i);
                    if (!satisfy_filter_func(segment_offset)) {
                        continue;
                    }
                    f32 d = dot_x_c_b + dot_v_e_[i];
                    const u8 *sq_data = sq_data_.data() + i * embedding_sq_bytes_;
                    for (u32 j = 0; j < dimension; ++j) {
                        d += x_mult_a[j] * sq_decode(sq_data, j);
                    }
                    add_result_func(d, segment_offset);
                }
                break;
            }
            case KnnDistanceType::kCosine: {
                // dot(x, v) = dot(x, c + b) + dot((x * a), n) + dot(v, e)
                // dot(v, v) = dot(c + b, c + b) + dot(a^2, n^2) + 2 * dot ((c + b) * a, n) + 2 * dot(v, e)
                const auto x_l2 = L2NormSquare<f32>(x_ptr, dimension);
                const auto c_plus_b = MakeUniqueForOverwrite<f32[]>(dimension);
                const auto x_mult_a = MakeUniqueForOverwrite<f32[]>(dimension);
                const auto a_square = MakeUniqueForOverwrite<f32[]>(dimension);
                const auto c_plus_b_mult_a_2 = MakeUniqueForOverwrite<f32[]>(dimension);
                for (u32 i = 0; i < dimension; ++i) {
                    c_plus_b[i] = c_ptr[i] + b_ptr[i];
                    x_mult_a[i] = x_ptr[i] * a_ptr[i];
                    a_square[i] = a_ptr[i] * a_ptr[i];
                    c_plus_b_mult_a_2[i] = 2.0f * c_plus_b[i] * a_ptr[i];
                }
                const auto dot_x_c_b = IPDistance<f32>(x_ptr, c_plus_b.get(), dimension);
                const auto c_plus_b_l2 = L2NormSquare<f32>(c_plus_b.get(), dimension);
                for (u32 i = 0; i < total_embedding_num; ++i) {
                    const auto segment_offset = embedding_segment_offset(i);
                    if (!satisfy_filter_func(segment_offset)) {
                        continue;
                    }
                    f32 x_v_ip = dot_x_c_b + dot_v_e_[i];
                    f32 v_l2 = c_plus_b_l2 + 2.0f * dot_v_e_[i];
                    const u8 *sq_data = sq_data_.data() + i * embedding_sq_bytes_;
                    for (u32 j = 0; j < dimension; ++j) {
                        const auto n = sq_decode(sq_data, j);
                        x_v_ip += x_mult_a[j] * n;
                        v_l2 += (a_square[j] * n + c_plus_b_mult_a_2[j]) * n;
                    }
                    const f32 d = x_v_ip / std::sqrt(x_l2 * v_l2);
                    add_result_func(d, segment_offset);
                }
                break;
            }
            case KnnDistanceType::kL2: {
                // l2(a * n + b + c - x) = dot(a^2, n^2) + l2(b + c - x) + 2 * dot(a * (b + c - x), n)
                const auto a_square = MakeUniqueForOverwrite<f32[]>(dimension);
                const auto b_plus_c_minus_x = MakeUniqueForOverwrite<f32[]>(dimension);
                const auto a_mult_b_plus_c_minus_x_2 = MakeUniqueForOverwrite<f32[]>(dimension);
                for (u32 i = 0; i < dimension; ++i) {
                    a_square[i] = a_ptr[i] * a_ptr[i];
                    b_plus_c_minus_x[i] = b_ptr[i] + c_ptr[i] - x_ptr[i];
                    a_mult_b_plus_c_minus_x_2[i] = 2.0f * a_ptr[i] * b_plus_c_minus_x[i];
                }
                const auto b_plus_c_minus_x_l2 = L2NormSquare<f32>(b_plus_c_minus_x.get(), dimension);
                for (u32 i = 0; i < total_embedding_num; ++i) {
                    const auto segment_offset = embedding_segment_offset(i);
                    if (!satisfy_filter_func(segment_offset)) {
                        continue;
                    }
                    f32 d = b_plus_c_minus_x_l2;
                    const u8 *sq_data = sq_data_.data() + i * embedding_sq_bytes_;
                    for (u32 j = 0; j < dimension; ++j) {
                        const auto n = sq_decode(sq_data, j);
                        d += (a_square[j] * n + a_mult_b_plus_c_minus_x_2[j]) * n;
                    }
                    add_result_func(d, segment_offset);
                }
                break;
            }
            default: {
                RecoverableError(Status::SyntaxError(fmt::format("IVFSQ does not support {} metric now.", KnnExpr::KnnDistanceType2Str(dist_type))));
                break;
            }
        }
    }
};

// PQ storage
struct PQ_Code_Storage {
    const u64 subspace_num_ = 0;
    explicit PQ_Code_Storage(const u32 subspace_num) : subspace_num_(subspace_num) {}
    virtual ~PQ_Code_Storage() = default;
    virtual void Save(LocalFileHandle &file_handle) const = 0;
    virtual void Load(LocalFileHandle &file_handle) = 0;
    virtual void ExtractCodes(u32 idx, u32 *output_codes) const = 0;
    virtual void AppendCodes(const u32 *input_codes) = 0;
};

template <std::unsigned_integral T>
struct PQ_Code_StorageB : PQ_Code_Storage {
    u64 last_code_id_ = 0;
    Vector<T> storage_{};
    using PQ_Code_Storage::PQ_Code_Storage;
    void Save(LocalFileHandle &file_handle) const final {
        file_handle.Append(&last_code_id_, sizeof(last_code_id_));
        const u32 storage_size = storage_.size();
        file_handle.Append(&storage_size, sizeof(storage_size));
        file_handle.Append(storage_.data(), storage_.size() * sizeof(T));
    }
    void Load(LocalFileHandle &file_handle) final {
        file_handle.Read(&last_code_id_, sizeof(last_code_id_));
        u32 storage_size = 0;
        file_handle.Read(&storage_size, sizeof(storage_size));
        storage_.resize(storage_size);
        file_handle.Read(storage_.data(), storage_size * sizeof(T));
    }
};

template <u32 storage_bits>
struct PQ_Code_StorageT;

// 4 bits per code
template <>
struct PQ_Code_StorageT<4> final : PQ_Code_StorageB<u8> {
    using PQ_Code_StorageB<u8>::PQ_Code_StorageB;
    void ExtractCodes(const u32 idx, u32 *output_codes) const override {
        auto read_pos = idx * subspace_num_;
        for (u32 i = 0; i < subspace_num_; ++i) {
            const auto access_pos = read_pos >> 1;
            output_codes[i] = ((read_pos & 1) ? (storage_[access_pos] >> 4) : (storage_[access_pos] & 0xf));
            ++read_pos;
        }
    }
    void AppendCodes(const u32 *input_codes) override {
        storage_.resize((last_code_id_ + subspace_num_ + 1) / 2);
        auto write_pos = last_code_id_;
        for (u32 i = 0; i < subspace_num_; ++i) {
            const auto access_pos = write_pos >> 1;
            const auto input_code = input_codes[i];
            storage_[access_pos] |= (write_pos & 1) ? (input_code << 4) : input_code;
            ++write_pos;
        }
        last_code_id_ += subspace_num_;
    }
};

// 8 bits per code
template <>
struct PQ_Code_StorageT<8> final : PQ_Code_StorageB<u8> {
    using PQ_Code_StorageB<u8>::PQ_Code_StorageB;
    void ExtractCodes(const u32 idx, u32 *output_codes) const override {
        auto read_pos = idx * subspace_num_;
        for (u32 i = 0; i < subspace_num_; ++i) {
            output_codes[i] = storage_[read_pos];
            ++read_pos;
        }
    }
    void AppendCodes(const u32 *input_codes) override {
        storage_.resize(last_code_id_ + subspace_num_);
        auto write_pos = last_code_id_;
        for (u32 i = 0; i < subspace_num_; ++i) {
            storage_[write_pos] = input_codes[i];
            ++write_pos;
        }
        last_code_id_ += subspace_num_;
    }
};

// 12 bits per code
// 3 * 4 bits
template <>
struct PQ_Code_StorageT<12> final : PQ_Code_StorageB<u8> {
    using PQ_Code_StorageB<u8>::PQ_Code_StorageB;
    void ExtractCodes(const u32 idx, u32 *output_codes) const override {
        const auto half_read_pos = idx * subspace_num_ * 3;
        auto byte_pos = half_read_pos >> 1;
        bool is_odd = half_read_pos & 1;
        for (u32 i = 0; i < subspace_num_; ++i) {
            const auto pack_16 = (static_cast<u32>(storage_[byte_pos]) | (static_cast<u32>(storage_[byte_pos + 1]) << 8));
            output_codes[i] = is_odd ? (pack_16 >> 4) : (pack_16 & 0xfff);
            byte_pos += is_odd ? 2 : 1;
            is_odd = !is_odd;
        }
    }
    void AppendCodes(const u32 *input_codes) override {
        storage_.resize(((last_code_id_ + subspace_num_) * 3 + 1) / 2);
        const auto half_write_pos = last_code_id_ * 3;
        auto byte_pos = half_write_pos >> 1;
        bool is_odd = half_write_pos & 1;
        for (u32 i = 0; i < subspace_num_; ++i) {
            const auto write_16 = is_odd ? (input_codes[i] << 4) : input_codes[i];
            storage_[byte_pos] |= (write_16 & 0xff);
            storage_[byte_pos + 1] |= (write_16 >> 8);
            byte_pos += is_odd ? 2 : 1;
            is_odd = !is_odd;
        }
        last_code_id_ += subspace_num_;
    }
};

// 16 bits per code
template <>
struct PQ_Code_StorageT<16> final : PQ_Code_StorageB<u16> {
    using PQ_Code_StorageB<u16>::PQ_Code_StorageB;
    void ExtractCodes(const u32 idx, u32 *output_codes) const override {
        auto read_pos = idx * subspace_num_;
        for (u32 i = 0; i < subspace_num_; ++i) {
            output_codes[i] = storage_[read_pos];
            ++read_pos;
        }
    }
    void AppendCodes(const u32 *input_codes) override {
        storage_.resize(last_code_id_ + subspace_num_);
        auto write_pos = last_code_id_;
        for (u32 i = 0; i < subspace_num_; ++i) {
            storage_[write_pos] = input_codes[i];
            ++write_pos;
        }
        last_code_id_ += subspace_num_;
    }
};

template <typename = void>
UniquePtr<PQ_Code_Storage> GetPQCodeStorageT(const u32 subspace_num, const u32 subspace_bits) {
    UnrecoverableError(fmt::format("Invalid subspace_bits: {}, expect number no bigger than 16.", subspace_bits));
    return {};
}

template <u32 I, u32... J>
    requires(I == std::min({I, J...}))
UniquePtr<PQ_Code_Storage> GetPQCodeStorageT(const u32 subspace_num, const u32 subspace_bits) {
    if (I >= subspace_bits) {
        return MakeUnique<PQ_Code_StorageT<I>>(subspace_num);
    }
    return GetPQCodeStorageT<J...>(subspace_num, subspace_bits);
}

UniquePtr<PQ_Code_Storage> GetPQCodeStorage(const u32 subspace_num, const u32 subspace_bits) {
    return GetPQCodeStorageT<4, 8, 12, 16>(subspace_num, subspace_bits);
}

template <EmbeddingDataType src_embedding_data_type>
class IVF_Part_Storage_PQ final : public IVF_Part_Storage {
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<src_embedding_data_type>;
    static_assert(IsAnyOf<ColumnEmbeddingElementT, f64, f32, Float16T, BFloat16T>);

    const u32 subspace_num_ = 0;
    const u32 subspace_bits_ = 0;
    UniquePtr<PQ_Code_Storage> pq_code_storage_ = GetPQCodeStorage(subspace_num_, subspace_bits_);

public:
    IVF_Part_Storage_PQ(const u32 part_id, const u32 subspace_num, const u32 subspace_bits)
        : IVF_Part_Storage(part_id), subspace_num_(subspace_num), subspace_bits_(subspace_bits) {
        assert(subspace_bits_ >= 4 && subspace_bits_ <= 16);
    }

    void Save(LocalFileHandle &file_handle) const override {
        IVF_Part_Storage::Save(file_handle);
        pq_code_storage_->Save(file_handle);
    }
    void Load(LocalFileHandle &file_handle) override {
        IVF_Part_Storage::Load(file_handle);
        pq_code_storage_->Load(file_handle);
    }

    void AppendOneEmbedding(const void *embedding_ptr,
                            const SegmentOffset segment_offset,
                            const IVF_Centroids_Storage *ivf_centroids_storage,
                            const IVF_Parts_Storage *ivf_parts_storage) override {
        const auto dimension = ivf_centroids_storage->embedding_dimension();
        const auto residual = MakeUniqueForOverwrite<f32[]>(dimension);
        const auto encoded_codes = MakeUniqueForOverwrite<u32[]>(subspace_num_);
        {
            const auto [src_embedding_f32, _] = GetF32Ptr(static_cast<const ColumnEmbeddingElementT *>(embedding_ptr), dimension);
            const auto centroid_data = ivf_centroids_storage->data() + part_id() * dimension;
            for (u32 i = 0; i < dimension; ++i) {
                residual[i] = src_embedding_f32[i] - centroid_data[i];
            }
        }
        const auto *ivf_parts_storage_info =
            dynamic_cast<const IVF_Parts_Storage_Info<IndexIVFStorageOption::Type::kProductQuantization> *>(ivf_parts_storage);
        assert(ivf_parts_storage_info);
        ivf_parts_storage_info->EncodeResidual(residual.get(), encoded_codes.get());
        pq_code_storage_->AppendCodes(encoded_codes.get());
        embedding_segment_offsets_.push_back(segment_offset);
        ++embedding_num_;
    }

    void SearchIndex(const IVF_Index_Storage *ivf_index_storage,
                     const KnnDistanceBase1 *knn_distance,
                     const void *query_ptr,
                     const EmbeddingDataType query_element_type,
                     const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                     const std::function<void(f32, SegmentOffset)> &add_result_func,
                     SearchIndexPartsReuseContext &context) const override {
        auto ReturnT = [&]<EmbeddingDataType query_element_type> {
            if constexpr ((query_element_type == EmbeddingDataType::kElemFloat && IsAnyOf<ColumnEmbeddingElementT, f64, f32, Float16T, BFloat16T>) ||
                          (query_element_type == src_embedding_data_type &&
                           (query_element_type == EmbeddingDataType::kElemInt8 || query_element_type == EmbeddingDataType::kElemUInt8))) {
                return SearchIndexT<query_element_type>(ivf_index_storage,
                                                        knn_distance,
                                                        static_cast<const EmbeddingDataTypeToCppTypeT<query_element_type> *>(query_ptr),
                                                        satisfy_filter_func,
                                                        add_result_func,
                                                        context);
            } else {
                UnrecoverableError("Invalid Query EmbeddingDataType");
            }
        };
        switch (query_element_type) {
            case EmbeddingDataType::kElemFloat: {
                return ReturnT.template operator()<EmbeddingDataType::kElemFloat>();
            }
            case EmbeddingDataType::kElemUInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemUInt8>();
            }
            case EmbeddingDataType::kElemInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemInt8>();
            }
            default: {
                UnrecoverableError("Invalid EmbeddingDataType");
            }
        }
    }

    template <EmbeddingDataType query_element_type>
    void SearchIndexT(const IVF_Index_Storage *ivf_index_storage,
                      const KnnDistanceBase1 *knn_distance,
                      const EmbeddingDataTypeToCppTypeT<query_element_type> *query_ptr,
                      const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                      const std::function<void(f32, SegmentOffset)> &add_result_func,
                      SearchIndexPartsReuseContext &context) const {
        using QueryDataType = EmbeddingDataTypeToCppTypeT<query_element_type>;
        static_assert(std::is_same_v<QueryDataType, f32>);
        auto knn_distance_1 = dynamic_cast<const KnnDistance1<QueryDataType, f32> *>(knn_distance);
        if (!knn_distance_1) [[unlikely]] {
            UnrecoverableError("Invalid KnnDistance1");
        }
        const auto &ivf_parts_storage =
            static_cast<const IVF_Parts_Storage_Info<IndexIVFStorageOption::Type::kProductQuantization> &>(ivf_index_storage->ivf_parts_storage());
        const auto subspace_num = subspace_num_;
        const auto real_subspace_centroid_num = ivf_parts_storage.real_subspace_centroid_num();
        const auto dimension = ivf_index_storage->embedding_dimension();
        const auto [query_f32, _] = GetF32Ptr(query_ptr, dimension);
        const auto centroid_data = ivf_index_storage->ivf_centroids_storage().data() + part_id() * dimension;
        const auto total_embedding_num = embedding_num();
        switch (const KnnDistanceType dist_type = knn_distance_1->dist_type_; dist_type) {
            case KnnDistanceType::kInnerProduct: {
                const auto query_centroid_ip = IPDistance<f32>(query_f32, centroid_data, dimension);
                auto &ip_table = context.pq_query_ip_table_;
                if (!ip_table) {
                    ip_table = ivf_parts_storage.GetIPTable(query_f32);
                }
                const auto encoded_codes = MakeUniqueForOverwrite<u32[]>(subspace_num_);
                for (u32 i = 0; i < total_embedding_num; ++i) {
                    const auto segment_offset = embedding_segment_offset(i);
                    if (!satisfy_filter_func(segment_offset)) {
                        continue;
                    }
                    pq_code_storage_->ExtractCodes(i, encoded_codes.get());
                    f32 d = query_centroid_ip;
                    for (u32 j = 0; j < subspace_num; ++j) {
                        d += ip_table[j * real_subspace_centroid_num + encoded_codes[j]];
                    }
                    add_result_func(d, segment_offset);
                }
                break;
            }
            case KnnDistanceType::kCosine: {
                const auto query_l2 = L2NormSquare<f32>(query_f32, dimension);
                const auto centroid_l2 = L2NormSquare<f32>(centroid_data, dimension);
                const auto query_centroid_ip = IPDistance<f32>(query_f32, centroid_data, dimension);
                auto &query_ip_table = context.pq_query_ip_table_;
                if (!query_ip_table) {
                    query_ip_table = ivf_parts_storage.GetIPTable(query_f32);
                }
                const auto centroid_ip_table = ivf_parts_storage.GetIPTable(centroid_data);
                const auto encoded_codes = MakeUniqueForOverwrite<u32[]>(subspace_num_);
                for (u32 i = 0; i < total_embedding_num; ++i) {
                    const auto segment_offset = embedding_segment_offset(i);
                    if (!satisfy_filter_func(segment_offset)) {
                        continue;
                    }
                    pq_code_storage_->ExtractCodes(i, encoded_codes.get());
                    f32 ip = query_centroid_ip;
                    f32 target_l2 = centroid_l2 * 0.5f;
                    for (u32 j = 0; j < subspace_num; ++j) {
                        const auto idx = j * real_subspace_centroid_num + encoded_codes[j];
                        ip += query_ip_table[idx];
                        target_l2 += centroid_ip_table[idx] - ivf_parts_storage.subspace_centroid_norms_neg_half_at_subspace(j)[encoded_codes[j]];
                    }
                    target_l2 *= 2.0f;
                    const auto d = ip / std::sqrt(query_l2 * target_l2);
                    add_result_func(d, segment_offset);
                }
                break;
            }
            case KnnDistanceType::kL2: {
                const auto residual_query = MakeUniqueForOverwrite<f32[]>(dimension);
                for (u32 i = 0; i < dimension; ++i) {
                    residual_query[i] = query_f32[i] - centroid_data[i];
                }
                const auto residual_query_l2 = L2NormSquare<f32>(residual_query.get(), dimension);
                const auto residual_ip_table = ivf_parts_storage.GetIPTable(residual_query.get());
                const auto encoded_codes = MakeUniqueForOverwrite<u32[]>(subspace_num_);
                for (u32 i = 0; i < total_embedding_num; ++i) {
                    const auto segment_offset = embedding_segment_offset(i);
                    if (!satisfy_filter_func(segment_offset)) {
                        continue;
                    }
                    pq_code_storage_->ExtractCodes(i, encoded_codes.get());
                    f32 d = residual_query_l2 * 0.5f;
                    for (u32 j = 0; j < subspace_num; ++j) {
                        d -= residual_ip_table[j * real_subspace_centroid_num + encoded_codes[j]] +
                             ivf_parts_storage.subspace_centroid_norms_neg_half_at_subspace(j)[encoded_codes[j]];
                    }
                    d *= 2.0f;
                    add_result_func(d, segment_offset);
                }
                break;
            }
            default: {
                RecoverableError(Status::SyntaxError(fmt::format("IVFPQ does not support {} metric now.", KnnExpr::KnnDistanceType2Str(dist_type))));
                break;
            }
        }
    }
};

UniquePtr<IVF_Part_Storage> IVF_Part_Storage::Make(const u32 part_id,
                                                   const u32 embedding_dimension,
                                                   const EmbeddingDataType embedding_data_type,
                                                   const IndexIVFStorageOption &ivf_storage_option) {
    switch (ivf_storage_option.type_) {
        case IndexIVFStorageOption::Type::kPlain: {
            auto GetPlainResult =
                [part_id, embedding_dimension, embedding_data_type]<EmbeddingDataType plain_data_type>() -> UniquePtr<IVF_Part_Storage> {
                return ApplyEmbeddingDataTypeToFunc(
                    embedding_data_type,
                    [part_id, embedding_dimension]<EmbeddingDataType src_embedding_data_type>() -> UniquePtr<IVF_Part_Storage> {
                        auto not_i8_u8 = [](const EmbeddingDataType t) consteval {
                            return t != EmbeddingDataType::kElemInt8 && t != EmbeddingDataType::kElemUInt8;
                        };
                        if constexpr (plain_data_type == src_embedding_data_type ||
                                      (not_i8_u8(plain_data_type) && not_i8_u8(src_embedding_data_type))) {
                            return MakeUnique<IVF_Part_Storage_Plain<plain_data_type, src_embedding_data_type>>(part_id, embedding_dimension);
                        } else {
                            return nullptr;
                        }
                    },
                    [] { return UniquePtr<IVF_Part_Storage>(); });
            };
            switch (ivf_storage_option.plain_storage_data_type_) {
                case EmbeddingDataType::kElemInt8: {
                    return GetPlainResult.template operator()<EmbeddingDataType::kElemInt8>();
                }
                case EmbeddingDataType::kElemUInt8: {
                    return GetPlainResult.template operator()<EmbeddingDataType::kElemUInt8>();
                }
                case EmbeddingDataType::kElemFloat: {
                    return GetPlainResult.template operator()<EmbeddingDataType::kElemFloat>();
                }
                case EmbeddingDataType::kElemFloat16: {
                    return GetPlainResult.template operator()<EmbeddingDataType::kElemFloat16>();
                }
                case EmbeddingDataType::kElemBFloat16: {
                    return GetPlainResult.template operator()<EmbeddingDataType::kElemBFloat16>();
                }
                case EmbeddingDataType::kElemDouble:
                case EmbeddingDataType::kElemBit:
                case EmbeddingDataType::kElemInt16:
                case EmbeddingDataType::kElemInt32:
                case EmbeddingDataType::kElemInt64:
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid IVF plain_data_type");
                    break;
                }
            }
            break;
        }
        case IndexIVFStorageOption::Type::kScalarQuantization: {
            const auto sq_bits = ivf_storage_option.scalar_quantization_bits_;
            auto GetSQResult = [part_id, embedding_dimension, sq_bits]<EmbeddingDataType src_embedding_data_type>() -> UniquePtr<IVF_Part_Storage> {
                switch (sq_bits) {
                    case 4: {
                        return MakeUnique<IVF_Part_Storage_SQ<4, src_embedding_data_type>>(part_id, embedding_dimension);
                    }
                    case 8: {
                        return MakeUnique<IVF_Part_Storage_SQ<8, src_embedding_data_type>>(part_id, embedding_dimension);
                    }
                    default: {
                        UnrecoverableError(fmt::format("Invalid scalar quantization bits: {}", sq_bits));
                        return {};
                    }
                }
            };
            switch (embedding_data_type) {
                case EmbeddingDataType::kElemDouble: {
                    return GetSQResult.template operator()<EmbeddingDataType::kElemDouble>();
                }
                case EmbeddingDataType::kElemFloat: {
                    return GetSQResult.template operator()<EmbeddingDataType::kElemFloat>();
                }
                case EmbeddingDataType::kElemFloat16: {
                    return GetSQResult.template operator()<EmbeddingDataType::kElemFloat16>();
                }
                case EmbeddingDataType::kElemBFloat16: {
                    return GetSQResult.template operator()<EmbeddingDataType::kElemBFloat16>();
                }
                default: {
                    UnrecoverableError("Unsupported embedding data type for IVFSQ.");
                    return {};
                }
            }
            break;
        }
        case IndexIVFStorageOption::Type::kProductQuantization: {
            const auto subspace_num = ivf_storage_option.product_quantization_subspace_num_;
            const auto subspace_bits = ivf_storage_option.product_quantization_subspace_bits_;
            auto GetPQResult = [part_id, subspace_num, subspace_bits]<EmbeddingDataType src_embedding_data_type>() {
                return MakeUnique<IVF_Part_Storage_PQ<src_embedding_data_type>>(part_id, subspace_num, subspace_bits);
            };
            switch (embedding_data_type) {
                case EmbeddingDataType::kElemDouble: {
                    return GetPQResult.template operator()<EmbeddingDataType::kElemDouble>();
                }
                case EmbeddingDataType::kElemFloat: {
                    return GetPQResult.template operator()<EmbeddingDataType::kElemFloat>();
                }
                case EmbeddingDataType::kElemFloat16: {
                    return GetPQResult.template operator()<EmbeddingDataType::kElemFloat16>();
                }
                case EmbeddingDataType::kElemBFloat16: {
                    return GetPQResult.template operator()<EmbeddingDataType::kElemBFloat16>();
                }
                default: {
                    UnrecoverableError("Unsupported embedding data type for IVFPQ.");
                    return {};
                }
            }
            break;
        }
    }
    return {};
}

} // namespace infinity
