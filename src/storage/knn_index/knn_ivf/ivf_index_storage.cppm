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

export module infinity_core:ivf_index_storage;

import :stl;
import :index_ivf;
import internal_types;
import logical_type;
import data_type;
import :ivf_index_util_func;
import :infinity_exception;

namespace infinity {

class LocalFileHandle;
class KnnDistanceBase1;

export class IVF_Index_Storage;

// always use float for centroids
class IVF_Centroids_Storage {
    u32 embedding_dimension_ = 0;
    u32 centroids_num_ = 0;
    Vector<f32> centroids_data_ = {};
    mutable Vector<f32> normalized_centroids_data_cache_ = {};

public:
    IVF_Centroids_Storage() = default;
    IVF_Centroids_Storage(u32 embedding_dimension, u32 centroids_num, Vector<f32> &&centroids_data);
    const f32 *data() const { return centroids_data_.data(); }
    u32 embedding_dimension() const { return embedding_dimension_; }
    u32 centroids_num() const { return centroids_num_; }
    void Save(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);
    Pair<u32, const f32 *> GetCentroidDataForMetric(const KnnDistanceBase1 *knn_distance) const;
    inline SizeT MemoryUsed() const { return sizeof(u32) * 2 + sizeof(f32) * (centroids_data_.size() + normalized_centroids_data_cache_.size()); }
};

class IVF_Parts_Storage {
    const u32 embedding_dimension_ = 0;
    const u32 centroids_num_ = 0;
    SizeT memory_used_ = 0;

protected:
    void IncreaseMemoryUsage(SizeT mem_usage) { memory_used_ += mem_usage; }
    void DecMemUsed(SizeT mem_decreased) { memory_used_ -= mem_decreased; }
    SizeT row_memory_cost_ = 0;
    explicit IVF_Parts_Storage(const u32 embedding_dimension, const u32 centroids_num)
        : embedding_dimension_(embedding_dimension), centroids_num_(centroids_num) {}
    virtual void
    AppendOneEmbedding(u32 part_id, const void *embedding_ptr, SegmentOffset segment_offset, const IVF_Centroids_Storage *ivf_centroids_storage) = 0;

public:
    virtual ~IVF_Parts_Storage() = default;
    static UniquePtr<IVF_Parts_Storage>
    Make(u32 embedding_dimension, u32 centroids_num, EmbeddingDataType embedding_data_type, const IndexIVFStorageOption &ivf_storage_option);
    [[nodiscard]] u32 embedding_dimension() const { return embedding_dimension_; }
    [[nodiscard]] u32 centroids_num() const { return centroids_num_; }

    virtual void Save(LocalFileHandle &file_handle) const = 0;
    virtual void Load(LocalFileHandle &file_handle) = 0;

    virtual void Train(u32 training_embedding_num, const f32 *training_data, const IVF_Centroids_Storage *ivf_centroids_storage) = 0;
    void AppendOneEmbeddingWithStat(u32 part_id,
                                    const void *embedding_ptr,
                                    SegmentOffset segment_offset,
                                    const IVF_Centroids_Storage *ivf_centroids_storage) {
        IncreaseMemoryUsage(row_memory_cost_);
        AppendOneEmbedding(part_id, embedding_ptr, segment_offset, ivf_centroids_storage);
    }

    virtual void SearchIndex(const Vector<u32> &part_ids,
                             const IVF_Index_Storage *ivf_index_storage,
                             const KnnDistanceBase1 *knn_distance,
                             const void *query_ptr,
                             EmbeddingDataType query_element_type,
                             const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                             const std::function<void(f32, SegmentOffset)> &add_result_func) const = 0;
    SizeT MemoryUsed() const { return memory_used_; }
};

class IVF_Index_Storage {
    const IndexIVFOption ivf_option_ = {};
    const LogicalType column_logical_type_ = LogicalType::kInvalid;
    const EmbeddingDataType embedding_data_type_ = EmbeddingDataType::kElemInvalid;
    const u32 embedding_dimension_ = 0;
    u32 row_count_ = 0;
    u32 embedding_count_ = 0;
    IVF_Centroids_Storage ivf_centroids_storage_ = {};
    UniquePtr<IVF_Parts_Storage> ivf_parts_storage_ = {};

public:
    IVF_Index_Storage(const IndexIVFOption &ivf_option,
                      LogicalType column_logical_type,
                      EmbeddingDataType embedding_data_type,
                      u32 embedding_dimension);
    [[nodiscard]] const IndexIVFOption &ivf_option() const { return ivf_option_; }
    [[nodiscard]] LogicalType column_logical_type() const { return column_logical_type_; }
    [[nodiscard]] EmbeddingDataType embedding_data_type() const { return embedding_data_type_; }
    [[nodiscard]] u32 embedding_dimension() const { return embedding_dimension_; }
    [[nodiscard]] const IVF_Centroids_Storage &ivf_centroids_storage() const { return ivf_centroids_storage_; }
    [[nodiscard]] const IVF_Parts_Storage &ivf_parts_storage() const { return *ivf_parts_storage_; }

    void Train(u32 training_embedding_num, const f32 *training_data, u32 expect_centroid_num = 0);
    void AddEmbedding(SegmentOffset segment_offset, const void *embedding_ptr);
    void AddEmbeddingBatch(SegmentOffset start_segment_offset, const void *embedding_ptr, u32 embedding_num);
    void AddEmbeddingBatch(const SegmentOffset *segment_offset_ptr, const void *embedding_ptr, u32 embedding_num);
    void AddMultiVector(SegmentOffset segment_offset, const void *multi_vector_ptr, u32 embedding_num);

    void SearchIndex(const KnnDistanceBase1 *knn_distance,
                     const void *query_ptr,
                     EmbeddingDataType query_element_type,
                     u32 nprobe,
                     const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                     const std::function<void(f32, SegmentOffset)> &add_result_func) const;

    void GetMemData(IVF_Index_Storage &&mem_data);
    void Save(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);
    SizeT MemoryUsed() const;

private:
    template <EmbeddingDataType embedding_data_type>
    void AddEmbeddingT(SegmentOffset segment_offset, const EmbeddingDataTypeToCppTypeT<embedding_data_type> *embedding_ptr);
    template <EmbeddingDataType embedding_data_type>
    void
    AddEmbeddingBatchT(SegmentOffset start_segment_offset, const EmbeddingDataTypeToCppTypeT<embedding_data_type> *embedding_ptr, u32 embedding_num);
    template <EmbeddingDataType embedding_data_type>
    void AddEmbeddingBatchT(const SegmentOffset *segment_offset_ptr,
                            const EmbeddingDataTypeToCppTypeT<embedding_data_type> *embedding_ptr,
                            u32 embedding_num);
    template <EmbeddingDataType embedding_data_type>
    void AddMultiVectorT(SegmentOffset segment_offset, const EmbeddingDataTypeToCppTypeT<embedding_data_type> *multi_vector_ptr, u32 embedding_num);
};

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

} // namespace infinity
