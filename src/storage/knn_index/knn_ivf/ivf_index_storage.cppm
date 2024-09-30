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

export module ivf_index_storage;

import stl;
import index_ivf;
import internal_types;
import logical_type;
import data_type;
import knn_expr;

namespace infinity {

class LocalFileHandle;
class FileHandler;

template <EmbeddingDataType t>
struct EmbeddingDataTypeToCppType;

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemInt8> {
    using type = i8;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemUInt8> {
    using type = u8;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemDouble> {
    using type = f64;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemFloat> {
    using type = f32;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemFloat16> {
    using type = Float16T;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemBFloat16> {
    using type = BFloat16T;
};

export template <EmbeddingDataType t>
using EmbeddingDataTypeToCppTypeT = typename EmbeddingDataTypeToCppType<t>::type;

export template <typename T>
EmbeddingDataType CppTypeToEmbeddingDataTypeV = EmbeddingDataType::kElemInvalid;

template <>
EmbeddingDataType CppTypeToEmbeddingDataTypeV<u8> = EmbeddingDataType::kElemUInt8;

template <>
EmbeddingDataType CppTypeToEmbeddingDataTypeV<i8> = EmbeddingDataType::kElemInt8;

template <>
EmbeddingDataType CppTypeToEmbeddingDataTypeV<f64> = EmbeddingDataType::kElemDouble;

template <>
EmbeddingDataType CppTypeToEmbeddingDataTypeV<f32> = EmbeddingDataType::kElemFloat;

template <>
EmbeddingDataType CppTypeToEmbeddingDataTypeV<Float16T> = EmbeddingDataType::kElemFloat16;

template <>
EmbeddingDataType CppTypeToEmbeddingDataTypeV<BFloat16T> = EmbeddingDataType::kElemBFloat16;


// always use float for centroids
class IVF_Centroids_Storage {
    u32 embedding_dimension_ = 0;
    u32 centroids_num_ = 0;
    Vector<f32> centroids_data_ = {};

public:
    IVF_Centroids_Storage() = default;
    IVF_Centroids_Storage(u32 embedding_dimension, u32 centroids_num, Vector<f32> &&centroids_data);
    const f32 *data() const { return centroids_data_.data(); }
    u32 embedding_dimension() const { return embedding_dimension_; }
    u32 centroids_num() const { return centroids_num_; }
    void Save(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);
};

class IVF_Part_Storage {
    u32 part_id_ = std::numeric_limits<u32>::max();
    u32 embedding_dimension_ = 0;

protected:
    u32 embedding_num_ = 0;
    Vector<SegmentOffset> embedding_segment_offsets_ = {};

public:
    IVF_Part_Storage(const u32 part_id, const u32 embedding_dimension) : part_id_(part_id), embedding_dimension_(embedding_dimension) {}
    virtual ~IVF_Part_Storage() = default;
    static UniquePtr<IVF_Part_Storage>
    Make(u32 part_id, u32 embedding_dimension, EmbeddingDataType embedding_data_type, const IndexIVFStorageOption &ivf_storage_option);
    u32 part_id() const { return part_id_; }
    u32 embedding_dimension() const { return embedding_dimension_; }
    u32 embedding_num() const { return embedding_num_; }
    SegmentOffset embedding_segment_offset(const u32 embedding_index) const { return embedding_segment_offsets_[embedding_index]; }

    virtual void Save(LocalFileHandle &file_handle) const;
    virtual void Load(LocalFileHandle &file_handle);

    virtual void AppendOneEmbedding(const void *embedding_ptr, SegmentOffset segment_offset, const IVF_Centroids_Storage *ivf_centroids_storage) = 0;

    virtual void SearchIndex(KnnDistanceType knn_distance_type,
                             const void *query_ptr,
                             EmbeddingDataType query_element_type,
                             std::function<void(f32, SegmentOffset)> add_result_func) const = 0;

    // only for unit-test, return f32 / i8 / u8 embedding data
    virtual Pair<const void *, SharedPtr<void>> GetDataForTest(u32 embedding_id) const = 0;
};

export class IVF_Index_Storage {
    const IndexIVFOption ivf_option_ = {};
    const LogicalType column_logical_type_ = LogicalType::kInvalid;
    const EmbeddingDataType embedding_data_type_ = EmbeddingDataType::kElemInvalid;
    const u32 embedding_dimension_ = 0;
    u32 row_count_ = 0;
    u32 embedding_count_ = 0;
    IVF_Centroids_Storage ivf_centroids_storage_ = {};
    Vector<UniquePtr<IVF_Part_Storage>> ivf_part_storages_ = {};

public:
    IVF_Index_Storage(const IndexIVFOption &ivf_option,
                      LogicalType column_logical_type,
                      EmbeddingDataType embedding_data_type,
                      u32 embedding_dimension);
    [[nodiscard]] const IndexIVFOption &ivf_option() const { return ivf_option_; }
    [[nodiscard]] LogicalType column_logical_type() const { return column_logical_type_; }
    [[nodiscard]] EmbeddingDataType embedding_data_type() const { return embedding_data_type_; }
    [[nodiscard]] u32 embedding_dimension() const { return embedding_dimension_; }
    // [[nodiscard]] u32 row_count() const { return row_count_; }
    // [[nodiscard]] u32 embedding_count() const { return embedding_count_; }

    void Train(u32 training_embedding_num, const f32 *training_data, u32 expect_centroid_num = 0);
    void AddEmbedding(SegmentOffset segment_offset, const void *embedding_ptr);
    void AddEmbeddingBatch(SegmentOffset start_segment_offset, const void *embedding_ptr, u32 embedding_num);
    void AddEmbeddingBatch(const SegmentOffset *segment_offset_ptr, const void *embedding_ptr, u32 embedding_num);
    void AddMultiVector(SegmentOffset segment_offset, const void *multi_vector_ptr, u32 embedding_num);

    void SearchIndex(KnnDistanceType knn_distance_type,
                     const void *query_ptr,
                     EmbeddingDataType query_element_type,
                     u32 nprobe,
                     std::function<void(f32, SegmentOffset)> add_result_func) const;

    void GetMemData(IVF_Index_Storage &&mem_data);
    void Save(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);

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

} // namespace infinity
