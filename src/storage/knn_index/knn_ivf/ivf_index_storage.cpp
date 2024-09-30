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
#include <vector>
module ivf_index_storage;

import stl;
import infinity_exception;
import status;
import logger;
import third_party;
import index_ivf;
import file_system;
import column_vector;
import internal_types;
import logical_type;
import data_type;
import kmeans_partition;
import search_top_1;
import search_top_k;
import column_vector;

namespace infinity {

inline auto ApplyEmbeddingDataTypeToFunc(const EmbeddingDataType embedding_data_type, auto func, auto default_f) {
    switch (embedding_data_type) {
        case EmbeddingDataType::kElemUInt8: {
            return func.template operator()<EmbeddingDataType::kElemUInt8>();
        }
        case EmbeddingDataType::kElemInt8: {
            return func.template operator()<EmbeddingDataType::kElemInt8>();
        }
        case EmbeddingDataType::kElemDouble: {
            return func.template operator()<EmbeddingDataType::kElemDouble>();
        }
        case EmbeddingDataType::kElemFloat: {
            return func.template operator()<EmbeddingDataType::kElemFloat>();
        }
        case EmbeddingDataType::kElemFloat16: {
            return func.template operator()<EmbeddingDataType::kElemFloat16>();
        }
        case EmbeddingDataType::kElemBFloat16: {
            return func.template operator()<EmbeddingDataType::kElemBFloat16>();
        }
        default: {
            UnrecoverableError("Unsupported embedding data type");
            return default_f();
        }
    }
}

// IVF_Centroids_Storage

IVF_Centroids_Storage::IVF_Centroids_Storage(const u32 embedding_dimension, const u32 centroids_num, Vector<f32> &&centroids_data)
    : embedding_dimension_(embedding_dimension), centroids_num_(centroids_num), centroids_data_(std::move(centroids_data)) {
    assert(centroids_data_.size() == embedding_dimension_ * centroids_num_);
}

void IVF_Centroids_Storage::Save(LocalFileHandle &file_handle) const {
    file_handle.Append(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handle.Append(&centroids_num_, sizeof(centroids_num_));
    const auto data_bytes = centroids_num_ * embedding_dimension_ * sizeof(f32);
    file_handle.Append(centroids_data_.data(), data_bytes);
}

void IVF_Centroids_Storage::Load(LocalFileHandle &file_handle) {
    file_handle.Read(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handle.Read(&centroids_num_, sizeof(centroids_num_));
    const auto vec_size = centroids_num_ * embedding_dimension_;
    centroids_data_.resize(vec_size);
    file_handle.Read(centroids_data_.data(), vec_size * sizeof(f32));
}

// IVF_Part_Storage

void IVF_Part_Storage::Save(LocalFileHandle &file_handle) const {
    file_handle.Append(&part_id_, sizeof(part_id_));
    file_handle.Append(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handle.Append(&embedding_num_, sizeof(embedding_num_));
    static_assert(std::is_same_v<SegmentOffset, typename decltype(embedding_segment_offsets_)::value_type>);
    assert(embedding_num_ == embedding_segment_offsets_.size());
    file_handle.Append(embedding_segment_offsets_.data(), embedding_num_ * sizeof(SegmentOffset));
}

void IVF_Part_Storage::Load(LocalFileHandle &file_handle) {
    file_handle.Read(&part_id_, sizeof(part_id_));
    file_handle.Read(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handle.Read(&embedding_num_, sizeof(embedding_num_));
    embedding_segment_offsets_.resize(embedding_num_);
    file_handle.Read(embedding_segment_offsets_.data(), embedding_num_ * sizeof(SegmentOffset));
}

template <EmbeddingDataType plain_data_type, EmbeddingDataType src_embedding_data_type>
class IVF_Part_Storage_Plain final : public IVF_Part_Storage {
    using StorageDataT = EmbeddingDataTypeToCppTypeT<plain_data_type>;
    static_assert(IsAnyOf<StorageDataT, i8, u8, f32, Float16T, BFloat16T>);
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<src_embedding_data_type>;
    static_assert(IsAnyOf<ColumnEmbeddingElementT, i8, u8, f64, f32, Float16T, BFloat16T>);
    static_assert(std::is_same_v<StorageDataT, ColumnEmbeddingElementT> ||
                  (!IsAnyOf<StorageDataT, i8, u8> && !IsAnyOf<ColumnEmbeddingElementT, i8, u8>));

    Vector<StorageDataT> data_{};

public:
    IVF_Part_Storage_Plain(const u32 part_id, const u32 embedding_dimension) : IVF_Part_Storage(part_id, embedding_dimension) {}

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

    void AppendOneEmbedding(const void *embedding_ptr, const SegmentOffset segment_offset, const IVF_Centroids_Storage *) override {
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

    void SearchIndex(KnnDistanceType knn_distance_type,
                     const void *query_ptr,
                     EmbeddingDataType query_element_type,
                     std::function<void(f32, SegmentOffset)> add_result_func) const override {
        // TODO
    }

    // only for unit-test, return f32 / i8 / u8 embedding data
    Pair<const void *, SharedPtr<void>> GetDataForTest(const u32 embedding_id) const override {
        if constexpr (IsAnyOf<StorageDataT, i8, u8, f32>) {
            return {data_.data() + embedding_id * embedding_dimension(), nullptr};
        } else if constexpr (IsAnyOf<StorageDataT, Float16T, BFloat16T>) {
            auto tmp_data = MakeUniqueForOverwrite<f32[]>(embedding_dimension());
            const auto *start_ptr = data_.data() + embedding_id * embedding_dimension();
            for (u32 i = 0; i < embedding_dimension(); ++i) {
                tmp_data[i] = static_cast<f32>(start_ptr[i]);
            }
            Pair<const void *, SharedPtr<void>> result(tmp_data.get(),
                                                       SharedPtr<void>(tmp_data.get(), [](void *ptr) { delete[] static_cast<f32 *>(ptr); }));
            tmp_data.release();
            return result;
        } else {
            static_assert(false, "unexpected type");
            return {};
        }
    }
};

UniquePtr<IVF_Part_Storage>
IVF_Part_Storage::Make(u32 part_id, u32 embedding_dimension, EmbeddingDataType embedding_data_type, const IndexIVFStorageOption &ivf_storage_option) {
    switch (ivf_storage_option.type_) {
        case IndexIVFStorageOption::Type::kPlain: {
            auto GetPlainResult =
                [part_id, embedding_dimension, embedding_data_type]<EmbeddingDataType plain_data_type>() -> UniquePtr<IVF_Part_Storage> {
                return ApplyEmbeddingDataTypeToFunc(
                    embedding_data_type,
                    [part_id, embedding_dimension]<EmbeddingDataType src_embedding_data_type>() -> UniquePtr<IVF_Part_Storage> {
                        auto not_i8_u8 = [](EmbeddingDataType t) consteval {
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
                    return {};
                }
            }
        }
        case IndexIVFStorageOption::Type::kScalarQuantization: {
            UnrecoverableError("Not implemented");
            return {};
        }
        case IndexIVFStorageOption::Type::kProductQuantization: {
            UnrecoverableError("Not implemented");
            return {};
        }
    }
}

// IVF_Index_Storage

void IVF_Index_Storage::Save(LocalFileHandle &file_handle) const {
    file_handle.Append(&row_count_, sizeof(row_count_));
    file_handle.Append(&embedding_count_, sizeof(embedding_count_));
    ivf_centroids_storage_.Save(file_handle);
    assert(ivf_centroids_storage_.centroids_num() == ivf_part_storages_.size());
    for (u32 part_id = 0; part_id < ivf_centroids_storage_.centroids_num(); ++part_id) {
        ivf_part_storages_[part_id]->Save(file_handle);
    }
}

void IVF_Index_Storage::Load(LocalFileHandle &file_handle) {
    file_handle.Read(&row_count_, sizeof(row_count_));
    file_handle.Read(&embedding_count_, sizeof(embedding_count_));
    ivf_centroids_storage_.Load(file_handle);
    ivf_part_storages_.resize(ivf_centroids_storage_.centroids_num());
    for (u32 part_id = 0; part_id < ivf_centroids_storage_.centroids_num(); ++part_id) {
        ivf_part_storages_[part_id] = IVF_Part_Storage::Make(part_id, embedding_dimension_, embedding_data_type_, ivf_option_.storage_option_);
        ivf_part_storages_[part_id]->Load(file_handle);
    }
}

void IVF_Index_Storage::GetMemData(IVF_Index_Storage &&mem_data) {
    assert(ivf_option() == mem_data.ivf_option());
    assert(column_logical_type() == mem_data.column_logical_type());
    assert(embedding_data_type() == mem_data.embedding_data_type());
    assert(embedding_dimension() == mem_data.embedding_dimension());
    row_count_ = mem_data.row_count_;
    embedding_count_ = mem_data.embedding_count_;
    ivf_centroids_storage_ = std::move(mem_data.ivf_centroids_storage_);
    ivf_part_storages_ = std::move(mem_data.ivf_part_storages_);
}

IVF_Index_Storage::IVF_Index_Storage(const IndexIVFOption &ivf_option,
                                     const LogicalType column_logical_type,
                                     const EmbeddingDataType embedding_data_type,
                                     const u32 embedding_dimension)
    : ivf_option_(ivf_option), column_logical_type_(column_logical_type), embedding_data_type_(embedding_data_type),
      embedding_dimension_(embedding_dimension) {}

void IVF_Index_Storage::Train(const u32 training_embedding_num, const f32 *training_data, const u32 expect_centroid_num) {
    Vector<f32> output_centroids;
    const auto partition_num = GetKMeansCentroids(ivf_option_.metric_,
                                                  embedding_dimension_,
                                                  training_embedding_num,
                                                  training_data,
                                                  output_centroids,
                                                  expect_centroid_num,
                                                  0,
                                                  ivf_option_.centroid_option_.min_points_per_centroid_,
                                                  ivf_option_.centroid_option_.max_points_per_centroid_,
                                                  ivf_option_.centroid_option_.centroids_num_ratio_);
    assert(expect_centroid_num == 0 || expect_centroid_num == partition_num);
    assert(output_centroids.size() == partition_num * embedding_dimension_);
    ivf_centroids_storage_ = IVF_Centroids_Storage(embedding_dimension_, partition_num, std::move(output_centroids));
    assert(ivf_centroids_storage_.centroids_num() == partition_num);
    assert(ivf_centroids_storage_.embedding_dimension() == embedding_dimension_);
    ivf_part_storages_.resize(partition_num);
    for (u32 part_id = 0; part_id < partition_num; ++part_id) {
        ivf_part_storages_[part_id] = IVF_Part_Storage::Make(part_id, embedding_dimension_, embedding_data_type_, ivf_option_.storage_option_);
    }
}

void IVF_Index_Storage::AddEmbedding(const SegmentOffset segment_offset, const void *embedding_ptr) {
    if (column_logical_type_ != LogicalType::kEmbedding) [[unlikely]] {
        UnrecoverableError("Column is not embedding type");
    }
    return ApplyEmbeddingDataTypeToFunc(
        embedding_data_type_,
        [segment_offset, embedding_ptr, this]<EmbeddingDataType embedding_data_type> {
            return AddEmbeddingT<embedding_data_type>(segment_offset,
                                                      static_cast<const EmbeddingDataTypeToCppTypeT<embedding_data_type> *>(embedding_ptr));
        },
        [] {});
}

void IVF_Index_Storage::AddEmbeddingBatch(const SegmentOffset start_segment_offset, const void *embedding_ptr, const u32 embedding_num) {
    if (column_logical_type_ != LogicalType::kEmbedding) [[unlikely]] {
        UnrecoverableError("Column is not embedding type");
    }
    return ApplyEmbeddingDataTypeToFunc(
        embedding_data_type_,
        [start_segment_offset, embedding_ptr, embedding_num, this]<EmbeddingDataType embedding_data_type> {
            return AddEmbeddingBatchT<embedding_data_type>(start_segment_offset,
                                                           static_cast<const EmbeddingDataTypeToCppTypeT<embedding_data_type> *>(embedding_ptr),
                                                           embedding_num);
        },
        [] {});
}

void IVF_Index_Storage::AddEmbeddingBatch(const SegmentOffset *segment_offset_ptr, const void *embedding_ptr, const u32 embedding_num) {
    if (column_logical_type_ != LogicalType::kEmbedding) [[unlikely]] {
        UnrecoverableError("Column is not embedding type");
    }
    return ApplyEmbeddingDataTypeToFunc(
        embedding_data_type_,
        [segment_offset_ptr, embedding_ptr, embedding_num, this]<EmbeddingDataType embedding_data_type> {
            return AddEmbeddingBatchT<embedding_data_type>(segment_offset_ptr,
                                                           static_cast<const EmbeddingDataTypeToCppTypeT<embedding_data_type> *>(embedding_ptr),
                                                           embedding_num);
        },
        [] {});
}

void IVF_Index_Storage::AddMultiVector(const SegmentOffset segment_offset, const void *multi_vector_ptr, const u32 embedding_num) {
    if (column_logical_type_ != LogicalType::kMultiVector) [[unlikely]] {
        UnrecoverableError("Column is not multi-vector type");
    }
    return ApplyEmbeddingDataTypeToFunc(
        embedding_data_type_,
        [segment_offset, multi_vector_ptr, embedding_num, this]<EmbeddingDataType embedding_data_type> {
            return AddMultiVectorT<embedding_data_type>(segment_offset,
                                                        static_cast<const EmbeddingDataTypeToCppTypeT<embedding_data_type> *>(multi_vector_ptr),
                                                        embedding_num);
        },
        [] {});
}

template <IsAnyOf<u8, i8, f64, f32, Float16T, BFloat16T> ColumnEmbeddingElementT>
Pair<const f32 *, UniquePtr<f32[]>> GetF32Ptr(const ColumnEmbeddingElementT *src_data_ptr, const u32 src_data_cnt) {
    Pair<const f32 *, UniquePtr<f32[]>> dst_data_ptr;
    if constexpr (std::is_same_v<f32, ColumnEmbeddingElementT>) {
        dst_data_ptr.first = src_data_ptr;
    } else {
        dst_data_ptr.second = MakeUniqueForOverwrite<f32[]>(src_data_cnt);
        dst_data_ptr.first = dst_data_ptr.second.get();
        for (u32 i = 0; i < src_data_cnt; ++i) {
            if constexpr (std::is_same_v<f64, ColumnEmbeddingElementT>) {
                dst_data_ptr.second[i] = static_cast<f32>(src_data_ptr[i]);
            } else {
                dst_data_ptr.second[i] = src_data_ptr[i];
            }
        }
    }
    return dst_data_ptr;
}

template <EmbeddingDataType embedding_data_type>
void IVF_Index_Storage::AddEmbeddingT(const SegmentOffset segment_offset, const EmbeddingDataTypeToCppTypeT<embedding_data_type> *embedding_ptr) {
    return AddEmbeddingBatchT<embedding_data_type>(segment_offset, embedding_ptr, 1);
}

template <EmbeddingDataType embedding_data_type>
void IVF_Index_Storage::AddEmbeddingBatchT(const SegmentOffset start_segment_offset,
                                           const EmbeddingDataTypeToCppTypeT<embedding_data_type> *embedding_ptr,
                                           const u32 embedding_num) {
    assert(embedding_data_type == embedding_data_type_);
    assert(ivf_centroids_storage_.embedding_dimension() == embedding_dimension_);
    assert(ivf_centroids_storage_.centroids_num() == ivf_part_storages_.size());
    const auto [embedding_f32_ptr, _] = GetF32Ptr(embedding_ptr, embedding_num * embedding_dimension_);
    auto part_ids = Vector<u32>(embedding_num, std::numeric_limits<u32>::max());
    search_top_1_without_dis<f32>(embedding_dimension_,
                                  embedding_num,
                                  embedding_f32_ptr,
                                  ivf_centroids_storage_.centroids_num(),
                                  ivf_centroids_storage_.data(),
                                  part_ids.data());
    for (u32 i = 0; i < embedding_num; ++i) {
        ivf_part_storages_[part_ids[i]]->AppendOneEmbedding(embedding_ptr + i * embedding_dimension_,
                                                            start_segment_offset + i,
                                                            &ivf_centroids_storage_);
    }
    embedding_count_ += embedding_num;
    row_count_ += embedding_num;
}

template <EmbeddingDataType embedding_data_type>
void IVF_Index_Storage::AddEmbeddingBatchT(const SegmentOffset *segment_offset_ptr,
                                           const EmbeddingDataTypeToCppTypeT<embedding_data_type> *embedding_ptr,
                                           const u32 embedding_num) {
    assert(embedding_data_type == embedding_data_type_);
    assert(ivf_centroids_storage_.embedding_dimension() == embedding_dimension_);
    assert(ivf_centroids_storage_.centroids_num() == ivf_part_storages_.size());
    const auto [embedding_f32_ptr, _] = GetF32Ptr(embedding_ptr, embedding_num * embedding_dimension_);
    auto part_ids = Vector<u32>(embedding_num, std::numeric_limits<u32>::max());
    search_top_1_without_dis<f32>(embedding_dimension_,
                                  embedding_num,
                                  embedding_f32_ptr,
                                  ivf_centroids_storage_.centroids_num(),
                                  ivf_centroids_storage_.data(),
                                  part_ids.data());
    for (u32 i = 0; i < embedding_num; ++i) {
        ivf_part_storages_[part_ids[i]]->AppendOneEmbedding(embedding_ptr + i * embedding_dimension_, segment_offset_ptr[i], &ivf_centroids_storage_);
    }
    embedding_count_ += embedding_num;
    row_count_ += embedding_num;
}

template <EmbeddingDataType embedding_data_type>
void IVF_Index_Storage::AddMultiVectorT(const SegmentOffset segment_offset,
                                        const EmbeddingDataTypeToCppTypeT<embedding_data_type> *multi_vector_ptr,
                                        const u32 embedding_num) {
    assert(embedding_data_type == embedding_data_type_);
    assert(ivf_centroids_storage_.embedding_dimension() == embedding_dimension_);
    assert(ivf_centroids_storage_.centroids_num() == ivf_part_storages_.size());
    const auto [embedding_f32_ptr, _] = GetF32Ptr(multi_vector_ptr, embedding_num * embedding_dimension_);
    auto part_ids = Vector<u32>(embedding_num, std::numeric_limits<u32>::max());
    search_top_1_without_dis<f32>(embedding_dimension_,
                                  embedding_num,
                                  embedding_f32_ptr,
                                  ivf_centroids_storage_.centroids_num(),
                                  ivf_centroids_storage_.data(),
                                  part_ids.data());
    for (u32 i = 0; i < embedding_num; ++i) {
        ivf_part_storages_[part_ids[i]]->AppendOneEmbedding(multi_vector_ptr + i * embedding_dimension_, segment_offset, &ivf_centroids_storage_);
    }
    embedding_count_ += embedding_num;
    ++row_count_;
}

void IVF_Index_Storage::SearchIndex(const KnnDistanceType knn_distance_type,
                                    const void *query_ptr,
                                    const EmbeddingDataType query_element_type,
                                    u32 nprobe,
                                    std::function<void(f32, SegmentOffset)> add_result_func) const {
    const auto dimension = embedding_dimension();
    const auto centroids_num = ivf_centroids_storage_.centroids_num();
    const auto *centroids_data = ivf_centroids_storage_.data();
    nprobe = std::min<u32>(nprobe, centroids_num);
    auto [query_f32_ptr, _] = ApplyEmbeddingDataTypeToFunc(
        query_element_type,
        [query_ptr, dimension]<EmbeddingDataType query_element_type> {
            return GetF32Ptr(static_cast<const EmbeddingDataTypeToCppTypeT<query_element_type> *>(query_ptr), dimension);
        },
        [] { return Pair<const f32 *, UniquePtr<f32[]>>(); });
    Vector<u32> nprobe_result(nprobe);
    if (nprobe == 1) {
        search_top_1_without_dis<f32>(dimension, 1, query_f32_ptr, centroids_num, centroids_data, nprobe_result.data());
    } else {
        const auto centroid_dists = MakeUniqueForOverwrite<f32[]>(nprobe);
        search_top_k_with_dis(nprobe, dimension, 1, query_f32_ptr, centroids_num, centroids_data, nprobe_result.data(), centroid_dists.get(), false);
    }
    for (const auto part_id : nprobe_result) {
        ivf_part_storages_[part_id]->SearchIndex(knn_distance_type, query_ptr, query_element_type, add_result_func);
    }
}

} // namespace infinity
