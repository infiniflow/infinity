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

#include <algorithm>
#include <cassert>
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
import knn_expr;
import vector_distance;
import mlas_matrix_multiply;

namespace infinity {

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

Pair<u32, const f32 *> IVF_Centroids_Storage::GetCentroidDataForMetric(const KnnDistanceBase1 *knn_distance) const {
    switch (knn_distance->dist_type_) {
        case KnnDistanceType::kInnerProduct:
        case KnnDistanceType::kL2: {
            return {centroids_num_, data()};
        }
        case KnnDistanceType::kCosine: {
            if (normalized_centroids_data_cache_.empty()) {
                normalized_centroids_data_cache_ = centroids_data_;
                assert(normalized_centroids_data_cache_.size() == embedding_dimension_ * centroids_num_);
                for (u32 i = 0; i < centroids_num_; ++i) {
                    f32 *centroid_data = normalized_centroids_data_cache_.data() + i * embedding_dimension_;
                    const f32 l2_rev = 1.0f / std::sqrt(L2NormSquare<f32>(centroid_data, embedding_dimension_));
                    for (u32 j = 0; j < embedding_dimension_; ++j) {
                        centroid_data[j] *= l2_rev;
                    }
                }
            }
            return {centroids_num_, normalized_centroids_data_cache_.data()};
        }
        default: {
            RecoverableError(
                Status::NotSupport(fmt::format("IVF does not support {} metric now.", KnnExpr::KnnDistanceType2Str(knn_distance->dist_type_))));
        }
    }
    return {};
}

// IVF_Index_Storage

void IVF_Index_Storage::Save(LocalFileHandle &file_handle) const {
    file_handle.Append(&row_count_, sizeof(row_count_));
    file_handle.Append(&embedding_count_, sizeof(embedding_count_));
    ivf_centroids_storage_.Save(file_handle);
    assert(ivf_centroids_storage_.centroids_num() == ivf_parts_storage_->centroids_num());
    ivf_parts_storage_->Save(file_handle);
}

void IVF_Index_Storage::Load(LocalFileHandle &file_handle) {
    file_handle.Read(&row_count_, sizeof(row_count_));
    file_handle.Read(&embedding_count_, sizeof(embedding_count_));
    ivf_centroids_storage_.Load(file_handle);
    ivf_parts_storage_ =
        IVF_Parts_Storage::Make(embedding_dimension_, ivf_centroids_storage_.centroids_num(), embedding_data_type_, ivf_option_.storage_option_);
    ivf_parts_storage_->Load(file_handle);
}

void IVF_Index_Storage::GetMemData(IVF_Index_Storage &&mem_data) {
    assert(ivf_option() == mem_data.ivf_option());
    assert(column_logical_type() == mem_data.column_logical_type());
    assert(embedding_data_type() == mem_data.embedding_data_type());
    assert(embedding_dimension() == mem_data.embedding_dimension());
    row_count_ = mem_data.row_count_;
    embedding_count_ = mem_data.embedding_count_;
    ivf_centroids_storage_ = std::move(mem_data.ivf_centroids_storage_);
    ivf_parts_storage_ = std::move(mem_data.ivf_parts_storage_);
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
    ivf_parts_storage_ =
        IVF_Parts_Storage::Make(embedding_dimension_, ivf_centroids_storage_.centroids_num(), embedding_data_type_, ivf_option_.storage_option_);
    ivf_parts_storage_->Train(training_embedding_num, training_data, &ivf_centroids_storage_);
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
    assert(ivf_centroids_storage_.centroids_num() == ivf_parts_storage_->centroids_num());
    const auto [embedding_f32_ptr, _] = GetF32Ptr(embedding_ptr, embedding_num * embedding_dimension_);
    auto part_ids = Vector<u32>(embedding_num, std::numeric_limits<u32>::max());
    search_top_1_without_dis<f32>(embedding_dimension_,
                                  embedding_num,
                                  embedding_f32_ptr,
                                  ivf_centroids_storage_.centroids_num(),
                                  ivf_centroids_storage_.data(),
                                  part_ids.data());
    for (u32 i = 0; i < embedding_num; ++i) {
        ivf_parts_storage_->AppendOneEmbedding(part_ids[i],
                                               embedding_ptr + i * embedding_dimension_,
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
    assert(ivf_centroids_storage_.centroids_num() == ivf_parts_storage_->centroids_num());
    const auto [embedding_f32_ptr, _] = GetF32Ptr(embedding_ptr, embedding_num * embedding_dimension_);
    auto part_ids = Vector<u32>(embedding_num, std::numeric_limits<u32>::max());
    search_top_1_without_dis<f32>(embedding_dimension_,
                                  embedding_num,
                                  embedding_f32_ptr,
                                  ivf_centroids_storage_.centroids_num(),
                                  ivf_centroids_storage_.data(),
                                  part_ids.data());
    for (u32 i = 0; i < embedding_num; ++i) {
        ivf_parts_storage_->AppendOneEmbedding(part_ids[i], embedding_ptr + i * embedding_dimension_, segment_offset_ptr[i], &ivf_centroids_storage_);
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
    assert(ivf_centroids_storage_.centroids_num() == ivf_parts_storage_->centroids_num());
    const auto [embedding_f32_ptr, _] = GetF32Ptr(multi_vector_ptr, embedding_num * embedding_dimension_);
    auto part_ids = Vector<u32>(embedding_num, std::numeric_limits<u32>::max());
    search_top_1_without_dis<f32>(embedding_dimension_,
                                  embedding_num,
                                  embedding_f32_ptr,
                                  ivf_centroids_storage_.centroids_num(),
                                  ivf_centroids_storage_.data(),
                                  part_ids.data());
    for (u32 i = 0; i < embedding_num; ++i) {
        ivf_parts_storage_->AppendOneEmbedding(part_ids[i], multi_vector_ptr + i * embedding_dimension_, segment_offset, &ivf_centroids_storage_);
    }
    embedding_count_ += embedding_num;
    ++row_count_;
}

void IVF_Index_Storage::SearchIndex(const KnnDistanceBase1 *knn_distance,
                                    const void *query_ptr,
                                    const EmbeddingDataType query_element_type,
                                    u32 nprobe,
                                    const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                                    const std::function<void(f32, SegmentOffset)> &add_result_func) const {
    const auto dimension = embedding_dimension();
    const auto [centroids_num, centroids_data] = ivf_centroids_storage_.GetCentroidDataForMetric(knn_distance);
    nprobe = std::min<u32>(nprobe, centroids_num);
    auto [query_f32_ptr, _] = ApplyEmbeddingDataTypeToFunc(
        query_element_type,
        [query_ptr, dimension]<EmbeddingDataType query_element_type> {
            return GetF32Ptr(static_cast<const EmbeddingDataTypeToCppTypeT<query_element_type> *>(query_ptr), dimension);
        },
        [] { return Pair<const f32 *, UniquePtr<f32[]>>(); });
    Vector<u32> nprobe_result;
    switch (knn_distance->dist_type_) {
        case KnnDistanceType::kL2: {
            nprobe_result.resize(nprobe);
            if (nprobe == 1) {
                search_top_1_without_dis<f32>(dimension, 1, query_f32_ptr, centroids_num, centroids_data, nprobe_result.data());
            } else {
                const auto centroid_dists = MakeUniqueForOverwrite<f32[]>(nprobe);
                search_top_k_with_dis(nprobe,
                                      dimension,
                                      1,
                                      query_f32_ptr,
                                      centroids_num,
                                      centroids_data,
                                      nprobe_result.data(),
                                      centroid_dists.get(),
                                      false);
            }
            break;
        }
        case KnnDistanceType::kCosine:
        case KnnDistanceType::kInnerProduct: {
            const auto ip_result = MakeUniqueForOverwrite<f32[]>(centroids_num);
            matrixA_multiply_matrixB_output_to_C(centroids_data, query_f32_ptr, centroids_num, 1, dimension, ip_result.get());
            nprobe_result.resize(centroids_num);
            std::iota(nprobe_result.begin(), nprobe_result.end(), static_cast<u32>(0));
            std::nth_element(nprobe_result.begin(), nprobe_result.begin() + nprobe, nprobe_result.end(), [&ip_result](const u32 a, const u32 b) {
                return ip_result[a] > ip_result[b];
            });
            nprobe_result.resize(nprobe);
            break;
        }
        default: {
            UnrecoverableError("Unsupported distance type");
        }
    }
    ivf_parts_storage_->SearchIndex(nprobe_result, this, knn_distance, query_ptr, query_element_type, satisfy_filter_func, add_result_func);
}

} // namespace infinity
